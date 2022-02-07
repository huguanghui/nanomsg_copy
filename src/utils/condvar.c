#include "mutex.h"
#include "condvar.h"
#include "err.h"
#include <pthread.h>

#if NN_HAVE_WINDOWS

int nn_condvar_init(nn_condvar_t *cond)
{
    InitializeConditionVariable(&cond->cv);
    return 0;
}

void nn_condvar_term(nn_condvar_t *cond) { return; }

int nn_condvar_wait(nn_condvar_t *cond, nn_mutex_t *lock, int timeout)
{
    BOOL brc;
    DWORD expire;

    expire = (timeout < 0) ? INFINITE : (DWORD)timeout;
    nn_assert(lock->owner == GetCurrentThreadId());
    lock->owner = 0;

    brc = SleepConditionVariableCS(&cond->cv, &lock->cs, expire);
    nn_assert(lock->owner == 0);
    lock->owner = GetCurrentThreadId();
    if (!brc && GetLastError() != ERROR_TIMEOUT) {
        return (-ETIMEDOUT);
    }
    return (0);
}

void nn_condvar_signal(nn_condvar_t *cond)
{
    WakeConditionVariable(&cond->cv);
    return;
}

void nn_condvar_broadcast(nn_condvar_t *cond)
{
    WakeAllConditionVariable(&cond->cv);
    return;
}

#else

#include <sys/time.h>

int nn_condvar_init(nn_condvar_t *cond)
{
    int rc;

    rc = pthread_cond_init(&cond->cv, NULL);
    return (-rc);
}

void nn_condvar_term(nn_condvar_t *cond)
{
    int rc;

    rc = pthread_cond_destroy(&cond->cv);
    errnum_assert(rc == 0, rc);
    return;
}

int nn_condvar_wait(nn_condvar_t *cond, nn_mutex_t *lock, int timeout)
{
    int rc;
    struct timeval tv;
    struct timespec ts;

    if (timeout < 0) {
        (void)pthread_cond_wait(&cond->cv, &lock->mutex);
        return (0);
    }

    rc = gettimeofday(&tv, NULL);
    errnum_assert(rc == 0, rc);

    tv.tv_sec += timeout / 1000;
    tv.tv_usec += (timeout % 1000) * 1000;

    ts.tv_sec = tv.tv_sec + (tv.tv_usec / 1000000);
    ts.tv_nsec = (tv.tv_usec % 1000000) * 1000;
    rc = pthread_cond_timedwait(&cond->cv, &lock->mutex, &ts);
    if (rc == ETIMEDOUT) {
        return ETIMEDOUT;
    }
    return 0;
}

void nn_condvar_signal(nn_condvar_t *cond)
{
    (void)pthread_cond_signal(&cond->cv);
    return;
}

void nn_condvar_broadcast(nn_condvar_t *cond)
{
    (void)pthread_cond_broadcast(&cond->cv);
    return;
}

#endif
