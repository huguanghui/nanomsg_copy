#ifndef NN_FD_INCLUDED
#define NN_FD_INCLUDED

#ifdef NN_HAVE_WINDOWS
#include "win.h"
typedef SOCKET nn_fd;
#else
typedef int nn_fd;
#endif //

#endif // !NN_FD_INCLUDED
