#include "../protocol.h"
#include "../transport.h"

#include "sock.h"
#include "global.h"
#include "ep.h"

#include "../utils/err.h"
#include "../utils/cont.h"
#include "../utils/clock.h"
#include "../utils/fast.h"
#include "../utils/alloc.h"
#include "../utils/msg.h"

#include <limits.h>

#define NN_SOCK_FLAG_IN (1)
#define NN_SOCK_FLAG_OUT (2)

#define NN_SOCK_STATE_INIT (1)
#define NN_SOCK_STATE_ACTIVE (2)
#define NN_SOCK_STATE_STOPPING_EPS (3)
#define NN_SOCK_STATE_STOPPING (4)
#define NN_SOCK_STATE_FINI (5)

#define NN_SOCK_ACTION_STOPPED (1)

#define NN_SOCK_SRC_EP (1)

static struct nn_optset *nn_sock_optset(struct nn_sock *self, int id);
static int nn_sock_setopt_inner(struct nn_sock *self, int level, int option,
    const void *optval, size_t optvallen);
static void nn_sock_onleave(struct nn_ctx *self);
static void nn_sock_handler(
    struct nn_fsm *self, int src, int type, void *srcptr);
static void nn_sock_shutdown(
    struct nn_fsm *self, int src, int type, void *srcptr);

int nn_sock_init(
    struct nn_sock *self, const struct nn_socktype *socktype, int fd)
{
    int rc;
    int i;

    nn_assert(!(socktype->flags & NN_SOCKTYPE_FLAG_NOSEND)
        || !(socktype->flags & NN_SOCKTYPE_FLAG_NORECV));

    nn_ctx_init(&self->ctx, nn_global_getpool(), nn_sock_onleave);

    nn_fsm_init_root(&self->fsm, nn_sock_handler, nn_sock_shutdown, &self->ctx);
    self->state = NN_SOCK_STATE_INIT;

    if (socktype->flags & NN_SOCKTYPE_FLAG_NOSEND) {
        memset(&self->sndfd, 0xcd, sizeof(self->sndfd));
    } else {
        rc = nn_efd_init(&self->sndfd);
        if (nn_slow(rc < 0)) {
            return rc;
        }
    }
    if (socktype->flags & NN_SOCKTYPE_FLAG_NORECV) {
        memset(&self->rcvfd, 0xcd, sizeof(self->rcvfd));
    } else {
        rc = nn_efd_init(&self->rcvfd);
        if (nn_slow(rc < 0)) {
            if (!(socktype->flags & NN_SOCKTYPE_FLAG_NOSEND)) {
                nn_efd_term(&self->sndfd);
            }
            return rc;
        }
    }
    nn_sem_init(&self->termsem);
    nn_sem_init(&self->relesem);
    if (nn_slow(rc < 0)) {
        if (!(socktype->flags & NN_SOCKTYPE_FLAG_NORECV)) {
            nn_efd_term(&self->rcvfd);
        }
        if (!(socktype->flags & NN_SOCKTYPE_FLAG_NOSEND)) {
            nn_efd_term(&self->sndfd);
        }
        return rc;
    }

    self->holds = 1;
    self->flags = 0;
    nn_list_init(&self->eps);
    nn_list_init(&self->sdeps);
    self->eid = 1;

    self->sndbuf = 128 * 1024;
    self->rcvbuf = 128 * 1024;
    self->rcvmaxsize = 1024 * 1024;
    self->sndtimeo = -1;
    self->rcvtimeo = -1;
    self->reconnect_ivl = 100;
    self->reconnect_ivl_max = 0;
    self->maxttl = 8;
    self->ep_template.sndprio = 8;
    self->ep_template.rcvprio = 8;
    self->ep_template.ipv4only = 1;

    memset(&self->statistics, 0, sizeof(self->statistics));

    sprintf(self->socket_name, "%d", fd);

    self->sec_attr = NULL;
    self->sec_attr_size = 0;
    self->inbuffersz = 4096;
    self->outbuffersz = 4096;

    for (i = 0; i != NN_MAX_TRANSPORT; ++i) {
        self->optsets[i] = NULL;
    }

    rc = socktype->create((void *)self, &self->sockbase);
    errnum_assert(rc == 0, -rc);
    self->socktype = socktype;

    nn_ctx_enter(&self->ctx);
    nn_fsm_start(&self->fsm);
    nn_ctx_leave(&self->ctx);

