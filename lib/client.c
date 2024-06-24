#include "revolt/client.h"

Revolt *revolt_bot_easy_init(const char *bot_token) {
    return revolt_init(
        bot_token,
        revolt_true,
        REVOLT_DEFAULT_API_URL,
        REVOLT_DEFAULT_WS_URL
    );
}

Revolt *revolt_init(
    const char *token,
    revolt_bool is_bot,
    const char *api_url,
    const char *ws_url
) {
    Revolt *client = (Revolt *)malloc(sizeof(Revolt));
    if (client == NULL) return NULL;

    client->http = revolt_http_init(
        api_url,
        token,
        is_bot,
        revolt_true /* TODO: make aviable to lib user */
    );

    client->ws = revolt_websocket_init(ws_url, token);

    return client;
}

void revolt_cleanup(Revolt* client) {
    if (client == NULL) return;

    if (client->http != NULL) revolt_http_cleanup(client->http);
    if (client->ws != NULL) revolt_websocket_cleanup(client->ws);
    /* TODO: eventloop */
    /* TODO: threads */

    free(client);
}
