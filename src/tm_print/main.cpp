/*
tm_print.h v0.0.18 - public domain - https://github.com/to-miz/tm
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

HISTORY
    v0.0.18 13.04.19 Fixed gcc/clang compilation warnings.
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

#include "../common/GENERATED_WARNING.inc"

// clang-format off
#include "dependencies_implementation.h"

#ifndef _TM_PRINT_H_INCLUDED_14E73C89_58CA_4CC4_9D19_99F0A3D7EA07_
#define _TM_PRINT_H_INCLUDED_14E73C89_58CA_4CC4_9D19_99F0A3D7EA07_

#define TMP_VERSION 0x00000012u

#include "dependencies_header.h"

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

#include "variadic_machinery.h"

}  // namespace tml

#endif  // _TM_PRINT_H_INCLUDED_14E73C89_58CA_4CC4_9D19_99F0A3D7EA07_

#ifdef TM_PRINT_IMPLEMENTATION
#include "implementation.cpp"
#endif  // TM_PRINT_IMPLEMENTATION

#include "../common/LICENSE.inc"