    return 0;
}

void nn_sock_stopped(struct nn_sock *self)
{
    self->fsm.stopped.fsm = &self->fsm;
    self->fsm.stopped.src == NN_FSM_ACTION;
    self->fsm.stopped.srcptr = NULL;
    self->fsm.stopped.type = NN_SOCK_ACTION_STOPPED;
    nn_ctx_raise(self->fsm.ctx, &self->fsm.stopped);
}

void nn_sock_stop(struct nn_sock *self)
{
    nn_ctx_enter(&self->ctx);
    nn_fsm_stop(&self->fsm);
    nn_ctx_leave(&self->ctx);
}

int nn_sock_term(struct nn_sock *self)
{
    int rc;
    int i;

    for (;;) {
        rc = nn_sem_wait(&self->termsem);
        if (nn_slow(rc == -EINTR)) {
            continue;
        }
        errnum_assert(rc == 0, -rc);
        break;
    }

    for (;;) {
        rc = nn_sem_wait(&self->relesem);
        if (nn_slow(rc == -EINTR)) {
            continue;
        }
        errnum_assert(rc == 0, -rc);
        break;
    }

    nn_ctx_enter(&self->ctx);
    nn_ctx_leave(&self->ctx);

    if (!(self->socktype->flags & NN_SOCKTYPE_FLAG_NORECV)) {
        nn_efd_term(&self->rcvfd);
    }
    if (!(self->socktype->flags & NN_SOCKTYPE_FLAG_NOSEND)) {
        nn_efd_term(&self->sndfd);
    }

    nn_fsm_stopped_noevent(&self->fsm);
    nn_fsm_term(&self->fsm);
    nn_sem_term(&self->termsem);
    nn_sem_term(&self->relesem);
    nn_list_term(&self->sdeps);
    nn_list_term(&self->eps);
    nn_ctx_term(&self->ctx);

    for (i = 0; i != NN_MAX_TRANSPORT; ++i) {
        if (self->optsets[i]) {
            self->optsets[i]->vfptr->destroy(self->optsets[i]);
        }
    }

    return 0;
}

struct nn_ctx *nn_sock_getctx(struct nn_sock *self) { return &self->ctx; }

int nn_sock_ispeer(struct nn_sock *self, int socktype)
{
    if ((self->socktype->protocol & 0xfff0) != (socktype & 0xfff0)) {
        return 0;
    }

    return self->socktype->ispeer(socktype);
}

int nn_sock_setopt(struct nn_sock *self, int level, int option,
    const void *optval, size_t optvallen)
{
    int rc;

    nn_ctx_enter(&self->ctx);
    rc = nn_sock_setopt_inner(self, level, option, optval, optvallen);
    nn_ctx_leave(&self->ctx);

    return rc;
}

static int nn_sock_setopt_inner(struct nn_sock *self, int level, int option,
    const void *optval, size_t optvallen)
{
    struct nn_optset *optset;
    int val;

    if (level > NN_SOL_SOCKET) {
        if (self->sockase->vfptr->setopt == NULL) {
            return -ENOPROTOOPT;
        }
        return self->sockbase->vfptr->setopt(
            self->sockbase, level, option, optval, optvallen);
    }

    if (level < NN_SOL_SOCKET) {
        optset = nn_sock_optset(self, level);
        if (!optset) {
            return -ENOPROTOOPT;
        }
        return optset->vfptr->setopt(optset, option, optval, optvallen);
    }

    nn_assert(level == NN_SOL_SOCKET);

    if (option == NN_SOCKET_NAME) {
        if (optvallen > 63) {
            return -EINVAL;
        }
        memcpy(self->socket_name, optval, optvallen);
        self->socket_name[optvallen] = 0;
        return 0;
    }

    if (optvallen != sizeof(int)) {
        return -EINVAL;
    }
    val = *(int *)optval;

    switch (option) {
    case NN_SNDBUF:
        if (val <= 0) {
            return -EINVAL;
        }
        self->sndbuf = val;
        return 0;
    case NN_RCVBUF:
        if (val <= 0) {
            return -EINVAL;
        }
        self->rcvbuf = val;
        return 0;
    case NN_RCVMAXSIZE:
        if (val <= 0) {
            return -EINVAL;
        }
        self->rcvmaxsize = val;
        return 0;
    case NN_SNDTIMEO:
        self->sndtimeo = val;
        return 0;
    case NN_RCVTIMEO:
        self->rcvtimeo = val;
        return 0;
    case NN_RECONNECT_IVL:
        if (val < 0) {
            return -EINVAL;
        }
        self->reconnect_ivl = val;
        return 0;
    case NN_RECONNECT_IVL_MAX:
        if (val < 0) {
            return -EINVAL;
        }
        self->reconnect_ivl_max = val;
        return 0;
    case NN_SNDPRIO:
        if (val < 1 || val > 16) {
            return -EINVAL;
        }
        self->ep_template.sndprio = val;
        return 0;
    case NN_RCVPRIO:
        if (val < 1 || val > 16) {
            return -EINVAL;
        }
        self->ep_template.rcvprio = val;
        return 0;
    case NN_IPV4ONLY:
        if (val != 0 && val != 1) {
            return -EINVAL;
        }
        self->ep_template.ipv4only = val;
        return 0;
    case NN_MAXTTL:
        if (val < 1 || val > 255) {
            return -EINVAL;
        }
        self->maxttl = val;
    case NN_LINGER:
        return 0;
    }

    return -ENOPROTOOPT;
}

