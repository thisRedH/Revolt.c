#define REVOLTC_NAMESPACELESS_DEFINES 1
#include "revolt/data_models/channel.h"
#include "revolt/core/util.h"
#include "revolt/core/json.h"
#include "revolt/error.h"

REVOLTC_API RevoltErr revolt_file_deserialize_json_obj(const RevoltcJSON *json, RevoltFile *file);

void revolt_channel_recipients_cleanup(
    struct RevoltChannelRecipients recipients
) {
    size_t i;
    for (i = 0; i < recipients.count; i++)
        free(recipients.user_ids[i]);
}

void revolt_channel_cleanup(
    RevoltChannel channel,
    revolt_bool clean_substructs
) {
    switch (channel.type) {
        case REVOLT_CHANNEL_TYPE_SM:
            free(channel.ch.sm.id);
            free(channel.ch.sm.owner_id);
            break;
        case REVOLT_CHANNEL_TYPE_DM:
            free(channel.ch.dm.id);
            free(channel.ch.dm.last_message_id);

            if (clean_substructs)
                revolt_channel_recipients_cleanup(channel.ch.dm.recipients);
            break;
        case REVOLT_CHANNEL_TYPE_GROUP:
            free(channel.ch.group.id);
            free(channel.ch.group.owner_id);
            free(channel.ch.group.last_message_id);

            free(channel.ch.group.name);
            free(channel.ch.group.description);

            if (clean_substructs) {
                revolt_file_cleanup(channel.ch.group.icon);
                revolt_channel_recipients_cleanup(channel.ch.group.recipients);
            }
            break;
        case REVOLT_CHANNEL_TYPE_TEXT:
            free(channel.ch.text.id);
            free(channel.ch.text.server_id);
            free(channel.ch.text.last_message_id);

            free(channel.ch.text.name);
            free(channel.ch.text.description);

            if (clean_substructs)
                revolt_file_cleanup(channel.ch.text.icon);
                /* TODO: clean role_perms */
            break;
        case REVOLT_CHANNEL_TYPE_VOICE:
            free(channel.ch.voice.id);
            free(channel.ch.voice.server_id);

            free(channel.ch.voice.name);
            free(channel.ch.voice.description);

            if (clean_substructs)
                revolt_file_cleanup(channel.ch.voice.icon);
                /* TODO: clean role_perms */
            break;
        default:
            break;
    }

    (void) memset(&channel, 0, sizeof(channel));
}

void revolt_channel_delete(
    RevoltChannel *channel,
    revolt_bool clean_substructs
) {
    if_un (NILC(channel))
        return;

    revolt_channel_cleanup(*channel, clean_substructs);
    free(channel);
}

static enum RevoltChannelType channel_type_from_str(const char *str) {
    if_un (NILC(str))
        return REVOLT_CHANNEL_TYPE_UNKNOWN;

    switch (tolower(str[0])) {
        case 's':
            return REVOLT_CHANNEL_TYPE_SM;
        case 'd':
            return REVOLT_CHANNEL_TYPE_DM;
        case 'g':
            return REVOLT_CHANNEL_TYPE_GROUP;
        case 't':
            return REVOLT_CHANNEL_TYPE_TEXT;
        case 'v':
            return REVOLT_CHANNEL_TYPE_VOICE;
        default:
            break;
    }
    return REVOLT_CHANNEL_TYPE_UNKNOWN;
}

RevoltErr revolt_channel_sm_deserialize_json_obj(
    const RevoltcJSON *json,
    struct RevoltChannelSavedMessages *ch
) {
    if_un (NILC(json) || NILC(ch))
        return REVOLTE_INVAL;

    (void) memset(ch, 0, sizeof(*ch));

    ch->id = revoltc_json_get_str(json, "_id");
    ch->owner_id = revoltc_json_get_str(json, "user");

    return REVOLTE_OK;
}

RevoltErr revolt_channel_dm_deserialize_json_obj(
    const RevoltcJSON *json,
    struct RevoltChannelDirectMessage *ch
) {
    if_un (NILC(json) || NILC(ch))
        return REVOLTE_INVAL;

    (void) memset(ch, 0, sizeof(*ch));

    ch->id = revoltc_json_get_str(json, "_id");
    ch->last_message_id = revoltc_json_get_str(json, "last_message_id");

    ch->active = revoltc_json_get_bool(json, "active");
    /* TODO recipients */

    return REVOLTE_OK;
}

