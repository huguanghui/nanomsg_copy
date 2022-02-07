#ifndef NN_CONDVAR_INCLUDED
#define NN_CONDVAR_INCLUDED

#include "mutex.h"

#ifdef NN_HAVE_WINDOWS
#include "win.h"
struct nn_condvar {
    CONDITION_VARIABLE cv;
};

#else

#include <pthread.h>

struct nn_condvar {
    pthread_cond_t cv;
};

#endif //

typedef struct nn_condvar nn_condvar_t;

int nn_condvar_init(nn_condvar_t *cond);

void nn_condvar_term(nn_condvar_t *cond);

int nn_condvar_wait(nn_condvar_t *cond, nn_mutex_t *lock, int timeout);

void nn_condvar_signal(nn_condvar_t *cond);

void nn_condvar_broadcast(nn_condvar_t *cond);

#endif // !NN_CONDVAR_INCLUDED
