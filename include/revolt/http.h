#ifndef _REVOLTC_HTTP_H_INCLUDED_
#define _REVOLTC_HTTP_H_INCLUDED_

#include "common.h"
#include <curl/curl.h>

#define REVOLT_DEFAULT_API_URL "https://api.revolt.chat"

#ifdef __cplusplus
extern "C" {
#endif

struct RevoltResponseBytes {
    revolt_byte *bytes;
    size_t size;
};

typedef struct RevoltResponse {
    long status_code;
    struct RevoltResponseBytes header;
    struct RevoltResponseBytes body;
} RevoltResponse;

RevoltResponse *revolt_request_perform(CURL *hnd);
char *revolt_response_bytes_str(const struct RevoltResponseBytes *const data);

typedef struct RevoltHTTP {
    revolt_bool do_curl_glob;
    const char *token;
    const char *api_url;
    const char *auth_header;
} RevoltHTTP;

RevoltHTTP *revolt_http_init(const char *api_url, const char *token, const char *auth_header, revolt_bool do_curl_glob);
void revolt_http_cleanup(RevoltHTTP *http);

RevoltResponse *revolt_http_user(RevoltHTTP *http, const char *user_id);
#define revolt_http_user_me(http) (revolt_http_user((http), "@me"))

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_HTTP_H_INCLUDED_ */
