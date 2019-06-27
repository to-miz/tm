/*
tm_print.h v0.0.21 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2016

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_PRINT_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

DESCRIPTION
    A typesafe formatting library using variadic templates.
    Formatting strings use placeholders where arguments will be placed into the format string like this:
        print("This is a placeholder: {}!", "Hello");
    Which will print "This is a placeholder: Hello!".

    The format specifier syntax is as follows:
        {[index]:[flags][width][.precision][specifier]}
    index:      An optional index specifying which argument to print. Not supplying an index will automatically print
                an argument once and move to the next one on the next non indexed placeholder.
    flags:      One of the following flags:
        '-':        Left justify argument.
        '+':        Always print sign.
        ' ':        Pad with spaces instead of zeroes.
        '0':        Pad with zeroes.
        '#':        Prepend radix prefix if applicable. See specifiers.
    width:      How wide the printed argument should be.
    precision:  How precise floating point output should be. Specifies how many digits to print after decimal point.
    specifier:  One of the following specifiers:
        For integers:
        'x':        Outputs a base 16 number. Prepends '0x' if '#' flag is also specified.
        'X':        Outputs a base 16 number. Prepends '0X' if '#' flag is also specified.
        'o':        Outputs a base 8 number. Prepends '0' if '#' flag is also specified.
        'b':        Outputs a base 2 number. Prepends '0b' if '#' flag is also specified.
        'B':        Outputs a base 2 number. Prepends '0B' if '#' flag is also specified.
        'c':        Outputs a character instead of a number for char arguments.

        For floating point:
        'e':        Outputs a float in scientific notation with lowercase letters.
        'E':        Outputs a float in scientific notation with uppercase letters.
        'f':        Outputs a float with trailing zeroes.
        'g':        Outputs a float either in shortest form or in scientific notation with lowercase letters.
        'G':        Outputs a float either in shortest form or in scientific notation with uppercase letters.
        'a':        Outputs a float in lowercase hex notation.
        'A':        Outputs a float in uppercase hex notation.

        For bool:
        'n':        Outputs booleans as a number (either '0' or '1').

SWITCHES
    TMP_STRING_WIDTH:
        Represents a function with this signature:
            int tmp_string_width(const char* str, tm_size_t str_len);
        Used for calculating the display width of a string when calculating alignment.
        It is unused when width format specifiers aren't used for strings or when all strings are ASCII.
        It should return -1 if the string is not printable.

        The width format specifier is only useful when the output is meant to be displayed in a console window
        or terminal. Getting the display width of a string for a console is highly dependent on the particular console
        and the used font, so there is no perfect algorithm to calculate the width.

        The default implementation assumes ASCII strings and just returns the number of bytes in the string.
        To switch implementations to a custom one, define TMP_STRING_WIDTH like this with a better implementation
        before including this file:
            #define TMP_STRING_WIDTH(str, str_len) better_string_width((str), (str_len))
        A better implementation should take into account things like encoding, grapheme clusters and ligatures present
        in the used font.

        A slightly better implementation can be found in <tm_unicode.h> as tmu_utf8_width_n for UTF-8 encoded strings,
        which uses sensible defaults for widths that are common when displayed.

ISSUES
    - The tm_conversion/charconv based implementation always outputs '.' as the decimal point character
      regardless of the locale, while the snprintf based output outputs the decimal point based on the
      current locale.
    - snprint may not be able to return necessary buffer len when supplied buffer isn't enough if a
      floating point number has to be printed that exceeds the small buffer size (200 bytes by default).
      In that case the return value is -1.
    - Types that are implicitly convertible to string_view produce errors currently.

HISTORY
    v0.0.21 30.05.19 Made error codes depend on <errno.h> by default.
    v0.0.20 03.05.19 Added static asserts to sanity check tm_conversion.h backend print flags compatibility.
    v0.0.19 14.04.19 Fixed tmp_has_custom_printer detecting wrong signature for custom snprint functions.
    v0.0.18 13.04.19 Fixed gcc/clang compilation warnings.
                     Fixed custom printing support that was broken after using std::decay.
    v0.0.17 11.04.19 Added toplevel namespace tml.
                     Added allocator support.
                     Changed tml::snprint to behave like snprintf.
                     Added std::string formatting.
                     Minor code formatting changes.
                     Fixed a bug in crt backend functions not reporting error in all cases.
    v0.0.16 10.04.19 Fixed a bug when printing C style array strings, template type deduction
                     wasn't decaying them into const char*.
                     Changed allocation function customization.
                     Added TMP_STRING_WIDTH abstraction to get display width of a string.
                     Added format specifier description.
                     Implemented snprint returning required size if supplied buffer isn't big enough.
                     Renamed some internal functions by adding tmp_ prefix.
                     Improved documentation.
    v0.0.15 09.04.19 Fixed a bug where some indexed placeholders weren't outputting anything when mixed
                     auto indexed placeholders.
                     Improved validation of format strings.
                     Simplified tmp_print and tmp_snprint by removing string_view overloads.
    v0.0.14 08.04.19 Allow print functions to be called with no variadic arguments.
    v0.0.13 11.03.19 Fixed printing with specified index.
    v0.0.12 10.03.19 Added char* specializations (non-const), fixing not being able to print raw char* strings.
    v0.0.11 09.03.19 Added tmp_parse_print_format for parsing the PrintFormat structure from other sources.
                     Added FILE* printing overloads for supplying an initial PrintFormat to be
                     used in printing.
                     Removed some unused code inside tmp_print_impl.
                     Elevated some internal print format flags to the public interface.
                     Removed TMP_NO_STDIO, fully replaced by TMP_NO_CRT_FILE_PRINTING.
    v0.0.10 14.01.19 Fixed msvc compilation errors in most conforming mode with all extensions disabled.
    v0.0.9  02.11.18 Moved the fixed size array inside PrintArgList into the variadic template functions
                     so its size can be deduced from the number of arguments.
    v0.0.8  06.10.18 Refactored some common macro blocks into include files.
                     Fixed compilation error when TM_STRING_VIEW is defined.
                     Added TMP_DEFAULT_FLAGS.
    v0.0.7  02.10.18 Refactored into multiple files that get merged.
                     Added multiple backends for int and float printing.
                     Added a couple more formatting options like o, a and c.
    v0.0.6  25.09.18 Reworked many printing functions because of breaking changes to tm_conversion.
    v0.0.5  01.09.18 Added MIT license option.
                     Refactored fillPrintArgList to not be dependend on preprocessor switches.
                     Removed fprinter.
                     Tmp_memory_printer can now grow and can use small buffer optimization.
                     Removed dependency on fprintf.
    v0.0.4e 25.08.18 Added repository link.
    v0.0.4d 10.01.17 Minor change from static const char* to static const char* const in print_bool.
    v0.0.4c 23.10.16 Added some assertions for bounds checking.
    v0.0.4b 07.10.16 Fixed some casting issues when tm_size_t is signed.
    v0.0.4a 29.09.16 Fixed a bug where inputting an escaped {{ was resulting in an infinite loop.
    v0.0.4  29.09.16 Added signed/unsigned char, short and long handling.
                     Fixed compiler errors when compiling with clang.
    v0.0.3  27.09.16 Added printing custom types by overloading snprint.
                     Added initial_formatting parameter to snprint so that custom printing can.
                     Inherit formatting options.
    v0.0.2  26.09.16 Changed makeFlags to tmp_type_flags so that it is guaranteed to be a.
                     Compile time constant.
                     Added string view overloads so that print can accept a string view as the.
                     Format parameter.
                     Fixed some compiler warnings when tm_size_t is defined as int.
    v0.0.1  24.09.16 Initial commit.
*/

/* This is a generated file, do not modify directly. You can find the generator files in the src directory. */

