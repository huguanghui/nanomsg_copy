#ifndef NN_CINPROC_INCLUDED
#define NN_CINPROC_INCLUDED

#include "ins.h"
#include "sinproc.h"

#include "../../transport.h"
#include "../../aio/fsm.h"
#include "../../utils/list.h"

struct nn_cinproc {
    struct nn_fsm fsm;
    int state;
    struct nn_ins_item item;
    struct nn_list sinprocs;
};

int nn_cinproc_create(struct nn_ep *);

#endif // !NN_CINPROC_INCLUDED
