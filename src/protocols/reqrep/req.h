#ifndef NN_REQ_INCLUDED
#define NN_REQ_INCLUDED

#include "xreq.h"
#include "task.h"

#include "../../protocol.h"
#include "../../aio/fsm.h"

struct nn_req {
    struct nn_xreq xreq;
    struct nn_fsm fsm;
    int state;
    uint32_t lastid;
    int resend_ivl;
    struct nn_task task;
};

void nn_req_init(
    struct nn_req *self, const struct nn_sockbase_vfptr *vfptr, void *hint);
void nn_req_term(struct nn_req *self);
int nn_req_inprogress(struct nn_req *self);
void nn_req_handler(struct nn_fsm *self, int src, int type, void *srcptr);
void nn_req_shutdown(struct nn_fsm *self, int src, int type, void *srcptr);
void nn_req_action_send(struct nn_req *self, int allow_delay);

void nn_req_stop(struct nn_sockbase *self);
void nn_req_destroy(struct nn_sockbase *self);
void nn_req_in(struct nn_sockbase *self, struct nn_pipe *pipe);
void nn_req_out(struct nn_sockbase *self, struct nn_pipe *pipe);
int nn_req_events(struct nn_sockbase *self);
int nn_req_csend(struct nn_sockbase *self, struct nn_msg *msg);
void nn_req_rm(struct nn_sockbase *self, struct nn_pipe *pipe);
int nn_req_crecv(struct nn_sockbase *self, struct nn_msg *msg);
int nn_req_setopt(struct nn_sockbase *self, int level, int option,
    const void *optval, size_t optvallen);
int nn_req_getopt(struct nn_sockbase *self, int level, int option,
    void *optval, size_t *optvallen);
int nn_req_csend(struct nn_sockbase *self, struct nn_msg *msg);
int nn_req_crecv(struct nn_sockbase *self, struct nn_msg *msg);

#endif // !NN_REQ_INCLUDED
