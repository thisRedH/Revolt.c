#include "revolt/rest.h"
#include <stdlib.h>

RevoltREST *revolt_rest_new(
    const char *url,
    const char *token,
    revolt_bool bot
) {
    RevoltREST *rest = calloc(1, sizeof(RevoltREST));
    rest->url = url;
    rest->token = token;
    rest->bot = bot;
    return rest;
}

void revolt_rest_delete(RevoltREST *rest) {
    if (rest == NULL)
        return;

    free(rest);
}

RevoltcHTTPResponse *revolt_rest_fetch_user(RevoltREST *rest, const char *user_id) {
    char *url = malloc(256);
    char *header = malloc(256);
    RevoltcHTTPResponse *resp;

    if (rest->bot) {
        snprintf(header, 256, "X-Bot-Token:%s", rest->token);
    } else {
        snprintf(header, 256, "X-Session-Token:%s", rest->token);
    }

    snprintf(url, 256, "%s/users/%s", rest->url, user_id);

    resp = revoltc_http_get(url, NULL, &header, 1);

    free(url);
    free(header);
    return resp;
}