int nn_sock_getopt(struct nn_sock *self, int level, int option, void *optval,
    size_t *optvallen)
{
    int rc;

    nn_ctx_enter(&self->ctx);
    rc = nn_sock_getopt_inner(self, level, option, optval, optvallen);
    nn_ctx_leave(&self->ctx);

    return rc;
}

int nn_sock_getopt_inner(struct nn_sock *self, int level, int option,
    void *optval, size_t *optvallen)
{
    struct nn_optset *optset;
    int intval;
    nn_fd fd;

    if (level > NN_SOL_SOCKET) {
        if (self->sockbase->vfptr->getopt == NULL) {
            return -ENOPROTOOPT;
        }
        return self->sockbase->vfptr->getopt(
            self->sockbase, level, option, optval, optvallen);
    }

    if (level < NN_SOL_SOCKET) {
        optset = nn_sock_optset(self, level);
        if (!optset) {
            return -ENOPROTOOPT;
        }
        return optset->vfptr->getopt(optset, option, optval, optvallen);
    }

    nn_assert(level == NN_SOL_SOCKET);

    switch (option) {
    case NN_DOMAIN:
        intval = self->socktype->domain;
        break;
    case NN_PROTOCOL:
        intval = self->socktype->protocol;
        break;
    case NN_LINGER:
        intval = 0;
        break;
    case NN_SNDBUF:
        intval = self->sndbuf;
        break;
    case NN_RCVBUF:
        intval = self->rcvbuf;
        break;
    case NN_RCVMAXSIZE:
        intval = self->rcvmaxsize;
        break;
    case NN_SNDTIMEO:
        intval = self->sndtimeo;
        break;
    case NN_RCVTIMEO:
        intval = self->rcvtimeo;
        break;
    case NN_RECONNECT_IVL:
        intval = self->reconnect_ivl;
        break;
    case NN_RECONNECT_IVL_MAX:
        intval = self->reconnect_ivl_max;
        break;
    case NN_SNDPRIO:
        intval = self->ep_template.sndprio;
        break;
    case NN_RCVPRIO:
        intval = self->ep_template.rcvprio;
        break;
    case NN_IPV4ONLY:
        intval = self->ep_template.ipv4only;
        break;
    case NN_MAXTTL:
        intval = self->maxttl;
        break;
    case NN_SNDFD:
        if (self->socktype->flags & NN_SOCKTYPE_FLAG_NOSEND) {
            return -ENOPROTOOPT;
        }
        fd = nn_efd_getfd(&self->sndfd);
        memcpy(optval, &fd,
            *optvallen < sizeof(nn_fd) ? *optvallen : sizeof(nn_fd));
        *optvallen = sizeof(nn_fd);
        return 0;
    case NN_RCVFD:
        if (self->socktype->flags & NN_SOCKTYPE_FLAG_NORECV) {
            return -ENOPROTOOPT;
        }
        fd = nn_efd_getfd(&self->rcvfd);
        memcpy(optval, &fd,
            *optvallen < sizeof(nn_fd) ? *optvallen : sizeof(nn_fd));
        *optvallen = sizeof(nn_fd);
        return 0;
    case NN_SOCKET_NAME:
        strncpy(optval, self->socket_name, *optvallen);
        *optvallen = strlen(self->socket_name);
        return 0;
    default:
        return -ENOPROTOOPT;
    }

    memcpy(
        optval, &intval, *optvallen < sizeof(int) ? *optvallen : sizeof(int));
    *optvallen = sizeof(int);

    return 0;
}

