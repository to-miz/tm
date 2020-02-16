/*
tm_uint128.h v0.0.4 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2020

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

/* This is a generated file, do not modify directly. You can find the generator files in the src directory. */

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

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* Native bools, override by defining TM_BOOL_DEFINED yourself before including this file. */
#ifndef TM_BOOL_DEFINED
    #define TM_BOOL_DEFINED
    #ifdef __cplusplus
        typedef bool tm_bool;
        #define TM_TRUE true
        #define TM_FALSE false
    #else
        typedef _Bool tm_bool;
        #define TM_TRUE 1
        #define TM_FALSE 0
    #endif
#endif /* !defined(TM_BOOL_DEFINED) */

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* Define to 1 if tm_size_t is signed. */
    #include <stddef.h> /* Include C version so identifiers are in global namespace. */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
    #define TM_ERRC_DEFINED
    #include <errno.h>
    enum TM_ERRC_CODES {
        TM_OK           = 0,            /* Alternatively std::errc() */
        TM_EPERM        = EPERM,        /* Alternatively std::errc::operation_not_permitted */
        TM_ENOENT       = ENOENT,       /* Alternatively std::errc::no_such_file_or_directory */
        TM_EIO          = EIO,          /* Alternatively std::errc::io_error */
        TM_EAGAIN       = EAGAIN,       /* Alternatively std::errc::resource_unavailable_try_again */
        TM_ENOMEM       = ENOMEM,       /* Alternatively std::errc::not_enough_memory */
        TM_EACCES       = EACCES,       /* Alternatively std::errc::permission_denied */
        TM_EBUSY        = EBUSY,        /* Alternatively std::errc::device_or_resource_busy */
        TM_EEXIST       = EEXIST,       /* Alternatively std::errc::file_exists */
        TM_EXDEV        = EXDEV,        /* Alternatively std::errc::cross_device_link */
        TM_ENODEV       = ENODEV,       /* Alternatively std::errc::no_such_device */
        TM_EINVAL       = EINVAL,       /* Alternatively std::errc::invalid_argument */
        TM_EMFILE       = EMFILE,       /* Alternatively std::errc::too_many_files_open */
        TM_EFBIG        = EFBIG,        /* Alternatively std::errc::file_too_large */
        TM_ENOSPC       = ENOSPC,       /* Alternatively std::errc::no_space_on_device */
        TM_ERANGE       = ERANGE,       /* Alternatively std::errc::result_out_of_range */
        TM_ENAMETOOLONG = ENAMETOOLONG, /* Alternatively std::errc::filename_too_long */
        TM_ENOLCK       = ENOLCK,       /* Alternatively std::errc::no_lock_available */
        TM_ECANCELED    = ECANCELED,    /* Alternatively std::errc::operation_canceled */
        TM_ENOSYS       = ENOSYS,       /* Alternatively std::errc::function_not_supported */
        TM_ENOTEMPTY    = ENOTEMPTY,    /* Alternatively std::errc::directory_not_empty */
        TM_EOVERFLOW    = EOVERFLOW,    /* Alternatively std::errc::value_too_large */
        TM_ETIMEDOUT    = ETIMEDOUT,    /* Alternatively std::errc::timed_out */
    };
    typedef int tm_errc;
#endif

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
TMI_DEF tmi_uint128_t tmi_make(uint64_t low, uint64_t high);
TMI_DEF tmi_uint128_t tmi_make_low(uint64_t low);
TMI_DEF tmi_uint128_t tmi_make_high(uint64_t high);
TMI_DEF tmi_uint128_t tmi_make_bitmask(uint64_t set_bit_position);

