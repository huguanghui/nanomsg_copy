#ifndef NN_WORKER_INCLUDED
#define NN_WORKER_INCLUDED

#include "fsm.h"
#include "timerset.h"

#if defined(NN_HAVE_WINDOWS)
#include "worker_win.h"
#else
#include "worker_posix.h"
#endif

#define NN_WORKER_TIMER_TIMEOUT 1

struct nn_worker_timer {
    struct nn_fsm *owner;
    struct nn_timerset_hndl hndl;
};

void nn_worker_timer_init(struct nn_worker_timer *self, struct nn_fsm *owner);
void nn_worker_timer_term(struct nn_worker_timer *self);
int nn_worker_timer_isactive(struct nn_worker_timer *self);

#define NN_WORKER_TASK_EXECUTE 1

struct nn_worker_task;

void nn_worker_task_init(
    struct nn_worker_task *self, int src, struct nn_fsm *owner);
void nn_worker_task_term(struct nn_worker_task *self);

struct nn_worker;

int nn_worker_init(struct nn_worker *self);
void nn_worker_term(struct nn_worker *self);
void nn_worker_execute(struct nn_worker *self, struct nn_worker_task *task);
void nn_worker_cancel(struct nn_worker *self, struct nn_worker_task *task);

void nn_worker_add_timer(
    struct nn_worker *self, int timeout, struct nn_worker_timer *timer);
void nn_worker_rm_timer(struct nn_worker *self, struct nn_worker_timer *timer);

#endif // !NN_WORKER_INCLUDED
