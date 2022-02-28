#ifndef NN_EP_INCLUDED
#define NN_EP_INCLUDED

#include "../transport.h"
#include "../aio/fsm.h"
#include "../utils/list.h"

#define NN_EP_STOPPED (1)

struct nn_ep {
    struct nn_fsm fsm;
    int state;
    struct nn_sock *sock;
    struct nn_ep_options options;
    int eid;
    struct nn_list_item item;
    char addr[NN_SOCKADDR_MAX + 1];
    int protocol;
    int last_errno;
    void *tran;

    struct nn_ep_ops ops;
};

int nn_ep_init(struct nn_ep *self, int src, struct nn_sock *sock, int eid,
    const struct nn_transport *transport, int bind, const char *addr);
void nn_ep_term(struct nn_ep *self);

void nn_ep_start(struct nn_ep *self);
void nn_ep_stop(struct nn_ep *self);

void nn_ep_stopped(struct nn_ep *self);

struct nn_ctx *nn_ep_getctx(struct nn_ep *self);
const char *nn_ep_getaddr(struct nn_ep *self);
void nn_ep_getopt(
    struct nn_ep *self, int level, int option, void *optval, size_t *optvallen);
int nn_ep_ispeer(struct nn_ep *self, int socktype);
void nn_ep_set_error(struct nn_ep *self, int errnum);
void nn_ep_clear_error(struct nn_ep *self);
void nn_ep_stat_increment(struct nn_ep *self, int name, int increment);

#endif // !NN_EP_INCLUDED
