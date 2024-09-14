#ifndef _REVOLTC_CORE_WEBSOCKET_H_INCLUDED_
#define _REVOLTC_CORE_WEBSOCKET_H_INCLUDED_

#include <revolt/core/defines.h>
#include <revolt/core/types.h>
#include <revolt/error.h>

REVOLTC_BEGIN_C_DECLS

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
enum RevoltcWSOpcodes {
    REVOLTC_WS_OPC_CONTINUATION = 0x0,
    REVOLTC_WS_OPC_TEXT         = 0x1,
    REVOLTC_WS_OPC_BINARY       = 0x2,
    REVOLTC_WS_OPC_RESV_3       = 0x3,
    REVOLTC_WS_OPC_RESV_4       = 0x4,
    REVOLTC_WS_OPC_RESV_5       = 0x5,
    REVOLTC_WS_OPC_RESV_6       = 0x6,
    REVOLTC_WS_OPC_RESV_7       = 0x7,
    REVOLTC_WS_OPC_CLOSE        = 0x8,
    REVOLTC_WS_OPC_PING         = 0x9,
    REVOLTC_WS_OPC_PONG         = 0xA,
    REVOLTC_WS_OPC_RESV_B       = 0xB,
    REVOLTC_WS_OPC_RESV_C       = 0xC,
    REVOLTC_WS_OPC_RESV_D       = 0xD,
    REVOLTC_WS_OPC_RESV_E       = 0xE,
    REVOLTC_WS_OPC_RESV_F       = 0xF
};

enum RevoltcWSStatusCodes {
    REVOLTC_WS_CLOSE_NORMAL                 = 1000,
    REVOLTC_WS_CLOSE_GOING_AWAY             = 1001,
    REVOLTC_WS_CLOSE_PROTOCOL_ERR           = 1002,
    REVOLTC_WS_CLOSE_UNSUPPORTED_DATA       = 1003,
    REVOLTC_WS_CLOSE_RESV_1004              = 1004,
    REVOLTC_WS_STATUS_NO_CODE_RECV          = 1005, /* not allowed in a close Frame */
    REVOLTC_WS_STATUS_CLOSED_ABNORMALLY     = 1006, /* not allowed in a close Frame */
    REVOLTC_WS_CLOSE_INVALID_DATA           = 1007,
    REVOLTC_WS_CLOSE_POLICY_VIOLATION       = 1008,
    REVOLTC_WS_CLOSE_MESSAGE_TOO_BIG        = 1009,
    REVOLTC_WS_CLOSE_UNSUPPORTED_EXT        = 1010,
    REVOLTC_WS_CLOSE_INTERNAL_SERVER_ERR    = 1011,
    REVOLTC_WS_STATUS_TLS_HANDSHAKE_ERR     = 1015, /* not allowed in a close Frame */
    REVOLTC_WS_STATUS_IANA_START            = 3000,
    REVOLTC_WS_STATUS_IANA_END              = 3999,
    REVOLTC_WS_STATUS_PRIVATE_START         = 4000,
    REVOLTC_WS_STATUS_PRIVATE_END           = 4999
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

#define REVOLTC_WS_HEADER_MAXS          14
#define REVOLTC_WS_HEADER_MINS          2
#define REVOLTC_WS_PAYLOAD_MINS         0
#define REVOLTC_WS_PAYLOAD_MAXS         INT64_MAX
#define REVOLTC_WS_PAYLOAD_MAXS_CNTRL   125 /* limit for control frame payload */
#define REVOLTC_WS_FRAME_MAXS           ((uint64_t)REVOLT_WS_PAYLOAD_MAXS + REVOLT_WS_HEADER_MAXS)
#define REVOLTC_WS_FRAME_MINS           (REVOLT_WS_HEADER_MINS + REVOLT_WS_PAYLOAD_MINS)

typedef struct RevoltcWSHeader {
    unsigned int fin    :1;
    unsigned int rsv1   :1;
    unsigned int rsv2   :1;
    unsigned int rsv3   :1;
    enum RevoltcWSOpcodes opcode;
    unsigned int masked :1;
    uint32_t masking_key;
    uint64_t payload_len;
} RevoltcWSHeader;

typedef struct RevoltcWSFrame {
    RevoltcWSHeader header;
    revolt_byte *payload;
} RevoltcWSFrame;

/**
 * Just a helper fn mainly for `revolt_ws_send_`... macros.
 * Since C89 dosn't have Compound Literals.
 */
RevoltcWSFrame revoltc_ws_frame__new(enum RevoltcWSOpcodes opc, revolt_byte *payload, uint64_t payload_len);

typedef struct RevoltcWS {
    /*CURL*/void *hnd;
    char *url;
    revolt_bool connected;
} RevoltcWS;

RevoltcWS *revoltc_ws_new(const char *url);
void revoltc_ws_delete(RevoltcWS *ws);

RevoltErr revoltc_ws_connect(RevoltcWS *ws);
RevoltErr revoltc_ws_disconnect(RevoltcWS *ws, enum RevoltcWSStatusCodes status, revolt_byte *msg, size_t msg_len);
#define revoltc_ws_disconnect_str(ws, status, str)      revoltc_ws_disconnect((ws),(status),(revolt_byte*)(str),strlen((str)))
#define revoltc_ws_disconnect_ok(ws)                    revoltc_ws_disconnect((ws),REVOLTC_WS_CLOSE_NORMAL,NULL,0)

void revoltc_ws_mask(const revolt_byte key[4], revolt_byte *data, uint64_t len);
#define revoltc_ws_unmask(key, data, len)               revoltc_ws_mask((key),(data),(len));

RevoltErr revoltc_ws_send_raw(RevoltcWS *ws, long timeout_ms, revolt_byte *data, uint64_t data_len);
RevoltErr revoltc_ws_send(RevoltcWS *ws, long timeout_ms, RevoltcWSFrame frame);
#define revoltc_ws_send_bin(ws, timeout_ms, bin, len)   revoltc_ws_send((ws),(timeout_ms),revoltc_ws_frame__new(REVOLTC_WS_OPC_BINARY,(revolt_byte*)(bin),(len)))
#define revoltc_ws_send_str(ws, timeout_ms, str)        revoltc_ws_send((ws),(timeout_ms),revoltc_ws_frame__new(REVOLTC_WS_OPC_TEXT,(revolt_byte*)(str),strlen((str))))
#define revoltc_ws_send_ping(ws, timeout_ms, data, len) revoltc_ws_send((ws),(timeout_ms),revoltc_ws_frame__new(REVOLTC_WS_OPC_PING,(revolt_byte*)(data),(len)))
#define revoltc_ws_send_pong(ws, timeout_ms, data, len) revoltc_ws_send((ws),(timeout_ms),revoltc_ws_frame__new(REVOLTC_WS_OPC_PONG,(revolt_byte*)(data),(len)))

RevoltErr revoltc_ws_recv_raw(RevoltcWS *ws, long timeout_ms, revolt_byte *buf /*out*/, uint64_t buf_size, uint64_t *bytes_recv /*out*/);
RevoltErr revoltc_ws_recv(RevoltcWS *ws, long timeout_ms, RevoltcWSFrame *frame /*out*/, uint64_t *bytes_recv /*out*/);

RevoltErr revoltc_ws_serialize_header(RevoltcWSHeader header, revolt_byte *data_buf /*out*/, int8_t *data_len /*out*/);
RevoltErr revoltc_ws_deserialize_header(const revolt_byte *data, size_t data_len, RevoltcWSHeader *header /*out*/, uint8_t *header_len /*out*/);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CORE_WEBSOCKET_H_INCLUDED_ */
