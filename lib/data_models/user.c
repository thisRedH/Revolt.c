#include "revolt/data_models/user.h"
#include "revolt/core/util.h"
#include "revolt/core/json.h"

extern RevoltErr revolt_file_deserialize_json_obj(const cJSON *json, RevoltFile *file);

void revolt_user_cleanup(RevoltUser user) {
    free(user.id);
    free(user.username);
    free(user.discriminator);
    free(user.display_name);

    revolt_file_cleanup(user.avatar);
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

RevoltErr revolt_user_status_deserialize_json_obj(const cJSON *json, struct RevoltUserStatus *status) {
    char *presence;

    if (json == NULL || status == NULL)
        return REVOLTE_INVAL;

    (void) memset(status, 0, sizeof(*status));

    revoltc_json_get_strn(status->text, json, "text", 128);
    revoltc_json_get_str(presence, json, "presence"); else presence = NULL;

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
                break;
        }
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

RevoltErr revolt_user_deserialize_json_obj(const cJSON *json, RevoltUser *user) {
    RevoltErr res;
    const cJSON *buf;
    char *str_buf;

    if (json == NULL || user == NULL)
        return REVOLTE_INVAL;

    (void) memset(user, 0, sizeof(*user));

    revoltc_json_get_str(user->id, json, "_id");
    revoltc_json_get_str(user->username, json, "username");
    revoltc_json_get_str(user->discriminator, json, "discriminator");
    revoltc_json_get_str(user->display_name, json, "display_name");

    buf = cJSON_GetObjectItem(json, "avatar");
    res = revolt_file_deserialize_json_obj(buf, &user->avatar);
    if (res != REVOLTE_OK)
        return res;

    buf = cJSON_GetObjectItem(json, "status");
    res = revolt_user_status_deserialize_json_obj(buf, &user->status);
    if (res != REVOLTE_OK)
        return res;

    buf = cJSON_GetObjectItem(json, "relations");
    /*TODO: impl relations */

    revoltc_json_get_str(str_buf, json, "relationship"); else str_buf = NULL;
    user->relationship = user_relationship_status_from_str(str_buf);
    free(str_buf);

    revoltc_json_get_int(user->badges, json, "badges");
    revoltc_json_get_int(user->flags, json, "flags");

    revoltc_json_get_bool(user->privileged, json, "privileged");
    revoltc_json_get_bool(user->online, json, "online");

    buf = cJSON_GetObjectItem(json, "bot");
    if (cJSON_IsObject(buf))
        revoltc_json_get_str(user->bot.owner_id, buf, "owner");

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

    res = revolt_user_deserialize_json_obj(json, user);

    cJSON_Delete(json);
    return res;
}
