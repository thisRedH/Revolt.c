#define REVOLTC_NAMESPACELESS_DEFINES 1
#include "revolt/data_models/user.h"
#include "revolt/core/util.h"
#include "revolt/core/json.h"

REVOLTC_API RevoltErr revolt_file_deserialize_json_obj(const RevoltcJSON *json, RevoltFile *file);

void revolt_user_cleanup(RevoltUser user) {
    free(user.id);
    free(user.username);
    free(user.discriminator);
    free(user.display_name);

    revolt_file_cleanup(user.avatar);
    free(user.status.text);

    for (; user.relations_count > 0; user.relations_count--)
        free(user.relations[user.relations_count - 1].user_id);
    free(user.relations);

    free(user.bot.owner_id);
    (void) memset(&user, 0, sizeof(user));
}

void revolt_user_delete(RevoltUser *user) {
    if_un (NILC(user))
        return;

    revolt_user_cleanup(*user);
    free(user);
}

static enum RevoltUserPresence user_presence_from_str(char *str) {
    if_un (NILC(str))
        return REVOLT_USER_PRESENCE_UNKNOWN;

    revoltc_util_str_tolower(str);
    switch (str[0]) {
        case 'o':
            return REVOLT_USER_PRESENCE_ONLINE;
            break;
        case 'i':
            if (str[1] == 'n')
                return REVOLT_USER_PRESENCE_INVISIBLE;
            else
                return REVOLT_USER_PRESENCE_IDLE;
            break;
        case 'f':
            return REVOLT_USER_PRESENCE_FOCUS;
            break;
        case 'b':
            return REVOLT_USER_PRESENCE_BUSY;
            break;
        default:
            break;
    }
    return REVOLT_USER_PRESENCE_UNKNOWN;
}

static enum RevoltUserRelationshipStatus user_relationship_status_from_str(char *str) {
    if_un (NILC(str))
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

RevoltErr revolt_user_deserialize_json_obj(const RevoltcJSON *json, RevoltUser *user) {
    RevoltErr res;
    const RevoltcJSON *buf;
    size_t max;
    char *str_buf;

    if_un (NILC(json) || NILC(user))
        return REVOLTE_INVAL;

    (void) memset(user, 0, sizeof(*user));

    user->id = revoltc_json_get_str(json, "_id");
    user->username = revoltc_json_get_str(json, "username");
    user->discriminator = revoltc_json_get_str(json, "discriminator");
    user->display_name = revoltc_json_get_str(json, "display_name");

    buf = revoltc_json_get_obj(json, "avatar");
    res = revolt_file_deserialize_json_obj(buf, &user->avatar);
    if (res != REVOLTE_OK)
        return res;

    user->status.text = revoltc_json_get_strn(json, "/status/text", 128);
    str_buf = revoltc_json_get_str(json, "/status/presence");
    user->status.presence = user_presence_from_str(str_buf);
    free(str_buf);

    buf = revoltc_json_get_arr(json, "relations");
    if (buf != NULL) {
        user->relations = malloc(
            sizeof(*user->relations) * revoltc_json_arr_size(buf));

        revoltc_json_arr_foreach(buf, user->relations_count, max, buf) {
            user->relations[user->relations_count].user_id =
                revoltc_json_get_str(buf, "_id");

            str_buf = revoltc_json_get_str(buf, "status");
            user->relations[user->relations_count].status =
                user_relationship_status_from_str(str_buf);
            free(str_buf);
        }
    }

    str_buf = revoltc_json_get_str(json, "relationship");
    user->relationship = user_relationship_status_from_str(str_buf);
    free(str_buf);

    user->badges = revoltc_json_get_int(json, "badges");
    user->flags = revoltc_json_get_int(json, "flags");

    user->privileged = revoltc_json_get_bool(json, "privileged");
    user->online = revoltc_json_get_bool(json, "online");

    user->bot.owner_id = revoltc_json_get_str(json, "/bot/owner");

    return REVOLTE_OK;
}

RevoltErr revolt_user_deserialize_json(const char *json_str, RevoltUser *user) {
    RevoltErr res;
    RevoltcJSON *json;

    if_un (NILC(json_str))
        return REVOLTE_INVAL;

    res = revoltc_json_parse(json_str, 0, &json);
    if (res != REVOLTE_OK)
        return res;

    res = revolt_user_deserialize_json_obj(json, user);

    revoltc_json_delete(json);
    return res;
}
