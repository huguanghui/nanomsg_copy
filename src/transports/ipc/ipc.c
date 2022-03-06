#include "bipc.h"
#include "cipc.h"

#include "../../ipc.h"

#include "../../utils/err.h"
#include "../../utils/alloc.h"
#include "../../utils/fast.h"
#include "../../utils/cont.h"

#include <string.h>
#if defined(NN_HAVE_WINDOWS)
#include "../../utils/win.h"
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

struct nn_ipc_optset {
    struct nn_optset base;
    void *sec_attr;
    int outbuffersz;
    int inbuffersz;
};

static void nn_ipc_optset_destroy(struct nn_optset *self);
static int nn_ipc_optset_setopt(
    struct nn_optset *self, int option, const void *optval, size_t optvallen);
static int nn_ipc_optset_getopt(
    struct nn_optset *self, int option, void *optval, size_t *optvallen);
static const struct nn_optset_vfptr nn_ipc_optset_vfptr
    = { nn_ipc_optset_destroy, nn_ipc_optset_setopt, nn_ipc_optset_getopt };

static int nn_ipc_bind(struct nn_ep *ep);
static int nn_ipc_connect(struct nn_ep *ep);
static struct nn_optset *nn_ipc_optset(void);

struct nn_transport nn_ipc = {
    "ipc",
    NN_IPC,
    NULL,
    NULL,
    nn_ipc_bind,
    nn_ipc_connect,
    nn_ipc_optset,
};

static int nn_ipc_bind(struct nn_ep *ep) { return nn_bipc_create(ep); }

static int nn_ipc_connect(struct nn_ep *ep) { return nn_cipc_create(ep); }

static struct nn_optset *nn_ipc_optset()
{
    struct nn_ipc_optset *optset;

    optset = nn_alloc(sizeof(struct nn_ipc_optset), "optset (ipc)");
    alloc_assert(optset);
    optset->base.vfptr = &nn_ipc_optset_vfptr;

    optset->sec_attr = NULL;
    optset->outbuffersz = 4096;
    optset->inbuffersz = 4096;
    return &optset->base;
}

static void nn_ipc_optset_destroy(struct nn_optset *self)
{
    struct nn_ipc_optset *optset;

    optset = nn_cont(self, struct nn_ipc_optset, base);
    nn_free(optset);
}

static int nn_ipc_optset_setopt(
    struct nn_optset *self, int option, const void *optval, size_t optvallen)
{
    struct nn_ipc_optset *optset;

    optset = nn_cont(self, struct nn_ipc_optset, base);
    if (optvallen < sizeof(int)) {
        return -EINVAL;
    }

    switch (option) {
    case NN_IPC_SEC_ATTR: {
        optset->sec_attr = (void *)optval;
        return 0;
    }
    case NN_IPC_OUTBUFSZ: {
        optset->outbuffersz = *(int *)optval;
        return 0;
    }
    case NN_IPC_INBUFSZ: {
        optset->inbuffersz = *(int *)optval;
        return 0;
    }
    default: {
        return -ENOPROTOOPT;
    }
    }
}

static int nn_ipc_optset_getopt(
    struct nn_optset *self, int option, void *optval, size_t *optvallen)
{
    struct nn_ipc_optset *optset;

    optset = nn_cont(self, struct nn_ipc_optset, base);

    switch (option) {
    case NN_IPC_SEC_ATTR: {
        memcpy(optval, &optset->sec_attr, sizeof(optset->sec_attr));
        *optvallen = sizeof(optset->sec_attr);
        return 0;
    }
    case NN_IPC_OUTBUFSZ: {
        *(int *)optval = optset->outbuffersz;
        *optvallen = sizeof(int);
        return 0;
    }
    case NN_IPC_INBUFSZ: {
        *(int *)optval = optset->inbuffersz;
        *optvallen = sizeof(int);
        return 0;
    }
    default: {
        return -ENOPROTOOPT;
    }
    }
}
