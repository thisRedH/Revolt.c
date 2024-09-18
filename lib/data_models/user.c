#include "revolt/data_models/user.h"
#include "revolt/core/util.h"
#include <cjson/cJSON.h>

void revolt_user_cleanup(RevoltUser user) {
    free(user.id);
    free(user.username);
    free(user.discriminator);
    free(user.display_name);

    /*TODO free(user.relations);*/

    free(user.bot.owner_id);
    (void) memset(&user, 0, sizeof(user));
}

void revolt_user_delete(RevoltUser *user) {
    if (user == NULL)
        return;

    revolt_user_cleanup(*user);
    free(user);
}

#define json_str_getn(dst, json, n) \
    if (cJSON_IsString((json)) && ((json)->valuestring != NULL)) \
        (dst) = revoltc_util_str_dupn((json)->valuestring,(n))

#define json_str_get(dst, json)     \
    if (cJSON_IsString((json)) && ((json)->valuestring != NULL)) \
        (dst) = revoltc_util_str_dup((json)->valuestring)

#define json_int_get(dst, json)     \
    if (cJSON_IsNumber((json))) (dst) = ((json)->valueint)

#define json_double_get(dst, json)  \
    if (cJSON_IsNumber((json))) (dst) = ((json)->valuedouble)

#define json_bool_get(dst, json)    \
    if (cJSON_IsBool((json)) || cJSON_IsNumber((json))) \
        (dst) = ((json)->valueint)

#define json_key_str_getn(dst, json, n, key)    \
    json_str_getn(dst, cJSON_GetObjectItem((json), (key)), n)

#define json_key_str_get(dst, json, key)        \
    json_str_get(dst, cJSON_GetObjectItem((json), (key)))

#define json_key_int_get(dst, json, key)        \
    json_int_get(dst, cJSON_GetObjectItem((json), (key)))

#define json_key_double_get(dst, json, key)     \
    json_double_get(dst, cJSON_GetObjectItem((json), (key)))

#define json_key_bool_get(dst, json, key)       \
    json_bool_get(dst, cJSON_GetObjectItem((json), (key)))

static RevoltErr user_status_deserialize_json(const cJSON *json, struct RevoltUserStatus *status) {
    char *presence;

    if (json == NULL || status == NULL)
        return REVOLTE_INVAL;

    (void) memset(status, 0, sizeof(*status));

    json_key_str_getn(status->text, json, 128, "text");
    json_key_str_get(presence, json, "presence"); else presence = NULL;

    if (presence != NULL) {
        revoltc_util_str_tolower(presence);
        switch (presence[0]) {
            case 'o':
                status->presence = REVOLT_USER_PRESENCE_ONLINE;
                break;
            case 'i':
                if (presence[1] == 'n')
                    status->presence = REVOLT_USER_PRESENCE_INVISIBLE;
                else
                    status->presence = REVOLT_USER_PRESENCE_IDLE;
                break;
            case 'f':
                status->presence = REVOLT_USER_PRESENCE_FOCUS;
                break;
            case 'b':
                status->presence = REVOLT_USER_PRESENCE_BUSY;
                break;
            default:
                status->presence = REVOLT_USER_PRESENCE_UNKNOWN;
                break;
        }
    } else {
        status->presence = REVOLT_USER_PRESENCE_UNKNOWN;
    }

    free(presence);
    return REVOLTE_OK;
}

static enum RevoltUserRelationshipStatus user_relationship_status_from_str(char *str) {
    if (str == NULL)
        return REVOLT_USER_RELATIONSHIP_UNKNOWN;

    revoltc_util_str_tolower(str);
    switch (str[0]) {
        case 'n':
            return REVOLT_USER_RELATIONSHIP_NONE;
        case 'u':
            return REVOLT_USER_RELATIONSHIP_USER;
        case 'f':
            return REVOLT_USER_RELATIONSHIP_FRIEND;
        case 'i':
            return REVOLT_USER_RELATIONSHIP_INCOMING;
        case 'o':
            return REVOLT_USER_RELATIONSHIP_OUTGOING;
        case 'b':
            if (strlen(str) > strlen("Blocked")) {
                return REVOLT_USER_RELATIONSHIP_BLOCKED_OTHER;
            } else {
                return REVOLT_USER_RELATIONSHIP_BLOCKED;
            }
        default:
            break;
    }
    return REVOLT_USER_RELATIONSHIP_UNKNOWN;
}

static RevoltErr user_deserialize_json(const cJSON *json, RevoltUser *user) {
    RevoltErr res;
    const cJSON *buf;
    char *str_buf;

    if (json == NULL || user == NULL)
        return REVOLTE_INVAL;

    (void) memset(user, 0, sizeof(*user));
    user->privileged = revolt_false;
    user->online = revolt_false;

    json_key_str_get(user->id, json, "_id");
    json_key_str_get(user->username, json, "username");
    json_key_str_get(user->discriminator, json, "discriminator");
    json_key_str_get(user->display_name, json, "display_name");

    buf = cJSON_GetObjectItem(json, "avatar");
    buf = cJSON_GetObjectItem(json, "status");
    res = user_status_deserialize_json(buf, &user->status);
    if (res != REVOLTE_OK)
        return res;

    buf = cJSON_GetObjectItem(json, "relations");
    json_key_str_get(str_buf, json, "relationship"); else str_buf = NULL;
    user->relationship = user_relationship_status_from_str(str_buf);
    free(str_buf);

    json_key_int_get(user->badges, json, "badges");
    json_key_int_get(user->flags, json, "flags");

    json_key_bool_get(user->privileged, json, "privileged");
    json_key_bool_get(user->online, json, "online");

    buf = cJSON_GetObjectItem(json, "bot");
    if (cJSON_IsObject(buf))
        json_key_str_get(user->bot.owner_id, buf, "owner");

    /*TODO: impl avatar and relations */

    return REVOLTE_OK;
}

RevoltErr revolt_user_deserialize_json(const char *json_str, RevoltUser *user) {
    RevoltErr res;
    cJSON *json;

    if (json_str == NULL)
        return REVOLTE_INVAL;

    json = cJSON_Parse(json_str);
    if (json == NULL)
        return REVOLTE_PARSE;

    res = user_deserialize_json(json, user);

    cJSON_Delete(json);
    return res;
}
