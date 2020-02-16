/*
tm_uint128.h v0.0.4 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak MERGE_YEAR

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_UINT128_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

DESCRIPTION
    A uint128 type that has a number of available backends.
    See SWITCHES for how to select a backend.
    Implements both a C API and a C++ wrapper if compiled in cpp mode.

SWITCHES
    #define any of the switches below before including this file to change the configuration.

    TMI_BACKEND_UINT64
        A backend that uses uint64_t as the underlying type to implement tmi_uint128_t.
        Its primary focus is portability and correctness.

    TMI_USE_INTRINSICS
        Whether to use compiler intrinsics for some low level functions.
        This controls whether to use __builtin_clz etc on gcc/clang
        or whether to use _mul128 and _BitScanForward etc on msvc.

    TMI_BACKEND_GCC_UINT128
        Whether to use unsigned __int128 that is available for gcc/clang as the backend.
        This will also automatically use compiler intrinsics.

    TMI_NO_STL
        Define this if you want to avoid including optional STL headers. This will improve
        compile times, but some APIs become unavailable:
            std::string returning tml::to_string functions.
        These headers will not be included:
            <string>

    TMI_ARGS_BY_POINTER
        Whether the arguments should by passed as pointers or by value for the C API.
        In the C++ wrapper determines, whether the arguments are passed by reference or by value.
        Can be used to benchmark performance of pointer passing vs value passing.

    TMI_NO_IEEE_754
        On platforms with no IEEE_754 floats this should be defined, so that tmi_to_float etc is
        disabled. Those are currently only implemented for IEEE_754 floats.

    TM_FEGETROUND
        By default tmi_to_float and tmi_to_double use fegetround to get the currently
        set rounding mode. To override this behavior, you can define it as follows:
            #define TM_FEGETROUND() FE_TONEAREST
        Another alternative is to use tmi_to_float_rm, it allows the rounding mode to
        be specified explicitly.

NOTES
    If you want to use this in conjunction with tm_conversion.h, include tm_conversion.h first.
    That way this header will share some definitions with tm_conversion regarding string conversions.

ISSUES
    - Not yet first release.

TODO
    - Write documentation.
    - SSE2 backend.

HISTORY     (DD.MM.YY)
    v0.0.4   16.02.20 Added tmi_to_float*, tmi_from_float*, tmi_to_float*, tmi_from_double*.
    v0.0.3   12.02.20 The msvc intrinsics path now also uses __shiftleft128 and __shiftright128.
                      Added safe tmi_ffs_s and tmi_fls_s.
    v0.0.2   10.02.20 Updated some documentation.
    v0.0.1   09.02.20 Initial Commit.
*/

#include "../common/GENERATED_WARNING.inc"

// clang-format off
#ifdef TM_UINT128_IMPLEMENTATION
    /* ctype.h dependency */
    #if !defined(TM_ISDIGIT) || !defined(TM_ISUPPER) || !defined(TM_ISLOWER) || !defined(TM_TOUPPER)
        #include <ctype.h>
        #define TM_ISDIGIT isdigit
        #define TM_ISUPPER isupper
        #define TM_ISLOWER islower
        #define TM_TOUPPER toupper
    #endif

    #ifndef TMI_NO_IEEE_754
        /* string.h dependency */
        #ifndef TM_MEMCPY
            #include <string.h>
            #define TM_MEMCPY memcpy
        #endif

        /* float.h dependency */
        #if !defined(TM_FLT_MAX) || !defined(TM_DBL_MAX)
            #include <float.h>
            #ifndef TM_FLT_MAX
                #define TM_FLT_MAX FLT_MAX
            #endif
            #ifndef TM_DBL_MAX
                #define TM_DBL_MAX DBL_MAX
            #endif
        #endif

        /* fenv.h dependency */
        #if !defined(TM_FE_DOWNWARD) || !defined(TM_FE_TONEAREST) \
            || !defined(TM_FE_TOWARDZERO) || !defined(TM_FE_UPWARD) || !defined(TM_FEGETROUND)
            #include <fenv.h>
            #ifndef TM_FE_DOWNWARD
                #define TM_FE_DOWNWARD FE_DOWNWARD
            #endif
            #ifndef TM_FE_TONEAREST
                #define TM_FE_TONEAREST FE_TONEAREST
            #endif
            #ifndef TM_FE_TOWARDZERO
                #define TM_FE_TOWARDZERO FE_TOWARDZERO
            #endif
            #ifndef TM_FE_UPWARD
                #define TM_FE_UPWARD FE_UPWARD
            #endif
            #ifndef TM_FEGETROUND
                // NOTE: On gcc you might need to link against libm.so by including -lm in the commandline
                // If you get undefined reference to symbol 'fegetround@@GLIBC_2.2.5' or similar.
                // Otherwise
                //  #define TM_FEGETROUND() FE_TONEAREST
                // also works, if you don't ever change rounding modes.
                #define TM_FEGETROUND fegetround
            #endif
        #endif

        /* math.h dependency */
        #if !defined(TM_ISFINITE)
            #include <math.h>
            #ifndef TM_ISFINITE
                #define TM_ISFINITE isfinite
            #endif
        #endif
    #endif
