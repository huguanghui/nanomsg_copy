#ifndef NN_SEM_INCLUDED
#define NN_SEM_INCLUDED

struct nn_sem;

void nn_sem_init(struct nn_sem *self);

void nn_sem_term(struct nn_sem *self);

void nn_sem_post(struct nn_sem *self);

int nn_sem_wait(struct nn_sem *self);

#if defined(NN_HAVE_WINDOWS)
#include <semaphore.h>

struct nn_sem {
    sem_t sem;
};
#else
#include <pthread.h>

struct nn_sem {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int signaled;
};
#endif

#endif // !NN_SEM_INCLUDED
