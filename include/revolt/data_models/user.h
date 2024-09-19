#ifndef _REVOLTC_DATA_MODELS_USER_H_INCLUDED_
#define _REVOLTC_DATA_MODELS_USER_H_INCLUDED_

#include <revolt/common.h>
#include <revolt/data_models/file.h>
REVOLTC_BEGIN_C_DECLS

enum RevoltUserPresence {
    REVOLT_USER_PRESENCE_UNKNOWN = 0,
    REVOLT_USER_PRESENCE_ONLINE,
    REVOLT_USER_PRESENCE_IDLE,
    REVOLT_USER_PRESENCE_FOCUS,
    REVOLT_USER_PRESENCE_BUSY,
    REVOLT_USER_PRESENCE_INVISIBLE
};

struct RevoltUserStatus {
    const char *text; /* max 128 chars */
    enum RevoltUserPresence presence;
};

enum RevoltUserBadges {
    REVOLT_USER_BADGE_DEVELOPER                 = 1,    /*Revolt Developer*/
    REVOLT_USER_BADGE_TRANSLATOR                = 2,    /*Helped translate Revolt*/
    REVOLT_USER_BADGE_SUPPORTER                 = 4,    /*Monetarily supported Revolt*/
    REVOLT_USER_BADGE_RESPONSIBLE_DISCLOSURE    = 8,    /*Responsibly disclosed a security issue*/
    REVOLT_USER_BADGE_FOUNDER                   = 16,   /*Revolt Founder*/
    REVOLT_USER_BADGE_PLATFORM_MODERATION       = 32,   /*Platform moderator*/
    REVOLT_USER_BADGE_ACTIVE_SUPPORTER          = 64,   /*Active monetary supporter*/
    REVOLT_USER_BADGE_PAW                       = 128,  /*🦊🦝*/
    REVOLT_USER_BADGE_EARLY_ADOPTER             = 256,  /*Joined as one of the first 1000 users in 2021*/
    REVOLT_USER_BADGE_RESERVED_JOKE_BADGE1      = 512,  /*Amogus*/
    REVOLT_USER_BADGE_RESERVED_JOKE_BADGE2      = 1024  /*Low resolution troll face*/
};

enum RevoltUserFlags {
    REVOLT_USER_FLAG_SUSPENDED  = 1,
    REVOLT_USER_FLAG_DELETED    = 2,
    REVOLT_USER_FLAG_BANNED     = 4,
    REVOLT_USER_FLAG_SPAM       = 8
};

enum RevoltUserRelationshipStatus {
    REVOLT_USER_RELATIONSHIP_UNKNOWN = 0,
    REVOLT_USER_RELATIONSHIP_NONE,
    REVOLT_USER_RELATIONSHIP_USER,
    REVOLT_USER_RELATIONSHIP_FRIEND,
    REVOLT_USER_RELATIONSHIP_OUTGOING,
    REVOLT_USER_RELATIONSHIP_INCOMING,
    REVOLT_USER_RELATIONSHIP_BLOCKED,
    REVOLT_USER_RELATIONSHIP_BLOCKED_OTHER
};

struct RevoltUserRelationship {
    char *user_id;
    enum RevoltUserRelationshipStatus status;
};

struct RevoltUserBotInfo {
    char *owner_id; /* NULL if not bot */
};

typedef struct RevoltUser {
    char *id;
    char *username;
    char *discriminator;
    char *display_name;

    RevoltFile avatar;
    struct RevoltUserStatus status;

    struct RevoltUserRelationship *relations; /*TODO: use array with size attrib*/
    enum RevoltUserRelationshipStatus relationship;

    uint32_t badges;    /* Bitfield of `RevoltUserBadges` */
    uint32_t flags;     /* Bitfield of `RevoltUserBadges` */

    revolt_bool privileged;
    revolt_bool online;

    struct RevoltUserBotInfo bot;
} RevoltUser;

REVOLTC_API void revolt_user_cleanup(RevoltUser user);
REVOLTC_API void revolt_user_delete(RevoltUser *user);
REVOLTC_API RevoltErr revolt_user_deserialize_json(const char *json_str, RevoltUser *user /*out*/);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_DATA_MODELS_USER_H_INCLUDED_ */
