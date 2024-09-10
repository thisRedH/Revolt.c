#define _DEFAULT_SOURCE
#define RVLTC_DEFINE_SLEEP 1
#include "revolt/client.h"
#include "revolt/rest.h"
#include "revolt/ws.h"

#define HERTZ       120
#define SLEEP_FOR   (int)(1000/(HERTZ))

Revolt *revolt_init(
    const char *token,
    revolt_bool is_bot,
    const char *api_url,
    const char *ws_url
) {
    Revolt *client = calloc(1, sizeof(Revolt));
    if (client == NULL)
        return NULL;

    client->rest = revolt_rest_new(api_url, token, is_bot);
    if (client->rest == NULL) {
        revolt_cleanup(client);
        return NULL;
    }

    /* TODO: get ws url from rest "/" (root) */

    client->ws = revolt_ws_new(ws_url, token);
    if (client->ws == NULL) {
        revolt_cleanup(client);
        return NULL;
    }

    return client;
}

void revolt_cleanup(Revolt* client) {
    if (client == NULL)
        return;

    revolt_stop(client);

    revolt_rest_delete(client->rest);
    revolt_ws_delete(client->ws);

    free(client);
}

RevoltErr revolt_run(Revolt* client) {
    RevoltErr res;

    if (client == NULL)
        return REVOLTE_INVAL;

    res = revolt_ws_connect(client->ws);
    if (res != REVOLTE_OK)
        return res;

    client->running = revolt_true;
    while (client->running) {
        res = revolt_ws_poll(client->ws);
        if (res != REVOLTE_OK)
            break;

        revolt_sleep(SLEEP_FOR);
    }

    return res;
}

void revolt_stop(Revolt* client) {
    if (client == NULL)
        return;

    client->running = revolt_false;
}
