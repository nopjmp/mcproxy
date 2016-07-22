#ifndef __SOCKET_H
#define __SOCKET_H

#include <arpa/inet.h>

int create_and_bind(char *port);

int resolve(const char *host, struct in_addr* addr);

int s_connect(const char *host, int port);

int set_nonblocking(int fd);

#endif
