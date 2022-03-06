#include "sinproc.h"

#include "../../utils/err.h"
#include "../../utils/cont.h"
#include "../../utils/attr.h"

#include <stddef.h>

#define NN_SINPROC_STATE_IDLE (1)
#define NN_SINPROC_STATE_CONNECTING (2)
#define NN_SINPROC_STATE_READY (3)
#define NN_SINPROC_STATE_ACTIVE (4)
#define NN_SINPROC_STATE_DISCONNECTED (5)
#define NN_SINPROC_STATE_STOPPING_PEER (6)
#define NN_SINPROC_STATE_STOPPING (7)

#define NN_SINPROC_ACTION_READY (1)
#define NN_SINPROC_ACTION_ACCEPTED (2)

#define NN_SINPROC_FLAG_SENDING (1)
#define NN_SINPROC_FLAG_RECEIVING (2)

static void nn_sinproc_handler(
    struct nn_fsm *self, int src, int type, void *srcptr);
static void nn_sinproc_shutdown(
    struct nn_fsm *self, int src, int type, void *srcptr);

static int nn_sinproc_send(struct nn_pipebase *self, struct nn_msg *msg);
static int nn_sinproc_recv(struct nn_pipebase *self, struct nn_msg *msg);

const struct nn_pipebase_vfptr nn_sinproc_pipebase_vfptr
    = { nn_sinproc_send, nn_sinproc_recv };

void nn_sinproc_init(
    struct nn_sinproc *self, int src, struct nn_ep *ep, struct nn_fsm *owner)
{
    int rcvbuf;
    size_t sz;

    nn_fsm_init(
        &self->fsm, nn_sinproc_handler, nn_sinproc_shutdown, src, self, owner);
    self->state = NN_SINPROC_STATE_IDLE;
    self->flags = 0;
    self->peer = NULL;
    nn_pipebase_init(&self->pipebase, &nn_sinproc_pipebase_vfptr, ep);
    sz = sizeof(rcvbuf);
    nn_ep_getopt(ep, NN_SOL_SOCKET, NN_RCVBUF, &rcvbuf, &sz);
    nn_assert(sz == sizeof(rcvbuf));
    nn_msgqueue_init(&self->msgqueue, rcvbuf);
    nn_msg_init(&self->msg, 0);
    nn_fsm_event_init(&self->event_connect);
    nn_fsm_event_init(&self->event_sent);
    nn_fsm_event_init(&self->event_received);
    nn_fsm_event_init(&self->event_disconnect);
    nn_list_item_init(&self->item);
}

void nn_sinproc_term(struct nn_sinproc *self)
{
    nn_list_item_term(&self->item);
    nn_fsm_event_term(&self->event_disconnect);
    nn_fsm_event_term(&self->event_received);
    nn_fsm_event_term(&self->event_sent);
    nn_fsm_event_term(&self->event_connect);
    nn_msg_term(&self->msg);
    nn_msgqueue_term(&self->msgqueue);
    nn_pipebase_term(&self->pipebase);
    nn_fsm_term(&self->fsm);
}

int nn_sinproc_isidle(struct nn_sinproc *self)
{
    return nn_fsm_isidle(&self->fsm);
}

void nn_sinproc_connect(struct nn_sinproc *self, struct nn_fsm *peer)
{
    nn_fsm_start(&self->fsm);

    nn_fsm_raiseto(&self->fsm, peer, &self->event_connect, NN_SINPROC_SRC_PEER,
        NN_SINPROC_CONNECT, self);

    nn_fsm_start(&self->fsm);
    nn_fsm_action(&self->fsm, NN_SINPROC_ACTION_READY);
}

void nn_sinproc_stop(struct nn_sinproc *self) { nn_fsm_stop(&self->fsm); }

static int nn_sinproc_send(struct nn_pipebase *self, struct nn_msg *msg)
{
    struct nn_sinproc *sinproc;
    struct nn_msg nmsg;

    sinproc = nn_cont(self, struct nn_sinproc, pipebase);

    if (sinproc->state == NN_SINPROC_STATE_DISCONNECTED) {
        return -ECONNRESET;
    }

    nn_assert_state(sinproc, NN_SINPROC_STATE_ACTIVE);
    nn_assert(!(sinproc->flags & NN_SINPROC_FLAG_SENDING));

    nn_msg_init(
        &nmsg, nn_chunkref_size(&msg->sphdr) + nn_chunkref_size(&msg->body));
    memcpy(nn_chunkref_data(&nmsg.body), nn_chunkref_data(&msg->sphdr),
        nn_chunkref_size(&msg->sphdr));
    memcpy((char *)nn_chunkref_data(&nmsg.body) + nn_chunkref_size(&msg->sphdr),
        nn_chunkref_data(&msg->body), nn_chunkref_size(&msg->body));
    nn_msg_term(msg);

    nn_msg_term(&sinproc->msg);
    nn_msg_mv(&sinproc->msg, &nmsg);

    sinproc->flags |= NN_SINPROC_FLAG_SENDING;
    nn_fsm_raiseto(&sinproc->fsm, &sinproc->peer->fsm,
        &sinproc->peer->event_sent, NN_SINPROC_SRC_PEER, NN_SINPROC_SENT,
        sinproc);

    return 0;
}

