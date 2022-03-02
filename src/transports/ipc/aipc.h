#ifndef NN_AIPC_INCLUDED
#define NN_AIPC_INCLUDED

#include "sipc.h"

#include "../../transport.h"
#include "../../ipc.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "../../utils/list.h"

#define NN_AIPC_ACCEPTED 34231
#define NN_AIPC_ERROR 34232
#define NN_AIPC_STOPPED 34233

struct nn_aipc {
    struct nn_fsm fsm;
    int state;
    struct nn_ep *ep;
    struct nn_usock usock;
    struct nn_usock *listener;
    struct nn_fsm_owner listener_owner;
    struct nn_sipc sipc;
    struct nn_fsm_event accepted;
    struct nn_fsm_event done;

    struct nn_list_item item;
};

void nn_aipc_init(struct nn_apic *self, int src, struct nn_ep *ep, struct nn_fsm *owner);
void nn_aipc_term(struct nn_apic *self);

int nn_aipc_isidle(struct nn_apic *self);
void nn_aipc_start(struct nn_apic *self, struct nn_usock *listener);
void nn_aipc_stop(struct nn_apic *self);

#endif // !NN_AIPC_INCLUDED
