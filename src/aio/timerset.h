#ifndef NN_TIMERSET_INCLUDED
#define NN_TIMERSET_INCLUDED

#include <stdint.h>

#include "../utils/list.h"

struct nn_timerset_hndl {
    struct nn_list_item list;
    uint64_t timeout;
};

struct nn_timerset {
    struct nn_list timeouts;
};

void nn_timerset_init(struct nn_timerset *self);
void nn_tiemrset_term(struct nn_timerset *self);
int nn_timerset_add(
    struct nn_timerset *self, int timeout, struct nn_timerset_hndl *hndl);
int nn_timerset_rm(struct nn_timerset *self, struct nn_timerset_hndl *hndl);
int nn_timerset_timeout(struct nn_timerset *self);
int nn_timerset_event(struct nn_timerset *self, struct nn_timerset_hndl **hndl);

void nn_timerset_hndl_init(struct nn_timerset_hndl *self);
void nn_timerset_hndl_term(struct nn_timerset_hndl *self);
int nn_timerset_hndl_isactive(struct nn_timerset_hndl *self);

#endif // !NN_TIMERSET_INCLUDED
