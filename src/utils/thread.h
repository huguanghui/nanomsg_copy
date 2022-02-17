#ifndef NN_THREAD_INCLUDED
#define NN_THREAD_INCLUDED

typedef void(nn_thread_routine)(void *);

#if defined(NN_HAVE_WINDOWS)
#include "thread_win.h"
#else
#include "thread_posix.h"
#endif

void nn_thread_init(
    struct nn_thread *self, nn_thread_routine *routine, void *arg);

void nn_thread_term(struct nn_thread *self);

#endif // !NN_THREAD_INCLUDED
