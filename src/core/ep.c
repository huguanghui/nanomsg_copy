#include "../transport.h"

#include "ep.h"
#include "sock.h"

#include "../utils/err.h"
#include "../utils/cont.h"
#include "../utils/fast.h"
#include "../utils/attr.h"

#include <string.h>

#define NN_EP_STATE_IDLE (1)
#define NN_EP_STATE_ACTIVE (2)
#define NN_EP_STATE_STOPPING (3)

#define NN_EP_ACTION_STOPPED (1)

static void nn_ep_handler(struct nn_fsm *self, int src, int type, void *srcptr);
static void nn_ep_shutdown(
    struct nn_fsm *self, int src, int type, void *srcptr);

int nn_ep_init(struct nn_ep *self, int src, struct nn_sock *sock, int eid,
    const struct nn_transport *transport, int bind, const char *addr)
{
    int rc;

    nn_fsm_init(
        &self->fsm, nn_ep_handler, nn_ep_shutdown, src, self, &sock->fsm);
    self->state = NN_EP_STATE_IDLE;

    self->sock = sock;
    self->eid = eid;
    self->last_errno = 0;
    nn_list_item_init(&self->item);
    memcpy(&self->options, &sock->ep_template, sizeof(struct nn_ep_options));

    nn_assert(strlen(addr) <= NN_SOCKADDR_MAX);
    strcpy(self->addr, addr);

    if (bind) {
        rc = transport->bind(self);
    } else {
        rc = transport->connect(self);
    }

    if (rc < 0)
    {
        nn_list_item_term(&self->item);
        nn_fsm_term(&self->fsm);
        return rc;
    }

    return 0;
}
