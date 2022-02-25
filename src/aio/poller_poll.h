#include <poll.h>

#define NN_POLLER_HAVE_ASYNC_ADD 0

struct nn_poller_hndl {
    int index;
};

struct nn_poller {
    int size;

    int index;

    int capacity;

    struct pollfd *pollset;

    struct nn_hndls_item {
        struct nn_poller_hndl *hndl;
        int prev;
        int next;
    } * hndls;
    int removed;
};
