#ifndef _REVOLTC_UTIL_H_INCLUDED_
#define _REVOLTC_UTIL_H_INCLUDED_

#include "revolt/common.h"

#ifdef __cplusplus
extern "C" {
#endif

void revoltc_util_u64_bytes_be(uint64_t v, uint8_t bytes_out[8]);
void revoltc_util_u32_bytes_be(uint32_t v, uint8_t bytes_out[4]);
void revoltc_util_u16_bytes_be(uint16_t v, uint8_t bytes_out[2]);

uint64_t revoltc_util_bytes_be_u64(const uint8_t bytes[8]);
uint32_t revoltc_util_bytes_be_u32(const uint8_t bytes[4]);
uint16_t revoltc_util_bytes_be_u16(const uint8_t bytes[2]);

char *revoltc_util_str_dup(const char *str);
char *revoltc_util_str_dupn(const char *str, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_UTIL_H_INCLUDED_ */
