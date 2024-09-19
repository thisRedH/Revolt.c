#ifndef _REVOLTC_DATA_MODELS_FILE_H_INCLUDED_
#define _REVOLTC_DATA_MODELS_FILE_H_INCLUDED_

#include <revolt/common.h>
REVOLTC_BEGIN_C_DECLS

enum RevoltFileMetadataType {
    REVOLT_FILE_METADATA_TYPE_UNKNOWN = 0,
    REVOLT_FILE_METADATA_TYPE_FILE,
    REVOLT_FILE_METADATA_TYPE_TEXT,
    REVOLT_FILE_METADATA_TYPE_IMAGE,
    REVOLT_FILE_METADATA_TYPE_VIDEO,
    REVOLT_FILE_METADATA_TYPE_AUDIO
};

struct RevoltFileMetadata {
    enum RevoltFileMetadataType type;
    union RevoltFileMetadataData {
        struct RevoltFileMetadataImage {
            uint64_t width;
            uint64_t height;
        } image;
        struct RevoltFileMetadataVideo {
            uint64_t width;
            uint64_t height;
        } video;
    } data;
};

typedef struct RevoltFile {
    char *id;
    char *tag;
    char *filename;
    char *content_type;
    int64_t size;
    struct RevoltFileMetadata metadata;

    revolt_bool deleted;
    revolt_bool reported;

    char *message_id;
    char *user_id;
    char *server_id;

    char *object_id;
} RevoltFile;

REVOLTC_API void revolt_file_cleanup(RevoltFile file);
REVOLTC_API void revolt_file_delete(RevoltFile *file);
REVOLTC_API RevoltErr revolt_file_deserialize_json(const char *json_str, RevoltFile *file /*out*/);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_DATA_MODELS_FILE_H_INCLUDED_ */
