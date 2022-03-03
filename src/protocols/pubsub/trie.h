#ifndef NN_TRIE_INCLUDED
#define NN_TRIE_INCLUDED

#include <stddef.h>
#include <stdint.h>

#define NN_TRIE_PREFIX_MAX (10)
#define NN_TRIE_SPARSE_MAX (8)
#define NN_TRIE_DENSE_TYPE (NN_TRIE_SPARSE_MAX + 1)

struct nn_trie_node {
    uint32_t refcount;
    uint8_t type;
    uint8_t prefix_len;
    uint8_t prefix[NN_TRIE_PREFIX_MAX];
    union {
        struct {
            uint8_t children[NN_TRIE_SPARSE_MAX];
        } sparse;
        struct {
            uint8_t min;
            uint8_t max;
            uint16_t nbr;
        } dense;
    } u;
};

struct nn_trie {
    struct nn_trie_node *root;
};

void nn_trie_init(struct nn_trie *self);

void nn_trie_term(struct nn_trie *self);

int nn_trie_subscribe(struct nn_trie *self, const uint8_t *data, size_t size);

int nn_trie_unsubscribe(struct nn_trie *self, const uint8_t *data, size_t size);

int nn_trie_match(struct nn_trie *self, const uint8_t *data, size_t size);

void nn_trie_dump(struct nn_trie *self);

#endif // !NN_TRIE_INCLUDED
