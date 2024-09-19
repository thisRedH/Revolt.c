#ifndef _REVOLTC_CORE_JSON_H_INCLUDED_
#define _REVOLTC_CORE_JSON_H_INCLUDED_

#include <revolt/core/defines.h>
#include <cjson/cJSON.h>
REVOLTC_BEGIN_C_DECLS

#define revoltc_json_obj_get_strn(dst, json, n) \
    if (cJSON_IsString((json)) && ((json)->valuestring != NULL)) \
        (dst) = revoltc_util_str_dupn((json)->valuestring,(n))

#define revoltc_json_obj_get_str(dst, json)     \
    if (cJSON_IsString((json)) && ((json)->valuestring != NULL)) \
        (dst) = revoltc_util_str_dup((json)->valuestring)

#define revoltc_json_obj_get_int(dst, json)     \
    if (cJSON_IsNumber((json))) (dst) = ((json)->valueint)

#define revoltc_json_obj_get_double(dst, json)  \
    if (cJSON_IsNumber((json))) (dst) = ((json)->valuedouble)

#define revoltc_json_obj_get_bool(dst, json)    \
    if (cJSON_IsBool((json)) || cJSON_IsNumber((json))) \
        (dst) = ((json)->valueint)

#define revoltc_json_get_strn(dst, json, key, n)    \
    revoltc_json_obj_get_strn(dst, cJSON_GetObjectItem((json), (key)), n)

#define revoltc_json_get_str(dst, json, key)        \
    revoltc_json_obj_get_str(dst, cJSON_GetObjectItem((json), (key)))

#define revoltc_json_get_int(dst, json, key)        \
    revoltc_json_obj_get_int(dst, cJSON_GetObjectItem((json), (key)))

#define revoltc_json_get_double(dst, json, key)     \
    revoltc_json_obj_get_double(dst, cJSON_GetObjectItem((json), (key)))

#define revoltc_json_get_bool(dst, json, key)       \
    revoltc_json_obj_get_bool(dst, cJSON_GetObjectItem((json), (key)))

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CORE_JSON_H_INCLUDED_ */
