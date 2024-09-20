#include "revolt/fetch.h"

const RevoltUser *revolt_fetch_rest_user(RevoltREST *rest, const char *user_id, RevoltErr *err) {
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
