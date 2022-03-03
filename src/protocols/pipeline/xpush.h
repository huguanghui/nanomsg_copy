#ifndef NN_XPUSH_INCLUDED
#define NN_XPUSH_INCLUDED

#include "../../protocol.h"

int nn_xpush_create(void *hint, struct nn_sockbase **sockbase);

int nn_xpush_ispeer(int socktype);

#endif // !NN_XPUSH_INCLUDED
