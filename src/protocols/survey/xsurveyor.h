#ifndef NN_XSURVEYOR_INCLUDED
#define NN_XSURVEYOR_INCLUDED

#include "../../protocol.h"

#include "../utils/dist.h"
#include "../utils/fq.h"

struct nn_xsurveyor_data {
    struct nn_pipe *pipe;
    struct nn_dist_data outitem;
    struct nn_fq_data initem;
};

struct nn_xsurveyor {
    struct nn_sockbase sockbase;
    struct nn_dist outpipes;
    struct nn_fq inpipes;
};

void nn_xsurveyor_init(struct nn_xsurveyor *self,
    const struct nn_sockbase_vfptr *vfptr, void *hint);
void nn_xsurveyor_term(struct nn_xsurveyor *self);

int nn_xsurveyor_add(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xsurveyor_rm(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xsurveyor_in(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_xsurveyor_out(struct nn_sockbase *self, struct nn_pipe *pipe);
int nn_xsurveyor_events(struct nn_sockbase *self);
int nn_xsurveyor_send(struct nn_sockbase *self, struct nn_msg *msg);
int nn_xsurveyor_recv(struct nn_sockbase *self, struct nn_msg *msg);
int nn_xsurveyor_setopt(struct nn_sockbase *self, int level, int option,
    const void *optval, size_t optvallen);
int nn_xsurveyor_getopt(struct nn_sockbase *self, int level, int option,
    void *optval, size_t *optvallen);

int nn_xsurveyor_ispeer(int socktype);

#endif // !NN_XSURVEYOR_INCLUDED
