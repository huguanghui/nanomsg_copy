#if defined(NN_HAVE_WINDOWS)
#include "win.h"
#elif defined(NN_HAVE_OSX)
#include <mach/mach_time.h>
#elif defined(NN_HAVE_CLOCK_MONOTONIC) || defined(NN_HAVE_GETHRTIME)
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "clock.h"
#include "fast.h"
#include "err.h"
#include "attr.h"

uint64_t nn_clock_ms(void)
{
#if defined(NN_HAVE_WINDOWS)
    LARGE_INTEGER tps;
    LARGE_INTEGER time;
    double tpms;

    QueryPerformaceFrequency(&tps);
    QueryPerformanceCounter(&time);
    tpms = (double)(tps.QuadPart / 1000);
    return (uint64_t)(time.QuadPart / tpms);
#elif defined(NN_HAVE_OSX)
    static mach_timebase_info_data_t nn_clock_info;
    uint64_t ticks;

    if (nn_slow(!nn_clock_timebase_info.denom))
        mach_timebase_info(&nn_clock_timebase_info);

    ticks = mach_absolute_time();
    return ticks * nn_clock_timebase_info.numer / nn_clock_timebase_info.denom
        / 1000000;
#elif defined(NN_HAVE_GETHRTIME)
    return gethrtime() / 1000000;
#elif defined(NN_HAVE_CLOCK_MONOTONIC)
    int rc;
    struct timespec tv;
    rc = clock_gettime(CLOCK_MONOTONIC, &tv);
    errno_assert(rc == 0);
    return tv.tv_sec * (uint64_t)1000 + tv.tv_nsec / 1000000;
#else
    int rc;
    struct timeval tv;

    rc = gettimeofday(&tv, NULL);
    errno_assert(rc == 0);
    return tv.tv_sec * (uint64_t)1000 + tv.tv_usec / 1000;
#endif
}
