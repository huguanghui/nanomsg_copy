#ifndef NN_XRESPONDENT_INCLUDED
#define NN_XRESPONDENT_INCLUDED

#include "../../protocol.h"

#include "../../utils/hash.h"
#include "../utils/fq.h"

#define NN_XRESPONDENT_OUT (1)

struct nn_xrespondent_data {
    struct nn_pipe *pipe;
    struct nn_hash_item outitem;
    struct nn_fq_data initem;
    uint32_t flags;
};

struct nn_xrespondent {
    struct nn_sockbase sockbase;
    uint32_t next_key;
    struct nn_hash outpipes;
    struct nn_fq inpipes;
};

void nn_xrespondent_init(struct nn_xrespondent *self,
    const struct nn_sockbase_vfptr *vfptr, void *hint);
void nn_xrespondent_term(struct nn_xrespondent *self);

int nn_xrespondent_add(struct nn_xrespondent *self, struct nn_pipe *pipe);
void nn_xrespondent_rm(struct nn_xrespondent *self, struct nn_pipe *pipe);
void nn_xrespondent_in(struct nn_xrespondent *self, struct nn_pipe *pipe);
void nn_xrespondent_out(struct nn_xrespondent *self, struct nn_pipe *pipe);
int nn_xrespondent_events(struct nn_xrespondent *self);
int nn_xrespondent_send(struct nn_xrespondent *self, struct nn_msg *msg);
int nn_xrespondent_recv(struct nn_xrespondent *self, struct nn_msg *msg);

int nn_xrespondent_ispeer(int socktype);

#endif // !NN_XRESPONDENT_INCLUDED
