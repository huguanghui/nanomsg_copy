#include "sem.h"
#include "err.h"
#include "fast.h"

#if defined(NN_HAVE_WINDOWS)

void nn_sem_init(struct nn_sem *self)
{
    self->h = CreateEvent(NULL, FALSE, FALSE, NULL);
    win_assert(self->h);
    return;
}

void nn_sem_term(struct nn_sem *self)
{
    BOOL brc;

    brc = CloseHandle(self->h);
    win_assert(brc);
    return;
}

void nn_sem_post(struct nn_sem *self)
{
    BOOL brc;

    brc = SetEvent(self->h);
    win_assert(brc);
    return;
}

int nn_sem_wait(struct nn_sem *self)
{
    DWORD rc;

    rc = WaitForSingleObject(self->h, INFINITE);
    win_assert(rc != WAIT_FAILED);
    nn_assert(rc == WAIT_OBJECT_0);
    return 0;
}

#elif defined NN_HAVE_SEMAPHORE

void nn_sem_init(struct nn_sem *self)
{
    int rc;

    rc = sem_init(&self->sem, 0, 0);
    errno_assert(rc == 0);
    return;
}

void nn_sem_term(struct nn_sem *self)
{
    int rc;

    rc = sem_destroy(&self->sem);
    errno_assert(rc == 0);
    return;
}

void nn_sem_post(struct nn_sem *self)
{
    int rc;

    rc = sem_post(&self->sem);
    errno_assert(rc == 0);
    return;
}

int nn_sem_wait(struct nn_sem *self)
{
    int rc;

    rc = sem_wait(&self->sem);
    if (nn_slow(rc < 0 && errno == EINTR)) {
        return -EINTR;
    }
    errno_assert(rc == 0);
    return 0;
}

#else

void nn_sem_init(struct nn_sem *self)
{
    int rc;

    rc = pthread_mutex_init(&self->mutex, NULL);
    errnum_assert(rc == 0, rc);
    rc = pthread_cond_init(&self->cond, NULL);
    errnum_assert(rc == 0, rc);
    self->signaled = 0;
    return;
}

void nn_sem_term(struct nn_sem *self)
{
    int rc;

    rc = pthread_cond_destroy(&self->cond);
    errnum_assert(rc == 0, rc);
    rc = pthread_mutex_destroy(&self->mutex);
    errnum_assert(rc == 0, rc);
    return;
}

void nn_sem_post(struct nn_sem *self)
{
    int rc;

    rc = pthread_mutex_lock(&self->mutex);
    errnum_assert(rc == 0, rc);
    nn_assert(self->signaled == 0);
    self->signaled = 1;
    rc = pthread_cond_signal(&self->cond);
    errnum_assert(rc == 0, rc);
    rc = pthread_mutex_unlock(&self->mutex);
    errnum_assert(rc == 0, rc);
    return;
}

int nn_sem_wait(struct nn_sem *self)
{
    int rc;

    rc = pthread_mutex_lock(&self->mutex);
    errnum_assert(rc == 0, rc);
    if (nn_fast(self->signaled)) {
        rc = pthread_mutex_unlock(&self->mutex);
        errnum_assert(rc == 0, rc);
        return 0;
    }
    rc = pthread_cond_wait(&self->cond, &self->mutex);
    errnum_assert(rc == 0, rc);
    if (nn_slow(!self->signaled)) {
        rc = pthread_mutex_unlock(&self->mutex);
        errnum_assert(rc == 0, rc);
        return -EINTR;
    }
    self->signaled = 0;
    rc = pthread_mutex_unlock(&self->mutex);
    errnum_assert(rc == 0, rc);
    return 0;
}

#endif /* defined(NN_HAVE_WINDOWS) */
