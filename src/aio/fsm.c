#include "fsm.h"
// #include "ctx.h"

#include "../utils/err.h"
#include "../utils//attr.h"

#include <stddef.h>

#define NN_FSM_STATE_IDLE 1
#define NN_FSM_STATE_ACTIVE 2
#define NN_FSM_STATE_STOPPING 3
