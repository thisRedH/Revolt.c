#include "revolt/core/json.h"
#include "revolt/core/util.h"

#if REVOLTC_WITH_YYJSON
#include <yyjson.h>

struct RevoltcJSON {
    yyjson_doc *doc;
    yyjson_val *root;

    revolt_bool is_child;
    const RevoltcJSON *child;
};

RevoltErr revoltc_json_parse(const char* const str, size_t len, RevoltcJSON **json) {
    if (str == NULL || json == NULL)
        return REVOLTE_INVAL;

    *json = calloc(1, sizeof(**json));
    if (*json == NULL)
        return REVOLTE_NOMEM;

    if (len == 0)
        len = strlen(str);

    (*json)->doc = yyjson_read(str, len, 0);
    if ((*json)->doc == NULL) {
        revoltc_json_delete(*json);
        *json = NULL;
        return REVOLTE_PARSE;
    }

    (*json)->root = yyjson_doc_get_root((*json)->doc);

    return REVOLTE_OK;
}

void revoltc_json_delete(RevoltcJSON *json) {
    if (json == NULL)
        return;

    revoltc_json_delete((RevoltcJSON*) json->child);
    json->child = NULL;

    if (!json->is_child)
        yyjson_doc_free(json->doc);

    free(json);
}

const RevoltcJSON *revoltc_json_get_obj(const RevoltcJSON *json, const char *key) {
    RevoltcJSON *child;

    if (json == NULL || key == NULL)
        return NULL;

    child = calloc(1, sizeof(*child));
    if (child == NULL)
        return NULL;

    child->is_child = revolt_true;
    ((RevoltcJSON*)json)->child = child;

    if (key[0] == '/') {
        child->root = yyjson_ptr_get(json->root, key);
    } else {
        child->root = yyjson_obj_get(json->root, key);
    }

    return child;
}

char *revoltc_json_get_strn(const RevoltcJSON *json, const char *key, size_t n) {
    yyjson_val *val;

    if (json == NULL || key == NULL)
        return NULL;

    if (key[0] == '/') {
        val = yyjson_ptr_get(json->root, key);
    } else {
        val = yyjson_obj_get(json->root, key);
    }

    if (yyjson_is_str(val)) {
        if (n == 0) {
            return revoltc_util_str_dup(yyjson_get_str(val));
        } else {
            return revoltc_util_str_dupn(yyjson_get_str(val), n);
        }
    }

    return NULL;
}

int64_t revoltc_json_get_i64(const RevoltcJSON *json, const char *key) {
    yyjson_val *val;

    if (json == NULL || key == NULL)
        return 0;

    if (key[0] == '/') {
        val = yyjson_ptr_get(json->root, key);
    } else {
        val = yyjson_obj_get(json->root, key);
    }

    if (yyjson_is_int(val))
        return yyjson_get_sint(val);

    return 0;
}

int revoltc_json_get_int(const RevoltcJSON *json, const char *key) {
    yyjson_val *val;

    if (json == NULL || key == NULL)
        return 0;

    if (key[0] == '/') {
        val = yyjson_ptr_get(json->root, key);
    } else {
        val = yyjson_obj_get(json->root, key);
    }

    if (yyjson_is_int(val))
        return yyjson_get_int(val);

    return 0;
}

double revoltc_json_get_double(const RevoltcJSON *json, const char *key) {
    yyjson_val *val;

    if (json == NULL || key == NULL)
        return 0;

    if (key[0] == '/') {
        val = yyjson_ptr_get(json->root, key);
    } else {
        val = yyjson_obj_get(json->root, key);
    }

    if (yyjson_is_real(val))
        return yyjson_get_real(val);

    return 0;
}

revolt_bool revoltc_json_get_bool(const RevoltcJSON *json, const char *key) {
    yyjson_val *val;

    if (json == NULL || key == NULL)
        return 0;

    if (key[0] == '/') {
        val = yyjson_ptr_get(json->root, key);
    } else {
        val = yyjson_obj_get(json->root, key);
    }

    if (yyjson_is_bool(val))
        return yyjson_get_bool(val);

    if (yyjson_is_num(val) && (
        yyjson_get_num(val) == 0 || yyjson_get_num(val) == 1
    )) return (revolt_bool)yyjson_get_num(val);

    return 0;
}

#endif
