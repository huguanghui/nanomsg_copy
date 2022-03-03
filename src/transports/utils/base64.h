#ifndef NN_BASE64_INCLUDED
#define NN_BASE64_INCLUDED

#include "../../utils/err.h"

#include <stddef.h>
#include <stdint.h>

int nn_base64_encode(
    const uint8_t *in, size_t in_len, char *out, size_t out_len);

int nn_base64_decode(
    const char *in, size_t in_len, uint8_t *out, size_t out_len);

#endif // !NN_BASE64_INCLUDED
