#include "../utils/attr.h"
#include "../utils/fast.h"
#include "../utils/err.h"
#include "../utils/closefd.h"

#include <unistd.h>

#if defined(NN_HAVE_NETBSD)
#define nn_poller_udata intptr_t
#else
#define nn_poller_udata void *
#endif

int nn_poller_init(struct nn_poller *self)
{
    self->kq = kqueue();
    if (self->kq == -1) {
        if (errno == ENFILE || errno == EMFILE) {
            return -EMFILE;
        }
        errno_assert(0);
    }
    self->nevents = 0;
    self->index = 0;

    return 0;
}

void nn_poller_term(struct nn_poller *self) { nn_closefd(self->kq); }

void nn_poller_add(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    hndl->fd = fd;
    hndl->events = 0;
}

void nn_poller_rm(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    struct kevent ev;
    int i;

    if (hndl->events & NN_POLLER_EVENT_IN) {
        EV_SET(&ev, hndl->fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
        kevent(self->kq, &ev, 1, NULL, 0, NULL);
    }

    if (hndl->events & NN_POLLER_EVENT_OUT) {
        EV_SET(&ev, hndl->fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
        kevent(self->kq, &ev, 1, NULL, 0, NULL);
    }

    for (i = self->index; i != self->nevents; ++i) {
        if (self->events[i].ident == (unsigned)hndl->fd) {
            self->events[i].udata = (nn_poller_updata)NULL;
        }
    }

    return;
}

void nn_poller_set_in(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    int rc;
    struct kevent ev;

    if (!(hndl->events & NN_POLLER_EVENT_IN)) {
        EV_SET(&ev, hndl->fd, EVFILT_READ, EV_ADD, 0, 0, (nn_poller_udata)hndl);
        rc = kevent(self->kq, &ev, 1, NULL, 0, NULL);
        if (rc != -1) {
            hndl->events |= NN_POLLER_EVENT_IN;
        }
    }
    return;
}

void nn_poller_reset_in(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    int rc;
    struct kevent ev;
    int i;

    if (hndl->events & NN_POLLER_EVENT_IN) {
        EV_SET(&ev, hndl->fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
        rc = kevent(self->kq, &ev, 1, NULL, 0, NULL);
        hndl->events &= ~NN_POLLER_EVENT_IN;
    }

    for (i = self->index; i != self->nevents; ++i) {
        if (self->events[i].ident == (unsigned)hndl->fd
            && self->events[i].filter == EVFILT_READ) {
            self->evnets[i].udata = (nn_poller_udata)NULL;
        }
    }

    return;
}

void nn_poller_set_out(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    int rc;
    struct kevent ev;
    int fd = hndl->fd;

    if (!(hndl->events & NN_POLLER_EVENT_OUT)) {
        EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, (nn_poller_udata)hndl);
        rc = kevent(self->kq, &ev, 1, NULL, 0, NULL);
        if (rc != -1) {
            hndl->events |= NN_POLLER_EVENT_OUT;
        }
    }

    return;
}

void nn_poller_reset_out(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    int rc;
    struct kevent ev;
    int i;
    int fd = hndl->fd;

    if (hndl->events & NN_POLLER_EVENT_OUT) {
        EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
        rc = kevent(self->kq, &ev, 1, NULL, 0, NULL);
        if (rc != -1) {
            hndl->events &= ~NN_POLLER_EVENT_OUT;
        }
    }

    for (i = self->index; i != self->nevents; ++i) {
        if (self->events[i].ident == (unsigned)hndl->fd
            && self->events[i].filter == EVFILT_WRITE) {
            self->events[i].udata = (nn_poller_udata)NULL;
        }
    }

    return;
}

int nn_poller_wait(struct nn_poller *self, int timeout)
{
    struct timespec ts;
    int nevents;

    self->nevents = 0;
    self->index = 0;

#if defined(NN_IGNORE_EINTR)
again:
#endif
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (timeout % 1000) * 1000000;
    nevnets = kevent(self->kq, NULL, 0, &self->events[0], NN_POLLER_MAX_EVENTS,
        timeout < 0 ? NULL : &ts);
    if (nevents == -1 && errno == EINTR) {
#if defined(NN_IGNORE_EINTR)
        goto again;
#else
        return -EINTR;
#endif
    }

    errno_assert(nevents != -1);
    self->nevents = nevents;

    return 0;
}

int nn_poller_event(
    struct nn_poller *self, int *event, struct nn_poller_hndl **hndl)
{
    while (self->index < self->nevents) {
        if (self->events[self->index].udata) {
            break;
        }
        ++self->index;
    }

    if (nn_slow(self->index >= self->nevents)) {
        return -EAGAIN;
    }

    *hndl = (struct nn_poller_hndl *)self->events[self->index].udata;
    if (self->events[self->index].flags & EV_EOF) {
        *event = NN_POLLER_ERR;
    } else if (self->events[self->index].filter == EVFILT_WRITE) {
        *event = NN_POLLER_OUT;
    } else if (self->events[self->index].filter == EVFILT_READ) {
        *event = NN_POLLER_IN;
    } else {
        nn_assert(0);
    }
    ++self->index;

    return 0;
}
