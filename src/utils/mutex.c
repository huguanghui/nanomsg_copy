#include "mutex.h"
#include "err.h"

#include <pthread.h>
#include <stdlib.h>

#ifdef NN_HAVE_WINDOWS

void nn_mutex_init(nn_mutex_t *self)
{
    InitializeCriticalSection(&self->cs);
    self->owner = 0;
}

void nn_mutex_term(nn_mutex_t *self)
{
    nn_assert(self->owner == 0);
    DeleteCriticalSection(&self->cs);
}

void nn_mutex_lock(nn_mutex_t *self)
{
    EnterCriticalSection(&self->cs);
    nn_assert(self->owner == 0);
    self->owner = GetCurrentThreadId();
}

void nn_mutex_unlock(nn_mutex_t *self)
{
    nn_assert(self->owner == GetCurrentThreadId());
    self->owner = 0;
    LeaveCriticalSection(&self->cs);
}

#else

void nn_mutex_init(nn_mutex_t *self)
{
    int rc;
    pthread_mutexattr_t attr;
    rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    errnum_assert(rc == 0, rc);
    rc = pthread_mutex_init(&self->mutex, NULL);
    errnum_assert(rc == 0, rc);
    pthread_mutexattr_destroy(&attr);
}

void nn_mutex_term(nn_mutex_t *self)
{
    int rc;

    rc = pthread_mutex_destroy(&self->mutex);
    errnum_assert(rc == 0, rc);
}

void nn_mutex_lock(nn_mutex_t *self)
{
    int rc;

    rc = pthread_mutex_lock(&self->mutex);
    errnum_assert(rc == 0, rc);
}

void nn_mutex_unlock(nn_mutex_t *self)
{
    int rc;

    rc = pthread_mutex_unlock(&self->mutex);
    errnum_assert(rc == 0, rc);
}

#endif