// clang-format off
#ifdef TM_PRINT_IMPLEMENTATION
    /* string.h dependency */
    #if !defined(TM_MEMCPY) || !defined(TM_MEMSET) || !defined(TM_MEMCHR) || !defined(TM_STRLEN)
        #include <cstring>
    #endif
    #ifndef TM_MEMCPY
        #define TM_MEMCPY std::memcpy
    #endif
    #ifndef TM_MEMMOVE
        #define TM_MEMMOVE std::memmove
    #endif
    #ifndef TM_MEMSET
        #define TM_MEMSET std::memset
    #endif
    #ifndef TM_MEMCHR
        #define TM_MEMCHR std::memchr
    #endif
    #ifndef TM_STRLEN
        #define TM_STRLEN std::strlen
    #endif

    /* Global allocation functions to use. */
    #if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
        // Either all or none have to be defined.
        #include <cstdlib>
        #define TM_MALLOC(size, alignment) std::malloc((size))
        #define TM_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) std::realloc((ptr), (new_size))
        #define TM_FREE(ptr, size, alignment) std::free((ptr))
    #endif

    /*
    What dependency/backend to use to convert values.
    Available backends are:
        Integer values (int32_t, int64_t, uint32_t, uint64_t):
            TMP_INT_BACKEND_CRT                inefficient, only base 10, 16 and 8 are possible, locale dependent
            TMP_INT_BACKEND_TM_CONVERSION      efficient, because size calculations are possible without printing
            TMP_INT_BACKEND_CHARCONV           needs C++17/20 standard library, but postprocessing needed after printing
        Floating point values (float, double):
            TMP_FLOAT_BACKEND_CRT              might not do correct rounding, locale dependent
            TMP_FLOAT_BACKEND_TM_CONVERSION    very inaccurate, hex and scientific printing not implemented yet
            TMP_FLOAT_BACKEND_CHARCONV         needs C++17/20 standard library, probably fast and accurate
    */
    #if !defined(TMP_INT_BACKEND_CRT) && !defined(TMP_INT_BACKEND_TM_CONVERSION) && !defined(TMP_INT_BACKEND_CHARCONV)
        #define TMP_INT_BACKEND_CRT
    #endif
    #if !defined(TMP_FLOAT_BACKEND_CRT) && !defined(TMP_FLOAT_BACKEND_TM_CONVERSION) && !defined(TMP_FLOAT_BACKEND_CHARCONV)
        #define TMP_FLOAT_BACKEND_CRT
    #endif

    #if defined(TMP_INT_BACKEND_CRT) + defined(TMP_INT_BACKEND_TM_CONVERSION) + defined(TMP_INT_BACKEND_CHARCONV) != 1
        #error "Only one backend for int is allowed."
    #endif

    #if defined(TMP_FLOAT_BACKEND_CRT) + defined(TMP_FLOAT_BACKEND_TM_CONVERSION) + defined(TMP_FLOAT_BACKEND_CHARCONV) != 1
        #error "Only one backend for float is allowed."
    #endif

    #if defined(TMP_INT_BACKEND_TM_CONVERSION) || defined(TMP_FLOAT_BACKEND_TM_CONVERSION)
        #define TMP_TM_CONVERSION_INCLUDED
        // in case you want to use tm_conversion but just don't want this header to include it
        #ifndef TMP_NO_INCLUDE_TM_CONVERSION
            #include <tm_conversion.h>
        #endif
    #endif

    #if defined(TMP_INT_BACKEND_CRT) || defined(TMP_FLOAT_BACKEND_CRT) || !defined(TMP_NO_CRT_FILE_PRINTING)
        #define TMP_CRT_INCLUDED
        #include <cstdio>
        #include <cinttypes>
    #endif

    #if defined(TMP_INT_BACKEND_CHARCONV) || defined(TMP_FLOAT_BACKEND_CHARCONV)
        #define TMP_CHARCONV_INCLUDED
        #include <charconv>
    #endif

    #if !defined(TMP_STRTOUL) && (!defined(TMP_TM_CONVERSION_INCLUDED) && !defined(TMP_CHARCONV_INCLUDED))
        #include <cstdlib>
        #define TMP_STRTOUL strtoul
    #endif

    #if !defined(TMP_SNPRINTF) && (defined(TMP_INT_BACKEND_CRT) || defined(TMP_FLOAT_BACKEND_CRT))
        #include <cstdio>
        #if defined(_MSC_VER) && _MSC_VER <= 1800
            // in case _MSC_VER is <= Visual Studio 2013
            // snprintf wasn't supported until after Visual Studio 2013
            #define TMP_SNPRINTF _snprintf
        #else
            #define TMP_SNPRINTF snprintf
        #endif
    #endif

    #if !defined(TMP_TOUPPER) && defined(TMP_INT_BACKEND_CHARCONV)
        #include <cctype>
        #define TMP_TOUPPER toupper
    #endif

    // The buffer size used for small buffer optimization, change this by profiling if malloc is a bottleneck.
    #ifndef TMP_SBO_SIZE
        #define TMP_SBO_SIZE 200
    #endif  // !defined( TMP_SBO_SIZE )
#endif


#ifndef _TM_PRINT_H_INCLUDED_14E73C89_58CA_4CC4_9D19_99F0A3D7EA07_
#define _TM_PRINT_H_INCLUDED_14E73C89_58CA_4CC4_9D19_99F0A3D7EA07_

#define TMP_VERSION 0x00000015u

/* assert */
#ifndef TM_ASSERT
    #include <assert.h>
    #define TM_ASSERT assert
#endif /* !defined(TM_ASSERT) */

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* Optional STL extensions, like printing directly to std::string. */
#ifdef TMP_USE_STL
    #include <string>
#endif

#include <type_traits>

/* Linkage defaults to extern, to override define TMP_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMP_DEF
    #define TMP_DEF extern
#endif

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

// default print flags value
#ifndef TMP_DEFAULT_FLAGS
    #define TMP_DEFAULT_FLAGS 0u
#endif

// clang-format on

namespace tml {

namespace PrintFlags {
enum Values : unsigned int {
    Fixed = (1u << 0u),
    Scientific = (1u << 1u),
    Hex = (1u << 2u),
    Shortest = (1u << 3u),
    TrailingZeroes = (1u << 4u),
    BoolAsNumber = (1u << 5u),
    Lowercase = (1u << 6u),
    Sign = (1u << 7u),
    Char = (1u << 8u),
    LeftJustify = (1u << 9u),
    PrependHex = (1u << 10u),
    PrependBinary = (1u << 11u),
    PrependOctal = (1u << 12u),
    EmitDecimalPoint = (1u << 13u),
    PadWithSpaces = (1u << 14u),

    // Misc.
    Default = TMP_DEFAULT_FLAGS,

    Count = 18,
    ClearMask = ((1u << 9u) - 1),
    General = Fixed | Scientific
};
}

struct PrintFormat {
    int base;
    int precision;
    int width;
    unsigned int flags;
};

inline PrintFormat default_print_format() { return PrintFormat{10, 6, 0, PrintFlags::Default}; }

namespace PrintType {
enum Values : uint64_t {
    Char = 1,
    Bool,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double,
    String,
    StringView,
    Custom,

    Last,  // Only here to check against Count to see if they are the same (see static_asserts below).

    // Count needs to be power of two so that Mask and Bits is valid.
    Count = 16,
    Mask = Count - 1,
    Bits = 4
};
}

#if !defined(TMP_TM_CONVERSION_INCLUDED)
struct PrintFormattedResult {
    tm_size_t size;
    tm_errc ec;
};
#else
typedef tmc_conv_result PrintFormattedResult;
#endif

// Sanity checks
static_assert(PrintType::Last <= PrintType::Count, "Values added to PrintType without adjusting Count and Bits");
static_assert((PrintType::Count & (PrintType::Count - 1)) == 0, "Count must be power of two");
static_assert((sizeof(uint64_t) * 8) / PrintType::Bits >= PrintType::Count,
              "Can't store PrintType::Count in a uint64_t");

#ifdef TMP_CUSTOM_PRINTING
// Custom printing functions should adhere to snprint/snprintf return value customs:
// Return used size, if used size is < len.
// Return necessary size, if necessary size >= len.
// Return -1 on error.
typedef int tmp_custom_printer_type(char* buffer, tm_size_t len, const PrintFormat& initial_formatting,
                                    const void* data);
#endif

union PrintValue {
    char c;
    bool b;
    int32_t i32;
    uint32_t u32;
    int64_t i64;
    uint64_t u64;
    float f;
    double d;
    const char* s;
    struct {
        const char* data;
        tm_size_t size;
    } v;
#ifdef TMP_CUSTOM_PRINTING
    struct {
        const void* data;
        tmp_custom_printer_type* customPrint;
    } custom;
#endif
};

struct PrintArgList {
    PrintValue* args;
    uint64_t flags;
    unsigned int size;
};

/*
snprint:
Basic formatting function. See DESCRIPTION for an explanation of how format strings are structured.
Always null-terminates output on success.
Params:
    dest:   Output buffer. Can be nullptr iff len == 0.
    len:    Output buffer length.
    format: Format string. See DESCRIPTION for an explanation of how format strings are structured.
    args:   Variadic arguments to be used when formatting. There can't be more than PrintFlags::Count
            variadic arguments (by default this is 16). Use more than one call to snprint when needed instead.
Result:
    Return value conforms to ::snprintf.
    Returns used size, if used size is < len.
    Returns necessary size, if necessary size >= len.
    Returns -1 on error.

    Notes:
        On success:
            Return value is strictly less than len parameter (result < len).
            Returns how many bytes were written into dest, not including the null-terminator.
            Output is always null-terminated.
        On failure:
            Returns required buffer size not including null-terminator if buffer in dest is not big enough.
            A return value equalling the len parameter (result == len) still indicates failure, since
            one of the output bytes was overwritten for null-termination.
            It is not safe to read from dest on failure.
        On formatting errors:
            Returns -1 on formatting errors. Formatting is not possible in this case.
*/
template <class... Types>
int snprint(char* dest, tm_size_t len, const char* format, const Types&... args);

// Define TMP_NO_CRT_FILE_PRINTING if you don't need printing to stdout or to FILE* handles.
// clang-format off
#ifndef TMP_NO_CRT_FILE_PRINTING
    TMP_DEF tm_errc tmp_print(FILE* out, const char* format, size_t format_len, const PrintFormat& initial_formatting,
                              const PrintArgList& args);
    #ifdef TM_STRING_VIEW
        TMP_DEF tm_errc tmp_print(FILE* out, TM_STRING_VIEW format, const PrintFormat& initial_formatting,
                                  const PrintArgList& args);
    #endif  // TM_STRING_VIEW
