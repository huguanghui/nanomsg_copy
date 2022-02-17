#ifndef NN_CLOSEFD_INCLUDED
#define NN_CLOSEFD_INCLUDED

#include "fd.h"

#if defined(NN_USE_EVENTFD)
#include "efd_eventfd.h"
#elif defined(NN_USE_PIPE)
#include "efd_pipe.h"
#elif defined(NN_USE_SOCKETPAIR)
#include "efd_socketpair.h"
#elif defined(NN_USE_WINSOCK)
#include "efd_win.h"
#else
#error
#endif

int nn_efd_init(struct nn_efd *self);

void nn_efd_term(struct nn_efd *self);

nn_fd nn_efd_getfd(struct nn_efd *self);

void nn_efd_stop(struct nn_efd *self);

void nn_efd_signal(struct nn_efd *self);

void nn_efd_unsignal(struct nn_efd *self);

int nn_efd_wait(struct nn_efd *self, int timeout);

#endif // !NN_CLOSEFD_INCLUDED
