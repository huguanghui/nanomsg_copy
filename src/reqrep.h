#ifndef REQREP_H_INCLUDED
#define REQREP_H_INCLUDED

#include "nn.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NN_PROTO_REQREP (3)

#define NN_REQ (NN_PROTO_REQREP * 16 + 0)
#define NN_REP (NN_PROTO_REQREP * 16 + 1)

#define NN_REQ_RESEND_IVL (1)

typedef union nn_req_handle {
    int i;
    void *ptr;
} nn_req_handle;

#ifdef __cplusplus
}
#endif

#endif // !REQREP_H_INCLUDED
