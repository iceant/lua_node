#ifndef INCLUDED_LN_TYPES_H
#define INCLUDED_LN_TYPES_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_LN_CONFIG_H
#include <ln_config.h>
#endif /*INCLUDED_LN_CONFIG_H*/

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif /*INCLUDED_STDINT_H*/

#ifndef INCLUDED_STDBOOL_H
#define INCLUDED_STDBOOL_H
#include <stdbool.h>
#endif /*INCLUDED_STDBOOL_H*/

#ifndef INCLUDED_INTTYPES_H
#define INCLUDED_INTTYPES_H
#include <inttypes.h>
#endif /*INCLUDED_INTTYPES_H*/


////////////////////////////////////////////////////////////////////////////////
////
#if defined(SIZEOF_VOID_P)
    #if (SIZEOF_VOID_P==4)
    typedef int32_t     ln_int_t;
    typedef uint32_t    ln_uint_t;
    typedef int32_t     ln_intptr_t;
    typedef uint32_t    ln_uintptr_t;
    typedef ln_uint_t   ln_size_t;
    #define LN_PRId     PRId32
    #define LN_PRIi     PRIi32
    #define LN_PRIo     PRIo32
    #define LN_PRIu     PRIu32
    #define LN_PRIx     PRIx32
    #define LN_PRIX     PRIX32
    #elif (SIZEOF_VOID_P==8)
    typedef int64_t     ln_int_t;
    typedef uint64_t    ln_uint_t;
    typedef int64_t     ln_intptr_t;
    typedef uint64_t    ln_uintptr_t;
    typedef ln_uint_t   ln_size_t;
    #define LN_PRId     PRId64
    #define LN_PRIi     PRIi64
    #define LN_PRIo     PRIo64
    #define LN_PRIu     PRIu64
    #define LN_PRIx     PRIx64
    #define LN_PRIX     PRIX64
    #endif
#endif /*defined(SIZEOF_VOID_P)*/

#if defined(__bool_true_false_are_defined)
    #define ln_bool_t bool
    #define LN_TRUE   true
    #define LN_FALSE  false
#else
    #define ln_bool_t int
    #define LN_TRUE   1
    #define LN_FALSE  0
#endif





#endif /*INCLUDED_LN_TYPES_H*/