TMI_DEF tmi_uint128_t tmi_add(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_add_u64(tmi_uint128_t_arg lhs, uint64_t rhs);
TMI_DEF tmi_uint128_t tmi_sub(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_sub_u64(tmi_uint128_t_arg lhs, uint64_t rhs);
TMI_DEF tmi_uint128_t tmi_mul(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_mul_u64(tmi_uint128_t_arg lhs, uint64_t rhs);
TMI_DEF tmi_uint128_t tmi_div(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_mod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_and(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_or(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_xor(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_not(tmi_uint128_t_arg v);
TMI_DEF tmi_uint128_t tmi_shl(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_shr(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_shl_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount);
TMI_DEF tmi_uint128_t tmi_shr_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount);
typedef struct {
    tmi_uint128_t div;
    tmi_uint128_t mod;
} tmi_divmod_result;
TMI_DEF tmi_divmod_result tmi_divmod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_divmod_result tmi_divmod_u64(tmi_uint128_t_arg lhs, uint64_t rhs);

TMI_DEF tmi_uint128_t tmi_inc(tmi_uint128_t_arg v);
TMI_DEF tmi_uint128_t tmi_dec(tmi_uint128_t_arg v);

TMI_DEF tm_bool tmi_lt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tm_bool tmi_lte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tm_bool tmi_gt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tm_bool tmi_gte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tm_bool tmi_eq(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF int tmi_cmp(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);

TMI_DEF tm_bool tmi_is_zero(tmi_uint128_t_arg v);
TMI_DEF tm_bool tmi_is_not_zero(tmi_uint128_t_arg v);
TMI_DEF tm_bool tmi_is_bit_set(tmi_uint128_t_arg v, uint64_t bit_position);
TMI_DEF tmi_uint128_t_arg tmi_set_bit(tmi_uint128_t_arg v, uint64_t bit_position);

TMI_DEF uint64_t tmi_low(tmi_uint128_t_arg v);
TMI_DEF uint64_t tmi_high(tmi_uint128_t_arg v);

/*!
 * @brief Find last set, returns the last set bit (most significant bit set to 1) of v. Undefined behavior if v is 0.
 * See tmi_fls_s for safe variants.
 * Also called bitscan reverse or (127 - clz) (count leading zeroes).
 * @param v[IN] The value to search the last set bit in. Undefined behavior if v is 0.
 * @return Returns the bit position of the last set bit. All bits to the left of this position will be 0.
 * Return value will satisfy (pseudo-code): (v & (1 << return_value)) != 0.
 */
TMI_DEF uint64_t tmi_fls(tmi_uint128_t_arg v);
/*! @copydoc tmi_fls() */
TMI_DEF uint32_t tmi_fls_u32(uint32_t v);
/*! @copydoc tmi_fls() */
TMI_DEF uint64_t tmi_fls_u64(uint64_t v);

/*!
 * @brief Find first set, returns the first set bit (least significant bit) of v. Undefined behavior if v is 0.
 * See tmi_ffs_s for safe variants.
 * Also called bitscan forward or ctz (count trailing zeroes).
 * @param v[IN] The value to search the first set bit in. Undefined behavior if v is 0.
 * @return Returns the bit position of the first set bit. All bits to the right of this position will be 0.
 * Return value will satisfy (pseudo-code): (v & (1 << return_value)) != 0.
 */
TMI_DEF uint64_t tmi_ffs(tmi_uint128_t_arg v);
/*! @copydoc tmi_ffs() */
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v);
/*! @copydoc tmi_ffs() */
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v);

/*!
 * @brief Find last set, returns the last set bit (most significant bit set to 1) of v.
 * Also called bitscan reverse, (127 - clz) (count leading zeroes) or floor(log2(v)).
 * @param v[IN] The value to search the last set bit in.
 * @return Returns the bit position of the last set bit if v != 0, return -1 otherwise. All bits to the left of this
 * position will be 0. Return value will satisfy (pseudo-code): (v & (1 << return_value)) != 0.
 */
TMI_DEF int tmi_fls_s(tmi_uint128_t_arg v);
/*! @copydoc tmi_fls_s() */
TMI_DEF int tmi_fls_u32_s(uint32_t v);
/*! @copydoc tmi_fls_s() */
TMI_DEF int tmi_fls_u64_s(uint64_t v);

/*!
 * @brief Find first set, returns the first set bit (least significant bit) of v.
 * Also called bitscan forward or ctz (count trailing zeroes).
 * @param v[IN] The value to search the first set bit in.
 * @return Returns the bit position of the first set bit if v != 0, return -1 otherwise. All bits to the right of this
 * position will be 0. Return value will satisfy (pseudo-code): (v & (1 << return_value)) != 0.
 */
TMI_DEF int tmi_ffs_s(tmi_uint128_t_arg v);
/*! @copydoc tmi_ffs_s() */
TMI_DEF int tmi_ffs_u32_s(uint32_t v);
/*! @copydoc tmi_ffs_s() */
TMI_DEF int tmi_ffs_u64_s(uint64_t v);

/*!
 * @brief Returns the count of set bits (bits set to 1) in v. Also called hamming weight or sideways sum.
 * @param v[IN] The value to count the bits in.
 * @return Count of bits set to 1 in v.
 */
TMI_DEF uint64_t tmi_popcount(tmi_uint128_t_arg v);
/*! @copydoc tmi_popcount() */
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v);
/*! @copydoc tmi_popcount() */
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v);

typedef struct {
    uint8_t entries[16];  //!< Little endian ordering: Least significant byte is entries[0].
} tmi_bytes;
/*!
 * @brief Returns byte representation of v.
 * @param v[IN] Value to turn into bytes.
 * @return Returns an object containing an array of 16 uint8_ts. The first entry is the least significant byte.
 */
TMI_DEF tmi_bytes tmi_to_bytes(tmi_uint128_t_arg v);

#ifndef TMI_NO_IEEE_754

/*!
 * @brief Converts a tmi_uint128_t to float using the rounding mode returned by TM_FEGETROUND().
 * @param v[IN] The value to be converted.
 * @return The converted float, or TM_FLT_MAX on overflow.
 */
TMI_DEF float tmi_to_float(tmi_uint128_t_arg v);
/*!
 * @brief Converts a tmi_uint128_t to float.
 * @param v[IN] The value to be converted.
 * @param rounding_mode[IN] The rounding mode to use. One of TM_FE_DOWNWARD, TM_FE_TONEAREST,
 * TM_FE_TOWARDZERO, TM_FE_UPWARD.
 * @return The converted float, or TM_FLT_MAX on overflow.
 */
TMI_DEF float tmi_to_float_rm(tmi_uint128_t_arg v, int rounding_mode);

/*!
 * @brief Converts a tmi_uint128_t to double.
 * @param v[IN] The value to be converted.
 * @param rounding_mode[IN] The rounding mode to use. One of TM_FE_DOWNWARD, TM_FE_TONEAREST,
 * TM_FE_TOWARDZERO, TM_FE_UPWARD.
 * @return The converted double, or TM_DBL_MAX on overflow.
 */
TMI_DEF double tmi_to_double(tmi_uint128_t_arg v);
/*!
 * @brief Converts a tmi_uint128_t to double using the rounding mode returned by TM_FEGETROUND().
 * @param v[IN] The value to be converted.
 * @param rounding_mode[IN] The rounding mode to use. One of TM_FE_DOWNWARD, TM_FE_TONEAREST,
 * TM_FE_TOWARDZERO, TM_FE_UPWARD.
 * @return The converted double, or TM_DBL_MAX on overflow.
 */
TMI_DEF double tmi_to_double_rm(tmi_uint128_t_arg v, int rounding_mode);

/*!
 * @brief Converts a float to tmi_uint128_t. Undefined behavior if v is NaN, +-Infinite or
 * would overflow on conversion.
 * Use the tmi_from_float_s for a safe variant.
 * @param v[IN] Must not be NaN, +-Infinite or bigger than tmi_max.
 * @return The converted value. If v is negative, twos complement of v is returned.
 */
TMI_DEF tmi_uint128_t tmi_from_float(float v);
/*!
 * @brief Converts a double to tmi_uint128_t. Undefined behavior if v is NaN, +-Infinite or
 * would overflow on conversion.
 * Use the tmi_from_double_s for a safe variant.
 * @param v[IN] Must not be NaN, +-Infinite or bigger than tmi_max.
 * @return The converted value. If v is negative, twos complement of v is returned.
 */
TMI_DEF tmi_uint128_t tmi_from_double(double v);

/*!
 * @brief Converts a float to tmi_uint128_t.
 * @param v[IN] The value to be converted. The function fails (returns false) if v is NaN,
 * +-Infinite or would overflow on conversion.
 * @param out[OUT] The output parameter. It is NOT written to if the return value is false. Must not be NULL.
 * @return True on success, false otherwise.
 */
TMI_DEF tm_bool tmi_from_float_s(float v, tmi_uint128_t* out);
/*!
 * @brief Converts a double to tmi_uint128_t.
 * @param v[IN] The value to be converted. The function fails (returns false) if v is NaN,
 * +-Infinite or would overflow on conversion.
 * @param out[OUT] The output parameter. It is NOT written to if the return value is false. Must not be NULL.
 * @return True on success, false otherwise.
 */
TMI_DEF tm_bool tmi_from_double_s(double v, tmi_uint128_t* out);

#endif /* !defined(TMI_NO_IEEE_754) */

// Include tm_conversion.h before this header to use tm_conversion types for string conversions.

// clang-format off
#if defined(_TM_CONVERSION_H_INCLUDED_)
    typedef tmc_conv_result tmi_conv_result;
    // Remove tmi prefix to keep in line with tm_conversion naming convention.
    #define tmi_scan_u128 scan_u128
    #define tmi_scan_u128_n scan_u128_n
    #define tmi_print_u128 print_u128
    #define tmi_print_decimal_u128 print_decimal_u128
    #define tmi_print_hex_u128 print_hex_u128
    #define tmi_get_digits_count_u128 get_digits_count_u128
#else
    typedef struct {
        tm_size_t size;
        tm_errc ec;
    } tmi_conv_result;
#endif
// clang-format on

/*!
 * @brief Converts a nullterminated string to a tmi_uint128_t using a given base.
 * @param nullterminated[IN] A nullterminated string.
 * @param out[OUT] The output parameter. Cannot be nullptr. On success *out will contain the converted value.
 * No modifications on error.
 * @param base[IN] The base to use for the conversion.
 * @return Returns a size and an error code.
 * On success ec equals TM_OK and size contains how many bytes of the input string were consumed.
 * Possible error codes are TM_EINVAL and TM_ERANGE.
 */
TMI_DEF tmi_conv_result tmi_scan_u128(const char* nullterminated, tmi_uint128_t* out, int32_t base);

/*!
 * @brief Converts a sized string to a tmi_uint128_t using a given base.
 * @param nullterminated[IN] A non-nullterminated string.
 * @param maxlen[IN] The length of the string in bytes.
 * @param out[OUT] The output parameter. Cannot be nullptr. On success *out will contain the converted value.
 * No modifications on error.
 * @param base[IN] The base to use for the conversion.
 * @return Returns a size and an error code.
 * On success ec equals TM_OK and size contains how many bytes of the input string were consumed.
 * Possible error codes are TM_EINVAL and TM_ERANGE.
 */
TMI_DEF tmi_conv_result tmi_scan_u128_n(const char* str, tm_size_t len, tmi_uint128_t* out, int32_t base);

/*!
 * @brief Converts a tmi_uint128_t to a string.
 * @param dest[IN,OUT] A mutable buffer that will receive the string output.
 * @param maxlen[IN] The length of the buffer in bytes.
 * @param value[IN] The value to be converted.
 * @param base[IN] The base to use for the conversion.
 * @param lowercase[IN] Whether to use lowercase or uppercase letters for bases > 10. Set to true for lowercase.
 * @return Returns a size and an error code.
 * On success ec equals TM_OK and size contains how many bytes of the output buffer were used.
 * Possible error codes are TM_EOVERFLOW.
 */
TMI_DEF tmi_conv_result tmi_print_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value, int32_t base, tm_bool lowercase);

// TMI_DEF tmi_conv_result tmi_print_decimal_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value);
// TMI_DEF tmi_conv_result tmi_print_hex_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value, tm_bool lowercase);

/*!
 * @brief Returns the number of digits (bytes) a tmi_uint128_t number would use if converted to string.
 * @param number[IN] The number to use.
 * @param base[IN] The base of the conversion to assume.
 * @return Returns the number of digits (bytes) the conversion would use. Can be used for allocations.
 * The return value should then be passed into tmi_print_u128_w instead of tmi_print_u128 so that the
 * digits count is not recalculated by the conversion.
 */
TMI_DEF tm_size_t tmi_get_digits_count_u128(tmi_uint128_t number, int32_t base);

/*!
 * @brief Converts a tmi_uint128_t to a string outputting at most {width} number of digits.
 * @param dest[IN,OUT] A mutable buffer that will receive the string output.
 * @param maxlen[IN] The length of the buffer in bytes.
 * @param width[IN] The number of digits to output. Must be calculated with tmi_get_digits_count_u128.
 * @param value[IN] The value to be converted.
 * @param base[IN] The base to use for the conversion.
 * @param lowercase[IN] Whether to use lowercase or uppercase letters for bases > 10. Set to true for lowercase.
 * @return Returns a size and an error code.
 * On success ec equals TM_OK and size contains how many bytes of the output buffer were used.
 * Possible error codes are TM_EOVERFLOW.
 */
TMI_DEF tmi_conv_result tmi_print_u128_w(char* dest, tm_size_t maxlen, tm_size_t width, tmi_uint128_t value,
                                         int32_t base, tm_bool lowercase);

/*!
 * @brief Convenience function to convert a string to a tmi_uint128_t ignoring errors.
 * @param nullterminated[IN] The nullterminated string to convert.
 * @return On conversion error 0 is returned. Otherwise the converted value is returned.
 */
TMI_DEF tmi_uint128_t tmi_from_string(const char* nullterminated, int32_t base);
/*!
 * @brief Convenience function to convert a string to a tmi_uint128_t ignoring errors.
 * @param str[IN] The sized string to convert.
 * @param maxlen[IN] The length of the string in bytes.
 * @return On conversion error 0 is returned. Otherwise the converted value is returned.
 */
TMI_DEF tmi_uint128_t tmi_from_string_n(const char* str, tm_size_t maxlen, int32_t base);

extern const tmi_uint128_t tmi_one;   //!< Same as tmi_make(1, 0) or tmi_make_low(1).
extern const tmi_uint128_t tmi_zero;  //!< Same as tmi_make(0, 0) or tmi_make_low(0).
extern const tmi_uint128_t tmi_max;   //!< Same as tmi_make(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull).

#ifdef __cplusplus
namespace tml {

class uint128_t {
   private:
    tmi_uint128_t value;

   public:
#ifdef TMI_ARGS_BY_POINTER
    typedef const uint128_t& uint128_t_arg;
#else
    typedef uint128_t uint128_t_arg;
#endif

    uint128_t(tmi_uint128_t x);
    uint128_t(uint64_t x);
    uint128_t(uint32_t x);
    uint128_t(uint16_t x);
    uint128_t(uint8_t x);
    uint128_t(uint64_t low, uint64_t high);

    uint128_t() = default;
    uint128_t(const uint128_t_arg&) = default;

    uint128_t& operator+=(uint128_t_arg rhs);
    uint128_t& operator-=(uint128_t_arg rhs);
    uint128_t& operator*=(uint128_t_arg rhs);
    uint128_t& operator/=(uint128_t_arg rhs);
    uint128_t& operator%=(uint128_t_arg rhs);
    uint128_t& operator&=(uint128_t_arg rhs);
    uint128_t& operator|=(uint128_t_arg rhs);
    uint128_t& operator^=(uint128_t_arg rhs);
    uint128_t& operator<<=(uint128_t_arg rhs);
    uint128_t& operator>>=(uint128_t_arg rhs);

    uint128_t& operator++();
    uint128_t operator++(int);

    uint128_t operator~() const;

    friend uint128_t operator+(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator-(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator*(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator/(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator%(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator&(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator|(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator^(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator<<(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator>>(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator==(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator!=(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator<(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator<=(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator>(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator>=(uint128_t_arg lhs, uint128_t_arg rhs);
    inline explicit operator bool() const { return tmi_is_not_zero(TMI_PASS(value)); }

    operator tmi_uint128_t();
    operator const tmi_uint128_t&() const;
};

uint128_t operator+(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator-(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator*(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator/(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator%(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator&(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator|(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator^(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator<<(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator>>(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator==(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator!=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator<(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator<=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator>(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator>=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);

#ifndef TMI_NO_STL
TMI_DEF std::string to_string(uint128_t v, int32_t base = 10);
TMI_DEF std::string to_string(tmi_uint128_t v, int32_t base = 10);
TMI_DEF uint128_t from_string(const char* nullterminated, int32_t base = 10);
TMI_DEF uint128_t from_string(const char* str, tm_size_t maxlen, int32_t base = 10);
#endif

}  // namespace tml
#endif

#endif /* !defined(TM_UINT128_H_INCLUDED_D43452BE_CF11_411B_94D5_F355D04FD218_) */

#ifdef TM_UINT128_IMPLEMENTATION

/* assert */
#ifndef TM_ASSERT
    #include <assert.h>
    #define TM_ASSERT assert
#endif /* !defined(TM_ASSERT) */

#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

#ifndef TM_UNREFERENCED_PARAM
	#define TM_UNREFERENCED_PARAM(x) ((void)(x))
	#define TM_UNREFERENCED(x) ((void)(x))
    #define TM_MAYBE_UNUSED(x) ((void)(x))
#endif

#ifdef TMI_BACKEND_UINT64

#ifdef TMI_USE_INTRINSICS

#if defined(__clang__)

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#define TMI_NO_MUL128
#define TMI_NO_SHIFT128

#if __has_builtin(__builtin_clz)
    TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
        TM_ASSERT(v);
        return 31 - __builtin_clz(v);
    }
#else
    #define TMI_NO_FLS32
#endif

#if __has_builtin(__builtin_ctz)
    TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
        TM_ASSERT(v);
        return __builtin_ctz(v);
    }
#else
    #define TMI_NO_FFS32
#endif

#if __has_builtin(__builtin_popcount)
    TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) { return __builtin_popcount(v); }
#else
    #define TMI_NO_POPCOUNT32
#endif

#if __has_builtin(__builtin_clzll)
    TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
        TM_ASSERT(v);
        return 63 - __builtin_clzll(v);
    }
#else
    #define TMI_NO_FLS64
#endif


#if __has_builtin(__builtin_ctzll)
    TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
        TM_ASSERT(v);
        return __builtin_ctzll(v);
    }
#else
    #define TMI_NO_FFS64
#endif

#if __has_builtin(__builtin_popcountll)
    TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) { return __builtin_popcountll(v); }
#else
    #define TMI_NO_POPCOUNT64
#endif

#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__) || defined(__LP64__))

#define TMI_NO_MUL128
#define TMI_NO_SHIFT128

TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
    TM_ASSERT(v);
    return 31 - __builtin_clz(v);
}
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
    TM_ASSERT(v);
    return __builtin_ctz(v);
}
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) { return __builtin_popcount(v); }
TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
    TM_ASSERT(v);
    return 63 - __builtin_clzll(v);
}
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
    TM_ASSERT(v);
    return __builtin_ctzll(v);
}
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) { return __builtin_popcountll(v); }

#elif defined(_MSC_VER) && _MSC_VER >= 1300

#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(__popcnt)

TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
    TM_ASSERT(v != 0);
    unsigned long result;
    _BitScanReverse(&result, (unsigned long)v);
    return (uint32_t)result;
}
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
    TM_ASSERT(v != 0);
    unsigned long result;
    _BitScanForward(&result, (unsigned long)v);
    return (uint32_t)result;
}
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) {
    return __popcnt(v);
}

#if defined(_WIN64)
    #pragma intrinsic(_umul128)
    #pragma intrinsic(__shiftleft128)
    #pragma intrinsic(__shiftright128)
    #pragma intrinsic(_BitScanForward64)
    #pragma intrinsic(_BitScanReverse64)
    #pragma intrinsic(__popcnt64)

    static tmi_uint128_t tmi_internal_mul64(uint64_t lhs, uint64_t rhs) {
        tmi_uint128_t result;
        result.low = _umul128(lhs, rhs, &result.high);
        return result;
    }

    TMI_DEF tmi_uint128_t tmi_shl_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
        TM_ASSERT(shift_amount < 128);
        if (shift_amount == 0) return lhs;

        tmi_uint128_t result;
        if (shift_amount >= 64) {
            shift_amount -= 64;
            result.low = 0;
            result.high = TMI_LOW(lhs) << shift_amount;
        } else {
            result.low = (TMI_LOW(lhs) << shift_amount);
            result.high = __shiftleft128(TMI_LOW(lhs), TMI_HIGH(lhs), (unsigned char)shift_amount);
        }
        return result;
    }
    TMI_DEF tmi_uint128_t tmi_shr_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
        TM_ASSERT(shift_amount < 128);
        if (shift_amount == 0) return lhs;

        tmi_uint128_t result;
        if (shift_amount >= 64) {
            shift_amount -= 64;
            result.low = TMI_HIGH(lhs) >> shift_amount;
            result.high = 0;
        } else {
            result.high = (TMI_HIGH(lhs) >> shift_amount);
            result.low = __shiftright128(TMI_LOW(lhs), TMI_HIGH(lhs), (unsigned char)shift_amount);
        }
        return result;
    }

    TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
        TM_ASSERT(v != 0);
        unsigned long result;
        _BitScanReverse64(&result, (unsigned __int64)v);
        return (uint64_t)result;
    }
    TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
        TM_ASSERT(v != 0);
        unsigned long result;
        _BitScanForward64(&result, (unsigned __int64)v);
        return (uint64_t)result;
    }
    TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) {
        return __popcnt64(v);
    }
#else
    #define TMI_NO_FLS64
    #define TMI_NO_FFS64
    #define TMI_NO_POPCOUNT64
    #define TMI_NO_MUL128
    #define TMI_NO_SHIFT128
#endif /* defined(_WIN64) */


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

TMI_DEF tmi_uint128_t tmi_make(uint64_t low, uint64_t high) {
    tmi_uint128_t result;
    result.low = low;
    result.high = high;
    return result;
}

TMI_DEF tmi_uint128_t tmi_make_low(uint64_t low) {
    tmi_uint128_t result;
    result.low = low;
    result.high = 0;
    return result;
}
TMI_DEF tmi_uint128_t tmi_make_high(uint64_t high) {
    tmi_uint128_t result;
    result.low = 0;
    result.high = high;
    return result;
}
TMI_DEF tmi_uint128_t tmi_make_bitmask(uint64_t set_bit_position) {
    TM_ASSERT(set_bit_position < 128);
    tmi_uint128_t result;
    if (set_bit_position >= 64) {
        result.low = 0;
        result.high = 1ull << (set_bit_position - 64);
    } else {
        result.low = 1ull << set_bit_position;
        result.high = 0;
    }
    return result;
}

// Adds two 64 Bit numbers together and returns result.
static tmi_uint128_t tmi_internal_add64(uint64_t lhs, uint64_t rhs) {
    tmi_uint128_t result;
    result.low = lhs + rhs;
    result.high = (result.low < lhs);  // Carry bit is set if result wrapped.
    return result;
}
static tmi_uint128_t tmi_internal_sub64(uint64_t lhs, uint64_t rhs) {
    tmi_uint128_t result;
    result.low = lhs - rhs;
    result.high = (result.low > lhs);  // Carry bit is set if result wrapped.
    return result;
}

TMI_DEF tmi_uint128_t tmi_add(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result = tmi_internal_add64(TMI_LOW(lhs), TMI_LOW(rhs));
    // High is lhs + rhs + carry_bit.
    TMI_HIGH(result) = TMI_HIGH(lhs) + TMI_HIGH(rhs) + TMI_HIGH(result);
    return result;
}
TMI_DEF tmi_uint128_t tmi_sub(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result = tmi_internal_sub64(TMI_LOW(lhs), TMI_LOW(rhs));
    // High is lhs - rhs - carry_bit.
    TMI_HIGH(result) = TMI_HIGH(lhs) - TMI_HIGH(rhs) - TMI_HIGH(result);
    return result;
}

TMI_DEF tmi_uint128_t tmi_add_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    tmi_uint128_t result = tmi_internal_add64(TMI_LOW(lhs), rhs);
    // High is lhs + carry_bit.
    TMI_HIGH(result) = TMI_HIGH(lhs) + TMI_HIGH(result);
    return result;
}
TMI_DEF tmi_uint128_t tmi_sub_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    tmi_uint128_t result = tmi_internal_sub64(TMI_LOW(lhs), rhs);
    // High is lhs - carry_bit.
    TMI_HIGH(result) = TMI_HIGH(lhs) - TMI_HIGH(result);
    return result;
}

static tmi_uint128_t tmi_internal_mul64(uint64_t lhs, uint64_t rhs);

TMI_DEF tmi_uint128_t tmi_mul(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result = tmi_internal_mul64(TMI_LOW(lhs), TMI_LOW(rhs));
    result.high += TMI_LOW(lhs) * TMI_HIGH(rhs);
    result.high += TMI_HIGH(lhs) * TMI_LOW(rhs);
    return result;
}
TMI_DEF tmi_uint128_t tmi_mul_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    tmi_uint128_t result = tmi_internal_mul64(TMI_LOW(lhs), rhs);
    result.high += TMI_HIGH(lhs) * rhs;
    return result;
}

TMI_DEF tmi_uint128_t tmi_div(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return tmi_divmod(lhs, rhs).div; }
TMI_DEF tmi_uint128_t tmi_mod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return tmi_divmod(lhs, rhs).mod; }
TMI_DEF tmi_uint128_t tmi_and(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result;
    result.low = TMI_LOW(lhs) & TMI_LOW(rhs);
    result.high = TMI_HIGH(lhs) & TMI_HIGH(rhs);
    return result;
}
TMI_DEF tmi_uint128_t tmi_or(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result;
    result.low = TMI_LOW(lhs) | TMI_LOW(rhs);
    result.high = TMI_HIGH(lhs) | TMI_HIGH(rhs);
    return result;
}
TMI_DEF tmi_uint128_t tmi_xor(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result;
    result.low = TMI_LOW(lhs) ^ TMI_LOW(rhs);
    result.high = TMI_HIGH(lhs) ^ TMI_HIGH(rhs);
    return result;
}
TMI_DEF tmi_uint128_t tmi_not(tmi_uint128_t_arg v) {
    tmi_uint128_t result;
    result.low = ~TMI_LOW(v);
    result.high = ~TMI_HIGH(v);
    return result;
}

TMI_DEF tmi_uint128_t tmi_shl(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(TMI_HIGH(rhs) == 0 && TMI_LOW(rhs) < 128);
    return tmi_shl_u64(lhs, TMI_LOW(rhs));
}
TMI_DEF tmi_uint128_t tmi_shr(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(TMI_HIGH(rhs) == 0 && TMI_LOW(rhs) < 128);
    return tmi_shr_u64(lhs, TMI_LOW(rhs));
}

#ifdef TMI_NO_SHIFT128
TMI_DEF tmi_uint128_t tmi_shl_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
    TM_ASSERT(shift_amount < 128);
    if (shift_amount == 0) return lhs;

    tmi_uint128_t result;
    if (shift_amount >= 64) {
        shift_amount -= 64;
        result.low = 0;
        result.high = TMI_LOW(lhs) << shift_amount;
    } else {
        uint64_t overflow_mask = TMI_LOW(lhs) >> (64 - shift_amount);
        result.low = (TMI_LOW(lhs) << shift_amount);
        result.high = (TMI_HIGH(lhs) << shift_amount) | overflow_mask;
    }
    return result;
}
TMI_DEF tmi_uint128_t tmi_shr_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
    TM_ASSERT(shift_amount < 128);
    if (shift_amount == 0) return lhs;

    tmi_uint128_t result;
    if (shift_amount >= 64) {
        shift_amount -= 64;
        result.low = TMI_HIGH(lhs) >> shift_amount;
        result.high = 0;
    } else {
        uint64_t overflow_mask = TMI_HIGH(lhs) << (64 - shift_amount);
        result.low = (TMI_LOW(lhs) >> shift_amount) | overflow_mask;
        result.high = (TMI_HIGH(lhs) >> shift_amount);
    }
    return result;
}
#endif

TMI_DEF tmi_uint128_t_arg tmi_set_bit(tmi_uint128_t_arg v, uint64_t bit_position) {
    tmi_uint128_t_arg result = TMI_DEREF(v);
    if (bit_position >= 64) {
        bit_position -= 64;
        result.high |= (1ull << bit_position);
    } else {
        result.low |= (1ull << bit_position);
    }
    return result;
}

TMI_DEF tmi_divmod_result tmi_divmod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(tmi_is_not_zero(rhs));

    tmi_divmod_result result;
    // Handle some trivial cases.
    if (TMI_HIGH(rhs) == 0 && TMI_LOW(rhs) == 1) {
        // lhs / 1
        result.div = TMI_DEREF(lhs);
        result.mod = tmi_zero;
    }
    if (TMI_HIGH(rhs) == 0 && TMI_LOW(rhs) == 2) {
        // lhs / 2
        result.div = tmi_shr(lhs, TMI_PASS(tmi_one));
        result.mod.low = (uint64_t)tmi_is_bit_set(lhs, 0);
        result.mod.high = 0;
    } else if (tmi_eq(lhs, rhs)) {
        // lhs / lhs
        result.div = tmi_one;
        result.mod = tmi_zero;
    } else if (tmi_lt(lhs, rhs)) {
        // (rhs - c) / rhs
        result.div = tmi_zero;
        result.mod = TMI_DEREF(lhs);
    } else {
        // Used algorithm is binary long division.
        result.div = tmi_zero;
        result.mod = tmi_zero;

        // lhs is guaranteed > 0 since we checked whether lhs <= rhs in the other cases and rhs cannot be 0.
        uint64_t i = tmi_fls(lhs);
        for (;;) {
            result.mod.low |= (uint64_t)tmi_is_bit_set(lhs, i);

            if (tmi_gte(TMI_PASS(result.mod), rhs)) {
                result.mod = tmi_sub(TMI_PASS(result.mod), rhs);
                result.div = tmi_set_bit(result.div, i);
            }
            if (!i) break;
            --i;

            // Inline shift one bit left
            result.mod.high = (result.mod.high << 1) | (result.mod.low >> 63);
            result.mod.low <<= 1;
        }
    }

    return result;
}
TMI_DEF tmi_divmod_result tmi_divmod_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    TM_ASSERT(rhs != 0);

    tmi_divmod_result result;
    // Handle some trivial cases.
    if (rhs == 1) {
        // lhs / 1
        result.div = TMI_DEREF(lhs);
        result.mod = tmi_zero;
    }
    if (rhs == 2) {
        // lhs / 2
        result.div = tmi_shr(lhs, TMI_PASS(tmi_one));
        result.mod.low = (uint64_t)tmi_is_bit_set(lhs, 0);
        result.mod.high = 0;
    } else if (TMI_HIGH(lhs) == 0 && TMI_LOW(lhs) == rhs) {
        // lhs / lhs
        result.div = tmi_one;
        result.mod = tmi_zero;
    } else if (TMI_HIGH(lhs) == 0 && TMI_LOW(lhs) < rhs) {
        // (rhs - c) / rhs
        result.div = tmi_zero;
        result.mod = TMI_DEREF(lhs);
    } else {
        // Used algorithm is binary long division.
        result.div = tmi_zero;
        result.mod = tmi_zero;

        // lhs is guaranteed > 0 since we checked whether lhs <= rhs in the other cases and rhs cannot be 0.
        uint64_t i = tmi_fls(lhs);
        for (;;) {
            result.mod.low |= (uint64_t)tmi_is_bit_set(lhs, i);

            if (TMI_HIGH(result.mod) || TMI_LOW(result.mod) >= rhs) {
                result.mod = tmi_sub_u64(TMI_PASS(result.mod), rhs);
                result.div = tmi_set_bit(result.div, i);
            }
            if (!i) break;
            --i;

            // Inline shift one bit left
            result.mod.high = (result.mod.high << 1) | (result.mod.low >> 63);
            result.mod.low <<= 1;
        }
    }

    return result;
}

TMI_DEF tmi_uint128_t tmi_inc(tmi_uint128_t_arg v) {
    tmi_uint128_t result = tmi_internal_add64(TMI_LOW(v), 1);
    TMI_HIGH(result) = TMI_HIGH(v) + TMI_HIGH(result);
    return result;
}

TMI_DEF tmi_uint128_t tmi_dec(tmi_uint128_t_arg v) {
    tmi_uint128_t result = tmi_internal_sub64(TMI_LOW(v), 1);
    TMI_HIGH(result) = TMI_HIGH(v) - TMI_HIGH(result);
    return result;
}

TMI_DEF tm_bool tmi_lt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_HIGH(lhs) == TMI_HIGH(rhs)) ? (TMI_LOW(lhs) < TMI_LOW(rhs)) : (TMI_HIGH(lhs) < TMI_HIGH(rhs));
}
TMI_DEF tm_bool tmi_lte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_HIGH(lhs) == TMI_HIGH(rhs)) ? (TMI_LOW(lhs) <= TMI_LOW(rhs)) : (TMI_HIGH(lhs) < TMI_HIGH(rhs));
}
TMI_DEF tm_bool tmi_gt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_HIGH(lhs) == TMI_HIGH(rhs)) ? (TMI_LOW(lhs) > TMI_LOW(rhs)) : (TMI_HIGH(lhs) > TMI_HIGH(rhs));
}
TMI_DEF tm_bool tmi_gte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_HIGH(lhs) == TMI_HIGH(rhs)) ? (TMI_LOW(lhs) >= TMI_LOW(rhs)) : (TMI_HIGH(lhs) > TMI_HIGH(rhs));
}
TMI_DEF tm_bool tmi_eq(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_LOW(lhs) == TMI_LOW(rhs)) && (TMI_HIGH(lhs) == TMI_HIGH(rhs));
}
TMI_DEF int tmi_cmp(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    if (TMI_HIGH(lhs) < TMI_HIGH(rhs)) return -1;
    if (TMI_HIGH(lhs) > TMI_HIGH(rhs)) return 1;
    if (TMI_LOW(lhs) < TMI_LOW(rhs)) return -1;
    if (TMI_LOW(lhs) > TMI_LOW(rhs)) return 1;
    return 0;
}

