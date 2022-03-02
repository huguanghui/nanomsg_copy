#ifndef NNS_AWS_INCLUDED
#define NNS_AWS_INCLUDED

#include "sws.h"

#include "../../transport.h"
#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "../../utils/list.h"

#define NN_AWS_ACCEPTED (34231)
#define NN_AWS_ERROR (34232)
#define NN_AWS_STOPPED (34233)

struct nn_aws {
    struct nn_fsm fsm;
    int state;

    struct nn_ep *ep;
    struct nn_usock usock;
    struct nn_usock *listener;
    struct nn_fsm_owner listener_owner;
    struct nn_sws sws;
    struct nn_fsm_event accepted;
    struct nn_fsm_event done;
    struct nn_list_item item;
};

void nn_aws_init(struct nn_aws *self, int src, struct nn_ep *ep, struct nn_fsm *owner);
void nn_aws_term(struct nn_aws *self);

int nn_aws_isidle(struct nn_aws *self);
void nn_aws_start(struct nn_aws *self, struct nn_usock *listener);
void nn_aws_stop(struct nn_aws *self);

#endif // !NNS_AWS_INCLUDED
