#ifndef NN_DIST_INCLUDED
#define NN_DIST_INCLUDED

#include "../../protocol.h"

#include "../../utils/list.h"

struct nn_dist_data {
    struct nn_list_item item;
    struct nn_pipe *pipe;
};

struct nn_dist {
    uint32_t count;
    struct nn_list pipes;
};

void nn_dist_init(struct nn_dist *self);
void nn_dist_term(struct nn_dist *self);
void nn_dist_add(
    struct nn_dist *self, struct nn_dist_data *data, struct nn_pipe *pipe);
void nn_dist_rm(struct nn_dist *self, struct nn_dist_data *data);
void nn_dist_out(struct nn_dist *self, struct nn_dist_data *data);

int nn_dist_send(
    struct nn_dist *self, struct nn_msg *msg, struct nn_pipe *exclude);

#endif // !NN_DIST_INCLUDED
