#ifndef _REVOLTC_DATA_MODELS_CHANNEL_H_INCLUDED_
#define _REVOLTC_DATA_MODELS_CHANNEL_H_INCLUDED_

#include <revolt/common.h>
#include <revolt/data_models/file.h>
REVOLTC_BEGIN_C_DECLS

static const uint64_t REVOLT_CHANNEL_PERMISSIONS_NONE                   = 0;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MANAGE_CHANNEL         = 1 << 0;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MANAGE_SERVER          = 1 << 1;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MANAGE_PERMISSIONS     = 1 << 2;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MANAGE_ROLE            = 1 << 3;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MANAGE_CUSTOMISATION   = 1 << 4;

static const uint64_t REVOLT_CHANNEL_PERMISSIONS_KICK_MEMBERS           = 1 << 6;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_BAN_MEMBERS            = 1 << 7;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_TIMEOUT_MEMBERS        = 1 << 8;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_ASSIGN_ROLES           = 1 << 9;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_CHANGE_NICKNAME        = 1 << 10;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MANAGE_NICKNAMES       = 1 << 11;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_CHANGE_AVATAR          = 1 << 12;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_REMOVE_AVATARS         = 1 << 13;

static const uint64_t REVOLT_CHANNEL_PERMISSIONS_VIEW_CHANNEL           = 1 << 20;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_READ_MESSAGE_HISTORY   = 1 << 21;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_SEND_MESSAGE           = 1 << 22;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MANAGE_MESSAGES        = 1 << 23;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MANAGE_WEBHOOKS        = 1 << 24;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_INVITE_OTHERS          = 1 << 25;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_SEND_EMBEDS            = 1 << 26;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_UPLOAD_FILES           = 1 << 27;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MASQUERADE             = 1 << 28;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_REACT                  = 1 << 29;

static const uint64_t REVOLT_CHANNEL_PERMISSIONS_CONNECT                = 1 << 30;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_SPEAK                  = 1 << 31;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_VIDEO                  = 1L << 32;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MUTE_MEMBERS           = 1L << 33;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_DEAFEN_MEMBERS         = 1L << 34;
static const uint64_t REVOLT_CHANNEL_PERMISSIONS_MOVE_MEMBERS           = 1L << 35;

enum RevoltChannelType {
    REVOLT_CHANNEL_TYPE_UNKNOWN = 0,
    REVOLT_CHANNEL_TYPE_SAVED_MESSAGES,
    REVOLT_CHANNEL_TYPE_DIRECT_MESSAGE,
    REVOLT_CHANNEL_TYPE_GROUP,
    REVOLT_CHANNEL_TYPE_TEXT_CHANNEL,
    REVOLT_CHANNEL_TYPE_VOICE_CHANNEL,

    REVOLT_CHANNEL_TYPE_SM      = REVOLT_CHANNEL_TYPE_SAVED_MESSAGES,
    REVOLT_CHANNEL_TYPE_DM      = REVOLT_CHANNEL_TYPE_DIRECT_MESSAGE,
    REVOLT_CHANNEL_TYPE_TEXT    = REVOLT_CHANNEL_TYPE_TEXT_CHANNEL,
    REVOLT_CHANNEL_TYPE_VOICE   = REVOLT_CHANNEL_TYPE_VOICE_CHANNEL
};

/* TODO: maybe move to a better place */
struct RevoltPermissionOverrideField {
    int64_t a;      /* Bitfield of `RevoltChannelPermissions` */
    int64_t d;      /* Bitfield of `RevoltChannelPermissions` */
};

struct RevoltChannelRecipients {
    char **user_ids;
    size_t count;
};

struct RevoltChannelSavedMessages {
    char *id;
    char *owner_id;
};

struct RevoltChannelDirectMessage {
    char *id;
    char *last_message_id;

    revolt_bool active;
    struct RevoltChannelRecipients recipients;
};

struct RevoltChannelGroup {
    char *id;
    char *owner_id;
    char *last_message_id;

    char *name;
    char *description;
    revolt_bool nsfw;
    RevoltFile icon;

    int64_t permissions;    /* Bitfield of `RevoltChannelPermissions` */
    struct RevoltChannelRecipients recipients;
};

struct RevoltChannelText {
    char *id;
    char *server_id;
    char *last_message_id;

    char *name;
    char *description;
    revolt_bool nsfw;
    RevoltFile icon;

    struct RevoltPermissionOverrideField default_perms;
    void *role_perms; /* TODO: Map: key=role_id, value=struct RevoltPermissionOverrideField */
};

struct RevoltChannelVoice {
    char *id;
    char *server_id;

    char *name;
    char *description;
    revolt_bool nsfw;
    RevoltFile icon;

    struct RevoltPermissionOverrideField default_perms;
    void *role_perms; /* TODO: Map: key=role_id, value=struct RevoltPermissionOverrideField */
};

typedef struct RevoltChannel {
    enum RevoltChannelType type;
    union RevoltChannelChannel {
        struct RevoltChannelSavedMessages sm;
        struct RevoltChannelDirectMessage dm;
        struct RevoltChannelGroup group;
        struct RevoltChannelText text;
        struct RevoltChannelVoice voice;
    } ch;
} RevoltChannel;

REVOLTC_API void revolt_channel_cleanup(RevoltChannel channel, revolt_bool clean_substructs);
REVOLTC_API void revolt_channel_delete(RevoltChannel *channel, revolt_bool clean_substructs);
REVOLTC_API RevoltErr revolt_channel_deserialize_json(const char *json_str, RevoltChannel *channel /*out*/);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_DATA_MODELS_CHANNEL_H_INCLUDED_ */
