#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>

#include "stream.h"
#include "socket.h"
#include "bytebuf.h"
#include "util.h"

void stream_c_read_callback(struct ev_loop *loop, ev_io *w, int revents);
void stream_c_write_callback(struct ev_loop *loop, ev_io *w, int wevents);
void stream_s_read_callback(struct ev_loop *loop, ev_io *w, int revents);
void stream_s_write_callback(struct ev_loop *loop, ev_io *w, int wevents);

// stream helper functions
int maybe_recv(int fd, void* buf, size_t len)
{
    int r;
    do {
	r = recv(fd, buf, len, 0);
    } while(r == -1 && errno == EINTR);
    return r;
}

int maybe_send(int fd, void* buf, size_t len, int flags)
{
    int r;
    do {
	r = send(fd, buf, len, flags);
    } while(r == -1 && errno == EINTR);
    return r;
}

int recv_length(int fd, int32_t *value)
{
    int i = 0;
    uint8_t b = 0;
    *value = 0;
    do {
	int n = maybe_recv(fd, &b, sizeof(uint8_t));
	if (n <= 0) return n;
	
	*value |= (b & 0x7F) << 7 * i;
	i++;
    } while ((b & 0x80) != 0 && i < 5);
    return 1;
}

int write_length(int fd, int32_t value)
{
    int i = 0;
    uint8_t b;
    do {
	b = (uint8_t)(value & 0x7F);
	value >>= 7;
	if (value != 0) {
	    b |= 0x80;
	}
	int n = maybe_send(fd, &b, sizeof(uint8_t), MSG_MORE);
	if (n <= 0) return n;
	i++;
    } while( value != 0 && i < 5);
    return 1;
}

int read_packet(int fd, bytebuf **buf)
{
    int32_t length;
    int r = recv_length(fd, &length);
    if (r <= 0) {
	return r;
    }
    *buf = bytebuf_new(length);
    return maybe_recv(fd, (*buf)->buf, length) < 0 ? -1 : 1; 
}

int write_packet(int fd, bytebuf *buf)
{
    int r = write_length(fd, buf->len);
    if (r <= 0) {
	return r;
    }
    r = maybe_send(fd, buf->buf, buf->len, 0) < 0 ? -1 : 1;
    return r;
}

stream *stream_new(struct ev_loop *loop, int cfd, int sfd)
{
    stream *_s = malloc(sizeof(stream));
    _s->loop = loop;
    ev_io_init(&(_s->c_read_io), stream_c_read_callback, cfd, EV_READ);
    ev_io_init(&(_s->c_write_io), stream_c_write_callback, cfd, EV_WRITE);
    
    ev_io_init(&(_s->s_read_io), stream_s_read_callback, sfd, EV_READ);
    ev_io_init(&(_s->s_write_io), stream_s_write_callback, sfd, EV_WRITE);

    _s->c_write_queue = queue_new();
    _s->s_write_queue = queue_new();

    _s->state = NONE;

    ev_io_start(loop, &(_s->c_read_io));
    ev_io_start(loop, &(_s->s_read_io));
    return _s;
}

void stream_free(stream *_s)
{
    // stop ev
    ev_io_stop(_s->loop, &(_s->c_read_io));
    ev_io_stop(_s->loop, &(_s->c_write_io));
    ev_io_stop(_s->loop, &(_s->s_read_io));
    ev_io_stop(_s->loop, &(_s->s_write_io));

     // close sockets
    close(_s->c_read_io.fd);
    close(_s->s_read_io.fd);
    
    queue_free(_s->c_write_queue);
    queue_free(_s->s_write_queue);
}

// helper macros
#define STREAM_PTR(ev_m, w) (stream *)(((char *)w) - offsetof(stream, ev_m))

// TODO: logging
#define CHECK_EVENTS(events, msg)		\
    if(events & EV_ERROR) {			\
	fprintf(stderr, msg);			\
	return;					\
    }
	

void stream_c_read_callback(struct ev_loop *loop, ev_io *w, int revents)
{
    CHECK_EVENTS(revents, "c read error")
    stream * s = STREAM_PTR(c_read_io, w);
    bytebuf *b;
    int r = read_packet(w->fd, &b);
    if (r <= 0) {
	stream_free(s);
	return;
    }
    stream_queue_server_add(s, b);
    /* switch(s->state) { */
    /* case NONE: */
    /* 	break; */
    /* case STATUS: */
    /* 	break; */
    /* case LOGIN: */
    /* 	break; */
    /* } */
}

void stream_c_write_callback(struct ev_loop *loop, ev_io *w, int wevents)
{
    CHECK_EVENTS(wevents, "c write error")
    stream * s = STREAM_PTR(c_write_io, w);
    queue_elem *elem = queue_pop(s->c_write_queue);
    bytebuf *b = (bytebuf*)elem->data;
    int r = write_packet(w->fd, b);
    bytebuf_free(b);

    if (r < 0) {
	stream_free(s);
	return;
    }
    
    if (queue_empty(s->c_write_queue))
	ev_io_stop(loop, w);
}

void stream_s_read_callback(struct ev_loop *loop, ev_io *w, int revents)
{
    CHECK_EVENTS(revents, "s read error")
    stream * s = STREAM_PTR(s_read_io, w);
    bytebuf *b;
    int r = read_packet(w->fd, &b);
    if (r <= 0) {
	stream_free(s);
	return;
    }
    stream_queue_client_add(s, b);
}

void stream_s_write_callback(struct ev_loop *loop, ev_io *w, int wevents)
{
    CHECK_EVENTS(wevents, "s write error")
    stream * s = STREAM_PTR(s_write_io, w);
    queue_elem *elem = queue_pop(s->s_write_queue);
    bytebuf *b = (bytebuf*)elem->data;
    int r = write_packet(w->fd, b);
    bytebuf_free(b);

    if (r <= 0) {
	stream_free(s);
	return;
    }

    if (queue_empty(s->s_write_queue))
	ev_io_stop(loop, w);
}

void stream_queue_client_add(stream *_s, bytebuf *b)
{
    queue_elem *elem = queue_elem_new(b);
    queue_push(_s->c_write_queue, elem);
    if (!ev_is_active(&(_s->c_write_io)))
	ev_io_start(_s->loop, &(_s->c_write_io));
}

void stream_queue_server_add(stream *_s, bytebuf *b)
{
    queue_elem *elem = queue_elem_new(b);
    queue_push(_s->s_write_queue, elem);
    if (!ev_is_active(&(_s->s_write_io)))
	ev_io_start(_s->loop, &(_s->s_write_io));
}
