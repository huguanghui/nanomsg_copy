#ifndef NN_TIMER_INCLUDED
#define NN_TIMER_INCLUDED

#include "fsm.h"
#include "worker.h"

#define NN_TIMER_TIMEOUT (1)
#define NN_TIMER_STOPPED (2)

struct nn_timer {
    struct nn_fsm fsm;
    int state;
    struct nn_worker_task start_task;
    struct nn_worker_task stop_task;
    struct nn_worker_timer wtimer;
    struct nn_fsm_event done;
    struct nn_worker *worker;
    int timeout;
};

void nn_timer_init(struct nn_timer *self, int src, struct nn_fsm *owner);
void nn_timer_term(struct nn_timer *self);

int nn_timer_isidle(struct nn_timer *self);
void nn_timer_start(struct nn_timer *self, int timeout);
void nn_timer_stop(struct nn_timer *self);

#endif // !NN_TIMER_INCLUDED
