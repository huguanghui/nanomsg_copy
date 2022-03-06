#ifndef NN_XREQ_INCLUDED
#define NN_XREQ_INCLUDED

#include "../../protocol.h"

#include "../utils/lb.h"
#include "../utils/fq.h"

struct nn_xreq {
    struct nn_sockbase sockbase;
    struct nn_lb lb;
    struct nn_fq fq;
};

void nn_xreq_init(
    struct nn_xreq *self, const struct nn_sockbase_vfptr *vfptr, void *hint);
void nn_xreq_term(struct nn_xreq *self);

int nn_xreq_add(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xreq_rm(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xreq_in(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xreq_out(struct nn_sockbase *self, struct nn_pipe *pipe);
int nn_xreq_events(struct nn_sockbase *self);
int nn_xreq_send(struct nn_sockbase *self, struct nn_msg *msg);
int nn_xreq_send_to(
    struct nn_sockbase *self, struct nn_msg *msg, struct nn_pipe *to);
int nn_xreq_recv(struct nn_sockbase *self, struct nn_msg *msg);
int nn_xreq_ispeer(int socktype);

#endif // !NN_XREQ_INCLUDED
