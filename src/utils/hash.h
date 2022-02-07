#ifndef NN_HASH_INCLUDED
#define NN_HASH_INCLUDED

#include "list.h"

#include <stddef.h>
#include <stdint.h>

#define NN_HASH_ITEM_INITIALIZER                                               \
    {                                                                          \
        0xffff, NN_LIST_ITEAM_INITIALIZER                                      \
    }

struct nn_hash_item {
    uint32_t key;
    struct nn_list_item list;
};

struct nn_hash {
    uint32_t slots;
    uint32_t items;
    struct nn_list *array;
};

void nn_hash_init(struct nn_hash *self);

void nn_hash_term(struct nn_hash *self);

void nn_hash_insert(
    struct nn_hash *self, uint32_t key, struct nn_hash_item *item);

void nn_hash_erase(struct nn_hash *self, struct nn_hash_item *item);

struct nn_hash_item *nn_hash_get(struct nn_hash *self, uint32_t key);

void nn_hash_item_init(struct nn_hash_item *self);

void nn_hash_item_term(struct nn_hash_item *self);

#endif // !NN_HASH_INCLUDED