TMI_DEF tm_bool tmi_is_zero(tmi_uint128_t_arg v) { return (TMI_LOW(v) == 0) && (TMI_HIGH(v) == 0); }
TMI_DEF tm_bool tmi_is_not_zero(tmi_uint128_t_arg v) { return (TMI_LOW(v) != 0) || (TMI_HIGH(v) != 0); }
TMI_DEF tm_bool tmi_is_bit_set(tmi_uint128_t_arg v, uint64_t bit_position) {
    TM_ASSERT(bit_position < 128);
    if (bit_position >= 64) return (TMI_HIGH(v) & (1ull << (bit_position - 64))) != 0;
    return (TMI_LOW(v) & (1ull << bit_position)) != 0;
}

TMI_DEF uint64_t tmi_low(tmi_uint128_t_arg v) { return TMI_LOW(v); }
TMI_DEF uint64_t tmi_high(tmi_uint128_t_arg v) { return TMI_HIGH(v); }

TMI_DEF uint64_t tmi_fls(tmi_uint128_t_arg v) {
    if (TMI_HIGH(v)) return tmi_fls_u64(TMI_HIGH(v)) + 64;
    return tmi_fls_u64(TMI_LOW(v));
}
TMI_DEF uint64_t tmi_ffs(tmi_uint128_t_arg v) {
    if (TMI_LOW(v)) return tmi_ffs_u64(TMI_LOW(v));
    return tmi_ffs_u64(TMI_HIGH(v)) + 64;
}
TMI_DEF uint64_t tmi_popcount(tmi_uint128_t_arg v) {
    return tmi_popcount_u64(TMI_LOW(v)) + tmi_popcount_u64(TMI_HIGH(v));
}