int nn_sock_add_ep(struct nn_sock *self, const struct nn_transport *transport,
    int bind, const char *addr)
{
    int rc;
    struct nn_ep *ep;
    int eid;

    nn_ctx_enter(&self->ctx);

    ep = nn_alloc(sizeof(struct nn_ep), "endpoint");
    if (!ep) {
        return -ENOMEM;
    }
    rc = nn_ep_init(ep, NN_SOCK_SRC_EP, self, self->eid, transport, bind, addr);
    if (nn_slow(rc < 0)) {
        nn_free(ep);
        nn_ctx_leave(&self->ctx);
        return rc;
    }
    nn_ep_start(ep);

    eid = self->eid;
    ++self->eid;

    nn_list_insert(&self->eps, &ep->item, nn_list_end(&self->eps));

    nn_ctx_leave(&self->ctx);

    return eid;
}

int nn_sock_em_ep(struct nn_sock *self, int eid)
{
    struct nn_list_item *it;
    struct nn_ep *ep;

    nn_ctx_enter(&self->ctx);
    ep = NULL;
    for (it = nn_list_begin(&self->eps); it != nn_list_end(&self->eps);
         it = nn_list_next(&self->eps, it)) {
        ep = nn_cont(it, struct nn_ep, item);
        if (ep->eid == eid) {
            break;
        }
        ep = NULL;
    }

    if (!ep) {
        nn_ctx_leave(&self->ctx);
        return -EINVAL;
    }

    nn_list_erase(&self->eps, &ep->item);
    nn_list_insert(&self->sdeps, &ep->item, nn_list_end(&self->sdeps));

    nn_ep_stop(ep);

    nn_ctx_leave(&self->ctx);
    return 0;
}

int nn_sock_send(struct nn_sock *self, struct nn_msg *msg, int flags)
{
    int rc;
    uint64_t deadline;
    uint64_t now;
    int timeout;

    if (nn_slow(self->socktype->flags & NN_SOCKTYPE_FLAG_NOSEND)) {
        return -ENOTSUP;
    }

    nn_ctx_enter(&self->ctx);

    if (self->sndtimeo < 0) {
        deadline = -1;
        timeout = -1;
    } else {
        deadline = nn_clock_ms() + self->sndtimeo;
        timeout = self->sndtimeo;
    }

    while (1) {
        switch (self->state) {
        case NN_SOCK_STATE_ACTIVE:
        case NN_SOCK_STATE_INIT:
            break;
        case NN_SOCK_STATE_STOPPING_EPS:
        case NN_SOCK_STATE_STOPPING:
        case NN_SOCK_STATE_FINI:
            nn_ctx_leave(&self->ctx);
            return -EBADF;
        }

        rc = self->sockbase->vfptr->send(self->sockbase, msg);
        if (nn_fast(rc == 0)) {
            nn_ctx_leave(&self->ctx);
            return 0;
        }
        nn_assert(rc < 0);

        if (nn_slow(rc != -EAGAIN)) {
            nn_ctx_leave(&self->ctx);
            return rc;
        }

        if (nn_fast(flags & NN_DONTWAIT)) {
            nn_ctx_leave(&self->ctx);
            return -EAGAIN;
        }

        nn_ctx_leave(&self->ctx);
        rc = nn_efd_wait(&self->sndfd, timeout);
        if (nn_slow(rc == -ETIMEDOUT)) {
            return -ETIMEDOUT;
        }
        if (nn_slow(rc == -EINTR)) {
            return -EINTR;
        }
        if (nn_slow(rc == -EBADF)) {
            return -EBADF;
        }
        errnum_assert(rc == 0, rc);
        nn_ctx_enter(&self->ctx);

        if (!nn_efd_wait(&self->sndfd, 0)) {
            self->flags |= NN_SOCK_FLAG_OUT;
        }

        if (self->sndtimeo >= 0) {
            now = nn_clock_ms();
            timeout = (int)(now > deadline ? 0 : deadline - now);
        }
    }
}

