#include "revolt/websocket.h"

RevoltWebSocket *revolt_websocket_init(
    const char *ws_url,
    const char *token
) {
    RevoltWebSocket *ws = (RevoltWebSocket *)malloc(sizeof(RevoltWebSocket));
    if (ws == NULL) return NULL;

    ws->token = token;
    ws->ws_url = ws_url;

    return ws;
}

void revolt_websocket_cleanup(RevoltWebSocket *ws) {
    free(ws);
}
