#ifndef NN_XREP_INCLUDED
#define NN_XREP_INCLUDED

#include "../../protocol.h"
#include "../../utils/hash.h"
#include "../utils/fq.h"

#include <stddef.h>

#define NN_XREP_OUT (1)

struct nn_xrep_data {
    struct nn_pipe *pipe;
    struct nn_hash_item outitem;
    struct nn_fq_data initem;
    uint32_t flags;
};

struct nn_xrep {
    struct nn_sockbase sockbase;
    uint32_t next_key;
    struct nn_hash outpipes;
    struct nn_fq inpipes;
};

void nn_xrep_init(struct nn_xrep *self, const struct nn_sockbase_vfptr *vfptr, void *hint);
void nn_xrep_term(struct nn_xrep *self);

int nn_xrep_add(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xrep_rm(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xrep_in(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xrep_out(struct nn_sockbase *self, struct nn_pipe *pipe);
int nn_xrep_events(struct nn_sockbase *self);
int nn_xrep_send(struct nn_sockbase *self, struct nn_msg *msg);
int nn_xrep_recv(struct nn_sockbase *self, struct nn_msg *msg);

int nn_xrep_ispeer(int socktype);

#endif // !NN_XREP_INCLUDED
