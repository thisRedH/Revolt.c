#ifndef _REVOLTC_WS_H_INCLUDED_
#define _REVOLTC_WS_H_INCLUDED_

#include <revolt/common.h>
#include <revolt/core/websocket.h>

REVOLTC_BEGIN_C_DECLS

#ifndef REVOLT_WS_DEFAULT_URL
#define REVOLT_WS_DEFAULT_URL "wss://ws.revolt.chat?version=1&format=json"
#endif

typedef struct RevoltWS RevoltWS;

enum RevoltWSEvent {
    REVOLT_WS_EV_OPEN = 0,
    REVOLT_WS_EV_CLOSE,
    REVOLT_WS_EV_RECV_STR,
    REVOLT_WS_EV_SEND_STR,
    REVOLT_WS_EV_RECV_BIN,
    REVOLT_WS_EV_SEND_BIN,
    REVOLT_WS_EV_RECV_PING,
    REVOLT_WS_EV_SEND_PING,
    REVOLT_WS_EV_RECV_PONG,
    REVOLT_WS_EV_SEND_PONG
};

typedef RevoltErr (*revolt_ws_callback)(
    enum RevoltWSEvent ev,
    revolt_byte *data,
    size_t data_len,
    RevoltWS *ws,
    void *userp
);

typedef struct RevoltWSCallback {
    revolt_ws_callback fn;
    void *userp;
} RevoltWSCallback;

typedef struct RevoltWSCallbacks {
    RevoltWSCallback open;
    RevoltWSCallback close;
    RevoltWSCallback str;
    RevoltWSCallback bin;
    RevoltWSCallback ping;
    RevoltWSCallback pong;
} RevoltWSCallbacks;

typedef struct RevoltWStx {
    enum RevoltcWSOpcodes opc;
    revolt_byte *data;
    uint64_t data_len;
} RevoltWStx;

struct RevoltWS {
    RevoltcWS *ws;
    char *token;
    RevoltWSCallbacks callbacks;
    RevoltWStx *tx;
    size_t tx_count;
    size_t tx_size;
};

REVOLTC_API RevoltWS *revolt_ws_new(const char *url, const char *token);
REVOLTC_API void revolt_ws_delete(RevoltWS *ws);

REVOLTC_API RevoltErr revolt_ws_connect(RevoltWS *ws);
REVOLTC_API void revolt_ws_disconnect(RevoltWS *ws);

REVOLTC_API RevoltErr revolt_ws_send(RevoltWS *ws, enum RevoltcWSOpcodes opc, revolt_byte *data, uint64_t data_len);
#define revolt_ws_send_str(ws, str)                 revolt_ws_send((ws),REVOLTC_WS_OPC_TEXT,(revolt_byte*)(str),strlen((str)))
#define revolt_ws_send_bin(ws, data, data_len)      revolt_ws_send((ws),REVOLTC_WS_OPC_BINARY,(data),strlen((data_len)))

REVOLTC_API RevoltErr revolt_ws_set_on(RevoltWS *ws, enum RevoltWSEvent ev, revolt_ws_callback fn, void *userp);
#define revolt_ws_set_on_open(ws, fn, userp)        revolt_ws_set_on((ws),REVOLT_WS_EV_OPEN,(fn),(userp))
#define revolt_ws_set_on_close(ws, fn, userp)       revolt_ws_set_on((ws),REVOLT_WS_EV_CLOSE,(fn),(userp))
#define revolt_ws_set_on_str(ws, fn, userp)         revolt_ws_set_on((ws),REVOLT_WS_EV_RECV_STR,(fn),(userp))
#define revolt_ws_set_on_bin(ws, fn, userp)         revolt_ws_set_on((ws),REVOLT_WS_EV_RECV_BIN,(fn),(userp))
#define revolt_ws_set_on_pong(ws, fn, userp)        revolt_ws_set_on((ws),REVOLT_WS_EV_RECV_PONG,(fn),(userp))
#define revolt_ws_set_on_ping(ws, fn, userp)        revolt_ws_set_on((ws),REVOLT_WS_EV_RECV_PING,(fn),(userp))

REVOLTC_API RevoltErr revolt_ws_poll(RevoltWS *ws);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_WS_H_INCLUDED_ */
