#ifndef NN_XPULL_INCLUDED
#define NN_XPULL_INCLUDED

#include "../../protocol.h"

int nn_xpull_create(void *hint, struct nn_sockbase **sockbase);

int nn_xpull_ispeer(int socktype);

#endif // !NN_XPULL_INCLUDED
