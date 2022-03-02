#ifndef WS_H_INCLUDED
#define WS_H_INCLUDED

#include "nn.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NN_WS (-4)

#define NN_WS_MSG_TYPE (1)

#define NN_WS_MSG_TYPE_TEXT (0x01)
#define NN_WS_MSG_TYPE_BINARY (0x02)

#ifdef __cplusplus
}
#endif

#endif // !WS_H_INCLUDED
