#ifndef __STREAM_H
#define __STREAM_H

#include "queue.h"
#include "bytebuf.h"

#include <ev.h>

enum estate {
    NONE,
    STATUS,
    LOGIN,
    PLAY
};

struct stream_t {
    // loop is not owned by stream_t
    struct ev_loop *loop;
    ev_io c_read_io;
    ev_io c_write_io;
    ev_io s_read_io;
    ev_io s_write_io;
    
    queue *c_write_queue;
    queue *s_write_queue;

    enum estate state;
};
typedef struct stream_t stream;
 
stream *stream_new(struct ev_loop *loop, int cfd, int sfd);
void stream_free(stream *);

void stream_queue_client_add(stream *, bytebuf *);
void stream_queue_server_add(stream *, bytebuf *);

#endif
