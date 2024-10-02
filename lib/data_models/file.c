#define REVOLTC_NAMESPACELESS_DEFINES 1
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

RevoltErr revolt_file_metadata_deserialize_json_obj(const RevoltcJSON *json, struct RevoltFileMetadata *metadata) {
    char *type;

    if_un (NILC(json) || NILC(metadata))
        return REVOLTE_INVAL;

    (void) memset(metadata, 0, sizeof(*metadata));

    type = revoltc_json_get_str(json, "type");
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
                metadata->data.image.width = revoltc_json_get_int(json, "width");
                metadata->data.image.height = revoltc_json_get_int(json, "height");
                break;
            case 'v':
                metadata->type = REVOLT_FILE_METADATA_TYPE_VIDEO;
                metadata->data.video.width = revoltc_json_get_int(json, "width");
                metadata->data.video.height = revoltc_json_get_int(json, "height");
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

RevoltErr revolt_file_deserialize_json_obj(const RevoltcJSON *json, RevoltFile *file) {
    RevoltErr res;
    const RevoltcJSON *buf;

    if_un (NILC(json) || NILC(file))
        return REVOLTE_INVAL;

    (void) memset(file, 0, sizeof(*file));

    file->id = revoltc_json_get_str(json, "_id");
    file->tag = revoltc_json_get_str(json, "tag");
    file->filename = revoltc_json_get_str(json, "filename");
    file->content_type = revoltc_json_get_str(json, "content_type");
    file->size = revoltc_json_get_int(json, "size");
    buf = revoltc_json_get_obj(json, "metadata");
    res = revolt_file_metadata_deserialize_json_obj(buf, &file->metadata);
    if (res != REVOLTE_OK)
        return res;

    file->deleted = revoltc_json_get_bool(json, "deleted");
    file->reported = revoltc_json_get_bool(json, "reported");

    file->message_id = revoltc_json_get_str(json, "message_id");
    file->user_id = revoltc_json_get_str(json, "user_id");
    file->server_id = revoltc_json_get_str(json, "server_id");

    file->object_id = revoltc_json_get_str(json, "object_id");

    return REVOLTE_OK;
}

RevoltErr revolt_file_deserialize_json(const char *json_str, RevoltFile *file) {
    RevoltErr res;
    RevoltcJSON *json;

    if_un (NILC(json_str))
        return REVOLTE_INVAL;

    res = revoltc_json_parse(json_str, 0, &json);
    if (res != REVOLTE_OK)
        return res;

    res = revolt_file_deserialize_json_obj(json, file);

    revoltc_json_delete(json);
    return res;
}
