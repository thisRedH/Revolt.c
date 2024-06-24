#ifndef _REVOLTC_DATA_MODELS_EMOJI_H_INCLUDED_
#define _REVOLTC_DATA_MODELS_EMOJI_H_INCLUDED_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RevoltEmojiParent {
    revolt_bool detached;
    const char *server_id;
};

typedef struct RevoltEmoji {
    const char *id;
    const char *name;
    const char *creator_id;
    struct RevoltEmojiParent parent;
    revolt_bool animated;
    revolt_bool nsfw;
} RevoltEmoji;

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_DATA_MODELS_EMOJI_H_INCLUDED_ */
