#ifndef _REVOLTC_FETCH_H_INCLUDED_
#define _REVOLTC_FETCH_H_INCLUDED_

#include "revolt/common.h"
#include "revolt/client.h"
#include "revolt/http.h"
#include "revolt/data_models/models.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RevoltUser */RVLTC_EXPORT RevoltResponse *revolt_fetch_user(Revolt* client, const char *user_id);
#define revolt_fetch_me(client) (revolt_fetch_user((client), "@me"))

RVLTC_UNIMPLEMENTED RVLTC_EXPORT RevoltMessage *revolt_fetch_message(Revolt* client, const char *message_id);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_FETCH_H_INCLUDED_ */