TMI_DEF tmi_bytes tmi_to_bytes(tmi_uint128_t_arg v) {
    tmi_bytes result;
    result.entries[0] = (uint8_t)((TMI_LOW(v) >> 0) & 0xFF);
    result.entries[1] = (uint8_t)((TMI_LOW(v) >> 8) & 0xFF);
    result.entries[2] = (uint8_t)((TMI_LOW(v) >> 16) & 0xFF);
    result.entries[3] = (uint8_t)((TMI_LOW(v) >> 24) & 0xFF);
    result.entries[4] = (uint8_t)((TMI_LOW(v) >> 32) & 0xFF);
    result.entries[5] = (uint8_t)((TMI_LOW(v) >> 40) & 0xFF);
    result.entries[6] = (uint8_t)((TMI_LOW(v) >> 48) & 0xFF);
    result.entries[7] = (uint8_t)((TMI_LOW(v) >> 56) & 0xFF);

    result.entries[8] = (uint8_t)((TMI_HIGH(v) >> 0) & 0xFF);
    result.entries[9] = (uint8_t)((TMI_HIGH(v) >> 8) & 0xFF);
    result.entries[10] = (uint8_t)((TMI_HIGH(v) >> 16) & 0xFF);
    result.entries[11] = (uint8_t)((TMI_HIGH(v) >> 24) & 0xFF);
    result.entries[12] = (uint8_t)((TMI_HIGH(v) >> 32) & 0xFF);
    result.entries[13] = (uint8_t)((TMI_HIGH(v) >> 40) & 0xFF);
    result.entries[14] = (uint8_t)((TMI_HIGH(v) >> 48) & 0xFF);
    result.entries[15] = (uint8_t)((TMI_HIGH(v) >> 56) & 0xFF);
    return result;
}

