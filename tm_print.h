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
                     fixed compilation error when TM_STRING_VIEW is defined
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

    /* cstdlib dependency */
    #if !defined(TMP_MALLOC) || !defined(TMP_REALLOC) || !defined(TMP_FREE)
        // either all or none have to be defined
        #include <cstdlib>
        #define TMP_MALLOC(type, size) (type*)malloc(size)
        #define TMP_FREE(ptr) free(ptr)
        #define TMP_REALLOC(type, ptr, size) (type*)realloc(ptr, size)
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

    // the buffer size used for small buffer optimization, change this by profiling if malloc is a bottleneck
    #ifndef TMP_SBO_SIZE
        #define TMP_SBO_SIZE 200
    #endif  // !defined( TMP_SBO_SIZE )
#endif


#ifndef _TM_PRINT_H_INCLUDED_
#define _TM_PRINT_H_INCLUDED_

#define TMP_VERSION 0x00000008u

/* assert */
#ifndef TM_ASSERT
    #include <assert.h>
    #define TM_ASSERT assert
#endif /* !defined(TM_ASSERT) */

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

#include <type_traits>

/* Linkage defaults to extern, to override define TMC_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMC_DEF
    #define TMC_DEF extern
#endif

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
    #include <stddef.h> /* include C version so identifiers are in global namespace */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
    #define TM_ERRC_DEFINED
    enum TM_ERRC_CODES {
        TM_OK        = 0,   /* same as std::errc() */
        TM_EOVERFLOW = 75,  /* same as std::errc::value_too_large */
        TM_ERANGE    = 34,  /* same as std::errc::result_out_of_range */
        TM_EINVAL    = 22,  /* same as std::errc::invalid_argument */
        TM_ENOMEM    = 12,  /* same as std::errc::not_enough_memory */
    };
    typedef int tm_errc;
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

template <class... Types>
struct tmp_type_flags;

template <>
struct tmp_type_flags<> {
    enum : uint64_t { Value = 0 };
};

