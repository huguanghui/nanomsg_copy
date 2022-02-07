#ifndef NN_RANDOM_INCLUDED
#define NN_RANDOM_INCLUDED

#include <stddef.h>

void nn_random_seed();

void nn_random_generate(void *buf, size_t len);

#endif // !NN_RANDOM_INCLUDED
