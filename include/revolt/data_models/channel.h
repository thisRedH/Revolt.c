#ifndef _REVOLTC_DATA_MODELS__H_INCLUDED_
#define _REVOLTC_DATA_MODELS__H_INCLUDED_

#include "revolt/common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RevoltChannelRecipients {
    const char **ids;
    size_t count;
};

struct RevoltChannelGroupIcon {int todo;/* TODO (maybe like RevoltServerIcon) */};

struct RevoltPermissionOverrideField {
    int64_t allowed_bitflag;
    int64_t disallowed_bitflag;
};

typedef struct RevoltChannelSavedMessages {
    const char *id;
    const char *owner_id;
} RevoltChannelSavedMessages;

typedef struct RevoltChannelDirectMessage {
    const char *id;
    revolt_bool active;
    struct RevoltChannelRecipients recipients;
    const char *last_message_id;
} RevoltChannelDirectMessage;
typedef struct RevoltChannelDirectMessage RevoltChannelDM;

typedef struct RevoltChannelGroup {
    const char *id;
    const char *name;
    const char *owner_id;
    const char *description;
    struct RevoltChannelRecipients recipients;
    struct RevoltChannelGroupIcon icon;
    const char *last_message_id;
    int64_t permissions_bitflag;
    revolt_bool nsfw;
} RevoltChannelGroup;

typedef struct RevoltChannelTextChannel {
    const char *id;
    const char *server_id;
    const char *name;
    const char *description;
    struct RevoltChannelGroupIcon icon;
    const char *last_message_id;
    struct RevoltPermissionOverrideField default_perms;
    void *role_perms; /* TODO: Map: key=role_id, value=struct RevoltPermissionOverrideField */
    revolt_bool nsfw;
} RevoltChannelTextChannel;
typedef struct RevoltChannelTextChannel RevoltChannelText;

typedef struct RevoltChannelVoiceChannel {
    const char *id;
    const char *server_id;
    const char *name;
    const char *description;
    struct RevoltChannelGroupIcon icon;
    struct RevoltPermissionOverrideField default_perms;
    void *role_perms; /* TODO: Map: key=role_id, value=struct RevoltPermissionOverrideField */
    revolt_bool nsfw;
} RevoltChannelVoiceChannel;
typedef struct RevoltChannelVoiceChannel RevoltChannelVoice;

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_DATA_MODELS__H_INCLUDED_ */