template <class... Types>
struct tmp_type_flags<char, Types...> {
    enum : uint64_t { Value = PrintType::Char | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};

// integer types to enum mapping based on their size
template <size_t N>
struct tmp_int_size;

template <>
struct tmp_int_size<1> {
    enum : uint64_t { Value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<2> {
    enum : uint64_t { Value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<4> {
    enum : uint64_t { Value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<8> {
    enum : uint64_t { Value = PrintType::Int64 };
    typedef int64_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.i64 = v;
        return result;
    }
};
template <size_t N>
struct tmp_uint_size;
template <>
struct tmp_uint_size<1> {
    enum : uint64_t { Value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<2> {
    enum : uint64_t { Value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<4> {
    enum : uint64_t { Value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<8> {
    enum : uint64_t { Value = PrintType::UInt64 };
    typedef uint64_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.u64 = v;
        return result;
    }
};

template <class... Types>
struct tmp_type_flags<signed char, Types...> {
    enum : uint64_t {
        Value = tmp_int_size<sizeof(signed char)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned char, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned char)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<short, Types...> {
    enum : uint64_t {
        Value = tmp_int_size<sizeof(short)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned short, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned short)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<long, Types...> {
    enum : uint64_t {
        Value = tmp_int_size<sizeof(long)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned long, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned long)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<int, Types...> {
    enum : uint64_t { Value = tmp_int_size<sizeof(int)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<unsigned int, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned int)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<long long, Types...> {
    enum : uint64_t {
        Value = tmp_int_size<sizeof(long long)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned long long, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned long long)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};

template <class... Types>
struct tmp_type_flags<bool, Types...> {
    enum : uint64_t { Value = PrintType::Bool | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<float, Types...> {
    enum : uint64_t { Value = PrintType::Float | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<double, Types...> {
    enum : uint64_t { Value = PrintType::Double | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<const char*, Types...> {
    enum : uint64_t { Value = PrintType::String | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
#ifdef TM_STRING_VIEW
template <class... Types>
struct tmp_type_flags<TM_STRING_VIEW, Types...> {
    enum : uint64_t { Value = PrintType::StringView | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
#endif
#ifdef TMP_CUSTOM_PRINTING
template <class T, class... Types>
struct tmp_type_flags<T, Types...> {
    enum : uint64_t { Value = PrintType::Custom | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
#else
template <class T, class... Types>
struct tmp_type_flags<T, Types...> {
    enum : uint64_t { Value = 0 };
};
#endif

template <class... Types>
void fillPrintArgList(PrintArgList* list, char value, const Types&... args) {
    list->args[list->size++].c = value;
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, signed char value, const Types&... args) {
    typedef tmp_int_size<sizeof(signed char)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned char value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned char)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, short value, const Types&... args) {
    typedef tmp_int_size<sizeof(short)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned short value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned short)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, long value, const Types&... args) {
    typedef tmp_int_size<sizeof(long)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned long value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned long)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, bool value, const Types&... args) {
    list->args[list->size++].b = value;
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, int value, const Types&... args) {
    typedef tmp_int_size<sizeof(int)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned int value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned int)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, long long value, const Types&... args) {
    typedef tmp_int_size<sizeof(long long)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned long long value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned long long)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, float value, const Types&... args) {
    list->args[list->size++].f = value;
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, double value, const Types&... args) {
    list->args[list->size++].d = value;
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, const char* value, const Types&... args) {
    list->args[list->size++].s = value;
    fillPrintArgList(list, args...);
}
#ifdef TM_STRING_VIEW
template <class... Types>
void fillPrintArgList(PrintArgList* list, TM_STRING_VIEW value, const Types&... args) {
    list->args[list->size].v.data = TM_STRING_VIEW_DATA(value);
    list->args[list->size].v.size = TM_STRING_VIEW_SIZE(value);
    ++list->size;
    fillPrintArgList(list, args...);
}
#endif
#ifdef TMP_CUSTOM_PRINTING
// this looks very confusing, but it checks for the existence of a specific overload of snprint
// this way we can do a static_assert on whether the overload exists and report an error
// otherwise
template <class T>
class tmp_has_custom_printer {
    typedef tm_size_t printer_t(char*, tm_size_t, const PrintFormat&, const T&);
    typedef char no;

    template <class C>
    static auto test(C c)
        -> decltype(static_cast<tm_size_t (*)(char*, tm_size_t, const PrintFormat&, const C&)>(&snprint));
    template <class C>
    static no test(...);

   public:
    enum { Value = (sizeof(test<T>(T{})) == sizeof(void*)) };
};

template <class T, class... Types>
void fillPrintArgList(PrintArgList* list, const T& value, const Types&... args) {
    static_assert(tmp_has_custom_printer<T>::Value,
                  "T is not printable, there is no snprint that takes value of type T");
    // if the static assert fails, the compiler will also report that there are no overloads of
    // snprint that accept the argument types. We could get rid of that error by using SFINAE
    // but that introduces more boilerplate

    // having constexpr if simplifies the error message
#ifdef TMP_HAS_CONSTEXPR_IF
    if constexpr (tmp_has_custom_printer<T>::Value)
#endif
    {
        auto custom = &list->args[list->size++].custom;
        custom->data = &value;
        custom->customPrint = [](char* buffer, tm_size_t len, const PrintFormat& initialFormatting, const void* data) {
            return snprint(buffer, len, initialFormatting, *(const T*)data);
        };
    }
    fillPrintArgList(list, args...);
}
#else
template <class T, class... Types>
void fillPrintArgList(PrintArgList* list, const T& value, const Types&... args) {
    static_assert(tmp_type_flags<T>::Value != 0,
                  "T is not printable, custom printing is disabled (TMP_CUSTOM_PRINTING not defined)");
    static_assert(tmp_type_flags<T>::Value == 0, "");  // this function is not allowed to be instantiated
}
#endif
void fillPrintArgList(PrintArgList*) {}

template <class... Types>
void makePrintArgList(PrintArgList* list, const Types&... args) {
    list->flags = tmp_type_flags<Types...>::Value;
    list->size = 0;
    fillPrintArgList(list, args...);
}

#ifndef TMP_NO_STDIO
template <class... Types>
tm_errc print(const char* format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print");
    PrintArgList argList;
    makePrintArgList(&argList, args...);
    return tmp_print(stdout, format, argList);
}
template <class... Types>
tm_errc print(FILE* out, const char* format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print");
    PrintArgList argList;
    makePrintArgList(&argList, args...);
    return tmp_print(out, format, argList);
}
// impl

#ifdef TM_STRING_VIEW
template <class... Types>
tm_errc print(TM_STRING_VIEW format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print");
    PrintArgList argList;
    makePrintArgList(&argList, args...);
    return tmp_print(stdout, format, argList);
}
template <class... Types>
tm_errc print(FILE* out, TM_STRING_VIEW format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print");
    PrintArgList argList;
    makePrintArgList(&argList, args...);
    return tmp_print(out, format, argList);
}
#endif  // defined( TM_STRING_VIEW )
#endif  // TMP_NO_STDIO

template <class... Types>
tm_size_t snprint(char* dest, tm_size_t len, const char* format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to snprint");
    PrintArgList argList;
    makePrintArgList(&argList, args...);
    return tmp_snprint(dest, len, format, defaultPrintFormat(), argList);
}
template <class... Types>
tm_size_t snprint(char* dest, tm_size_t len, const char* format, const PrintFormat& initialFormatting,
                  const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to snprint");
    PrintArgList argList;
    makePrintArgList(&argList, args...);
    return tmp_snprint(dest, len, format, initialFormatting, argList);
}
#ifdef TM_STRING_VIEW
template <class... Types>
tm_size_t snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to snprint");
    PrintArgList argList;
    makePrintArgList(&argList, args...);
    return tmp_snprint(dest, len, format, defaultPrintFormat(), argList);
}
template <class... Types>
tm_size_t snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initialFormatting,
                  const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to snprint");
    PrintArgList argList;
    makePrintArgList(&argList, args...);
    return tmp_snprint(dest, len, format, initialFormatting, argList);
}
#endif  // defined( TM_STRING_VIEW )

#endif  // _TM_PRINT_H_INCLUDED_

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

namespace {

struct PrintSizes {
    tm_size_t digits;
    tm_size_t decorated;
    tm_size_t size;
};

#if !defined(TMP_TM_CONVERSION_INCLUDED)
    struct PrintFormattedResult {
        tm_size_t size;
        tm_errc ec;
    };

    static PrintFormattedResult scan_u32_n(const char* str, tm_size_t maxlen, uint32_t* out, int32_t base) {
        TM_ASSERT(out);
        TM_ASSERT(base >= 2 && base <= 36);
        TM_ASSERT_VALID_SIZE(maxlen);

        PrintFormattedResult result = {maxlen, TM_EOVERFLOW};
        if (maxlen <= 0) {
            return result;
        }
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
    typedef tmc_conv_result PrintFormattedResult;

    template <class T>
    PrintSizes tmp_get_print_sizes(T value, const PrintFormat& format, bool negative);
#endif
// clang-format on

namespace FormatSpecifierFlags {
enum Values : unsigned int {
    LeftJustify = (1u << 8u),
    PoundSpecified = (1u << 9u),
    PrependHex = (1u << 10u),
    PrependBinary = (1u << 11u),
    PrependOctal = (1u << 12u),
    EmitDecimalPoint = (1u << 13u),
    IndexSpecified = (1u << 14u),
    WidthSpecified = (1u << 15u),
    PrecisionSpecified = (1u << 16u),
    PadWithSpaces = (1u << 17u),
};
}

static unsigned int tmp_parse_format_specifiers(const char* p, tm_size_t len, PrintFormat* format,
                                                uint32_t* currentIndex) {
    unsigned int result = 0;
    if (len != 0) {
        auto end = p + len;
        // parse what is inside {}

        auto scan_index_result = scan_u32_n(p, (tm_size_t)(end - p), currentIndex, 10);
        if (scan_index_result.ec == TM_OK) {
            p += scan_index_result.size;
            result |= FormatSpecifierFlags::IndexSpecified;
        }
        if (*p == ':') {
            ++p;
            // custom formatting was defined, so we reject the initial formatting
            *format = defaultPrintFormat();
        }
        // parse flags
        {
            // pad with spaces by default
            format->flags |= FormatSpecifierFlags::PadWithSpaces;

            bool parseFlags = true;
            do {
                switch (*p) {
                    case '-': {
                        result |= FormatSpecifierFlags::LeftJustify;
                        ++p;
                        break;
                    }
                    case '+': {
                        format->flags |= PrintFlags::Sign;
                        ++p;
                        break;
                    }
                    case ' ': {
                        format->flags |= FormatSpecifierFlags::PadWithSpaces;
                        ++p;
                        break;
                    }
                    case '#': {
                        result |= FormatSpecifierFlags::PoundSpecified;
                        ++p;
                        break;
                    }
                    case '0': {
                        format->flags &= ~FormatSpecifierFlags::PadWithSpaces;
                        ++p;
                        break;
                    }
                    default: {
                        parseFlags = false;
                        break;
                    }
                }
            } while (parseFlags);
        }

        // parse width
        unsigned width = 0;
        auto scan_width_result = scan_u32_n(p, (tm_size_t)(end - p), &width, 10);
        if (scan_width_result.ec == TM_OK) {
            p += scan_width_result.size;
            result |= FormatSpecifierFlags::WidthSpecified;
        }
        format->width = (int)width;

        // parse precision
        if (*p == '.') {
            ++p;
            unsigned precision = 0;
            auto scan_precision_result = scan_u32_n(p, (tm_size_t)(end - p), &precision, 10);
            if (scan_precision_result.ec == TM_OK) {
                p += scan_precision_result.size;
                result |= FormatSpecifierFlags::PrecisionSpecified;
            }
            format->precision = (int)precision;
        }

        switch (*p) {
            case 'x': {
                format->flags |= PrintFlags::Lowercase;
                format->base = 16;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependHex;
                }
                break;
            }
            case 'X': {
                format->base = 16;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependHex;
                }
                break;
            }
            case 'o': {
                format->base = 8;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependOctal;
                }
                break;
            }
            case 'b': {
                format->flags |= PrintFlags::Lowercase;
                format->base = 2;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependBinary;
                }
                break;
            }
            case 'B': {
                format->base = 2;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependBinary;
                }
                break;
            }
            case 'c': {
                format->flags |= PrintFlags::Char;
                break;
            }

            // floating point
            case 'e': {
                format->flags |= PrintFlags::Lowercase;
                format->flags |= PrintFlags::Scientific;
                break;
            }
            case 'E': {
                format->flags |= PrintFlags::Scientific;
                break;
            }
            case 'f': {
                format->flags |= PrintFlags::TrailingZeroes | PrintFlags::Fixed;
                break;
            }
            case 'g': {
                format->flags |= PrintFlags::Lowercase;
                format->flags |= PrintFlags::General;
                format->flags |= PrintFlags::Shortest;
                break;
            }
            case 'G': {
                format->flags |= PrintFlags::General;
                format->flags |= PrintFlags::Shortest;
                break;
            }
            case 'a': {
                format->flags |= PrintFlags::Lowercase;
                format->flags |= PrintFlags::Hex;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependHex;
                }
                break;
            }
            case 'A': {
                format->flags |= PrintFlags::Hex;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependHex;
                }
                break;
            }

            // non printf
            case 'n': {
                result |= PrintFlags::BoolAsNumber;
                break;
            }
        }
    }
    return result;
}

// TODO: this could be done better with dependency to type_traits
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

static PrintFormattedResult print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format, const char* str,
                                            tm_size_t str_len) {
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT_VALID_SIZE(str_len);

    const auto width = format.width;
    tm_size_t size = (width > 0 && (tm_size_t)width > str_len) ? width : str_len;
    if (size > maxlen) {
        return {maxlen, TM_EOVERFLOW};
    }

    auto remaining = maxlen;

    if (width > 0 && (tm_size_t)width > str_len) {
        tm_size_t padding = (tm_size_t)width - str_len;
        if (!(format.flags & FormatSpecifierFlags::LeftJustify)) {
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

static PrintFormattedResult print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format, bool value) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    auto flags = format.flags;
    if (flags & PrintFlags::BoolAsNumber) {
        return print_formatted(dest, maxlen, format, (value) ? "1" : "0", 1);
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
    return print_formatted(dest, maxlen, format, str, str_len);
}

static tm_size_t tmp_get_decorated_size(tm_size_t digits, const PrintFormat& format, bool negative) {
    tm_size_t result = digits;
    if (negative || (format.flags & PrintFlags::Sign)) {
        ++result;
    }
    if ((format.flags & FormatSpecifierFlags::PrependHex) || (format.flags & FormatSpecifierFlags::PrependBinary)) {
        result += 2;
    }
    if (format.flags & FormatSpecifierFlags::PrependOctal) {
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
        if (!(flags & FormatSpecifierFlags::LeftJustify)) {
            auto padding = (tm_size_t)format.width - sizes.decorated;
            TM_ASSERT(padding <= maxlen);
            if (flags & FormatSpecifierFlags::PadWithSpaces) {
                decorate_pos = dest + padding;
            } else {
                TM_ASSERT(sizes.decorated >= sizes.digits);
                dest += sizes.decorated - sizes.digits;
            }
            TM_MEMSET(dest, (flags & FormatSpecifierFlags::PadWithSpaces) ? ' ' : '0', padding);
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

    if (flags & FormatSpecifierFlags::PrependHex) {
        *decorate_pos++ = '0';
        *decorate_pos++ = (flags & PrintFlags::Lowercase) ? 'x' : 'X';
        remaining -= 2;
    } else if (flags & FormatSpecifierFlags::PrependBinary) {
        *decorate_pos++ = '0';
        *decorate_pos++ = (flags & PrintFlags::Lowercase) ? 'b' : 'B';
        remaining -= 2;
    } else if (flags & FormatSpecifierFlags::PrependOctal) {
        *decorate_pos++ = '0';
        --remaining;
    }
}

static PrintFormattedResult tmp_move_printed_value_and_decorate(char* dest, tm_size_t maxlen, const PrintFormat& format,
                                                                PrintFormattedResult printResult, bool negative) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    if (printResult.ec != TM_OK) return printResult;

    PrintSizes sizes = {};
    sizes.digits = printResult.size;
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
    if (width <= 0 || (format.flags & FormatSpecifierFlags::LeftJustify)) {
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

    tmp_print_decoration(dest, maxlen, sizes, format, negative);
    printResult.size = sizes.size;
    return printResult;
}

template <class T>
static PrintFormattedResult print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format, T value) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    if (maxlen <= 0 || (format.width > 0 && (tm_size_t)format.width > maxlen)) {
        return {maxlen, TM_EOVERFLOW};
    }

    auto pair = tmp_make_unsigned(value);
    bool sign = pair.negative || ((format.flags & PrintFlags::Sign) != 0);
    auto result = tmp_print(dest + sign, maxlen - sign, pair.value, format);
    return tmp_move_printed_value_and_decorate(dest, maxlen, format, result, pair.negative);
}

#ifdef TMP_INT_BACKEND_CRT
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, uint32_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    PrintFormattedResult result = {maxlen, TM_EOVERFLOW};
    const char* format_string = "%" PRIu32;
    if (format.base == 16) {
        format_string = (format.flags & PrintFlags::Lowercase) ? "%" PRIx32 : "%" PRIX32;
    } else if (format.base == 8) {
        format_string = "%" PRIo32;
    }
    auto size = TMP_SNPRINTF(dest, (size_t)maxlen, format_string, value);
    if (size > 0 && (tm_size_t)size <= maxlen) {
        result.size = (tm_size_t)size;
        result.ec = TM_OK;
    }
    return result;
}
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, uint64_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    PrintFormattedResult result = {};
    const char* format_string = "%" PRIu64;
    if (format.base == 16) {
        format_string = (format.flags & PrintFlags::Lowercase) ? "%" PRIx64 : "%" PRIX64;
    } else if (format.base == 8) {
        format_string = "%" PRIo64;
    }
    auto size = TMP_SNPRINTF(dest, (size_t)maxlen, format_string, value);
    if (size > 0 && (tm_size_t)size <= maxlen) {
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

static PrintFormattedResult tmp_print_decimal_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value) {
    return print_decimal_u32_w(dest, maxlen, width, value);
}
static PrintFormattedResult tmp_print_decimal_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value) {
    return print_decimal_u64_w(dest, maxlen, width, value);
}
static PrintFormattedResult tmp_print_hex_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value,
                                            bool lowercase) {
    return print_hex_u32_w(dest, maxlen, width, value, lowercase);
}
static PrintFormattedResult tmp_print_hex_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value,
                                            bool lowercase) {
    return print_hex_u64_w(dest, maxlen, width, value, lowercase);
}
static PrintFormattedResult tmp_print_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value, int32_t base,
                                        bool lowercase) {
    return print_u32_w(dest, maxlen, width, value, base, lowercase);
}
static PrintFormattedResult tmp_print_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value, int32_t base,
                                        bool lowercase) {
    return print_u64_w(dest, maxlen, width, value, base, lowercase);
}

template <class T>
static PrintFormattedResult print_formatted_unsigned(char* dest, tm_size_t maxlen, const PrintSizes& sizes,
                                                     const PrintFormat& format, T value, bool negative) {
    PrintFormattedResult result = {0, TM_OK};
    if (sizes.size > maxlen) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }
    result.size = sizes.size;

