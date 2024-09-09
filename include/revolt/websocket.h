#ifndef _REVOLTC_WEBSOCKET_H_INCLUDED_
#define _REVOLTC_WEBSOCKET_H_INCLUDED_

#include "revolt/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
    %x0 denotes a continuation frame
    %x1 denotes a text frame
    %x2 denotes a binary frame
    %x3-7 are reserved for further non-control frames
    %x8 denotes a connection close
    %x9 denotes a ping
    %xA denotes a pong
    %xB-F are reserved for further control frames
 */
enum RevoltWSOpcodes {
    REVOLT_WS_OPC_CONTINUATION  = 0x0,
    REVOLT_WS_OPC_TEXT          = 0x1,
    REVOLT_WS_OPC_BINARY        = 0x2,
    REVOLT_WS_OPC_RESV_3        = 0x3,
    REVOLT_WS_OPC_RESV_4        = 0x4,
    REVOLT_WS_OPC_RESV_5        = 0x5,
    REVOLT_WS_OPC_RESV_6        = 0x6,
    REVOLT_WS_OPC_RESV_7        = 0x7,
    REVOLT_WS_OPC_CLOSE         = 0x8,
    REVOLT_WS_OPC_PING          = 0x9,
    REVOLT_WS_OPC_PONG          = 0xA,
    REVOLT_WS_OPC_RESV_B        = 0xB,
    REVOLT_WS_OPC_RESV_C        = 0xC,
    REVOLT_WS_OPC_RESV_D        = 0xD,
    REVOLT_WS_OPC_RESV_E        = 0xE,
    REVOLT_WS_OPC_RESV_F        = 0xF
};

enum RevoltWSStatusCodes {
    REVOLT_WS_CLOSE_NORMAL              = 1000,
    REVOLT_WS_CLOSE_GOING_AWAY          = 1001,
    REVOLT_WS_CLOSE_PROTOCOL_ERR        = 1002,
    REVOLT_WS_CLOSE_UNSUPPORTED_DATA    = 1003,
    REVOLT_WS_CLOSE_RESV_1004           = 1004,
    REVOLT_WS_STATUS_NO_CODE_RECV       = 1005, /* not allowed in a close Frame */
    REVOLT_WS_STATUS_CLOSED_ABNORMALLY  = 1006, /* not allowed in a close Frame */
    REVOLT_WS_CLOSE_INVALID_DATA        = 1007,
    REVOLT_WS_CLOSE_POLICY_VIOLATION    = 1008,
    REVOLT_WS_CLOSE_MESSAGE_TOO_BIG     = 1009,
    REVOLT_WS_CLOSE_UNSUPPORTED_EXT     = 1010,
    REVOLT_WS_CLOSE_INTERNAL_SERVER_ERR = 1011,
    REVOLT_WS_STATUS_TLS_HANDSHAKE_ERR  = 1015, /* not allowed in a close Frame */
    REVOLT_WS_STATUS_IANA_START         = 3000,
    REVOLT_WS_STATUS_IANA_END           = 3999,
    REVOLT_WS_STATUS_PRIVATE_START      = 4000,
    REVOLT_WS_STATUS_PRIVATE_END        = 4999
};

/*
     0                   1                   2                   3
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-------+-+-------------+-------------------------------+
    |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
    |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
    |N|V|V|V|       |S|             |   (if payload len==126/127)   |
    | |1|2|3|       |K|             |                               |
    +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
    |     Extended payload length continued, if payload len == 127  |
    + - - - - - - - - - - - - - - - +-------------------------------+
    |                               |Masking-key, if MASK set to 1  |
    +-------------------------------+-------------------------------+
    | Masking-key (continued)       |          Payload Data         |
    +-------------------------------- - - - - - - - - - - - - - - - +
    :                     Payload Data continued ...                :
    + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
    |                     Payload Data continued ...                |
    +---------------------------------------------------------------+
 */

