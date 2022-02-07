#include "msg.h"

#include <string.h>

void nn_msg_init(struct nn_msg *self, size_t size)
{
    nn_chunkref_init(&self->sphdr, 0);
    nn_chunkref_init(&self->hdrs, 0);
    nn_chunkref_init(&self->body, size);
    return;
}

void nn_msg_init_chunk(struct nn_msg *self, void *chunk)
{
    nn_chunkref_init(&self->sphdr, 0);
    nn_chunkref_init(&self->hdrs, 0);
    nn_chunkref_init_chunk(&self->body, chunk);
    return;
}

void nn_msg_term(struct nn_msg *self)
{
    nn_chunkref_term(&self->sphdr);
    nn_chunkref_term(&self->hdrs);
    nn_chunkref_term(&self->body);
    return;
}

void nn_msg_mv(struct nn_msg *dst, struct nn_msg *src)
{
    nn_chunkref_mv(&dst->sphdr, &src->sphdr);
    nn_chunkref_mv(&dst->hdrs, &src->hdrs);
    nn_chunkref_mv(&dst->body, &src->body);
    return;
}

void nn_msg_cp(struct nn_msg *dst, struct nn_msg *src)
{
    nn_chunkref_cp(&dst->sphdr, &src->sphdr);
    nn_chunkref_cp(&dst->hdrs, &src->hdrs);
    nn_chunkref_cp(&dst->body, &src->body);
    return;
}

void nn_msg_bulkcopy_start(struct nn_msg *self, uint32_t copies)
{
    nn_chunkref_bulkcopy_start(&self->sphdr, copies);
    nn_chunkref_bulkcopy_start(&self->hdrs, copies);
    nn_chunkref_bulkcopy_start(&self->body, copies);
    return;
}

void nn_msg_bulkcopy_cp(struct nn_msg *dst, struct nn_msg *src)
{
    nn_chunkref_bulkcopy_cp(&dst->sphdr, &src->sphdr);
    nn_chunkref_bulkcopy_cp(&dst->hdrs, &src->hdrs);
    nn_chunkref_bulkcopy_cp(&dst->body, &src->body);
    return;
}

void nn_msg_replace_body(struct nn_msg *self, struct nn_chunkref new_body)
{
    nn_chunkref_term(&self->body);
    self->body = new_body;
    return;
}
