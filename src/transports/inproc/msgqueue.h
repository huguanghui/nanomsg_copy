#ifndef NN_MSGQUEUE_INCLUDED
#define NN_MSGQUEUE_INCLUDED

#include "../../utils/msg.h"

#include <stddef.h>

#define NN_MSGQUEUE_GRANULARITY (126)

struct nn_msgqueue_chunk {
    struct nn_msg msgs[NN_MSGQUEUE_GRANULARITY];
    struct nn_msgqueue_chunk *next;
};

struct nn_msgqueue {
    struct {
        struct nn_msgqueue_chunk *chunk;
        int pos;
    } out;
    struct {
        struct nn_msgqueue_chunk *chunk;
        int pos;
    } in;

    size_t count;
    size_t mem;
    size_t maxmem;

    struct nn_msgqueue_chunk *cache;
};

void nn_msgqueue_init (struct nn_msgqueue *self, size_t maxmem);

void nn_msgqueue_term (struct nn_msgqueue *self);

int nn_msgqueue_empty (struct nn_msgqueue *self);

int nn_msgqueue_send (struct nn_msgqueue *self, struct nn_msg *msg);

int nn_msgqueue_recv (struct nn_msgqueue *self, struct nn_msg *msg);

#endif // !NN_MSGQUEUE_INCLUDED
