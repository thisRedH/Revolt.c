#ifndef _REVOLTC_COMMON_H_INCLUDED_
#define _REVOLTC_COMMON_H_INCLUDED_

#include <stdlib.h>
#include <stdio.h>
#include <pstdint/pstdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
    #define RVLTC_OS_WIN 1
#elif defined(linux) || defined(__linux__) || defined(__linux)
    #define RVLTC_OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define RVLTC_OS_APPLE 1
    #warning "APPLE is not supported"
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

#ifndef RVLTC_INLINE
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #define RVLTC_INLINE             inline
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
    #define RVLTC_EXPORT                        __attribute__((visibility("default")))^
#else
    #define RVLTC_EXPORT
#endif
#endif /* RVLTC_EXPORT */

#ifdef RVLTC_OS_WIN
    #define snprintf _snprintf
#endif

#define RVLTC_MAX(x, y)                         ((x) > (y) ? (x) : (y))
#define RVLTC_MIN(x, y)                         ((x) < (y) ? (x) : (y))
#define RVLTC_CLAMP(x, min, max)                RVLTC_MIN(RVLTC_MAX((x), (min)), (max))

typedef enum _revolt_bool {
    revolt_false = 0,
    revolt_true = !revolt_false
} revolt_bool;

typedef unsigned char revolt_byte;

#ifdef __cplusplus
}
#endif

#endif /* _REVOLTC_COMMON_H_INCLUDED_ */
