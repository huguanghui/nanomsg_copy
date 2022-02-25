#include <stdint.h>
#include <sys/types.h>
#include <sys/epoll.h>

#define NN_POLLER_HAVE_ASYNC_ADD 1

#define NN_POLLER_MAX_EVENTS 32

struct nn_poller_hndl {
    int fd;
    uint32_t events;
};

struct nn_poller {
    int ep;
    int nevents;
    int index;
    struct epoll_event events[NN_POLLER_MAX_EVENTS];
};
