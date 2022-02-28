#ifndef NN_POOL_INCLUDED
#define NN_POOL_INCLUDED

#include "worker.h"

struct nn_pool {
    struct nn_worker worker;
};

int nn_pool_init(struct nn_pool *self);
void nn_pool_term(struct nn_pool *self);
struct nn_worker *nn_pool_choose_worker(struct nn_pool *self);

#endif // !NN_POOL_INCLUDED
