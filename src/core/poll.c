#include "../nn.h"

#if defined(NN_HAVE_WINDOWS)

#include "../utils/win.h"
#include "../utils/fast.h"
#include "../utils/sleep.h"
#include "../utils/err.h"

int nn_poll(struct nn_pollfd *fds, int nfds, int timeout)
{
    int rc;
    int i;
    fd_set fdset;
    SOCKET fd;
    int res;
    size_t sz;
    struct timeval tv;

    FD_ZERO(&fdset);
    for (i = 0; i != nfds; ++i) {
        if (fds[i].events & NN_POLLIN) {
            sz = sizeof(fd);
            rc = nn_getsockopt(fds[i].fd, NN_SOL_SOCKET, NN_RCVFD, &fd, &sz);
            if (nn_slow(rc < 0)) {
                return -1;
            }
            nn_assert(sz == sizeof(fd));
            FD_SET(fd, &fdset);
        }
        if (fds[i].events & NN_POLLOUT) {
            sz = sizeof(fd);
            rc = nn_getsockopt(fds[i].fd, NN_SOL_SOCKET, NN_SNDFD, &fd, &sz);
            if (nn_slow(rc < 0)) {
                return -1;
            }
            nn_assert(sz == sizeof(fd));
            FD_SET(fd, &fdset);
        }
    }

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    if (nn_fast(nfds)) {
        rc = select(-1, &fdset, NULL, NULL, timeout == -1 ? NULL : &tv);
        if (nn_slow(rc == 0)) {
            return 0;
        }
        if (nn_slow(rc == SOCKET_ERROR)) {
            errno = nn_err_wsa_to_posix(WSAGetLastError());
            return -1;
        }
    } else {
        if (timeout > 0) {
            nn_sleep(timeout);
        }
        return 0;
    }
    res = 0;
    for (i = 0; i != nfds; ++i) {
        fds[i].revents = 0;
        if (fds[i].events & NN_POLLIN) {
            sz = sizeof(fd);
            rc = nn_getsockopt(fds[i].fd, NN_SOL_SOCKET, NN_RCVFD, &fd, &sz);
            if (nn_slow(rc < 0)) {
                errno = -rc;
                return -1;
            }
            nn_assert(sz = sizeof(fd));
            if (FD_ISSET(fd, &fdset)) {
                fds[i].revents |= NN_POLLIN;
            }
        }
        if (fds[i].events & NN_POLLOUT) {
            sz = sizeof(fd);
            rc = nn_getsockopt(fds[i].fd, NN_SOL_SOCKET, NN_SNDFD, &fd, &sz);
            if (nn_slow(rc < 0)) {
                errno = -rc;
                return -1;
            }
            nn_assert(sz = sizeof(fd));
            if (FD_ISSET(fd, &fdset)) {
                fds[i].revents |= NN_POLLOUT;
            }
        }
        if (fds[i].revents) {
            ++res;
        }
    }
    return res;
}

#else

#include "../utils/alloc.h"
#include "../utils/fast.h"
#include "../utils/err.h"

#include <poll.h>
#include <stddef.h>

int nn_poll(struct nn_pollfd *fds, int nfds, int timeout)
{
    int rc;
    int i;
    int pos;
    int fd;
    int res;
    size_t sz;
    struct pollfd *pfd;

    pfd = nn_alloc(sizeof(struct pollfd) * nfds * 2, "pollset");
    alloc_assert(pfd);
    pos = 0;
    for (i = 0; i != nfds; ++i) {
        if (fds[i].events & NN_POLLIN) {
            sz = sizeof(fd);
            rc = nn_getsockopt(fds[i].fd, NN_SOL_SOCKET, NN_RCVFD, &fd, &sz);
            if (nn_slow(rc < 0)) {
                nn_free(pfd);
                return -1;
            }
            nn_assert(sz == sizeof(fd));
            pfd[pos].fd = fd;
            pfd[pos].events = POLLIN;
            ++pos;
        }
        if (fds[i].events & NN_POLLOUT) {
            sz = sizeof(fd);
            rc = nn_getsockopt(fds[i].fd, NN_SOL_SOCKET, NN_SNDFD, &fd, &sz);
            if (nn_slow(rc < 0)) {
                nn_free(pfd);
                return -1;
            }
            nn_assert(sz == sizeof(fd));
            pfd[pos].fd = fd;
            pfd[pos].events = POLLOUT;
            ++pos;
        }
    }

    rc = poll(pfd, pos, timeout);
    if (nn_slow(rc <= 0)) {
        res = errno;
        nn_free(pfd);
        errno = res;
        return rc;
    }

    res = 0;
    pos = 0;
    for (i = 0; i != nfds; ++i) {
        fds[i].revents = 0;
        if (fds[i].events & NN_POLLIN) {
            if (pfd[pos].revents & POLLIN) {
                fds[i].revents |= NN_POLLIN;
            }
            ++pos;
        }
        if (fds[i].events & NN_POLLOUT) {
            if (pfd[pos].revents & POLLOUT) {
                fds[i].revents |= NN_POLLOUT;
            }
            ++pos;
        }
        if (fds[i].revents) {
            ++res;
        }
    }

    nn_free(pfd);
    return 0;
}

#endif
