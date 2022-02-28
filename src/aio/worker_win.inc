#include "ctx.h"
#include "usock.h"

#include "../utils/err.h"
#include "../utils/cont.h"
#include "../utils/fast.h"

#define NN_WORKER_MAX_EVENTS (32)

#define NN_WORKER_OP_STATE_IDLE (1)
#define NN_WORKER_OP_STATE_ACTIVE (2)

const int nn_worker_stop = 0;

static void nn_worker_routine(void *arg);

void nn_worker_task_init(
    struct nn_worker_task *self, int src, struct nn_fsm *owner)
{
    self->src = src;
    self->owner = owner;
}

void nn_worker_task_term(struct nn_worker_task *self) {}

void nn_worker_op_init(struct nn_worker_op *self, int src, struct nn_fsm *owner)
{
    self->src = src;
    self->owner = owner;
    self->state = NN_WORKER_OP_STATE_IDLE;
    self->start = NULL;
    self->buf = NULL;
    self->resid = 0;
    self->zero_is_error = 0;
}

void nn_worker_op_term(struct nn_worker_op *self)
{
    nn_assert_state(self, NN_WORKER_OP_STATE_IDLE);
}

void nn_worker_op_start(struct nn_worker_op *self)
{
    self->state = NN_WORKER_OP_STATE_ACTIVE;
}

int nn_worker_op_isidle(struct nn_worker_op *self)
{
    return self->state == NN_WORKER_OP_STATE_IDLE ? 1 : 0;
}

int nn_worker_init(struct nn_worker *self)
{
    self->cp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    win_assert(self->cp);
    nn_timerset_init(&self->timerset);
    nn_thread_init(&self->thread, nn_worker_routine, self);

    return 0;
}

void nn_worker_term(struct nn_worker *self)
{
    BOOL brc;

    brc = PostQueuedCompletionStatus(
        self->cp, 0, (ULONG_PTR)&nn_worker_stop, NULL);
    win_assert(brc);

    nn_thread_term(&self->thread);

    nn_timerset_term(&self->timerset);
    brc = CloseHandle(self->cp);
    win_assert(brc);
}

void nn_worker_execute(struct nn_worker *self, struct nn_worker_task *task)
{
    BOOL brc;

    brc = PostQueuedCompletionStatus(self->cp, 0, (ULONG_PTR)task, NULL);
    wine_assert(brc);
}

void nn_worker_add_timer(
    struct nn_worker *self, int timeout, struct nn_worker_timer *timer)
{
    nn_timerset_add(
        &((struct nn_worker *)self)->timerset, timeout, &timer->hndl);
}

void nn_worker_rm_timer(struct nn_worker *self, struct nn_worker_timer *timer)
{
    nn_timerset_rm(&((struct nn_worker *)self)->timerset, &timer->hndl);
}

HANDLE nn_worker_getcp(struct nn_worker *self) { return self->cp; }

static void nn_worker_routine(void *arg)
{
    int rc;
    BOOL brc;
    struct nn_worker *self;
    int timeout;
    ULONG count;
    ULONG i;
    struct nn_timerset_hndl *thndl;
    struct nn_worker_timer *timer;
    struct nn_worker_task *task;
    struct nn_worker_op *op;
    OVERLAPPED_ENTRY entries[NN_WORKER_MAX_EVENTS];

    self = (struct nn_worker *)arg;

    while (1) {
        while (1) {
            rc = nn_timerset_event(&self->timerset, &thndl);
            if (nn_fast(rc == -EAGAIN)) {
                break;
            }
            errnum_assert(rc == 0, -rc);
            timer = nn_cont(thndl, struct nn_worker_timer, hndl);
            nn_ctx_enter(timer->owner->ctx);
            nn_fsm_enter(timer->owner->ctx);
            nn_fsm_feed(timer->owner, -1, NN_WORKER_TIMER_TIMEOUT, timer);
            nn_ctx_leave(timer->owner->ctx);
        }

        timeout = nn_timerset_timeout(&self->timerset);

        brc = GetQueuedCompletionStatusEx(self->cp, entries,
            NN_WORKER_MAX_EVENTS, &count, timeout < 0 ? INFINITE : timeout,
            FALSE);
        if (nn_slow(!brc && GetLastError() == WAIT_TIMEOUT)) {
            continue;
        }
        win_assert(brc);

        for (i = 0; i != count; ++i) {
            if (nn_fast(entries[i].lpOverlapped != NULL)) {
                DWORD nxfer;
                op = nn_cont(
                    entries[i].lpOverlapped, struct nn_worker_op, olpd);

                rc = entries[i].Internal & 0xc0000000;
                switch (rc) {
                case 0x00000000:
                    nxfer = entries[i].dwNumberOfBytesTransferred;

                    if ((nxfer == 0) && (op->zero_is_error != 0)) {
                        rc = NN_WORKER_OP_ERROR;
                        break;
                    }
                    if (op->start != NULL) {
                        if (nxfer > op->resid) {
                            rc = NN_WORKER_OP_ERROR;
                            break;
                        }
                        op->resid -= nxfer;
                        op->buf += nxfer;

                        if (op->resid != 0) {
                            op->start(op->arg);
                        }
                        continue;
                    }
                    rc = NN_WORKER_OP_DONE;
                    break;
                case 0xc0000000:
                    nxfer = 0;
                    rc = NN_WORKER_OP_ERROR;
                    break;
                default:
                    continue;
                }

                nn_ctx_enter(op->owner->ctx);
                nn_assert(op->state != NN_WORKER_OP_STATE_IDLE);

                op->state = NN_WORKER_OP_STATE_IDLE;

                nn_fsm_feed(op->owner, op->src, rc, op);
                nn_ctx_leave(op->owner->ctx);

                continue;
            }

            if (nn_slow(
                    entries[i].lpCompletionKey == (ULONG_PTR)&nn_worker_stop)) {
                return;
            }

            task = (struct nn_worker_task *)entries[i].lpCompletionKey;
            nn_ctx_enter(task->owner->ctx);
            nn_fsm_feed(task->owner, task->src, NN_WORKER_TASK_EXECUTE, task);
            nn_ctx_leave(task->owner->ctx);
        }
    }
}
