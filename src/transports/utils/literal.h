#ifndef NN_LITERAL_INCLUDED
#define NN_LITERAL_INCLUDED

#if defined(NN_HAVE_WINDOWS)
#include "../../utils/win.h"
#else
#include <sys/socket.h>
#endif

#include <stddef.h>

int nn_literal_resolve(const char *addr, size_t addrlen, int ipv4only,
    struct sockaddr_storage *result, size_t *resultlen);

#endif // !NN_LITERAL_INCLUDED
