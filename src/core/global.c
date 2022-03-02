#include "../nn.h"
#include "../transport.h"
#include "../protocol.h"

#include "global.h"
#include "sock.h"
#include "ep.h"

#include "../aio/pool.h"
#include "../aio/timer.h"

#include "../utils/err.h"
#include "../utils/alloc.h"
#include "../utils/mutex.h"
#include "../utils/condvar.h"
#include "../utils/once.h"
#include "../utils/list.h"
#include "../utils/cont.h"
#include "../utils/random.h"
#include "../utils/chunk.h"
#include "../utils/msg.h"
#include "../utils/attr.h"

#include "../pubsub.h"
#include "../pipeline.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(NN_HAVE_WINDOWS)
#include "../utils/win.h"
#else
#include <unistd.h>
#endif

#ifndef NN_MAX_SOCKETS
#define NN_MAX_SOCKETS (512)
#endif /* !NN_MAX_SOCKETS */

struct nn_global {
    struct nn_sock **socks;
    uint16_t *unused;
    size_t nsocks;
    int flags;
    struct nn_pool pool;
    int state;
    int print_errors;
    int inited;
    nn_mutex_t lock;
    nn_condvar_t cond;
};

static struct nn_global self;
// static nn_once_t once = NN_ONCE_INITIALIZER

static void nn_global_init(void);
static void nn_global_term(void);

static int nn_global_create_ep(struct nn_sock *, const char *addr, int bind);

static int nn_global_create_socket(int domain, int protocol);

static int nn_global_hold_socket(struct nn_sock **sockp, int s);
static int nn_global_hold_socket_locked(struct nn_sock **sockp, int s);
static void nn_global_rele_socket(struct nn_sock *);

int nn_errno(void) { return nn_err_errno(); }

const char *nn_strerror(int errnum) { return nn_err_strerror(errnum); }

static void nn_global_init(void) { return; }

static void nn_global_term(void) { return; }

void nn_term(void) {}

static void nn_lib_init(void) {}

void nn_init(void) {}

void *nn_allocmsg(size_t size, int type) { return NULL; }

void *nn_reallocmsg(void *msg, size_t size) { return NULL; }

int nn_freemsg(void *msg) { return 0; }

struct nn_cmsghdr *nn_cmsg_nxthdr(
    struct nn_msghdr *mhdr, const struct nn_cmsghdr *cmsg)
{
    return NULL;
}

int nn_global_create_socket(int domain, int protocol) { return 0; }

int nn_socket(int domain, int protocol) { return 0; }

int nn_close(int s) { return 0; }

int nn_setsockopt(
    int s, int level, int option, const void *optval, size_t optvallen)
{
    return 0;
}

int nn_getsockopt(int s, int level, int option, void *optval, size_t *optvallen)
{
    return 0;
}

int nn_bind(int s, const char *addr) { return 0; }

int nn_connect(int s, const char *addr) { return 0; }

int nn_shutdown(int s, int how) { return 0; }

int nn_send(int s, const void *buf, size_t len, int flags) { return 0; }

int nn_recv(int s, void *buf, size_t len, int flags) { return 0; }

int nn_sendmsg(int s, const struct nn_msghdr *msg, int flags) { return 0; }

int nn_recvmsg(int s, struct nn_msghdr *msg, int flags) { return 0; }

uint64_t nn_get_statistic(int s, int statistic) { return 0; }

static int nn_global_create_ep(struct nn_sock *sock, const char *addr, int bind)
{
    return 0;
}

const struct nn_transport *nn_global_transport(int id) { return NULL; }

struct nn_pool *nn_global_getpool() { return NULL; }

int nn_global_print_errors() { return 0; }

int nn_global_hold_socket_locked(struct nn_sock **sockp, int s) { return 0; }

int nn_global_hold_socket(struct nn_sock **sockp, int s) { return 0; }

void nn_global_rele_socket(struct nn_sock *sock) {}
