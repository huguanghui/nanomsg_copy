#ifndef NN_XPUB_INCLUDED
#define NN_XPUB_INCLUDED

#include "../../protocol.h"

int nn_xpub_create(void *hint, struct nn_sockbase **sockbase);
int nn_xpub_ispeer(int socktype);

#endif // !NN_XPUB_INCLUDED
