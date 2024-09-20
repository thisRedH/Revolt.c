#include "revolt/fetch.h"
#include "revolt/error.h"

RevoltUser *revolt_fetch_rest_user(
    RevoltREST *rest,
    const char *user_id,
    RevoltErr *err
) {
    RevoltcHTTPResponse resp = {0};
    RevoltUser *user;
    RevoltErr res;
    char *path;

    if (err == NULL)
        err = &res;

    if (user_id == NULL)
        user_id = "@me";

    user = malloc(sizeof(*user));
    if (user == NULL) {
        *err = REVOLTE_NOMEM;
        return NULL;
    }

    path = malloc(strlen("users/") + strlen(user_id) + 1);
    if (path == NULL) {
        *err = REVOLTE_NOMEM;
        return NULL;
    }

    (void) sprintf(path, "users/%s", user_id);

    *err = revolt_rest_get0(rest, path, &resp);

    if (*err == REVOLTE_OK)
        *err = revolt_user_deserialize_json(resp.body, user);

    free(path);
    revoltc_http_response_cleanup(resp);
    return user;
}

RevoltChannel *revolt_fetch_rest_channel(
    RevoltREST *rest,
    const char *channel_id,
    RevoltErr *err
) {
    RevoltcHTTPResponse resp = {0};
    RevoltChannel *channel;
    RevoltErr res;
    char *path;

    if (err == NULL)
        err = &res;

    if (channel_id == NULL) {
        *err = REVOLTE_INVAL;
        return NULL;
    }

    channel = malloc(sizeof(*channel));
    if (channel == NULL) {
        *err = REVOLTE_NOMEM;
        return NULL;
    }

    path = malloc(strlen("channels/") + strlen(channel_id) + 1);
    if (path == NULL) {
        *err = REVOLTE_NOMEM;
        return NULL;
    }

    (void) sprintf(path, "channels/%s", channel_id);

    *err = revolt_rest_get0(rest, path, &resp);

    if (*err == REVOLTE_OK)
        *err = revolt_channel_deserialize_json(resp.body, channel);

    free(path);
    revoltc_http_response_cleanup(resp);
    return channel;
}
