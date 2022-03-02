#ifndef NN_SINPROC_INCLUDED
#define NN_SINPROC_INCLUDED

#include "msgqueue.h"

#include "../../transport.h"

#include "../../aio/fsm.h"

#include "../../utils/msg.h"
#include "../../utils/list.h"

#define NN_SINPROC_CONNECT (1)
#define NN_SINPROC_READY (2)
#define NN_SINPROC_ACCEPTED (3)
#define NN_SINPROC_SENT (4)
#define NN_SINPROC_RECEIVED (5)
#define NN_SINPROC_DISCONNECT (6)
#define NN_SINPROC_STOPPED (7)

#define NN_SINPROC_SRC_PEER (27713)

struct nn_sinproc {
    struct nn_fsm fsm;
    int state;
    int flags;
    struct nn_sinproc *peer;
    struct nn_pipebase pipebase;
    struct nn_msgqueue msgqueue;
    struct nn_msg msg;

    struct nn_fsm_event event_connect;
    struct nn_fsm_event event_sent;
    struct nn_fsm_event event_received;
    struct nn_fsm_event event_disconnect;

    struct nn_list_item item;
};

void nn_sinproc_init(
    struct nn_sinproc *self, int src, struct nn_ep *ep, struct nn_fsm *owner);
void nn_sinproc_term(struct nn_sinproc *self);
int nn_sinproc_isidle(struct nn_sinproc *self);

void nn_sinproc_connect(struct nn_sinproc *self, struct nn_fsm *peer);
void nn_sinproc_accept(struct nn_sinproc *self, struct nn_sinproc *peer);
void nn_sinproc_stop(struct nn_sinproc *self);

#endif // !NN_SINPROC_INCLUDED
