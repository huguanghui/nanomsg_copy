#ifndef NN_STREAMHDR_INCLUDED
#define NN_STREAMHDR_INCLUDED

#include "../../transport.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"
#include "../../aio/timer.h"

#define NN_STREAMHDR_OK (1)
#define NN_STREAMHDR_ERROR (2)
#define NN_STREAMHDR_STOPPED (3)

struct nn_streamhdr {
    struct nn_fsm fsm;
    int state;
    struct nn_timer timer;
    struct nn_usock *usock;
    struct nn_fsm_owner usock_owner;
    struct nn_pipebase *pipebase;
    uint8_t protohdr[8];
    struct nn_fsm_event done;
};

void nn_streamhdr_init(
    struct nn_streamhdr *self, int src, struct nn_fsm *owner);
void nn_streamhdr_term(struct nn_streamhdr *self);

int nn_streamhdr_isidle(struct nn_streamhdr *self);
void nn_streamhdr_start(struct nn_streamhdr *self, struct nn_usock *usock,
    struct nn_pipebase *pipebase);
void nn_streamhdr_stop(struct nn_streamhdr *self);

#endif // !NN_STREAMHDR_INCLUDED
