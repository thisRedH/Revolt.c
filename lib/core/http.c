#include "revolt/core/http.h"
#define CURL_NO_OLDIES
#include <curl/curl.h>

#ifndef REVLOTC_USERAGENT
#define REVOLTC_USERAGENT "Revolt.c/0.1.0"
#endif

typedef struct WriteCallbackUserp {
    revolt_byte *data;
    size_t data_len;
} WriteCallbackUserp;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    WriteCallbackUserp *up = (WriteCallbackUserp*) userp;
    size_t realsize = size * nmemb;

    if (up == NULL)
        return 0;

    up->data = realloc(up->data, up->data_len + realsize + 1);
    if(up->data == NULL)
        return 0;

    memcpy(up->data + up->data_len, contents, realsize);

    up->data_len += realsize;
    up->data[up->data_len] = '\0';

    return realsize;
}

static RevoltcHTTPResponse *request_perform(CURL *hnd) {
    RevoltcHTTPResponse *response = malloc(sizeof(RevoltcHTTPResponse));
    WriteCallbackUserp header = {0};
    WriteCallbackUserp body = {0};

    if (response == NULL)
        return NULL;

    curl_easy_setopt(hnd, CURLOPT_HEADERFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_HEADERDATA, &header);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &body);

    if (curl_easy_perform(hnd) != CURLE_OK) {
        free(response);
        return NULL;
    }

    response->body = body.data;
    response->body_len = body.data_len;
    response->header = header.data;
    response->header_len = header.data_len;
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &(response->status_code));

    return response;
}

const char *revoltc_http_method_str(enum RevoltcHTTPMethod method) {
    switch (method) {
        case REVOLT_HTTP_GET:       return "GET";
        case REVOLT_HTTP_HEAD:      return "HEAD";
        case REVOLT_HTTP_POST:      return "POST";
        case REVOLT_HTTP_PUT:       return "PUT";
        case REVOLT_HTTP_DELETE:    return "DELETE";
        case REVOLT_HTTP_CONNECT:   return "CONNECT";
        case REVOLT_HTTP_OPTIONS:   return "OPTIONS";
        default:                    return "GET";
    }
}

RevoltcHTTPResponse *revoltc_http_request(
    enum RevoltcHTTPMethod method,
    const char *url,
    const char *useragent,
    char **headers,
    size_t header_count,
    const char *body,
    size_t body_len
) {
    RevoltcHTTPResponse *resp;
    struct curl_slist *slist = NULL;
    CURL *hnd;
    size_t i;

    if (url == NULL || (headers == NULL && header_count > 0))
        return NULL;

    if (useragent == NULL)
        useragent = REVOLTC_USERAGENT;

    hnd = curl_easy_init();
    if (hnd == NULL)
        return NULL;

    for (i = 0; i <= header_count; i++)
        slist = curl_slist_append(slist, headers[i]);

    curl_easy_setopt(hnd, CURLOPT_URL, url);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, useragent);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, revoltc_http_method_str(method));

    if (body != NULL) {
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, body);
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, body_len);
    }

    resp = request_perform(hnd);
    if (resp == NULL)
        return NULL;

    curl_slist_free_all(slist);
    curl_easy_cleanup(hnd);
    return resp;
}
