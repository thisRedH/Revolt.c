#ifndef _REVOLTC_HTTP_H_INCLUDED_
#define _REVOLTC_HTTP_H_INCLUDED_

#include "revolt/common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum RevoltcHTTPMethod {
    REVOLTC_HTTP_GET = 0,
    REVOLTC_HTTP_HEAD,
    REVOLTC_HTTP_POST,
    REVOLTC_HTTP_PUT,
    REVOLTC_HTTP_DELETE,
    REVOLTC_HTTP_CONNECT,
    REVOLTC_HTTP_OPTIONS,
    REVOLTC_HTTP_TRACE,
    REVOLTC_HTTP_PATCH
};

const char *revoltc_http_method_str(enum RevoltcHTTPMethod method);

typedef struct RevoltcHTTPResponse {
    long status_code;
    revolt_byte *header;
    size_t header_len;
    revolt_byte *body;
    size_t body_len;
} RevoltcHTTPResponse;

void revoltc_http_response_cleanup(RevoltcHTTPResponse resp);

RevoltErr revoltc_http_request(
    enum RevoltcHTTPMethod method,
    const char *url,
    const char *useragent,
    const char **headers,
    size_t header_count,
    const char *body,
    size_t body_len,
    RevoltcHTTPResponse *response
);

#define revoltc_http_get(url, useragent, headers, header_count)         \
    revoltc_http_request(REVOLTC_HTTP_GET,(url),(useragent),(headers),(header_count),NULL,0)
#define revoltc_http_head(url, useragent, headers, header_count)        \
    revoltc_http_request(REVOLTC_HTTP_HEAD,(url),(useragent),(headers),(header_count),NULL,0)
#define revoltc_http_post(url, useragent, headers, header_count, data)  \
    revoltc_http_request(REVOLTC_HTTP_POST,(url),(useragent),(headers),(header_count),(data),-1)
#define revoltc_http_put(url, useragent, headers, header_count, data)   \
    revoltc_http_request(REVOLTC_HTTP_PUT,(url),(useragent),(headers),(header_count),(data),-1)
#define revoltc_http_delete(url, useragent, headers, header_count, data)\
    revoltc_http_request(REVOLTC_HTTP_DELETE,(url),(useragent),(headers),(header_count),(data),0)

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_HTTP_H_INCLUDED_ */
