#ifndef NN_ONCE_INCLUDED
#define NN_ONCE_INCLUDED

#ifdef NN_HAVE_WINDOWS
#include "win.h"
struct nn_once {
    INIT_ONCE once;
};
#define NN_ONCE_INITIALIZER                                                    \
    {                                                                          \
        INIT_ONCE_STATIC_INIT                                                  \
    }

#else

#include <pthread.h>

struct nn_once {
    pthread_once_t once;
};
#define NN_ONCE_INITIALIZER                                                    \
    {                                                                          \
        PTHREAD_ONCE_INIT                                                      \
    }

#endif //

typedef struct nn_once nn_once_t;
void nn_do_once(nn_once_t *once, void (*func)(void));

#endif // !NN_ONCE_INCLUDED