#endif  // TMP_NO_CRT_FILE_PRINTING
// clang-format on

TMP_DEF int tmp_snprint(char* dest, tm_size_t len, const char* format, size_t format_len,
                        const PrintFormat& initial_formatting, const PrintArgList& args);
#ifdef TM_STRING_VIEW
TMP_DEF int tmp_snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initial_formatting,
                        const PrintArgList& args);
#endif

TMP_DEF tm_size_t tmp_parse_print_format(const char* format_specifiers, tm_size_t format_specifiers_len,
                                         PrintFormat* out);

#ifdef TMP_USE_STL
TMP_DEF ::std::string tmp_string_format(const char* format, size_t format_len, const PrintFormat& initial_formatting,
                                        const PrintArgList& args);
#endif

template <class... Types>
struct tmp_type_flags;

template <>
struct tmp_type_flags<> {
    enum : uint64_t { value = 0 };
};

template <class... Types>
struct tmp_type_flags<char, Types...> {
    enum : uint64_t { value = PrintType::Char | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};

// integer types to enum mapping based on their size
template <size_t N>
struct tmp_int_size;

template <>
struct tmp_int_size<1> {
    enum : uint64_t { value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<2> {
    enum : uint64_t { value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<4> {
    enum : uint64_t { value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<8> {
    enum : uint64_t { value = PrintType::Int64 };
    typedef int64_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.i64 = v;
        return result;
    }
};
template <size_t N>
struct tmp_uint_size;
template <>
struct tmp_uint_size<1> {
    enum : uint64_t { value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<2> {
    enum : uint64_t { value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<4> {
    enum : uint64_t { value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<8> {
    enum : uint64_t { value = PrintType::UInt64 };
    typedef uint64_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.u64 = v;
        return result;
    }
};

template <class... Types>
struct tmp_type_flags<signed char, Types...> {
    enum : uint64_t {
        value = tmp_int_size<sizeof(signed char)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned char, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned char)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<short, Types...> {
    enum : uint64_t {
        value = tmp_int_size<sizeof(short)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned short, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned short)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<long, Types...> {
    enum : uint64_t {
        value = tmp_int_size<sizeof(long)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned long, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned long)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<int, Types...> {
    enum : uint64_t { value = tmp_int_size<sizeof(int)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<unsigned int, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned int)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<long long, Types...> {
    enum : uint64_t {
        value = tmp_int_size<sizeof(long long)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned long long, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned long long)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};

template <class... Types>
struct tmp_type_flags<bool, Types...> {
    enum : uint64_t { value = PrintType::Bool | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<float, Types...> {
    enum : uint64_t { value = PrintType::Float | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<double, Types...> {
    enum : uint64_t { value = PrintType::Double | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<const char*, Types...> {
    enum : uint64_t { value = PrintType::String | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<char*, Types...> {
    enum : uint64_t { value = PrintType::String | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
#ifdef TM_STRING_VIEW
template <class... Types>
struct tmp_type_flags<TM_STRING_VIEW, Types...> {
    enum : uint64_t { value = PrintType::StringView | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
#endif
#ifdef TMP_CUSTOM_PRINTING
template <class T, class... Types>
struct tmp_type_flags<T, Types...> {
    enum : uint64_t { value = PrintType::Custom | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
#else
template <class T, class... Types>
struct tmp_type_flags<T, Types...> {
    enum : uint64_t { value = 0 };
};
#endif

template <class... Types>
void fill_print_arg_list(PrintArgList* list, char value, const Types&... args) {
    list->args[list->size++].c = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, signed char value, const Types&... args) {
    typedef tmp_int_size<sizeof(signed char)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned char value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned char)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, short value, const Types&... args) {
    typedef tmp_int_size<sizeof(short)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned short value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned short)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, long value, const Types&... args) {
    typedef tmp_int_size<sizeof(long)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned long value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned long)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, bool value, const Types&... args) {
    list->args[list->size++].b = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, int value, const Types&... args) {
    typedef tmp_int_size<sizeof(int)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned int value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned int)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, long long value, const Types&... args) {
    typedef tmp_int_size<sizeof(long long)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned long long value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned long long)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, float value, const Types&... args) {
    list->args[list->size++].f = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, double value, const Types&... args) {
    list->args[list->size++].d = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, const char* value, const Types&... args) {
    list->args[list->size++].s = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, char* value, const Types&... args) {
    list->args[list->size++].s = value;
    fill_print_arg_list(list, args...);
}
#ifdef TM_STRING_VIEW
template <class... Types>
void fill_print_arg_list(PrintArgList* list, TM_STRING_VIEW value, const Types&... args) {
    list->args[list->size].v.data = TM_STRING_VIEW_DATA(value);
    list->args[list->size].v.size = TM_STRING_VIEW_SIZE(value);
    ++list->size;
    fill_print_arg_list(list, args...);
}
#endif
#ifdef TMP_CUSTOM_PRINTING
template <class T, class... Types>
void fill_print_arg_list(PrintArgList* list, const T& value, const Types&... args);
#else
template <class T, class... Types>
void fill_print_arg_list(PrintArgList*, const T&, const Types&...) {
    static_assert(tmp_type_flags<T>::value != 0,
                  "T is not printable, custom printing is disabled (TMP_CUSTOM_PRINTING not defined)");
    static_assert(tmp_type_flags<T>::value == 0, "");  // this function is not allowed to be instantiated
}
#endif
void fill_print_arg_list(PrintArgList*) {}

template <class... Types>
void make_print_arg_list(PrintArgList* list, size_t capacity, const Types&... args) {
    list->flags = tmp_type_flags<typename std::decay<Types>::type...>::value;
    list->size = 0;
    fill_print_arg_list(list, args...);
    (void)capacity;
    TM_ASSERT(list->size == capacity);
}
template <class... Types>
void make_print_arg_list(PrintArgList* list, size_t capacity) {
    list->flags = 0;
    list->size = 0;
    (void)capacity;
    TM_ASSERT(list->size == capacity);
}

// Wrapper to an arguments list array on the stack.
// The array size is a ternary expression so that the argument list also works with zero arguments.
#define TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args)                                              \
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print"); \
    PrintValue values[sizeof...(args) ? sizeof...(args) : 1];                                   \
    PrintArgList arg_list = {values, /*flags=*/0, /*size=*/0};                                  \
    make_print_arg_list(&arg_list, sizeof...(args), args...);

#ifndef TMP_NO_CRT_FILE_PRINTING
template <class... Types>
tm_errc print(const char* format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(stdout, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
tm_errc print(FILE* out, const char* format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(out, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
tm_errc print(const char* format, const PrintFormat& initial_formatting, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(stdout, format, TM_STRLEN(format), initial_formatting, arg_list);
}
template <class... Types>
tm_errc print(FILE* out, const char* format, const PrintFormat& initial_formatting, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(out, format, TM_STRLEN(format), initial_formatting, arg_list);
}
// impl

#ifdef TM_STRING_VIEW
template <class... Types>
tm_errc print(TM_STRING_VIEW format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(stdout, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
tm_errc print(FILE* out, TM_STRING_VIEW format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(out, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
tm_errc print(TM_STRING_VIEW format, const PrintFormat& initial_formatting, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(stdout, TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), initial_formatting, arg_list);
}
template <class... Types>
tm_errc print(FILE* out, TM_STRING_VIEW format, const PrintFormat& initial_formatting, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(out, TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), initial_formatting, arg_list);
}
#endif  // defined(TM_STRING_VIEW)
#endif  // TMP_NO_CRT_FILE_PRINTING

template <class... Types>
int snprint(char* dest, tm_size_t len, const char* format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_snprint(dest, len, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
int snprint(char* dest, tm_size_t len, const char* format, const PrintFormat& initial_formatting,
            const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_snprint(dest, len, format, TM_STRLEN(format), initial_formatting, arg_list);
}
#ifdef TM_STRING_VIEW
template <class... Types>
int snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_snprint(dest, len, TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), default_print_format(),
                       arg_list);
}
template <class... Types>
int snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initial_formatting,
            const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_snprint(dest, len, TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), initial_formatting,
                       arg_list);
}
#endif  // defined(TM_STRING_VIEW)

#ifdef TMP_USE_STL
template <class... Types>
::std::string string_format(const char* format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_string_format(format, TM_STRLEN(format), default_print_format(), arg_list);
}
#ifdef TM_STRING_VIEW
template <class... Types>
::std::string string_format(TM_STRING_VIEW format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_string_format(TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), default_print_format(),
                             arg_list);
}
#endif  // defined(TM_STRING_VIEW)
#endif  // defined(TMP_USE_STL)

#undef TMP_INTERNAL_MAKE_ARG_LIST

#ifdef TMP_CUSTOM_PRINTING
// This looks very confusing, but it checks for the existence of a specific overload of snprint.
// This way we can do a static_assert on whether the overload exists and report an error otherwise.
template <class T>
class tmp_has_custom_printer {
    typedef char no;

    template <class C>
    static auto test(C c)
        -> decltype(static_cast<int (*)(char*, tm_size_t, const ::tml::PrintFormat&, const C&)>(&snprint));
    template <class C>
    static no test(...);

   public:
    enum { value = (sizeof(test<T>(T{})) == sizeof(void*)) };
};

template <class T, class... Types>
void fill_print_arg_list(PrintArgList* list, const T& value, const Types&... args) {
    static_assert(tmp_has_custom_printer<T>::value,
                  "T is not printable, there is no snprint that takes value of type T");
    // If the static assert fails, the compiler will also report that there are no overloads of snprint that accept the
    // argument types. We could get rid of that error by using SFINAE but that introduces more boilerplate.

    // Having constexpr if simplifies the error message.
#ifdef TMP_HAS_CONSTEXPR_IF
    if constexpr (tmp_has_custom_printer<T>::value)
#endif
    {
        auto custom = &list->args[list->size++].custom;
        custom->data = &value;
        custom->customPrint = [](char* buffer, tm_size_t len, const PrintFormat& initial_formatting, const void* data) {
            return snprint(buffer, len, initial_formatting, *(const T*)data);
        };
    }
    fill_print_arg_list(list, args...);
}
#endif  // defined(TMP_CUSTOM_PRINTING)

}  // namespace tml

#endif  // _TM_PRINT_H_INCLUDED_14E73C89_58CA_4CC4_9D19_99F0A3D7EA07_

#ifdef TM_PRINT_IMPLEMENTATION
#ifndef TM_MIN
	#define TM_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

// clang-format off
#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

namespace tml {

namespace {

struct PrintSizes {
    tm_size_t digits;
    tm_size_t decorated;
    tm_size_t size;
};

#if !defined(TMP_TM_CONVERSION_INCLUDED)
    static PrintFormattedResult scan_u32_n(const char* str, tm_size_t maxlen, uint32_t* out, int32_t base) {
        TM_ASSERT(out);
        TM_ASSERT(base >= 2 && base <= 36);
        TM_ASSERT_VALID_SIZE(maxlen);

        PrintFormattedResult result = {maxlen, TM_EOVERFLOW};
        if (maxlen <= 0) return result;
        #if defined(TMP_CHARCONV_INCLUDED)
            auto std_result = std::from_chars(str, str + maxlen, *out, base);
            if(std_result.ec == std::errc{}) {
                result.size = (tm_size_t)(std_result.ptr - str);
                result.ec = TM_OK;
            }
        #elif defined(TMP_STRTOUL)
            char* endptr = nullptr;
            auto value = TMP_STRTOUL(str, &endptr, base);
            auto size = (tm_size_t)(endptr - str);
            if (endptr && size > 0 && size <= maxlen) {
                *out = value;
                result.size = size;
                result.ec = TM_OK;
            }
        #else
            #error "TMP_STRTOUL not defined"
        #endif
        return result;
    }
#else
    template <class T>
    PrintSizes tmp_get_print_sizes(T value, const PrintFormat& format, bool negative);
#endif
// clang-format on

static bool tmp_parse_format_specifiers(const char* p, tm_size_t len, PrintFormat* format, uint32_t* current_index) {
    if (len <= 0) return false;
    bool index_specified = false;
    const char* end = p + len;
    auto scan_index_result = scan_u32_n(p, (tm_size_t)(end - p), current_index, 10);
    if (scan_index_result.ec == TM_OK) {
        p += scan_index_result.size;
        index_specified = true;
    }

    if (p < end && *p == ':') {
        ++p;
        tmp_parse_print_format(p, (tm_size_t)(end - p), format);
    }

    return index_specified;
}

template <class T>
struct UnsignedPair {
    T value;
    bool negative;
};

static UnsignedPair<double> tmp_make_unsigned(double v) {
    bool negative = v < 0;
    if (negative) v = -v;
    return {v, negative};
}
static UnsignedPair<float> tmp_make_unsigned(float v) {
    bool negative = v < 0;
    if (negative) v = -v;
    return {v, negative};
}

#if !defined(TMP_INT_BACKEND_TM_CONVERSION)
static UnsignedPair<uint32_t> tmp_make_unsigned(int32_t v) {
    bool negative = v < 0;
    if (negative) v = -v;
    return {(uint32_t)v, negative};
}
static UnsignedPair<uint64_t> tmp_make_unsigned(int64_t v) {
    bool negative = v < 0;
    if (negative) v = -v;
    return {(uint64_t)v, negative};
}

static UnsignedPair<uint32_t> tmp_make_unsigned(uint32_t v) { return {v, false}; }
static UnsignedPair<uint64_t> tmp_make_unsigned(uint64_t v) { return {v, false}; }
#endif  // !defined(TMP_INT_BACKEND_TM_CONVERSION)

static PrintFormattedResult tmp_print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format,
                                                const char* str, tm_size_t str_len, tm_size_t str_width) {
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT_VALID_SIZE(str_len);
    TM_ASSERT_VALID_SIZE(str_width);

    const auto width = format.width;
    tm_size_t size = str_len;
    if (format.width > 0) {
        tm_size_t spaces = ((tm_size_t)format.width > str_width) ? ((tm_size_t)format.width - width) : 0;
        size = str_len + spaces;
    }
    if (size > maxlen) {
        return {maxlen, TM_EOVERFLOW};
    }

    auto remaining = maxlen;

    if (width > 0 && (tm_size_t)width > str_len) {
        tm_size_t padding = (tm_size_t)width - str_len;
        if (!(format.flags & PrintFlags::LeftJustify)) {
            TM_ASSERT(padding <= remaining);
            TM_MEMSET(dest, ' ', (size_t)padding);
            dest += padding;
            remaining -= padding;
        } else {
            TM_ASSERT(padding + str_len <= maxlen);
            TM_MEMSET(dest + str_len, ' ', (size_t)padding);
        }
    }

    TM_ASSERT(str_len <= remaining);
    TM_MEMCPY(dest, str, (size_t)str_len);
    return {size, TM_OK};
}

static PrintFormattedResult tmp_print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format, bool value) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    auto flags = format.flags;
    if (flags & PrintFlags::BoolAsNumber) {
        return tmp_print_formatted(dest, maxlen, format, (value) ? "1" : "0", 1, 1);
    }
    const bool lowercase = ((flags & PrintFlags::Lowercase) != 0);
    const char* str = nullptr;
    tm_size_t str_len = 0;
    if (value) {
        str = (lowercase) ? "true" : "TRUE";
        str_len = 4;
    } else {
        str = (lowercase) ? "false" : "FALSE";
        str_len = 5;
    }
    return tmp_print_formatted(dest, maxlen, format, str, str_len, str_len);
}

static tm_size_t tmp_get_decorated_size(tm_size_t digits, const PrintFormat& format, bool negative) {
    tm_size_t result = digits;
    if (negative || (format.flags & PrintFlags::Sign)) {
        ++result;
    }
    if ((format.flags & PrintFlags::PrependHex) || (format.flags & PrintFlags::PrependBinary)) {
        result += 2;
    }
    if (format.flags & PrintFlags::PrependOctal) {
        result += 1;
    }
    return result;
}

static void tmp_print_decoration(char* dest, tm_size_t maxlen, const PrintSizes& sizes, const PrintFormat& format,
                                 bool negative) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(sizes.size <= maxlen);
    auto remaining = maxlen;

    char* decorate_pos = dest;
    const auto flags = format.flags;

    // padding
    if (format.width > 0 && sizes.size <= (tm_size_t)format.width) {
        if (!(flags & PrintFlags::LeftJustify)) {
            auto padding = (tm_size_t)format.width - sizes.decorated;
            TM_ASSERT(padding <= maxlen);
            if (flags & PrintFlags::PadWithSpaces) {
                decorate_pos = dest + padding;
            } else {
                TM_ASSERT(sizes.decorated >= sizes.digits);
                dest += sizes.decorated - sizes.digits;
            }
            TM_MEMSET(dest, (flags & PrintFlags::PadWithSpaces) ? ' ' : '0', padding);
            dest += padding;
            remaining -= padding;
        } else {
            dest += sizes.decorated;
            TM_ASSERT((tm_size_t)format.width >= sizes.decorated);
            TM_MEMSET(dest, ' ', (tm_size_t)format.width - sizes.decorated);
        }
    } else {
        TM_ASSERT(sizes.decorated >= sizes.digits);
        dest += sizes.decorated - sizes.digits;
    }

    if (negative) {
        TM_ASSERT(remaining > 0);
        *decorate_pos++ = '-';
        --remaining;
    } else if (flags & PrintFlags::Sign) {
        TM_ASSERT(remaining > 0);
        *decorate_pos++ = '+';
        --remaining;
    }

    if (flags & PrintFlags::PrependHex) {
        *decorate_pos++ = '0';
        *decorate_pos++ = (flags & PrintFlags::Lowercase) ? 'x' : 'X';
        remaining -= 2;
    } else if (flags & PrintFlags::PrependBinary) {
        *decorate_pos++ = '0';
        *decorate_pos++ = (flags & PrintFlags::Lowercase) ? 'b' : 'B';
        remaining -= 2;
    } else if (flags & PrintFlags::PrependOctal) {
        *decorate_pos++ = '0';
        --remaining;
    }
}

static PrintFormattedResult tmp_move_printed_value_and_decorate(char* dest, tm_size_t maxlen, const PrintFormat& format,
                                                                PrintFormattedResult print_result, bool negative) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    if (print_result.ec != TM_OK) return print_result;

    PrintSizes sizes = {};
    sizes.digits = print_result.size;
    sizes.decorated = tmp_get_decorated_size(sizes.digits, format, negative);
    sizes.size = sizes.decorated;

    auto width = format.width;
    if (width > 0 && sizes.size < (tm_size_t)width) {
        sizes.size = (tm_size_t)width;
    }
    TM_ASSERT(sizes.size >= sizes.decorated);
    TM_ASSERT(sizes.size >= sizes.digits);
    TM_ASSERT(sizes.decorated >= sizes.digits);

    tm_size_t digits_pos = 0;
    if (width <= 0 || (format.flags & PrintFlags::LeftJustify)) {
        digits_pos = sizes.decorated - sizes.digits;
    } else {
        digits_pos = sizes.size - sizes.digits;
    }
    if (digits_pos > 0) {
        tm_size_t sign = negative || ((format.flags & PrintFlags::Sign) != 0);
        if (digits_pos != sign) {
            TM_MEMMOVE(dest + digits_pos, dest + sign, sizes.digits);
        }
    }

    if (sizes.size > maxlen) {
        print_result.size = sizes.size;
        print_result.ec = TM_ERANGE;
        return print_result;
    }
    tmp_print_decoration(dest, maxlen, sizes, format, negative);
    print_result.size = sizes.size;
    return print_result;
}

#ifdef TMP_INT_BACKEND_CRT
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, uint32_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    PrintFormattedResult result = {maxlen, TM_EOVERFLOW};
    const char* format_string = "%" PRIu32;
    if (format.base == 16) {
        format_string = (format.flags & PrintFlags::Lowercase) ? "%" PRIx32 : "%" PRIX32;
    } else if (format.base == 8) {
        format_string = "%" PRIo32;
    }
    int size = TMP_SNPRINTF(dest, (size_t)maxlen, format_string, value);
    if (size < 0) {
        result.ec = TM_EINVAL;
    } else if ((tm_size_t)size >= maxlen) {
        result.size = (tm_size_t)size;
        result.ec = TM_ERANGE;
    } else {
        result.size = (tm_size_t)size;
        result.ec = TM_OK;
    }
    return result;
}
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, uint64_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    PrintFormattedResult result = {};
    const char* format_string = "%" PRIu64;
    if (format.base == 16) {
        format_string = (format.flags & PrintFlags::Lowercase) ? "%" PRIx64 : "%" PRIX64;
    } else if (format.base == 8) {
        format_string = "%" PRIo64;
    }
    int size = TMP_SNPRINTF(dest, (size_t)maxlen, format_string, value);
    if (size < 0) {
        result.ec = TM_EINVAL;
    } else if ((tm_size_t)size >= maxlen) {
        result.size = (tm_size_t)size;
        result.ec = TM_ERANGE;
    } else {
        result.size = (tm_size_t)size;
        result.ec = TM_OK;
    }
    return result;
}
#endif  // defined(TMP_INT_BACKEND_CRT)

#ifdef TMP_INT_BACKEND_TM_CONVERSION
static tm_size_t tmp_get_digits_count_decimal(uint64_t value) { return get_digits_count_decimal_u64(value); }
static tm_size_t tmp_get_digits_count_decimal(uint32_t value) { return get_digits_count_decimal_u32(value); }
static tm_size_t tmp_get_digits_count_hex(uint32_t value) { return get_digits_count_hex_u32(value); }
static tm_size_t tmp_get_digits_count_hex(uint64_t value) { return get_digits_count_hex_u64(value); }
static tm_size_t tmp_get_digits_count(uint32_t value, int32_t base) { return get_digits_count_u32(value, base); }
static tm_size_t tmp_get_digits_count(uint64_t value, int32_t base) { return get_digits_count_u64(value, base); }

template <class T>
PrintSizes tmp_get_print_sizes(T value, const PrintFormat& format, bool negative) {
    static_assert(std::is_unsigned<T>::value, "T has to be unsigned");

    PrintSizes result = {};

    switch (format.base) {
        case 10: {
            result.digits = tmp_get_digits_count_decimal(value);
            break;
        }
        case 16: {
            result.digits = tmp_get_digits_count_hex(value);
            break;
        }
        default: {
            result.digits = tmp_get_digits_count(value, format.base);
            break;
        }
    }

    result.decorated = tmp_get_decorated_size(result.digits, format, negative);
    result.size = result.decorated;
    if (format.width > 0 && result.size < (tm_size_t)format.width) {
        result.size = (tm_size_t)format.width;
    }

    TM_ASSERT(result.size >= result.decorated);
    TM_ASSERT(result.size >= result.digits);
    TM_ASSERT(result.decorated >= result.digits);
    return result;
}

static PrintFormattedResult tmp_print_value_decimal_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value) {
    return print_decimal_u32_w(dest, maxlen, width, value);
}
static PrintFormattedResult tmp_print_value_decimal_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value) {
    return print_decimal_u64_w(dest, maxlen, width, value);
}
static PrintFormattedResult tmp_print_value_hex_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value,
                                                  bool lowercase) {
    return print_hex_u32_w(dest, maxlen, width, value, lowercase);
}
static PrintFormattedResult tmp_print_value_hex_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value,
                                                  bool lowercase) {
    return print_hex_u64_w(dest, maxlen, width, value, lowercase);
}
static PrintFormattedResult tmp_print_value_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value,
                                              int32_t base, bool lowercase) {
    return print_u32_w(dest, maxlen, width, value, base, lowercase);
}
static PrintFormattedResult tmp_print_value_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value,
                                              int32_t base, bool lowercase) {
    return print_u64_w(dest, maxlen, width, value, base, lowercase);
}

template <class T>
static PrintFormattedResult tmp_print_formatted_unsigned(char* dest, tm_size_t maxlen, const PrintSizes& sizes,
                                                         const PrintFormat& format, T value, bool negative) {
    PrintFormattedResult result = {0, TM_OK};
    if (sizes.size > maxlen) {
        result.size = sizes.size;
        result.ec = TM_ERANGE;
        return result;
    }
    result.size = sizes.size;

    tmp_print_decoration(dest, maxlen, sizes, format, negative);

    tm_size_t padding = 0;
    auto flags = format.flags;
    if (flags & PrintFlags::LeftJustify) {
        padding = sizes.decorated - sizes.digits;
    } else {
        padding = sizes.size - sizes.digits;
    }
    dest += padding;
    auto remaining = maxlen - padding;
    PrintFormattedResult print_result = {};
    switch (format.base) {
        case 10: {
            print_result = tmp_print_value_decimal_w(dest, remaining, sizes.digits, value);
            break;
        }
        case 16: {
            bool lowercase = (flags & PrintFlags::Lowercase) != 0;
            print_result = tmp_print_value_hex_w(dest, remaining, sizes.digits, value, lowercase);
            break;
        }
        default: {
            bool lowercase = (flags & PrintFlags::Lowercase) != 0;
            print_result = tmp_print_value_w(dest, remaining, sizes.digits, value, format.base, lowercase);
            break;
        }
    }
    if (print_result.ec != TM_OK) {
        result.size = maxlen;
        result.ec = print_result.ec;
        return result;
    }

    return result;
}
#endif  // defined(TMP_INT_BACKEND_TM_CONVERSION)

#ifdef TMP_INT_BACKEND_CHARCONV
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, uint32_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    auto std_result = std::to_chars(dest, dest + maxlen, value, format.base);
    if(std_result.ec == std::errc{}) {
        if(!(format.flags & PrintFlags::Lowercase)) {
            for(char* p = dest, *end = dest + maxlen; p < end; ++p) {
                *p = (char)TMP_TOUPPER((unsigned char)*p);
            }
        }
        return {(tm_size_t)(std_result.ptr - dest), TM_OK};
    }
    return {maxlen, TM_EOVERFLOW};
}
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, uint64_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    auto std_result = std::to_chars(dest, dest + maxlen, value, format.base);
    if(std_result.ec == std::errc{}) {
        if(!(format.flags & PrintFlags::Lowercase)) {
            for(char* p = dest, *end = dest + maxlen; p < end; ++p) {
                *p = (char)TMP_TOUPPER((unsigned char)*p);
            }
        }
        return {(tm_size_t)(std_result.ptr - dest), TM_OK};
    }
    return {maxlen, TM_EOVERFLOW};
}
#endif  // defined(TMP_INT_BACKEND_CHARCONV)

#ifdef TMP_FLOAT_BACKEND_CRT
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    PrintFormattedResult result = {maxlen, TM_EOVERFLOW};
    auto precision = format.precision;
    char fmt_buffer[5];
    char* p = fmt_buffer;
    *p++ = '%';
    if (precision >= 0) {
        *p++ = '.';
        *p++ = '*';
    }
    *p = 'f';
    if ((format.flags & PrintFlags::General) == PrintFlags::General) {
        *p = (format.flags & PrintFlags::Lowercase) ? 'g' : 'G';
    } else if (format.flags & PrintFlags::Scientific) {
        *p = (format.flags & PrintFlags::Lowercase) ? 'e' : 'E';
    } else if (format.flags & PrintFlags::Hex) {
        *p = (format.flags & PrintFlags::Lowercase) ? 'a' : 'A';
    }
    ++p;
    *p = 0;
    TM_ASSERT(p < fmt_buffer + 5);

    int size = -1;
    if (precision < 0) {
        size = TMP_SNPRINTF(dest, maxlen, fmt_buffer, value);
    } else {
        size = TMP_SNPRINTF(dest, maxlen, fmt_buffer, precision, value);
    }
    if (size < 0) {
        result.ec = TM_EINVAL;
    } else if ((tm_size_t)size >= maxlen) {
        result.size = (tm_size_t)size;
        result.ec = TM_ERANGE;
    } else {
        if (format.flags & PrintFlags::Hex) {
            // Remove 0x prefix to make output same as other backends.
            if (size > 2) {
                TM_ASSERT(dest[0] == '0');
                TM_ASSERT(dest[1] == 'x' || dest[1] == 'X');
                TM_MEMMOVE(dest, dest + 2, size - 2);
                size -= 2;
            }
        }
        result.size = (tm_size_t)size;
        result.ec = TM_OK;
    }
    return result;
}
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
    return tmp_print_value(dest, maxlen, (double)value, format);
}
#endif  // defined(TMP_FLOAT_BACKEND_CRT)

#ifdef TMP_FLOAT_BACKEND_TM_CONVERSION

static uint32_t tmp_convert_flags(uint32_t flags) {

#define tmp_version_mismatch_error              \
    "Internal Error: Incompatible print flags." \
    " Are tm_conversion.h and tm_print.h versions mismatched?"

    // Make sure that print flags are 1 to 1 compatible with the ones in tm_conversion.h.
    static_assert((uint32_t)PrintFlags::Fixed == (uint32_t)PF_FIXED, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Scientific == (uint32_t)PF_SCIENTIFIC, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Hex == (uint32_t)PF_HEX, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Shortest == (uint32_t)PF_SHORTEST, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::TrailingZeroes == (uint32_t)PF_TRAILING_ZEROES, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::BoolAsNumber == (uint32_t)PF_BOOL_AS_NUMBER, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Lowercase == (uint32_t)PF_LOWERCASE, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Sign == (uint32_t)PF_SIGNBIT, tmp_version_mismatch_error);
    static_assert(PF_COUNT == 8, tmp_version_mismatch_error);
#undef tmp_version_mismatch_error

    // Assuming that flags are 1 to 1 compatible with the ones in tm_conversion.h.
    return flags & ((1u << PF_COUNT) - 1);  // Mask out flags not defined in tm_conversion.
}

static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
    return print_double(dest, maxlen, value, tmp_convert_flags(format.flags), format.precision);
}
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
    return print_double(dest, maxlen, (double)value, tmp_convert_flags(format.flags), format.precision);
}
#endif  // defined(TMP_FLOAT_BACKEND_TM_CONVERSION)

#ifdef TMP_FLOAT_BACKEND_CHARCONV
std::chars_format tmp_to_std_flags(unsigned flags) {
    std::chars_format result = std::chars_format::fixed;
    if ((flags & PrintFlags::General) == PrintFlags::General) {
        result = std::chars_format::fixed | std::chars_format::scientific;
    } else if (flags & PrintFlags::Scientific) {
        result = std::chars_format::scientific;
    } else if (flags & PrintFlags::Hex) {
        result = std::chars_format::hex;
    }
    return result;
}

static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    auto precision = format.precision;
    std::to_chars_result std_result = {};
    if (precision < 0) {
        std_result = std::to_chars(dest, dest + maxlen, value, tmp_to_std_flags(format.flags));
    } else {
        std_result = std::to_chars(dest, dest + maxlen, value, tmp_to_std_flags(format.flags), precision);
    }
    if (std_result.ec == std::errc{}) {
        return {(tm_size_t)(std_result.ptr - dest), TM_OK};
    }
    return {maxlen, TM_EOVERFLOW};
}
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    auto precision = format.precision;
    std::to_chars_result std_result = {};
    if (precision < 0) {
        std_result = std::to_chars(dest, dest + maxlen, value, tmp_to_std_flags(format.flags));
    } else {
        std_result = std::to_chars(dest, dest + maxlen, value, tmp_to_std_flags(format.flags), precision);
    }
    if (std_result.ec == std::errc{}) {
        return {(tm_size_t)(std_result.ptr - dest), TM_OK};
    }
    return {maxlen, TM_EOVERFLOW};
}
#endif  // defined(TMP_FLOAT_BACKEND_CHARCONV)

template <class T>
static PrintFormattedResult tmp_print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format, T value) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    if (maxlen <= 0) return {maxlen, TM_EOVERFLOW};
    TM_ASSERT(dest);

