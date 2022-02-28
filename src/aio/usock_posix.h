#include "fsm.h"
#include "worker.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

struct nn_usock {
    struct nn_fsm fsm;
    int state;

    struct nn_worker *worker;

    int s;
    struct nn_worker_fd wfd;

    struct {
        uint8_t *buf;
        size_t len;
        uint8_t *batch;
        size_t batch_len;
        size_t batch_pos;
        int *pfd;
    } in;

    struct {
        struct msghdr hdr;
        struct iovec iov[NN_USOCK_MAX_IOVCNT];
    } out;

    struct nn_worker_task task_connecting;
    struct nn_worker_task task_connected;
    struct nn_worker_task task_accept;
    struct nn_worker_task task_send;
    struct nn_worker_task task_recv;
    struct nn_worker_task task_stop;

    struct nn_fsm_event event_established;
    struct nn_fsm_event event_sent;
    struct nn_fsm_event event_received;
    struct nn_fsm_event event_error;

    struct nn_usock *asock;

    int errnum;
};
