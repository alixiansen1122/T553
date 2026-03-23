/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: socket adapt src file.
 * Author: CompanyName
 * Create:
 */
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <byteswap.h>

int socket(int domain, int type, int protocol)
{
    return 0;
}

int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
    return 0;
}

int getsockopt(int fd, int level, int optname, void *restrict optval, socklen_t *restrict optlen)
{
    return 0;
}

int getsockname(int fd, struct sockaddr *restrict addr, socklen_t *restrict len)
{
    return 0;
}

int bind(int fd, const struct sockaddr *addr, socklen_t len)
{
    return 0;
}

int listen(int fd, int backlog)
{
    return 0;
}

int accept(int fd, struct sockaddr *restrict addr, socklen_t *restrict len)
{
    return 0;
}

int connect(int fd, const struct sockaddr *addr, socklen_t len)
{
    return 0;
}

int select(int n, fd_set *restrict rfds, fd_set *restrict wfds, fd_set *restrict efds,
    struct timeval *restrict tv)
{
    return 0;
}

ssize_t send(int fd, const void *buf, size_t len, int flags)
{
    return 0;
}

ssize_t recv(int fd, void *buf, size_t len, int flags)
{
    return 0;
}

int shutdown(int fd, int how)
{
    return 0;
}

int inet_pton(int af, const char *restrict s, void *restrict a0)
{
    return 0;
}

const char *inet_ntop(int af, const void *restrict a0, char *restrict s, socklen_t l)
{
    return 0;
}

uint32_t ntohl(uint32_t n)
{
    union { int i; char c; } u = { 1 };
    return u.c ? bswap_32(n) : n;
}

uint32_t htonl(uint32_t n)
{
    union { int i; char c; } u = { 1 };
    return u.c ? bswap_32(n) : n;
}
