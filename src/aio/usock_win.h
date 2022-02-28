#include "fsm.h"
#include "worker.h"

#include "../utils/win.h"

struct nn_usock {
    struct nn_fsm fsm;
    int state;

    union {
        SOCKET s;
        HANDLE p;
    };

    int isaccepted;

    struct nn_worker_op in;
    struct nn_worker_op out;

    int domain;
    int type;
    int protocol;

    struct nn_fsm_event event_established;
    struct nn_fsm_event event_sent;
    struct nn_fsm_event event_received;
    struct nn_fsm_event event_error;

    struct nn_usock *asock;

    void *ainfo;

    struct sockaddr_un pipename;

    void *pipesendbuf;

    SECURITY_ATTRIBUTES *sec_attr;

    int outbuffersz;
    int inbuffersz;

    int errnum;
};
