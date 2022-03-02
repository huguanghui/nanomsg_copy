#ifndef NN_TRANSPORT_INCLUDED
#define NN_TRANSPORT_INCLUDED

#include "nn.h"

#include "aio/fsm.h"

#include "utils/list.h"
#include "utils/msg.h"

#include <stddef.h>

struct nn_sock;

struct nn_optset;

struct nn_optset_vfptr {
    void (*destroy)(struct nn_optset *self);
    int (*setopt)(struct nn_optset *self, int option, const void *optval,
        size_t optvallen);
    int (*getopt)(
        struct nn_optset *self, int option, void *optval, size_t *optvallen);
};

struct nn_optset {
    const struct nn_optset_vfptr *vfptr;
};

struct nn_ep;

struct nn_ep_ops {
    void (*stop)(void *);
    void (*destroy)(void *);
};

void nn_ep_tran_setup(struct nn_ep *, const struct nn_ep_ops *, void *);

void nn_ep_stopped(struct nn_ep *);

struct nn_ctx *nn_ep_getctx(struct nn_ep *);

const char *nn_ep_getaddr(struct nn_ep *self);

void nn_ep_getopt(
    struct nn_ep *, int level, int option, void *optval, size_t *optvallen);

int nn_ep_ispeer(struct nn_ep *, int socktype);

int nn_ep_ispeer_ep(struct nn_ep *, struct nn_ep *);

void nn_ep_set_error(struct nn_ep *, int errnum);

void nn_ep_clear_error(struct nn_ep *);

void nn_ep_stat_increment(struct nn_ep *, int name, int increment);

struct nn_pipebase;

#define NN_PIPEBASE_RELEASE (1)

#define NN_PIPEBASE_PARSED (2)

struct nn_pipebase_vfptr {
    int (*send)(struct nn_pipebase *self, struct nn_msg *msg);
    int (*recv)(struct nn_pipebase *self, struct nn_msg *msg);
};

struct nn_ep_options {
    int sndprio;
    int rcvprio;
    int ipv4only;
};

struct nn_pipebase {
    struct nn_fsm fsm;
    const struct nn_pipebase_vfptr *vfptr;
    uint8_t state;
    uint8_t instate;
    uint8_t outstate;
    struct nn_sock *sock;
    void *data;
    struct nn_fsm_event in;
    struct nn_fsm_event out;
    struct nn_ep_options options;
};

void nn_pipebase_init(struct nn_pipebase *self,
    const struct nn_pipebase_vfptr *vfptr, struct nn_ep *ep);

void nn_pipebase_term(struct nn_pipebase *self);

int nn_pipebase_start(struct nn_pipebase *self);

void nn_pipebase_stop(struct nn_pipebase *self);

void nn_pipebase_received(struct nn_pipebase *self);

void nn_pipebase_sent(struct nn_pipebase *self);

void nn_pipebase_getopt(struct nn_pipebase *self, int level, int option,
    void *optval, size_t *optvallen);

int nn_pipebase_ispeer(struct nn_pipebase *self, int socktype);

struct nn_transport {
    const char *name;
    int id;
    void (*init)(void);
    void (*term)(void);

    int (*bind)(struct nn_ep *);
    int (*connect)(struct nn_ep *);

    struct nn_optset *(*optset)(void);
};

#endif // !NN_TRANSPORT_INCLUDED