int nn_sock_recv(struct nn_sock *self, struct nn_msg *msg, int flags)
{
    int rc;
    uint64_t deadline;
    uint64_t now;
    int timeout;

    if (nn_slow(self->socktype->flags & NN_SOCKTYPE_FLAG_NORECV)) {
        return -ENOTSUP;
    }

    nn_ctx_enter(&self->ctx);

    if (self->rcvtimeo < 0) {
        deadline = -1;
        timeout = -1;
    } else {
        deadline = nn_clock_ms() + self->rcvtimeo;
        timeout = self->rcvtimeo;
    }

    while (1) {
        switch (self->state) {
        case NN_SOCK_STATE_ACTIVE:
        case NN_SOCK_STATE_INIT:
            break;

        case NN_SOCK_STATE_STOPPING_EPS:
        case NN_SOCK_STATE_STOPPING:
        case NN_SOCK_STATE_FINI:
            nn_ctx_leave(&self->ctx);
            return -EBADF;
        }

        rc = self->sockbase->vfptr->recv(self->sockbase, msg);
        if (nn_fast(rc == 0)) {
            nn_ctx_leave(&self->ctx);
            return 0;
        }
        nn_assert(rc < 0);

        if (nn_slow(rc != -EAGAIN)) {
            nn_ctx_leave(&self->ctx);
            return rc;
        }

        if (nn_fast(flags & NN_DONTWAIT)) {
            nn_ctx_leave(&self->ctx);
            return -EAGAIN;
        }

        nn_ctx_leave(&self->ctx);
        rc = nn_efd_wait(&self->rcvfd, timeout);
        if (nn_slow(rc == -ETIMEDOUT)) {
            return -ETIMEDOUT;
        }
        if (nn_slow(rc == -EINTR)) {
            return -EINTR;
        }
        if (nn_slow(rc == -EBADF)) {
            return -EBADF;
        }
        errnum_assert(rc == 0, rc);
        nn_ctx_enter(&self->ctx);

        if (!nn_efd_wait(&self->rcvfd, 0)) {
            self->flags |= NN_SOCK_FLAG_IN;
        }

        if (self->rcvtimeo >= 0) {
            now = nn_clock_ms();
            timeout = (int)(now > deadline ? 0 : deadline - now);
        }
    }
}

int nn_sock_add(struct nn_sock *self, struct nn_pipe *pipe)
{
    int rc;

    rc = self->sockbase->vfptr->add(self->sockbase, pipe);
    if (nn_slow(rc >= 0)) {
        nn_sock_stat_increment(self, NN_STAT_CURRENT_CONNECTIONS, 1);
    }

    return rc;
}

void nn_sock_rm(struct nn_sock *self, struct nn_pipe *pipe)
{
    self->sockbase->vfptr->rm(self->sockbase, pipe);
    nn_sock_stat_increment(self, NN_STAT_CURRENT_CONNECTIONS, -1);
}

static void nn_sock_onleae(struct nn_ctx *self)
{
    struct nn_sock *sock;
    int events;

    sock = nn_cont(self, struct nn_sock, ctx);

    if (nn_slow(sock->state != NN_SOCK_STATE_ACTIVE)) {
        return;
    }

    events = sock->sockbase->vfptr->events(sock->sockbase);
    errnum_assert(events >= 0, -events);

    if (!(sock->socktype->flags & NN_SOCKTYPE_FLAG_NORECV)) {
        if (events & NN_SOCKBASE_EVENT_IN) {
            if (!(sock->flags & NN_SOCK_FLAG_IN)) {
                sock->flags |= NN_SOCK_FLAG_IN;
                nn_efd_signal(&sock->rcvfd);
            }
        } else {
            if (sock->flags & NN_SOCK_FLAG_IN) {
                sock->flags &= ~NN_SOCK_FLAG_IN;
                nn_efd_unsignal(&sock->rcvfd);
            }
        }
    }

    if (!(sock->socktype->flags & NN_SOCKTYPE_FLAG_NOSEND)) {
        if (events & NN_SOCKBASE_EVENT_OUT) {
            if (!(sock->flags & NN_SOCK_FLAG_OUT)) {
                sock->flags |= NN_SOCK_FLAG_OUT;
                nn_efd_signal(&sock->sndfd);
            }
        } else {
            if (sock->flags & NN_SOCK_FLAG_OUT) {
                sock->flags &= ~NN_SOCK_FLAG_OUT;
                nn_efd_unsignal(&sock->sndfd);
            }
        }
    }
}