    auto pair = tmp_make_unsigned(value);
    bool sign = pair.negative || ((format.flags & PrintFlags::Sign) != 0);
    auto result = tmp_print_value(dest + sign, maxlen - sign, pair.value, format);
    return tmp_move_printed_value_and_decorate(dest, maxlen, format, result, pair.negative);
}

struct tmp_reallocate_result {
    char* ptr;
    tm_size_t size;
};

struct tmp_allocator_context {
    void* context;
    tmp_reallocate_result (*reallocate)(void* context, char* old_ptr, tm_size_t old_size, tm_size_t new_size);
    void (*destroy)(void* context, char* ptr, tm_size_t size);
};

static tmp_reallocate_result tmp_allocator_default_reallocate(void* /*context*/, char* old_ptr, tm_size_t old_size,
                                                              tm_size_t new_size) {
    TM_ASSERT_VALID_SIZE(new_size);

    tmp_reallocate_result result = {nullptr, 0};
    if (old_ptr && old_size > 0) {
        result.ptr =
            (char*)TM_REALLOC(old_ptr, old_size * sizeof(char), sizeof(char), new_size * sizeof(char), sizeof(char));
    } else {
        result.ptr = (char*)TM_MALLOC(new_size * sizeof(char), sizeof(char));
    }
    result.size = (result.ptr) ? new_size : 0;
    return result;
}
static void tmp_allocator_default_destroy(void* /*context*/, char* ptr, tm_size_t size) {
    (void)size;
    TM_FREE(ptr, size * sizeof(char), sizeof(char));
}

static tmp_allocator_context tmp_default_allocator() {
    return {nullptr, tmp_allocator_default_reallocate, tmp_allocator_default_destroy};
}

#ifdef TMP_USE_STL
static tmp_reallocate_result tmp_allocator_std_string_reallocate(void* context, char* /*old_ptr*/,
                                                                 tm_size_t /*old_size*/, tm_size_t new_size) {
    TM_ASSERT(context);
    TM_ASSERT_VALID_SIZE(new_size);

    auto str = static_cast<::std::string*>(context);
#if 1
    // We use two resizes, first to allocate memory, second to make use of all memory allocated.
    str->resize((size_t)new_size);
    str->resize(str->capacity());
    // TODO: This requires C++17, maybe use &((*str)[0]) instead, which isn't guaranteed to be contiguous?
    return {str->data(), (tm_size_t)str->size()};
#else
    str->resize((size_t)new_size);
    return {str->data(), new_size};
#endif
}
static void tmp_allocator_std_string_destroy(void* /*context*/, char* /*ptr*/, tm_size_t /*size*/) {}

static tmp_allocator_context tmp_std_string_allocator(::std::string* out) {
    return {static_cast<void*>(out), tmp_allocator_std_string_reallocate, tmp_allocator_std_string_destroy};
}
#endif /* defined(TMP_USE_STL) */
/* See SWITCHES for how to redefine this. */
#ifndef TMP_STRING_WIDTH
#define TMP_STRING_WIDTH(str, str_len) (int)(str_len)
#endif

static PrintFormattedResult tmp_get_print_size(char*, tm_size_t, bool value, const PrintFormat& format) {
    // '0' or '1' are just a single byte.
    if (format.flags & PrintFlags::BoolAsNumber) return {1, TM_OK};
    // The strings "true" and "false" are 4 and 5 bytes.
    return {(tm_size_t)((value) ? 4 : 5), TM_OK};
}

template <class T>
static PrintFormattedResult tmp_get_print_size(char* dest, tm_size_t maxlen, T value, const PrintFormat& format) {
    // FIXME: This will fail for floating point numbers that need more than TMP_SBO_SIZE characters when supplied dest
    // buffer isn't big enough. Should be very rare, but might be unexpected and lead to bugs.
    // Should we allocate memory in that case?

    char buffer[(TMP_SBO_SIZE)];
    if (maxlen < (tm_size_t)(TMP_SBO_SIZE)) {
        dest = buffer;
        maxlen = (tm_size_t)(TMP_SBO_SIZE);
    }
    return tmp_print_formatted(dest, maxlen, format, value);
}

struct tmp_memory_printer {
    char* data;
    tm_size_t size = 0;
    tm_size_t necessary = 0;
    tm_size_t capacity;
    bool can_grow;
    bool owns = false;
    tm_errc ec = TM_OK;

