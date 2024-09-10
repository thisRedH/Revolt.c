#include "revolt/core/util.h"

void revoltc_util_u64_bytes_be(uint64_t v, uint8_t bytes_out[8]) {
    int i;
    for (i = 0; i < 8; i++)
        bytes_out[7 - i] = (v >> (i * 8)) & 0xFF;
}

void revoltc_util_u32_bytes_be(uint32_t v, uint8_t bytes_out[4]) {
    int i;
    for (i = 0; i < 4; i++)
        bytes_out[3 - i] = (v >> (i * 8)) & 0xFF;
}

void revoltc_util_u16_bytes_be(uint16_t v, uint8_t bytes_out[2]) {
    bytes_out[0] = (v >> 8) & 0xFF;
    bytes_out[1] = (v >> 0) & 0xFF; 
}

uint64_t revoltc_util_bytes_be_u64(const uint8_t bytes[8]) {
    return
        ((uint64_t)bytes[0]) << 56 |
        ((uint64_t)bytes[1]) << 48 |
        ((uint64_t)bytes[2]) << 40 |
        ((uint64_t)bytes[3]) << 32 |
        ((uint64_t)bytes[4]) << 24 |
        ((uint64_t)bytes[5]) << 16 |
        ((uint64_t)bytes[6]) << 8  |
        ((uint64_t)bytes[7]) << 0;
}

uint32_t revoltc_util_bytes_be_u32(const uint8_t bytes[4]) {
    return
        ((uint32_t)bytes[0]) << 24 |
        ((uint32_t)bytes[1]) << 16 |
        ((uint32_t)bytes[2]) << 8  |
        ((uint32_t)bytes[3]) << 0;
}

uint16_t revoltc_util_bytes_be_u16(const uint8_t bytes[2]) {
    return
        ((uint16_t)bytes[0]) << 8 |
        ((uint16_t)bytes[1]) << 0;
}

void str_tolower(char* str) {
    for (; *str; str++) { *str = tolower(*str); }
}

char *revoltc_util_str_dup(const char *str) {
    char *dup = malloc(strlen(str) + 1);
    if (dup != NULL)
        (void) strcpy(dup, str);

    return dup;
}

char *revoltc_util_str_dupn(const char *str, size_t n) {
    char *dup = malloc(n + 1);
    if (dup != NULL) {
        (void) strncpy(dup, str, n);
        dup[n] = '\0';
    }
    return dup;
}

void revoltc_util_str_tolower(char* str) {
    for (; *str; str++) { *str = tolower(*str); }
}
