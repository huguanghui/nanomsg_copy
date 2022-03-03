#ifndef NN_XBUS_INCLUDED
#define NN_XBUS_INCLUDED

#include "../../protocol.h"

#include "../utils/dist.h"
#include "../utils/fq.h"

struct nn_xbus_data {
    struct nn_dist_data outitem;
    struct nn_fq_data initem;
};

struct nn_xbus {
    struct nn_sockbase sockbase;
    struct nn_dist outpipes;
    struct nn_fq inpipes;
};

void nn_xbus_init(
    struct nn_xbus *self, const struct nn_sockbase_vfptr *vfptr, void *hint);
void nn_xbuf_term(struct nn_xbus *self);
int nn_xbus_add(struct nn_sockbase *self, struct nn_pipe *pipe);
int nn_xbus_rm(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xbus_in(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xbus_out(struct nn_sockbase *self, struct nn_pipe *pipe);
int nn_xbus_events(struct nn_sockbase *self);
int nn_xbus_send(struct nn_sockbase *self, struct nn_msg *msg);
int nn_xbus_recv(struct nn_sockbase *self, struct nn_msg *msg);
int nn_xbus_ispeer(int socktype);

#endif // !NN_XBUS_INCLUDED
