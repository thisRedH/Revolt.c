#ifndef _REVOLTC_CLIENT_H_INCLUDED_
#define _REVOLTC_CLIENT_H_INCLUDED_

#include <revolt/common.h>
#include <revolt/rest.h>
#include <revolt/ws.h>

REVOLTC_BEGIN_C_DECLS

typedef struct Revolt {
    RevoltREST *rest;
    RevoltWS *ws;
    revolt_bool running;
} Revolt;

REVOLTC_API Revolt *revolt_init(const char* token, revolt_bool is_bot, const char* api_url, const char* ws_url);
#define revolt_init_bot(bot_token, api_url, ws_url)     revolt_init((bot_token),revolt_true,(api_url),(ws_url))
#define revolt_init_easy(bot_token, api_url)            revolt_init((bot_token),revolt_false,(api_url),NULL)
#define revolt_init_easy_bot(bot_token, api_url)        revolt_init((bot_token),revolt_true,(api_url),NULL)

REVOLTC_API void revolt_cleanup(Revolt *client);

REVOLTC_API RevoltErr revolt_run(Revolt* client);
REVOLTC_API void revolt_stop(Revolt* client);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CLIENT_H_INCLUDED_ */