#define REVOLT_WS_HEADER_MAXS                   14
#define REVOLT_WS_HEADER_MINS                   2
#define REVOLT_WS_PAYLOAD_MINS                  0
#define REVOLT_WS_PAYLOAD_MAXS                  INT64_MAX
#define REVOLT_WS_PAYLOAD_MAXS_CNTRL            125 /* limit for control frame payload */
#define REVOLT_WS_FRAME_MAXS                    ((uint64_t)REVOLT_WS_PAYLOAD_MAXS + REVOLT_WS_HEADER_MAXS)
#define REVOLT_WS_FRAME_MINS                    (REVOLT_WS_HEADER_MINS + REVOLT_WS_PAYLOAD_MINS)

typedef struct RevoltWSHeader {
    unsigned int fin    :1;
    unsigned int rsv1   :1;
    unsigned int rsv2   :1;
    unsigned int rsv3   :1;
    enum RevoltWSOpcodes opcode;
    unsigned int masked :1;
    uint32_t masking_key;
    uint64_t payload_len;
} RevoltWSHeader;

typedef struct RevoltWSFrame {
    RevoltWSHeader header;
    revolt_byte *payload;
} RevoltWSFrame;

/**
 * Just a helper fn mainly for `revolt_ws_send_`... macros.
 * Since C89 dosn't have Compound Literals.
 */
RevoltWSFrame revolt_ws_frame__new(enum RevoltWSOpcodes opc, revolt_byte *payload, uint64_t payload_len);

typedef struct RevoltWS {
    /*CURL*/void *hnd;
    char *url;
    revolt_bool connected;
} RevoltWS;

RevoltWS *revolt_ws_new(const char *url);
void revolt_ws_delete(RevoltWS *ws);

RevoltErr revolt_ws_connect(RevoltWS *ws);
RevoltErr revolt_ws_disconnect(RevoltWS *ws, enum RevoltWSStatusCodes status, revolt_byte *msg, size_t msg_len);
#define revolt_ws_disconnect_str(ws, status, str)       revolt_ws_disconnect((ws),(status),(revolt_byte*)(str),strlen((str)))
#define revolt_ws_disconnect_ok(ws)                     revolt_ws_disconnect((ws),REVOLT_WS_CLOSE_NORMAL,NULL,0)

void revolt_ws_mask(const revolt_byte key[4], revolt_byte *data, uint64_t len);
#define revolt_ws_unmask(key, data, len)                revolt_ws_mask((key),(data),(len));

RevoltErr revolt_ws_send_raw(RevoltWS *ws, long timeout_ms, revolt_byte *data, uint64_t data_len);
RevoltErr revolt_ws_send(RevoltWS *ws, long timeout_ms, RevoltWSFrame frame);
#define revolt_ws_send_bin(ws, timeout_ms, bin, len)    revolt_ws_send((ws),(timeout_ms),revolt_ws_frame__new(REVOLT_WS_OPC_BINARY,(revolt_byte*)(bin),(len)))
#define revolt_ws_send_str(ws, timeout_ms, str)         revolt_ws_send((ws),(timeout_ms),revolt_ws_frame__new(REVOLT_WS_OPC_TEXT,(revolt_byte*)(str),strlen((str))))
#define revolt_ws_send_ping(ws, timeout_ms, data, len)  revolt_ws_send((ws),(timeout_ms),revolt_ws_frame__new(REVOLT_WS_OPC_PING,(revolt_byte*)(data),(len)))
#define revolt_ws_send_pong(ws, timeout_ms, data, len)  revolt_ws_send((ws),(timeout_ms),revolt_ws_frame__new(REVOLT_WS_OPC_PONG,(revolt_byte*)(data),(len)))

RevoltErr revolt_ws_recv_raw(RevoltWS *ws, long timeout_ms, revolt_byte *buf /*out*/, uint64_t buf_size, uint64_t *bytes_recv /*out*/);
RevoltErr revolt_ws_recv(RevoltWS *ws, long timeout_ms, RevoltWSFrame *frame /*out*/, uint64_t *bytes_recv /*out*/);

RevoltErr revolt_ws_serialize_header(RevoltWSHeader header, revolt_byte *data_buf /*out*/, int8_t *data_len /*out*/);
RevoltErr revolt_ws_deserialize_header(const revolt_byte *data, size_t data_len, RevoltWSHeader *header /*out*/, uint8_t *header_len /*out*/);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_WEBSOCKET_H_INCLUDED_ */
