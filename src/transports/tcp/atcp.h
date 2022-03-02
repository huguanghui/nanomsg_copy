#ifndef NN_ATCP_INCLUDED
#define NN_ATCP_INCLUDED

#include "stcp.h"

#include "../../transport.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "../../utils/list.h"

#define NN_ATCP_ACCEPTED (34231)
#define NN_ATCP_ERROR (34232)
#define NN_ATCP_STOPPED (34233)

struct nn_atcp {
    struct nn_fsm fsm;
    int state;

    struct nn_ep *ep;

    struct nn_usock usock;
    struct nn_usock *listener;
    struct nn_fsm_owner listener_owner;

    struct nn_stcp stcp;

    struct nn_fsm_event accepted;
    struct nn_fsm_event done;

    struct nn_list_item item;
};

void nn_atcp_init(
    struct nn_atcp *self, int src, struct nn_ep *ep, struct nn_fsm *owner);
void nn_atcp_term(struct nn_atcp *self);

int nn_atcp_isidle(struct nn_atcp *self);
void nn_atcp_start(struct nn_atcp *self, struct nn_usock *listener);
void nn_atcp_stop(struct nn_atcp *self);

#endif // !NN_ATCP_INCLUDED
