#ifndef NN_ATOMIC_INCLUDED
#define NN_ATOMIC_INCLUDED

#if defined(NN_HAVE_WINDOWS)
#include "win.h"
#define NN_ATOMIC_WINAPI
#elif NN_HAVE_ATOMIC_SOLARIS
#include <atomic.h>
#define NN_ATOMIC_SOLARIS
#elif defined NN_HAVE_GCC_ATOMIC_BUILTINS
#define NN_ATOMIC_GCC_BUILTINS
#else
#include "mutex.h"
#define NN_ATOMIC_MUTEX
#endif

#include <stdint.h>

struct nn_atomic {
#if defined NN_ATOMIC_MUTEX
    struct nn_mutex sync;
#endif
    volatile uint32_t n;
};

void nn_atomic_init(struct nn_atomic *self, uint32_t n);

void nn_atomic_term(struct nn_atomic *self);

uint32_t nn_atomic_inc(struct nn_atomic *self, uint32_t n);

uint32_t nn_atomic_dec(struct nn_atomic *self, uint32_t n);

#endif // !NN_ATOMIC_INCLUDED
