#include "revolt/core/json.h"
#include "revolt/core/types.h"
#include "revolt/core/util.h"

#if REVOLTC_WITH_CJSON
#include <cjson/cJSON.h>

static const char* next_token(const char* ptr);
static cJSON* cjson_ptr_get(cJSON* json, const char* ptr);

struct RevoltcJSON {
    cJSON *root;

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

    (*json)->root = cJSON_Parse(str);
    if ((*json)->root == NULL) {
        revoltc_json_delete(*json);
        *json = NULL;
        return REVOLTE_PARSE;
    }

    return REVOLTE_OK;
}

void revoltc_json_delete(RevoltcJSON *json) {
    if (json == NULL)
        return;

    revoltc_json_delete((RevoltcJSON*) json->child);
    json->child = NULL;

    if (!json->is_child)
        cJSON_Delete(json->root);

    free(json);
}

const RevoltcJSON *revoltc_json_child_new(void *val) {
    RevoltcJSON *child;

    if (val == NULL)
        return NULL;

    child = calloc(1, sizeof(*child));
    if (child == NULL)
        return NULL;

    child->root = val;
    child->is_child = revolt_true;
    return child;
}

const RevoltcJSON *revoltc_json_child(const RevoltcJSON *json, void *val) {
    if (json == NULL)
        return NULL;

    revoltc_json_delete((RevoltcJSON*) json->child);
    REVOLTC_CCONST(RevoltcJSON*, json)->child = revoltc_json_child_new(val);

    return json->child;
}

const RevoltcJSON *revoltc_json_get_obj(const RevoltcJSON *json, const char *key) {
    if (json == NULL || key == NULL)
        return NULL;

    if (key[0] == '/') {
        return revoltc_json_child(json, cjson_ptr_get(json->root, key));
    } else {
        return revoltc_json_child(json, cJSON_GetObjectItem(json->root, key));
    }
}

const RevoltcJSON *revoltc_json_get_arr(const RevoltcJSON *json, const char *key) {
    const RevoltcJSON *child;

    child = revoltc_json_get_obj(json, key);
    if (child == NULL || !cJSON_IsArray(child->root))
        return NULL;

    return child;
}

char *revoltc_json_get_strn(const RevoltcJSON *json, const char *key, size_t n) {
    const cJSON *obj;

    if (json == NULL || key == NULL)
        return NULL;

    if (key[0] == '/') {
        obj = cjson_ptr_get(json->root, key);
    } else {
        obj = cJSON_GetObjectItem(json->root, key);
    }

    if (cJSON_IsString(obj) && (obj->valuestring != NULL)) {
        if (n == 0) {
            return revoltc_util_str_dup(obj->valuestring);
        } else {
            return revoltc_util_str_dupn(obj->valuestring, n);
        }
    }

    return NULL;
}

int64_t revoltc_json_get_i64(const RevoltcJSON *json, const char *key) {
    const cJSON *obj;
    double value;

    if (json == NULL || key == NULL)
        return 0;

    if (key[0] == '/') {
        obj = cjson_ptr_get(json->root, key);
    } else {
        obj = cJSON_GetObjectItem(json->root, key);
    }

    if (!cJSON_IsNumber(obj))
        return 0;

    value = cJSON_GetNumberValue(obj);
    if (value < (double)INT64_MIN || value > (double)INT64_MAX)
        return 0;

    return (int64_t) value;
}

int revoltc_json_get_int(const RevoltcJSON *json, const char *key) {
    const cJSON *obj;

    if (json == NULL || key == NULL)
        return 0;

    if (key[0] == '/') {
        obj = cjson_ptr_get(json->root, key);
    } else {
        obj = cJSON_GetObjectItem(json->root, key);
    }

    if (cJSON_IsNumber(obj))
        return obj->valueint;

    return 0;
}

double revoltc_json_get_double(const RevoltcJSON *json, const char *key) {
    const cJSON *obj;

    if (json == NULL || key == NULL)
        return 0;

    if (key[0] == '/') {
        obj = cjson_ptr_get(json->root, key);
    } else {
        obj = cJSON_GetObjectItem(json->root, key);
    }

    if (cJSON_IsNumber(obj))
        return cJSON_GetNumberValue(obj);

    return 0;
}

revolt_bool revoltc_json_get_bool(const RevoltcJSON *json, const char *key) {
    const cJSON *obj;

    if (json == NULL || key == NULL)
        return revolt_false;

    if (key[0] == '/') {
        obj = cjson_ptr_get(json->root, key);
    } else {
        obj = cJSON_GetObjectItem(json->root, key);
    }

    if (cJSON_IsBool(obj) || cJSON_IsNumber(obj))
        return REVOLTC_BOOL_CLAMP(obj->valuedouble);

    return revolt_false;
}

const RevoltcJSON *revoltc_json_arr_get(const RevoltcJSON *arr, size_t idx) {
    if (arr == NULL || !cJSON_IsArray(arr->root))
        return NULL;

    return revoltc_json_child(arr, cJSON_GetArrayItem(arr->root, idx));
}

size_t revoltc_json_arr_size(const RevoltcJSON *arr) {
    if (arr == NULL)
        return 0;

    return cJSON_GetArraySize(arr->root);
}

const RevoltcJSON *revoltc_json_unsafe_next(const RevoltcJSON *json) {
    if (json == NULL)
        return NULL;

    return revoltc_json_child(json, json->root->next);
}

static const char* next_token(const char* ptr) {
    const char* next;

    if (ptr == NULL || *ptr == '\0')
        return NULL;

    next = strchr(ptr + 1, '/');
    return next ? next : ptr + strlen(ptr);
}

static cJSON* cjson_ptr_get(cJSON* json, const char* ptr) {
    char token[256];
    const char* next;
    char* endptr;
    long i;

    if (json == NULL || ptr == NULL || *ptr != '/')
        return NULL;

    ptr++;
    while (*ptr) {
        next = next_token(ptr);
        if (next - ptr >= (long) sizeof(token))
            return NULL;

        (void) strncpy(token, ptr, next - ptr);
        token[next - ptr] = '\0';

        i = strtol(token, &endptr, 10);
        if (*endptr == '\0') {
            json = cJSON_GetArrayItem(json, i);
        } else {
            json = cJSON_GetObjectItem(json, token);
        }

        if (json == NULL)
            return NULL;

        ptr = next;
        if (*ptr == '/')
            ptr++;
    }

    return json;
}

#endif
