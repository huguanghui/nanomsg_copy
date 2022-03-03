#ifndef NN_LB_INCLUDED
#define NN_LB_INCLUDED

#include "../../protocol.h"

#include "priolist.h"

struct nn_lb_data {
    struct nn_priolist_data priodata;
};

struct nn_lb {
    struct nn_priolist priolist;
};

void nn_lb_init(struct nn_lb *self);
void nn_lb_term(struct nn_lb *self);
void nn_lb_add(struct nn_lb *self, struct nn_lb_data *data,
    struct nn_pipe *pipe, int priority);
void nn_lb_rm(struct nn_lb *self, struct nn_lb_data *data);
void nn_lb_out(struct nn_lb *self, struct nn_lb_data *data);
int nn_lb_can_send(struct nn_lb *self);
int nn_lb_get_priority(struct nn_lb *self);
int nn_lb_send(struct nn_lb *self, struct nn_msg *msg, struct nn_pipe **to);

#endif // !NN_LB_INCLUDED
