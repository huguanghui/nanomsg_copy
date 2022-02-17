#ifndef NN_FSM_INCLUDED
#define NN_FSM_INCLUDED

/*    FSM - 有限状态机    */
#include "../utils/queue.h"

struct nn_ctx;
struct nn_fsm;
struct nn_worker;

struct nn_fsm_event {
    struct nn_fsm *fsm;
    int src;
    void *srcptr;
    int type;
    struct nn_queue_item item;
};

void nn_fsm_event_init(struct nn_fsm_event *self);
void nn_fsm_event_term(struct nn_fsm_event *self);
int nn_fsm_event_active(struct nn_fsm_event *self);
void nn_fsm_event_process(struct nn_fsm_event *self);

#define NN_FSM_ACTION (-2)

#define NN_FSM_START (-2)
#define NN_FSM_STOP (-3)

typedef void (*nn_fsm_fn)(struct nn_fsm *self, int src, int type, void *srcptr);

struct nn_fsm_owner {
    int src;
    struct nn_fsm *fsm;
};

struct nn_fsm {
    nn_fsm_fn fn;
    nn_fsm_fn shutdown_fn;
    int state;
    int src;
    void *srcptr;
    struct nn_fsm *owner;
    struct nn_ctx *ctx;
    struct nn_fsm_event stopped;
};

void nn_fsm_init_root(struct nn_fsm *self, nn_fsm_fn fn, nn_fsm_fn shutdown_fn,
    struct nn_ctx *ctx);
void nn_fsm_init(struct nn_fsm *self, nn_fsm_fn fn, nn_fsm_fn shutdown_fn,
    int src, void *srcptr, struct nn_fsm *owner);
void nn_fsm_term(struct nn_fsm *self);

int nn_fsm_isidle(struct nn_fsm *self);
void nn_fsm_start(struct nn_fsm *self);
void nn_fsm_stop(struct nn_fsm *self);
void nn_fsm_stopped(struct nn_fsm *self, int type);
void nn_fsm_stopped_noevent(struct nn_fsm *self);

void nn_fsm_swap_owner(struct nn_fsm *self, struct nn_fsm_owner *owner);

struct nn_worker *nn_fsm_choose_worker(struct nn_fsm *self);

void nn_fsm_action(struct nn_fsm *self, int type);

void nn_fsm_raise_from_src(
    struct nn_fsm *self, struct nn_fsm_event *event, int src, int type);

void nn_fsm_raise(struct nn_fsm *self, struct nn_fsm_event *event, int type);

void nn_fsm_raiseto(struct nn_fsm *self, struct nn_fsm *dst,
    struct nn_fsm_event *event, int src, int type, void *srcptr);

void nn_fsm_feed(struct nn_fsm *self, int src, int type, void *srcptr);

#endif // !NN_FSM_INCLUDED