    tmp_allocator_context allocator;

    tmp_memory_printer(char* buffer, tm_size_t capacity, tmp_allocator_context allocator)
        : data(buffer), capacity(capacity), can_grow(true), allocator(allocator) {
        TM_ASSERT(allocator.reallocate);
        TM_ASSERT(allocator.destroy);
    }
    tmp_memory_printer(char* buffer, tm_size_t capacity) : data(buffer), capacity(capacity), can_grow(false) {}
    ~tmp_memory_printer() {
        if (owns) allocator.destroy(allocator.context, data, capacity);
    }
    bool grow(tm_size_t by_at_least = 0) {
        TM_ASSERT(can_grow);
        tm_size_t new_capacity = 3 * ((capacity + 2) / 2);
        if (new_capacity < capacity + by_at_least) new_capacity = capacity + by_at_least;

        char* new_data = nullptr;
        if (owns) {
            auto reallocate_result = allocator.reallocate(allocator.context, data, capacity, new_capacity);
            new_data = reallocate_result.ptr;
            new_capacity = reallocate_result.size;
        } else {
            auto reallocate_result = allocator.reallocate(allocator.context, nullptr, 0, new_capacity);
            new_data = reallocate_result.ptr;
            new_capacity = reallocate_result.size;

            if (new_data && data && size > 0) {
                TM_MEMCPY(new_data, data, size * sizeof(char));
            }
        }
        if (!new_data) {
            ec = TM_ENOMEM;
            return false;
        }
        data = new_data;
        capacity = new_capacity;
        owns = true;
        return true;
    }
    tm_size_t remaining() {
        TM_ASSERT(size <= capacity);
        return capacity - size;
    }
    char* end() { return data + size; }

