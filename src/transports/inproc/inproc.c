#include "ins.h"
#include "binproc.h"
#include "cinproc.h"

#include "../../inproc.h"

#include <string.h>

static void nn_inproc_init(void);
static void nn_inproc_term(void);
static int nn_inproc_bind(struct nn_ep *);
static int nn_inproc_connect(struct nn_ep *);

struct nn_transport nn_inproc = {
    "inproc",
    NN_INPROC,
    nn_inproc_init,
    nn_inproc_term,
    nn_inproc_bind,
    nn_inproc_connect,
    NULL,
};

static void nn_inproc_init(void) { nn_ins_init(); }

static void nn_inproc_term(void) { nn_ins_term(); }

static int nn_inproc_bind(struct nn_ep *ep) { return nn_binproc_create(ep); }

static int nn_inproc_connect(struct nn_ep *ep) { return nn_cinproc_create(ep); }
