#ifndef _REVOLTC_DATA_MODELS_MESSAGE_H_INCLUDED_
#define _REVOLTC_DATA_MODELS_MESSAGE_H_INCLUDED_

#include "revolt/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RevoltMessage {
    const char *id;
    const char *channel_id;
    const char *author_id;
    const char *content;
    const char *nonce;
    const char *system_raw;
    const char *edited;
    const char *masquerade_raw;
} RevoltMessage;

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_DATA_MODELS_MESSAGE_H_INCLUDED_ */