    template <class T>
    bool print_value(T value, PrintFormat& format) {
        PrintFormattedResult result = {};
        if (ec == TM_OK) {
            for (;;) {
                result = tmp_print_formatted(end(), remaining(), format, value);
                if (result.ec == TM_OK) break;
                // TM_EOVERFLOW means there wasn't enough remaining size, but we don't know how much we need.
                // TM_ERANGE means there wasn't enough remaining size, and we know exactly how much we need.
                if (result.ec == TM_EOVERFLOW || result.ec == TM_ERANGE) {
                    if (!can_grow) {
                        if (result.ec == TM_ERANGE) {
                            ec = TM_ERANGE;
                            necessary += result.size;
                            return true;
                        }
                        // We break, so we can calculate the necessary size below.
                        break;
                    }
                    if (!grow((result.ec == TM_ERANGE) ? (result.size - remaining()) : 0)) return false;
                    continue;
                }
                // Any other error code is fatal.
                ec = result.ec;
                return false;
            }
            if (result.ec == TM_OK) {
                size += result.size;
                necessary += result.size;
            } else if (result.ec == TM_EOVERFLOW) {
                ec = TM_ERANGE;
            } else {
                ec = result.ec;
            }
        }

        // Error code might have changed, check again.
        if (ec == TM_ERANGE) {
            // Calculate necessary buffer size.
            auto necessary_size = tmp_get_print_size(data, capacity, value, format);
            if (necessary_size.ec != TM_OK) {
                ec = necessary_size.ec;
                return false;
            }
            necessary += necessary_size.size;
            return true;
        }
        return result.ec == TM_OK;
    }
    bool print_string_value(const char* str, tm_size_t str_len, PrintFormat& format) {
        tm_size_t str_width = str_len;
        tm_size_t bytes_needed = str_len;
        if (format.width > 0) {
            int string_width_result = TMP_STRING_WIDTH(str, str_len);
            if (string_width_result < 0) {
                ec = TM_EINVAL;
                return false;
            }
            str_width = (tm_size_t)string_width_result;
            tm_size_t width = ((tm_size_t)format.width > str_width) ? ((tm_size_t)format.width - str_width) : 0;
            bytes_needed = str_len + width;
        }

        necessary += bytes_needed;
        if (ec == TM_ERANGE) return true;

        if (bytes_needed > remaining()) {
            if (!can_grow) {
                ec = TM_ERANGE;
                return true;
            }
            if (!grow(bytes_needed - remaining())) return false;
        }
        auto print_result = tmp_print_formatted(end(), remaining(), format, str, str_len, str_width);
        if (print_result.ec == TM_OK) {
            size += print_result.size;
        } else {
            ec = print_result.ec;
            return false;
        }
        return true;
    }
#ifdef TMP_INT_BACKEND_TM_CONVERSION
    template <class T>
    bool print_unsigned(T value, PrintFormat& format, bool negative) {
        auto sizes = tmp_get_print_sizes(value, format, negative);
        necessary += sizes.size;
        if (sizes.size > remaining() && !(can_grow && grow(sizes.size))) {
            if (!can_grow) {
                ec = TM_ERANGE;
                return true;
            }
            return ec == TM_OK;
        }
        auto result = tmp_print_formatted_unsigned(end(), remaining(), sizes, format, value, negative);
        TM_ASSERT(result.ec == TM_OK);
        size += result.size;
        return result.ec == TM_OK;
    }
    bool print_value(int32_t value, PrintFormat& format) {
        bool negative = value < 0;
        if (negative) value = -value;
        return print_unsigned((uint32_t)value, format, negative);
    }
    bool print_value(int64_t value, PrintFormat& format) {
        bool negative = value < 0;
        if (negative) value = -value;
        return print_unsigned((uint64_t)value, format, negative);
    }
    bool print_value(uint32_t value, PrintFormat& format) { return print_unsigned(value, format, false); }
    bool print_value(uint64_t value, PrintFormat& format) { return print_unsigned(value, format, false); }
#endif  // defined(TMP_INT_BACKEND_TM_CONVERSION)