static int nn_sinproc_recv(struct nn_pipebase *self, struct nn_msg *msg)
{
    int rc;
    struct nn_sinproc *sinproc;

    sinproc = nn_cont(self, struct nn_sinproc, pipebase);

    nn_assert(sinproc->state == NN_SINPROC_STATE_ACTIVE
        || sinproc->state == NN_SINPROC_STATE_DISCONNECTED);

    rc = nn_msgqueue_recv(&sinproc->msgqueue, msg);
    errnum_assert(rc == 0, -rc);

    if (sinproc->state != NN_SINPROC_STATE_DISCONNECTED) {
        if (nn_slow(sinproc->flags & NN_SINPROC_FLAG_RECEIVING)) {
            rc = nn_msgqueue_send(&sinproc->msgqueue, &sinproc->peer->msg);
            nn_assert(rc == 0 || rc == -EAGAIN);
            if (rc == 0) {
                errnum_assert(rc == 0, -rc);
                nn_msg_init(&sinproc->peer->msg, 0);
                nn_fsm_raiseto(&sinproc->fsm, &sinproc->peer->fsm,
                    &sinproc->peer->event_received, NN_SINPROC_SRC_PEER,
                    NN_SINPROC_RECEIVED, sinproc);
                sinproc->flags &= ~NN_SINPROC_FLAG_RECEIVING;
            }
        }
    }

    if (!nn_msgqueue_empty(&sinproc->msgqueue)) {
        nn_pipebase_received(&sinproc->pipebase);
    }

    return 0;
}

static void nn_sinproc_shutdown_events(
    struct nn_sinproc *self, int src, int type, NN_UNUSED void *srcptr)
{
    switch (src) {
    case NN_FSM_ACTION: {
        switch (type) {
        case NN_FSM_STOP: {
            if (self->state != NN_SINPROC_STATE_IDLE
                && self->state != NN_SINPROC_STATE_DISCONNECTED) {
                nn_pipebase_stop(&self->pipebase);
                nn_assert(self->fsm.state == 2 || self->fsm.state == 3);
                nn_fsm_raiseto(&self->fsm, &self->peer->fsm,
                    &self->peer->event_disconnect, NN_SINPROC_SRC_PEER,
                    NN_SINPROC_DISCONNECT, self);
                self->state = NN_SINPROC_STATE_STOPPING_PEER;
            } else {
                self->state = NN_SINPROC_STATE_STOPPING;
            }
            return;
        }
        default: {
            break;
        }
        }
    }
    case NN_SINPROC_SRC_PEER: {
        switch (type) {
        case NN_SINPROC_RECEIVED: {
            return;
        }
        }
    }
    }
    switch (self->state) {
    case NN_SINPROC_STATE_STOPPING_PEER: {
        switch (src) {
        case NN_SINPROC_SRC_PEER: {
            switch (type) {
            case NN_SINPROC_DISCONNECT: {
                self->state = NN_SINPROC_STATE_STOPPING;
                return;
            }
            default: {
                return;
            }
            }
        }
        default: {
            nn_fsm_bad_source(self->state, src, type);
        }
        }
    }
    default: {
        nn_fsm_bad_state(self->state, src, type);
    }
    }

    nn_fsm_bad_action(self->state, src, type);
}

static void nn_sinproc_shutdown(
    struct nn_fsm *self, int src, int type, void *srcptr)
{
    struct nn_sinproc *sinproc;

    sinproc = nn_cont(self, struct nn_sinproc, fsm);
    nn_assert(sinproc->fsm.state == 3);

    nn_sinproc_shutdown_events(sinproc, src, type, srcptr);

    if (nn_slow(sinproc->state != NN_SINPROC_STATE_STOPPING)) {
        return;
    }

    if (nn_fsm_event_active(&sinproc->event_received)
        || nn_fsm_event_active(&sinproc->event_disconnect)) {
        return;
    }

    nn_assert(!nn_fsm_event_active(&sinproc->event_connect));
    nn_assert(!nn_fsm_event_active(&sinproc->event_sent));

    nn_fsm_stopped(&sinproc->fsm, NN_SINPROC_STOPPED);
    return;
}

static void nn_sinproc_handler(
    struct nn_fsm *self, int src, int type, void *srcptr)
{
    int rc;
    struct nn_sinproc *sinproc;
    int empty;

    sinproc = nn_cont(self, struct nn_sinproc, fsm);

