#ifndef NN_REP_INCLUDED
#define NN_REP_INCLUDED

#include "../../protocol.h"
#include "xrep.h"

struct nn_rep {
    struct nn_xrep xrep;
    uint32_t flags;
    struct nn_chunkref backtrace;
};

void nn_rep_init(
    struct nn_rep *self, const struct nn_sockbase_vfptr *vfptr, void *hint);
void nn_rep_term(struct nn_rep *self);

void nn_rep_destroy(struct nn_sockbase *self);
int nn_rep_events(struct nn_sockbase *self);
int nn_rep_send(struct nn_sockbase *self, struct nn_msg *msg);
int nn_rep_recv(struct nn_sockbase *self, struct nn_msg *msg);

#endif // !NN_XREP_INCLUDED
