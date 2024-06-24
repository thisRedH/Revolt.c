#ifndef _REVOLTC_CLIENT_H_INCLUDED_
#define _REVOLTC_CLIENT_H_INCLUDED_

#include <pthread.h>
#include <pstdint/pstdint.h>
#include "common.h"
#include "http.h"
#include "websocket.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Revolt {
    RevoltHTTP* http;
    RevoltWebSocket* ws;
} Revolt;

RVLTC_EXPORT Revolt* revolt_bot_easy_init(const char* bot_token);
RVLTC_EXPORT RVLTC_UNIMPLEMENTED Revolt* revolt_init(const char* token, revolt_bool is_bot, const char* api_url, const char* ws_url);
RVLTC_EXPORT void revolt_cleanup(Revolt* client);
RVLTC_EXPORT RVLTC_UNIMPLEMENTED void revolt_run(Revolt* client);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_CLIENT_H_INCLUDED_ */
