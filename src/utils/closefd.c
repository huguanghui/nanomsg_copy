#if !defined(NN_HAVE_WINDOWS)

#include "closefd.h"
#include "fast.h"
#include "err.h"

#include <unistd.h>

void nn_closefd(int fd)
{
    int rc;
    if (nn_slow(fd < 0)) {
        return;
    }
    rc = close(fd);
    if (nn_fast(rc == 0)) {
        return;
    }
    errno_assert(errno == EINTR || errno == ETIMEDOUT || errno == EWOULDBLOCK
        || errno == EINPROGRESS || errno == ECONNRESET);
}

#endif
