#include "revolt/core/http.h"
#define CURL_NO_OLDIES
#include <curl/curl.h>

#ifndef REVLOTC_USERAGENT
#define REVOLTC_USERAGENT "Revolt.c"
#endif

const char *revoltc_http_method_str(enum RevoltcHTTPMethod method) {
    switch (method) {
        case REVOLTC_HTTP_GET:      return "GET";
        case REVOLTC_HTTP_HEAD:     return "HEAD";
        case REVOLTC_HTTP_POST:     return "POST";
        case REVOLTC_HTTP_PUT:      return "PUT";
        case REVOLTC_HTTP_DELETE:   return "DELETE";
        case REVOLTC_HTTP_CONNECT:  return "CONNECT";
        case REVOLTC_HTTP_OPTIONS:  return "OPTIONS";
        case REVOLTC_HTTP_TRACE:    return "TRACE";
        case REVOLTC_HTTP_PATCH:    return "PATCH";
        default:                    return "GET";
    }
}

void revoltc_http_response_cleanup(RevoltcHTTPResponse resp) {
    free(resp.header);
    free(resp.body);
}

struct WriteCallbackUserp {
    char *data;
    size_t data_len;
};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    struct WriteCallbackUserp *up = userp;
    size_t realsize = size * nmemb;

    if (up == NULL)
        return 0;

    up->data = realloc(up->data, up->data_len + realsize + 1);
    if(up->data == NULL)
        return 0;

    (void) memcpy(up->data + up->data_len, contents, realsize);

    up->data_len += realsize;
    up->data[up->data_len] = '\0';

    return realsize;
}

RevoltErr request_perform(CURL *hnd, RevoltcHTTPResponse *response) {
    RevoltcHTTPResponse resp = {0};
    struct WriteCallbackUserp header = {0};
    struct WriteCallbackUserp body = {0};
    CURLcode res;

    curl_easy_setopt(hnd, CURLOPT_HEADERFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_HEADERDATA, &header);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &body);

    if ((res = curl_easy_perform(hnd)) != CURLE_OK)
        return REVOLTE_CURL | res;

    resp.body = body.data;
    resp.body_len = body.data_len;
    resp.header = header.data;
    resp.header_len = header.data_len;
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &(resp.status_code));

    if (response != NULL) {
        *response = resp;
    } else {
        revoltc_http_response_cleanup(resp);
    }

    return REVOLTE_OK;
}

RevoltErr revoltc_http_request(
    enum RevoltcHTTPMethod method,
    const char *url,
    const char *useragent,
    const char **headers,
    size_t header_count,
    const char *body,
    size_t body_len,
    RevoltcHTTPResponse *response
) {
    size_t i;
    RevoltErr res;
    struct curl_slist *slist = NULL;
    CURL *hnd;

    if (url == NULL || (headers == NULL && header_count > 0))
        return REVOLTE_INVAL;

    if (useragent == NULL)
        useragent = REVOLTC_USERAGENT;

    hnd = curl_easy_init();
    if (hnd == NULL)
        return REVOLTE_CURL_INIT;

    for (i = 0; i < header_count; i++)
        slist = curl_slist_append(slist, headers[i]);

    curl_easy_setopt(hnd, CURLOPT_URL, url);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, useragent);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, revoltc_http_method_str(method));

    if (body != NULL) {
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, body);
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, body_len);
    }

    res = request_perform(hnd, response);

    curl_slist_free_all(slist);
    curl_easy_cleanup(hnd);
    return res;
}
