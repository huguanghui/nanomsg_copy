#ifndef NN_XPAIR_INCLUDED
#define NN_XPAIR_INCLUDED

#include "../../protocol.h"

int nn_xpair_create(void *hint, struct nn_sockbase **sockbase);

int nn_xpair_ispeer(int socktype);

#endif // !NN_XPAIR_INCLUDED
