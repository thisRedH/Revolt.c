#define REVOLTC_NAMESPACELESS_DEFINES 1
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
    if_un (NILC(str) || NILC(json))
        return REVOLTE_INVAL;

    *json = calloc(1, sizeof(**json));
    if_un (NILC(*json))
        return REVOLTE_NOMEM;

    if (ZEROC(len))
        len = strlen(str);

    (*json)->doc = yyjson_read(str, len, 0);
    if (NILC((*json)->doc)) {
        revoltc_json_delete(*json);
        *json = NULL;
        return REVOLTE_PARSE;
    }

    (*json)->root = yyjson_doc_get_root((*json)->doc);

    return REVOLTE_OK;
}

void revoltc_json_delete(RevoltcJSON *json) {
    if_un (NILC(json))
        return;

    revoltc_json_delete((RevoltcJSON*) json->child);
    json->child = NULL;

    if (!json->is_child)
        yyjson_doc_free(json->doc);

    free(json);
}

const RevoltcJSON *revoltc_json_child_new(void *val) {
    RevoltcJSON *child;

    if_un (NILC(val))
        return NULL;

    child = calloc(1, sizeof(*child));
    if_un (NILC(child))
        return NULL;

    child->root = val;
    child->is_child = revolt_true;
    return child;
}

const RevoltcJSON *revoltc_json_child(const RevoltcJSON *json, void *val) {
    if_un (NILC(json))
        return NULL;

    revoltc_json_delete((RevoltcJSON*) json->child);
    CAST_CONST(RevoltcJSON*, json)->child = revoltc_json_child_new(val);

    return json->child;
}

const RevoltcJSON *revoltc_json_get_obj(const RevoltcJSON *json, const char *key) {
    if_un (NILC(json) || NILC(key))
        return NULL;

    if (key[0] == '/') {
        return revoltc_json_child(json, yyjson_ptr_get(json->root, key));
    } else {
        return revoltc_json_child(json, yyjson_obj_get(json->root, key));
    }
}

const RevoltcJSON *revoltc_json_get_arr(const RevoltcJSON *json, const char *key) {
    const RevoltcJSON *child;

    child = revoltc_json_get_obj(json, key);
    if (NILC(child) || !yyjson_is_arr(child->root))
        return NULL;

    return child;
}

char *revoltc_json_get_strn(const RevoltcJSON *json, const char *key, size_t n) {
    yyjson_val *val;

    if_un (NILC(json) || NILC(key))
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

    if_un (NILC(json) || NILC(key))
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

    if_un (NILC(json) || NILC(key))
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

    if_un (NILC(json) || NILC(key))
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

    if_un (NILC(json) || NILC(key))
        return 0;

    if (key[0] == '/') {
        val = yyjson_ptr_get(json->root, key);
    } else {
        val = yyjson_obj_get(json->root, key);
    }

    if (yyjson_is_bool(val))
        return yyjson_get_bool(val);

    if (yyjson_is_num(val))
        return REVOLTC_BOOL_CLAMP(yyjson_get_num(val));

    return 0;
}

const RevoltcJSON *revoltc_json_arr_get(const RevoltcJSON *arr, size_t idx) {
    if_un (NILC(arr) || !yyjson_is_arr(arr->root))
        return NULL;

    return revoltc_json_child(arr, yyjson_arr_get(arr->root, idx));
}

size_t revoltc_json_arr_size(const RevoltcJSON *arr) {
    if_un (NILC(arr))
        return 0;

    return yyjson_arr_size(arr->root);
}

const RevoltcJSON *revoltc_json_unsafe_next(const RevoltcJSON *json) {
    if_un (NILC(json))
        return NULL;

    return revoltc_json_child(json, unsafe_yyjson_get_next(json->root));
}

#endif
