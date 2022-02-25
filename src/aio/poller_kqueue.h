#include <sys/time.h>
#include <sys/types.h>
#include <sys/event.h>

#define NN_POLLER_MAX_EVENTS 32

#define NN_POLLER_EVENT_IN 1
#define NN_POLLER_EVENT_OUT 2

struct nn_poller_hndl {
    int fd;
    int events;
};

struct nn_poller {
    int kq;

    int nevents;

    int index;

    struct kevent events[NN_POLLER_MAX_EVENTS];
};
