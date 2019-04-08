/*
tm_print.h v0.0.14 - public domain - https://github.com/to-miz/tm
author: Tolga Mizrak 2016

no warranty; use at your own risk

LICENSE
    see license notes at end of file

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_PRINT_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

ISSUES
    - The tm_conversion/charconv based implementation always outputs '.' as the decimal point character
      regardless of the locale, while the snprintf based output outputs the decimal point based on the
      current locale.
    - snprint functions do not return the required buffer length if supplied buffer isn't large enough.
      This is in contrast to how snprintf works. Currenlty return value is -1 if buffer isn't large enough.

HISTORY
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
                     Added initialFormatting parameter to snprint so that custom printing can.
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

#ifndef _TM_PRINT_H_INCLUDED_
#define _TM_PRINT_H_INCLUDED_

#define TMP_VERSION 0x0000000Du

#include "dependencies_header.h"

// default print flags value
#ifndef TMP_DEFAULT_FLAGS
    #define TMP_DEFAULT_FLAGS 0u
#endif

// clang-format on

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
    // PoundSpecified = (1u << 15u),
    // WidthSpecified = (1u << 16u),
    // PrecisionSpecified = (1u << 17u),

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

inline PrintFormat defaultPrintFormat() { return PrintFormat{10, 6, 0, PrintFlags::Default}; }

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

    Last,  // only here to check against Count to see if they are the same (see static_asserts
           // below)

    // count needs to be power of two so that Mask and Bits is valid
    Count = 16,
    Mask = Count - 1,
    Bits = 4
};
}

#ifndef TMP_STATIC
#define TMP_DEF extern
#else
#define TMP_DEF static
#endif

// sanity checks
static_assert(PrintType::Last <= PrintType::Count, "Values added to PrintType without adjusting Count and Bits");
static_assert((PrintType::Count & (PrintType::Count - 1)) == 0, "Count must be power of two");
static_assert((sizeof(uint64_t) * 8) / PrintType::Bits >= PrintType::Count,
              "Can't store PrintType::Count in a uint64_t");

#ifdef TMP_CUSTOM_PRINTING
typedef tm_size_t tmp_custom_printer_type(char* buffer, tm_size_t len, const PrintFormat& initialFormatting,
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

// define TMP_NO_CRT_FILE_PRINTING if you don't need printing to stdout or to FILE* handles
// clang-format off
#ifndef TMP_NO_CRT_FILE_PRINTING
    TMP_DEF tm_errc tmp_print(FILE* out, const char* format, const PrintFormat& initialFormatting,
                              const PrintArgList& args);
    #ifdef TM_STRING_VIEW
        TMP_DEF tm_errc tmp_print(FILE* out, TM_STRING_VIEW format, const PrintFormat& initialFormatting,
                                  const PrintArgList& args);
    #endif  // TM_STRING_VIEW
#endif  // TMP_NO_CRT_FILE_PRINTING
// clang-format on

TMP_DEF int tmp_snprint(char* dest, tm_size_t len, const char* format, const PrintFormat& initialFormatting,
                        const PrintArgList& args);
#ifdef TM_STRING_VIEW
TMP_DEF int tmp_snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initialFormatting,
                        const PrintArgList& args);
#endif

TMP_DEF tm_size_t tmp_parse_print_format(const char* format_specifiers, tm_size_t format_specifiers_len,
                                         PrintFormat* out);

#include "variadic_machinery.h"

#endif  // _TM_PRINT_H_INCLUDED_

#ifdef TM_PRINT_IMPLEMENTATION
#include "implementation.cpp"
#endif  // TM_PRINT_IMPLEMENTATION

#include "../common/LICENSE.inc"