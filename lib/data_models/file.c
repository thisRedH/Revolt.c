#include "revolt/data_models/file.h"
#include "revolt/core/util.h"
#include "revolt/core/json.h"

void revolt_file_cleanup(RevoltFile file) {
    free(file.id);
    free(file.tag);
    free(file.filename);
    free(file.content_type);

    free(file.message_id);
    free(file.user_id);
    free(file.server_id);

    free(file.object_id);

    (void) memset(&file, 0, sizeof(file));
}

void revolt_file_delete(RevoltFile *file) {
    if (file == NULL)
        return;

    revolt_file_cleanup(*file);
    free(file);
}

RevoltErr revolt_file_metadata_deserialize_json_obj(const cJSON *json, struct RevoltFileMetadata *metadata) {
    char *type;

    if (json == NULL || metadata == NULL)
        return REVOLTE_INVAL;

    (void) memset(metadata, 0, sizeof(*metadata));

    revoltc_json_get_str(type, json, "type"); else type = NULL;

    if (type != NULL) {
        revoltc_util_str_tolower(type);
        switch (type[0]) {
            case 'f':
                metadata->type = REVOLT_FILE_METADATA_TYPE_FILE;
                break;
            case 't':
                metadata->type = REVOLT_FILE_METADATA_TYPE_TEXT;
                break;
            case 'i':
                metadata->type = REVOLT_FILE_METADATA_TYPE_IMAGE;
                revoltc_json_get_int(metadata->data.image.width, json, "width");
                revoltc_json_get_int(metadata->data.image.height, json, "height");
                break;
            case 'v':
                metadata->type = REVOLT_FILE_METADATA_TYPE_VIDEO;
                revoltc_json_get_int(metadata->data.video.width, json, "width");
                revoltc_json_get_int(metadata->data.video.height, json, "height");
                break;
            case 'a':
                metadata->type = REVOLT_FILE_METADATA_TYPE_AUDIO;
                break;
            default:
                break;
        }
    }

    free(type);
    return REVOLTE_OK;
}

RevoltErr revolt_file_deserialize_json_obj(const cJSON *json, RevoltFile *file) {
    RevoltErr res;
    const cJSON *buf;

    if (json == NULL || file == NULL)
        return REVOLTE_INVAL;

    (void) memset(file, 0, sizeof(*file));

    revoltc_json_get_str(file->id, json, "_id");
    revoltc_json_get_str(file->tag, json, "tag");
    revoltc_json_get_str(file->filename, json, "filename");
    revoltc_json_get_str(file->content_type, json, "content_type");
    revoltc_json_get_int(file->size, json, "size");
    buf = cJSON_GetObjectItem(json, "metadata");
    res = revolt_file_metadata_deserialize_json_obj(buf, &file->metadata);
    if (res != REVOLTE_OK)
        return res;

    revoltc_json_get_bool(file->deleted, json, "deleted");
    revoltc_json_get_bool(file->reported, json, "reported");

    revoltc_json_get_str(file->message_id, json, "message_id");
    revoltc_json_get_str(file->user_id, json, "user_id");
    revoltc_json_get_str(file->server_id, json, "server_id");

    revoltc_json_get_str(file->object_id, json, "object_id");

    return REVOLTE_OK;
}

RevoltErr revolt_file_deserialize_json(const char *json_str, RevoltFile *file) {
    RevoltErr res;
    cJSON *json;

    if (json_str == NULL)
        return REVOLTE_INVAL;

    json = cJSON_Parse(json_str);
    if (json == NULL)
        return REVOLTE_PARSE;

    res = revolt_file_deserialize_json_obj(json, file);

    cJSON_Delete(json);
    return res;
}
