#include "chunk.h"
#include "atomic.h"
#include "alloc.h"
#include "fast.h"
#include "wire.h"
#include "err.h"

#include <stdint.h>
#include <string.h>

#define NN_CHUNK_TAG 0xdeadcafe
#define NN_CHUNK_TAG_DEALLOCATED 0xbeefbeef

typedef void (*nn_chunk_free_fn)(void *p);

struct nn_chunk {
    struct nn_atomic refcount;
    size_t size;
    nn_chunk_free_fn ffn;
};

static struct nn_chunk *nn_chunk_getptr(void *p);
static void *nn_chunk_getdata(struct nn_chunk *c);
static void nn_chunk_default_free(void *p);
static size_t nn_chunk_hdrsize();

int nn_chunk_alloc(size_t size, int type, void **result)
{
    size_t sz;
    struct nn_chunk *self;
    const size_t hdrsz = nn_chunk_hdrsize();

    sz = hdrsz + size;
    if (nn_slow(sz < hdrsz))
        return -ENOMEM;

    switch (type) {
    case 0:
        self = nn_alloc(sz, "message chunk");
        break;
    default:
        return -EINVAL;
    }
    if (nn_slow(!self))
        return -ENOMEM;

    nn_atomic_init(&self->refcount, 1);
    self->size = size;
    self->ffn = nn_chunk_default_free;

    nn_putl((uint8_t *)((uint32_t *)(self + 1)), 0);
    nn_putl((uint8_t *)((((uint32_t *)(self + 1)))) + 1, NN_CHUNK_TAG);

    *result = nn_chunk_getdata(self);
    return 0;
}

int nn_chunk_realloc(size_t size, void **chunk)
{
    struct nn_chunk *self;
    void *new_ptr;
    size_t hdr_size;
    int rc;
    void *p = *chunk;

    self = nn_chunk_getptr(p);

    if (self->refcount.n == 1) {
        size_t grow;
        size_t empty;

        if (size <= self->size) {
            self->size = size;
            return 0;
        }

        hdr_size = nn_chunk_hdrsize();
        empty = (uint8_t *)p - (uint8_t *)self - hdr_size;
        grow = size - self->size;

        if (hdr_size + size < size) {
            return -ENOMEM;
        }

        if (grow <= empty) {
            new_ptr = (uint8_t *)p - grow;
            memmove(new_ptr, p, self->size);
            self->size = size;

            empty = (int8_t *)new_ptr - (int8_t *)self - hdr_size;
            nn_putl((uint8_t *)(((uint32_t *)new_ptr) - 1), NN_CHUNK_TAG);
            nn_putl((uint8_t *)((((uint32_t *)new_ptr)) - 2), (uint32_t)empty);
            *chunk = p;
            return 0;
        }
    }

    new_ptr = NULL;
    rc = nn_chunk_alloc(size, 0, &new_ptr);

    if (nn_slow(rc != 0)) {
        return rc;
    }

    memcpy(new_ptr, nn_chunk_getdata(self), self->size);
    *chunk = new_ptr;
    nn_chunk_free(p);

    return 0;
}

void nn_chunk_free(void *p)
{
    struct nn_chunk *self;

    self = nn_chunk_getptr(p);

    if (nn_atomic_dec(&self->refcount, 1) <= 1) {
        nn_putl((uint8_t *)(((uint32_t *)p) - 1), NN_CHUNK_TAG_DEALLOCATED);

        nn_atomic_term(&self->refcount);

        self->ffn(self);
    }
}

size_t nn_chunk_size(void *p) { return nn_chunk_getptr(p)->size; }

void *nn_chunk_trim(void *p, size_t n)
{
    struct nn_chunk *self;
    const size_t hdrsz = sizeof(struct nn_chunk) + 2 * sizeof(uint32_t);
    size_t empty_space;

    self = nn_chunk_getptr(p);

    nn_assert(n <= self->size);

    p = ((uint8_t *)p) + n;
    nn_putl((uint8_t *)(((uint32_t *)p) - 1), NN_CHUNK_TAG);
    empty_space = (uint8_t *)p - (uint8_t *)self - hdrsz;
    nn_assert(empty_space < UINT32_MAX);
    nn_putl((uint8_t *)(((uint32_t *)p) - 2), (uint32_t)empty_space);

    self->size -= n;

    return p;
}

static struct nn_chunk *nn_chunk_getptr(void *p)
{
    uint32_t off;

    nn_assert(nn_getl((uint8_t *)p - sizeof(uint32_t)) == NN_CHUNK_TAG);
    off = nn_getl((uint8_t *)p - 2 * sizeof(uint32_t));
    return (struct nn_chunk *)((uint8_t *)p - 2 * sizeof(uint32_t) - off
        - sizeof(struct nn_chunk));
}

static void *nn_chunk_getdata(struct nn_chunk *self)
{
    return ((uint8_t *)(self + 1)) + 2 * sizeof(uint32_t);
}

static void nn_chunk_default_free(void *p) { nn_free(p); }

static size_t nn_chunk_hdrsize()
{
    return sizeof(struct nn_chunk) + 2 * sizeof(uint32_t);
}
