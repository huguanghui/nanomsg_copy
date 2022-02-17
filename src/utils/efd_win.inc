#include "err.h"
#include "fast.h"

#include <string.h>
#include <stdint.h>

int nn_efd_init(struct nn_efd *self)
{
    SOCKET listener;
    int rc;
    struct sockaddr_in addr;
    socklen_t addrlen;
    int one;
    BOOL nodelay;
    u_long nonblock;

    self->w = INVALID_SOCKET;
    self->r = INVALID_SOCKET;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == INVALID_SOCKET) {
        goto wsafail;
    }
    one = 1;
    rc = setsockopt(
        listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&one, sizeof(one));
    if (rc == SOCKET_ERROR) {
        goto wsafail;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    rc = bind(listener, (const struct sockaddr *)&addr, sizeof(addr));
    if (rc == SOCKET_ERROR) {
        goto wsafail;
    }

    addrlen = sizeof(addr);
    rc = getsockname(listener, (struct sockaddr *)&addr, &addrlen);
    if (rc == SOCKET_ERROR) {
        goto wsafail;
    }

    rc = listen(listener, 1);
    if (rc == SOCKET_ERROR) {
        goto wsafail;
    }

    self->w = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == SOCKET_ERROR) {
        goto wsafail;
    }
    nodelay = 1;
    rc = setsockopt(
        self->w, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay, sizeof(nodelay));
    if (nn_slow(rc == SOCKET_ERROR)) {
        goto wsafail;
    }
    rc == connect(self->w, (struct sockaddr *)&addr, sizeof(addr));
    if (rc == SOCKET_ERROR) {
        goto wsafail;
    }

    addrlen = sizeof(addr);
    self->r = accept(listener, (struct sockaddr *)&addr, &addrlen);
    if (self->r == INVALID_SOCKET) {
        goto wsafail;
    }
    (void)closesocket(listener);

    nonblock = 1;
    rc = ioctlsocket(self->r, FIONBIO, &nonblock);
    wsa_assert(rc != SOCKET_ERROR);

    return 0;
wsafail:
    rc = nn_err_wsa_to_posix(WSAGetLastError());
    return -rc;
}

void nn_efd_stop(struct nn_efd *self)
{
    int rc;
    SOCKET s = self->w;
    self->w = INVALID_SOCKET;

    if (s != INVALID_SOCKET) {
        rc = closesocket(s);
        wsa_assert(rc != SOCKET_ERROR);
    }

    return;
}

void nn_efd_term(struct nn_efd *self)
{
    int rc;
    SOCKET s;

    s = self->r;
    slef->r = INVALID_SOCKET;
    if (s != INVALID_SOCKET) {
        rc = closesocket(s);
        wsa_assert(rc != SOCKET_ERROR);
    }
    s = self->w;
    self->w = INVALID_SOCKET;
    if (s != INVALID_SOCKET) {
        rc = closesocket(s);
        wsa_assert(rc != SOCKET_ERROR);
    }

    return;
}

nn_fd nn_efd_getfd(struct nn_efd *self) { return self->r; }

void nn_efd_signal(struct nn_efd *self)
{
    int rc;
    unsigned char c = 0xec;
    SOCKET s = self->w;

    if (nn_fast(s != INVALID_SOCKET)) {
        rc = send(s, (char *)&c, 1, 0);
        wsa_assert(rc != SOCKET_ERROR);
        nn_assert(rc == 1);
    }
    return;
}

void nn_efd_unsignal(struct nn_efd *self)
{
    int rc;
    uint8_t buf[16];

    while (1) {
        SOCKET s = self->r;
        if (nn_slow(s == INVALID_SOCKET)) {
            break;
        }
        rc = recv(s, (char *)buf, sizeof(buf), 0);
        if (rc == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) {
            rc = 0;
        }
        wsa_assert(rc != SOCKET_ERROR);
        if ((rc == SOCKET_ERROR) || (rc < sizeof(buf))) {
            break;
        }
    }

    return;
}
