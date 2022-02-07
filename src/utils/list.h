#ifndef NN_LIST_INCLUDED
#define NN_LIST_INCLUDED

struct nn_list_item {
    struct nn_list_item *next;
    struct nn_list_item *prev;
};

struct nn_list {
    struct nn_list_item *first;
    struct nn_list_item *last;
};

#define NN_LIST_NOTINLIST ((struct nn_list_item *)-1)

#define NN_LIST_INITIALIZER                                                    \
    {                                                                          \
        NN_LIST_NOTINLIST, NN_LIST_NOTINLIST                                   \
    }

void nn_list_init(struct nn_list *self);

void nn_list_term(struct nn_list *self);

int nn_list_empty(struct nn_list *self);

struct nn_list_item *nn_list_begin(struct nn_list *self);

struct nn_list_item *nn_list_end(struct nn_list *self);

struct nn_list_item *nn_list_prev(
    struct nn_list *self, struct nn_list_item *it);

struct nn_list_item *nn_list_next(
    struct nn_list *self, struct nn_list_item *it);

void nn_list_insert(
    struct nn_list *self, struct nn_list_item *item, struct nn_list_item *it);

struct nn_list_item *nn_list_erase(
    struct nn_list *self, struct nn_list_item *item);

void nn_list_item_init(struct nn_list_item *self);

void nn_list_item_term(struct nn_list_item *self);

int nn_list_item_isinlist(struct nn_list_item *self);

#endif // !NN_LIST_INCLUDED
