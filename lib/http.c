#include "revolt/http.h"

/* can be set at lib compile time (if needed) */
#ifndef RVLTC_USERAGENT
#define RVLTC_USERAGENT "Revolt.c/0.1.0"
#endif

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;

    struct RevoltResponseBytes *ret = (struct RevoltResponseBytes *)userp;
    if (ret == NULL) return 0;

    ret->bytes = (revolt_byte *)realloc(ret->bytes, ret->size + realsize);
    if(ret->bytes == NULL) return 0;

    memcpy(&(ret->bytes[ret->size]), contents, realsize);

    ret->size += realsize;
    return realsize;
}

RevoltResponse *revolt_request_perform(CURL *hnd) {
    CURLcode code = CURLE_OK;
    RevoltResponse *resp = (RevoltResponse *)calloc(1, sizeof(RevoltResponse));
    if (resp == NULL) return NULL;

    curl_easy_setopt(hnd, CURLOPT_HEADERFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_HEADERDATA, (void *)&(resp->header));
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&(resp->body));

    code = curl_easy_perform(hnd);
    if (code != CURLE_OK) {
        free((void *)resp);
        return NULL;
    }

    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &(resp->status_code));

    return resp;
}

char *revolt_response_bytes_str(const struct RevoltResponseBytes *const data) {
    char *str;

    if (data == NULL || data->bytes == NULL) return NULL;

    str = (char *)malloc(data->size + 1);
    if (str == NULL) return NULL;

    memcpy(str, data->bytes, data->size);
    str[data->size] = '\0';

    return str;
}

RevoltHTTP* revolt_http_init(
    const char *api_url,
    const char *token,
    revolt_bool is_bot,
    revolt_bool do_curl_glob
) {
    RevoltHTTP *http = (RevoltHTTP *)malloc(sizeof(RevoltHTTP));
    if (http == NULL) return NULL;

    if (do_curl_glob) {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    http->do_curl_glob = do_curl_glob;
    http->api_url = api_url;
    http->token = token;
    http->is_bot = is_bot;

    return http;
}

void revolt_http_cleanup(RevoltHTTP *http) {
    if (http == NULL) return;

    if (http->do_curl_glob) {
        curl_global_cleanup();
    }

    free((void *)http);
}

RVLTC_INLINE char *format_header_field(const char *header, const char *value, size_t bufsize) {
    char *buf = (char *)malloc(bufsize * sizeof(char));
    snprintf(buf, bufsize, "%s: %s", header, value);
    buf[bufsize -1] = '\0';
    return buf;
}

#define RVLTC_URL_BUFS 256

RevoltResponse *revolt_http_fetch(
    RevoltHTTP *http,
    const char *method,
    const char *endpoint,
    const char *body
) {
    CURL *hnd;
    struct curl_slist *slist = NULL;
    char *buffer;
    RevoltResponse *resp;

    if (http == NULL) return NULL;

    hnd = curl_easy_init();
    if (hnd == NULL) return NULL;

    if (http->is_bot) {
        buffer = format_header_field("X-Bot-Token", http->token, 128);
    } else {
        buffer = format_header_field("X-Session-Token", http->token, 128);
    }

    slist = curl_slist_append(slist, buffer);
    slist = curl_slist_append(slist, "Content-Type: application/json");

    free(buffer);
    buffer = (char *)malloc(RVLTC_URL_BUFS * sizeof(char));
    snprintf(buffer, RVLTC_URL_BUFS, "%s/%s", http->api_url, endpoint);
    buffer[RVLTC_URL_BUFS -1] = '\0';

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(hnd, CURLOPT_URL, buffer);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, RVLTC_USERAGENT);
    /* curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, body); */

    resp = revolt_request_perform(hnd);
    if (resp == NULL) return NULL;

    curl_slist_free_all(slist);
    curl_easy_cleanup(hnd);
    free(buffer);

    return resp;
}
