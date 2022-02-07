#ifndef NN_QUEUE_INCLUDED
#define NN_QUEUE_INCLUDED

#define NN_QUEUE_NOTINQUEUE ((struct nn_queue_item *)-1)

#define NN_QUEUE_ITEM_INITIALIZER                                              \
    {                                                                          \
        NN_LIST_NOTINQUEUE                                                     \
    }

struct nn_queue_item {
    struct nn_queue_item *next;
};

struct nn_queue {
    struct nn_queue_item *head;
    struct nn_queue_item *tail;
};

void nn_queue_init(struct nn_queue *self);

void nn_queue_term(struct nn_queue *self);

int nn_queue_empty(struct nn_queue *self);

void nn_queue_push(struct nn_queue *self, struct nn_queue_item *item);

void nn_queue_remove(struct nn_queue *self, struct nn_queue_item *item);

struct nn_queue_item *nn_queue_pop(struct nn_queue *self);

void nn_queue_item_init(struct nn_queue_item *self);

void nn_queue_item_term(struct nn_queue_item *self);

int nn_queue_item_isinqueue(struct nn_queue_item *self);

#endif // !NN_QUEUE_INCLUDED
