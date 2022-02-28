#include "../utils/alloc.h"
#include "../utils/err.h"

#define NN_POLLER_GRANULARITY (16)

int nn_poller_init(struct nn_poller *self)
{
    self->size = 0;
    self->index = 0;
    self->capacity = NN_POLLER_GRANULARITY;
    self->pollset
        = nn_alloc(sizeof(struct pollfd) * NN_POLLER_GRANULARITY, "pollset");
    alloc_assert(self->pollset);
    self->hndls = nn_alloc(
        sizeof(struct nn_hndls_item) * NN_POLLER_GRANULARITY, "hndls");
    alloc_assert(self->hndls);
    self->removed = -1;

    return 0;
}

void nn_poller_term(struct nn_poller *self)
{
    nn_free(self->pollset);
    nn_free(self->hndls);
    return;
}

void nn_poller_add(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    int rc;

    if (nn_slow(self->size >= self->capacity)) {
        self->capacity *= 2;
        self->pollset
            = nn_realloc(self->pollset, sizeof(struct follfd) * self->capacity);
        alloc_assert(self->pollset);
        self->hndls = nn_realloc(
            self->hndls, sizeof(struct nn_hndls_item) * self->capacity);
        alloc_assert(self->hndls);
    }
    return;
}

void nn_poller_rm(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    self->pollset[hndl->index].revents = 0;

    if (self->removed != -1) {
        self->hndls[self->removed].prev = hndl->index;
        self->hndls[hndl->index].hndl = NULL;
        self->hndls[hndl->index].prev = -1;
        self->hndls[hndl->index].next = self->removed;
        self->removed = hndl->index;
    }
    return;
}

void nn_poller_set_in(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    self->pollset[hndl->index.events |= POLLIN;
    return;
}

void nn_poller_reset_in(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    self->pollset[hndl->index].events &= ~POLLIN;
    self->pollset[hndl->index].revents &= ~POLLIN;
    return;
}

void nn_poller_set_out(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    self->pollset[hndl->index].events |= POLLOUT;
    return;
}

void nn_poller_reset_out(struct nn_poller *self, struct nn_poller_hndl *hndl)
{
    self->pollset[hndl->index].events &= ~POLLOUT;
    self->pollset[hndl->index].revents &= ~POLLOUT;
    return;
}

int nn_poller_wait(struct nn_poller *self, int timeout)
{
    int rc;
    int i;

    while (self->removed != -1) {
        i = self->removed;
        self->removed = self->hndls[i].next;

        --self->size;
        if (i != self->size) {
            self->pollset[i] = self->pollset[self->size];
            if (self->hndl[i].next != -1) {
                self->hndls[self->hndls[i].next].prev = -1;
            }
            self->hndls[i] = self->hndls[self->size];
            if (self->hndls[i].hndl) {
                self->hndls[i].hndl->index = i;
            }
        }

        if (nn_slow(!self->hndls[i].hndl)) {
            if (self->hndls[i].prev != -1) {
                self->hndls[self->hndls[i].prev].next = i;
            }
            if (self->hndls[i].next != -1) {
                self->hndls[self->hndls[i].next].prev = i;
            }
            if (self->removed == self->size) {
                self->removed = i;
            }
        }
    }

    self->index = 0;

#if defined(NN_IGNORE_EINTR)
again:
#endif
    rc = poll(self->pollset, self->size, timeout);
    if (nn_slow(rc < 0 && errno == EINTR)) {
#if defined(NN_IGNORE_EINTR)
        goto again;
#else
        return -EINTR;
#endif
    }
    errno_assert(rc >= 0);

    return 0;
}

int nn_poller_event(
    struct nn_poller *self, int *event, struct nn_poller_hndl **hndl)
{
    int rc;

    while (self->index < self->size) {
        if (self->pollset[self->index].revents != 0) {
            break;
        }
        ++self->index;
    }

    if (nn_slow(self->index >= self->size)) {
        return -EAGAIN;
    }

    *hndl = self->hndls[self->index].hndl;
    if (nn_fast(self->pollset[self->index].revents & POLLIN)) {
        *event = NN_POLLER_IN;
        self->pollset[self->index].revents &= ~POLLIN;
        return 0;
    } else if (nn_fast(self->pollset[self->index].revents & POLLOUT)) {
        *event = NN_POLLER_OUT;
        self->pollset[self->index].revents &= ~POLLOUT;
        return 0;
    } else {
        *event = NN_POLLER_ERR;
        ++self->index;
        return 0;
    }
    return 0;
}
