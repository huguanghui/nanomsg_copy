#ifndef NN_EXCL_INCLUDED
#define NN_EXCL_INCLUDED

#include "../../protocol.h"

#include <stddef.h>

struct nn_excl {
    struct nn_pipe *pipe;
    struct nn_pipe *inpipe;
    struct nn_pipe *outpipe;
};

void nn_excl_init(struct nn_excl *self);
void nn_excl_term(struct nn_excl *self);
int nn_excl_add(struct nn_excl *self, struct nn_pipe *pipe);
void nn_excl_rm(struct nn_excl *self, struct nn_pipe *pipe);
void nn_excl_in(struct nn_excl *self, struct nn_pipe *pipe);
void nn_excl_out(struct nn_excl *self, struct nn_pipe *pipe);
int nn_excl_send(struct nn_excl *self, struct nn_msg *msg);
int nn_excl_recv(struct nn_excl *self, struct nn_msg *msg);
int nn_excl_can_send(struct nn_excl *self);
int nn_excl_can_recv(struct nn_excl *self);

#endif // !NN_EXCL_INCLUDED
