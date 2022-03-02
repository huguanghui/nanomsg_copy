#ifndef NN_SIPC_INCLUDED
#define NN_SIPC_INCLUDED

#include "../../transport.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "../utils/streamhdr.h"

#include "../../utils/msg.h"

#define NN_SIPC_ERROR (1)
#define NN_SIPC_STOPPED (2)

struct nn_sipc {
    struct nn_fsm fsm;
    int state;
    struct nn_usock *usock;
    struct nn_streamhdr streamhdr;
    struct nn_fsm_owner usock_owner;
    struct nn_pipebase pipebase;
    int instate;
    uint8_t inhdr[9];
    struct nn_msg inmsg;
    int outstate;
    uint8_t outhdr[9];
    struct nn_msg outmsg;
    struct nn_fsm_event done;
};

void nn_sipc_init(struct nn_sipc *self, int src, struct nn_ep *ep, struct nn_fsm *owner);
void nn_sipc_term(struct nn_sipc *self);

int nn_sipc_isidle(struct nn_sipc *self);
void nn_spic_start(struct nn_sipc *self, struct nn_usock *usock);
void nn_sipc_stop(struct nn_sipc *self);

#endif // !NN_SIPC_INCLUDED
