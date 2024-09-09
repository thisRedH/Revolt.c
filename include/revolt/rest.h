#ifndef _REVOLTC_REST_H_INCLUDED_
#define _REVOLTC_REST_H_INCLUDED_

#include "revolt/common.h"
#include "revolt/core/http.h"
#include "revolt/data_models/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RevoltREST {
    const char *url;
    const char *token;
    revolt_bool bot;
} RevoltREST;

RVLTC_EXPORT RevoltREST *revolt_rest_new(const char *url, const char *token, revolt_bool bot);
RVLTC_EXPORT void revolt_rest_delete(RevoltREST *rest);

/* RevoltUser */RVLTC_EXPORT RevoltcHTTPResponse *revolt_rest_fetch_user(RevoltREST *rest, const char *user_id);
#define revolt_rest_fetch_me(rest)          revolt_rest_fetch_user((rest), "@me")

RVLTC_UNIMPLEMENTED RVLTC_EXPORT RevoltMessage *revolt_rest_fetch_message(RevoltREST *rest, const char *message_id);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_REST_H_INCLUDED_ */
