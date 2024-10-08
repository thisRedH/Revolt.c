#define REVOLTC_NAMESPACELESS_DEFINES 1
#include "revolt/core/websocket.h"
#include "revolt/core/util.h"
#define CURL_NO_OLDIES
#include <curl/curl.h>

#define CNTRL_FRAME(opc) (           \
    (opc) == REVOLTC_WS_OPC_CLOSE || \
    (opc) == REVOLTC_WS_OPC_PING  || \
    (opc) == REVOLTC_WS_OPC_PONG)

#define ILLEGAL_PAYLOAD_LEN(opc, len)   \
    ((len) > REVOLTC_WS_PAYLOAD_MAXS || \
    ((len) > REVOLTC_WS_PAYLOAD_MAXS_CNTRL && CNTRL_FRAME(opc)))

char *url_get_scheme(const char *url) {
    const char *p = strstr(url, "://");
    if_un (NILC(p))
        return NULL;

    return revoltc_util_str_dupn(url, p - url);
}

char *url_get_host(const char *url) {
    const char *p1;
    const char *p2;

    p1 = strstr(url, "://");
    if (NILC(p1))
        return NULL;

    p1 += 3;

    p2 = strpbrk(p1, " :/?#");
    if (NILC(p2))
        return revoltc_util_str_dup(p1);

    return revoltc_util_str_dupn(p1, p2 - p1);
}

char *url_get_path(const char *url) {
    const char *p1;
    const char *p2;

    p1 = strstr(url, "://");
    if (NILC(p1))
        return NULL;

    p1 += 3;

    p1 = strpbrk(p1, "/?");
    if (NILC(p1))
        return calloc(1, 1);

    if (*p1 == '/')
        p1++;

    p2 = strchr(p1, '#');
    if (NILC(p2))
        return revoltc_util_str_dup(p1);

    return revoltc_util_str_dupn(p1, p2 - p1);
}

char *url_normalize_scheme(const char *url, const char *fallback) {
    char *res;

    if (strncmp(url, "http://", 7) == 0 || strncmp(url, "https://", 8) == 0) {
        res = revoltc_util_str_dup(url);
    } else if (strncmp(url, "ws://", 5) == 0) {
        res = malloc(strlen(url) + 3);
        if (!NILC(res)) {
            strcpy(res, "http://");
            strcat(res, url + 5);
        }
    } else if (strncmp(url, "wss://", 6) == 0) {
        res = malloc(strlen(url) + 3);
        if (!NILC(res)) {
            strcpy(res, "https://");
            strcat(res, url + 6);
        }
    } else {
        if (strncmp(url, "://", 3) == 0) {
            res = malloc(strlen(url) + strlen(fallback) - 2);
            if (!NILC(res)) {
                strcpy(res, fallback);
                strcat(res, url + 3);
            }
        } else {
            res = malloc(strlen(url) + strlen(fallback) + 1);
            if (!NILC(res)) {
                strcpy(res, fallback);
                strcat(res, url);
            }
        }
    }
    return res;
}

static int wait_on_socket_curl(curl_socket_t sockfd, revolt_bool for_recv, long timeout_ms) {
    fd_set infd, outfd, errfd;
    struct timeval tv;
    int res;

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (int)(timeout_ms % 1000) * 1000;

    FD_ZERO(&infd);
    FD_ZERO(&outfd);
    FD_ZERO(&errfd);

    FD_SET(sockfd, &errfd);

    if(for_recv) {
        FD_SET(sockfd, &infd);
    } else {
        FD_SET(sockfd, &outfd);
    }

    res = select((int)sockfd + 1, &infd, &outfd, &errfd, &tv);
    return res;
}

/* TODO rewrite */
static revolt_bool http_header_valid(
    const char *header,
    const char *response_sec_ws_key
) {
    uint16_t i = 0, lc = 0;
    uint8_t f = 0;
    char *start, *end;
    char *lines[128] = {NULL};

    /* split lines */
    start = (char*) header;
    end = start;

    for (lc = 0; *start && lc < sizeof(lines); lc++) {
        while (*end && (*end != '\r' && *(end + 1) != '\n')) end++;

        i = (uint16_t)(end - start);
        if (i > 0) {
            lines[lc] = malloc(i + 1);
            if (NULL == lines[lc]) goto end;

            (void) strncpy(lines[lc], start, i);
            lines[lc][i] = '\0';
        }

        while (*end && (*end == '\r' || *end == '\n')) end++;
        start = end;
    }

    /* validate lines */
    if (lc < 4) goto end;
    if (0 != strcmp(lines[0], "HTTP/1.1 101 Switching Protocols")) goto end;

    for (i = 1; i < lc; i++) {
        revoltc_util_str_tolower(lines[i]);
        if (0 == strncmp(lines[i], "upgrade", 7)) {
            if (NULL != strstr(lines[i] + 7, "websocket"))          f |= 1;
        } else if (0 == strncmp(lines[i], "connection", 10)) {
            if (NULL != strstr(lines[i] + 10, "upgrade"))           f |= 2;
        } else if (0 == strncmp(lines[i], "sec-websocket-accept", 20)) {
            if (NULL != strstr(lines[i] + 20, response_sec_ws_key)) f |= 4;
        }
    }

end:
    for (i = 0; i < lc; i++)
        free(lines[i]);
    return (f == 7); /* 7 = 1 | 2 | 4 */
}

