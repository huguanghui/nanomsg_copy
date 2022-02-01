#ifndef NN_ALLOC_INCLUDED
#define NN_ALLOC_INCLUDED

#include <stddef.h>

void nn_alloc_init(void);
void nn_alloc_term(void);
void *nn_realloc(void *ptr, size_t size);
void nn_free(void *ptr);

#if defined(NN_ALLOC_MONITOR)
#define nn_alloc(size, name) nn_alloc_(size, name)
void *nn_alloc_(size_t size, const char *name);
#else
#define nn_alloc(size, name) nn_alloc_(size)
void *nn_alloc_(size_t size);
#endif

#endif // !NN_ALLOC_INCLUDED