    bool operator()(int type, const PrintValue& value, PrintFormat& format) {
        switch (type) {
            case PrintType::Char: {
                if (format.flags & PrintFlags::Char) {
                    ++necessary;
                    if (remaining() <= 0) {
                        if (!can_grow) {
                            ec = TM_ERANGE;
                            return true;
                        }
                        if (!grow()) return false;
                    }
                    *end() = value.c;
                    ++size;
                    return true;
                } else {
                    return print_value((int32_t)value.c, format);
                }
            }
            case PrintType::Bool: {
                return print_value(value.b, format);
            }
            case PrintType::Int32: {
                return print_value(value.i32, format);
            }
            case PrintType::UInt32: {
                return print_value(value.u32, format);
            }
            case PrintType::Int64: {
                return print_value(value.i64, format);
            }
            case PrintType::UInt64: {
                return print_value(value.u64, format);
            }
            case PrintType::Float: {
#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
                auto flags = format.flags;
                format.flags |= PrintFlags::TrailingZeroes;
#endif
                bool result = print_value(value.f, format);
#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
                format.flags = flags;
#endif
                return result;
            }
            case PrintType::Double: {
#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
                auto flags = format.flags;
                format.flags |= PrintFlags::TrailingZeroes;
#endif
                bool result = print_value(value.d, format);
#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
                format.flags = flags;
#endif
                return result;
            }
            case PrintType::String: {
                TM_ASSERT(value.s);
                return print_string_value(value.s, (tm_size_t)TM_STRLEN(value.s), format);
            }
            case PrintType::StringView: {
                return print_string_value(value.v.data, (tm_size_t)value.v.size, format);
            }
#ifdef TMP_CUSTOM_PRINTING
            case PrintType::Custom: {
                auto print_size = value.custom.customPrint(end(), remaining(), format, value.custom.data);
                if (print_size < 0) {
                    ec = TM_EINVAL;
                    return false;
                }
                if (can_grow && (tm_size_t)print_size > remaining()) {
                    if (!grow(print_size)) return false;
                    print_size = value.custom.customPrint(end(), remaining(), format, value.custom.data);
                }
                necessary += (tm_size_t)print_size;
                if ((tm_size_t)print_size <= remaining()) {
                    size += print_size;
                } else {
                    ec = TM_ERANGE;
                }
                return true;
            }
#endif
            default: {
                TM_ASSERT(0 && "invalid code path");
                return false;
            }
        }
    }
    bool operator()(const char* str) { return operator()(str, (tm_size_t)TM_STRLEN(str)); }
    bool operator()(const char* str, tm_size_t len) {
        necessary += len;
        if (len > remaining()) {
            if (!can_grow) {
                ec = TM_ERANGE;
                return true;
            }
            if (!grow(len)) return false;
        }
        auto rem = remaining();
        auto print_size = TM_MIN(len, rem);
        TM_ASSERT_VALID_SIZE(print_size);
        if (str && len > 0) {
            TM_MEMCPY(end(), str, print_size * sizeof(char));
        }
        size += print_size;
        return true;
    }
};

static const char* tmp_find(const char* first, const char* last, char c) {
    return (const char*)TM_MEMCHR(first, c, last - first);
}

static bool tmp_print_string_escaped(const char* first, const char* last, tmp_memory_printer& printout) {
    const char* p = first;
    while ((p = tmp_find(first, last, '}')) != nullptr) {
        // Print string including '}'.
        ++p;
        if (!printout(first, (tm_size_t)(p - first))) return false;

        // Check for unescaped '}'.
        if (p >= last || *p != '}') {
            printout.ec = TM_EINVAL;
            return false;
        }
        ++p;  // Skip escaped '}'.

        first = p;
    }
    if (first < last) {
        if (!printout(first, (tm_size_t)(last - first))) return false;
    }
    return true;
}

static void tmp_print_impl(const char* format, size_t format_len, const PrintFormat& initial_formatting,
                           const PrintArgList& args, tmp_memory_printer& printout) {
    // Sanitize flags.
    uint32_t format_flags = initial_formatting.flags & ((1u << PrintFlags::Count) - 1);

    const char* format_first = format;
    const char* format_last = format + format_len;
    auto index = 0u;
    const char* p = format_first;
    uint64_t currentArgTypeFlags = args.flags;
    while ((p = tmp_find(format_first, format_last, '{')) != nullptr) {
        if (!tmp_print_string_escaped(format_first, p, printout)) return;
        ++p;
        if (*p == '{') {
            if (!printout("{", 1)) return;
            ++p;
            format_first = p;
            continue;
        }

        // Parse until '}'.
        auto next = tmp_find(format_first, format_last, '}');
        if (!next) {
            printout.ec = TM_EINVAL;
            return;
        }

        PrintFormat print_format = initial_formatting;
        print_format.flags = format_flags;
        auto current_index = index;
        auto current_arg_type = currentArgTypeFlags & PrintType::Mask;
        bool index_specified = tmp_parse_format_specifiers(p, (tm_size_t)(next - p), &print_format, &current_index);
        // Check whether specified index in format specifier is valid.
        if (current_index >= args.size) {
            printout.ec = TM_EINVAL;
            return;
        }

        if (!index_specified) {
            ++index;
            currentArgTypeFlags >>= PrintType::Bits;
        } else {
            current_arg_type = (args.flags >> (current_index * PrintType::Bits)) & PrintType::Mask;
        }
        format_first = next + 1;

        // Check whether current arg type is a valid entry of PrintType.
        // This can happen if there are more unindexed '{}' placeholders than supplied arguments.
        if (current_arg_type <= 0 || current_arg_type >= PrintType::Last) {
            printout.ec = TM_EINVAL;
            return;
        }
        if (!printout((int)current_arg_type, args.args[current_index], print_format)) return;
    }
    if (format_first < format_last) {
        if (!tmp_print_string_escaped(format_first, format_last, printout)) return;
    }
}

}  // anonymous namespace

TMP_DEF tm_size_t tmp_parse_print_format(const char* format_specifiers, tm_size_t format_specifiers_len,
                                         PrintFormat* out) {
    TM_ASSERT(format_specifiers || format_specifiers_len == 0);
    TM_ASSERT(out);
    TM_ASSERT_VALID_SIZE(format_specifiers_len);

    *out = default_print_format();
    unsigned int flags = PrintFlags::Default;

    // Pad with spaces by default.
    flags |= PrintFlags::PadWithSpaces;

    const char* p = format_specifiers;
    const char* last = p + format_specifiers_len;

    bool pound_specified = false;

    for (bool parseFlags = true; p < last && parseFlags;) {
        switch (*p) {
            case '-': {
                flags |= PrintFlags::LeftJustify;
                ++p;
                break;
            }
            case '+': {
                flags |= PrintFlags::Sign;
                ++p;
                break;
            }
            case ' ': {
                flags |= PrintFlags::PadWithSpaces;
                ++p;
                break;
            }
            case '#': {
                pound_specified = true;
                ++p;
                break;
            }
            case '0': {
                flags &= ~PrintFlags::PadWithSpaces;
                ++p;
                break;
            }
            default: {
                parseFlags = false;
                break;
            }
        }
    }

    // Parse width.
    unsigned width = out->width;
    auto scan_width_result = scan_u32_n(p, (tm_size_t)(last - p), &width, 10);
    if (scan_width_result.ec == TM_OK) p += scan_width_result.size;
    out->width = (int)width;

    // Parse precision.
    if (p < last && *p == '.') {
        ++p;
        unsigned precision = out->precision;
        auto scan_precision_result = scan_u32_n(p, (tm_size_t)(last - p), &precision, 10);
        if (scan_precision_result.ec == TM_OK) p += scan_precision_result.size;
        out->precision = (int)precision;
    }

    if (p < last) {
        switch (*p) {
            case 'x': {
                flags |= PrintFlags::Lowercase;
                out->base = 16;
                if (pound_specified) flags |= PrintFlags::PrependHex;
                ++p;
                break;
            }
            case 'X': {
                out->base = 16;
                if (pound_specified) flags |= PrintFlags::PrependHex;
                ++p;
                break;
            }
            case 'o': {
                out->base = 8;
                if (pound_specified) flags |= PrintFlags::PrependOctal;
                ++p;
                break;
            }
            case 'b': {
                flags |= PrintFlags::Lowercase;
                out->base = 2;
                if (pound_specified) flags |= PrintFlags::PrependBinary;
                ++p;
                break;
            }
            case 'B': {
                out->base = 2;
                if (pound_specified) flags |= PrintFlags::PrependBinary;
                ++p;
                break;
            }
            case 'c': {
                flags |= PrintFlags::Char;
                ++p;
                break;
            }

            // floating point
            case 'e': {
                flags |= PrintFlags::Lowercase;
                flags |= PrintFlags::Scientific;
                ++p;
                break;
            }
            case 'E': {
                flags |= PrintFlags::Scientific;
                ++p;
                break;
            }
            case 'f': {
                flags |= PrintFlags::TrailingZeroes | PrintFlags::Fixed;
                ++p;
                break;
            }
            case 'g': {
                flags |= PrintFlags::Lowercase;
                flags |= PrintFlags::General;
                flags |= PrintFlags::Shortest;
                ++p;
                break;
            }
            case 'G': {
                flags |= PrintFlags::General;
                flags |= PrintFlags::Shortest;
                ++p;
                break;
            }
            case 'a': {
                flags |= PrintFlags::Lowercase;
                flags |= PrintFlags::Hex;
                if (pound_specified) flags |= PrintFlags::PrependHex;
                ++p;
                break;
            }
            case 'A': {
                flags |= PrintFlags::Hex;
                if (pound_specified) flags |= PrintFlags::PrependHex;
                ++p;
                break;
            }

            // Non printf.
            case 'n': {
                flags |= PrintFlags::BoolAsNumber;
                ++p;
                break;
            }
        }
    }

    out->flags = flags;
    return (tm_size_t)(p - format_specifiers);
}

#ifndef TMP_NO_CRT_FILE_PRINTING
TMP_DEF tm_errc tmp_print(FILE* out, const char* format, size_t format_len, const PrintFormat& initial_formatting,
                          const PrintArgList& args) {
    char sbo[TMP_SBO_SIZE];
    tmp_memory_printer printer = {sbo, TMP_SBO_SIZE, tmp_default_allocator()};

    tmp_print_impl(format, format_len, initial_formatting, args, printer);
    fwrite(printer.data, sizeof(char), printer.size, out);
    return printer.ec;
}
#endif  // !defined(TMP_NO_CRT_FILE_PRINTING)

TMP_DEF int tmp_snprint(char* dest, tm_size_t len, const char* format, size_t format_len,
                        const PrintFormat& initial_formatting, const PrintArgList& args) {
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(dest || len == 0);
    tmp_memory_printer mem{dest, len};
    tmp_print_impl(format, format_len, initial_formatting, args, mem);
    if (mem.ec == TM_OK) {
        // Force null termination, overwriting last character if must be.
        // This is also the behavior of snprintf, we want to imitate it to minimize surprises.
        if (mem.remaining()) {
            *mem.end() = 0;
        } else {
            if (dest && len > 0) dest[len - 1] = 0;
        }
        return (int)mem.size;
    }
    if (mem.ec == TM_ERANGE) return (int)mem.necessary;
    return -1;
}

#ifdef TMP_USE_STL
TMP_DEF ::std::string tmp_string_format(const char* format, size_t format_len, const PrintFormat& initial_formatting,
                                        const PrintArgList& args) {
    ::std::string result;
    tmp_memory_printer mem{nullptr, 0, tmp_std_string_allocator(&result)};
    tmp_print_impl(format, format_len, initial_formatting, args, mem);
    if (mem.ec == TM_OK) {
        // Resize resulting string to the actual size consumed by memory printer.
        // Memory printer will allocate in advance and then might not use all of the capacity.
        result.resize(mem.size);
    } else {
        result.clear();
    }
    return result;
}
#endif /* defined(TMP_USE_STL) */

}  // namespace tml
#endif  // TM_PRINT_IMPLEMENTATION

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2016 Tolga Mizrak

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
