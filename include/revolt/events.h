#ifndef _REVOLTC_EVENTS_H_INCLUDED_
#define _REVOLTC_EVENTS_H_INCLUDED_

#include "revolt/common.h"
#include "revolt/client.h"
#include "revolt/data_models/models.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RevoltRawEvent {
    const char *data;
} RevoltRawEvent;

typedef struct RevoltBulkEvent {
    RevoltRawEvent *events;
    uint32_t count;
} RevoltBulkEvent;

enum RevoltErrorEventEnum {
    REVOLT_ERROR_EVENT_LABEL_ME = 0,
    REVOLT_ERROR_EVENT_INTERNAL_ERROR,
    REVOLT_ERROR_EVENT_INVALID_SESSION,
    REVOLT_ERROR_EVENT_ONBOARDING_NOT_FINISHED,
    REVOLT_ERROR_EVENT_ALREADY_AUTHENTICATED,
    REVOLT_ERROR_EVENT_UNIMPLEMENTED,
    REVOLT_ERROR_EVENT_UNKNOWN,
    REVOLT_ERROR_EVENT_ENUM_MAX
};

typedef struct RevoltErrorEvent {
    enum RevoltErrorEventEnum error;
    const char *raw_error; /* Only for REVOLT_ERROR_EVENT_UNIMPLEMENTED else NULL */
} RevoltErrorEvent;

typedef struct RevoltPongEvent {
    uint64_t timestamp;
} RevoltPongEvent;

typedef struct RevoltReadyEvent {
    RevoltUser *users;
    RevoltServer *servers;
    void *channels; /*TODO*/
    RevoltEmoji *emoji;
} RevoltReadyEvent;

/* Lib Events */

REVOLTC_API void revolt_set_on_ws_open(Revolt *client, void(*callback)(Revolt *client));
REVOLTC_API void revolt_set_on_ws_close(Revolt *client, void(*callback)(Revolt *client));
REVOLTC_API void revolt_set_on_ws_data(Revolt *client, void(*callback)(Revolt *client, const char *data));

/* Events from https://developers.revolt.chat/developers/events/protocol.html */

REVOLTC_API void revolt_set_on_raw_event(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));

REVOLTC_API void revolt_set_on_error(Revolt *client, void(*callback)(Revolt *client, const RevoltErrorEvent *event));

REVOLTC_API void revolt_set_on_authenticated(Revolt *client, void(*callback)(Revolt *client));
REVOLTC_API void revolt_set_on_logout(Revolt *client, void(*callback)(Revolt *client));

REVOLTC_API void revolt_set_on_bulk(Revolt *client, void(*callback)(Revolt *client, const RevoltBulkEvent *event));
REVOLTC_API void revolt_set_on_pong(Revolt *client, void(*callback)(Revolt *client, const RevoltPongEvent *event));
REVOLTC_API void revolt_set_on_ready(Revolt *client, void(*callback)(Revolt *client, const RevoltReadyEvent *event));

REVOLTC_API void revolt_set_on_message(Revolt *client, void(*callback)(Revolt *client, const RevoltMessage *message));
REVOLTC_API void revolt_set_on_message_update(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_message_append(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_message_delete(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_message_react(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_message_unreact(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_message_remove_reaction(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));

REVOLTC_API void revolt_set_on_channel_create(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *channel));
REVOLTC_API void revolt_set_on_channel_update(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_channel_delete(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_channel_group_join(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_channel_group_leave(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_channel_start_typing(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_channel_stop_typing(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_channel_ack(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));

REVOLTC_API void revolt_set_on_server_create(Revolt *client, void(*callback)(Revolt *client, const RevoltServer *server));
REVOLTC_API void revolt_set_on_server_update(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_server_delete(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_server_member_update(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_server_member_join(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_server_member_leave(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_server_role_update(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_server_role_delete(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));

REVOLTC_API void revolt_set_on_user_update(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_user_relationship(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_user_platform_wipe(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));

REVOLTC_API void revolt_set_on_emoji_create(Revolt *client, void(*callback)(Revolt *client, const RevoltEmoji *event));
REVOLTC_API void revolt_set_on_emoji_delete(Revolt *client, void(*callback)(Revolt *client, const char *emoji_id));

REVOLTC_API void revolt_set_on_auth(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));

REVOLTC_API void revolt_set_on_delete_session(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));
REVOLTC_API void revolt_set_on_delete_all_session(Revolt *client, void(*callback)(Revolt *client, const RevoltRawEvent *event));

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_EVENTS_H_INCLUDED_ */
