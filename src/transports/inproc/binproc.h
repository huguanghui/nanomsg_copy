#ifndef NN_BINPROC_INCLUDED
#define NN_BINPROC_INCLUDED

#include "ins.h"

#include "../../transport.h"
#include "../../aio/fsm.h"
#include "../../utils/list.h"

struct nn_cinproc;

struct nn_binproc{
    struct nn_fsm fsm;
    int state;
    struct nn_ins_item item;
    struct nn_list sinprocs;
};

int nn_binproc_create(struct nn_ep *);

#endif // !NN_BINPROC_INCLUDED
