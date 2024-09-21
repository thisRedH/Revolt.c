#ifndef _REVOLTC_CORE_HASH_H_INCLUDED_
#define _REVOLTC_CORE_HASH_H_INCLUDED_

#include <revolt/core/defines.h>
#include <revolt/core/types.h>
REVOLTC_BEGIN_C_DECLS

static REVOLTC_INLINE uint32_t revoltc_hash_sdbm(const unsigned char* str) {
    uint32_t hash = 0;
    unsigned char c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

static REVOLTC_INLINE uint32_t revoltc_hash_djb2(const unsigned char* str) {
    uint32_t hash = 5381;
    unsigned char c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

static REVOLTC_INLINE uint32_t revoltc_hash_fnv1a_32(const unsigned char* str) {
    uint32_t hash = 0x811c9dc5;

    while (*str) {
        hash ^= (uint32_t)(*str++);
        hash *= 0x01000193;
    }

    return hash;
}

static REVOLTC_INLINE uint64_t revoltc_hash_fnv1a_64(const unsigned char* str) {
    uint64_t hash = 0xcbf29ce484222325;

    while (*str) {
        hash ^= (uint64_t)(*str++);
        hash *= 0x100000001b3;
    }

    return hash;
}

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CORE_HASH_H_INCLUDED_ */