static struct nn_optset *nn_sock_optset(struct nn_sock *self, int id)
{
    int index;
    const struct nn_transport *tp;

    index = (-id) - 1;

    if (nn_slow(index < 0 || index >= NN_MAX_TRANSPORT)) {
        return NULL;
    }

    if (nn_fast(self->optsets[index] != NULL)) {
        return self->optsets[index];
    }

    tp = nn_global_transport(id);
    if (nn_slow(!tp)) {
        return NULL;
    }
    if (nn_slow(!tp->optset)) {
        return NULL;
    }
    self->optsets[index] = tp->optset();

    return self->optsets[index];
}

static void nn_sock_shutdown(
    struct nn_fsm *self, int src, int type, void *srcptr)
{
    struct nn_sock *sock;
    struct nn_list_item *it;
    struct nn_ep *ep;

    sock = nn_cont(self, struct nn_sock, fsm);

    if (nn_slow(src == NN_FSM_ACTION && type == NN_FSM_STOP)) {
        nn_assert(sock->state == NN_SOCK_STATE_ACTIVE);

        if (!(sock->socktype->flags & NN_SOCKTYPE_FLAG_NORECV)) {
            nn_efd_stop(&sock->rcvfd);
        }
        if (!(sock->socktype->flags & NN_SOCKTYPE_FLAG_NOSEND)) {
            nn_efd_stop(&sock->sndfd);
        }

        it = nn_list_begin(&sock->eps);
        while (it != nn_list_end(&sock->eps)) {
            ep = nn_cont(it, struct nn_ep, item);
            it = nn_list_next(&sock->eps, it);
            nn_list_erase(&sock->eps, &ep->item);
            nn_list_insert(&sock->sdeps, &ep->item, nn_list_end(&sock->sdeps));
            nn_ep_stop(ep);
        }
        sock->state = NN_SOCK_STATE_STOPPING_EPS;
        goto finish2;
    }
    if (nn_slow(sock->state == NN_SOCK_STATE_STOPPING_EPS)) {
        if (!(src == NN_SOCK_SRC_EP && type == NN_EP_STOPPED)) {
            return;
        }
        ep = (struct nn_ep *)srcptr;
        nn_list_erase(&sock->sdeps, &ep->item);
        nn_ep_term(ep);
        nn_free(ep);
    finish2:
        if (!nn_list_empty(&sock->sdeps)) {
            return;
        }
        nn_assert(nn_list_empty(&sock->eps));
        sock->state = NN_SOCK_STATE_STOPPING;
        if (!sock->sockbase->vfptr->stop) {
            goto finish1;
        }
        sock->sockbase->vfptr->stop(sock->sockbase);
        return;
    }
    if (nn_slow(sock->state == NN_SOCK_STATE_STOPPING)) {
        nn_assert(src == NN_FSM_ACTION && type == NN_SOCK_ACTION_STOPPED);
    finish1:
        sock->sockbase->vfptr->destroy(sock->sockbase);
        sock->state = NN_SOCK_STATE_FINI;

        nn_sem_post(&sock->termsem);

        return;
    }

    nn_fsm_bad_state(sock->state, src, type);
}

static void nn_sock_handler(
    struct nn_fsm *self, int src, int type, void *srcptr)
{
    struct nn_sock *sock;
    struct nn_ep *ep;

    sock = nn_cont(self, struct nn_sock, fsm);

    switch (sock->state) {
    case NN_SOCK_STATE_INIT:
        switch (src) {
        case NN_FSM_ACTION:
            switch (type) {
            case NN_FSM_START:
                sock->state = NN_SOCK_STATE_ACTIVE;
                return;
            default:
                nn_fsm_bad_action(sock->state, src, type);
            }
        default:
            nn_fsm_bad_source(sock->state, src, type);
        }
    case NN_SOCK_STATE_ACTIVE:
        switch (src) {
        case NN_FSM_ACTION:
            switch (type) {
            default:
                nn_fsm_bad_action(sock->state, src, type);
            }
        case NN_SOCK_SRC_EP:
            switch (type) {
            case NN_EP_STOPPED:
                ep = (struct nn_ep *)srcptr;
                nn_list_erase(&sock->eps, &ep->item);
                nn_ep_term(ep);
                nn_free(ep);
                return;
            default:
                nn_fsm_bad_action(sock->state, src, type);
            }
        default:
            switch (type) {
            case NN_PIPE_IN:
                sock->sockbase->vfptr->in(
                    sock->sockbase, (struct nn_pipe *)srcptr);
                return;
            case NN_PIPE_OUT:
                sock->sockbase->vfptr->out(
                    sock->sockbase, (struct nn_pipe *)srcptr);
                return;
            default:
                nn_fsm_bad_action(sock->state, src, type);
            }
        }
    default:
        nn_fsm_bad_state(sock->state, src, type);
    }
}

