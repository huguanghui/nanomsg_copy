#include "fsm.h"
#include "timerset.h"

#include "../utils/win.h"
#include "../utils/thread.h"

struct nn_worker_task {
    int src;
    struct nn_fsm *owner;
};

#define NN_WORKER_OP_DONE (1)
#define NN_WORKER_OP_ERROR (2)

struct nn_worker_op {
    int src;
    struct nn_fsm *owner;
    int state;

    OVERLAPPED olpd;

    size_t resid;
    char *buf;
    void *arg;
    void (*start)(struct nn_usock *);
    int zero_is_error;
};

void nn_worker_op_init(
    struct nn_worker_op *self, int src, struct nn_fsm *owner);

void nn_worker_op_term(struct nn_worker_op *self);

void nn_worker_op_start(struct nn_worker_op *self);

int nn_worker_op_isidle(struct nn_worker_op *self);

struct nn_worker {
    HANDLE cp;
    struct nn_timerset timerset;
    struct nn_thread thread;
};
