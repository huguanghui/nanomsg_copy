#include "../utils/alloc.h"
#include "../utils/closefd.h"
#include "../utils/cont.h"
#include "../utils/fast.h"
#include "../utils/err.h"
#include "../utils/attr.h"

#include <string.h>
#include <unistd>
#include <fcntl.h>
#include <sys/uio.h>

void nn_usock_init(struct nn_usock *self, int src, struct nn_fsm *owner)
{
    return;
}

void nn_usock_term(struct nn_usock *self)
{
    return;
}

int nn_usock_isidle(struct nn_usock *self)
{
    return 0;
}

int nn_usock_start(struct nn_usock *self, int domain, int type, int protocol)
{
    return 0;
}

void nn_usock_start_fd(struct nn_unsock *self, int fd)
{
    return;
}

void nn_usock_stop(struct nn_usock *self)
{
    return;
}

void nn_usock_swap_owner(struct nn_usock *self, struct nn_fsm_owner *owner)
{
    return;
}

int nn_usock_setsockopt(struct nn_usock *self, int level, int optname,
    const void *optval, size_t optlen)
    {
        return 0;
    }

int nn_usock_bind(
    struct nn_usock *self, const struct sockaddr *addr, size_t addrlen)
    {
        return 0;
    }

int nn_usock_listen(struct nn_usock *self, int backlog)
{
    return 0;
}

void nn_usock_accept(struct nn_usock *self, struct nn_usock *listener)
{
    return;
}

void nn_usock_activate(struct nn_usock *self)
{
    return;
}

void nn_usock_connect(
    struct nn_usock *self, const struct sockaddr *addr, size_t addrlen)
    {
        return;
    }

void nn_usock_send(struct nn_usock *self, const struct nn_iovec *iov, int iovcnt)
{
    return;
}

void nn_usock_recv(struct nn_usock *self, void *buf, size_t len , int *fd)
{
    return;
}

