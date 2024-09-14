#ifndef _REVOLTC_REST_H_INCLUDED_
#define _REVOLTC_REST_H_INCLUDED_

#include <revolt/common.h>
#include <revolt/core/http.h>

REVOLTC_BEGIN_C_DECLS

#ifndef REVOLT_REST_DEFAULT_URL
#define REVOLT_REST_DEFAULT_URL "https://api.revolt.chat"
#endif

typedef struct RevoltREST {
    char *url;
    char *token;
    revolt_bool bot;
} RevoltREST;

REVOLTC_API RevoltREST *revolt_rest_new(const char *url, const char *token, revolt_bool bot);
REVOLTC_API void revolt_rest_delete(RevoltREST *rest);

RevoltErr revolt_rest_request(
    RevoltREST *rest,
    const char *path,
    enum RevoltcHTTPMethod method,
    const char **headers,
    size_t header_count,
    const char *body,
    RevoltcHTTPResponse *resp
);

#define revolt_rest_get0(rest, path, resp)      revolt_rest_request((rest),(path),REVOLTC_HTTP_GET,NULL,0,NULL,(resp))
#define revolt_rest_del0(rest, path, resp)      revolt_rest_request((rest),(path),REVOLTC_HTTP_DELETE,NULL,0,NULL,(resp))
#define revolt_rest_put0(rest, path, resp)      revolt_rest_request((rest),(path),REVOLTC_HTTP_PUT,NULL,0,NULL,(resp))

#define revolt_rest_get(rest, path, hdrs, hdr_count, body, resp)    revolt_rest_request((rest),(path),REVOLTC_HTTP_GET,(hdrs),(hdr_count),(body),(resp))
#define revolt_rest_del(rest, path, hdrs, hdr_count, body, resp)    revolt_rest_request((rest),(path),REVOLTC_HTTP_DELETE,(hdrs),(hdr_count),(body),(resp))
#define revolt_rest_post(rest, path, hdrs, hdr_count, body, resp)   revolt_rest_request((rest),(path),REVOLTC_HTTP_POST,(hdrs),(hdr_count),(body),(resp))
#define revolt_rest_put(rest, path, hdrs, hdr_count, body, resp)    revolt_rest_request((rest),(path),REVOLTC_HTTP_PUT,(hdrs),(hdr_count),(body),(resp))
#define revolt_rest_patch(rest, path, hdrs, hdr_count, body, resp)  revolt_rest_request((rest),(path),REVOLTC_HTTP_PATCH,(hdrs),(hdr_count),(body),(resp))

REVOLTC_API RevoltErr revolt_rest_fetch_user(RevoltREST *rest, const char *user_id, /*TODO RevoltUser*/RevoltcHTTPResponse *user);
#define revolt_rest_fetch_me(rest)          revolt_rest_fetch_user((rest), NULL)

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_REST_H_INCLUDED_ */
