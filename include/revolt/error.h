#ifndef _REVOLTC_ERROR_H_INCLUDED_
#define _REVOLTC_ERROR_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum RevoltErr {
    REVOLTE_OK      = 0,
    REVOLTE_UNKNOWN = 1,

    REVOLTE_NOT_IMPLEMENTED,                    /* function not yet implemented */
    REVOLTE_DEPRECATED,                         /* function removed due to deprecation */
    REVOLTE_NOMEM,                              /* allocation failed */
    REVOLTE_INVAL,                              /* invalid arguments passed to function */
    REVOLTE_LIMIT,                              /* value/argument to large or too small */
    REVOLTE_OVERFLOW,
    REVOLTE_UNDERFLOW,
    REVOLTE_PARSE,

    REVOLTE_IO,                                 /* generic io error */
    REVOLTE_PERMISSION,
    REVOLTE_NOT_CONNECTED,                      /* not connected to remote host */
    REVOLTE_CONNECT,                            /* could not connect to remote host */
    REVOLTE_TIMEOUT,                            /* connection timed out */
    REVOLTE_SEND,                               /* could not send data */
    REVOLTE_RECV,                               /* could not receive data */
    REVOLTE_INSUFFICIENT_DATA,

    REVOLTE_DEPENDENCY,
    REVOLTE_CURL_INIT,
    REVOLTE_CURL    = 1 << 24,

    REVOLTE_LAST
} RevoltErr;

#define REVOLT_ERR_3RD_PARTY_MASK (0xFF << 24)

extern const char *const REVOLT_ERR_STR_OK;
extern const char *const REVOLT_ERR_STR_UNKNOWN;

extern const char *const REVOLT_ERR_STR_NOT_IMPLEMENTED;
extern const char *const REVOLT_ERR_STR_DEPRECATED;
extern const char *const REVOLT_ERR_STR_NOMEM;
extern const char *const REVOLT_ERR_STR_INVAL;
extern const char *const REVOLT_ERR_STR_LIMIT;
extern const char *const REVOLT_ERR_STR_OVERFLOW;
extern const char *const REVOLT_ERR_STR_UNDERFLOW;
extern const char *const REVOLT_ERR_STR_PARSE;

extern const char *const REVOLT_ERR_STR_IO;
extern const char *const REVOLT_ERR_STR_PERMISSION;
extern const char *const REVOLT_ERR_STR_NOT_CONNECTED;
extern const char *const REVOLT_ERR_STR_CONNECT;
extern const char *const REVOLT_ERR_STR_TIMEOUT;
extern const char *const REVOLT_ERR_STR_SEND;
extern const char *const REVOLT_ERR_STR_RECV;
extern const char *const REVOLT_ERR_STR_INSUFFICIENT_DATA;

extern const char *const REVOLT_ERR_STR_DEPENDENCY;
extern const char *const REVOLT_ERR_STR_CURL_INIT;

const char* revolt_err_str(RevoltErr err);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_ERROR_H_INCLUDED_ */
