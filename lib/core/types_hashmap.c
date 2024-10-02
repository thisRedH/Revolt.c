#define REVOLTC_NAMESPACELESS_DEFINES 1
#include "revolt/core/types.h"
#include "revolt/core/hash.h"
#include "revolt/core/util.h"

#ifndef REVOLTC_HASH_MAP_HASH
#define REVOLTC_HASH_MAP_HASH revoltc_hash_sdbm
#endif

#define hash_map_buckets_index(bucket_count, key) \
    (REVOLTC_HASH_MAP_HASH((const unsigned char*)(key)) % (bucket_count))

#define hash_map_index(map, key) \
    hash_map_buckets_index((map)->bucket_count, key)

RevoltcHashMap *revoltc_hash_map_new(uint32_t bucket_count, void(*free_fn)(void*)) {
    RevoltcHashMap *map;

    if_un (ZEROC(bucket_count))
        bucket_count = 128;

    if_un (bucket_count < 16)
        bucket_count = 16;

    map = calloc(1, sizeof(*map));
    if (map == NULL)
        return NULL;

    map->buckets = calloc(bucket_count, sizeof(struct RevoltcHashMapNode));
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }

    map->bucket_count = (const uint32_t)bucket_count;
    map->free_fn = free_fn;

    return map;
}

void revoltc_hash_map_delete(RevoltcHashMap *map) {
    struct RevoltcHashMapNode *node, *next;
    uint32_t i;

    if_un (NILC(map))
        return;

    if (map->buckets != NULL) {
        for (i = 0; i < map->bucket_count; i++) {
            node = map->buckets[i];

            while (node != NULL) {
                free(node->key);
                if (map->free_fn != NULL)
                    map->free_fn(node->value);

                next = node->next;
                free(node);
                node = next;
            }
        }
    }

    free(map->buckets);
    free(map);
}

static REVOLTC_INLINE struct RevoltcHashMapNode *hash_map_get_node(
    const RevoltcHashMap *map,
    const char* key
) {
    struct RevoltcHashMapNode *node;
    uint32_t i;

    if_un (NILC(map) || NILC(key))
        return NULL;

    i = hash_map_index(map, key);
    node = map->buckets[i];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0)
            return node;

        node = node->next;
    }

    return NULL;
}

void *revoltc_hash_map_get(const RevoltcHashMap *map, const char* key) {
    struct RevoltcHashMapNode *node;

    node = hash_map_get_node(map, key);
    if (node != NULL)
        return node->value;

    return NULL;
}

static REVOLTC_INLINE RevoltErr hash_map_buckets_insert(
    struct RevoltcHashMapNode **buckets,
    uint32_t bucket_count,
    char* key,
    void *value
) {
    struct RevoltcHashMapNode *new;
    uint32_t i;

    if_un (NILC(buckets) || NILC(key) || NILC(value))
        return REVOLTE_INVAL;

    new = malloc(sizeof(*new));
    if (NILC(new))
        return REVOLTE_NOMEM;

    i = hash_map_buckets_index(bucket_count, key);

    new->key = key;
    new->value = value;
    new->next = buckets[i];
    buckets[i] = new;

    return REVOLTE_OK;
}

RevoltErr revoltc_hash_map_insert(
    RevoltcHashMap *map,
    const char* key,
    void *value
) {
    RevoltErr res;

    if_un (NILC(map))
        return REVOLTE_INVAL;

    if (map->count >= map->bucket_count * 0.75) {
        if (map->bucket_count <= 1024) {
            res = revoltc_hash_map_grow(map, 1.00);
        } else if (map->bucket_count <= 16384) {
            res = revoltc_hash_map_grow(map, 0.50);
        } else {
            res = revoltc_hash_map_grow(map, 0.25);
        }

        if (res != REVOLTE_OK)
            return res;
    }

    map->count++;
    return hash_map_buckets_insert(
        map->buckets,
        map->bucket_count,
        revoltc_util_str_dup(key),
        value
    );
}

void *revoltc_hash_map_remove(RevoltcHashMap *map, const char* key) {
    struct RevoltcHashMapNode *node, *prev = NULL;
    uint32_t i;
    void *val;

    if_un (NILC(map) || NILC(key))
        return NULL;

    i = hash_map_index(map, key);
    node = map->buckets[i];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (prev == NULL) {
                map->buckets[i] = node->next;
            } else {
                prev->next = node->next;
            }

            val = node->value;
            free(node->key);
            free(node);
            return val;
        }

        prev = node;
        node = node->next;
    }

    return NULL;
}

RevoltErr revoltc_hash_map_grow(RevoltcHashMap *map, float growth_by) {
    struct RevoltcHashMapNode **new_buckets;
    struct RevoltcHashMapNode *node;
    uint32_t new_bucket_count, new_count = 0;
    RevoltErr res;
    uint32_t i;

    if_un (NILC(map) || growth_by > 10 || growth_by < 0.01)
        return REVOLTE_INVAL;

    new_bucket_count = map->bucket_count * (1.0f + growth_by);
    new_buckets = calloc(new_bucket_count, sizeof(struct RevoltcHashMapNode*));
    if_un (NILC(new_buckets))
        return REVOLTE_NOMEM;

    if (map->buckets != NULL) {
        for (i = 0; i < map->bucket_count; i++) {
            while (map->buckets[i] != NULL) {
                node = map->buckets[i];
                map->buckets[i] = node->next;

                new_count++;
                res = hash_map_buckets_insert(
                    new_buckets,
                    new_bucket_count,
                    node->key,
                    node->value
                );

                free(node);
                if (res != REVOLTE_OK)
                    return res;
            }
        }
    }

    free(map->buckets);
    map->buckets = new_buckets;
    map->bucket_count = new_bucket_count;
    map->count = new_count;
    return REVOLTE_OK;
}

#if defined(NDEBUG) || !defined(HAVE_SNPRINTF)
char *revoltc_hash_map_str_visualize(const RevoltcHashMap *map) {return "";}
#else /*NDEBUG*/
#include "revolt/core/common.h"

static void format_node(char *buf, size_t size, struct RevoltcHashMapNode *node) {
    while (node) {
        snprintf(
            buf + strlen(buf),
            size - strlen(buf),
            "{%s = %p} ",
            node->key,
            node->value
        );
        node = node->next;
    }
    snprintf(buf + strlen(buf), size - strlen(buf), "\n");
}

char *revoltc_hash_map_str_visualize(const RevoltcHashMap *map) {
    size_t size = 8192;
    char *buf;
    uint32_t i;

    buf = malloc(size);
    buf[0] = '\0';

    for (i = 0; i < map->bucket_count; i++) {
        if (map->buckets[i] != NULL) {
            snprintf(buf + strlen(buf), size - strlen(buf), "[%u]: ", i);
            format_node(buf, size, map->buckets[i]);
        }
    }

    return buf;
}
#endif /*NDEBUG*/
