#ifndef NN_STCP_INCLUDED
#define NN_STCP_INCLUDED

#include "../../transport.h"
#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "../utils/treamhdr.h"

#include "../../utils/msg.h"

#define NN_STCP_ERROR (1)
#define NN_STCP_STOPPED (2)

struct nn_stcp {
    struct nn_fsm fsm;
    int state;
    struct nn_usock *usock;
    struct nn_streamhdr streamhdr;
    struct nn_fsm_owner usock_owner;
    struct nn_pipebase pipebase;
    int instate;
    uint8_t inhdr[8];
    struct nn_msg inmsg;
    int outstate;
    uint8_t outhdr[8];
    struct nn_msg outmsg;
    struct nn_fsm_event done;
};

void nn_stcp_init(
    struct nn_stcp *self, int src, struct nn_ep *ep, struct nn_fsm *owner);
void nn_stcp_term(struct nn_stcp *self);

int nn_stcp_isidle(struct nn_stcp *self);
void nn_stcp_start(struct nn_stcp *self, struct nn_usock *usock);
void nn_stcp_stop(struct nn_stcp *self);

#endif // !NN_STCP_INCLUDED
