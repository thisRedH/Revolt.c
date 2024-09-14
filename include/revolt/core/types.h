#ifndef _REVOLTC_CORE_TYPES_H_INCLUDED_
#define _REVOLTC_CORE_TYPES_H_INCLUDED_

#ifdef HAVE_STDINT_H
    #include <stdint.h>
#else
    #include <pstdint/pstdint.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <revolt/core/defines.h>

REVOLTC_BEGIN_C_DECLS

typedef enum _revolt_bool {
    revolt_false = 0,
    revolt_true = !revolt_false
} revolt_bool;

typedef unsigned char revolt_byte;

REVOLTC_END_C_DECLS
#endif /* _REVOLTC_CORE_TYPES_H_INCLUDED_ */
