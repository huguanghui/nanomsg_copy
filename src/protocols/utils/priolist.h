#ifndef NN_PRIOLIST_INCLUDED
#define NN_PRIOLIST_INCLUDED

#include "../../protocol.h"

#include "../../utils/list.h"

#define NN_PRIOLIST_SLOTS 16

struct nn_priolist_data {
    struct nn_pipe *pipe;
    int priority;
    struct nn_list_item item;
};

struct nn_priolist_slot {
    struct nn_list pipes;
    struct nn_priolist_data *current;
};

struct nn_priolist {
    struct nn_priolist_slot slots[NN_PRIOLIST_SLOTS];
    int current;
};

void nn_priolist_init(struct nn_priolist *self);

void nn_priolist_term(struct nn_priolist *self);

void nn_priolist_add(struct nn_priolist *self, struct nn_priolist_data *data,
    struct nn_pipe *pipe, int priority);

void nn_priolist_rm(struct nn_priolist *self, struct nn_priolist_data *data);

void nn_priolist_activate(struct nn_priolist *self, struct nn_priolist_data *data);

int nn_priolist_is_active(struct nn_priolist *self);

struct nn_pipe *nn_priolist_getpipe(struct nn_priolist *self);

void nn_priolist_advance(struct nn_priolist *self, int release);

int nn_priolist_get_priority(struct nn_priolist *self);

#endif // !NN_PRIOLIST_INCLUDED
