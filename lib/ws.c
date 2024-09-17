#include "revolt/ws.h"
#include "revolt/core/util.h"

RevoltWS *revolt_ws_new(const char *url, const char *token) {
    RevoltWS *ws;

    if (url == NULL || token == NULL)
        return NULL;

    ws = calloc(1, sizeof(*ws));
    if (ws == NULL)
        return NULL;

    ws->ws = revoltc_ws_new(url);
    if (ws->ws == NULL) {
        revolt_ws_delete(ws);
        return NULL;
    }

    ws->token = revoltc_util_str_dup(token);
    if (ws->token == NULL) {
        revolt_ws_delete(ws);
        return NULL;
    }

    ws->tx_size = 128;
    ws->tx = malloc(ws->tx_size);
    if (ws->tx == NULL) {
        revolt_ws_delete(ws);
        return NULL;
    }

    return ws;
}

void revolt_ws_delete(RevoltWS *ws) {
    if (ws == NULL)
        return;

    free(ws->token);
    free(ws->tx);
    revoltc_ws_delete(ws->ws);

    free(ws);
}

RevoltErr revolt_ws_connect(RevoltWS *ws) {
    const char *fmt = "{\"type\":\"Authenticate\",\"token\":\"%s\"}";
    RevoltErr res;
    char *str;

    if (ws == NULL)
        return REVOLTE_INVAL;

    res = revoltc_ws_connect(ws->ws);
    if (res != REVOLTE_OK)
        return res;

    str = malloc(strlen(fmt) + strlen(ws->token));
    if (str == NULL)
        return REVOLTE_NOMEM;

    (void) sprintf(str, fmt, ws->token);

    res = revoltc_ws_send_str(ws->ws, 2000L, str);

    free(str);
    return res;
}

void revolt_ws_disconnect(RevoltWS *ws) {
    if (ws == NULL)
        return ;

    revoltc_ws_disconnect_ok(ws->ws);
}

RevoltErr revolt_ws_send(
    RevoltWS *ws,
    enum RevoltcWSOpcodes opc,
    revolt_byte *data,
    uint64_t data_len
) {
    if (ws == NULL)
        return REVOLTE_INVAL;

    if (ws->tx_count >= ws->tx_size)
        return REVOLTE_LIMIT;

    ws->tx[ws->tx_count].opc = opc;
    ws->tx[ws->tx_count].data = data;
    ws->tx[ws->tx_count].data_len = data_len;
    ws->tx_count++;

    return REVOLTE_OK;
}

RevoltErr revolt_ws_set_on(
    RevoltWS *ws,
    enum RevoltWSEvent ev,
    revolt_ws_callback fn,
    void *userp
) {
    if (ws == NULL)
        return REVOLTE_INVAL;

    switch (ev) {
        case REVOLT_WS_EV_OPEN:
            ws->callbacks.open.fn = fn;
            ws->callbacks.open.userp = userp;
            break;
        case REVOLT_WS_EV_CLOSE:
            ws->callbacks.close.fn = fn;
            ws->callbacks.close.userp = userp;
            break;
        case REVOLT_WS_EV_RECV_STR:
        case REVOLT_WS_EV_SEND_STR:
            ws->callbacks.str.fn = fn;
            ws->callbacks.str.userp = userp;
            break;
        case REVOLT_WS_EV_RECV_BIN:
        case REVOLT_WS_EV_SEND_BIN:
            ws->callbacks.bin.fn = fn;
            ws->callbacks.bin.userp = userp;
            break;
        case REVOLT_WS_EV_RECV_PING:
        case REVOLT_WS_EV_SEND_PING:
            ws->callbacks.ping.fn = fn;
            ws->callbacks.ping.userp = userp;
            break;
        case REVOLT_WS_EV_RECV_PONG:
        case REVOLT_WS_EV_SEND_PONG:
            ws->callbacks.pong.fn = fn;
            ws->callbacks.pong.userp = userp;
            break;
        default:
            return REVOLTE_INVAL;
    }

    return REVOLTE_OK;
}

