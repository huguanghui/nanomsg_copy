#include "timerset.h"

#include "../utils/fast.h"
#include "../utils/cont.h"
#include "../utils/clock.h"
#include "../utils/err.h"

void nn_timerset_init(struct nn_timerset *self)
{
    nn_list_init(&self->timeouts);
    return;
}

void nn_timerset_term(struct nn_timerset *self)
{
    nn_list_term(&self->timeouts);
    return;
}

int nn_timerset_add(
    struct nn_timerset *self, int timeout, struct nn_timerset_hndl *hndl)
{
    struct nn_list_item *it;
    struct nn_timerset_hndl *ith;
    int first;

    hndl->timeout = nn_clock_ms() + timeout;

    for (it = nn_list_begin(&self->timeouts);
         it != nn_list_end(&self->timeouts);
         it = nn_list_next(&self->timeouts, it)) {
        ith = nn_cont(it, struct nn_timerset_hndl, list);
        if (hndl->timeout < ith->timeout) {
            break;
        }
    }

    first = nn_list_begin(&self->timeouts) == it ? 1 : 0;
    nn_list_insert(&self->timeouts, &hndl->list, it);
    return first;
}

int nn_timerset_rm(struct nn_timerset *self, struct nn_timerset_hndl *hndl)
{
    int first;

    if (!nn_list_item_isinlist(&hndl->list)) {
        return 0;
    }
    first = nn_list_begin(&self->timeouts) == &hndl->list ? 1 : 0;
    nn_list_erase(&self->timeouts, &hndl->list);
    return first;
}
int nn_timerset_timeout(struct nn_timerset *self)
{
    int timeout;

    if (nn_fast(nn_list_empty(&self->timeouts))) {
        return -1;
    }

    timeout = (int)(nn_cont(nn_list_begin(&self->timeouts),
                        struct nn_timerset_hndl, list)
                        ->timeout
        - nn_clock_ms());

    return timeout < 0 ? 0 : timeout;
}

int nn_timerset_event(struct nn_timerset *self, struct nn_timerset_hndl **hndl)
{
    struct nn_timerset_hndl *first;

    if (nn_fast(nn_list_empty(&self->timeouts))) {
        return -EAGAIN;
    }

    first = nn_cont(
        nn_list_begin(&self->timeouts), struct nn_timerset_hndl, list);
    if (first->timeout > nn_clock_ms()) {
        return -EAGAIN;
    }

    nn_list_erase(&self->timeouts, &first->list);
    *hndl = first;
    return 0;
}

void nn_timerset_hndl_init(struct nn_timerset_hndl *self)
{
    nn_list_item_init(&self->list);
}

void nn_timerset_hndl_term(struct nn_timerset_hndl *self)
{
    nn_list_item_term(&self->list);
}

int nn_timerset_hndl_isactive(struct nn_timerset_hndl *self)
{
    return nn_list_item_isinlist(&self->list);
}
