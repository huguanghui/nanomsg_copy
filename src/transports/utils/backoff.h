#ifndef NN_BACKOFF_INCLUDED
#define NN_BACKOFF_INCLUDED

#include "../../aio/timer.h"

#define NN_BACKOFF_TIMEOUT NN_TIMER_TIMEOUT
#define NN_BACKOFF_STOPPED NN_TIMER_STOPPED

struct nn_backoff {
    struct nn_timer timer;
    int minivl;
    int maxivl;
    int n;
};

void nn_backoff_init(struct nn_backoff *self, int src, int minival, int maxival,
    struct nn_fsm *owner);
void nn_backoff_term(struct nn_backoff *self);

int nn_backoff_isidle(struct nn_backoff *self);
void nn_backoff_start(struct nn_backoff *self);
void nn_backoff_stop(struct nn_backoff *self);

void nn_backoff_reset(struct nn_backoff *self);

#endif // !NN_BACKOFF_INCLUDED
