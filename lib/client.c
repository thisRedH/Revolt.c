#include "revolt/client.h"
#include "websocket.h"

Revolt *revolt_init(
    const char *token,
    revolt_bool is_bot,
    const char *api_url,
    const char *ws_url
) {
    Revolt *client = malloc(sizeof(Revolt));
    if (client == NULL)
        return NULL;

    client->http = revolt_http_init(
        api_url,
        token,
        is_bot,
        revolt_true /* TODO: make aviable to lib user */
    );
    if (client->http == NULL) {
        revolt_cleanup(client);
        return NULL;
    }

    client->ws = revolt_ws_new(ws_url);
    if (client->ws == NULL) {
        revolt_cleanup(client);
        return NULL;
    }

    return client;
}

void revolt_cleanup(Revolt* client) {
    if (client == NULL)
        return;

    if (client->http != NULL)
        revolt_http_cleanup(client->http);
    if (client->ws != NULL)
        revolt_ws_delete(client->ws);

    /* TODO: eventloop */
    /* TODO: threads */

    free(client);
}
