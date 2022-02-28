#include "pool.h"

int nn_pool_init(struct nn_pool *self) { return nn_worker_init(&self->worker); }

void nn_pool_term(struct nn_pool *self)
{
    nn_worker_term(&self->worker);
    return;
}

struct nn_worker *nn_pool_choose_worker(struct nn_pool *self)
{
    return &self->worker;
}
