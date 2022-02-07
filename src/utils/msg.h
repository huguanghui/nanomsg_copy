#ifndef NN_MSG_INCLUDED
#define NN_MSG_INCLUDED

#include "chunkref.h"

#include <stddef.h>

struct nn_msg {
    struct nn_chunkref sphdr;
    struct nn_chunkref hdrs;
    struct nn_chunkref body;
};

void nn_msg_init(struct nn_msg *self, size_t size);

void nn_msg_init_chunk(struct nn_msg *self, void *chunk);

void nn_msg_term(struct nn_msg *self);

void nn_msg_mv(struct nn_msg *dst, struct nn_msg *src);

void nn_msg_cp(struct nn_msg *dst, struct nn_msg *src);

void nn_msg_bulkcopy_start(struct nn_msg *self, uint32_t copies);

void nn_msg_bulkcopy_cp(struct nn_msg *dst, struct nn_msg *src);

void nn_msg_replace_body(struct nn_msg *self, struct nn_chunkref new_body);

#endif // !NN_MSG_INCLUDED
