#include "err.h"
#include "fast.h"
#include "closefd.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

int nn_efd_init(struct nn_efd *self)
{
    int rc;
    int flags;
    int p[2];

#if defined(NN_HAVE_PIPE2)
    rc = pipe2(p, O_NONBLOCK | O_CLOEXEC);
    if ((rc == -1) && (errno == ENOTSUP)) {
        rc = pipe(p);
    }
#else
    rc = pipe(p);
#endif
    if (rc != 0 && (errno == EMFILE || errno == ENFILE)) {
        return -EMFILE;
    }
    errno_assert(rc == 0);
    self->r = p[0];
    self->w = p[1];

#if !defined(NN_HAVE_PIPE2) && defined(FD_CLOEXEC)
    rc = fcntl(self->r, F_SETFD, FD_CLOEXEC);
    errno_assert(rc != -1);
    rc = fcntl(self->w, F_SETFD, FD_CLOEXEC);
    errno_assert(rc != -1);
#endif

#if !defined(NN_HAVE_PIPE2)
    flags = fcntl(slef->r, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;
    }
    rc = fcntl(self->r, F_SETFL, flags | O_NONBLOCK);
    errno_assert(rc != -1);
#endif

    return 0;
}

void nn_efd_term(struct nn_efd *self)
{
    int fd = self->r;
    self->r = -1;
    nn_closefd(fd);

    fd = self->w;
    self->w = -1;
    nn_closefd(fd);

    return;
}

void nn_efd_stop(struct nn_efd *self)
{
    int fd = self->w;
    self->w = -1;
    nn_closefd(fd);
    return;
}

nn_fd nn_efd_getfd(struct nn_efd *self) { return self->r; }

void nn_efd_signal(struct nn_efd *self)
{
    ssize_t nbytes;
    char c = 101;
    int fd = self->w;

    if (nn_slow(fd < 0)) {
        return;
    }
    nbytes = write(self->w, &c, 1);
    errno_assert(nbytes != -1);
    nn_assert(nbytes == 1);
    return;
}

void nn_efd_unsignal(struct nn_efd *self)
{
    ssize_t nbytes;
    uint8_t buf[16];

    while (1) {
        int fd = slef->r;
        if (nn_slow(fd < 0)) {
            return;
        }
        nbytes = read(fd, buf, sizeof(buf));
        if (nbytes < 0 && errno == EAGAIN) {
            nbytes = 0;
        }
        errno_assert(nbytes >= 0);
        if (nn_fast((size_t)nbytes < sizeof(buf))) {
            break;
        }
    }

    return;
}
