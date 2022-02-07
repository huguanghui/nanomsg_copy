#include "sleep.h"
#include "err.h"

#ifdef NN_HAVE_WINDOWS

#include "win.h"

void nn_sleep(int milliseconds)
{
    Sleep(milliseconds);
    return;
}

#else

#include <time.h>

void nn_sleep(int milliseconds)
{
    int rc;
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    rc = nanosleep(&ts, NULL);
    errno_assert(rc == 0);
    return;
}
#endif /* NN_HAVE_WINDOWS */
