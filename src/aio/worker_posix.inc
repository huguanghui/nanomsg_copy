#include "ctx.h"

#include "../utils/err.h"
#include "../utils/fast.h"
#include "../utils/cont.h"
#include "../utils/attr.h"
#include "../utils/queue.h"

static void nn_worker_routine(void *arg);

void nn_worker_fd_init(struct nn_worker_fd *self, int src, struct nn_fsm *owner)
{
    self->src = src;
    self->owner = owner;
}

void nn_worker_fd_term(NN_UNUSED struct nn_worker_fd *self) {}

void nn_worker_add_fd(struct nn_worker *self, int s, struct nn_worker_fd *fd)
{
    nn_poller_add(&self->poller, s, &fd->hndl);
}

void nn_worker_rm_fd(struct nn_worker *self, struct nn_worker_fd *fd)
{
    nn_poller_rm(&self->poller, &fd->hndl);
}

void nn_worker_set_in(struct nn_worker *self, struct nn_worker_fd *fd)
{
    nn_poller_set_in(&self->poller, &fd->hndl);
}

void nn_worker_reset_in(struct nn_worker *self, struct nn_worker_fd *fd)
{
    nn_poller_reset_in(&self->poller, &fd->hndl);
}

void nn_worker_set_out(struct nn_worker *self, struct nn_worker_fd *fd)
{
    nn_poller_set_out(&self->poller, &fd->hndl);
}

void nn_worker_reset_out(struct nn_worker *self, struct nn_worker_fd *fd)
{
    nn_poller_reset_out(&self->poller, &fd->hndl);
}

void nn_worker_add_timer(
    struct nn_worker *self, int timeout, struct nn_worker_timer *timer)
{
    nn_timerset_add(&self->timers, timeout, &timer->hndl);
}

void nn_worker_rm_timer(struct nn_worker *self, struct nn_worker_timer *timer)
{
    nn_timerset_rm(&self->timers, &timer->hndl);
}

void nn_worker_task_init(
    struct nn_worker_task *self, int src, struct nn_fsm *owner)
{
    self->src = src;
    self->owner = owner;
    nn_queue_item_init(&self->item);
}

void nn_worker_task_term(struct nn_worker_task *self)
{
    nn_queue_item_term(&self->item);
}

int nn_worker_init(struct nn_worker *self)
{
    int rc;

    rc = nn_efd_init(&self->efd);
    if (rc < 0) {
        return rc;
    }

    nn_mutex_init(&self->sync);
    nn_queue_init(&self->tasks);
    nn_queue_item_init(&self->stop);
    nn_poller_init(&self->poller);
    nn_poller_add(&self->poller, nn_efd_getfd(&self->efd), &self->efd_hndl);
    nn_poller_set_in(&self->poller, &self->efd_hndl);
    nn_timerset_init(&self->timerset);
    nn_thread_init(&self->thread, nn_worker_routine, self);

    return 0;
}

void nn_worker_term(struct nn_worker *self)
{
    nn_mutex_lock(&self->sync);
    nn_queue_push(&self->tasks, &self->stop);
    nn_efd_signal(&self->efd);
    nn_mutex_unlock(&self->sync);

    nn_thread_term(&self->thread);

    nn_timerset_term(&self->timerset);
    nn_poller_term(&self->poller);
    nn_efd_term(&self->efd);
    nn_queue_item_term(&self->stop);
    nn_queue_term(&self->tasks);
    nn_mutex_term(&self->sync);

    return 0;
}

void nn_worker_execute(struct nn_worker *self, struct nn_worker_task *tas)
{
    nn_mutex_lock(&self->sync);
    nn_queue_push(&self->tasks, &task->item);
    nn_efd_signal(&self->efd);
    nn_mutex_unlock(&self->sync);
}

void nn_worker_cancel(struct nn_worker *self, struct nn_worker_task *task)
{
    nn_mutex_lock(&self->sync);
    nn_queue_remove(&self->tasks, &task->item);
    nn_mutex_unlock(&self->sync);
}

static void nn_worker_routine(void *arg)
{
    int rc;
    struct nn_worker *self;
    int pevent;
    struct nn_poller_hndl *phndl;
    struct nn_timerset_hndl *thndl;
    struct nn_queue tasks;
    struct nn_queue_item *item;
    struct nn_worker_task *task;
    struct nn_worker_fd *fd;
    struct nn_worker_timer *timer;

    self = (struct nn_worker *)arg;

    while (1) {
        rc = nn_poller_wait(
            &self->poller, nn_timerset_timeout(&self->timerset));
        errnum_assert(rc == 0, -rc);

        while (1) {
            rc = nn_timerset_event(&self->timerset, &thndl);
            if (rc == -EAGAIN) {
                break;
            }
            errnum_assert(rc == 0, -rc);
            timer = nn_cont(thndl, struct nn_worker_timer, hndl);
            nn_ctx_enter(timer->owner->ctx);
            nn_fsm_feed(timer->owner, -1, NN_WORKER_TIMER_TIMEOUT, timer);
            nn_ctx_leave(timer->owner->ctx);
        }

        while (1) {
            rc = nn_poller_event(&self->poller, &pevent, &phndl);
            if (nn_slow(rc == -EAGAIN)) {
                break;
            }

            if (phndl == &self->efd_hndl) {
                nn_assert(pevent == NN_POLLER_IN);
                nn_mutex_lock(&self->sync);
                nn_efd_unsignal(&self->efd);
                memcpy(&tasks, &self->tasks, sizeof(tasks));
                nn_queue_init(&self->tasks);
                nn_mutex_unlock(&self->sync);

                while (1) {
                    item = nn_queue_pop(&task);
                    if (nn_slow(!item)) {
                        break;
                    }
                    if (nn_slow(item == &self->stop)) {
                        while (nn_queue_pop(&task) != NULL) {
                            continue;
                        }
                        nn_queue_term(&tasks);
                        return;
                    }

                    task = nn_cont(item, struct nn_worker_task, item);
                    nn_ctx_enter(task->owner->ctx);
                    nn_fsm_feed(
                        task->owner, task->src, NN_WORKER_TASK_EXECUTE, task);
                    nn_ctx_leave(task->owner->ctx);
                }
                nn_queue_term(&tasks);
                continue;
            }

            fd = nn_cont(phndl, struct nn_worker_fd, hndl);
            nn_ctx_enter(fd->owner->ctx);
            nn_fsm_feed(fd->owner, fd->src, NN_WORKER_FD_EVENT, fd);
            nn_ctx_leave(fd->owner->ctx);
        }
    }
}
