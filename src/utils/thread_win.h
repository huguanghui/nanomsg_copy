#include "win.h"

struct nn_thread
{
    nn_thread_routine *routine;
    void *arg;
    HANDLE handle;
};
