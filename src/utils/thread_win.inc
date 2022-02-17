#include "err.h"

static unsigned int __stdcall nn_thread_main_routine(void *arg)
{
    struct nn_thread *self;

    self = (struct nn_thread *) arg;
    self->routine(self->arg);
    return 0;
}

void nn_thread_init(struct nn_thread *self, nn_thread_routine *routine, void *arg)
{
    self->routine = routine;
    self->arg = arg;
    self->handle = (HANDLE) _beginthreadex (NULL, 0, nn_thread_main_routine, (void*)self, 0 , NULL);
    win_assert(self->handle != NULL);
    return;
}

void nn_thread_term(struct nn_thread *self)
{
    DWORD rc;
    BOOL brc;

    rc = WaitForSingleObject(self->handle, INFINITE);
    win_assert(rc != WAIT_FAILED);
    brc = CloseHandle(self->handle);
    win_assert(brc != 0);
    return;
}
