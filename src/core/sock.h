#ifndef NN_SOCK_INCLUDED
#define NN_SOCK_INCLUDED

#include "../protocol.h"
#include "../transport.h"

#include "../aio/ctx.h"
#include "../aio/fsm.h"

#include "../utils/efd.h"
#include "../utils/sem.h"
#include "../utils/list.h"

struct nn_pipe;

#define NN_MAX_TRANSPORT (4)

struct nn_sock {
    struct nn_fsm fsm;
    int state;

    struct nn_sockbase *sockbase;

    const struct nn_socktype *socktype;

    int flags;

    struct nn_ctx ctx;
    struct nn_efd sndfd;
    struct nn_efd rcvfd;
    struct nn_sem termsem;
    struct nn_sem relesem;

    struct nn_list eps;

    struct nn_list sdeps;

    int eid;

    int holds;

    int sndbuf;
    int rcvbuf;
    int rcvmaxsize;
    int sndtimeo;
    int rcvtimeo;
    int reconnect_ivl;
    int reconnect_ivl_max;
    int maxttl;

    struct nn_ep_options ep_template;

    struct nn_optset *optsets[NN_MAX_TRANSPORT];

    struct {
        uint64_t established_connections;
        uint64_t accepted_connections;
        uint64_t dropped_connections;
        uint64_t broken_connections;
        uint64_t connect_errors;
        uint64_t bind_errors;
        uint64_t accept_errors;

        uint64_t message_sent;
        uint64_t message_received;
        uint64_t bytes_sent;
        uint64_t bytes_received;

        int current_connections;
        int inprogress_connections;
        int current_snd_priority;
        int current_ep_errors;
    } statistics;

    char socket_name[64];

    void *sec_attr;
    size_t sec_attr_size;
    int outbuffersz;
    int inbuffersz;
};

int nn_sock_init(
    struct nn_sock *self, const struct nn_socktype *socktype, int fd);

void nn_sock_stop(struct nn_sock *self);

int nn_sock_term(struct nn_sock *self);

void nn_sock_stopped(struct nn_sock *self);

struct nn_ctx *nn_sock_getctx(struct nn_sock *self);

int nn_sock_ispeer(struct nn_sock *self, int socktype);

int nn_sock_add_ep(struct nn_sock *self, const struct nn_transport *transport,
    int bind, const char *addr);

int nn_sock_rm_ep(struct nn_sock *self, int eid);

int nn_sock_send(struct nn_sock *self, struct nn_msg *msg, int flags);

int nn_sock_recv(struct nn_sock *self, struct nn_msg *msg, int flags);

int nn_sock_setopt(struct nn_sock *self, int level, int option,
    const void *optval, size_t optvallen);

int nn_sock_getopt(struct nn_sock *self, int level, int option, void *optval,
    size_t *optvallen);

int nn_sock_getopt_inner(struct nn_sock *self, int level, int option,
    void *optval, size_t *optvallen);

int nn_sock_add(struct nn_sock *self, struct nn_pipe *pipe);

void nn_sock_rm(struct nn_sock *self, struct nn_pipe *pipe);

void nn_sock_report_error(struct nn_sock *self, struct nn_ep *ep, int errnum);
void nn_sock_stat_increment(struct nn_sock *self, int name, int64_t increment);

int nn_sock_hold(struct nn_sock *self);
void nn_sock_rele(struct nn_sock *self);

#endif // !NN_SOCK_INCLUDED
