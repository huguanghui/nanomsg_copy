#ifndef NN_FQ_INCLUDED
#define NN_FQ_INCLUDED

#include "../../protocol.h"

#include "priolist.h"

struct nn_fq_data {
    struct nn_priolist_data priodata;
};

struct nn_fq {
    struct nn_priolist priolist;
};

void nn_fq_init(struct nn_fq *self);
void nn_fq_term(struct nn_fq *self);
void nn_fq_add(struct nn_fq *self, struct nn_fq_data *data,
    struct nn_pipe *pipe, int priority);
void nn_fq_rm(struct nn_fq *self, struct nn_fq_data *data);
void nn_fq_in(struct nn_fq *self, struct nn_fq_data *data);
int nn_fq_can_recv(struct nn_fq *self);
int nn_fq_recv(struct nn_fq *self, struct nn_msg *msg, struct nn_pipe **pipe);

#endif // !NN_FQ_INCLUDED
