#include "fsm.h"
// #include "ctx.h"

#include "../utils/err.h"
#include "../utils/attr.h"

#include <stddef.h>

#define NN_FSM_STATE_IDLE 1
#define NN_FSM_STATE_ACTIVE 2
#define NN_FSM_STATE_STOPPING 3

void nn_fsm_event_init(struct nn_fsm_event *self)
{
    self->fsm = NULL;
    self->src = -1;
    self->srcptr = NULL;
    self->type = -1;
    nn_queue_item_init(&self->item);
}

void nn_fsm_event_term(NN_UNUSED struct nn_fsm_event *self) {}

int nn_fsm_event_active(struct nn_fsm_event *self)
{
    return nn_queue_item_isinqueue(&self->item);
}

void nn_fsm_event_process(struct nn_fsm_event *self)
{
    int src;
    int type;
    void *srcptr;

    src = self->src;
    type = self->type;
    srcptr = self->srcptr;
    self->src = -1;
    self->type = -1;
    self->srcptr = NULL;

    nn_fsm_feed(self->fsm, src, type, srcptr);
    return;
}

void nn_fsm_feed(struct nn_fsm *self, int src, int type, void *srcptr)
{
    if (nn_slow(self->state != NN_FSM_STATE_STOPPING)) {
        self->fn(self, src, type, srcptr);
    } else {
        self->shutdown_fn(self, src, type, srcptr);
    }
    return;
}
