#include "../nn.h"
#include "../inproc.h"
#include "../ipc.h"
#include "../tcp.h"

#include "../pair.h"
#include "../pubsub.h"
#include "../reqrep.h"
#include "../pipeline.h"
#include "../survey.h"
#include "../bus.h"
#include "../ws.h"

#include <string.h>

#define NN_SYM(sym, namespace, typ, unit)                                      \
    {                                                                          \
        sym, #sym, NN_NS_##namespace, NN_TYPE_##typ, NN_UNIT_##unit            \
    }

static const struct nn_symbol_properties sym_value_names[] = {
    NN_SYM(NN_NS_NAMESPACE, NAMESPACE, NONE, NONE),
    NN_SYM(NN_NS_VERSION, NAMESPACE, NONE, NONE),
};

const int SYM_VALUE_NAMES_LEN
    = (sizeof(sym_value_names) / sizeof(sym_value_names[0]));

const char *nn_symbol(int i, int *value) { return NULL; }

int nn_symbol_info(int i, struct nn_symbol_properties *buf, int buflen)
{
    return -1;
}
