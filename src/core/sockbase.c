#include "../protocol.h"

#include "sock.h"

#include "../utils/err.h"
#include "../utils/attr.h"

void nn_sockbase_init(
    struct nn_sockbase *self, const struct nn_sockbase_vfptr *vfptr, void *hint)
{
    self->vfptr = vfptr;
    self->sock = (struct nn_sock *)hint;
}

void nn_sockbase_term(NN_UNUSED struct nn_sockbase *self)
{
    /*  Do nothing by default. */
}

void nn_sockbase_stopped(struct nn_sockbase *self)
{
    nn_sock_stopped(self->sock);
}

struct nn_ctx *nn_sockbase_getctx(struct nn_sockbase *self)
{
    return nn_sock_getctx(self->sock);
}

int nn_sockbase_getopt(
    struct nn_sockbase *self, int option, void *optval, size_t *optvallen)
{
    return nn_sock_getopt_inner(
        self->sock, NN_SOL_SOCKET, option, optval, optvallen);
}

void nn_sockbase_stat_increment(
    struct nn_sockbase *self, int name, int increment)
{
    nn_sock_stat_increment(self->sock, name, increment);
}
