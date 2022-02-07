#ifndef NN_CHUNK_INCLUDED
#define NN_CHUNK_INCLUDED

#include <stddef.h>
#include <stdint.h>

int nn_chunk_alloc(size_t size, int type, void **result);

int nn_chunk_realloc(size_t size, void **chunk);

void nn_chunk_free(void *p);

void nn_chunk_addref(void *p, uint32_t n);

size_t nn_chunk_size(void *p);

void *nn_chunk_trim(void *p, size_t n);

#endif // !NN_CHUNK_INCLUDED
