#include "usock.h"

#if defined(NN_HAVE_WINDOWS)
#include "usock_win.inc"
#else
#include "usock_posix.inc"
#endif

intnn_usock_geterrno(struct nn_usock *self) { return self->errnum; }
