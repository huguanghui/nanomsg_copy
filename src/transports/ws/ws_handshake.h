#ifndef NN_WS_HANDSHAKE_INCLUDED
#define NN_WS_HANDSHAKE_INCLUDED

#include "../../transport.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"
#include "../../aio/timer.h"

#define NN_WS_HANDSHAKE_OK (1)
#define NN_WS_HANDSHAKE_ERROR (2)
#define NN_WS_HANDSHAKE_STOPPED (3)

#define NN_WS_CLIENT (1)
#define NN_WS_SERVER (2)

#define NN_WS_HANDSHAKE_MAX_SIZE (4096)

#define NN_WS_HANDSHAKE_MAGIC_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define NN_WS_HANDSHAKE_TERMSEQ "\r\n\r\n"
#define NN_WS_HANDSHAKE_TERMSEQ_LEN strlen(NN_SW_HANDSHAKE_TERMSEQ)

#define NN_WS_HANDSHAKE_ACCEPT_KEY_LEN (28)

struct nn_ws_handshake {
    struct nn_fsm fsm;
    int state;
    int mode;
    struct nn_timer timer;
    int timeout;
    struct nn_usock *usock;
    struct nn_fsm_owner usock_owner;
    struct nn_pipebase *pipebase;
    const char *resource;
    const char *remote_host;
    char opening_hs[NN_WS_HANDSHAKE_MAX_SIZE];
    int retries;
    size_t recv_pos;
    size_t recv_len;
    const char *host;
    size_t host_len;
    const char *origin;
    size_t origin_len;
    const char *key;
    size_t key_len;
    const char *upgrade;
    size_t upgrade_len;
    const char *conn;
    size_t conn_len;
    const char *version;
    size_t version_len;
    const char *protocol;
    size_t protocol_len;
    const char *server;
    size_t server_len;
    const char *accept_key;
    size_t accept_key_len;
    char expected_accept_key[NN_WS_HANDSHAKE_ACCEPT_KEY_LEN + 1];
    const char *status_code;
    size_t status_code_len;
    const char *reason_phrase;
    size_t reason_phrase_len;
    const char *uri;
    size_t uri_len;
    const char *extensions;
    size_t extensions_len;
    int response_code;
    char response[512];
    struct nn_fsm_event done;
};

struct nn_ws_sp_map {
    int server;
    int client;
    const char *ws_sp;
};

void nn_ws_handshake_init(
    struct nn_ws_handshake *self, int src, struct nn_fsm *owner);
void nn_ws_handshake_term(struct nn_ws_handshake *self);

int nn_ws_handshake_isidle(struct nn_ws_handshake *self);
void nn_ws_handshake_start(struct nn_ws_handshake *self, struct nn_usock *usock,
    struct nn_pipebase *pipebase, int mode, const char *resouce,
    const char *host);
void nn_ws_handshake_stop(struct nn_ws_handshake *self);

#endif // !NN_WS_HANDSHAKE_INCLUDED
