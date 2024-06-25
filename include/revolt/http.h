#ifndef _REVOLTC_HTTP_H_INCLUDED_
#define _REVOLTC_HTTP_H_INCLUDED_

#include "revolt/common.h"
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

/**
 * \brief Converts a RevoltResponseBytes struct to a null-terminated string
 * 
 * \param data 
 * \return char* Has to be freed by the caller
 */
char *revolt_response_bytes_str(const struct RevoltResponseBytes *const data);

typedef struct RevoltHTTP {
    revolt_bool do_curl_glob;
    revolt_bool is_bot;
    const char *token;
    const char *api_url;
} RevoltHTTP;

RevoltHTTP *revolt_http_init(const char *api_url, const char *token, revolt_bool is_bot, revolt_bool do_curl_glob);
void revolt_http_cleanup(RevoltHTTP *http);

RevoltResponse *revolt_http_request(RevoltHTTP *http, const char *method, const char *path, const char *body, size_t body_size);
#define revolt_http_get(http, path) revolt_http_request((http), "GET", (path), NULL, 0)

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_HTTP_H_INCLUDED_ */
