/*
tm_print.h v0.0.8 - public domain - https://github.com/to-miz/tm
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
    - The tm_conversion based implementation always outputs '.' as the decimal point character
    regardless of the locale, while the snprintf based output outputs the decimal point based on the
    current locale.

HISTORY
    v0.0.8  06.10.18 refactored some common macro blocks into include files
    v0.0.7  02.10.18 refactored into multiple files that get merged
                     added multiple backends for int and float printing
                     added a couple more formatting options like o, a and c
    v0.0.6  25.09.18 reworked many printing functions because of breaking changes to tm_conversion
    v0.0.5  01.09.18 added MIT license option
                     refactored fillPrintArgList to not be dependend on preprocessor switches
                     removed fprinter
                     tmp_memory_printer can now grow and can use small buffer optimization
                     removed dependency on fprintf
    v0.0.4e 25.08.18 added repository link
    v0.0.4d 10.01.17 minor change from static const char* to static const char* const in print_bool
    v0.0.4c 23.10.16 added some assertions for bounds checking
    v0.0.4b 07.10.16 fixed some casting issues when tm_size_t is signed
    v0.0.4a 29.09.16 fixed a bug where inputting an escaped {{ was resulting in an infinite loop
    v0.0.4  29.09.16 added signed/unsigned char, short and long handling
                     fixed compiler errors when compiling with clang
    v0.0.3  27.09.16 added printing custom types by overloading snprint
                     added initialFormatting parameter to snprint so that custom printing can
                     inherit formatting options
    v0.0.2  26.09.16 changed makeFlags to tmp_type_flags so that it is guaranteed to be a
                     compile time constant
                     added string view overloads so that print can accept a string view as the
                     format parameter
                     fixed some compiler warnings when tm_size_t is defined as int
    v0.0.1  24.09.16 initial commit
*/

#include "../common/GENERATED_WARNING.inc"

// clang-format off
#include "dependencies_implementation.h"

#ifndef _TM_PRINT_H_INCLUDED_
#define _TM_PRINT_H_INCLUDED_

#define TMP_VERSION 0x00000008u

#include "dependencies_header.h"

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

    Default = 0,

    Count = 9,
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
    PrintValue args[PrintType::Count];
    uint64_t flags;
    unsigned int size;
};

// define TMP_NO_CRT_FILE_PRINTING if you don't need printing to stdout or to FILE* handles
// clang-format off
#ifndef TMP_NO_CRT_FILE_PRINTING
    TMP_DEF tm_errc tmp_print(FILE* out, const char* format, const PrintArgList& args);
    #ifdef TM_STRING_VIEW
        TMP_DEF tm_errc tmp_print(FILE* out, TM_STRING_VIEW format, const PrintArgList& args);
    #endif  // TM_STRING_VIEW
#endif  // TMP_NO_STDIO
// clang-format on

TMP_DEF tm_size_t tmp_snprint(char* dest, tm_size_t len, const char* format, const PrintFormat& initialFormatting,
                              const PrintArgList& args);
#ifdef TM_STRING_VIEW
TMP_DEF tm_size_t tmp_snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initialFormatting,
                              const PrintArgList& args);
#endif

#include "variadic_machinery.h"

#endif  // _TM_PRINT_H_INCLUDED_

#ifdef TM_PRINT_IMPLEMENTATION
#include "implementation.cpp"
#endif // TM_PRINT_IMPLEMENTATION

#include "../common/LICENSE.inc"