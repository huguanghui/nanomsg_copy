#include <pthread.h>

struct nn_thread
{
    nn_thread_routine *routine;
    void *arg;
    pthread_t handle;
};
