#ifndef _REVOLTC_WS_H_INCLUDED_
#define _REVOLTC_WS_H_INCLUDED_

#include "revolt/common.h"
#include "revolt/core/websocket.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RevoltWS {
    RevoltcWS *ws;
    char *url;
} RevoltWS;

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_WS_H_INCLUDED_ */
