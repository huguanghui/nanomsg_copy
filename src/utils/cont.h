#ifndef NN_CONT_INCLUDED
#define NN_CONT_INCLUDED

#include <stddef.h>

#define nn_cont(ptr, type, member)                                             \
    (ptr ? ((type *)(((char *)ptr) - offsetof(type, member))) : NULL)

#endif // !NN_CONT_INCLUDED
