#include "revolt/fetch.h"

RevoltResponse *revolt_fetch_user(Revolt *client, const char *user_id) {
    char buffer[96];
    RevoltResponse *resp;

    snprintf(buffer, sizeof(buffer), "users/%s", user_id);
    resp = revolt_http_get(client->http, buffer);
    return resp;
}
