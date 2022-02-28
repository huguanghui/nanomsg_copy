#ifndef NN_GLOBAL_INCLUDED
#define NN_GLOBAL_INCLUDED

const struct nn_transport *nn_global_transport(int id);

struct nn_pool *nn_global_getpool();
int nn_global_print_errors();

#endif // !NN_GLOBAL_INCLUDED
