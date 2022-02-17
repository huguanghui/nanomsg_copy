#include "err.h"
#include "closefd.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int nn_efd_init(struct nn_nn_efd *self)
{
    int rc;
    int flags;

    self->efd = eventfd(0, EFD_CLOSEXEC);
    if (self->efd == -1 && (errno == EMFILE || errno == ENFILE)) {
        return -EMFILE;
    }
    errno_assert(self->efd != -1);

    flags = fcntl(self->efd, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;
    }
    rc = fcntl(self->efd, F_SETFL, flags | O_NONBLOCK);
    errno_assert(rc != -1);

    return 0;
}

void nn_efd_term(struct nn_efd *self)
{
    int fd = self->efd;
    self->efd = -1;
    nn_closefd(fd);

    return;
}

void nn_efd_stop(struct nn_efd *self)
{
    nn_efd_signal(self);
    return;
}

nn_fd nn_efd_getfd(struct nn_efd *self) { return self->efd; }

void nn_efd_signal(struct nn_efd *self)
{
    const uint64_t one = 1;
    ssize_t nbytes;
    int fd = self->efd;

    if (nn_slow(fd < 0)) {
        return;
    }

    nbytes = write(fd, &one, sizeof(one));
    errno_assert(nbytes == sizeof(one));
    return;
}

void nn_efd_unsignal(struct nn_efd *self)
{
    uint64_t count;
    int fd = self->efd;

    if (nn_slow(fd < 0)) {
        return;
    }

    ssize_t sz = read(fd, &count, sizeof(count));
    errno_assert(sz >= 0);
    nn_assert(sz == sizeof(count));

    return;
}
