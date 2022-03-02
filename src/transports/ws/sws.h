#ifndef NN_SWS_INCLUDED
#define NN_SWS_INCLUDED

#include "../../transport.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "ws_handshake.h"

#include "../../utils/msg.h"
#include "../../utils/list.h"

#define NN_SWS_RETURN_ERROR (1)
#define NN_SWS_RETURN_CLOSE_HANDSHAKE (2)
#define NN_SWS_RETURN_STOPPED (3)

#define NN_SWS_FRAME_SIZE_INITIAL (2)
#define NN_SWS_FRAME_SIZE_PAYLOAD_0 (0)
#define NN_SWS_FRAME_SIZE_PAYLOAD_16 (2)
#define NN_SWS_FRAME_SIZE_PAYLOAD_63 (8)
#define NN_SWS_FRAME_SIZE_MASK (4)

#define NN_SWS_FRAME_BITMASK_FIN (0x80)
#define NN_SWS_FRAME_BITMASK_RSV1 (0x40)
#define NN_SWS_FRAME_BITMASK_RSV2 (0x20)
#define NN_SWS_FRAME_BITMASK_RSV3 (0x10)
#define NN_SWS_FRAME_BITMASK_OPCODE (0x0F)

#define NN_SWS_UTF8_MAX_CODEPOINT_LEN (4)

#define NN_SWS_FRAME_MAX_HDR_LEN (14)

#define NN_SWS_PAYLOAD_MAX_LENGTH (125)
#define NN_SWS_PAYLOAD_MAX_LEHGTH_16 (65535)
#define NN_SWS_PAYLOAD_MAX_LEHGTH_63 (0x9223372036854775807)
#define NN_SWS_PAYLOAD_FRAME_16 (0x7E)
#define NN_SWS_PAYLOAD_FRAME_63 (0x7F)

#define NN_SWS_CLOSE_CODE_LEN (2)

struct nn_sws {
    struct nn_fsm fsm;
    int state;
    uint8_t msg_type;
    int mode;
    struct nn_usock *usock;
    struct nn_sws_handshake handshaker;
    struct nn_sws_owner usock_owner;
    struct nn_pipebase pipebase;
    const char *resource;
    const char *remote_host;
    int instate;
    uint8_t inhdr[NN_SWS_FRAME_MAX_HDR_LEN];

    uint8_t opcode;
    uint8_t payload_ctl;
    uint8_t masked;
    uint8_t *mask;
    size_t ext_hdr_len;
    int is_final_frame;
    int is_control_frame;

    int continuing;

    uint8_t utf8_code_pt_fragment[NN_SWS_UTF8_MAX_CODEPOINT_LEN];
    size_t utf8_code_pt_fragment_len;

    int pings_sent;
    int pongs_sent;
    int pings_received;
    int pongs_received;

    struct nn_list inmsg_array;
    uint8_t *inmsg_current_chunk_buf;
    size_t inmsg_current_chunk_len;
    size_t inmsg_total_size;
    int inmsg_chunks;
    uint8_t inmsg_hdr;

    uint8_t inmsg_control[NN_SWS_PAYLOAD_MAX_LENGTH];
    char fail_msg[NN_SWS_PAYLOAD_MAX_LENGTH];
    size_t fail_msg_len;

    int outstate;
    uint8_t outhdr[NN_SWS_FRAME_MAX_HDR_LEN];
    struct nn_msg outmsg;
    struct nn_fsm_event done;
};

struct msg_chunk {
    struct nn_list_item item;
    struct nn_chunkref chunk;
};

void *nn_msg_chunk_new(size_t size, struct nn_list *msg_array);

void nn_msg_chunk_term(struct msg_chunk *it, struct nn_list *msg_array);

void nn_msg_array_term(struct nn_list *msg_array);

void nn_sws_init(struct nn_sws *self, int src, struct nn_ep *ep, struct nn_fsm *owner);
void nn_sws_term(struct nn_sws *self);

int nn_sws_isidle(struct nn_sws *self);
void nn_sws_start(struct nn_sws *self, struct nn_usock *usock, int mode, const char *resource, const char *host, uint8_t msg_type);
void nn_sws_stop(struct nn_sws *self);

#endif // !NN_SWS_INCLUDED
