#ifndef _REVOLTC_CORE_DEFINES_H_INCLUDED_
#define _REVOLTC_CORE_DEFINES_H_INCLUDED_

#include <hedley/hedley.h>

#define REVOLTC_MESSAGE(msg)        HEDLEY_MESSAGE(msg)
#define REVOLTC_WARNING(msg)        HEDLEY_WARNING(msg)

#define REVOLTC_LIKELY(expr)        HEDLEY_LIKELY(expr)
#define REVOLTC_UNLIKELY            HEDLEY_UNLIKELY(expr)
#define REVOLTC_MALLOC              HEDLEY_MALLOC
#define REVOLTC_PURE                HEDLEY_PURE
#define REVOLTC_CONST               HEDLEY_CONST
#define REVOLTC_RESTRICT            HEDLEY_RESTRICT
#define REVOLTC_INLINE              HEDLEY_INLINE
#define REVOLTC_ALWAYS_INLINE       HEDLEY_ALWAYS_INLINE
#define REVOLTC_NEVER_INLINE        HEDLEY_NEVER_INLINE

#define REVOLTC_BEGIN_C_DECLS       HEDLEY_BEGIN_C_DECLS
#define REVOLTC_END_C_DECLS         HEDLEY_END_C_DECLS
#define REVOLTC_CAST_CONST(T,expr)  HEDLEY_CONST_CAST(T,expr)
#define REVOLTC_CAST_REINT(T,expr)  HEDLEY_REINTERPRET_CAST(T,expr)
#define REVOLTC_CAST_STATIC(T,expr) HEDLEY_STATIC_CAST(T,expr)

#define REVOLTC_DEPRECATED(since)   HEDLEY_DEPRECATED(since)
#define REVOLTC_PRIVATE             HEDLEY_PRIVATE

#if defined(REVOLTC_BUILD)
    #define REVOLTC_API             HEDLEY_PUBLIC
#else
    #define REVOLTC_API             HEDLEY_IMPORT
#endif

/* Compiler detection */
#if defined(HEDLEY_MSVC_VERSION)
    #define REVOLTC_COMP_MSVC       HEDLEY_MSVC_VERSION
#elif defined(HEDLEY_GCC_VERSION)
    #define REVOLTC_COMP_GCC        HEDLEY_GCC_VERSION
    #define REVOLTC_COMP_GNUC       HEDLEY_GNUC_VERSION
#elif defined(HEDLEY_GNUC_VERSION)
    #define REVOLTC_COMP_GNUC       HEDLEY_GNUC_VERSION
    #if defined(__clang__)
        #define REVOLTC_COMP_CLANG  HEDLEY_VERSION_ENCODE(__clang__, __clang_minor__, __clang_patchlevel__)
    #endif
#else
    #define REVOLTC_COMP_UNKNOWN    1
#endif

/* OS detection */
#if defined(_WIN32) || defined(_WIN64)
    #define REVOLTC_OS_WIN 1
#elif defined(linux) || defined(__linux__) || defined(__linux)
    #define REVOLTC_OS_LINUX 1
    #define REVOLTC_OS_UNIX 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define REVOLTC_OS_APPLE 1
    #define REVOLTC_OS_UNIX 1
    REVOLTC_WARNING("APPLE is not officially supported")
#elif defined(unix) || defined(__unix__) || defined(__unix)
    #define REVOLTC_OS_UNIX 1
#else
    REVOLTC_WARNING("Could not detect OS")
#endif

#endif /* _REVOLTC_CORE_DEFINES_H_INCLUDED_ */
