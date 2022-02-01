#ifndef NN_MUTEX_INCLUDED
#define NN_MUTEX_INCLUDED

#ifdef NN_HAVE_WINDOWS
#include "win.h"
#else
#include <pthread.h>
#endif

struct nn_mutex {
#ifdef NN_HAVE_WINDOWS
    CRITICAL_SECTION cs;
    DWORD owner;
    int debug;
#else
    pthread_mutex_t mutex;
#endif
};

typedef struct nn_mutex nn_mutex_t;

void nn_mutex_init(nn_mutex_t *self);

void nn_mutex_term(nn_mutex_t *self);

void nn_mutex_lock(nn_mutex_t *self);

void nn_mutex_unlock(nn_mutex_t *self);

#endif // !NN_MUTEX_INCLUDED
