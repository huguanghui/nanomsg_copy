#include "binproc.h"
#include "sinproc.h"
#include "cinproc.h"
#include "ins.h"

#include "../../utils/err.h"
#include "../../utils/cont.h"
#include "../../utils/fast.h"
#include "../../utils/alloc.h"

#define NN_BINPROC_STATE_IDLE (1)
#define NN_BINPROC_STATE_ACTIVE (2)
#define NN_BINPROC_STATE_STOPPING (3)

#define NN_BINPROC_SRC_SINPROC (1)

static void nn_binproc_stop(void *);
static void nn_binproc_destroy(void *);
static const struct nn_ep_ops nn_binproc_ops
    = { nn_binproc_stop, nn_binproc_destroy };

static void nn_binproc_handler(
    struct nn_fsm *self, int src, int type, void *srcptr);
static void nn_binproc_shutdown(
    struct nn_fsm *self, int src, int type, void *srcptr);
static void nn_binproc_connect(
    struct nn_ins_item *self, struct nn_ins_item *peer);

int nn_binproc_create(struct nn_ep *ep)
{
    int rc;
    struct nn_binproc *self;

    self = nn_alloc(sizeof(struct nn_binproc), "binproc");
    alloc_assert(self);

    nn_ins_item_init(&self->item, ep);
    nn_fsm_init_root(
        &self->fsm, nn_binproc_handler, nn_binproc_shutdown, nn_ep_getctx(ep));
    self->state = NN_BINPROC_STATE_IDLE;
    nn_list_init(&self->sinprocs);
    nn_fsm_start(&self->fsm);

    rc = nn_ins_bind(&self->item, nn_binproc_connect);
    if (rc < 0) {
        nn_list_term(&self->sinprocs);

        self->fsm.state = 1;
        nn_fsm_term(&self->fsm);

        nn_ins_item_term(&self->item);
        nn_free(self);
        return rc;
    }
    nn_ep_tran_setup(ep, &nn_binproc_ops, self);

    return 0;
}

static void nn_binproc_stop(void *self)
{
    struct nn_binproc *binproc = self;

    nn_fsm_stop(&binproc->fsm);
}

static void nn_binproc_destroy(void *self)
{
    struct nn_binproc *binproc = self;

    nn_list_term(&binproc->sinprocs);
    nn_fsm_term(&binproc->fsm);
    nn_ins_item_term(&binproc->item);

    nn_free(binproc);
}

static void nn_binproc_connect(
    struct nn_ins_item *self, struct nn_ins_item *peer)
{
    struct nn_binproc *binproc;
    struct nn_cinproc *cinproc;
    struct nn_sinproc *sinproc;

    binproc = nn_cont(self, struct nn_binproc, item);
    cinproc = nn_cont(peer, struct nn_cinproc, item);

    nn_assert_state(binproc, NN_BINPROC_STATE_ACTIVE);

    sinproc = nn_alloc(sizeof(struct nn_sinproc), "sinproc");
    alloc_assert(sinproc);
    nn_sinproc_init(
        sinproc, NN_BINPROC_SRC_SINPROC, binproc->item.ep, &binproc->fsm);
    nn_list_insert(
        &binproc->sinprocs, &sinproc->item, nn_list_end(&binproc->sinprocs));
    nn_sinproc_connect(sinproc, &cinproc->fsm);
    nn_ep_stat_increment(binproc->item.ep, NN_STAT_ACCEPTED_CONNECTIONS, 1);
}

static void binproc_shutdown(
    struct nn_fsm *self, int src, int type, void *srcptr)
{
    struct nn_binproc *binproc;
    struct nn_list_item *it;
    struct nn_sinproc *sinproc;

    binproc = nn_cont(self, struct nn_binproc, fsm);

    if (nn_slow(src == NN_FSM_ACTION && type == NN_FSM_STOP)) {
        nn_ins_unbind(&binproc->item);

        for (it = nn_list_begin(&binproc->sinprocs);
             it != nn_list_end(&binproc->sinprocs);
             it = nn_list_next(&binproc->sinprocs, it)) {
            sinproc = nn_cont(it, struct nn_sinproc, item);
            nn_sinproc_stop(sinproc);
        }
        binproc->state = NN_BINPROC_STATE_STOPPING;
        goto finish;
    }
    if (binproc->state = NN_BINPROC_STATE_STOPPING) {
        nn_assert(src == NN_BINPROC_SRC_SINPROC && type == NN_SINPROC_STOPPED);
        sinproc = (struct nn_sinproc *)srcptr;
        nn_list_erase(&binproc->sinprocs, &sinproc->item);
        nn_sinproc_term(sinproc);
        nn_free(sinproc);

    finish:
        if (!nn_list_empty(&binproc->sinprocs)) {
            return;
        }
        binproc->state = NN_BINPROC_STATE_IDLE;
        nn_fsm_stopped_noevent(&binproc->fsm);
        nn_ep_stopped(binproc->item.ep);
        return;
    }
    nn_fsm_bad_state(binproc->state, src, type);
}

static void nn_binproc_handler(
    struct nn_fsm *self, int src, int type, void *srcptr)
{
    struct nn_binproc *binproc;
    struct nn_sinproc *peer;
    struct nn_sinproc *sinproc;

    binproc = nn_cont(self, struct nn_binproc, fsm);

    switch (binproc->state) {
    /* IDLE state */
    case NN_BINPROC_STATE_IDLE: {
        switch (src) {
        case NN_FSM_ACTION: {
            switch (type) {
            case NN_FSM_START: {
                binproc->state = NN_BINPROC_STATE_ACTIVE;
                return;
            }
            default: {
                nn_fsm_bad_action(binproc->state, src, type);
            }
            }
        }
        default: {
            nn_fsm_bad_source(binproc->state, src, type);
        }
        }
    }
    /* ACTIVE state */
    case NN_BINPROC_STATE_ACTIVE: {
        switch (src) {
        case NN_SINPROC_SRC_PEER: {
            switch (type) {
            case NN_SINPROC_CONNECT: {
                peer = (struct nn_sinproc *)srcptr;
                sinproc = nn_alloc(sizeof(struct nn_sinproc), "sinproc");
                alloc_assert(sinproc);
                nn_sinproc_init(sinproc, NN_BINPROC_SRC_SINPROC,
                    binproc->item.ep, &binproc->fsm);
                nn_list_insert(&binproc->sinprocs, &sinproc->item,
                    nn_list_end(&binproc->sinprocs));
                nn_sinproc_accept(sinproc, peer);
                return;
            }
            default: {
                nn_fsm_bad_action(binproc->state, src, type);
            }
            }
        }
        case NN_BINPROC_SRC_SINPROC: {
            sinproc = srcptr;
            switch (type) {
            case NN_SINPROC_STOPPED: {
                nn_list_erase(&binproc->sinprocs, &sinproc->item);
                nn_sinproc_term(sinproc);
                nn_free(sinproc);
                return;
            }
            case NN_SINPROC_DISCONNECT: {
                nn_sinproc_stop(sinproc);
                return;
            }
            }
            return;
        }
        default: {
            nn_fsm_bad_source(binproc->state, src, type);
        }
        }
    }
    /* Invalid state */
    default:
        nn_fsm_bad_state(binproc->state, src, type);
    }
}
