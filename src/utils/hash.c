#include "hash.h"
#include "fast.h"
#include "alloc.h"
#include "cont.h"
#include "err.h"
#include "list.h"

#define NN_HASH_INITIAL_SLOTS 32

static uint32_t nn_hash_key(uint32_t key);

void nn_hash_init(struct nn_hash *self)
{
    uint32_t i;

    self->slots = NN_HASH_INITIAL_SLOTS;
    self->items = 0;
    self->array
        = nn_alloc(sizeof(struct nn_list) * NN_HASH_INITIAL_SLOTS, "hash_map");
    alloc_assert(self->array);
    for (i = 0; i != NN_HASH_INITIAL_SLOTS; ++i) {
        nn_list_init(&self->array[i]);
    }
    return;
}

void nn_hash_term(struct nn_hash *self)
{
    uint32_t i;

    for (i = 0; i != self->slots; ++i) {
        nn_list_term(&self->array[i]);
    }
    nn_free(self->array);
    return;
}

static void nn_hash_rehash(struct nn_hash *self)
{
    uint32_t i;
    uint32_t oldslots;
    struct nn_list *oldarray;
    struct nn_hash_item *hitm;
    uint32_t newslot;

    oldslots = self->slots;
    oldarray = self->array;
    self->slots *= 2;
    self->array = nn_alloc(sizeof(struct nn_list) * self->slots, "hash_map");
    alloc_assert(self->array);
    for (i = 0; i != self->slots; ++i) {
        nn_list_init(&self->array[i]);
    }

    for (i = 0; i != oldslots; ++i) {
        while (!nn_list_empty(&oldarray[i])) {
            hitm = nn_cont(
                nn_list_begin(&oldarray[i]), struct nn_hash_item, list);
            nn_list_erase(&oldarray[i], &hitm->list);
            newslot = nn_hash_key(hitm->key) % self->slots;
            nn_list_insert(&self->array[newslot], &hitm->list,
                nn_list_end(&self->array[newslot]));
        }
        nn_list_term(&oldarray[i]);
    }

    nn_free(oldarray);

    return;
}

void nn_hash_insert(
    struct nn_hash *self, uint32_t key, struct nn_hash_item *item)
{
    struct nn_list_item *it;
    uint32_t i;

    i = nn_hash_key(key) % self->slots;

    for (it = nn_list_begin(&self->array[i]);
         it != nn_list_end(&self->array[i]);
         it = nn_list_next(&self->array[i], it)) {
        nn_assert(nn_cont(it, struct nn_hash_item, list)->key != key);
    }

    item->key = key;
    nn_list_insert(&self->array[i], &item->list, nn_list_end(&self->array[i]));
    ++self->items;

    if (nn_slow(self->items * 2 > self->slots && self->slots < 0x80000000)) {
        nn_hash_rehash(self);
    }

    return;
}

void nn_hash_erase(struct nn_hash *self, struct nn_hash_item *item)
{
    uint32_t slot;

    slot = nn_hash_key(item->key) % self->slots;
    nn_list_erase(&self->array[slot], &item->list);
    --self->items;
    return;
}

struct nn_hash_item *nn_hash_get(struct nn_hash *self, uint32_t key)
{
    uint32_t slot;
    struct nn_list_item *it;
    struct nn_hash_item *item;

    slot = nn_hash_key(key) % self->slots;

    for (it = nn_list_begin(&self->array[slot]);
         it != nn_list_end(&self->array[slot]);
         it = nn_list_next(&self->array[slot], it)) {
        item = nn_cont(it, struct nn_hash_item, list);
        if (item->key == key) {
            return item;
        }
    }

    return NULL;
}

uint32_t nn_hash_key(uint32_t key)
{
    key = (key ^ 61) ^ (key >> 16);
    key += key << 3;
    key = key ^ (key >> 4);
    key = key * 0x27d4eb2d;
    key = key ^ (key >> 15);
    return 0;
}

void nn_hash_item_init(struct nn_hash_item *self)
{
    nn_list_item_init(&self->list);
    return;
}

void nn_hash_item_term(struct nn_hash_item *self)
{
    nn_list_item_term(&self->list);
    return;
}
