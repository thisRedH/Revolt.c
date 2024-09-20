#ifndef _REVOLTC_FETCH_H_INCLUDED_
#define _REVOLTC_FETCH_H_INCLUDED_

#include <revolt/common.h>
#include <revolt/rest.h>
#include <revolt/client.h>
#include <revolt/data_models/models.h>
REVOLTC_BEGIN_C_DECLS

REVOLTC_API RevoltUser *revolt_fetch_rest_user(RevoltREST *rest, const char *user_id, RevoltErr *err /*out*/);

static REVOLTC_INLINE const RevoltUser *revolt_fetch_user(Revolt *client, const char *user_id, RevoltErr *err /*out*/) {
    /*TODO: add user to user cache of client*/
    return revolt_fetch_rest_user(client->rest, user_id, err);
}

static REVOLTC_INLINE const RevoltUser *revolt_fetch_self(Revolt *client, RevoltErr *err /*out*/) {
    /*TODO: add user to user cache of client*/
    return revolt_fetch_rest_user(client->rest, NULL, err);
}

REVOLTC_API RevoltChannel *revolt_fetch_rest_channel(RevoltREST *rest, const char *channel_id, RevoltErr *err /*out*/);

static REVOLTC_INLINE const RevoltChannel *revolt_fetch_channel(Revolt *client, const char *channel_id, RevoltErr *err /*out*/) {
    /*TODO: add user to channel cache of client*/
    return revolt_fetch_rest_channel(client->rest, channel_id, err);
}

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_FETCH_H_INCLUDED_ */