static RevoltErr on_close(RevoltWS *ws, RevoltcWSFrame rx) {
    RevoltErr res = REVOLTE_OK;

    revoltc_ws_disconnect_ok(ws->ws);

    if (ws->callbacks.close.fn != NULL) {
        res = ws->callbacks.close.fn(
            REVOLT_WS_EV_CLOSE,
            rx.payload,
            rx.header.payload_len,
            NULL,
            ws->callbacks.close.userp
        );
    }

    revolt_ws_disconnect(ws);
    return res;
}

static RevoltErr on_ping(RevoltWS *ws, RevoltcWSFrame rx) {
    RevoltErr res = REVOLTE_OK;

    res = revoltc_ws_send_pong(ws->ws, 250L, rx.payload, rx.header.payload_len);
    if (res != REVOLTE_OK)
        return res;

    if (ws->callbacks.ping.fn != NULL) {
        res = ws->callbacks.ping.fn(
            REVOLT_WS_EV_RECV_PING,
            rx.payload,
            rx.header.payload_len,
            ws,
            ws->callbacks.ping.userp
        );
    }

    return res;
}

static RevoltErr on_pong(RevoltWS *ws, RevoltcWSFrame rx) {
    RevoltErr res = REVOLTE_OK;

    if (ws->callbacks.pong.fn != NULL) {
        res = ws->callbacks.pong.fn(
            REVOLT_WS_EV_RECV_PONG,
            rx.payload,
            rx.header.payload_len,
            ws,
            ws->callbacks.pong.userp
        );
    }

    return res;
}

static RevoltErr on_text(RevoltWS *ws, RevoltcWSFrame rx) {
    RevoltErr res = REVOLTE_OK;

    if (ws->callbacks.str.fn != NULL) {
        res = ws->callbacks.str.fn(
            REVOLT_WS_EV_RECV_STR,
            rx.payload,
            rx.header.payload_len,
            ws,
            ws->callbacks.str.userp
        );
    }

    return res;
}

static RevoltErr on_binary(RevoltWS *ws, RevoltcWSFrame rx) {
    RevoltErr res = REVOLTE_OK;

    if (ws->callbacks.bin.fn != NULL) {
        res = ws->callbacks.bin.fn(
            REVOLT_WS_EV_RECV_BIN,
            rx.payload,
            rx.header.payload_len,
            ws,
            ws->callbacks.bin.userp
        );
    }

    return res;
}

RevoltErr revolt_ws_poll(RevoltWS *ws) {
    uint64_t rxn = 0;
    RevoltcWSFrame rx = {0};
    RevoltErr res;

    if (ws == NULL)
        return REVOLTE_INVAL;

    if (ws->tx_count > 0) {
        ws->tx_count--;
        res = revoltc_ws_send(ws->ws, 1000L, revoltc_ws_frame__new(
            ws->tx[ws->tx_count].opc,
            ws->tx[ws->tx_count].data,
            ws->tx[ws->tx_count].data_len
        ));
        if (res != REVOLTE_OK)
            return res;

        /* TODO: dispatch send event callbacks */
    }

    res = revoltc_ws_recv(ws->ws, 1000L, &rx, &rxn);
    if (res == REVOLTE_OK && rxn > 0) {
        switch (rx.header.opcode) {
            case REVOLTC_WS_OPC_CLOSE:
                on_close(ws, rx);
                break;
            case REVOLTC_WS_OPC_PING:
                on_ping(ws, rx);
                break;
            case REVOLTC_WS_OPC_PONG:
                on_pong(ws, rx);
                break;
            case REVOLTC_WS_OPC_TEXT:
                on_text(ws, rx);
                break;
            case REVOLTC_WS_OPC_BINARY:
                on_binary(ws, rx);
                break;
            default:
                /* TODO: impl all frames */
                break;
        }

        free(rx.payload);
    } else if (res == REVOLTE_DISCONNECT || res == REVOLTE_TIMEOUT) {
        revolt_ws_disconnect(ws);
        return revolt_ws_connect(ws);
    } else if (res == REVOLTE_PARSE) {
        /*TODO: add reson to disconnect*/
        revoltc_ws_disconnect(ws->ws, REVOLTC_WS_CLOSE_PROTOCOL_ERR, NULL, 0);
        revolt_ws_disconnect(ws);
        return revolt_ws_connect(ws);
    } else if (res != REVOLTE_OK) {
        return res;
    }

    return REVOLTE_OK;
}
