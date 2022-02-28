#include "ctx.h"

#include "../utils/err.h"
#include "../utils/cont.h"
#include "../utils/fast.h"

void nn_ctx_init(
    struct nn_ctx *self, struct nn_pool *pool, nn_ctx_onleave onleave)
{
    nn_mutex_init(&self->sync);
    self->pool = pool;
    nn_queue_init(&self->events);
    nn_queue_init(&self->eventsto);
    self->onleave = onleave;
}

void nn_ctx_term(struct nn_ctx *self)
{
    nn_queue_term(&self->eventsto);
    nn_queue_term(&self->events);
    nn_mutex_term(&self->sync);
}

void nn_ctx_enter(struct nn_ctx *self) { nn_mutex_lock(&self->sync); }

void nn_ctx_leave(struct nn_ctx *self)
{
    struct nn_queue_item *item;
    struct nn_fsm_event *event;
    struct nn_queue eventsto;

    while (1) {
        item = nn_queue_pop(&self->events);
        event = nn_cont(item, struct nn_fsm_event, item);
        if (!event) {
            break;
        }
        nn_fsm_event_process(event);
    }

    if (nn_fast(self->onleave != NULL)) {
        self->onleave(self);
    }

    if (nn_queue_empty(&self->eventsto)) {
        nn_mutex_unlock(&self->sync);
        return;
    }

    eventsto = self->eventsto;
    nn_queue_init(&self->eventsto);
    nn_mutex_unlock(&self->sync);

    while (1) {
        item = nn_queue_pop(&eventsto);
        event = nn_cont(item, struct nn_fsm_event, item);
        if (!event) {
            break;
        }
        nn_ctx_enter(event->fsm->ctx);
        nn_fsm_event_process(event);
        nn_ctx_leave(event->fsm->ctx);
    }

    nn_queue_term(&eventsto);
    return;
}

struct nn_worker *nn_ctx_choose_worker(struct nn_ctx *self)
{
    return nn_pool_choose_worker(self->pool);
}

void nn_ctx_raise(struct nn_ctx *self, struct nn_fsm_event *event)
{
    nn_queue_push(&self->events, &event->item);
    return;
}

void nn_ctx_raiseto(struct nn_ctx *self, struct nn_fsm_event *event)
{
    nn_queue_push(&self->eventsto, &event->item);
    return;
}