RevoltErr revolt_channel_group_deserialize_json_obj(
    const RevoltcJSON *json,
    struct RevoltChannelGroup *ch
) {
    const RevoltcJSON *buf;
    RevoltErr res;

    if_un (NILC(json) || NILC(ch))
        return REVOLTE_INVAL;

    (void) memset(ch, 0, sizeof(*ch));

    ch->id = revoltc_json_get_str(json, "_id");
    ch->owner_id = revoltc_json_get_str(json, "owner");
    ch->last_message_id = revoltc_json_get_str(json, "last_message_id");

    ch->name = revoltc_json_get_str(json, "name");
    ch->description = revoltc_json_get_str(json, "description");
    ch->nsfw = revoltc_json_get_bool(json, "nsfw");
    buf = revoltc_json_get_obj(json, "icon");
    res = revolt_file_deserialize_json_obj(buf, &ch->icon);
    if (res != REVOLTE_OK)
        return res;

    ch->permissions = revoltc_json_get_i64(json, "permissions");
    /* TODO recipients */

    return REVOLTE_OK;
}

RevoltErr revolt_channel_text_deserialize_json_obj(
    const RevoltcJSON *json,
    struct RevoltChannelText *ch
) {
    const RevoltcJSON *buf;
    RevoltErr res;

    if_un (NILC(json) || NILC(ch))
        return REVOLTE_INVAL;

    (void) memset(ch, 0, sizeof(*ch));

    ch->id = revoltc_json_get_str(json, "_id");
    ch->server_id = revoltc_json_get_str(json, "server");
    ch->last_message_id = revoltc_json_get_str(json, "last_message_id");

    ch->name = revoltc_json_get_str(json, "name");
    ch->description = revoltc_json_get_str(json, "description");
    ch->nsfw = revoltc_json_get_bool(json, "nsfw");
    buf = revoltc_json_get_obj(json, "icon");
    res = revolt_file_deserialize_json_obj(buf, &ch->icon);
    if (res != REVOLTE_OK)
        return res;

    ch->default_perms.a = revoltc_json_get_i64(json, "/default_permissions/a");
    ch->default_perms.d = revoltc_json_get_i64(json, "/default_permissions/d");
    /* TODO role_perms */

    return REVOLTE_OK;
}

RevoltErr revolt_channel_voice_deserialize_json_obj(
    const RevoltcJSON *json,
    struct RevoltChannelVoice *ch
) {
    const RevoltcJSON *buf;
    RevoltErr res;

    if_un (NILC(json) || NILC(ch))
        return REVOLTE_INVAL;

    (void) memset(ch, 0, sizeof(*ch));

    ch->id = revoltc_json_get_str(json, "_id");
    ch->server_id = revoltc_json_get_str(json, "server");

    ch->name = revoltc_json_get_str(json, "name");
    ch->description = revoltc_json_get_str(json, "description");
    ch->nsfw = revoltc_json_get_bool(json, "nsfw");
    buf = revoltc_json_get_obj(json, "icon");
    res = revolt_file_deserialize_json_obj(buf, &ch->icon);
    if (res != REVOLTE_OK)
        return res;

    ch->default_perms.a = revoltc_json_get_i64(json, "/default_permissions/a");
    ch->default_perms.d = revoltc_json_get_i64(json, "/default_permissions/d");
    /* TODO role_perms */

    ch->id = revoltc_json_get_str(json, "_id");
    return REVOLTE_OK;
}

RevoltErr revolt_channel_deserialize_json_obj(
    const RevoltcJSON *json,
    RevoltChannel *channel
) {
    char *str_buf;

    if_un (NILC(json) || NILC(channel))
        return REVOLTE_INVAL;

    (void) memset(channel, 0, sizeof(*channel));

    str_buf = revoltc_json_get_str(json, "channel_type");
    channel->type = channel_type_from_str(str_buf);
    free(str_buf);

    switch (channel->type) {
        case REVOLT_CHANNEL_TYPE_SM:
            return revolt_channel_sm_deserialize_json_obj(
                json,
                &channel->ch.sm
            );
        case REVOLT_CHANNEL_TYPE_DM:
            return revolt_channel_dm_deserialize_json_obj(
                json,
                &channel->ch.dm
            );
        case REVOLT_CHANNEL_TYPE_GROUP:
            return revolt_channel_group_deserialize_json_obj(
                json,
                &channel->ch.group
            );
        case REVOLT_CHANNEL_TYPE_TEXT:
            return revolt_channel_text_deserialize_json_obj(
                json,
                &channel->ch.text
            );
        case REVOLT_CHANNEL_TYPE_VOICE:
            return revolt_channel_voice_deserialize_json_obj(
                json,
                &channel->ch.voice
            );
        default:
            return REVOLTE_PARSE;
    }
}

RevoltErr revolt_channel_deserialize_json(
    const char *json_str,
    RevoltChannel *channel
) {
    RevoltErr res;
    RevoltcJSON *json;

    if_un (NILC(json_str))
        return REVOLTE_INVAL;

    res = revoltc_json_parse(json_str, 0, &json);
    if (res != REVOLTE_OK)
        return res;

    res = revolt_channel_deserialize_json_obj(json, channel);

    revoltc_json_delete(json);
    return res;
}
