#ifndef NN_SHA1_INCLUDED
#define NN_SHA1_INCLUDED

#include <stdint.h>

#define SHA1_HASH_LEN (20)
#define SHA1_BLOCK_LEN (64)

struct nn_sha1 {
    uint32_t buffer[SHA1_BLOCK_LEN / sizeof(uint32_t)];
    uint32_t state[SHA1_HASH_LEN / sizeof(uint32_t)];
    uint32_t bytes_hashed;
    uint8_t buffer_offset;
    uint8_t is_little_endian;
};

void nn_sha1_init(struct nn_sha1 *self);
void nn_sha1_hashbyte(struct nn_sha1 *self, uint8_t data);
uint8_t *nn_sha1_result(struct nn_sha1 *self);

#endif // !NN_SHA1_INCLUDED
