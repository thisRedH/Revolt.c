#ifndef _REVOLTC_CORE_JSON_H_INCLUDED_
#define _REVOLTC_CORE_JSON_H_INCLUDED_

#include <revolt/core/defines.h>
#include <revolt/core/types.h>
#include <revolt/error.h>
#include <cjson/cJSON.h>
REVOLTC_BEGIN_C_DECLS

typedef struct RevoltcJSON RevoltcJSON;

REVOLTC_API RevoltErr revoltc_json_parse(const char* const str, size_t len, RevoltcJSON **json /*out*/);
REVOLTC_API void revoltc_json_delete(RevoltcJSON *json);

REVOLTC_API const RevoltcJSON *revoltc_json_get_obj(const RevoltcJSON *json, const char *key);
REVOLTC_API char *revoltc_json_get_strn(const RevoltcJSON *json, const char *key, size_t n);
#define revoltc_json_get_str(json, key)     revoltc_json_get_strn((json),(key),0)
REVOLTC_API int64_t revoltc_json_get_i64(const RevoltcJSON *json, const char *key);
REVOLTC_API int revoltc_json_get_int(const RevoltcJSON *json, const char *key);
REVOLTC_API double revoltc_json_get_double(const RevoltcJSON *json, const char *key);
REVOLTC_API revolt_bool revoltc_json_get_bool(const RevoltcJSON *json, const char *key);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CORE_JSON_H_INCLUDED_ */
