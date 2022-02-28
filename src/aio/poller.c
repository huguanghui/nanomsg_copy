#include "poller.h"

#if defined(NN_USE_EPOLL)
#include "poller_epoll.inc"
#elif defined(NN_USE_KQUEUE)
#include "poller_kqueue.inc"
#elif defined(NN_USE_POLL)
#include "poller_poll.inc"
#else
#error
#endif
