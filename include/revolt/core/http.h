#ifndef _REVOLTC_CORE_HTTP_H_INCLUDED_
#define _REVOLTC_CORE_HTTP_H_INCLUDED_

#include <revolt/core/defines.h>
#include <revolt/core/types.h>
#include <revolt/error.h>

REVOLTC_BEGIN_C_DECLS

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
    char *header;
    size_t header_len;
    char *body;
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
    revoltc_http_request(REVOLTC_HTTP_DELETE,(url),(useragent),(headers),(header_count),(data),-1)

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CORE_HTTP_H_INCLUDED_ */
