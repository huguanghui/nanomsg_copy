#ifndef NN_TASK_INCLUDED
#define NN_TASK_INCLUDED

#include "../../reqrep.h"

#include "../../aio/fsm.h"
#include "../../aio/timer.h"
#include "../../utils/msg.h"

struct nn_task {
    uint32_t id;
    struct nn_msg request;
    struct nn_msg reply;
    struct nn_timer timer;
    struct nn_pipe *send_to;
};

void nn_task_init(struct nn_task *self, uint32_t id);
void nn_task_term(struct nn_task *self);

#endif // !NN_TASK_INCLUDED