#endif

#ifndef TM_UINT128_H_INCLUDED_D43452BE_CF11_411B_94D5_F355D04FD218_
#define TM_UINT128_H_INCLUDED_D43452BE_CF11_411B_94D5_F355D04FD218_

#if !defined(TMI_NO_STL) && defined(__cplusplus)
    #include <string>
#endif

#include "../common/stdint.inc"

#include "../common/tm_bool.inc"

#include "../common/tm_size_t.inc"

#include "../common/tm_errc.inc"

/* Linkage defaults to extern, to override define TMI_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMI_DEF
    #define TMI_DEF extern
#endif

#if !defined(TMI_BACKEND_GCC_UINT128) && !defined(TMI_BACKEND_UINT64)
    #error Please select a backend. See SWITCHES at the top of <tm_uint128.h>.
#endif

// Check availability of gcc __int128, otherwise fallback to uint64_t implementation.
#if defined(TMI_BACKEND_GCC_UINT128) && !defined(__GNUC__) && !defined(TMI_BACKEND_UINT64)
    #define TMI_BACKEND_UINT64
    #undef TMI_BACKEND_GCC_UINT128
#endif

#ifdef TMI_BACKEND_UINT64
    typedef struct {
        uint64_t low;
        uint64_t high;
    } tmi_uint128_t;
#endif

#ifdef TMI_BACKEND_GCC_UINT128
    typedef struct {
        __extension__ unsigned __int128 value;
    } tmi_uint128_t;
#endif

#ifdef TMI_ARGS_BY_POINTER
    typedef const tmi_uint128_t* tmi_uint128_t_arg;
    #define TMI_DEREF(x) (*(x))
    #define TMI_PASS(x) (&(x))
    #define TMI_LOW(x) ((x)->low)
    #define TMI_HIGH(x) ((x)->high)
#else
    typedef tmi_uint128_t tmi_uint128_t_arg;
    #define TMI_DEREF(x) (x)
    #define TMI_PASS(x) (x)
    #define TMI_LOW(x) ((x).low)
    #define TMI_HIGH(x) ((x).high)
#endif
// clang-format on

#include <limits.h>
#include "functions.h"

#ifdef __cplusplus
#include "cpp_wrapper.h"
#endif

#endif /* !defined(TM_UINT128_H_INCLUDED_D43452BE_CF11_411B_94D5_F355D04FD218_) */

#ifdef TM_UINT128_IMPLEMENTATION

#include "../common/tm_assert.inc"

#include "../common/tm_assert_valid_size.inc"

#include "../common/tm_unreferenced_param.inc"

#ifdef TMI_BACKEND_UINT64

#ifdef TMI_USE_INTRINSICS

#if defined(__clang__)

#include "backend_uint64_intrin_clang.cpp"

#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__) || defined(__LP64__))

#include "backend_uint64_intrin_gcc.cpp"

#elif defined(_MSC_VER) && _MSC_VER >= 1300

#include "backend_uint64_intrin_msvc.cpp"

#else

#define TMI_NO_MUL128
#define TMI_NO_SHIFT128
#define TMI_NO_FFS32
#define TMI_NO_FFS64
#define TMI_NO_FLS32
#define TMI_NO_FLS64
#define TMI_NO_POPCOUNT32
#define TMI_NO_POPCOUNT64

#endif /* Intrinsics per platform */

#endif /* defined(TMI_USE_INTRINSICS) */

#ifndef TMI_USE_INTRINSICS
    #define TMI_NO_MUL128
    #define TMI_NO_SHIFT128
    #define TMI_NO_FFS32
    #define TMI_NO_FFS64
    #define TMI_NO_FLS32
    #define TMI_NO_FLS64
    #define TMI_NO_POPCOUNT32
    #define TMI_NO_POPCOUNT64
#endif

#include "backend_uint64.cpp"

#endif /* defined(TMI_BACKEND_UINT64) */

#ifdef TMI_BACKEND_GCC_UINT128

#include "backend_gcc_uint128.cpp"

#endif /* defined(TMI_BACKEND_GCC_UINT128) */

#include "bittwiddling.cpp"

#include "common.cpp"

#include "string_conversion.cpp"

#ifdef __cplusplus
#include "cpp_wrapper.cpp"
#endif

#endif /* defined(TM_UINT128_IMPLEMENTATION) */

#include "../common/LICENSE.inc"