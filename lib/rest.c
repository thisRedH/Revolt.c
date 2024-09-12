#include "revolt/rest.h"
#include "revolt/core/util.h"

RevoltREST *revolt_rest_new(
    const char *url,
    const char *token,
    revolt_bool bot
) {
    size_t len;
    RevoltREST *rest;

    if (url == NULL || token == NULL || (len = strlen(url)) <= 0)
        return NULL;

    rest = calloc(1, sizeof(RevoltREST));
    if (rest == NULL)
        return NULL;

    rest->url = revoltc_util_str_dupn(url, len);
    rest->token = revoltc_util_str_dup(token);
    rest->bot = bot;

    if (rest->url == NULL || rest->token == NULL) {
        revolt_rest_delete(rest);
        return NULL;
    }

    if (rest->url[len - 1] == '/')
        rest->url[len - 1] = '\0';

    return rest;
}

void revolt_rest_delete(RevoltREST *rest) {
    if (rest == NULL)
        return;

    free(rest->url);
    free(rest->token);
    free(rest);
}

RevoltErr revolt_rest_request(
    RevoltREST *rest,
    const char *path,
    enum RevoltcHTTPMethod method,
    const char **headers,
    size_t header_count,
    const char *body,
    RevoltcHTTPResponse *resp
) {
    size_t i;
    char *url;
    const char **hdrs;
    const char *auth_hdr;
    RevoltErr res;

    if (rest == NULL || path == NULL)
        return REVOLTE_INVAL;

    if (path == NULL)
        path = "";

    if (headers == NULL)
        header_count = 0;

    auth_hdr = rest->bot ? "X-Bot-Token" : "X-Session-Token";

    url = malloc(strlen(rest->url) + strlen(path) + 2);
    if (url == NULL)
        return REVOLTE_NOMEM;

    hdrs = malloc((header_count + 1) * sizeof(*hdrs));
    if (hdrs == NULL) {
        free(url);
        return REVOLTE_NOMEM;
    }

    hdrs[0] = malloc(strlen(auth_hdr) + strlen(rest->token) + 2);
    if (hdrs[0] == NULL) {
        free(url);
        free(hdrs);
        return REVOLTE_NOMEM;
    }

    if (path[0] == '/')
        path++;

    (void) sprintf(url, "%s/%s", rest->url, path);
    (void) sprintf((char*) hdrs[0], "%s:%s", auth_hdr, rest->token);

    for (i = 0; i < header_count; i++)
        hdrs[i + 1] = headers[i];

    res = revoltc_http_request(
        method,
        url,
        NULL,
        hdrs,
        header_count + 1,
        body,
        body ? strlen(body) : 0,
        resp
    );

    free(url);
    free((void*) hdrs[0]);
    free(hdrs);
    return res;
}

RevoltErr revolt_rest_fetch_user(RevoltREST *rest, const char *user_id, RevoltcHTTPResponse *user) {
    RevoltcHTTPResponse resp = {0};
    RevoltErr res;
    char *path;

    if (rest == NULL || user == NULL)
        return REVOLTE_INVAL;

    if (user_id == NULL)
        user_id = "@me";

    path = malloc(strlen("users/") + strlen(user_id) + 1);
    if (path == NULL)
        return REVOLTE_NOMEM;

    (void) sprintf(path, "users/%s", user_id);

    res = revolt_rest_rget0(rest, path, &resp);
    if (res == REVOLTE_OK) {
        *user = resp;
    } else {
        (void) memset(user, 0, sizeof(*user));
    }

    free(path);
    return res;
}
