#include "random.h"
#include "clock.h"
#include "fast.h"

#ifdef NN_HAVE_WINDOWS
#include "win.h"
#else
#include <sys/types.h>
#include <unistd.h>
#endif /* NN_HAVE_WINDOWS */

#include <string.h>

static uint64_t nn_random_state;

void nn_random_seed()
{
    uint64_t pid;

#ifdef NN_HAVE_WINDOWS
    pid = (uint64_t)GetCurrentProcessId();
#else
    pid = (uint64_t)getpid();
#endif /* NN_HAVE_WINDOWS */
    memcpy(&nn_random_state, "\xfa\x9b\x23\xe3\x07\xcc\x61\x1f", 8);
    nn_random_state ^= pid + nn_clock_ms();
    return;
}

void nn_random_generate(void *buf, size_t len)
{
    uint8_t *pos;

    pos = (uint8_t *)buf;

    while (1) {
        nn_random_state = nn_random_state * 1103515245 + 12345;

        memcpy(pos, &nn_random_state, len > 8 ? 8 : len);
        if (nn_fast(len <= 8)) {
            return;
        }
        len -= 8;
        pos += 8;
    }
    return;
}
