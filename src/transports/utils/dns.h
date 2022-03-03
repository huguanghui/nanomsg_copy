#ifndef NN_DNS_INCLUDED
#define NN_DNS_INCLUDED

#include "../../aio/fsm.h"

#include <stddef.h>

int nn_dns_check_hostname(const char *name, size_t namelen);

#define NN_DNS_DONE (1)
#define NN_DNS_STOPPED (2)

#if defined NN_HAVE_GETADDRINFO_A && !defined NN_DISABLE_GETADDRINFO_A
#include "dns_getaddrinfo_a.h"
#else
#include "dns_getaddrinfo.h"
#endif

struct nn_dns_result {
    int error;
    struct sockaddr_storage addr;
    size_t addrlen;
};

void nn_dns_init(struct nn_dns *self, int src, struct nn_fsm *owner);
void nn_dns_term(struct nn_dns *self);

int nn_dns_isidle(struct nn_dns *self);
void nn_dns_start(struct nn_dns *self, const char *addr, size_t addrlen,
    int ipv4only, struct nn_dns_result *result);
void nn_dns_stop(struct nn_dns *self);

#endif // !NN_DNS_INCLUDED
