#ifndef _REVOLTC_WEBSOCKET_H_INCLUDED_
#define _REVOLTC_WEBSOCKET_H_INCLUDED_

#include "common.h"

#define REVOLT_DEFAULT_WS_URL "wss://ws.revolt.chat/"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RevoltWebSocket {
    const char* token;
    const char* ws_url;
} RevoltWebSocket;

RevoltWebSocket *revolt_websocket_init(const char *ws_url, const char *token);
void revolt_websocket_cleanup(RevoltWebSocket *ws);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_WEBSOCKET_H_INCLUDED_ */
