#ifndef NN_CTX_INCLUDED
#define NN_CTX_INCLUDED

#include "../utils/mutex.h"
#include "../utils/queue.h"

#include "worker.h"
#include "pool.h"
#include "fsm.h"

typedef void (*nn_ctx_onleave)(struct nn_ctx *self);

struct nn_ctx {
    struct nn_mutex sync;
    struct nn_pool *pool;
    struct nn_queue events;
    struct nn_queue eventsto;
    nn_ctx_onleave onleave;
};

void nn_ctx_init(struct nn_ctx *self, struct nn_pool *pool, nn_ctx_onleave onleave);
void nn_ctx_term(struct nn_ctx *self);

void nn_ctx_enter(struct nn_ctx *self);
void nn_ctx_leave(struct nn_ctx *self);

struct nn_worker *nn_ctx_choose_worker(struct nn_ctx *self);

void nn_ctx_raise(struct nn_ctx *self, struct nn_fsm_event *event);
void nn_ctx_raiseto(struct nn_ctx *self, struct nn_fsm_event *event);

#endif // !NN_CTX_INCLUDED