    switch (sinproc->state) {
    /* IDLE state */
    case NN_SINPROC_STATE_IDLE: {
        switch (src) {
        case NN_FSM_ACTION: {
            switch (type) {
            case NN_FSM_START: {
                sinproc->state = NN_SINPROC_STATE_CONNECTING;
                return;
            }
            default: {
                nn_fsm_bad_source(sinproc->state, src, type);
            }
            }
        }
        default: {
            nn_fsm_bad_source(sinproc->state, src, type);
        }
        }
    }
    /* CONNECTING state */
    case NN_SINPROC_STATE_CONNECTING: {
        switch (src) {
        case NN_FSM_ACTION: {
            switch (type) {
            case NN_SINPROC_ACTION_READY: {
                sinproc->state = NN_SINPROC_STATE_READY;
                return;
            }
            default: {
                nn_fsm_bad_action(sinproc->state, src, type);
            }
            }
        }
        case NN_SINPROC_SRC_PEER: {
            switch (type) {
            case NN_SINPROC_READY: {
                sinproc->peer = (struct nn_sinproc *)srcptr;
                rc = nn_pipebase_start(&sinproc->pipebase);
                errnum_assert(rc == 0, -rc);
                sinproc->state = NN_SINPROC_STATE_ACTIVE;
                nn_fsm_raiseto(&sinproc->fsm, &sinproc->peer->fsm,
                    &sinproc->event_connect, NN_SINPROC_SRC_PEER,
                    NN_SINPROC_ACCEPTED, self);
                return;
            }
            default: {
                nn_fsm_bad_action(sinproc->state, src, type);
            }
            }
        }
        default: {
            nn_fsm_bad_source(sinproc->state, src, type);
        }
        }
    }
    /* READY state */
    case NN_SINPROC_STATE_READY: {
        switch (src) {
        case NN_SINPROC_SRC_PEER: {
            switch (type) {
            case NN_SINPROC_READY: {
                rc = nn_pipebase_start(&sinproc->pipebase);
                errnum_assert(rc == 0, -rc);
                sinproc->state = NN_SINPROC_STATE_ACTIVE;
                return;
            }
            case NN_SINPROC_ACCEPTED: {
                rc = nn_pipebase_start(&sinproc->pipebase);
                if (rc != 0) {
                    nn_pipebase_stop(&sinproc->pipebase);
                    sinproc->state = NN_SINPROC_STATE_DISCONNECTED;
                    sinproc->peer = NULL;
                    nn_fsm_raise(&sinproc->fsm, &sinproc->event_disconnect,
                        NN_SINPROC_DISCONNECT);
                    return;
                }
                errnum_assert(rc == 0, -rc);
                sinproc->state = NN_SINPROC_STATE_ACTIVE;
                return;
            }
            default: {
                nn_fsm_bad_action(sinproc->state, src, type);
            }
            }
        }
        default: {
            nn_fsm_bad_source(sinproc->state, src, type);
        }
        }
    }
    /* ACTIVE state */
    case NN_SINPROC_STATE_ACTIVE: {
        switch (src) {
        case NN_SINPROC_SRC_PEER: {
            switch (type) {
            case NN_SINPROC_SENT: {
                empty = nn_msgqueue_empty(&sinproc->msgqueue);

                rc = nn_msgqueue_send(&sinproc->msgqueue, &sinproc->peer->msg);
                if (rc == -EAGAIN) {
                    sinproc->flags |= NN_SINPROC_FLAG_RECEIVING;
                    return;
                }
                errnum_assert(rc == 0, -rc);
                nn_msg_init(&sinproc->peer->msg, 0);

                if (empty) {
                    nn_pipebase_received(&sinproc->pipebase);
                }

                nn_fsm_raiseto(&sinproc->fsm, &sinproc->peer->fsm,
                    &sinproc->peer->event_received, NN_SINPROC_SRC_PEER,
                    NN_SINPROC_RECEIVED, sinproc);
                return;
            }
            case NN_SINPROC_RECEIVED: {
                nn_assert(sinproc->flags & NN_SINPROC_FLAG_SENDING);
                nn_pipebase_sent(&sinproc->pipebase);
                sinproc->flags &= ~NN_SINPROC_FLAG_SENDING;
                return;
            }
            case NN_SINPROC_DISCONNECT: {
                nn_pipebase_stop(&sinproc->pipebase);
                nn_fsm_raiseto(&sinproc->fsm, &sinproc->peer->fsm,
                    &sinproc->peer->event_disconnect, NN_SINPROC_SRC_PEER,
                    NN_SINPROC_DISCONNECT, sinproc);
                sinproc->state = NN_SINPROC_STATE_DISCONNECTED;
                sinproc->peer = NULL;
                nn_fsm_raise(&sinproc->fsm, &sinproc->event_disconnect,
                    NN_SINPROC_DISCONNECT);
                return;
            }
            default: {
                nn_fsm_bad_action(sinproc->state, src, type);
            }
            }
        }
        default: {
            nn_fsm_bad_source(sinproc->state, src, type);
        }
        }
    }
    /* DISCONNECTED state */
    case NN_SINPROC_STATE_DISCONNECTED: {
        switch (src) {
        case NN_SINPROC_SRC_PEER: {
            switch (type) {
            case NN_SINPROC_RECEIVED: {
                return;
            }
            default: {
                nn_fsm_bad_action(sinproc->state, src, type);
            }
            }
        }
        default: {
            nn_fsm_bad_source(sinproc->state, src, type);
        }
        }
    }
    /* Invalid state */
    default: {
        nn_fsm_bad_state(sinproc->state, src, type);
    }
    }
}