#define REVOLTC_NAMESPACELESS_DEFINES 1
#include "revolt/core/types.h"
#include "revolt/core/util.h"

RevoltcLRUCache *revoltc_lru_cache_new(
    uint32_t capacity,
    void(*free_fn)(void*)
) {
    RevoltcLRUCache *cache;

    if_un (capacity < 8)
        capacity = 8;

    cache = calloc(1, sizeof(*cache));
    if_un (NILC(cache))
        return NULL;

    cache->map = revoltc_hash_map_new(capacity + 1, free_fn);
    cache->capacity = capacity;
    cache->free_fn = free_fn;
    return cache;
}

void revoltc_lru_cache_delete(RevoltcLRUCache *cache) {
    struct RevoltcLRUCacheNode *node;

    if_un (NILC(cache))
        return;

    while (cache->head) {
        node = cache->head;
        cache->head = node->next;

        if (cache->free_fn != NULL)
            cache->free_fn(node->value);

        free(node->key);
        free(node);
    }

    revoltc_hash_map_delete(cache->map);
    free(cache);
}

REVOLTC_INLINE void lru_cache_move_to_head(
    RevoltcLRUCache *cache,
    struct RevoltcLRUCacheNode *node
) {
    if (cache->head != node) {
        if (cache->head != NULL)
            cache->head->prev = node;

        if (node == cache->tail)
            cache->tail = node->prev;

        if (node->prev != NULL)
            node->prev->next = node->next;

        if (node->next != NULL)
            node->next->prev = node->prev;

        node->prev = NULL;
        node->next = cache->head;

        cache->head = node;
    }
}

void *revoltc_lru_cache_get(RevoltcLRUCache *cache, const char *key) {
    struct RevoltcLRUCacheNode* node;

    if_un (NILC(cache) || NILC(key))
        return NULL;

    node = revoltc_hash_map_get(cache->map, key);
    if_un (NILC(node))
        return NULL;

    lru_cache_move_to_head(cache, node);
    return node->value;
}


REVOLTC_API RevoltErr revoltc_lru_cache_put(RevoltcLRUCache *cache, const char *key, void *value) {
    struct RevoltcLRUCacheNode* node;
    RevoltErr res;

    if_un (NILC(cache) || NILC(key) || NILC(value))
        return REVOLTE_INVAL;

    node = revoltc_hash_map_get(cache->map, key);
    if (!NILC(node)) {
        node->value = value;
        lru_cache_move_to_head(cache, node);
        return REVOLTE_OK;
    }

    node = malloc(sizeof(*node));
    node->key = revoltc_util_str_dup(key);
    node->value = value;
    node->prev = NULL;
    node->next = cache->head;

    if (NILC(cache->tail))
        cache->tail = node;

    if (!NILC(cache->head))
        cache->head->prev = node;
    cache->head = node;

    res = revoltc_hash_map_insert(cache->map, key, node);
    if (res != REVOLTE_OK)
        return res;

    cache->count++;
    if (cache->count > cache->capacity) {
        node = cache->tail;

        if (!NILC(cache->tail->prev))
            cache->tail->prev->next = NULL;
        cache->tail = cache->tail->prev;

        revoltc_hash_map_remove_free(cache->map, node->key);
        free(node->key);
        free(node);

        cache->count--;
    }

    return REVOLTE_OK;
}

#if defined(NDEBUG) || !defined(HAVE_SNPRINTF)
char *revoltc_lru_cache_str_visualize(const RevoltcLRUCache *cache) {return "";}
#else /*NDEBUG*/
#include "revolt/core/common.h"

char *revoltc_lru_cache_str_visualize(const RevoltcLRUCache *cache) {
    const struct RevoltcLRUCacheNode *node;
    size_t size = 8192;
    char *buf;

    buf = malloc(size);
    buf[0] = '\0';

    node = cache->head;
    while (node) {
        snprintf(
            buf + strlen(buf),
            size - strlen(buf),
            "%s = %p -> ",
            node->key,
            node->value
        );
        node = node->next;
    }

    snprintf(buf + strlen(buf), size - strlen(buf), "NULL\n");
    return buf;
}
#endif /*NDEBUG*/
