#include "revolt/core/types.h"
#include "revolt/core/hash.h"
#include "revolt/core/util.h"

RevoltcHashMap *revoltc_hash_map_new(uint32_t bucket_count, void(*free_fn)(void*)) {
    RevoltcHashMap *map;

    if (bucket_count == 0)
        bucket_count = 128;

    if (bucket_count < 16)
        bucket_count = 16;

    map = malloc(sizeof(*map));
    if (map == NULL)
        return NULL;

    map->buckets = calloc(bucket_count, sizeof(struct RevoltcHashMapNode));
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }

    map->buckets_used = 0;
    map->bucket_count = (const uint32_t)bucket_count;
    map->free_fn = free_fn;

    return map;
}

void revoltc_hash_map_delete(RevoltcHashMap *map) {
    struct RevoltcHashMapNode *node, *next;
    uint32_t i;

    if (map == NULL)
        return;

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

    free(map->buckets);
    free(map);
}

void *revoltc_hash_map_get(const RevoltcHashMap *map, const char* key) {
    struct RevoltcHashMapNode *node;
    uint32_t i;

    if (map == NULL || key == NULL)
        return NULL;

    i = revoltc_hash_fnv1a_32((const unsigned char*) key);
    i = i % map->bucket_count;

    node = map->buckets[i];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0)
            return node->value;

        node = node->next;
    }

    return NULL;
}

RevoltErr revoltc_hash_map_insert(
    RevoltcHashMap *map,
    const char* key,
    void *value
) {
    struct RevoltcHashMapNode *new;
    uint32_t i;

    if (map == NULL || key == NULL || value == NULL)
        return REVOLTE_INVAL;

    new = malloc(sizeof(*new));
    if (new == NULL)
        return REVOLTE_NOMEM;

    if (map->buckets_used >= map->bucket_count * 0.75)
        revoltc_hash_map_grow(map, 0.25);

    i = revoltc_hash_fnv1a_32((const unsigned char*) key);
    i = i % map->bucket_count;

    if (map->buckets[i] == NULL)
        map->buckets_used++;

    new->key = revoltc_util_str_dup(key);
    new->value = value;
    new->next = map->buckets[i];
    map->buckets[i] = new;

    return REVOLTE_OK;
}

void *revoltc_hash_map_remove(RevoltcHashMap *map, const char* key) {
    struct RevoltcHashMapNode *node, *prev = NULL;
    void *val;
    uint32_t i;

    if (map == NULL || key == NULL)
        return NULL;

    i = revoltc_hash_fnv1a_32((const unsigned char*) key);
    i = i % map->bucket_count;

    node = map->buckets[i];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            val = node->value;
            if (prev == NULL) {
                map->buckets[i] = node->next;
            } else {
                prev->next = node->next;
            }

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
    uint32_t new_bucket_count;
    struct RevoltcHashMapNode *node, *next;
    uint32_t i, index;

    if (map == NULL || growth_by > 10 || growth_by < 0.01)
        return REVOLTE_INVAL;

    new_bucket_count = (uint32_t)(map->bucket_count * (1.0f + growth_by));
    new_buckets = calloc(new_bucket_count, sizeof(struct RevoltcHashMapNode*));
    if (new_buckets == NULL)
        return REVOLTE_NOMEM;

    for (i = 0; i < map->bucket_count; i++) {
        node = map->buckets[i];

        while (node != NULL) {
            next = node->next;

            index = revoltc_hash_fnv1a_32((const unsigned char*) node->key);
            index = index % new_bucket_count;

            node->next = new_buckets[index];
            new_buckets[index] = node;
            node = next;
        }
    }

    free(map->buckets);

    map->buckets = new_buckets;
    map->bucket_count = new_bucket_count;

    return REVOLTE_OK;
}

#ifdef NDEBUG

static void format_node(char *buf, size_t size, struct RevoltcHashMapNode *node) {return;}
char *revoltc_hash_map_to_str(const RevoltcHashMap *map) {return "";}

#else /*NDEBUG*/

static void format_node(char *buf, size_t size, struct RevoltcHashMapNode *node) {
    while (node) {
        snprintf(
            buf + strlen(buf),
            size - strlen(buf),
            "{k: %s, v: %p} ",
            node->key,
            node->value
        );
        node = node->next;
    }
    snprintf(buf + strlen(buf), size - strlen(buf), "\n");
}

char *revoltc_hash_map_to_str(const RevoltcHashMap *map) {
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
