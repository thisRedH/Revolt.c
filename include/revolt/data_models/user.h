#ifndef _REVOLTC_DATA_MODELS_USER_H_INCLUDED_
#define _REVOLTC_DATA_MODELS_USER_H_INCLUDED_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum RevoltUserPresence {
    REVOLT_USER_ONLINE = 0,
    REVOLT_USER_IDLE,
    REVOLT_USER_FOCUS,
    REVOLT_USER_BUSY,
    REVOLT_USER_INVISIBLE,
    REVOLT_USER_PRESENCE_MAX,
};

struct RevoltUserStatus {
    const char *status_text; /* max 128 chars */
    enum RevoltUserPresence presence;
};

struct RevoltUserAvetar {
    /*TODO*/
};

struct RevoltUserRelation {
    /*TODO*/
};

typedef struct RevoltUser {
    const char *id;
    const char username[32];
    const char *author_id;
    const char *discriminator;
    const char *display_name;
    struct RevoltUserAvetar avatar;
    struct RevoltUserRelation relations;
    uint32_t badges;
    uint32_t flags;
    revolt_bool privileged;
} RevoltUser;

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_DATA_MODELS_USER_H_INCLUDED_ */
