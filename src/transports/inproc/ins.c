#include "ins.h"

#include "../../utils/mutex.h"
#include "../../utils/alloc.h"
#include "../../utils/list.h"
#include "../../utils/cont.h"
#include "../../utils/fast.h"
#include "../../utils/err.h"

struct nn_ins {
    struct nn_mutex sync; // 同步保护
    struct nn_list bound;
    struct nn_list connected;
};

static struct nn_ins self;

void nn_ins_item_init(struct nn_ins_item *self, struct nn_ep *ep)
{
    self->ep = ep;
    nn_list_item_init(&self->item);
}

void nn_ins_item_term(struct nn_ins_item *self)
{
    nn_list_item_term(&self->item);
}

void nn_ins_init(void)
{
    nn_mutex_init(&self.sync);
    nn_list_init(&self.bound);
    nn_list_init(&self.connected);
}

void nn_ins_term(void)
{
    nn_list_term(&self.connected);
    nn_list_term(&self.bound);
    nn_mutex_term(&self.sync);
}

int nn_ins_bind(struct nn_ins_item *item, nn_ins_fn fn)
{
    struct nn_list_item *it;
    struct nn_ins_item *bitem;
    struct nn_ins_item *citem;

    nn_mutex_lock(&self.sync);
    for (it = nn_list_begin(&self.bound); it != nn_list_end(&self.bound);
         it = nn_list_next(&self.bound, it)) {
        bitem = nn_cont(it, struct nn_ins_item, item);

        if (strncmp(nn_ep_getaddr(bitem->ep), nn_ep_getaddr(item->ep),
                NN_SOCKADDR_MAX)
            == 0) {
            nn_mutex_unlock(&self.sync);
            return -EADDRINUSE;
        }
    }
    nn_list_insert(&self.bound, &item->item, nn_list_end(&self.bound));

    for (it = nn_list_begin(&self.connected);
         it != nn_list_end(&self.connected);
         it = nn_list_next(&self.connected, it)) {
        citem = nn_cont(it, struct nn_ins_item, item);
        if (strncmp(nn_ep_getaddr(item->ep), nn_ep_getaddr(citem->ep),
                NN_SOCKADDR_MAX)
            == 0) {
            if (!nn_ep_ispeer_ep(item->ep, citem->ep)) {
                continue;
            }
            fn(item, citem);
        }
    }
    nn_mutex_unlock(&self.sync);
    return 0;
}

void nn_ins_connect(struct nn_ins_item *item, nn_ins_fn fn)
{
    struct nn_list_item *it;
    struct nn_ins_item *bitem;

    nn_mutex_lock(&self.sync);
    nn_list_insert(&self.connected, &item->item, nn_list_end(&self.connected));

    for (it = nn_list_begin(&self.bound); it != nn_list_end(&self.bound);
         it = nn_list_next(&self.bound, it)) {
        bitem = nn_cont(it, struct nn_ins_item, item);
        if (strncmp(nn_ep_getaddr(item->ep), nn_ep_getaddr(bitem->ep),
                NN_SOCKADDR_MAX)
            == 0) {
            if (!nn_ep_ispeer_ep(item->ep, bitem->ep)) {
                break;
            }
            fn(item, bitem);
            break;
        }
    }
    nn_mutex_unlock(&self.sync);
}

void nn_ins_disconnect(struct nn_ins_item *item)
{
    nn_mutex_lock(&self.sync);
    nn_list_erase(&self.connected, &item->item);
    nn_mutex_unlock(&self.sync);
}

void nn_ins_unbind(struct nn_ins_item *item)
{
    nn_mutex_lock(&self.sync);
    nn_list_erase(&self.bound, &item->item);
    nn_mutex_unlock(&self.sync);
}
