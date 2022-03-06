#include "msgqueue.h"

#include "../../utils/alloc.h"
#include "../../utils/fast.h"
#include "../../utils/err.h"

#include <string.h>

void nn_msgqueue_init(struct nn_msgqueue *self, size_t maxmem)
{
    struct nn_msgqueue_chunk *chunk;

    self->count = 0;
    self->mem = 0;
    self->maxmem = maxmem;

    chunk = nn_alloc(sizeof(struct nn_msgqueue_chunk), "msgqueue chunk");
    alloc_assert(chunk);
    chunk->next = NULL;

    self->out.chunk = chunk;
    self->out.pos = 0;
    self->in.chunk = chunk;
    self->in.pos = 0;

    self->cache = NULL;
}

void nn_msgqueue_term(struct nn_msgqueue *self)
{
    int rc;
    struct nn_msg msg;

    while (1) {
        rc = nn_msgqueue_recv(self, &msg);
        if (rc == -EAGAIN) {
            break;
        }
        errnum_assert(rc >= 0, -rc);
        nn_msg_term(&msg);
    }

    nn_assert(self->in.chunk == self->out.chunk);
    nn_free(self->in.chunk);

    if (self->cache) {
        nn_free(self->cache);
    }
}

int nn_msgqueue_empty(struct nn_msgqueue *self)
{
    return self->count == 0 ? 1 : 0;
}

int nn_msgqueue_send(struct nn_msgqueue *self, struct nn_msg *msg)
{
    size_t msgsz;

    msgsz = nn_chunkref_size(&msg->sphdr) + nn_chunkref_size(&msg->body);
    if (nn_slow(self->count > 0 && self->mem + msgsz >= self->maxmem)) {
        return -EAGAIN;
    }

    ++self->count;
    self->mem += msgsz;

    nn_msg_mv(&self->out.chunk->msgs[self->out.pos], msg);
    ++self->out.pos;

    if (nn_slow(self->out.pos == NN_MSGQUEUE_GRANULARITY)) {
        if (nn_slow(!self->cache)) {
            self->cache
                = nn_alloc(sizeof(struct nn_msgqueue_chunk), "msgqueue chunk");
        }
        self->out.chunk->next = self->cache;
        self->out.chunk = self->cache;
        self->cache = NULL;
        self->out.pos = 0;
    }

    return 0;
}

int nn_msgqueue_recv(struct nn_msgqueue *self, struct nn_msg *msg)
{
    struct nn_msgqueue_chunk *o;

    if (nn_slow(!self->count)) {
        return -EAGAIN;
    }

    nn_msg_mv(msg, &self->in.chunk->msgs[self->in.pos]);

    ++self->in.pos;
    if (nn_slow(self->in.pos == NN_MSGQUEUE_GRANULARITY)) {
        o = self->in.chunk;
        self->in.chunk = self->in.chunk->next;
        self->in.pos = 0;
        if (nn_fast(!self->cache)) {
            self->cache = o;
        } else {
            nn_free(o);
        }
    }

    --self->count;
    self->mem -= (nn_chunkref_size(&msg->sphdr) + nn_chunkref_size(&msg->body));

    return 0;
}