static RevoltErr ws_send_raw(RevoltcWS *ws, long timeout_ms, revolt_byte *data, uint64_t data_len);
static RevoltErr ws_recv_raw(RevoltcWS *ws, long timeout_ms, revolt_byte *buf, uint64_t buf_size, uint64_t *nrecv);

/* TODO generate Sec-WebSocket-Key instead of having one predefined */

static RevoltErr ws_handshake_send(RevoltcWS *ws) {
    const char *const fmt =
        "GET /%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhpc1JlZEgvUmV2b2x0Qw==\r\n"
        "Sec-WebSocket-Version: 13\r\n\r\n";
    RevoltErr res;
    char *path, *host;
    revolt_byte *buf;
    uint64_t n;

    host = url_get_host(ws->url);
    if_un (NILC(host))
        return REVOLTE_NOMEM;

    path = url_get_path(ws->url);
    if_un (NILC(path)) {
        free(host);
        return REVOLTE_NOMEM;
    }

    buf = malloc(strlen(fmt) + strlen(path) + strlen(host) + 1);
    if_un (NILC(buf)) {
        free(host);
        free(path);
        return REVOLTE_NOMEM;
    }

    n = sprintf((char*) buf, fmt, path, host);

    free(host);
    free(path);

    res = ws_send_raw(ws, 6000L, buf, n);

    free(buf);
    return res;
}

static RevoltErr ws_handshake_recv(RevoltcWS *ws) {
    /* static const char *const response_sec_key = "B93ot2qde0NCXSyAgIkgWocnbB8="; */
    static const char *const response_sec_key = "b93ot2qde0ncxsyagikgwocnbb8=";
    RevoltErr res;
    size_t needed = 100; /* min switching protocol ws */
    revolt_byte *buf;
    size_t buf_size = 256;
    uint64_t n, nrecv = 0;

    buf = malloc(buf_size);
    if_un (NILC(buf))
        return REVOLTE_NOMEM;

    for (;;) {
        if (buf_size < needed) {
            buf_size = needed + 1;
            buf = realloc(buf, buf_size);
            if_un (NILC(buf))
                return REVOLTE_NOMEM;
        }

        res = ws_recv_raw(ws, 6000L, buf + nrecv, needed - nrecv, &n);
        nrecv += n;
        if_un (res != REVOLTE_OK) {
            free(buf);
            return res;
        }

        if (nrecv < 4)
            continue;

        if (strncmp((char*) buf + nrecv - 4, "\r\n\r\n", 4) == 0) {
            break;
        } else if (buf[nrecv - 3] == '\r') {
            needed += 1;
        } else if (buf[nrecv - 2] == '\r') {
            needed += 2;
        } else if (buf[nrecv - 1] == '\r') {
            needed += 3;
        } else {
            needed += 4;
        }
    };

    buf[nrecv] = '\0';
    if (!http_header_valid((char*) buf, response_sec_key))
        res = REVOLTE_PARSE;

    free(buf);
    return res;
}

static RevoltErr ws_handshake(RevoltcWS *ws) {
    RevoltErr res = ws_handshake_send(ws);
    if_un (res != REVOLTE_OK)
        return res;
    return ws_handshake_recv(ws);
}

RevoltcWSFrame revoltc_ws_frame__new(
    enum RevoltcWSOpcodes opc,
    revolt_byte *payload,
    uint64_t payload_len
) {
    RevoltcWSFrame frame = {0};
    frame.header.fin = 1;
    frame.header.opcode = opc;
    frame.header.masked = 1;
    frame.header.masking_key = rand(); /* TODO use other rng */
    frame.header.payload_len = payload_len;
    frame.payload = payload;
    return frame;
}