const tmi_uint128_t tmi_one = {1, 0};
const tmi_uint128_t tmi_zero = {0, 0};
const tmi_uint128_t tmi_max = {0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull};

#ifdef TMI_NO_MUL128
// Multiplies two 32 Bit numbers together and returns result.
static tmi_uint128_t tmi_internal_mul64(uint64_t lhs, uint64_t rhs) {
    uint64_t lhs_low = lhs & 0xFFFFFFFFull;
    uint64_t lhs_high = lhs >> 32ull;
    uint64_t rhs_low = rhs & 0xFFFFFFFFull;
    uint64_t rhs_high = rhs >> 32ull;

    uint64_t low_low = lhs_low * rhs_low;
    uint64_t low_high = lhs_low * rhs_high + (low_low >> 32ull);
    uint64_t high_low = lhs_high * rhs_low;
    uint64_t high_high = lhs_high * rhs_high + (high_low >> 32ull);

    uint64_t a = low_low & 0xFFFFFFFFull;
    uint64_t b = low_high + (high_low & 0xFFFFFFFFull);
    uint64_t c = high_high + (b >> 32ull);
    b <<= 32ull;

    tmi_uint128_t result;
    result.low = a | b;
    result.high = c;
    return result;
}
#endif



#endif /* defined(TMI_BACKEND_UINT64) */

#ifdef TMI_BACKEND_GCC_UINT128

// __extension__ so we don't get a warning about using gcc/clang extension.
__extension__ typedef unsigned __int128 tmi_u128;

TMI_DEF tmi_uint128_t tmi_make(uint64_t low, uint64_t high) { return {(tmi_u128)low | (((tmi_u128)high) << 64)}; }

TMI_DEF tmi_uint128_t tmi_make_low(uint64_t low) { return {low}; }
TMI_DEF tmi_uint128_t tmi_make_high(uint64_t high) { return {((tmi_u128)high) << 64}; }
TMI_DEF tmi_uint128_t tmi_make_bitmask(uint64_t set_bit_position) { return {((tmi_u128)1) << set_bit_position}; }

TMI_DEF tmi_uint128_t tmi_add(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value + rhs.value}; }
TMI_DEF tmi_uint128_t tmi_add_u64(tmi_uint128_t_arg lhs, uint64_t rhs) { return {lhs.value + rhs}; }
TMI_DEF tmi_uint128_t tmi_sub(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value - rhs.value}; }
TMI_DEF tmi_uint128_t tmi_sub_u64(tmi_uint128_t_arg lhs, uint64_t rhs) { return {lhs.value - rhs}; }
TMI_DEF tmi_uint128_t tmi_mul(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value * rhs.value}; }
TMI_DEF tmi_uint128_t tmi_mul_u64(tmi_uint128_t_arg lhs, uint64_t rhs) { return {lhs.value * rhs}; }
TMI_DEF tmi_uint128_t tmi_div(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(rhs.value != 0);
    return {lhs.value / rhs.value};
}
TMI_DEF tmi_uint128_t tmi_mod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(rhs.value != 0);
    return {lhs.value % rhs.value};
}
TMI_DEF tmi_uint128_t tmi_and(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value & rhs.value}; }
TMI_DEF tmi_uint128_t tmi_or(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value | rhs.value}; }
TMI_DEF tmi_uint128_t tmi_xor(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value ^ rhs.value}; }
TMI_DEF tmi_uint128_t tmi_not(tmi_uint128_t_arg v) { return {~v.value}; }
TMI_DEF tmi_uint128_t tmi_shl(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(rhs.value < 128);
    return {lhs.value << rhs.value};
}
TMI_DEF tmi_uint128_t tmi_shr(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(rhs.value < 128);
    return {lhs.value >> rhs.value};
}
TMI_DEF tmi_uint128_t tmi_shl_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
    TM_ASSERT(shift_amount < 128);
    return {lhs.value << shift_amount};
}
TMI_DEF tmi_uint128_t tmi_shr_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
    TM_ASSERT(shift_amount < 128);
    return {lhs.value >> shift_amount};
}
TMI_DEF tmi_divmod_result tmi_divmod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return {{lhs.value / rhs.value}, {lhs.value % rhs.value}};
}
TMI_DEF tmi_divmod_result tmi_divmod_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    return {{lhs.value / rhs}, {lhs.value % rhs}};
}

