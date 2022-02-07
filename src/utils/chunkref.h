#ifndef NN_CHUNKREF_INCLUDED
#define NN_CHUNKREF_INCLUDED

#define NN_CHUNKREF_MAX 32

#include "chunk.h"

#include <stddef.h>
#include <stdint.h>

struct nn_chunkref {
    union {
        uint8_t ref[NN_CHUNKREF_MAX];
        void *unused;
    } u;
};

void nn_chunkref_init(struct nn_chunkref *self, size_t size);

void nn_chunkref_init_chunk(struct nn_chunkref *self, void *chunk);

void nn_chunkref_term(struct nn_chunkref *self);

void *nn_chunkref_getchunk(struct nn_chunkref *self);

void nn_chunkref_mv(struct nn_chunkref *dst, struct nn_chunkref *src);

void nn_chunkref_cp(struct nn_chunkref *dst, struct nn_chunkref *src);

void *nn_chunkref_data(struct nn_chunkref *self);

void nn_chunkref_trim(struct nn_chunkref *self, size_t n);

void nn_chunkref_bulkcopy_start(struct nn_chunkref *self, uint32_t copies);

void nn_chunkref_bulkcopy_cp(struct nn_chunkref *dst, struct nn_chunkref *src);

#endif // !NN_CHUNKREF_INCLUDED