RevoltcWS *revoltc_ws_new(const char *url) {
    RevoltcWS *ws;

    if_un (NILC(url))
        return NULL;

    ws = malloc(sizeof(RevoltcWS));
    if_un (NILC(ws))
        return NULL;

    ws->hnd = NULL;
    ws->connected = revolt_false;
    ws->url = url_normalize_scheme(url, "https://");
    if_un (NILC(ws->url)) {
        free(ws);
        return NULL;
    }

    return ws;
}

void revoltc_ws_delete(RevoltcWS *ws) {
    (void) revoltc_ws_disconnect_ok(ws);
    free(ws->url);
    free(ws);
}

RevoltErr revoltc_ws_connect(RevoltcWS *ws) {
    CURLcode res;
    RevoltErr err;

    if_un (NILC(ws))
        return REVOLTE_INVAL;

    if (ws->connected)
        return REVOLTE_OK;

    ws->hnd = curl_easy_init();
    if (NILC(ws->hnd))
        return REVOLTE_CURL_INIT;

    curl_easy_setopt(ws->hnd, CURLOPT_URL, ws->url);
    curl_easy_setopt(ws->hnd, CURLOPT_CONNECT_ONLY, 1L);
    curl_easy_setopt(ws->hnd, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

    res = curl_easy_perform(ws->hnd);
    if (res != CURLE_OK)
        return REVOLTE_CURL | res;

    err = ws_handshake(ws);
    if (err != REVOLTE_OK)
        return err;

    ws->connected = revolt_true;
    return REVOLTE_OK;
}

RevoltErr revoltc_ws_disconnect(
    RevoltcWS *ws,
    enum RevoltcWSStatusCodes status,
    revolt_byte *msg,
    size_t msg_len
) {
    revolt_byte *msg_bytes = NULL;
    RevoltErr res;

    if_un (NILC(ws))
        return REVOLTE_INVAL;

    if (!ws->connected)
        goto end;

    msg_bytes = malloc(msg_len + 2);
    if_un (NILC(msg_bytes))
        return REVOLTE_NOMEM;

    revoltc_util_u16_bytes_be((uint16_t) status, msg_bytes);
    if (msg_len > 0 && !NILC(msg))
        (void) memcpy(msg_bytes + 2, msg, msg_len);

    res = revoltc_ws_send(ws, 3000L, revoltc_ws_frame__new(
        REVOLTC_WS_OPC_CLOSE,
        msg_bytes,
        msg_len + 2
    ));
    if (res != REVOLTE_OK)
        goto end;

    res = revoltc_ws_recv(ws, 3000L, NULL, NULL);
    if (res != REVOLTE_OK)
        goto end;

    /* TODO check close frame */

end:
    curl_easy_cleanup(ws->hnd);
    ws->hnd = NULL;
    ws->connected = revolt_false;

    free(msg_bytes);
    return REVOLTE_OK;
}

void revoltc_ws_mask(const revolt_byte key[4], revolt_byte *data, uint64_t len) {
    uint64_t i;
    for (i = 0; i < len; ++i)
        data[i] ^= key[i % 4];
}

static RevoltErr ws_send_raw(
    RevoltcWS *ws,
    long timeout_ms,
    revolt_byte *data,
    uint64_t data_len
) {
    CURLcode res;
    curl_socket_t sockfd;
    uint64_t nsent = 0;
    size_t n;

    res = curl_easy_getinfo(ws->hnd, CURLINFO_ACTIVESOCKET, &sockfd);
    if (res != CURLE_OK)
        return REVOLTE_CURL | res;

    do {
        /* sending more then once isnt tested as i cant produce it */
        res = curl_easy_send(ws->hnd, data + nsent, data_len - nsent, &n);
        nsent += (uint64_t) n;

        if(res == CURLE_AGAIN) {
            if (!wait_on_socket_curl(sockfd, revolt_false, timeout_ms)) {
                return REVOLTE_TIMEOUT;
            }
        }
    } while (res == CURLE_AGAIN);

    if (res != CURLE_OK)
        return REVOLTE_CURL | res;

    if (nsent < data_len)
        return REVOLTE_SEND;

    return REVOLTE_OK;
}

RevoltErr revoltc_ws_send_raw(
    RevoltcWS *ws,
    long timeout_ms,
    revolt_byte *data,
    uint64_t data_len
) {
    if_un (NILC(ws) || NILC(data))
        return REVOLTE_INVAL;

    if (!ws->connected)
        return REVOLTE_NOT_CONNECTED;

    return ws_send_raw(ws, timeout_ms, data, data_len);
}

RevoltErr revoltc_ws_send(RevoltcWS *ws, long timeout_ms, RevoltcWSFrame frame) {
    RevoltErr res;
    revolt_byte *buf;
    int8_t header_len;

    if_un (NILC(ws))
        return REVOLTE_INVAL;

    if (!ws->connected)
        return REVOLTE_NOT_CONNECTED;

    buf = malloc(REVOLTC_WS_HEADER_MAXS + frame.header.payload_len);
    if_un (NILC(buf))
        return REVOLTE_NOMEM;

    res = revoltc_ws_serialize_header(frame.header, buf, &header_len);

    if (res == REVOLTE_OK) {
        if (frame.header.payload_len > 0) {
            (void) memcpy(
                buf + header_len,
                frame.payload,
                frame.header.payload_len
            );
            if (frame.header.masked) {
                revoltc_ws_mask(
                    buf + header_len - 4,
                    buf + header_len,
                    header_len + frame.header.payload_len
                );
            }
        }

        res = ws_send_raw(
            ws,
            timeout_ms,
            buf,
            header_len + frame.header.payload_len
        );
    }

    free(buf);
    return res;
}

/* TODO add internal buffer so that we dont have to be 100% exact with
 * the amount of data we are recieving in other functions */

static RevoltErr ws_recv_raw(
    RevoltcWS *ws,
    long timeout_ms,
    revolt_byte *buf,
    uint64_t buf_size,
    uint64_t *nrecv
) {
    CURLcode res;
    curl_socket_t sockfd;
    size_t n;
    *nrecv = 0;

    res = curl_easy_getinfo(ws->hnd, CURLINFO_ACTIVESOCKET, &sockfd);
    if (res != CURLE_OK)
        return REVOLTE_CURL | res;

    do {
        res = curl_easy_recv(ws->hnd, buf + (*nrecv), buf_size - (*nrecv), &n);
        (*nrecv) += (uint64_t) n;

        if ((*nrecv) == 0) {
            if (res == CURLE_AGAIN) { /* No data, dont block */
                return REVOLTE_OK;
            } else {
                return REVOLTE_DISCONNECT;
            }
        }

        if(res == CURLE_AGAIN) {
            if (!wait_on_socket_curl(sockfd, revolt_true, timeout_ms)) {
                return REVOLTE_TIMEOUT;
            }
        }
    } while (res == CURLE_AGAIN);

    if (res != CURLE_OK)
        return REVOLTE_CURL | res;

    return REVOLTE_OK;
}

RevoltErr revoltc_ws_recv_raw(
    RevoltcWS *ws,
    long timeout_ms,
    revolt_byte *buf,
    uint64_t buf_size,
    uint64_t *nrecv
) {
    if_un (NILC(ws) || NILC(nrecv) || NILC(buf) || ZEROC(buf_size))
        return REVOLTE_INVAL;

    if (!ws->connected)
        return REVOLTE_NOT_CONNECTED;

    return ws_recv_raw(ws, timeout_ms, buf, buf_size, nrecv);
}

/* TODO make fn revolt_ws_recv smaller (maybe split?) */

RevoltErr revoltc_ws_recv(
    RevoltcWS *ws,
    long timeout_ms,
    RevoltcWSFrame *frame,
    uint64_t *bytes_recv
) {
    RevoltErr ret = REVOLTE_UNKNOWN;
    RevoltcWSHeader header = {0};
    revolt_byte *buf;
    size_t buf_size = 256;
    uint64_t needed = REVOLTC_WS_HEADER_MINS;
    uint64_t nrecv = 0;
    uint8_t header_len;
    size_t n;

    if (!NILC(bytes_recv))
        *bytes_recv = 0;

    if (!NILC(frame))
        (void) memset(frame, 0, sizeof(*frame));

    if_un (NILC(ws))
        return REVOLTE_INVAL;

    if (!ws->connected)
        return REVOLTE_NOT_CONNECTED;

    buf = malloc(buf_size);
    if_un (NILC(buf))
        return REVOLTE_NOMEM;

    for (;;) {
        if (buf_size < needed) {
            buf_size = needed;
            buf = realloc(buf, buf_size);
            if_un (NILC(buf))
                return REVOLTE_NOMEM;
        }

        ret = ws_recv_raw(ws, timeout_ms, buf + nrecv, needed - nrecv, &n);
        nrecv += (uint64_t) n;
        if (ret != REVOLTE_OK || nrecv == 0)
            goto end;

        ret = revoltc_ws_deserialize_header(buf, nrecv, &header, &header_len);
        if (ret == REVOLTE_INSUFFICIENT_DATA) {
            needed += 2;
        } else if (ret == REVOLTE_OK) {
            if (header.payload_len == 0)
                break;

            if ((nrecv - header_len) >= header.payload_len)
                break;

            needed = header_len + header.payload_len;
        } else {
            goto end;
        }
    }

    if (nrecv < needed) {
        ret = REVOLTE_INSUFFICIENT_DATA;
        goto end;
    }

    if (frame != NULL) {
        frame->header = header;
        if (header.payload_len > 0) {
            frame->payload = malloc(header.payload_len + 1);
            memcpy(frame->payload, buf + header_len, header.payload_len);
            frame->payload[header.payload_len] = '\0';
        } else {
            frame->payload = NULL;
        }
    }

    ret = REVOLTE_OK;

end:
    if (!NILC(bytes_recv))
        *bytes_recv = nrecv;

    free(buf);
    return ret;
}

/* data_buf The buffer must be allocated by the caller. (min `REVOLT_WS_HEADER_MAXS`) */
RevoltErr revoltc_ws_serialize_header(
    RevoltcWSHeader header,
    revolt_byte *data_buf,
    int8_t *data_len
) {
    int8_t pos = 0;

    if_un (NILC(data_buf) || NILC(data_len))
        return REVOLTE_INVAL;

    if_un (ILLEGAL_PAYLOAD_LEN(header.opcode, header.payload_len))
        return REVOLTE_PARSE;

    data_buf[pos++] =
        (header.fin  << 7) |
        (header.rsv1 << 6) |
        (header.rsv2 << 5) |
        (header.rsv3 << 4) |
        (header.opcode);

    data_buf[pos] = header.masked << 7;

    if (header.payload_len < 126) {
        data_buf[pos++] |= header.payload_len;
    } else if (header.payload_len <= UINT16_MAX) {
        data_buf[pos++] |= 126;

        revoltc_util_u16_bytes_be(header.payload_len, data_buf + pos);
        pos += 2;
    } else {
        data_buf[pos++] |= 127;

        revoltc_util_u64_bytes_be(header.payload_len, data_buf + pos);
        pos += 8;
    }

    if (header.masked) {
        revoltc_util_u32_bytes_be(header.masking_key, data_buf + pos);
        pos += 4;
    }

    *data_len = pos;
    return REVOLTE_OK;
}

RevoltErr revoltc_ws_deserialize_header(
    const revolt_byte *data,
    size_t data_len,
    RevoltcWSHeader *header,
    uint8_t *header_len
) {
    int8_t pos = 0;

    if_un (NILC(data) || NILC(header) || NILC(header_len))
        return REVOLTE_INVAL;

    *header_len = 0;

    if (data_len < REVOLTC_WS_HEADER_MINS)
        return REVOLTE_INSUFFICIENT_DATA;

    header->fin          = (data[pos] >> 7) & 0x01;
    header->rsv1         = (data[pos] >> 6) & 0x01;
    header->rsv2         = (data[pos] >> 5) & 0x01;
    header->rsv3         = (data[pos] >> 4) & 0x01;
    header->opcode       = (data[pos] & 0x0F);
    pos++;

    header->masked       = data[pos] >> 7;

    /* payload len */
    header->payload_len  = data[pos] & 0x7F;
    pos++;

    if ((data_len - pos) == 0) {
        if (header->payload_len < 126) {
            *header_len = pos;
            return REVOLTE_OK;
        } else {
            return REVOLTE_INSUFFICIENT_DATA;
        }
    }

    if (header->payload_len == 126) {
        if ((data_len - pos) < 2)
            return REVOLTE_INSUFFICIENT_DATA;

        header->payload_len = revoltc_util_bytes_be_u16(data + pos);
        pos += 2;
    } else if (header->payload_len == 127) {
        if ((data_len - pos) < 8)
            return REVOLTE_INSUFFICIENT_DATA;

        header->payload_len = revoltc_util_bytes_be_u64(data + pos);
        pos += 8;
    }

    if (ILLEGAL_PAYLOAD_LEN(header->opcode, header->payload_len))
        return REVOLTE_PARSE;

    /* masking key */
    if (header->masked) {
        if ((data_len - pos) < 4)
            return REVOLTE_INSUFFICIENT_DATA;

        header->masking_key = revoltc_util_bytes_be_u32(data + pos);
        pos += 4;
    } else {
        header->masking_key = 0;
    }

    *header_len = pos;
    return REVOLTE_OK;
}
