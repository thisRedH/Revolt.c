#ifndef _REVOLTC_DATA_MODELS_SERVER_H_INCLUDED_
#define _REVOLTC_DATA_MODELS_SERVER_H_INCLUDED_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum RevoltSystemMessageType {
    REVOLT_SYSTEM_MESSAGE_USER_JOINED = 0,
    REVOLT_SYSTEM_MESSAGE_USER_LEFT,
    REVOLT_SYSTEM_MESSAGE_USER_KICKED,
    REVOLT_SYSTEM_MESSAGE_USER_BANNED,
    REVOLT_SYSTEM_MESSAGE_TYPE_MAX,
};

struct RevoltSystemMessage {
    enum RevoltSystemMessageType type;
    const char *channel_id;
};

struct RevoltServerCategory {
    const char *id;
    const char *title;
    const char *channel_ids;
};

struct RevoltServerRole {/* TODO */};
struct RevoltServerIcon {/* TODO (maybe like RevoltChannelGroupIcon) */};
struct RevoltServerBanner {/* TODO */};

typedef struct RevoltServer {
    const char *id;
    const char *owner_id;
    const char *name;
    const char *description;
    const char **channel_ids;
    struct RevoltServerCategory *categories;
    void *roles; /* TODO: Map: key=role_id, value=struct RevoltServerRole */
    struct RevoltServerIcon icon;
    struct RevoltServerBanner banner;
    uint32_t flags;
    revolt_bool nsfw;
    revolt_bool analytics;
    revolt_bool discoverable;
} RevoltServer;

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_DATA_MODELS_SERVER_H_INCLUDED_ */
