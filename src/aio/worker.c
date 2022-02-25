#include "worker.h"

#if defined (NN_HAVE_WINDOWS)
#include "worker_win.inc"
#else
#include "worker_posix.inc"
#endif

void nn_worker_timer_init(struct nn_worker_timer *self, struct nn_fsm *owner)
{
    self->owner = owner;
    nn_timerset_hndl_init(&self->hndl);
    return;
}

void nn_worker_timer_term(struct nn_worker_timer *self)
{
    nn_timerset_hndl_term(&self->hndl);
    return;
}

int nn_worker_timer_isactive(struct nn_worker_timer *self)
{
    return nn_timerset_hndl_isactive(&self->hndl);
}
