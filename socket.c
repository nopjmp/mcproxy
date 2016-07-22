#include "socket.h"

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <netinet/tcp.h>

int create_and_bind(char *port)
{
    // TODO add support for single IP listen
    struct addrinfo hints = {
	.ai_family = AF_UNSPEC,
	.ai_socktype = SOCK_STREAM,
	.ai_flags = AI_PASSIVE
    };
    struct addrinfo *result, *rp;
    int s, sfd;

    s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0) {
	// log error here
	return -1;
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
	sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
	if (sfd == -1)
	    continue;
	int optval = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
	if (s == 0)
	    break;

	// failed to bind
	close(sfd);
    }
    
    if (rp == NULL) {
	fprintf(stderr, "failed to bind\n");
	return -1;
    }

    freeaddrinfo(result);
    
    return sfd;
}

int resolve(const char *host, struct in_addr* addr)
{
    int ret = 0;
    struct addrinfo *res;
    if ((ret = getaddrinfo(host, NULL, NULL, &res)) == 0) {
	memcpy(addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr,
	   sizeof(struct in_addr));
	freeaddrinfo(res);
    }
    
    return ret;
}

int s_connect(const char *host, int port)
{
    struct sockaddr_in addr = {
	.sin_family = AF_INET,
	.sin_port = htons(port)
    };
    if (resolve(host, &(addr.sin_addr)) != 0) {
	inet_pton(AF_INET, host, &(addr.sin_addr));
    }

    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int r;
    do {
	errno = 0;
	r = connect(sfd, (struct sockaddr *)&addr, sizeof(addr));
    } while (r == -1 && errno == EINTR);
    return sfd;
}

int set_nonblocking(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