TMI_DEF tmi_uint128_t tmi_inc(tmi_uint128_t_arg v) { return {v.value + 1}; }
TMI_DEF tmi_uint128_t tmi_dec(tmi_uint128_t_arg v) { return {v.value - 1}; }

TMI_DEF tm_bool tmi_lt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value < rhs.value; }
TMI_DEF tm_bool tmi_lte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value <= rhs.value; }
TMI_DEF tm_bool tmi_gt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value > rhs.value; }
TMI_DEF tm_bool tmi_gte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value >= rhs.value; }
TMI_DEF tm_bool tmi_eq(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value == rhs.value; }
TMI_DEF int tmi_cmp(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    if (lhs.value < rhs.value) return -1;
    if (lhs.value > rhs.value) return 1;
    return 0;
}

TMI_DEF tm_bool tmi_is_zero(tmi_uint128_t_arg v) { return v.value == 0; }
TMI_DEF tm_bool tmi_is_not_zero(tmi_uint128_t_arg v) { return v.value != 0; }
TMI_DEF tm_bool tmi_is_bit_set(tmi_uint128_t_arg v, uint64_t bit_position) {
    TM_ASSERT(bit_position < 128);
    return (v.value & ((tmi_u128)1 << bit_position)) != 0;
}
TMI_DEF tmi_uint128_t_arg tmi_set_bit(tmi_uint128_t_arg v, uint64_t bit_position) {
    TM_ASSERT(bit_position < 128);
    return {v.value | ((tmi_u128)1 << bit_position)};
}

TMI_DEF uint64_t tmi_low(tmi_uint128_t_arg v) { return (uint64_t)v.value; }
TMI_DEF uint64_t tmi_high(tmi_uint128_t_arg v) { return (uint64_t)(v.value >> 64); }

TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
    TM_ASSERT(v);
    return 31 - __builtin_clz(v);
}
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
    TM_ASSERT(v);
    return __builtin_ctz(v);
}
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) { return __builtin_popcount(v); }
TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
    TM_ASSERT(v);
    return 63 - __builtin_clzll(v);
}
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
    TM_ASSERT(v);
    return __builtin_ctzll(v);
}
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) { return __builtin_popcountll(v); }

TMI_DEF uint64_t tmi_fls(tmi_uint128_t_arg v) {
    uint64_t high = tmi_high(v);
    if (high) return tmi_fls_u64(high) + 64;
    return tmi_fls_u64(tmi_low(v));
}
TMI_DEF uint64_t tmi_ffs(tmi_uint128_t_arg v) {
    uint64_t low = tmi_low(v);
    if (low) return tmi_ffs_u64(low);
    return tmi_ffs_u64(tmi_high(v)) + 64;
}
TMI_DEF uint64_t tmi_popcount(tmi_uint128_t_arg v) {
    return tmi_popcount_u64(tmi_low(v)) + tmi_popcount_u64(tmi_high(v));
}

TMI_DEF tmi_bytes tmi_to_bytes(tmi_uint128_t_arg v) {
    tmi_bytes result;
    result.entries[0] = (uint8_t)((v.value >> 0) & 0xFF);
    result.entries[1] = (uint8_t)((v.value >> 8) & 0xFF);
    result.entries[2] = (uint8_t)((v.value >> 16) & 0xFF);
    result.entries[3] = (uint8_t)((v.value >> 24) & 0xFF);
    result.entries[4] = (uint8_t)((v.value >> 32) & 0xFF);
    result.entries[5] = (uint8_t)((v.value >> 40) & 0xFF);
    result.entries[6] = (uint8_t)((v.value >> 48) & 0xFF);
    result.entries[7] = (uint8_t)((v.value >> 56) & 0xFF);

    result.entries[8] = (uint8_t)((v.value >> 64) & 0xFF);
    result.entries[9] = (uint8_t)((v.value >> 72) & 0xFF);
    result.entries[10] = (uint8_t)((v.value >> 80) & 0xFF);
    result.entries[11] = (uint8_t)((v.value >> 88) & 0xFF);
    result.entries[12] = (uint8_t)((v.value >> 96) & 0xFF);
    result.entries[13] = (uint8_t)((v.value >> 104) & 0xFF);
    result.entries[14] = (uint8_t)((v.value >> 112) & 0xFF);
    result.entries[15] = (uint8_t)((v.value >> 120) & 0xFF);
    return result;
}

const tmi_uint128_t tmi_one = {1};
const tmi_uint128_t tmi_zero = {0};
const tmi_uint128_t tmi_max = {~((tmi_u128)0)};

#endif /* defined(TMI_BACKEND_GCC_UINT128) */

#ifdef TMI_NO_FLS32
TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
    TM_ASSERT(v != 0);
    // from http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn (Public Domain)
    static const int32_t MultiplyDeBruijnBitPosition[32] = {0,  9,  1,  10, 13, 21, 2,  29, 11, 14, 16,
                                                            18, 22, 25, 3,  30, 8,  12, 20, 28, 15, 17,
                                                            24, 7,  19, 27, 23, 6,  26, 5,  4,  31};

    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}
#endif

#ifdef TMI_NO_FFS32
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
    TM_ASSERT(v != 0);
    // from http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup (Public Domain)
    static const int32_t MultiplyDeBruijnBitPosition[32] = {0,  1,  28, 2,  29, 14, 24, 3,  30, 22, 20,
                                                            15, 25, 17, 4,  8,  31, 27, 13, 23, 21, 19,
                                                            16, 7,  26, 12, 18, 6,  11, 5,  10, 9};
    return MultiplyDeBruijnBitPosition[((uint32_t)((v & (~(v - 1))) * 0x077CB531u)) >> 27];
}
#endif

#ifdef TMI_NO_POPCOUNT32
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) {
    // from https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel (Public Domain)
    v = v - ((v >> 1) & 0x55555555u);
    v = (v & 0x33333333u) + ((v >> 2) & 0x33333333u);
    v = (v + (v >> 4)) & 0x0F0F0F0Fu;
    return (v * 0x01010101u) >> 24u;
}
#endif

#ifdef TMI_NO_POPCOUNT64
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) {
    // from https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel (Public Domain)
    v = v - ((v >> 1) & 0x5555555555555555ull);
    v = (v & 0x3333333333333333ull) + ((v >> 2) & 0x3333333333333333ull);
    v = (v + (v >> 4)) & 0x0F0F0F0F0F0F0F0Full;
    return (v * 0x0101010101010101ull) >> 56ull;
}
#endif

#ifdef TMI_NO_FLS64
TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
    uint32_t high = (uint32_t)(v >> 32u);
    if (high) return tmi_fls_u32(high) + 32;
    return tmi_fls_u32((uint32_t)v);
}
#endif

#ifdef TMI_NO_FFS64
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
    uint32_t low = (uint32_t)(v & 0xFFFFFFFFull);
    if (low) return tmi_ffs_u32(low);
    return tmi_ffs_u32((uint32_t)(v >> 32)) + 32;
}
#endif

TMI_DEF int tmi_fls_s(tmi_uint128_t_arg v) {
    if (tmi_is_zero(v)) return -1;
    return (int)tmi_fls(v);
}
TMI_DEF int tmi_fls_u32_s(uint32_t v) {
    if (v == 0) return -1;
    return (int)tmi_fls_u32(v);
}
TMI_DEF int tmi_fls_u64_s(uint64_t v) {
    if (v == 0) return -1;
    return (int)tmi_fls_u64(v);
}
TMI_DEF int tmi_ffs_s(tmi_uint128_t_arg v) {
    if (tmi_is_zero(v)) return -1;
    return (int)tmi_ffs(v);
}
TMI_DEF int tmi_ffs_u32_s(uint32_t v) {
    if (v == 0) return -1;
    return (int)tmi_ffs_u32(v);
}
TMI_DEF int tmi_ffs_u64_s(uint64_t v) {
    if (v == 0) return -1;
    return (int)tmi_ffs_u64(v);
}

#ifndef TMI_NO_IEEE_754

