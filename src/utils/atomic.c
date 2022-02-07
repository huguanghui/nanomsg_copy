#include "atomic.h"
#include "err.h"
#include "attr.h"

void nn_atomic_init(struct nn_atomic *self, uint32_t n)
{
    self->n = n;
#if defined NN_ATOMIC_MUTEX
    nn_mutex_init(&self->sync);
#endif
}

#if defined(NN_ATOMIC_MUTEX)
void nn_atomic_term(struct nn_atomic *self) { nn_mutex_term(&self->sync); }
#else
void nn_atomic_term(struct nn_atomic *self) { (void)self; }
#endif /*  defined */

uint32_t nn_atomic_inc(struct nn_atomic *sel, uint32_t n)
{
#if defined NN_ATOMIC_WINAPI
    return (uint32_t)InterlockedExchangeAdd((LONG *)&sel->n, (LONG)n);
#elif defined NN_ATOMIC_SOLARIS
    return atomic_add_32_nv(&sel->n, n) - n;
#elif defined NN_ATOMIC_GCC_BUILTINS
    return (uint32_t)__sync_fetch_and_add(&sel->n, n);
#elif defined NN_ATOMIC_MUTEX
    uint32_t res;
    nn_mutex_lock(&sel->sync);
    res = sel->n;
    sel->n += n;
    nn_mutex_unlock(&sel->sync);
    return res;
#else
#error
#endif /* defined NN_ATOMIC_WINAPI */
}

uint32_t nn_atomic_dec(struct nn_atomic *sel, uint32_t n)
{
#if defined NN_ATOMIC_WINAPI
    return (uint32_t)InterlockedExchangeAdd((LONG *)&sel->n, (LONG)-n);
#elif defined NN_ATOMIC_SOLARIS
    return atomic_add_32_nv(&sel->n, -n) + n;
#elif defined NN_ATOMIC_GCC_BUILTINS
    return (uint32_t)__sync_fetch_and_sub(&sel->n, n);
#elif defined NN_ATOMIC_MUTEX
    uint32_t res;
    nn_mutex_lock(&sel->sync);
    res = sel->n;
    sel->n -= n;
    nn_mutex_unlock(&sel->sync);
    return res;
#else
#error
#endif
}
