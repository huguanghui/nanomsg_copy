#include "efd.h"
#include "clock.h"

#if defined(NN_USE_EVENTFD)
#include "efd_eventfd.inc"
#elif defined(NN_USE_PIPE)
#include "efd_pipe.inc"
#elif defined(NN_USE_SOCKETPAIR)
#include "efd_socketpair.inc"
#elif defined(NN_USE_WINSOCK)
#error
#endif

#if defined(NN_HAVE_POLL)

#include <poll.h>

int nn_efd_wait(struct nn_efd *self, int timeout)
{
    int rc;
    struct pollfd pfd;

    pfd.fd = nn_efd_getfd(self);
    pfd.events = POLLIN;
    if (pfd.fd < 0) {
        return -EBADF;
    }
    rc = poll(&pfd, 1, timeout);
    switch (rc) {
    case 0:
        return -ETIMEDOUT;
    case -1:
        return (-errno);
    }

    return 0;
}

#elif defined(NN_HAVE_WINDOWS)

int nn_efd_wait(struct nn_efd *self, int timeout)
{
    int rc;
    WSAPOLLED pfd;

    pfd.fd = self->r;
    if (nn_slow(pfd.fd == INVALID_SOCKET)) {
        return -EBADF;
    }
    pfd.events = POLLIN;
    rc = WSAPoll(&pfd, 1, timeout);

    switch (rc) {
    case 0:
        return -ETIMEDOUT;
    case SOCKET_ERROR:
        rc = nn_err_wsa_to_posix(WSAGetLastError());
        errno = rc;

        if (rc == EINTR || rc == ENOTSOCK) {
            return self->r == INVALID_SOCKET ? -EBADF : -EINTR;
        }
        return (-rc);
    }

    return (0);
}

#else
#error
#endif
