#ifndef _REVOLTC_COMMON_H_INCLUDED_
#define _REVOLTC_COMMON_H_INCLUDED_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_STDINT_H
    #include <stdint.h>
#else
    #include <pstdint/pstdint.h>
#endif
#include "revolt/error.h"

/* IEEE Std. 1003.1-1990 */
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE == 1L
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199009L
#endif

/* IEEE Std. 1003.2-1992 */
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE == 2L
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199209L
#endif

/* X/Open */
#ifdef _XOPEN_SOURCE
#if (_XOPEN_SOURCE - 0L >= 700L) && (!defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE - 0L < 200809L)
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#elif (_XOPEN_SOURCE - 0L >= 600L) && (!defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE - 0L < 200112L)
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200112L
#elif (_XOPEN_SOURCE - 0L >= 500L) && (!defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE - 0L < 199506L)
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199506L
#endif
#endif

#if defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 198808L
#endif

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
    #define RVLTC_OS_WIN 1
#elif defined(linux) || defined(__linux__) || defined(__linux)
    #define RVLTC_OS_LINUX 1
    #define RVLTC_OS_UNIX 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define RVLTC_OS_APPLE 1
    #if !defined(RVLTC_OS_FORCE)
        #error "APPLE is not officially supported"
    #endif
#elif defined(unix) || defined(__unix__) || defined(__unix)
    #define RVLTC_OS_UNIX 1
#else
    #error "Unknown platform. Could not detect."
#endif

/* Compiler detection */
#if defined(_MSC_VER)
    #define RVLTC_COMP_MSVC 1
#elif defined(__clang__)
    #define RVLTC_COMP_CLANG 1
    #define RVLTC_COMP_GCC_CLANG 1
#elif defined(__GNUC__)
    #define RVLTC_COMP_GCC 1
    #define RVLTC_COMP_GCC_CLANG 1
#else
    #define RVLTC_COMP_UNKNOWN 1
    #warning "Could not detect compiler"
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #define RVLTC_RESTRICT                      restrict
#else
    #define RVLTC_RESTRICT
#endif

#ifndef RVLTC_INLINE
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #define RVLTC_INLINE                        inline
#else
    #if defined(RVLTC_COMP_MSVC)
        #define RVLTC_INLINE                    __inline
    #elif defined(RVLTC_COMP_GCC_CLANG)
        #define RVLTC_INLINE                    __inline__
    #else
        #define RVLTC_INLINE
    #endif
#endif
#endif /* RVLTC_INLINE */

#ifndef RVLTC_DEPRECATED
#if defined(RVLTC_COMP_MSVC)
    #define RVLTC_DEPRECATED                    __declspec(deprecated)
#elif defined(RVLTC_COMP_GCC_CLANG)
    #define RVLTC_DEPRECATED                    __attribute__((deprecated))
#else
    #define RVLTC_DEPRECATED
#endif
#endif /* RVLTC_DEPRECATED */

#ifndef RVLTC_UNIMPLEMENTED
    #define RVLTC_UNIMPLEMENTED
#endif /* RVLTC_UNIMPLEMENTED */

#ifndef RVLTC_EXPORT
#if defined(RVLTC_OS_WIN)
    #ifdef RVLTC_BUILD_STATIC
        #define RVLTC_EXPORT
    #else
        #ifdef RVLTC_BUILD
            #define RVLTC_EXPORT                __declspec(dllexport)
        #else
            #define RVLTC_EXPORT                __declspec(dllimport)
        #endif
    #endif
#elif defined(RVLTC_OS_UNIX) && defined(RVLTC_COMP_GCC_CLANG)
    #define RVLTC_EXPORT                        __attribute__((visibility("default")))
#else
    #define RVLTC_EXPORT
#endif
#endif /* RVLTC_EXPORT */

#ifdef __cplusplus
extern "C" {
#endif

#define RVLTC_MAX(x, y)                         ((x) > (y) ? (x) : (y))
#define RVLTC_MIN(x, y)                         ((x) < (y) ? (x) : (y))
#define RVLTC_CLAMP(x, min, max)                RVLTC_MIN(RVLTC_MAX((x), (min)), (max))

#if RVLTC_DEFINE_SLEEP == 1
#if defined(RVLTC_OS_UNIX)
    #if _POSIX_C_SOURCE >= 199309L
        #include <time.h>
        #define revolt_sleep(ms) {              \
            struct timespec ts;                 \
            ts.tv_sec = (ms) / 1000;            \
            ts.tv_nsec = ((ms) % 1000) * 1000000; \
            nanosleep(&ts, NULL);               \
        }
    #elif defined(_BSD_SOURCE) || defined(_DEFAULT_SOURCE)
        #include <unistd.h>
        #define revolt_sleep(ms)                (void)usleep((unsigned int)((ms) * 1000))
    #else
        #error "Could not detect sleep implementation! Define _POSIX_C_SOURCE, _BSD_SOURCE or _DEFAULT_SOURCE before any system header in this translation unit."
    #endif
#elif defined(RVLTC_OS_WIN)
    #include <windows.h>
    #define revolt_sleep(ms)                    Sleep((DWORD)(ms))
#endif
#endif

typedef enum _revolt_bool {
    revolt_false = 0,
    revolt_true = !revolt_false
} revolt_bool;

typedef unsigned char revolt_byte;

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_COMMON_H_INCLUDED_ */
