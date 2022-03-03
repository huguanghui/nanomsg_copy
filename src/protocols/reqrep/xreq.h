#ifndef NN_XREQ_INCLUDED
#define NN_XREQ_INCLUDED

#include "../../protocol.h"

#include "../utils/lb.h"
#include "../utils/fq.h"

struct nn_xreq {
    struct nn_sockbase sockbase;
    struct nn_lb lb;
    struct nn_fq fq;
};

#endif // !NN_XREQ_INCLUDED
