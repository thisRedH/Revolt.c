#include "revolt/http.h"

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

    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "Revolt.c by thisRedH");
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
    const char *auth_header,
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
    http->auth_header = auth_header;

    return http;
}

void revolt_http_cleanup(RevoltHTTP *http) {
    if (http == NULL) return;

    if (http->do_curl_glob) {
        curl_global_cleanup();
    }

    free((void *)http);
}

RevoltResponse *revolt_http_user(RevoltHTTP *http, const char *user_id) {
    char auth_buffer[128];
    char url_buffer[128];
    CURL *hnd;
    RevoltResponse *resp;
    struct curl_slist *slist;

    if (http == NULL) return NULL;

    hnd = curl_easy_init();
    if (hnd == NULL) return NULL;

    slist = NULL;

    snprintf(auth_buffer, sizeof(auth_buffer), "%s: %s", http->auth_header, http->token);
    auth_buffer[sizeof(auth_buffer) - 1] = '\0';
    slist = curl_slist_append(slist, auth_buffer);

    snprintf(url_buffer, sizeof(url_buffer), "%s/users/%s", http->api_url, user_id);
    url_buffer[sizeof(url_buffer) - 1] = '\0';

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_URL, url_buffer);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 512L);

    resp = revolt_request_perform(hnd);
    if (resp == NULL) return NULL;

    curl_slist_free_all(slist);
    curl_easy_cleanup(hnd);

    return resp;
}
