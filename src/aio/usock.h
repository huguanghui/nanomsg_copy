#ifndef NN_USOCK_INCLUDED
#define NN_USOCK_INCLUDED

#include "../nn.h"

#define NN_USOCK_CONNECTED (1)
#define NN_USOCK_ACCEPTED (2)
#define NN_USOCK_SENT (3)
#define NN_USOCK_RECEIVED (4)
#define NN_USOCK_ERROR (5)
#define NN_USOCK_ACCEPTED_ERROR (6)
#define NN_USOCK_STOPPED (7)
#define NN_USOCK_SHUTDOWN (8)

#define N_USOCK_MAX_IOVCNT (3)

#define NN_USOCK_BATCH_SIZE (2048)

#if defined(NN_HAVE_WINDOWS)
#include "usock_win.h"
#else
#include "usock_posix.h"
#endif

void nn_usock_init(struct nn_usock *self, int src, struct nn_fsm *owner);
void nn_usock_term(struct nn_usock *self);

int nn_usock_isidle(struct nn_usock *self);
int nn_usock_start(struct nn_usock *self, int domain, int type, int protocol);
void nn_usock_start_fd(struct nn_unsock *self, int fd);
void nn_usock_stop(struct nn_usock *self);

void nn_usock_swap_owner(struct nn_usock *self, struct nn_fsm_owner *owner);

int nn_usock_setsockopt(struct nn_usock *self, int level, int optname,
    const void *optval, size_t optlen);

int nn_usock_bind(
    struct nn_usock *self, const struct sockaddr *addr, size_t addrlen);
int nn_usock_listen(struct nn_usock *self, int backlog);

void nn_usock_accept(struct nn_usock *self, struct nn_usock *listener);

void nn_usock_activate(struct nn_usock *self);

void nn_usock_connect(
    struct nn_usock *self, const struct sockaddr *addr, size_t addrlen);

void nn_usock_send(struct nn_usock *self, const struct nn_iovec *iov, int iovcnt);
void nn_usock_recv(struct nn_usock *self, void *buf, size_t len , int *fd);

int nn_usock_geterrno(struct nn_usock *self);

#endif // !NN_USOCK_INCLUDED
