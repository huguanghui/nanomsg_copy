#ifndef NN_INS_INCLUDED
#define NN_INS_INCLUDED

#include "../../transport.h"
#include "../../utils/list.h"

struct nn_ins_item {
    struct nn_list_item item;
    struct nn_ep *ep;
    int protocol;
};

void nn_ins_item_init(struct nn_ins_item *self, struct nn_ep *ep);
void nn_ins_item_term(struct nn_ins_item *self);

void nn_ins_init(void);
void nn_ins_term(void);

typedef void (*nn_ins_fn)(struct nn_ins_item *self, struct nn_ins_item *peer);

int nn_ins_bind(struct nn_ins_item *item, nn_ins_fn fn);
void nn_ins_connect(struct nn_ins_item *item, nn_ins_fn fn);
void nn_ins_disconnect(struct nn_ins_item *item);
void nn_ins_unbind(struct nn_ins_item *item);

#endif // !NN_INS_INCLUDED
