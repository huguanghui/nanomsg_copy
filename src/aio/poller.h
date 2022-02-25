#ifndef NN_POLLER_INCLUDED
#define NN_POLLER_INCLUDED

#define NN_POLLER_IN 1
#define NN_POLLER_OUT 2
#define NN_POLLER_ERR 3

#if defined(NN_USE_EPOLL)
#include "poller_epoll.h"
#elif defined(NN_USE_KQUEUE)
#include "poller_kqueue.h"
#elif defined(NN_USE_POLL)
#include "poller_poll.h"
#else
#error
#endif

int nn_poller_init(struct nn_poller *self);
void nn_poller_term(struct nn_poller *self);
void nn_poller_add(struct nn_poller *self, struct nn_poller_hndl *hndl);
void nn_poller_rm(struct nn_poller *self, struct nn_poller_hndl *hndl);
void nn_poller_set_in(struct nn_poller *self, struct nn_poller_hndl *hndl);
void nn_poller_reset_in(struct nn_poller *self, struct nn_poller_hndl *hndl);
void nn_poller_set_out(struct nn_poller *self, struct nn_poller_hndl *hndl);
void nn_poller_reset_out(struct nn_poller *self, struct nn_poller_hndl *hndl);
int nn_poller_wait(struct nn_poller *self, int timeout);
int nn_poller_event(struct nn_poller *self, int *event, struct nn_poller_hndl **hndl);

#endif // !NN_POLLER_INCLUDED
