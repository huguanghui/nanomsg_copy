#include "err.h"

#include <signal.h>

static void *nn_thread_main_routine(void *arg)
{
    struct nn_thread *self;

    self = (struct nn_thread *)arg;
    self->routine(self->arg);

    return NULL;
}

void nn_thread_init(
    struct nn_thread *self, nn_thread_routine *routine, void *arg)
{
    int rc;
    sigset_t new_sigmask;
    sigset_t old_sigmask;

    rc = sigfillset(&new_sigmask);
    errno_assert(rc == 0);
    rc = pthread_sigmask(SIG_BLOCK, &new_sigmask, &old_sigmask);
    errnum_assert(rc == 0, rc);

    self->routine = routine;
    self->arg = arg;
    rc = pthread_create(
        &self->handle, NULL, nn_thread_main_routine, (void *)self);
    errnum_assert(rc == 0, rc);

    rc = pthread_sigmask(SIG_SETMASK, &old_sigmask, NULL);
    errnum_assert(rc == 0, rc);

    return;
}

void nn_thread_term(struct nn_thread *self)
{
    int rc;

    rc = pthread_join(self->handle, NULL);
    errnum_assert(rc == 0, rc);

    return;
}
