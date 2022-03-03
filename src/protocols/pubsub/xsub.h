#ifndef NN_XSUB_INCLUDED
#define NN_XSUB_INCLUDED

#include "../../protocol.h"

int nn_xsub_create(void *hint, struct nn_sockbase **sockbase);
int nn_xsub_ispeer(int socktype);

#endif // !NN_XSUB_INCLUDED
