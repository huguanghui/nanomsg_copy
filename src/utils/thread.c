#include "thread.h"

#ifdef NN_HAVE_WINDOWS
#include "thread_win.inc"
#else
#include "thread_posix.inc"
#endif /* !NN_HAVE_WINDOWS */