void nn_sock_report_error(struct nn_sock *self, struct nn_ep *ep, int errnum)
{
    if (!nn_global_print_errors()) {
        return;
    }

    if (errnum == 0) {
        return;
    }

    if (ep) {
        fprintf(stderr, "nanomsg: socket.%s[%s]: Error: %s\n",
            self->socket_name, nn_ep_getaddr(ep), nn_strerror(errnum));
    } else {
        fprintf(stderr, "nanomsg: socket.%s: Error: %s\n", self->socket_name,
            nn_strerror(errnum));
    }
}

void nn_sock_stat_increment(struct nn_sock *self, int name, int64_t increment)
{
    switch (name) {
    case NN_STAT_ESTABLISHED_CONNECTIONS:
        nn_assert(increment > 0);
        self->statistics.established_connections += increment;
        break;
    case NN_STAT_ACCEPTED_CONNECTIONS:
        nn_assert(increment > 0);
        self->statistics.accepted_connections += increment;
        break;
    case NN_STAT_DROPPED_CONNECTIONS:
        nn_assert(increment > 0);
        self->statistics.dropped_connections += increment;
        break;
    case NN_STAT_BROKEN_CONNECTIONS:
        nn_assert(increment > 0);
        self->statistics.broken_connections += increment;
        break;
    case NN_STAT_CONNECT_ERRORS:
        nn_assert(increment > 0);
        self->statistics.connect_errors += increment;
        break;
    case NN_STAT_BIND_ERRORS:
        nn_assert(increment > 0);
        self->statistics.bind_errors += increment;
        break;
    case NN_STAT_ACCEPT_ERRORS:
        nn_assert(increment > 0);
        self->statistics.accept_errors += increment;
        break;
    case NN_STAT_MESSAGES_SENT:
        nn_assert(increment > 0);
        self->statistics.messages_sent += increment;
        break;
    case NN_STAT_MESSAGES_RECEIVED:
        nn_assert(increment > 0);
        self->statistics.messages_received += increment;
        break;
    case NN_STAT_BYTES_SENT:
        nn_assert(increment > 0);
        self->statistics.bytes_sent += increment;
        break;
    case NN_STAT_BYTES_RECEIVED:
        nn_assert(increment > 0);
        self->statistics.bytes_received += increment;
        break;

    case NN_STAT_CURRENT_CONNECTIONS:
        nn_assert(increment > 0
            || self->statistics.current_connections >= -increment);
        nn_assert(increment < INT_MAX && increment > -INT_MAX);
        self->statistics.current_connections += (int)increment;
        break;
    case NN_STAT_INPROGRESS_CONNECTIONS:
        nn_assert(increment > 0
            || self->statistics.inprogress_connections >= -increment);
        nn_assert(increment < INT_MAX && increment > -INT_MAX);
        self->statistics.inprogress_connections += (int)increment;
        break;
    case NN_STAT_CURRENT_SND_PRIORITY:
        nn_assert((increment > 0 && increment <= 16) || increment == -1);
        self->statistics.current_snd_priority = (int)increment;
        break;
    case NN_STAT_CURRENT_EP_ERRORS:
        nn_assert(
            increment > 0 || self->statistics.current_ep_errors >= -increment);
        nn_assert(increment < INT_MAX && increment > -INT_MAX);
        self->statistics.current_ep_errors += (int)increment;
        break;
    }
}

int nn_sock_hold(struct nn_sock *self)
{
    switch (self->state) {
    case NN_SOCK_STATE_ACTIVE:
    case NN_SOCK_STATE_INIT:
        self->holds++;
        return 0;
    case NN_SOCK_STATE_STOPPING:
    case NN_SOCK_STATE_STOPPING_EPS:
    case NN_SOCK_STATE_FINI:
    default:
        return -EBADF;
    }
}

void nn_sock_rele(struct nn_sock *self)
{
    self->holds--;
    if (self->holds == 0) {
        nn_sem_post(&self->relesem);
    }
}
