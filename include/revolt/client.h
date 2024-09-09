#ifndef _REVOLTC_CLIENT_H_INCLUDED_
#define _REVOLTC_CLIENT_H_INCLUDED_

#include "revolt/common.h"
#include "revolt/rest.h"
#include "revolt/ws.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Revolt {
    RevoltREST *rest;
    RevoltWS *ws;
} Revolt;

RVLTC_EXPORT Revolt *revolt_init(const char* token, revolt_bool is_bot, const char* api_url, const char* ws_url);
#define revolt_init_bot(bot_token, api_url, ws_url)     revolt_init((bot_token), revolt_true, (api_url), (ws_url))
#define revolt_init_easy(bot_token, api_url)            revolt_init((bot_token), revolt_false, (api_url), NULL)
#define revolt_init_easy_bot(bot_token, api_url)        revolt_init((bot_token), revolt_true, (api_url), NULL)

RVLTC_EXPORT void revolt_cleanup(Revolt *client);

RVLTC_EXPORT RVLTC_UNIMPLEMENTED void revolt_run(Revolt* client);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_CLIENT_H_INCLUDED_ */
