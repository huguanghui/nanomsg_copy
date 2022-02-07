#include "once.h"

#if NN_HAVE_WINDOWS

BOOL CALLBACK nn_do_once_cb(
    PINT_ONCE InitOnce, PVOID Parameter, PVOID *lpContext)
{
    void (*func)(void) = Parameter;
    func();
    return (TRUE);
}

void nn_do_once(nn_once_t *once, void (*func)(void))
{
    (void)InitOnceExecuteOnce(&once->once, nn_do_once_cb, func, NULL);
    return;
}
#else

void nn_do_once(nn_once_t *once, void (*func)(void))
{
    pthread_once(&once->once, func);
    return;
}
#endif /* NN_HAVE_WINDOWS */