#ifndef TMI_BACKEND_GCC_UINT128
TMI_DEF float tmi_to_float(tmi_uint128_t_arg v) { return tmi_to_float_rm(v, TM_FEGETROUND()); }
#else
TMI_DEF float tmi_to_float(tmi_uint128_t_arg v) { return (float)v.value; }
#endif

TMI_DEF float tmi_to_float_rm(tmi_uint128_t_arg v, int rounding_mode) {
    if (tmi_is_zero(v)) return 0.0f;

    uint64_t shift_amount = tmi_fls(v);
    tmi_uint128_t normalized = tmi_shl_u64(v, 127 - shift_amount);
    uint32_t exponent = 127 + (uint32_t)shift_amount;
    if (exponent >= 256) return FLT_MAX;  // TODO: FLT_MAX or INF?

    uint32_t mantissa = (uint32_t)(tmi_high(TMI_PASS(normalized)) >> 32ull);
    uint32_t round_up = 0;
    if (rounding_mode == TM_FE_TONEAREST) {
        /*
        Rounding.
        Rounding algorithm from https://www.exploringbinary.com/decimal-to-floating-point-needs-arbitrary-precision/

        If bit 25 is 0, round down
        If bit 25 is 1
            If any bit beyond bit 25 is 1, round up
            If all bits beyond bit 25 are 0 (meaning the number is halfway between two floating-point numbers)
                If bit 53 is 0, round down
                If bit 53 is 1, round up
        */
        // If bit 25 is 0, round down
        // If bit 25 is 1
        if (mantissa & (1 << 7u)) {
            // If any bit beyond bit 25 is 1, round up
            // If all bits beyond bit 25 are 0 (meaning the number is halfway between two floating-point numbers)
            round_up =
                (tmi_low(TMI_PASS(normalized)) != 0) | ((tmi_high(TMI_PASS(normalized)) & 0x17FFFFFFFFFull) != 0);
        }
    } else if (rounding_mode == TM_FE_UPWARD) {
        // Upward means that if any bit is 1 we round up.
        round_up = (tmi_low(TMI_PASS(normalized)) != 0) | ((tmi_high(TMI_PASS(normalized)) & 0x1FFFFFFFFFFull) != 0);
    } else {
        // TM_FE_TOWARDZERO and TM_FE_DOWNWARD are the same thing for unsigned values.
        // We just truncate in this case.
    }
    mantissa >>= 8u;        // Shift mantissa into right position
    mantissa += round_up;   // Apply rounding.
    mantissa &= 0x7FFFFFu;  // Mask off leading 1 bit.
    uint32_t bits = (exponent << 23u) | mantissa;

    // Type pruning via safe memcpy. This is defined behavior.
    float result = 0.0f;
    TM_ASSERT(sizeof(result) == sizeof(bits));
    TM_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

#ifndef TMI_BACKEND_GCC_UINT128
TMI_DEF double tmi_to_double(tmi_uint128_t_arg v) { return tmi_to_double_rm(v, TM_FEGETROUND()); }
#else
TMI_DEF double tmi_to_double(tmi_uint128_t_arg v) { return (double)v.value; }
#endif

TMI_DEF double tmi_to_double_rm(tmi_uint128_t_arg v, int rounding_mode) {
    if (tmi_is_zero(v)) return 0.0;

    uint64_t shift_amount = tmi_fls(v);
    tmi_uint128_t normalized = tmi_shl_u64(v, 127 - shift_amount);
    uint64_t exponent = 1023 + shift_amount;
    if (exponent >= 2048) return DBL_MAX;  // TODO: DBL_MAX or INF?

    uint64_t mantissa = tmi_high(TMI_PASS(normalized));
    uint64_t round_up = 0;
    if (rounding_mode == TM_FE_TONEAREST) {
        /*
        Rounding.
        Rounding algorithm from https://www.exploringbinary.com/decimal-to-floating-point-needs-arbitrary-precision/

        If bit 54 is 0, round down
        If bit 54 is 1
            If any bit beyond bit 54 is 1, round up
            If all bits beyond bit 54 are 0 (meaning the number is halfway between two floating-point numbers)
                If bit 53 is 0, round down
                If bit 53 is 1, round up
        */
        // If bit 54 is 0, round down
        // If bit 54 is 1
        if (mantissa & (1 << 10u)) {
            // If any bit beyond bit 54 is 1, round up
            // If all bits beyond bit 54 are 0 (meaning the number is halfway between two floating-point numbers)
            round_up = (tmi_low(TMI_PASS(normalized)) != 0) | ((mantissa & 0xBFFull) != 0);
        }
    } else if (rounding_mode == TM_FE_UPWARD) {
        // Upward means that if any bit is 1 we round up.
        round_up = (tmi_low(TMI_PASS(normalized)) != 0) | ((mantissa & 0x7FFull) != 0);
    } else {
        // TM_FE_TOWARDZERO and TM_FE_DOWNWARD are the same thing for unsigned values.
        // We just truncate in this case.
    }
    mantissa >>= 11u;                // Shift mantissa into right position
    mantissa += round_up;            // Apply rounding.
    mantissa &= 0xFFFFFFFFFFFFFull;  // Mask off leading 1 bit.
    uint64_t bits = (exponent << 52u) | mantissa;

    // Type pruning via safe memcpy. This is defined behavior.
    double result = 0.0;
    TM_ASSERT(sizeof(result) == sizeof(bits));
    TM_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

TMI_DEF tmi_uint128_t tmi_from_float(float v) {
    // Type pruning via safe memcpy. This is defined behavior.
    uint32_t bits = 0;
    TM_ASSERT(sizeof(v) == sizeof(bits));
    TM_MEMCPY(&bits, &v, sizeof(bits));

    uint32_t sign = (bits >> 31u);
    int32_t exponent = (int32_t)((bits >> 23) & 0x7FFull) - 127 - 23;
    if (exponent < -23) return tmi_zero;
    tmi_uint128_t result = tmi_make_low(bits & 0x7FFFFFu);
    result = tmi_set_bit(result, 23); // Set implicit 1 bit.
    if (exponent < 0) {
        result = tmi_shr_u64(TMI_PASS(result), (uint64_t)(-exponent));
    } else {
        result = tmi_shl_u64(TMI_PASS(result), (uint64_t)exponent);
    }
    if (sign) result = tmi_sub(TMI_PASS(tmi_max), TMI_PASS(result));
    return result;
}
TMI_DEF tmi_uint128_t tmi_from_double(double v) {
    // Type pruning via safe memcpy. This is defined behavior.
    uint64_t bits = 0;
    TM_ASSERT(sizeof(v) == sizeof(bits));
    TM_MEMCPY(&bits, &v, sizeof(bits));

    uint64_t sign = (bits >> 63u);
    int64_t exponent = (int64_t)((bits >> 52) & 0x7FFull) - 1023 - 52;
    if (exponent < -52) return tmi_zero;
    tmi_uint128_t result = tmi_make_low(bits & 0xFFFFFFFFFFFFFull);
    result = tmi_set_bit(result, 52); // Set implicit 1 bit.
    if (exponent < 0) {
        result = tmi_shr_u64(TMI_PASS(result), (uint64_t)(-exponent));
    } else {
        result = tmi_shl_u64(TMI_PASS(result), (uint64_t)exponent);
    }
    if (sign) result = tmi_sub(TMI_PASS(tmi_max), TMI_PASS(result));
    return result;
}

TMI_DEF tm_bool tmi_from_float_s(float v, tmi_uint128_t* out) {
    TM_ASSERT(out);
    if (TM_ISFINITE(v)) {
        *out = tmi_from_float(v);
        return TM_TRUE;
    }
    return TM_FALSE;
}
TMI_DEF tm_bool tmi_from_double_s(double v, tmi_uint128_t* out) {
    TM_ASSERT(out);
    if (TM_ISFINITE(v)) {
        *out = tmi_from_double(v);
        return TM_TRUE;
    }
    return TM_FALSE;
}

#endif

TMI_DEF tmi_conv_result tmi_scan_u128(const char* nullterminated, tmi_uint128_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    typedef tmi_uint128_t utype;
    const utype UMAX_VAL = tmi_max;

    if (!nullterminated) {
        tmi_conv_result result = {0, TM_EINVAL};
        return result;
    }
    const char* start = nullterminated;
    const char* p = nullterminated;

    const uint64_t ubase = (uint64_t)base;
    int rangeError = 0;
    const tmi_divmod_result divmod = tmi_divmod_u64(TMI_PASS(UMAX_VAL), ubase);
    const utype maxValue = divmod.div;
    const uint64_t maxDigit = tmi_low(TMI_PASS(divmod.mod));
    utype value = tmi_zero;
    for (; *p; ++p) {
        int32_t cp = (uint8_t)(*p);
        uint64_t digit;
        if (TM_ISDIGIT(cp)) {
            digit = (uint64_t)(cp - '0');
        } else if (TM_ISUPPER(cp)) {
            digit = (uint64_t)(cp - 'A' + 10);
        } else if (TM_ISLOWER(cp)) {
            digit = (uint64_t)(cp - 'a' + 10);
        } else {
            break;
        }
        if (digit >= ubase) break;
        if (rangeError || tmi_gt(TMI_PASS(value), TMI_PASS(maxValue)) ||
            (tmi_eq(TMI_PASS(value), TMI_PASS(maxValue)) && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = tmi_mul_u64(TMI_PASS(value), ubase);
            value = tmi_add_u64(TMI_PASS(value), digit);
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);
    tmi_conv_result result = {0, TM_EINVAL};

    /* Did we scan anything? */
    if (dist == 0) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = UMAX_VAL;
#endif
        } else {
            *out = value;
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMI_DEF tmi_conv_result tmi_scan_u128_n(const char* str, tm_size_t len, tmi_uint128_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    typedef tmi_uint128_t utype;
    const utype UMAX_VAL = tmi_max;

    tmi_conv_result result = {0, TM_EINVAL};
    if (len <= 0) return result;

    const char* start = str;
    const char* p = str;

    const uint64_t ubase = (uint64_t)base;
    int rangeError = 0;
    const tmi_divmod_result divmod = tmi_divmod_u64(TMI_PASS(UMAX_VAL), ubase);
    const utype maxValue = divmod.div;
    const uint64_t maxDigit = tmi_low(TMI_PASS(divmod.mod));
    utype value = tmi_zero;
    for (; len; ++p, --len) {
        int32_t cp = (uint8_t)(*p);
        uint64_t digit;
        if (TM_ISDIGIT(cp)) {
            digit = (uint64_t)(cp - '0');
        } else if (TM_ISUPPER(cp)) {
            digit = (uint64_t)(cp - 'A' + 10);
        } else if (TM_ISLOWER(cp)) {
            digit = (uint64_t)(cp - 'a' + 10);
        } else {
            break;
        }
        if (digit >= ubase) break;
        if (rangeError || tmi_gt(TMI_PASS(value), TMI_PASS(maxValue)) ||
            (tmi_eq(TMI_PASS(value), TMI_PASS(maxValue)) && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = tmi_mul_u64(TMI_PASS(value), ubase);
            value = tmi_add_u64(TMI_PASS(value), digit);
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);

    /* Did we scan anything? */
    if (dist == 0) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = UMAX_VAL;
#endif
        } else {
            *out = value;
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}

TMI_DEF tmi_conv_result tmi_print_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value, int32_t base,
                                       tm_bool lowercase) {
    return tmi_print_u128_w(dest, maxlen, tmi_get_digits_count_u128(value, base), value, base, lowercase);
}
TMI_DEF tm_size_t tmi_get_digits_count_u128(tmi_uint128_t number, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    tm_size_t result = 1;

    const uint64_t ubase = (uint64_t)base;
    const uint64_t base_pow_1 = ubase;
    const uint64_t base_pow_2 = base_pow_1 * ubase;
    const uint64_t base_pow_3 = base_pow_2 * ubase;
    const uint64_t base_pow_4 = base_pow_3 * ubase;

    for (;;) {
        if (tmi_high(number) == 0 && tmi_low(number) < base_pow_4) {
            uint64_t number_low = tmi_low(number);
            return result + (number_low >= base_pow_1) + (number_low >= base_pow_2) + (number_low >= base_pow_3);
        }
        number = tmi_divmod_u64(TMI_PASS(number), base_pow_4).div;
        result += 4;
    }
}

// Use TMC macro to share tables with tm_conversion if compiled in the same translation unit.
#ifndef TMC_PRINT_NUMBERTOCHARTABLE_DEFINED
#define TMC_PRINT_NUMBERTOCHARTABLE_DEFINED
static const char print_NumberToCharTableUpper[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
                                                    'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                                    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
static const char print_NumberToCharTableLower[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
                                                    'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                                    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
#endif

TMI_DEF tmi_conv_result tmi_print_u128_w(char* dest, tm_size_t maxlen, tm_size_t width, tmi_uint128_t value,
                                         int32_t base, tm_bool lowercase) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen == 0);
    /* See comment on declaration for why width has to equal a specific value. */
    TM_ASSERT(width > 0);
    TM_ASSERT(width == tmi_get_digits_count_u128(value, base));

    tmi_conv_result result = {0, TM_OK};
    if (width > maxlen || width <= 0 || (tmi_is_zero(TMI_PASS(value)) && width != 1)) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }

    result.size = width;
    char* p = dest + width - 1;

    uint64_t ubase = (uint64_t)base;

    /* string conversion */
    const char* table = (lowercase) ? print_NumberToCharTableLower : print_NumberToCharTableUpper;
    if (tmi_is_zero(TMI_PASS(value))) {
        TM_ASSERT(width == 1);
        *p = '0';
    } else {
        TM_ASSERT(width > 0);
        do {
            tmi_divmod_result divmod = tmi_divmod_u64(TMI_PASS(value), ubase);
            TM_ASSERT(tmi_low(divmod.mod) <
                      sizeof(print_NumberToCharTableLower) / sizeof(print_NumberToCharTableLower[0]));
            *p-- = table[tmi_low(divmod.mod)];
            value = divmod.div;
            --width;
        } while (tmi_is_not_zero(TMI_PASS(value)) && width > 0);

        if (tmi_is_not_zero(value) && width <= 0) {
            result.size = maxlen;
            result.ec = TM_EOVERFLOW;
            return result;
        }
    }
    return result;
}

TMI_DEF tmi_uint128_t tmi_from_string(const char* nullterminated, int32_t base) {
    tmi_uint128_t result = tmi_zero;
    tmi_scan_u128(nullterminated, &result, base);
    return result;
}
TMI_DEF tmi_uint128_t tmi_from_string_n(const char* str, tm_size_t maxlen, int32_t base) {
    tmi_uint128_t result = tmi_zero;
    tmi_scan_u128_n(str, maxlen, &result, base);
    return result;
}

#ifdef __cplusplus
tml::uint128_t::uint128_t(tmi_uint128_t x) : value(x) {}
tml::uint128_t::uint128_t(uint64_t x) : value(tmi_make_low(x)) {}
tml::uint128_t::uint128_t(uint32_t x) : value(tmi_make_low(x)) {}
tml::uint128_t::uint128_t(uint16_t x) : value(tmi_make_low(x)) {}
tml::uint128_t::uint128_t(uint8_t x) : value(tmi_make_low(x)) {}
tml::uint128_t::uint128_t(uint64_t low, uint64_t high) : value(tmi_make(low, high)) {}

tml::uint128_t& tml::uint128_t::operator+=(uint128_t_arg rhs) {
    value = tmi_add(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator-=(uint128_t_arg rhs) {
    value = tmi_sub(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator*=(uint128_t_arg rhs) {
    value = tmi_mul(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator/=(uint128_t_arg rhs) {
    value = tmi_div(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator%=(uint128_t_arg rhs) {
    value = tmi_mod(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator&=(uint128_t_arg rhs) {
    value = tmi_and(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator|=(uint128_t_arg rhs) {
    value = tmi_or(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator^=(uint128_t_arg rhs) {
    value = tmi_xor(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator<<=(uint128_t_arg rhs) {
    value = tmi_shl(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator>>=(uint128_t_arg rhs) {
    value = tmi_shr(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}

tml::uint128_t& tml::uint128_t::operator++() {
    value = tmi_inc(value);
    return *this;
}
tml::uint128_t tml::uint128_t::operator++(int) {
    auto old = *this;
    value = tmi_inc(value);
    return old;
}

tml::uint128_t tml::uint128_t::operator~() const { return {tmi_not(TMI_PASS(value))}; }

tml::uint128_t tml::operator+(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_add(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator-(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_sub(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator*(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_mul(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator/(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_div(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator%(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_mod(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator&(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_and(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator|(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_or(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator^(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_xor(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator<<(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_shl(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator>>(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_shr(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
bool tml::operator==(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_eq(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator!=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return !tmi_eq(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator<(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_lt(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator<=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_lte(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator>(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_gt(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator>=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_gte(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}

tml::uint128_t::operator tmi_uint128_t() { return value; }
tml::uint128_t::operator const tmi_uint128_t&() const { return value; }

TMI_DEF std::string tml::to_string(tml::uint128_t v, int32_t base /*= 10*/) {
    return to_string(static_cast<const tmi_uint128_t&>(v), base);
}
TMI_DEF std::string tml::to_string(tmi_uint128_t v, int32_t base /*= 10*/) {
    auto width = tmi_get_digits_count_u128(v, base);
    std::string result(width, 0);
    tmi_print_u128_w(result.data(), (tm_size_t)result.size(), width, v, base, /*lowercase=*/false);
    return result;
}
TMI_DEF tml::uint128_t tml::from_string(const char* nullterminated, int32_t base /*= 10*/) {
    return {tmi_from_string(nullterminated, base)};
}
TMI_DEF tml::uint128_t tml::from_string(const char* str, tm_size_t maxlen, int32_t base /*= 10*/) {
    return {tmi_from_string_n(str, maxlen, base)};
}
#endif

#endif /* defined(TM_UINT128_IMPLEMENTATION) */

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2020 Tolga Mizrak

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------

Public Domain (www.unlicense.org):
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

---------------------------------------------------------------------------
*/