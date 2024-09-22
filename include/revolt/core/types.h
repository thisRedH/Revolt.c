#ifndef _REVOLTC_CORE_TYPES_H_INCLUDED_
#define _REVOLTC_CORE_TYPES_H_INCLUDED_

#ifdef HAVE_STDINT_H
    #include <stdint.h>
#else
    #include <pstdint/pstdint.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <revolt/core/defines.h>
#include <revolt/error.h>
REVOLTC_BEGIN_C_DECLS

typedef enum _revolt_bool {
    revolt_false = 0,
    revolt_true = !revolt_false
} revolt_bool;

typedef unsigned char revolt_byte;


struct RevoltcHashMapNode {
    char *key;
    void *value;
    struct RevoltcHashMapNode *next;
};

typedef struct RevoltcHashMap {
    struct RevoltcHashMapNode **buckets;
    uint32_t count;
    uint32_t bucket_count;
    void(*free_fn)(void*);
} RevoltcHashMap;

REVOLTC_API RevoltcHashMap *revoltc_hash_map_new(uint32_t bucket_count, void(*free_fn)(void*));
REVOLTC_API void revoltc_hash_map_delete(RevoltcHashMap *map);

REVOLTC_API void *revoltc_hash_map_get(const RevoltcHashMap *map, const char* key);
REVOLTC_API RevoltErr revoltc_hash_map_insert(RevoltcHashMap *map, const char* key, void *value);
REVOLTC_API void *revoltc_hash_map_remove(RevoltcHashMap *map, const char* key);

#define revoltc_hash_map_remove_free(map, key) {                \
    if ((map) != NULL && (map)->free_fn != NULL)                \
        (map)->free_fn(revoltc_hash_map_remove((map),(key)));   \
    else                                                        \
        (void)revoltc_hash_map_remove((map),(key));             \
}

REVOLTC_API RevoltErr revoltc_hash_map_grow(RevoltcHashMap *map, float growth_by);


struct RevoltcLRUCacheNode {
    char *key;
    void *value;
    struct RevoltcLRUCacheNode *prev;
    struct RevoltcLRUCacheNode *next;
};

typedef struct RevoltcLRUCache {
    RevoltcHashMap *map;
    struct RevoltcLRUCacheNode *head;
    struct RevoltcLRUCacheNode *tail;
    uint32_t count;
    uint32_t capacity;
    void(*free_fn)(void*);
} RevoltcLRUCache;

REVOLTC_API RevoltcLRUCache *revoltc_lru_cache_new(uint32_t capacity, void(*free_fn)(void*));
REVOLTC_API void revoltc_lru_cache_delete(RevoltcLRUCache *cache);

REVOLTC_API void *revoltc_lru_cache_get(RevoltcLRUCache *cache, const char *key);
REVOLTC_API RevoltErr revoltc_lru_cache_put(RevoltcLRUCache *cache, const char *key, void *value);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CORE_TYPES_H_INCLUDED_ */