    tmp_print_decoration(dest, maxlen, sizes, format, negative);

    tm_size_t padding = 0;
    auto flags = format.flags;
    if (flags & FormatSpecifierFlags::LeftJustify) {
        padding = sizes.decorated - sizes.digits;
    } else {
        padding = sizes.size - sizes.digits;
    }
    dest += padding;
    auto remaining = maxlen - padding;
    PrintFormattedResult print_result = {};
    switch (format.base) {
        case 10: {
            print_result = tmp_print_decimal_w(dest, remaining, sizes.digits, value);
            break;
        }
        case 16: {
            bool lowercase = (flags & PrintFlags::Lowercase) != 0;
            print_result = tmp_print_hex_w(dest, remaining, sizes.digits, value, lowercase);
            break;
        }
        default: {
            bool lowercase = (flags & PrintFlags::Lowercase) != 0;
            print_result = tmp_print_w(dest, remaining, sizes.digits, value, format.base, lowercase);
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
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, uint32_t value, const PrintFormat& format) {
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
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, uint64_t value, const PrintFormat& format) {
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
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
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
    if (size > 0 && (tm_size_t)size <= maxlen) {
        if (format.flags & PrintFlags::Hex) {
            // remove 0x prefix to make output same as other backends
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
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
    return tmp_print(dest, maxlen, (double)value, format);
}
#endif  // defined(TMP_FLOAT_BACKEND_CRT)

#ifdef TMP_FLOAT_BACKEND_TM_CONVERSION
static uint32_t tmp_convert_flags(uint32_t flags) {
    // assuming that flags are 1 to 1 compatible with the ones in tm_conversion.h
    return flags & ((1u << PF_COUNT) - 1);  // mask out flags not defined in tm_conversion
}

static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
    return print_double(dest, maxlen, value, tmp_convert_flags(format.flags), format.precision);
}
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
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

static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
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
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
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

struct tmp_memory_printer {
    char* data;
    tm_size_t size;
    tm_size_t capacity;
    bool allowResize;
    bool owns;
    tm_errc ec = TM_OK;

    ~tmp_memory_printer() {
        if (owns) {
            TMP_FREE(data);
        }
    }
    bool grow(tm_size_t byAtLeast = 0) {
        TM_ASSERT(allowResize);
        tm_size_t newCapacity = 3 * ((capacity + 2) / 2);
        if (newCapacity < capacity + byAtLeast) {
            newCapacity = capacity + byAtLeast;
        }
        if (owns) {
            data = TMP_REALLOC(char, data, newCapacity);
            TM_ASSERT(data);
            if (data) {
                capacity = newCapacity;
                return true;
            }
        } else {
            char* newData = TMP_MALLOC(char, newCapacity);
            TM_ASSERT(newData);
            if (newData) {
                TM_MEMCPY(newData, data, size);
                data = newData;
                capacity = newCapacity;
                owns = true;
                return true;
            }
        }
        ec = TM_ENOMEM;
        return false;
    }
    tm_size_t remaining() {
        TM_ASSERT(size <= capacity);
        return capacity - size;
    }
    char* end() { return data + size; }

    template <class T>
    bool print_value(T value, PrintFormat& format) {
        PrintFormattedResult result = {};
        do {
            result = print_formatted(end(), remaining(), format, value);
        } while (allowResize && result.ec == TM_EOVERFLOW && grow());
        if (result.ec == TM_OK)
            size += result.size;
        else
            ec = result.ec;
        return result.ec == TM_OK;
    }
    bool print_string_value(const char* str, tm_size_t str_len, PrintFormat& format) {
        tm_size_t len = (format.width > 0 && (tm_size_t)format.width > str_len) ? ((tm_size_t)format.width) : (str_len);

        bool result = true;
        if (len > remaining()) {
            result = grow(len);
        }
        if (result) {
            auto print_result = print_formatted(end(), remaining(), format, str, str_len);
            if (print_result.ec == TM_OK) {
                size += print_result.size;
            } else {
                result = false;
            }
        }
        return result;
    }
#ifdef TMP_INT_BACKEND_TM_CONVERSION
    template <class T>
    bool print_unsigned(T value, PrintFormat& format, bool negative) {
        auto sizes = tmp_get_print_sizes(value, format, negative);
        if (sizes.size > remaining() && !(allowResize && grow(sizes.size))) {
            if (!allowResize) ec = TM_EOVERFLOW;
            return false;
        }
        auto result = print_formatted_unsigned(end(), remaining(), sizes, format, value, negative);
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
                    if (remaining() || (allowResize && grow())) {
                        *end() = value.c;
                        ++size;
                        return true;
                    }
                    return false;
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
                bool result = true;
                auto print_size = value.custom.customPrint(end(), remaining(), format, value.custom.data);
                if (allowResize && print_size >= remaining()) {
                    result = grow();
                    if(result) {
                        print_size = value.custom.customPrint(end(), remaining(), format, value.custom.data);
                    }
                }
                if (print_size <= remaining()) {
                    size += print_size;
                }
                return result;
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
        bool result = true;
        if (len > remaining()) {
            result = grow(len);
        }
        auto rem = remaining();
        auto printSize = TM_MIN(len, rem);
        TM_ASSERT_VALID_SIZE(printSize);
        TM_MEMCPY(end(), str, printSize);
        size += printSize;

        return result;
    }
};

static const char* tmp_find(const char* first, const char* last, char c) {
    return (const char*)TM_MEMCHR(first, c, last - first);
}

static void tmp_print_impl(const char* format, size_t formatLen, const PrintFormat& initialFormatting,
                           const PrintArgList& args, tmp_memory_printer& printout) {
    // sanitize flags
    uint32_t format_flags = initialFormatting.flags & ((1u << PrintFlags::Count) - 1);

    const char* formatFirst = format;
    const char* formatLast = format + formatLen;
    auto index = 0u;
    const char* p = formatFirst;
    auto flags = args.flags;
    while (flags && (p = tmp_find(formatFirst, formatLast, '{')) != nullptr) {
        if (!printout(formatFirst, (tm_size_t)(p - formatFirst))) return;
        ++p;
        if (*p == '{') {
            if (!printout("{", 1)) return;
            ++p;
            formatFirst = p;
            continue;
        }

        // parse until '}'
        auto next = tmp_find(formatFirst, formatLast, '}');
        if (!next) {
            TM_ASSERT(0 && "illformed format");
            break;
        }

        PrintFormat printFormat = initialFormatting;
        printFormat.flags = format_flags;
        auto currentIndex = index;
        auto current = flags & PrintType::Mask;
        auto formatFlags = tmp_parse_format_specifiers(p, (tm_size_t)(next - p), &printFormat, &currentIndex);
        printFormat.flags |= formatFlags;

        if (!(formatFlags & FormatSpecifierFlags::IndexSpecified)) {
            ++index;
            flags >>= PrintType::Bits;
        } else {
            current = (args.flags >> (currentIndex * PrintType::Bits)) & PrintType::Mask;
        }
        formatFirst = next + 1;

        TM_ASSERT(currentIndex < args.size);
        if ((formatFlags & FormatSpecifierFlags::PrecisionSpecified) &&
            (current == PrintType::Int32 || current == PrintType::UInt32 || current == PrintType::Int64 ||
             current == PrintType::UInt64)) {
            if (formatFlags & FormatSpecifierFlags::WidthSpecified) {
                printFormat.width = TM_MIN(printFormat.precision, printFormat.width);
            } else {
                printFormat.width = printFormat.precision;
            }
        }
        if (!printout((int)current, args.args[currentIndex], printFormat)) return;
    }
    if (formatFirst < formatLast) {
        if (!printout(formatFirst, (tm_size_t)(formatLast - formatFirst))) return;
    }
}

}  // anonymous namespace

#ifndef TMP_NO_CRT_FILE_PRINTING
TMP_DEF tm_errc tmp_print(FILE* out, const char* format, const PrintArgList& args) {
    char sbo[TMP_SBO_SIZE];
    tmp_memory_printer printer = {sbo, 0, TMP_SBO_SIZE, true, false};

    tmp_print_impl(format, TM_STRLEN(format), defaultPrintFormat(), args, printer);
    fwrite(printer.data, sizeof(char), printer.size, out);
    return printer.ec;
}
#ifdef TM_STRING_VIEW
TMP_DEF tm_errc tmp_print(FILE* out, TM_STRING_VIEW format, const PrintArgList& args) {
    char sbo[TMP_SBO_SIZE];
    tmp_memory_printer printer = {sbo, 0, TMP_SBO_SIZE, true, false};

    tmp_print_impl(TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), defaultPrintFormat(), args, printer);
    fwrite(printer.data, sizeof(char), printer.size, out);
    return printer.ec;
}
#endif  // defined(TM_STRING_VIEW)
#endif  // !defined(TMP_NO_CRT_FILE_PRINTING)

TMP_DEF tm_size_t tmp_snprint(char* dest, tm_size_t len, const char* format, const PrintFormat& initialFormatting,
                              const PrintArgList& args) {
    tmp_memory_printer mem{dest, 0, len, false, false};
    tmp_print_impl(format, TM_STRLEN(format), initialFormatting, args, mem);
    return mem.size;
}
#ifdef TM_STRING_VIEW
TMP_DEF tm_size_t tmp_snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initialFormatting,
                              const PrintArgList& args) {
    tmp_memory_printer mem{dest, 0, len, false, false};
    tmp_print_impl(TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), initialFormatting, args, mem);
    return mem.size;
}
#endif  // defined(TM_STRING_VIEW)

#endif // TM_PRINT_IMPLEMENTATION

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