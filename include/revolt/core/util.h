#ifndef _REVOLTC_CORE_UTIL_H_INCLUDED_
#define _REVOLTC_CORE_UTIL_H_INCLUDED_

#include <revolt/core/defines.h>
#include <revolt/core/types.h>

/* TODO: move to time or sleep file */
#if REVOLTC_DEFINE_SLEEP == 1
#if defined(REVOLTC_OS_UNIX)
    #if _POSIX_C_SOURCE >= 199309L
        #include <time.h>
        #define revolt_sleep(ms) {              \
            struct timespec ts;                 \
            ts.tv_sec = (ms) / 1000;            \
            ts.tv_nsec = ((ms) % 1000) * 1000000; \
            nanosleep(&ts, NULL);               \
        }
    #else
        #include <unistd.h>
        #define revolt_sleep(ms)                (void)usleep((unsigned int)((ms) * 1000))
    #endif
#elif defined(REVOLTC_OS_WIN)
    #include <windows.h>
    #define revolt_sleep(ms)                    Sleep((DWORD)(ms))
#endif
#endif

#define REVOLTC_NULL_CHECK(ptr)     ((ptr) == NULL)
#define REVOLTC_ZERO_CHECK(num)     ((num) == 0)

#if REVOLTC_NAMESPACELESS_DEFINES
    #define NILC(ptr)               REVOLTC_NULL_CHECK(ptr)
    #define ZEROC(num)              REVOLTC_ZERO_CHECK(num)
    #define if_un(expr)             if (REVOLTC_UNLIKELY(expr))
    #define if_li(expr)             if (REVOLTC_LIKELY(expr))
#endif /* REVOLTC_NAMESPACELESS_DEFINES */

REVOLTC_BEGIN_C_DECLS

REVOLTC_API void revoltc_util_u64_bytes_be(uint64_t v, uint8_t bytes_out[8]);
REVOLTC_API void revoltc_util_u32_bytes_be(uint32_t v, uint8_t bytes_out[4]);
REVOLTC_API void revoltc_util_u16_bytes_be(uint16_t v, uint8_t bytes_out[2]);

REVOLTC_API uint64_t revoltc_util_bytes_be_u64(const uint8_t bytes[8]);
REVOLTC_API uint32_t revoltc_util_bytes_be_u32(const uint8_t bytes[4]);
REVOLTC_API uint16_t revoltc_util_bytes_be_u16(const uint8_t bytes[2]);

REVOLTC_API char *revoltc_util_str_dup(const char *str);
REVOLTC_API char *revoltc_util_str_dupn(const char *str, size_t n);

REVOLTC_API void revoltc_util_str_tolower(char* str);

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CORE_UTIL_H_INCLUDED_ */
