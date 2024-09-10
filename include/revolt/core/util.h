#ifndef _REVOLTC_UTIL_H_INCLUDED_
#define _REVOLTC_UTIL_H_INCLUDED_

#include "revolt/common.h"

#ifdef __cplusplus
extern "C" {
#endif

RVLTC_EXPORT void revoltc_util_u64_bytes_be(uint64_t v, uint8_t bytes_out[8]);
RVLTC_EXPORT void revoltc_util_u32_bytes_be(uint32_t v, uint8_t bytes_out[4]);
RVLTC_EXPORT void revoltc_util_u16_bytes_be(uint16_t v, uint8_t bytes_out[2]);

RVLTC_EXPORT uint64_t revoltc_util_bytes_be_u64(const uint8_t bytes[8]);
RVLTC_EXPORT uint32_t revoltc_util_bytes_be_u32(const uint8_t bytes[4]);
RVLTC_EXPORT uint16_t revoltc_util_bytes_be_u16(const uint8_t bytes[2]);

RVLTC_EXPORT char *revoltc_util_str_dup(const char *str);
RVLTC_EXPORT char *revoltc_util_str_dupn(const char *str, size_t n);

RVLTC_EXPORT void revoltc_util_str_tolower(char* str);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_UTIL_H_INCLUDED_ */
