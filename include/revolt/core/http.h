#ifndef _REVOLTC_HTTP_H_INCLUDED_
#define _REVOLTC_HTTP_H_INCLUDED_

#include "revolt/common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum RevoltcHTTPMethod {
    REVOLT_HTTP_GET = 0,
    REVOLT_HTTP_HEAD,
    REVOLT_HTTP_POST,
    REVOLT_HTTP_PUT,
    REVOLT_HTTP_DELETE,
    REVOLT_HTTP_CONNECT,
    REVOLT_HTTP_OPTIONS
};

const char *revoltc_http_method_str(enum RevoltcHTTPMethod method);

typedef struct RevoltcHTTPResponse {
    long status_code;
    revolt_byte *header;
    size_t header_len;
    revolt_byte *body;
    size_t body_len;
} RevoltcHTTPResponse;

RevoltcHTTPResponse *revoltc_http_request(
    enum RevoltcHTTPMethod method,
    const char *url,
    const char *useragent,
    char **headers,
    size_t header_count,
    const char *body,
    size_t body_len
);

#define revoltc_http_get(url, useragent, headers, header_count) \
    revoltc_http_request(REVOLT_HTTP_GET, (url), (useragent), (headers), (header_count), NULL, 0)

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_HTTP_H_INCLUDED_ */
