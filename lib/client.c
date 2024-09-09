#include "revolt/client.h"

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

    client->ws = NULL;
    /* if (client->ws == NULL) {
        revolt_cleanup(client);
        return NULL;
    }
*/
    return client;
}

void revolt_cleanup(Revolt* client) {
    if (client == NULL)
        return;

    if (client->rest != NULL)
        revolt_rest_delete(client->rest);

    /* if (client->ws != NULL)
        revoltc_ws_delete(client->ws); */

    free(client);
}
