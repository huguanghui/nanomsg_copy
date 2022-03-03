#if defined NN_HAVE_WINDOWS
#include "../../utils/win.h"
#else
#include <sys/socket.h>
#endif

struct nn_dns {
    struct nn_fsm fsm;
    int state;
    struct nn_dns_result *result;
    struct nn_fsm_event done;
};
