#ifndef NN_PROTOCOL_INCLUDED
#define NN_PROTOCOL_INCLUDED

#include "utils/msg.h"
#include "utils/list.h"

#include <stddef.h>
#include <stdint.h>

struct nn_ctx;

#define NN_PIPE_RELEASE (1)

#define NN_PIPE_PARSED (2)

#define NN_PIPE_IN (33987)
#define NN_PIPE_OUT (33988)

struct nn_pipe;

void nn_pipe_setdata(struct nn_pipe *self, void *data);

void *nn_pipe_getdata(struct nn_pipe *self);

int nn_pipe_send(struct nn_pipe *self, struct nn_msg *msg);

int nn_pipe_recv(struct nn_pipe *self, struct nn_msg *msg);

void nn_pipe_getopt(struct nn_pipe *self, int level, int option, void *optval,
    size_t *optvallen);

struct nn_sockbase;

#define NN_SOCKBASE_EVENT_IN (1)
#define NN_SOCKBASE_EVENT_OUT (2)

struct nn_sockbase_vfptr {
    void (*stop)(struct nn_sockbase *self);
    void (*destroy)(struct nn_sockbase *self);

    int (*add)(struct nn_sockbase *self, struct nn_pipe *pipe);
    void (*rm)(struct nn_sockbase *self, struct nn_pipe *pipe);
    void (*in)(struct nn_sockbase *self, struct nn_pipe *pipe);
    void (*out)(struct nn_sockbase *self, struct nn_pipe *pipe);

    int (*events)(struct nn_sockbase *self);

    int (*send)(struct nn_sockbase *self, struct nn_msg *msg);
    int (*recv)(struct nn_sockbase *self, struct nn_msg *msg);

    int (*setopt)(struct nn_sockbase *self, int level, int option,
        const void *optval, size_t optvallen);
    int (*getopt)(struct nn_sockbase *self, int level, int option, void *optval,
        size_t *optvallen);
};

struct nn_sockbase {
    const struct nn_sockbase_vfptr *vfptr;
    struct nn_sock *sock;
};

void nn_sockbase_init(struct nn_sockbase *self,
    const struct nn_sockbase_vfptr *vfptr, void *hint);

void nn_sockbase_term(struct nn_sockbase *self);

void nn_sockbase_stopped(struct nn_sockbase *self);

struct nn_ctx *nn_sockbase_getctx(struct nn_sockbase *self);

int nn_sockbase_getopt(
    struct nn_sockbase *self, int option, void *optval, size_t *optvallen);

void nn_sockbase_stat_increment(
    struct nn_sockbase *self, int name, int increment);

#define NN_SOCKTYPE_FLAG_NORECV (1)
#define NN_SOCKTYPE_FLAG_NOSEND (2)

struct nn_socktype {
    int domain;
    int protocol;

    int flags;

    int (*create)(void *hint, struct nn_sockbase **sockbase);
    int (*ispeer)(int socktype);
};

#endif // !NN_PROTOCOL_INCLUDED
