#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#include "socket.h"
#include "stream.h"
#include "util.h"
#include <ev.h>

void accept_cb(struct ev_loop *loop, ev_io *w, int events)
{
    UNUSED(events);
    struct sockaddr_in addr = {};
    socklen_t len = sizeof(addr);
    int cfd = accept(w->fd, (struct sockaddr *)&addr, &len);
    set_nonblocking(cfd);
    printf("accepted %d\n", cfd);
    int mfd = s_connect("127.0.0.1", 25565);
    // TODO: ERROR CHECKING PLS
    set_nonblocking(mfd);
    printf("connected %d %d\n", cfd, mfd);

    stream_new(loop, cfd, mfd);
}

int main()
{
    // start server
    printf("start\n");
    struct ev_loop *loop = EV_DEFAULT;

    signal(SIGPIPE, SIG_IGN);

    int sfd = create_and_bind("25555");
    if (sfd == -1) {
	perror("create_and_bind");
	return -1;
    }

    printf("server %d\n", sfd);

    int s = set_nonblocking(sfd);
    if (s == -1) {
	perror("nonblocking");
	return -1;
    }

    s = listen(sfd, SOMAXCONN);
    if (s == -1) {
	perror("listen");
	return -1;
    }

    ev_io accept_io;
    ev_io_init(&accept_io, accept_cb, sfd, EV_READ);
    ev_io_start(loop, &accept_io);

    int events = 0;
    do {
	events = ev_run(loop, 0);
	printf("asdf %d", events);
    } while(events);
    
    return 0;
}
