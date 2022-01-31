#include "nn.h"
#include <stdio.h>

int nn_errno(void)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

const char *nn_strerror(int errnum)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return "";
}

const char *nn_symbol(int i, int *value)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return "";
}

int nn_symbol_info(int i, struct nn_symbol_properties *buf, int buflen)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

void nn_term(void) { printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__); }

void *nn_allocmsg(size_t size, int type)
{
    printf("[HGH-TIVE][%s %d]\n", __FUNCTION__, __LINE__);
    return NULL;
}

void *nn_reallocmsg(void *msg, size_t size)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return NULL;
}

void nn_freemsg(void *msg)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
}

struct nn_cmsghdr *nn_cmsg_nxthdr_(
    const struct nn_msghdr *mhdr, const struct nn_cmsghdr *cmsg)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return NULL;
}

int nn_socket(int domain, int protocol)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_close(int s)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_setsockopt(
    int s, int level, int option, const void *optval, size_t optvallen)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_getsockopt(int s, int level, int option, void *optval, size_t *optvallen)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_bind(int s, const char *addr)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_connect(int s, const char *addr)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_shutdown(int s, int how)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_send(int s, const void *buf, size_t len, int flags)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_recv(int s, void *buf, size_t len, int flags)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_sendmsg(int s, const struct nn_msghdr *msghdr, int flags)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_recvmsg(int s, struct nn_msghdr *msghdr, int flags)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_poll(struct nn_pollfd *fds, int nfds, int timeout)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

int nn_device(int s1, int s2)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}

uint64_t nn_get_statistic(int s, int stat)
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    return 0;
}
