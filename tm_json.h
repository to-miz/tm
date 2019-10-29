/*
tm_json.h v0.4.1 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2016

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_JSON_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

    See SWITCHES for more options.

PURPOSE
    A Utf-8 json parser, that is extensible using a lot of different flags.
    Can be used as both a simple and lightweight non validating parser, as well as a strict
    validating parser. Using the functions with the Ex extension, you can make the parser
    accept Json5 files.
    The parser only works with and expects valid Utf-8 encoded input. Utf-8 encoding is
    not validated by the parser.

DOCUMENTATION
    Nil Object:
    Nil Array:
    Nil Value:
        Any function or method that returns a JsonObject or JsonArray directly returns a nil object
        or array in case the value is not containing an object or array.
        If you then pass these nil objects or arrays into functions that accept JsonObjects or
        JsonArrays, you will be essentially performing a noop. Getting members of nil objects in
        turn return nil values, which when converted into an int for instance return the default
        value supplied to the conversion function.
        Nil objects allow you to write code without having to check for validity whenever you do a
        query. The code will run silently without raising errors and you will fill your data with
        default values.
        You can check whether an object, array or value is nil using a simple if in C++ (all of
        them overload explicit operator bool) or using jsonIsValidObject/Array/Value.

    Difference between Nil Values and Values that contain null:
        Nil values are returned when querying for a value that does not exist in the json (nil =
        not in list). Values contain null if they exist in the json and were assigned null
        explicitly (ie "value": null).
        You can differentiate between nil values and values that contain null by calling
        jsonIsNull or isNull member function.
        Nil values return false for jsonIsNull, but values that contain a null return true.

SWITCHES
    You can change how most of the things inside this library are implemented.
    Starting from which dependencies to use, you can make this library not use the crt by defining
    the functions it depends on (see the block of #defines at TM_MEMCHR).
    These need to be defined before the header is included.
    You can toggle some extra functionality using these switches:
        TMJ_NO_INT64:
            define this if your target platform does not have 64bit integers.
        TM_STRING_VIEW:
            define this if you have a string_view class. This will enable some overloads that will
            accept your string_view class. Define like this:
                #define TM_STRING_VIEW my_string_view_class_name
                #define TM_STRING_VIEW_DATA(name) (name).data()
                #define TM_STRING_VIEW_SIZE(name) (name).size()
            The example above assumes that getting the data and size of the string_view happens
            through member functions. If your string_view interface differs, change accordingly.
        TMJ_TO_INT(first, last, out_ptr, base):
        TMJ_TO_UINT(first, last, out_ptr, base):
        TMJ_TO_INT64(first, last, out_ptr, base):
        TMJ_TO_UINT64(first, last, out_ptr, base):
        TMJ_TO_FLOAT(first, last, out_ptr):
        TMJ_TO_DOUBLE(first, last, out_ptr):
            Define these macros if you have locale independent string conversion functions that
            accept non nullterminated strings.
            These macros represent functions with these signatures:
                void to_int(const char* first, const char* last, int32_t* out_ptr, int32_t base)
                void to_uint(const char* first, const char* last, uint32_t* out_ptr, int base)
                void to_int64(const char* first, const char* last, int64_t* out_ptr, int base)
                void to_uint64(const char* first, const char* last, uint64_t* out_ptr, int base)
                void to_float(const char* first, const char* last, float* out_ptr)
                void to_double(const char* first, const char* last, double* out_ptr)
            Arguments:
                first/last: A pair of iterators to a non nullterminated string.
                out_ptr:    Output value to be written on successful conversion. On error, it should
                            stay unmodified.
                base:       Base to use for the integer conversion.
            If you do not define these, the implementation will use stdlib.h to implement them.
            Since the crt string conversion functions do not allow non nullterminated strings, the
            implementation has to copy the strings into a temporary buffer and nullterminate it
            before passing them into the crt string conversion functions.
        TMJ_DEFINE_INFINITY_AND_NAN:
            Define this if you want to use the float extension flag and the math.h definitions of
            INFINITY and NAN. If you do not wish to use the float extension flag
            when parsing (JSON_READER_EXTENDED_FLOATS), you do not need to define these.
            Otherwise define TM_INFINITY and TM_NAN yourself with the inf and nan representations
            on your platform.
        TM_INFINITY:
        TM_NAN:
            These are automatically defined if you define TMJ_DEFINE_INFINITY_AND_NAN and if math.h
            has definitions for INFINITY and NAN. If you do not wish to use the float extension flag
            when parsing (JSON_READER_EXTENDED_FLOATS), you do not need to define these.
            Otherwise define TM_INFINITY and TM_NAN yourself with the inf and nan representations
            on your platform.

ISSUES
    - No Utf-8 encoding validation of input data.
      Not really an issue but out of scope of this library, since the parsing functions expect
      valid Utf-8 encoded input. Validation should be done before parsing, for instance by using
      tm_unicode.h.
    - Mismatched brackets will be reported as JERR_UNEXPECTED_EOF instead of JERR_MISMATCHED_BRACKETS
      when using jsonMakeDocument, because jsonSkipCurrent skips until eof on mismatched brackets.
    - Missing documentation and example usage code.
    - Json objects aren't implemented as dictionaries/hashmaps, instead they use linear lookup.
      Json objects should be turned into hashmaps manually when applicable.
    - Fallback string conversion is based on <strlib.h> and thus locale dependent.
      Workaround is not changing the LC_NUMERIC locale from the default "C" locale
      or not using the fallback string conversion routines at all (see SWITCHES and TMJ_TO_INT).
    - Json5 considers some multibyte unicode codepoints as whitespace. This parser currently only supports
      single byte whitespace.
    - Json5 unquoted identifiers don't allow for \u unicode escape sequences or unicode letters.

HISTORY
    v0.4.1  29.10.19  Added JSON_READER_ALLOW_EXTENDED_WHITESPACE.
                      Fixed various issues with unquoted property names, escaped newlines and json5 string validation.
                      Parser is now tested against json5-tests (https://github.com/json5/json5-tests).
                      Added new issues regarding Json5 parsing.
    v0.4.0  29.10.19  Fixed single line and block comments being incorrectly handled in allocated documents.
                      Added new internal token type JTOK_PROPERTYCOLON to allow for comments between property
                      names and colons.
                      Added JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT for improved json5 parsing.
                      Fixed some json5 parsing issues regarding number parsing.
    v0.3.1  30.07.19  Fixed a logic error in readNumber (missing parenthesis around logical expression).
    v0.3.0  03.07.19  Fixed strict parsing, parser now allows for root level values.
                      Fixed false error reporting for some valid json files.
                      Parser is now tested against JSONTestSuite (https://github.com/nst/JSONTestSuite).
    v0.2.5  13.04.19  Fixed macro argument names of TMJ_TO_INT etc. having wrong order.
    v0.2.4  25.02.19  Implemented Unicode escape sequences.
                      Fixed a bug with unescaped string copying.
                      Fixed signed size_t compilation warnings.
    v0.2.3  24.02.19  Fixed a bug with string escaping. Escaped quotation marks weren't detected properly.
    v0.2.2  15.01.19  Changed signature of conversion functions to use sized integer types like int32_t.
    v0.2.1  14.01.19  Fixed some compilation errors when both TM_SIGNED_SIZE_T and TM_STRING_VIEW
                      are defined.
                      Improved support for TM_STRING_VIEW.
    v0.2.0  03.01.19  Removed TMJ_PASS_BY_POINTER.
                      Removed attempts ate locale independent string conversions based on CRT due to
                      being unsafe/slow.
                      Changed the TMJ_TO_INT/etc macros to be easier to redefine using <charconv>.
    v0.1.8  01.01.19  Removed unnecessary debug assertion from jsonIsNull.
                      Made the fallback string conversion functions locale independent.
    v0.1.7  31.12.18  Added stringStartsWith to be used when appropriate instead of stringEquals.
                      Fixed a bug with null values not being parsed properly.
                      Added more error info to jsonDocument.
                      Fixed GCC unused-function warning when building in release builds.
                      Improved default implementation of string conversion functions.
                      Fixed clang compilation errors for C99 compilation.
                      Fixed cl compilation errors for C99 compilation.
                      Switched from using toupper to tolower for case insensitive comparisons.
                      Fixed jsonToFloat to also use TM_INFINITY and TM_NAN.
                      Changed remaining assert calls to TM_ASSERT.
    v0.1.6  29.12.18  Fixed GCC warnings for multi-line comment, missing-field-initializers
                      and implicit-fallthrough.
                      Fixed a bug with compareString acting like stringStartsWith.
                      Renamed compareString functions to stringEquals, since they only check equality.
                      Removed tmj_bool and TMJ_NULL, using tm_bool and TM_NULL instead.
                      Reformatted and switched to indentation using spaces.
    v0.1.5  06.10.18  Refactored preprocessor.
                      Changed tmj_size_t to tm_size_t.
                      Changed TMJ_STRING_VIEW to TM_STRING_VIEW.
    v0.1.4b 25.08.18  Added repository link.
    v0.1.4a 22.07.18  Fixed some warnings on C99 compilation.
    v0.1.4  22.07.18  Removed overloads of C functions for C++, updated licensing.
    v0.1.3  10.02.17  String view operators are now preferred to const char* operators in JsonObject.
    v0.1.2  28.01.17  Added jsonObjectArray for usage with C++11 range based loops.
    v0.1.1d 10.01.17  Minor change from static const char* to static const char* const in some places.
    v0.1.1c 07.11.16  Minor edits, no runtime changes.
    v0.1.1b 10.10.16  Fixed some warnings when tm_size_t is signed.
    v0.1.1a 07.10.16  Removed usage of unsigned arithmetic when tm_size_t is signed.
    v0.1.1  13.09.16  Changed JsonValue interface to have operator[] overloads for convenience.
                      Added more string_view support through TM_STRING_VIEW.
    v0.1    11.09.16  Initial commit.
*/

/* Examples. */

#if 0
// Sample code to use C++17 <charconv> header as the backend for string conversions.
// <charconv> has the advantage that it is faster and locale independent from the get go.

#include <charconv>

#define TMJ_TO_INT(first, last, out_ptr, base) std::from_chars((first), (last), *(out_ptr), (base))
#define TMJ_TO_UINT(first, last, out_ptr, base) std::from_chars((first), (last), *(out_ptr), (base))
#define TMJ_TO_INT64(first, last, out_ptr, base) std::from_chars((first), (last), *(out_ptr), (base))
#define TMJ_TO_UINT64(first, last, out_ptr, base) std::from_chars((first), (last), *(out_ptr), (base))
#define TMJ_TO_FLOAT(first, last, out_ptr) std::from_chars((first), (last), *(out_ptr))
#define TMJ_TO_DOUBLE(first, last, out_ptr) std::from_chars((first), (last), *(out_ptr))
#define TM_JSON_IMPLEMENTATION
#include <tm_json.h>

// Now the implementation of tm_json will use <charconv> as the backend for string conversions.
#endif

/* clang-format off */
#ifdef TM_JSON_IMPLEMENTATION
    /* define these to avoid crt */
    #ifndef TM_MEMCHR
        #include <string.h>
        #define TM_MEMCHR memchr
    #endif
    #ifndef TM_STRLEN
        #include <string.h>
        #define TM_STRLEN strlen
    #endif
    #if !defined(TM_ISDIGIT) || !defined(TM_ISALPHA) || !defined(TM_ISXDIGIT) \
        || !defined(TM_ISSPACE) || !defined(TM_TOLOWER)
        #include <ctype.h>
        #ifndef TM_ISDIGIT
            #define TM_ISDIGIT isdigit
        #endif
        #ifndef TM_ISALPHA
            #define TM_ISALPHA isalpha
        #endif
        #ifndef TM_ISXDIGIT
            #define TM_ISXDIGIT isxdigit
        #endif
        #ifndef TM_ISSPACE
            #define TM_ISSPACE isspace
        #endif
        #ifndef TM_TOLOWER
            #define TM_TOLOWER tolower
        #endif
    #endif
    #ifndef TM_MEMCMP
        #include <string.h>
        #define TM_MEMCMP memcmp
    #endif
    #ifndef TM_MEMCPY
        #include <string.h>
        #define TM_MEMCPY memcpy
    #endif
    #ifndef TM_MEMSET
        #include <string.h>
        #define TM_MEMSET memset
    #endif
    // define TMJ_ALLOCATE and TMJ_FREE with your own allocator if dynamic allocation is unwanted
    #ifndef TMJ_ALLOCATE
        #ifdef __cplusplus
            #define TMJ_ALLOCATE(size, alignment) (new char[(size)])
            #define TMJ_FREE(ptr, size) (delete[] (ptr))
        #else
            #include <stdlib.h>
            #define TMJ_ALLOCATE(size, alignment) (malloc(size))
            #define TMJ_FREE(ptr, size) (free(ptr))
        #endif
    #endif

    // define this if you want to use infinity and nan extensions for json using math.h
    // otherwise define TM_INFINITY and TM_NAN yourself with whatever the infinity and nan values
    // are for your target platform
    #ifdef TMJ_DEFINE_INFINITY_AND_NAN
        #ifndef TM_INFINITY
            #include <math.h>
            #ifdef INFINITY
                #define TM_INFINITY INFINITY
            #endif
            #ifdef NAN
                #define TM_NAN NAN
            #endif
        #endif
    #endif

    // define these if you have string conversion functions that accept non nullterminated strings
    // if you do not define TMJ_TO_INT etc, the implementation uses stdlib.h to implement these
    // the signature of these functions are:
    // str: a non nullterminated string
    // len: the length of str
    // base: base to use when converting str into the return value
    // def: the default value to use if conversion fails
    // for more details, see TMJ_TO_INT section at SWITCHES at the top of this file
    #ifndef TMJ_TO_INT
        #define TMJ_DEFINE_OWN_STRING_CONVERSIONS
        #define TMJ_TO_INT(str, len, out_ptr, base) tmj_to_int((str), (len), (out_ptr), (base))
        #define TMJ_TO_UINT(str, len, out_ptr, base) tmj_to_uint((str), (len), (out_ptr), (base))
        #define TMJ_TO_INT64(str, len, out_ptr, base) tmj_to_int64((str), (len), (out_ptr), (base))
        #define TMJ_TO_UINT64(str, len, out_ptr, base) tmj_to_uint64((str), (len), (out_ptr), (base))
        #define TMJ_TO_FLOAT(str, len, out_ptr) tmj_to_float((str), (len), (out_ptr))
        #define TMJ_TO_DOUBLE(str, len, out_ptr) tmj_to_double((str), (len), (out_ptr))
    #endif
#endif

#ifndef _TM_JSON_H_INCLUDED_
#define _TM_JSON_H_INCLUDED_

#ifndef TM_ASSERT
    #include <assert.h>
    #define TM_ASSERT assert
#endif

#include <stdint.h>

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
    #include <stddef.h> /* include C version so identifiers are in global namespace */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

typedef unsigned char tmj_nesting_count;
#define TMJ_MAX_NESTING_COUNT 255

#ifndef TMJ_STATIC
    #define TMJ_DEF extern
#else
    #define TMJ_DEF static
#endif

/* Use null of the underlying language. */
#ifndef TM_NULL
    #ifdef __cplusplus
        #define TM_NULL nullptr
    #else
        #define TM_NULL NULL
    #endif
#endif

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

/* clang-format on */

typedef enum {
    JVAL_NULL,
    JVAL_STRING,
    JVAL_OBJECT,
    JVAL_ARRAY,
    JVAL_INT,
    JVAL_UINT,
    JVAL_BOOL,
    JVAL_FLOAT,

    // extended types
    // JVAL_UINT_HEX,
    JVAL_RAW_STRING,
    JVAL_CONCAT_STRING,
} JsonValueType;

typedef enum {
    JTOK_EOF,

    JTOK_PROPERTYNAME,
    JTOK_OBJECT_START,
    JTOK_ARRAY_START,
    JTOK_OBJECT_END,
    JTOK_ARRAY_END,
    JTOK_VALUE,
    JTOK_COMMENT,

    // These two tokens will not be returned by jsonNextToken and similar, they are internal only.
    JTOK_COMMA,
    JTOK_PROPERTYCOLON,

    // Returned on tokenizing error.
    JTOK_ERROR,
} JsonTokenType;

typedef enum {
    // allow single line comments with '//'
    JSON_READER_SINGLE_LINE_COMMENTS = (1u << 0u),

    // allow block comments with '/**/'
    JSON_READER_BLOCK_COMMENTS = (1u << 1u),

    // allow python comments using '#'
    JSON_READER_PYTHON_COMMENTS = (1u << 2u),

    // allow trailing commas in arrays and objects
    JSON_READER_TRAILING_COMMA = (1u << 3u),

    // allow unquoted property names
    JSON_READER_UNQUOTED_PROPERTY_NAMES = (1u << 4u),

    // allow single quoted strings using '
    JSON_READER_SINGLE_QUOTED_STRINGS = (1u << 5u),

    // allow infinity, -infinity, nan, -nan
    JSON_READER_EXTENDED_FLOATS = (1u << 6u),

    // allow hexadecimal integers using prefix '0x'
    JSON_READER_HEXADECIMAL = (1u << 7u),

    /*
    allow multiline strings using escaped newlines
    example:
        "property": "this is a multiline \
        string"
    */
    JSON_READER_ESCAPED_MULTILINE_STRINGS = (1u << 8u),

    // allow C++0x style raw string literals
    // example
    // "property": R"delim(...)delim"
    // if both cpp style raw strings and python style raw strings are enabled,
    // cpp style raw strings have precedence
    JSON_READER_CPP_RAW_STRINGS = (1u << 9u),

    // allow python style raw string literals
    // example
    // "property": r'...'
    JSON_READER_PYTHON_RAW_STRINGS = (1u << 10u),

    // allow C style concatenated multiline strings after a property name
    // example
    // "property":  "this is a multiline "
    //              "string"
    JSON_READER_CONCATENATED_STRINGS = (1u << 11u),

    // allow C style concatenated multiline strings in arrays, errorprone in case a comma is
    // forgotten mistakenly
    JSON_READER_CONCATENATED_STRINGS_IN_ARRAYS = (1u << 12u),

    // allow '=' and ':' for property names
    JSON_READER_ALLOW_EQUAL = (1u << 13u),

    // allow numbers to begin with an explicit plus sign
    JSON_READER_ALLOW_PLUS_SIGN = (1u << 14u),

    // allow case insensitive true, false, null keywords and infinity etc in case of extended floats
    JSON_READER_IGNORE_CASE_KEYWORDS = (1u << 15u),

    // allow decimal point to lead and trail numbers like .8 or 8.
    JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT = (1u << 16u),

    // allow some whitespace that strict json doesn't allow in json files like form feed.
    JSON_READER_ALLOW_EXTENDED_WHITESPACE = (1u << 17u),

    // use these flags to parse json5 files
    JSON_READER_JSON5 =
        (JSON_READER_SINGLE_LINE_COMMENTS | JSON_READER_BLOCK_COMMENTS | JSON_READER_TRAILING_COMMA |
         JSON_READER_UNQUOTED_PROPERTY_NAMES | JSON_READER_SINGLE_QUOTED_STRINGS | JSON_READER_EXTENDED_FLOATS |
         JSON_READER_HEXADECIMAL | JSON_READER_ESCAPED_MULTILINE_STRINGS | JSON_READER_ALLOW_PLUS_SIGN |
         JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT | JSON_READER_ALLOW_EXTENDED_WHITESPACE),

    // all flags
    JSON_READER_ALL =
        (JSON_READER_SINGLE_LINE_COMMENTS | JSON_READER_BLOCK_COMMENTS | JSON_READER_PYTHON_COMMENTS |
         JSON_READER_TRAILING_COMMA | JSON_READER_UNQUOTED_PROPERTY_NAMES | JSON_READER_SINGLE_QUOTED_STRINGS |
         JSON_READER_EXTENDED_FLOATS | JSON_READER_HEXADECIMAL | JSON_READER_ESCAPED_MULTILINE_STRINGS |
         JSON_READER_CPP_RAW_STRINGS | JSON_READER_PYTHON_RAW_STRINGS | JSON_READER_CONCATENATED_STRINGS |
         JSON_READER_CONCATENATED_STRINGS_IN_ARRAYS | JSON_READER_ALLOW_EQUAL | JSON_READER_ALLOW_PLUS_SIGN |
         JSON_READER_IGNORE_CASE_KEYWORDS),

    // reasonably extended json, most parsers allow at least this much
    JSON_READER_REASONABLE = (JSON_READER_SINGLE_LINE_COMMENTS | JSON_READER_TRAILING_COMMA),

    // strict
    JSON_READER_STRICT = 0u
} JsonReaderFlags;

typedef enum { JSON_CONTEXT_NULL, JSON_CONTEXT_OBJECT, JSON_CONTEXT_ARRAY } JsonContext;

typedef enum {
    JSON_OK,
    JERR_UNEXPECTED_TOKEN,
    JERR_UNEXPECTED_EOF,
    JERR_ILLFORMED_STRING,
    JERR_MISMATCHED_BRACKETS,
    JERR_OUT_OF_CONTEXT_STACK_MEMORY,  // Not enough contextStackMemory to parse a given json file,
                                       // occurs if there are more alternating context (going from
                                       // { to [ and back) switches than memory.
    JERR_NO_ROOT,
    JERR_OUT_OF_MEMORY,
    JERR_INTERNAL_ERROR
} JsonErrorType;

TMJ_DEF const char* jsonGetErrorString(JsonErrorType error);

typedef struct {
    const char* data;
    tm_size_t size;

#if defined(__cplusplus) && defined(TM_STRING_VIEW)
    inline operator TM_STRING_VIEW() const { return TM_STRING_VIEW_MAKE(data, size); };
#endif
} JsonStringView;
typedef struct {
    int8_t context;
    tmj_nesting_count count;
} JsonContextEntry;
typedef struct {
    JsonContextEntry* data;
    tm_size_t size;
    tm_size_t capacity;
} JsonContextStack;
typedef struct {
    const char* data;  // parsing data
    tm_size_t size;

    tm_size_t line;    // current line
    tm_size_t column;  // current column

    // these next 3 fields are safe to access
    JsonStringView current;   // the contents of the current token
    JsonTokenType lastToken;  // last token that was parsed
    JsonValueType valueType;  // value type of token, if token is JTOK_VALUE

    unsigned int flags;  // parser flags

    JsonContextStack contextStack;  // contextStack used when parsing in non implicit mode
    JsonErrorType errorType;
} JsonReader;

// getter functions if you are uncomfortable accessing the inner of JsonReader
inline JsonStringView jsonGetString(JsonReader* reader) { return reader->current; }
inline JsonValueType jsonGetValueType(JsonReader* reader) { return reader->valueType; }

// Initializes and returns a JsonReader.
// params:
//  data: The Utf-8 json file contents. Must be a valid Utf-8 encoded string.
//  size: Size of data in bytes.
//  contextStackMemory:
//      The stack memory the reader uses for bookkeeping. Only needed if you use jsonNextToken or
//      jsonNextTokenEx. If you plan to use the implicit versions (jsonNextTokenImplicit or
//      jsonNextTokenImplicitEx), this can be NULL.
//  contextStackSize: Element count of contextStackMemory.
//  flags: Parsing flags used when parsing. See enum JsonReaderFlags for all valid flags.
TMJ_DEF JsonReader jsonMakeReader(const char* data, tm_size_t size, JsonContextEntry* contextStackMemory,
                                  tm_size_t contextStackSize, unsigned int flags);

// parses json only accepting the following parsing flags:
//      - JSON_READER_SINGLE_QUOTED_STRINGS
//      - JSON_READER_SINGLE_LINE_COMMENTS
//      - JSON_READER_TRAILING_COMMA
//      - JSON_READER_STRICT
//      - JSON_READER_ALLOW_EQUAL
//      - JSON_READER_ESCAPED_MULTILINE_STRINGS
//      - JSON_READER_ALLOW_PLUS_SIGN
//      - JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT
// since there are fewer special cases, it is slightly faster than jsonNextTokenEx and
// jsonNextTokenImplicitEx
TMJ_DEF JsonTokenType jsonNextTokenImplicit(JsonReader* reader, JsonContext currentContext);
TMJ_DEF JsonTokenType jsonNextToken(JsonReader* reader);
// parses the json until reaching eof, call this after having reached the closing bracket of root
// to make sure that nothing follows after the closing bracket
TMJ_DEF tm_bool jsonIsValidUntilEof(JsonReader* reader);

// parses json accepting all flags
TMJ_DEF JsonTokenType jsonNextTokenImplicitEx(JsonReader* reader, JsonContext currentContext);
TMJ_DEF JsonTokenType jsonNextTokenEx(JsonReader* reader);
TMJ_DEF tm_bool jsonIsValidUntilEofEx(JsonReader* reader);

// reads from json until root type is encountered
TMJ_DEF JsonTokenType jsonReadRootType(JsonReader* reader, tm_bool ex);
// skips current context
TMJ_DEF tm_bool jsonSkipCurrent(JsonReader* reader, JsonContext currentContext, tm_bool ex);

// copy the unescaped version of str into buffer with size
// JsonReader only stores the unprocessed contents of the current token, so you need to call this to
// get a copy of the parsed/unescaped string.
tm_size_t jsonCopyUnescapedString(JsonStringView str, char* buffer, tm_size_t size);

// copy a concatenated string.
// You only need to call this if you use JSON_READER_CONCATENATED_STRINGS or
// JSON_READER_CONCATENATED_STRINGS_IN_ARRAYS and the valueType of the current token is
// JVAL_CONCAT_STRING
tm_size_t jsonCopyConcatenatedString(JsonStringView str, char* buffer, tm_size_t size);

struct JsonStackAllocatorStruct;
typedef struct JsonStackAllocatorStruct JsonStackAllocator;

// same as the copy versions above, but these use JsonStackAllocator to allocate the result
JsonStringView jsonAllocateUnescapedString(JsonStackAllocator* allocator, JsonStringView str);
JsonStringView jsonAllocateConcatenatedString(JsonStackAllocator* allocator, JsonStringView str);

// value conversion functions to turn the contents of the current token to values
TMJ_DEF int32_t jsonToInt(JsonStringView str, int32_t def);
TMJ_DEF uint32_t jsonToUInt(JsonStringView str, uint32_t def);
TMJ_DEF float jsonToFloat(JsonStringView str, float def);
TMJ_DEF double jsonToDouble(JsonStringView str, double def);
TMJ_DEF tm_bool jsonToBool(JsonStringView str, tm_bool def);
#ifndef TMJ_NO_INT64
TMJ_DEF int64_t jsonToInt64(JsonStringView str, int64_t def);
TMJ_DEF uint64_t jsonToUInt64(JsonStringView str, uint64_t def);
#endif

// json document
struct JsonValueStruct;
struct JsonNodeStruct;
typedef struct JsonNodeStruct JsonNode;
typedef struct JsonValueStruct JsonValue;

// string view wrappers
#ifdef TM_STRING_VIEW
typedef TM_STRING_VIEW tmj_string_arg;
typedef TM_STRING_VIEW tmj_string_view;
#define TMJ_STRING_VIEW_MAKE(data, size) TM_STRING_VIEW_MAKE(data, size)
#else
typedef const char* tmj_string_arg;
typedef JsonStringView tmj_string_view;
#define TMJ_STRING_VIEW_MAKE(data, size) {(data), (size)}
#endif

typedef struct {
    JsonNode* nodes;
    tm_size_t count;

#ifdef __cplusplus
    // caching of which node we accessed last, makes jsonGetMember and operator[] slightly more
    // efficient. Not really part of the state of JsonObject, so mutable is fine here.
    mutable tm_size_t lastAccess;

    tm_size_t size() const;
    inline explicit operator bool() const { return nodes != nullptr; }

    bool exists(tmj_string_arg name) const;
    JsonValue* find(tmj_string_arg name) const;
    JsonValue operator[](tmj_string_arg name) const;
#endif
} JsonObject;

typedef struct {
    JsonValue* values;
    tm_size_t count;

#ifdef __cplusplus
    tm_size_t size() const;
    JsonValue operator[](tm_size_t index) const;
    inline explicit operator bool() const { return values != nullptr; }
#endif
} JsonArray;

typedef struct {
    JsonValue* values;
    tm_size_t count;

#ifdef __cplusplus
    tm_size_t size() const;
    JsonObject operator[](tm_size_t index) const;
    inline explicit operator bool() const { return values != nullptr; }
#endif
} JsonObjectArray;

struct JsonValueStruct {
    JsonValueType type;
    union {
        JsonStringView content;
        JsonArray array;
        JsonObject object;
    } data;

#ifdef __cplusplus
    tmj_string_view getString(tmj_string_view def = {}) const;

    int32_t getInt(int32_t def = 0) const;
    uint32_t getUInt(uint32_t def = 0) const;
    float getFloat(float def = 0.0f) const;
    double getDouble(double def = 0.0f) const;
    bool getBool(bool def = false) const;
#ifndef TMJ_NO_INT64
    int64_t getInt64(int64_t def = 0) const;
    uint64_t getUInt64(uint64_t def = 0) const;
#endif

    // same as jsonGetObject
    // returns a JsonObject if value contains an object, returns a nil object otherwise
    // see documentation at the top of the file for what nil objects are
    JsonObject getObject() const;
    // same as jsonGetArray
    // returns a JsonArray if value contains an array, returns a nil array otherwise
    // see documentation at the top of the file for what nil arrays are
    JsonArray getArray() const;
    JsonObjectArray getObjectArray() const;

    // returns whether value is null
    bool isNull() const;
    // returns whether value is present
    inline explicit operator bool() const { return type != JVAL_NULL; }

    // convenience overload to treat a value as an object directly
    // use getObject directly, if you need to invoke operator[] or jsonGetMember repeatedly
    inline JsonValue operator[](tmj_string_arg name) const { return getObject()[name]; }

    // convenience overload to treat a value as an array directly
    // use getArray directly, if you need to invoke operator[] or jsonGetEntry repeatedly
    inline JsonValue operator[](tm_size_t index) { return getArray()[index]; }

    // returns true if value is int, uint, bool or float
    bool isIntegral() const;
    bool isString() const;
#endif
};
struct JsonNodeStruct {
    JsonStringView name;
    JsonValue value;
};

struct JsonStackAllocatorStruct {
    char* ptr;
    size_t size;
    size_t capacity;
};
unsigned int jsonGetAlignmentOffset(const void* ptr, unsigned int alignment);
void* jsonAllocate(JsonStackAllocator* allocator, size_t size, unsigned int alignment);

typedef struct JsonDocumentStruct {
    JsonValue root;
    struct {
        JsonErrorType type;
        tm_size_t line;
        tm_size_t column; // Byte column/offset from beginning of line.
        tm_size_t offset; // Offset from beginning of the json file in bytes.
        tm_size_t length; // Length of the token that caused the error.
    } error;
#ifdef __cplusplus
    inline explicit operator bool() const { return error.type == JSON_OK; }
#endif
} JsonDocument;

typedef struct JsonAllocatedDocumentStruct {
    JsonDocument document;
    char* pool;
    size_t poolSize;
} JsonAllocatedDocument;

// parses json only accepting the following parsing flags:
//      - JSON_READER_SINGLE_QUOTED_STRINGS
//      - JSON_READER_SINGLE_LINE_COMMENTS
//      - JSON_READER_TRAILING_COMMA
//      - JSON_READER_STRICT
//      - JSON_READER_ALLOW_EQUAL
//      - JSON_READER_ESCAPED_MULTILINE_STRINGS
//      - JSON_READER_ALLOW_PLUS_SIGN
//      - JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT
// since there are fewer special cases, it is slightly faster than jsonAllocateDocumentEx
TMJ_DEF JsonAllocatedDocument jsonAllocateDocument(const char* data, tm_size_t size, unsigned int flags);
TMJ_DEF void jsonFreeDocument(JsonAllocatedDocument* doc);
TMJ_DEF JsonDocument jsonMakeDocument(JsonStackAllocator* allocator, const char* data, tm_size_t size,
                                      unsigned int flags);

// parses json accepting all flags
TMJ_DEF JsonAllocatedDocument jsonAllocateDocumentEx(const char* data, tm_size_t size, unsigned int flags);
TMJ_DEF JsonDocument jsonMakeDocumentEx(JsonStackAllocator* allocator, const char* data, tm_size_t size,
                                        unsigned int flags);
/* clang-format off */
#ifdef __cplusplus
    JsonNode* begin(const JsonObject& a);
    JsonNode* end(const JsonObject& a);
    JsonValue* begin(const JsonArray& a);
    JsonValue* end(const JsonArray& a);

    // iterator for range based loops that yield JsonObjects when dereferenced
    struct JsonObjectArrayIterator {
        JsonValue* ptr;

        bool operator!=(JsonObjectArrayIterator other) const;
        bool operator==(JsonObjectArrayIterator other) const;
        JsonObjectArrayIterator& operator++();
        JsonObjectArrayIterator operator++(int);
        JsonObject operator*() const;
    };

    JsonObjectArrayIterator begin(const JsonObjectArray& a);
    JsonObjectArrayIterator end(const JsonObjectArray& a);
#endif
/* clang-format on */

TMJ_DEF tm_bool jsonIsNull(const JsonValue* value);
TMJ_DEF tm_bool jsonIsIntegral(const JsonValue* value);
TMJ_DEF tm_bool jsonIsString(const JsonValue* value);
// returns a JsonObject if value contains an object, returns a nil object otherwise
// see documentation at the top of the file for what nil objects are
TMJ_DEF JsonObject jsonGetObject(const JsonValue* value);
// returns a JsonArray if value contains an array, returns a nil array otherwise
// see documentation at the top of the file for what nil arrays are
TMJ_DEF JsonArray jsonGetArray(const JsonValue* value);
TMJ_DEF JsonObjectArray jsonGetObjectArray(const JsonValue* value);

TMJ_DEF JsonValue jsonGetMember(const JsonObject* object, const char* name);
TMJ_DEF JsonValue* jsonQueryMember(const JsonObject* object, const char* name);
TMJ_DEF JsonValue jsonGetMemberCached(const JsonObject* object, const char* name, tm_size_t* lastAccess);
TMJ_DEF JsonValue* jsonQueryMemberCached(const JsonObject* object, const char* name, tm_size_t* lastAccess);
TMJ_DEF JsonValue jsonGetEntry(const JsonArray* object, tm_size_t index);

// string_view overloads
#if defined(__cplusplus) && defined(TM_STRING_VIEW)
TMJ_DEF JsonValue jsonGetMember(const JsonObject* object, TM_STRING_VIEW name);
TMJ_DEF JsonValue* jsonQueryMember(const JsonObject* object, TM_STRING_VIEW name);
TMJ_DEF JsonValue jsonGetMemberCached(const JsonObject* object, TM_STRING_VIEW name, tm_size_t* lastAccess);
TMJ_DEF JsonValue* jsonQueryMemberCached(const JsonObject* object, TM_STRING_VIEW name, tm_size_t* lastAccess);
#endif  // defined( __cplusplus ) && defined( TM_STRING_VIEW )

TMJ_DEF tm_bool jsonIsValidObject(const JsonObject* object);
TMJ_DEF tm_bool jsonIsValidArray(const JsonArray* array);
TMJ_DEF tm_bool jsonIsValidValue(const JsonValue* value);

TMJ_DEF int32_t jsonGetInt(const JsonValue* value, int32_t def);
TMJ_DEF uint32_t jsonGetUInt(const JsonValue* value, uint32_t def);
TMJ_DEF float jsonGetFloat(const JsonValue* value, float def);
TMJ_DEF double jsonGetDouble(const JsonValue* value, double def);
TMJ_DEF tm_bool jsonGetBool(const JsonValue* value, tm_bool def);
#ifndef TMJ_NO_INT64
TMJ_DEF int64_t jsonGetInt64(const JsonValue* value, int64_t def);
TMJ_DEF uint64_t jsonGetUInt64(const JsonValue* value, uint64_t def);
#endif

// inline implementations

// define tmj_valid_index differently if tm_size_t is unsigned, so we don't get -Wtype-limits warning
#if !defined(TM_SIZE_T_IS_SIGNED) || TM_SIZE_T_IS_SIGNED
inline static tm_bool tmj_valid_index(tm_size_t index, tm_size_t size) { return index >= 0 && index < size; }
#else
inline static tm_bool tmj_valid_index(tm_size_t index, tm_size_t size) { return index < size; }
#endif

#ifdef __cplusplus
inline JsonNode* begin(const JsonObject& a) { return a.nodes; }
inline JsonNode* end(const JsonObject& a) { return a.nodes + a.count; }
inline JsonValue* begin(const JsonArray& a) { return a.values; }
inline JsonValue* end(const JsonArray& a) { return a.values + a.count; }

inline bool JsonObjectArrayIterator::operator!=(JsonObjectArrayIterator other) const { return ptr != other.ptr; }
inline bool JsonObjectArrayIterator::operator==(JsonObjectArrayIterator other) const { return ptr != other.ptr; }
inline JsonObjectArrayIterator& JsonObjectArrayIterator::operator++() {
    ++ptr;
    return *this;
}
inline JsonObjectArrayIterator JsonObjectArrayIterator::operator++(int) {
    JsonObjectArrayIterator result = *this;
    ++ptr;
    return result;
}
inline JsonObject JsonObjectArrayIterator::operator*() const { return ptr->getObject(); }

inline JsonObjectArrayIterator begin(const JsonObjectArray& a) { return {a.values}; }
inline JsonObjectArrayIterator end(const JsonObjectArray& a) { return {a.values + a.count}; }

inline tmj_string_view JsonValueStruct::getString(tmj_string_view def) const {
    if ((type == JVAL_NULL && data.content.size == 0) || type == JVAL_OBJECT || type == JVAL_ARRAY) {
        return def;
    }
    return TMJ_STRING_VIEW_MAKE(data.content.data, data.content.size);
}

inline JsonObject JsonValueStruct::getObject() const { return jsonGetObject(this); }
inline JsonArray JsonValueStruct::getArray() const { return jsonGetArray(this); }
inline JsonObjectArray JsonValueStruct::getObjectArray() const { return jsonGetObjectArray(this); }
inline bool JsonValueStruct::isNull() const { return jsonIsNull(this); }
inline bool JsonValueStruct::isIntegral() const { return jsonIsIntegral(this); }
inline bool JsonValueStruct::isString() const { return jsonIsString(this); }

inline tm_size_t JsonArray::size() const { return count; }
inline JsonValue JsonArray::operator[](tm_size_t index) const { return jsonGetEntry(this, index); }

inline tm_size_t JsonObjectArray::size() const { return count; }
inline JsonObject JsonObjectArray::operator[](tm_size_t index) const {
    TM_ASSERT(tmj_valid_index(index, count));
    return jsonGetObject(&values[index]);
}

inline tm_size_t JsonObject::size() const { return count; }
inline JsonValue JsonObject::operator[](tmj_string_arg name) const { return jsonGetMember(this, name); }
inline bool JsonObject::exists(tmj_string_arg name) const { return jsonQueryMember(this, name) != TM_NULL; }
inline JsonValue* JsonObject::find(tmj_string_arg name) const { return jsonQueryMember(this, name); }

inline int32_t JsonValueStruct::getInt(int32_t def) const { return jsonGetInt(this, def); }
inline uint32_t JsonValueStruct::getUInt(uint32_t def) const { return jsonGetUInt(this, def); }
inline float JsonValueStruct::getFloat(float def) const { return jsonGetFloat(this, def); }
inline double JsonValueStruct::getDouble(double def) const { return jsonGetDouble(this, def); }
inline bool JsonValueStruct::getBool(bool def) const { return jsonGetBool(this, def); }
#ifndef TMJ_NO_INT64
inline int64_t JsonValueStruct::getInt64(int64_t def) const { return jsonGetInt64(this, def); }
inline uint64_t JsonValueStruct::getUInt64(uint64_t def) const { return jsonGetUInt64(this, def); }
#endif /* !defined(TMJ_NO_INT64) */
#endif /* defined(__cplusplus) */

#endif  // _TM_JSON_H_INCLUDED_

// implementation
#ifdef TM_JSON_IMPLEMENTATION

// clang-format off
#ifndef TM_MIN
    #define TM_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */
// clang-format on

static tm_bool stringEquals(const char* a, size_t aSize, const char* b, size_t bSize) {
    TM_ASSERT(bSize > 0);
    if (aSize != bSize) return TM_FALSE;
    return TM_MEMCMP(a, b, bSize) == 0;
}
static tm_bool stringEqualsIgnoreCase(const char* a, size_t aSize, const char* b, size_t bSize) {
    TM_ASSERT(bSize > 0);
    if (aSize != bSize) return TM_FALSE;
    do {
        // We know that b is already uppercased.
        if (TM_TOLOWER((unsigned char)*a) != (unsigned char)*b) return TM_FALSE;
        ++a;
        ++b;
        --bSize;
    } while (bSize);
    return TM_TRUE;
}

static tm_bool stringStartsWith(const char* a, size_t aSize, const char* b, size_t bSize) {
    TM_ASSERT(bSize > 0);
    if (aSize < bSize) return TM_FALSE;
    return TM_MEMCMP(a, b, bSize) == 0;
}

static tm_bool stringStartsWithIgnoreCase(const char* a, size_t aSize, const char* b, size_t bSize) {
    TM_ASSERT(bSize > 0);
    if (aSize < bSize) return TM_FALSE;
    return stringEqualsIgnoreCase(a, bSize, b, bSize);
}

#ifdef TMJ_DEFINE_OWN_STRING_CONVERSIONS

#include <stdlib.h>
#include <errno.h>

static void tmj_to_int(const char* first, const char* last, int* value, int base) {
    TM_ASSERT(value);
    TM_ASSERT(last >= first);
    size_t size = (size_t)(last - first);
    if (size > 32) return;
    char buffer[33];
    TM_MEMCPY(buffer, first, size);
    buffer[size] = 0;
    errno = 0;
    int result = (int)strtol(buffer, TM_NULL, base);
    if (errno == ERANGE) return;
    *value = result;
}
static void tmj_to_uint(const char* first, const char* last, unsigned int* value, int base) {
    TM_ASSERT(value);
    TM_ASSERT(last >= first);
    size_t size = (size_t)(last - first);
    if (size > 32) return;
    char buffer[33];
    TM_MEMCPY(buffer, first, size);
    buffer[size] = 0;
    errno = 0;
    unsigned int result = (unsigned int)strtoul(buffer, TM_NULL, base);
    if (errno == ERANGE) return;
    *value = result;
}
static tm_bool tmj_to_double(const char* first, const char* last, double* value) {
    TM_ASSERT(value);
    TM_ASSERT(last >= first);
    size_t size = (size_t)(last - first);
    if (size > 511) return TM_FALSE;
    char buffer[512];
    TM_MEMCPY(buffer, first, size);
    buffer[size] = 0;
    errno = 0;
    double result = strtod(buffer, TM_NULL);
    if (errno == ERANGE) return TM_FALSE;
    *value = result;
    return TM_TRUE;
}
static void tmj_to_float(const char* first, const char* last, float* value) {
    double val = 0;
    if (tmj_to_double(first, last, &val)) {
        *value = (float)val;
    }
}
#ifndef TMJ_NO_INT64
static void tmj_to_int64(const char* first, const char* last, int64_t* value, int base) {
    TM_ASSERT(value);
    TM_ASSERT(last >= first);
    size_t size = (size_t)(last - first);
    if (size > 64) return;
    char buffer[65];
    TM_MEMCPY(buffer, first, size);
    buffer[size] = 0;
    errno = 0;
    int64_t result = (int64_t)strtoll(buffer, TM_NULL, base);
    if (errno == ERANGE) return;
    *value = result;
}
static void tmj_to_uint64(const char* first, const char* last, uint64_t* value, int base) {
    TM_ASSERT(value);
    TM_ASSERT(last >= first);
    size_t size = (size_t)(last - first);
    if (size > 64) return;
    char buffer[65];
    TM_MEMCPY(buffer, first, size);
    buffer[size] = 0;
    errno = 0;
    uint64_t result =  (uint64_t)strtoull(buffer, TM_NULL, base);
    if (errno == ERANGE) return;
    *value = result;
}
#endif  // !defined(TMJ_NO_INT64)

#endif  // defined(TMJ_DEFINE_OWN_STRING_CONVERSIONS)

TMJ_DEF int32_t jsonToInt(JsonStringView str, int32_t def) {
    if (str.size <= 0) return def;
    if (str.size >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        if (str.size == 2) return def;
        TMJ_TO_INT(str.data + 2, str.data + str.size - 2, &def, 16);
    } else {
        TMJ_TO_INT(str.data, str.data + str.size, &def, 10);
    }
    return def;
}
TMJ_DEF uint32_t jsonToUInt(JsonStringView str, uint32_t def) {
    if (str.size <= 0) return def;
    if (str.data[0] == '-') {
        // Special case for -0.
        if (str.size == 2 && str.data[1] == '0') return 0;
        return def;
    }
    if (str.size >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        if (str.size == 2) return def;
        TMJ_TO_UINT(str.data + 2, str.data + str.size - 2, &def, 16);
    } else {
        TMJ_TO_UINT(str.data, str.data + str.size, &def, 10);
    }
    return def;
}
TMJ_DEF float jsonToFloat(JsonStringView str, float def) {
    if (str.size <= 0) return def;
#if defined(TM_INFINITY) || defined(TM_NAN)
    {
        // check for inf, nan etc
        tm_bool neg = TM_FALSE;
        JsonStringView str_ = str;
        if (str_.data[0] == '+') {
            ++str_.data;
            --str_.size;
        } else if (str_.data[0] == '-') {
            neg = TM_TRUE;
            ++str_.data;
            --str_.size;
        }
#if defined(TM_INFINITY)
        if (stringEqualsIgnoreCase(str_.data, str_.size, "infinity", 8)) {
            return (float)((neg) ? (-TM_INFINITY) : (TM_INFINITY));
        }
#endif
#if defined(TM_NAN)
        if (stringEqualsIgnoreCase(str_.data, str_.size, "nan", 3)) {
            return (float)((neg) ? (-TM_NAN) : (TM_NAN));
        }
#endif
    }
#endif
    TMJ_TO_FLOAT(str.data, str.data + str.size, &def);
    return def;
}
TMJ_DEF double jsonToDouble(JsonStringView str, double def) {
    if (str.size <= 0) return def;
#if defined(TM_INFINITY) || defined(TM_NAN)
    {
        // check for inf, nan etc
        tm_bool neg = TM_FALSE;
        JsonStringView str_ = str;
        if (str_.data[0] == '+') {
            ++str_.data;
            --str_.size;
        } else if (str_.data[0] == '-') {
            neg = TM_TRUE;
            ++str_.data;
            --str_.size;
        }
#if defined(TM_INFINITY)
        if (stringEqualsIgnoreCase(str_.data, str_.size, "infinity", 8)) {
            return (neg) ? (-TM_INFINITY) : (TM_INFINITY);
        }
#endif
#if defined(TM_NAN)
        if (stringEqualsIgnoreCase(str_.data, str_.size, "nan", 3)) {
            return (neg) ? (-TM_NAN) : (TM_NAN);
        }
#endif
    }
#endif
    TMJ_TO_DOUBLE(str.data, str.data + str.size, &def);
    return def;
}
TMJ_DEF tm_bool jsonToBool(JsonStringView str, tm_bool def) {
    if (str.size <= 0) return def;
    if (str.size == 1) {
        switch (str.data[0]) {
            case '0':
                return TM_FALSE;
            case '1':
                return TM_TRUE;
            default:
                return def;
        }
    }
    if (stringEqualsIgnoreCase(str.data, str.size, "true", 4)) {
        return TM_TRUE;
    } else if (stringEqualsIgnoreCase(str.data, str.size, "false", 5)) {
        return TM_FALSE;
    }
    return def;
}
#ifndef TMJ_NO_INT64
TMJ_DEF int64_t jsonToInt64(JsonStringView str, int64_t def) {
    if (str.size <= 0) return def;
    if (str.size >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        if (str.size == 2) return def;
        TMJ_TO_INT64(str.data + 2, str.data + str.size - 2, &def, 16);
    } else {
        TMJ_TO_INT64(str.data, str.data + str.size, &def, 10);
    }
    return def;
}
TMJ_DEF uint64_t jsonToUInt64(JsonStringView str, uint64_t def) {
    if (str.size <= 0) return def;
    if (str.data[0] == '-') {
        // Special case for -0.
        if (str.size == 2 && str.data[1] == '0') return 0;
        return def;
    }
    if (str.size >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        if (str.size == 2) return def;
        TMJ_TO_UINT64(str.data + 2, str.data + str.size - 2, &def, 16);
    } else {
        TMJ_TO_UINT64(str.data, str.data + str.size, &def, 10);
    }
    return def;
}
#endif

TMJ_DEF const char* jsonGetErrorString(JsonErrorType error) {
    TM_ASSERT(error >= 0 && error <= JERR_INTERNAL_ERROR);
    static const char* const ErrorStrings[] = {"Ok",
                                               "Unexpected Token",
                                               "Unexpected EOF",
                                               "Illformed String",
                                               "Mismatched Brackets",
                                               "Out Of Context Stack Memory",
                                               "No Root",
                                               "Out Of Memory",
                                               "Internal Error"};
    return ErrorStrings[error];
}

#define TMJ_ROOT_ENTERED (1u << 31)

static void jsonAdvance(JsonReader* reader) {
    ++reader->data;
    ++reader->column;
    --reader->size;
}
static tm_size_t skipWhitespaceHelper(JsonReader* reader, const char* whitespace, size_t len) {
    const char* p;
    while ((p = (const char*)TM_MEMCHR(whitespace, reader->data[0], len)) != TM_NULL) {
        if (reader->data[0] == '\n') {
            ++reader->line;
            reader->column = 0;
        } else {
            ++reader->column;
        }
        ++reader->data;
        --reader->size;
    }
    return reader->size;
}
static tm_size_t skipWhitespace(JsonReader* reader) {
    static const char* const whitespace = "\t\n\r ";
    return skipWhitespaceHelper(reader, whitespace, 4);
}
static tm_size_t skipWhitespaceEx(JsonReader* reader, tm_bool ex) {
    static const char* const json_whitespace = "\t\n\r ";
    /*
    Json5 considers these whitespace:
    U+0009  Horizontal tab
    U+000A  Line feed
    U+000B  Vertical tab
    U+000C  Form feed
    U+000D  Carriage return
    U+0020  Space
    U+00A0  Non-breaking space
    U+2028  Line separator
    U+2029  Paragraph separator
    U+FEFF  Byte order mark
    Unicode Zs category Any other character in the Space Separator Unicode category

    Currently Line separator (U+2028), Paragraph separator (U+2029), Byte order mark (U+FEFF)
    and Space Separator Unicode category are not supported, since whitespace skipping is not unicode based.
    */
    static const char* const json5_whitespace = "\x09\x0A\x0B\x0C\x0D\x20\xA0";

    const char* whitespace;
    size_t len;
    if ((ex && (reader->flags & JSON_READER_ALLOW_EXTENDED_WHITESPACE))) {
        whitespace = json5_whitespace;
        len = 7;
    } else {
        whitespace = json_whitespace;
        len = 4;
    }
    return skipWhitespaceHelper(reader, whitespace, len);
}
static void setError(JsonReader* reader, JsonErrorType error) {
    reader->errorType = error;
    reader->current.data = reader->data;
    reader->current.size = 1;
}

#define TMJ_MAX_UTF32 0x10FFFFu
#define TMJ_LEAD_SURROGATE_MIN 0xD800u
#define TMJ_LEAD_SURROGATE_MAX 0xDBFFu
#define TMJ_TRAILING_SURROGATE_MIN 0xDC00u
#define TMJ_TRAILING_SURROGATE_MAX 0xDFFFu
#define TMJ_SURROGATE_OFFSET (0x10000u - (0xD800u << 10u) - 0xDC00u)

static tm_bool tmj_is_valid_codepoint(uint32_t codepoint) {
    return codepoint <= TMJ_MAX_UTF32 && (codepoint < TMJ_LEAD_SURROGATE_MIN || codepoint > TMJ_TRAILING_SURROGATE_MAX);
}

static tm_size_t tmj_get_codepoint(const char* first, tm_size_t remaining, uint32_t* codepoint) {
    tm_size_t starting_size = remaining;

    TM_ASSERT(codepoint);
    if (remaining < 4) return 0;
    if (!TM_ISXDIGIT((unsigned char)first[0]) || !TM_ISXDIGIT((unsigned char)first[1]) ||
        !TM_ISXDIGIT((unsigned char)first[2]) || !TM_ISXDIGIT((unsigned char)first[3])) {
        return 0;
    }
    uint32_t lead = 0xFFFFFFFFu;
    TMJ_TO_UINT(first, first + 4, &lead, 16);
    if (lead == 0xFFFFFFFFu) return 0;
    first += 4;
    remaining -= 4;

    if (lead >= TMJ_LEAD_SURROGATE_MIN && lead <= TMJ_LEAD_SURROGATE_MAX) {
        if (remaining < 6) return 0;
        if (*first != '\\' || *(first + 1) != 'u') return 0;
        first += 2;
        remaining -= 2;
        if (!TM_ISXDIGIT((unsigned char)first[0]) || !TM_ISXDIGIT((unsigned char)first[1]) ||
            !TM_ISXDIGIT((unsigned char)first[2]) || !TM_ISXDIGIT((unsigned char)first[3])) {
            return 0;
        }
        uint32_t trail = 0xFFFFFFFFu;
        TMJ_TO_UINT(first, first + 4, &trail, 16);
        if (trail == 0xFFFFFFFFu) return 0;
        first += 4;
        remaining -= 4;

        if (trail >= TMJ_TRAILING_SURROGATE_MIN && trail <= TMJ_TRAILING_SURROGATE_MAX) {
            *codepoint = (lead << 10) + trail + TMJ_SURROGATE_OFFSET;
        } else {
            return 0;
        }
    } else {
        *codepoint = lead;
    }
    if (!tmj_is_valid_codepoint(*codepoint)) return 0;

    TM_ASSERT(starting_size > remaining);
    tm_size_t advance = starting_size - remaining;
    TM_ASSERT(advance == 4 || advance == 10);
    return advance;
}
static tm_size_t tmj_utf8_encode(uint32_t codepoint, char* out, tm_size_t out_len) {
    TM_ASSERT(out || out_len == 0);
    TM_ASSERT(tmj_is_valid_codepoint(codepoint));

    if (codepoint < 0x80) {
        /* 1 byte sequence */
        if (out_len < 1) return 1;
        out[0] = (char)(codepoint);
        return 1;
    } else if (codepoint < 0x800) {
        /* 2 byte sequence 110xxxxx 10xxxxxx */
        if (out_len < 2) return 2;
        out[0] = (char)(0xC0 | (uint8_t)(codepoint >> 6));
        out[1] = (char)(0x80 | (uint8_t)(codepoint & 0x3F));
        return 2;
    } else if (codepoint < 0x10000) {
        /* 3 byte sequence 1110xxxx 10xxxxxx 10xxxxxx */
        if (out_len < 3) return 3;
        out[0] = (char)(0xE0 | (uint8_t)(codepoint >> 12));
        out[1] = (char)(0x80 | ((uint8_t)(codepoint >> 6) & 0x3F));
        out[2] = (char)(0x80 | ((uint8_t)(codepoint & 0x3F)));
        return 3;
    } else {
        /* 4 byte sequence 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        if (out_len < 4) return 4;
        out[0] = (char)(0xF0 | ((uint8_t)(codepoint >> 18) & 0x7));
        out[1] = (char)(0x80 | ((uint8_t)(codepoint >> 12) & 0x3F));
        out[2] = (char)(0x80 | ((uint8_t)(codepoint >> 6) & 0x3F));
        out[3] = (char)(0x80 | ((uint8_t)(codepoint & 0x3F)));
        return 4;
    }
}
#undef TMJ_MAX_UTF32
#undef TMJ_LEAD_SURROGATE_MIN
#undef TMJ_LEAD_SURROGATE_MAX
#undef TMJ_TRAILING_SURROGATE_MIN
#undef TMJ_TRAILING_SURROGATE_MAX
#undef TMJ_SURROGATE_OFFSET


static tm_bool tmj_is_char_unescaped(const char* first, const char* last) {
    /* Count how many backslashes precede last. */
    const char* pos = last;
    while (pos > first && *(pos - 1) == '\\') --pos;
    TM_ASSERT(last >= pos);
    size_t preceding_escape_chars_count = (size_t)(last - pos);
    /* If there are even number of backslashes before last, they all escaped each other. */
    return (preceding_escape_chars_count & 1) == 0;
}

static tm_bool tmj_is_valid_json_string(const char* first, tm_size_t size, char quote, tm_bool allow_escaped_newlines,
                                        tm_bool ex) {
    for (tm_size_t i = 0; i < size; ++i) {
        unsigned char c = (unsigned char)first[i];
        /* Raw control characters are not valid json strings. */
        if (c < 32) return TM_FALSE;
        if (c == '\\') {
            tm_size_t remaining = size - i;
            if (!remaining) return TM_FALSE;
            unsigned char next = (unsigned char)first[i + 1];
            ++i;
            switch (next) {
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case 'u': {
                    break;
                }
                case '"': {
                    if (quote == '"') break;
                    return TM_FALSE;
                }
                case '\'': {
                    if (quote == '\'') break;
                    return TM_FALSE;
                }
                case '\n': {
                    if (allow_escaped_newlines) break;
                    return TM_FALSE;
                }
                case '\r': {
                    if (allow_escaped_newlines && ex) {
                        // Treat \r\n as a single newline by skipping one char.
                        if (remaining > 1 && first[i + 1] == '\n') ++i;
                        break;
                    }
                    return TM_FALSE;
                }
                default: {
                    return TM_FALSE;
                }
            }
            if (next == 'u') {
                ++i;
                /* Validate that \u is always followed by four hexadecimal digits. */
                uint32_t codepoint = 0;
                tm_size_t advance = tmj_get_codepoint(first + i, size - i, &codepoint);
                if (advance <= 0) return TM_FALSE;
                i += advance - 1;
            }
        }
    }
    return TM_TRUE;
}

static const char* tmj_find_char_unescaped(const char* first, tm_size_t size, char c) {
    TM_ASSERT(first || size == 0);

    if (size <= 0) return TM_NULL;

    const char* str = first;
    const char* last = first + size;
    if (*str == c) return str;
    for (;;) {
        ++str;
        str = (const char*)TM_MEMCHR(str, (unsigned char)c, (size_t)(last - str));
        if (!str) return TM_NULL;
        if (tmj_is_char_unescaped(first, str)) return str;
    }
}

static tm_bool readQuotedString(JsonReader* reader) {
    char quote = reader->data[0];
    TM_ASSERT(quote == '\'' || quote == '"');
    const char* start = reader->data;
    ++reader->data;
    --reader->size;
    reader->current.data = reader->data;
    reader->current.size = 0;

    /* TODO: Profile to see which is faster, memchr twice over the string or go byte by byte once. */
    const char* p = tmj_find_char_unescaped(reader->data, reader->size, quote);
    if (p) {
        reader->size -= (tm_size_t)(p + 1 - reader->data);
        reader->data = p + 1;
        /* Unescaped quotation mark, we found the string. */
        reader->current.size = (tm_size_t)(reader->data - reader->current.data - 1);
        int ex = reader->flags & JSON_READER_ALLOW_EXTENDED_WHITESPACE;
        if (reader->flags & JSON_READER_ESCAPED_MULTILINE_STRINGS) {
            const char* last = reader->current.data;
            tm_size_t size = reader->current.size;
            for (;;) {
                if (ex) {
                    p = (const char*)TM_MEMCHR(last, '\r', size);
                    if (!p) p = (const char*)TM_MEMCHR(last, '\n', size);
                } else {
                    p = (const char*)TM_MEMCHR(last, '\n', size);
                }
                if (!p) break;
                if (p == reader->current.data || tmj_is_char_unescaped(last, p)) {
                    reader->errorType = JERR_ILLFORMED_STRING;
                    reader->current.data = p;
                    reader->current.size = 1;
                    return TM_FALSE;
                }
                reader->column = 0;
                ++reader->line;
                if (ex && size > 1 && *p == '\r') {
                    /* Skip \r\n */
                    if (*(p + 1) == '\n') ++p;
                }
                size -= (tm_size_t)(p - last + 1);
                last = p + 1;
            }
            reader->column = size;
        } else {
            reader->column += reader->current.size + 1;
        }
        if (!tmj_is_valid_json_string(reader->current.data, reader->current.size, quote,
                                      (reader->flags & JSON_READER_ESCAPED_MULTILINE_STRINGS) != 0,
                                      reader->flags & JSON_READER_ALLOW_EXTENDED_WHITESPACE) != 0) {
            reader->errorType = JERR_ILLFORMED_STRING;
            reader->current.data = p;
            reader->current.size = 1;
            return TM_FALSE;
        }
        return TM_TRUE;
    }

    // reached eof without reading a string
    reader->errorType = JERR_UNEXPECTED_EOF;
    reader->data += reader->size;
    reader->size = 0;
    // set current to the expected token before reaching eof
    reader->current.data = start;
    reader->current.size = 1;
    return TM_FALSE;
}

static tm_bool readNumber(JsonReader* reader) {
    if (!reader->size) {
        setError(reader, JERR_UNEXPECTED_EOF);
        return TM_FALSE;
    }

    reader->valueType = JVAL_UINT;
    reader->lastToken = JTOK_VALUE;
    reader->current.data = reader->data;
    if (reader->data[0] == '-') {
        reader->valueType = JVAL_INT;
        jsonAdvance(reader);
    } else if ((reader->flags & JSON_READER_ALLOW_PLUS_SIGN) && reader->data[0] == '+') {
        jsonAdvance(reader);
        // By skipping the plus sign in the token string we can use the usual json string conversion functions.
        reader->current.data = reader->data;
    }

    if (!reader->size) {
        setError(reader, JERR_UNEXPECTED_EOF);
        return TM_FALSE;
    }

    int has_leading_digits = TM_FALSE;
    int has_decimal_point = TM_FALSE;
    int has_trailing_digits = TM_FALSE;

    if (reader->data[0] == '0') {
        has_leading_digits = TM_TRUE;
        jsonAdvance(reader);
        if (!reader->size) {
            reader->current.size = (tm_size_t)(reader->data - reader->current.data);
            return TM_TRUE;
        }

        // hexadecimal
        if ((reader->data[0] == 'x' || reader->data[0] == 'X') && (reader->flags & JSON_READER_HEXADECIMAL)) {
            jsonAdvance(reader);
            if (!reader->size) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return TM_FALSE;
            }
            if (!TM_ISXDIGIT((unsigned char)reader->data[0])) {
                setError(reader, JERR_UNEXPECTED_TOKEN);
                return TM_FALSE;
            }
            do {
                jsonAdvance(reader);
            } while (reader->size && TM_ISXDIGIT((unsigned char)reader->data[0]));
            reader->current.size = (tm_size_t)(reader->data - reader->current.data);
            return TM_TRUE;
        }
    } else if (TM_ISDIGIT((unsigned char)reader->data[0])) {
        has_leading_digits = TM_TRUE;
        do {
            jsonAdvance(reader);
            if (!reader->size) return TM_TRUE;
        } while (TM_ISDIGIT((unsigned char)reader->data[0]));
    } else if (!(reader->flags & JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT) || reader->data[0] != '.') {
        setError(reader, JERR_UNEXPECTED_TOKEN);
        return TM_FALSE;
    }

    if (reader->size && reader->data[0] == '.') {
        has_decimal_point = TM_TRUE;
        reader->valueType = JVAL_FLOAT;
        jsonAdvance(reader);
        if (!reader->size) {
            if (reader->flags & JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT) return TM_TRUE;
            setError(reader, JERR_UNEXPECTED_EOF);
            return TM_FALSE;
        }
        // At least one digit has to follow after '.' unless explicitly allowed.
        has_trailing_digits = (TM_ISDIGIT((unsigned char)reader->data[0]) != 0);
        if (!has_trailing_digits) {
            if (!(reader->flags & JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT)) {
                setError(reader, JERR_UNEXPECTED_TOKEN);
                return TM_FALSE;
            }
        } else {
            do {
                jsonAdvance(reader);
            } while (reader->size && TM_ISDIGIT((unsigned char)reader->data[0]));
        }
    }

    if (reader->size && (reader->data[0] == 'e' || reader->data[0] == 'E')) {
        has_trailing_digits = TM_TRUE;
        if (!has_decimal_point && !has_leading_digits) {
            setError(reader, JERR_UNEXPECTED_TOKEN);
            return TM_FALSE;
        }
        reader->valueType = JVAL_FLOAT;
        jsonAdvance(reader);
        if (reader->data[0] == '+' || reader->data[0] == '-') {
            jsonAdvance(reader);
        }
        if (!TM_ISDIGIT((unsigned char)reader->data[0])) {
            setError(reader, JERR_UNEXPECTED_TOKEN);
            return TM_FALSE;
        }
        do {
            jsonAdvance(reader);
        } while (reader->size && TM_ISDIGIT((unsigned char)reader->data[0]));
    }

    if (!has_leading_digits && !has_trailing_digits && has_decimal_point) {
        /* Unadvance to decimal point. */
        --reader->data;
        --reader->column;
        ++reader->size;
        TM_ASSERT(*reader->data == '.');
        setError(reader, JERR_UNEXPECTED_TOKEN);
        return TM_FALSE;
    }

    reader->current.size = (tm_size_t)(reader->data - reader->current.data);
    return TM_TRUE;
}
static JsonTokenType advanceValue(JsonReader* reader, tm_size_t size) {
    reader->current.data = reader->data;
    reader->current.size = size;
    reader->data += size;
    reader->size -= size;
    reader->lastToken = JTOK_VALUE;
    return JTOK_VALUE;
}
static tm_bool readNumberEx(JsonReader* reader) {
    if (reader->flags & JSON_READER_EXTENDED_FLOATS) {
        if (reader->size) {
            const char* start = reader->data;
            tm_size_t size = reader->size;
            int offset = 0;
            if (*start == '-') {
                ++start;
                --size;
                offset = 1;
            } else if (*start == '+' && (reader->flags & JSON_READER_ALLOW_PLUS_SIGN)) {
                ++start;
                --size;
                offset = 1;
            }
            if (reader->flags & JSON_READER_IGNORE_CASE_KEYWORDS) {
                if (stringStartsWithIgnoreCase(start, size, "infinity", 8)) {
                    reader->valueType = JVAL_FLOAT;
                    advanceValue(reader, 8 + offset);
                    return TM_TRUE;
                } else if (stringStartsWithIgnoreCase(start, size, "nan", 3)) {
                    reader->valueType = JVAL_FLOAT;
                    advanceValue(reader, 3 + offset);
                    return TM_TRUE;
                }
            } else {
                if (stringStartsWith(start, size, "Infinity", 8)) {
                    reader->valueType = JVAL_FLOAT;
                    advanceValue(reader, 8 + offset);
                    return TM_TRUE;
                } else if (stringStartsWith(start, size, "NaN", 3)) {
                    reader->valueType = JVAL_FLOAT;
                    advanceValue(reader, 3 + offset);
                    return TM_TRUE;
                }
            }
        }
    }
    return readNumber(reader);
}
static JsonTokenType readValue(JsonReader* reader) {
    if (stringStartsWith(reader->data, reader->size, "true", 4)) {
        reader->valueType = JVAL_BOOL;
        return advanceValue(reader, 4);
    } else if (stringStartsWith(reader->data, reader->size, "false", 5)) {
        reader->valueType = JVAL_BOOL;
        return advanceValue(reader, 5);
    } else if (stringStartsWith(reader->data, reader->size, "null", 4)) {
        reader->valueType = JVAL_NULL;
        return advanceValue(reader, 4);
    } else if (readNumber(reader)) {
        return reader->lastToken;
    } else {
        setError(reader, JERR_UNEXPECTED_TOKEN);
        return JTOK_ERROR;
    }
}
static JsonTokenType readValueEx(JsonReader* reader) {
    if (reader->flags & JSON_READER_IGNORE_CASE_KEYWORDS) {
        if (stringStartsWithIgnoreCase(reader->data, reader->size, "true", 4)) {
            reader->valueType = JVAL_BOOL;
            return advanceValue(reader, 4);
        } else if (stringStartsWithIgnoreCase(reader->data, reader->size, "false", 5)) {
            reader->valueType = JVAL_BOOL;
            return advanceValue(reader, 5);
        } else if (stringStartsWithIgnoreCase(reader->data, reader->size, "null", 4)) {
            reader->valueType = JVAL_NULL;
            return advanceValue(reader, 4);
        }
    } else {
        if (stringStartsWith(reader->data, reader->size, "true", 4)) {
            reader->valueType = JVAL_BOOL;
            return advanceValue(reader, 4);
        } else if (stringStartsWith(reader->data, reader->size, "false", 5)) {
            reader->valueType = JVAL_BOOL;
            return advanceValue(reader, 5);
        } else if (stringStartsWith(reader->data, reader->size, "null", 4)) {
            reader->valueType = JVAL_NULL;
            return advanceValue(reader, 4);
        }
    }
    if (readNumberEx(reader)) {
        return reader->lastToken;
    } else {
        setError(reader, JERR_UNEXPECTED_TOKEN);
        return JTOK_ERROR;
    }
}

static tm_bool jsonPushContext(JsonReader* reader, JsonContext context) {
    JsonContextStack* stack = &reader->contextStack;
    if (stack->size && (JsonContext)stack->data[stack->size - 1].context == context) {
        if (stack->data[stack->size - 1].count < TMJ_MAX_NESTING_COUNT) {
            ++stack->data[stack->size - 1].count;
            return TM_TRUE;
        }
    }

    if (stack->size < stack->capacity) {
        JsonContextEntry* entry = &stack->data[stack->size++];
        entry->context = (int8_t)context;
        entry->count = 1;
        return TM_TRUE;
    } else {
        setError(reader, JERR_OUT_OF_CONTEXT_STACK_MEMORY);
        return TM_FALSE;
    }
}
static tm_bool jsonPopContext(JsonReader* reader, JsonContext context) {
    JsonContextStack* stack = &reader->contextStack;
    if (!stack->size || (JsonContext)stack->data[stack->size - 1].context != context) {
        setError(reader, JERR_MISMATCHED_BRACKETS);
        return TM_FALSE;
    }
    if (stack->data[stack->size - 1].count > 1) {
        --stack->data[stack->size - 1].count;
    } else {
        TM_ASSERT(stack->data[stack->size - 1].count == 1);
        --stack->size;
    }
    return TM_TRUE;
}
inline static JsonContext jsonCurrentContext(JsonReader* reader) {
    TM_ASSERT(reader->contextStack.size);
    return (JsonContext)reader->contextStack.data[reader->contextStack.size - 1].context;
}
static tm_bool jsonCanValueFollowLastToken(JsonReader* reader, JsonContext currentContext) {
    /* Check whether a value can follow last token in current context. */
    switch (currentContext) {
        case JSON_CONTEXT_NULL: {
            // A value can only exist at root level if no other value was parsed already.
            return (reader->flags & TMJ_ROOT_ENTERED) == 0;
        }
        case JSON_CONTEXT_OBJECT: {
            if (reader->lastToken != JTOK_PROPERTYCOLON) return TM_FALSE;
            break;
        }
        case JSON_CONTEXT_ARRAY: {
            if (reader->lastToken != JTOK_ARRAY_START && reader->lastToken != JTOK_COMMA) return TM_FALSE;
            break;
        }
        default: {
            return TM_FALSE;
        }
    }
    return TM_TRUE;
}
static tm_bool jsonCanEndBracketFollowLastToken(JsonReader* reader, JsonContext currentContext,
                                                 JsonContext endingContext) {
    if (currentContext == JSON_CONTEXT_NULL || currentContext != endingContext) {
        return TM_FALSE;
    }
    switch (reader->lastToken) {
        case JTOK_COMMA: {
            if (!(reader->flags & JSON_READER_TRAILING_COMMA)) {
                return TM_FALSE;
            }
            break;
        }
        case JTOK_PROPERTYNAME:
        case JTOK_PROPERTYCOLON: {
            return TM_FALSE;
        }
        default: {
            break;
        }
    }
    return TM_TRUE;
}
inline static tm_bool jsonCompareCurrentContextTo(JsonReader* reader, int8_t context) {
    return reader->contextStack.size && reader->contextStack.data[reader->contextStack.size - 1].context == context;
}
static void jsonReadCommentLine(JsonReader* reader) {
    reader->current.data = reader->data;
    const char* p = (const char*)TM_MEMCHR(reader->data, '\n', reader->size);
    int has_newline = (p != TM_NULL);
    if (!has_newline) p = reader->data + reader->size;
    if (!has_newline && (reader->flags & JSON_READER_ALLOW_EXTENDED_WHITESPACE)) {
        const char* alt = (const char*)TM_MEMCHR(reader->data, '\r', reader->size);
        if (alt) {
            p = alt;
            has_newline = TM_TRUE;
        }
    }
    reader->current.size = (tm_size_t)(p - reader->data);
    reader->column = 0;
    ++reader->line;
    reader->data += reader->current.size + has_newline;
    reader->size -= reader->current.size + has_newline;
}
static void jsonCountNewlines(JsonReader* reader) {
    tm_size_t newlines = 0;
    const char* p;
    const char* last = reader->current.data;
    tm_size_t size = reader->current.size;
    while ((p = (const char*)TM_MEMCHR(last, '\n', size)) != TM_NULL) {
        ++newlines;
        size -= (tm_size_t)(p - last + 1);
        last = p + 1;
    }
    if (newlines) {
        reader->column = size;
        reader->line += newlines;
    } else {
        reader->column += size;
    }
}
static JsonTokenType jsonParseSingleLineComment(JsonReader* reader) {
    if (reader->flags & JSON_READER_SINGLE_LINE_COMMENTS) {
        jsonAdvance(reader);
        if (!reader->size) {
            setError(reader, JERR_UNEXPECTED_EOF);
            return JTOK_ERROR;
        }
        if (reader->data[0] == '/') {
            jsonAdvance(reader);
            jsonReadCommentLine(reader);
            // do not alter reader->lastToken, comments have no follow
            return JTOK_COMMENT;
        }
    }
    setError(reader, JERR_UNEXPECTED_TOKEN);
    return JTOK_ERROR;
}
static JsonTokenType jsonParsePythonComment(JsonReader* reader) {
    if (reader->flags & JSON_READER_PYTHON_COMMENTS) {
        if (reader->data[0] == '#') {
            jsonAdvance(reader);
            jsonReadCommentLine(reader);
            // do not alter reader->lastToken, comments have no follow
            return JTOK_COMMENT;
        }
    }
    setError(reader, JERR_UNEXPECTED_TOKEN);
    return JTOK_ERROR;
}
static JsonTokenType jsonParseBlockComment(JsonReader* reader) {
    if (reader->flags & JSON_READER_BLOCK_COMMENTS) {
        jsonAdvance(reader);
        if (!reader->size) {
            setError(reader, JERR_UNEXPECTED_EOF);
            return JTOK_ERROR;
        }
        if (reader->data[0] == '*') {
            jsonAdvance(reader);
            if (!reader->size) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return JTOK_ERROR;
            }
            reader->current.data = reader->data;
            reader->current.size = 0;
            const char* p;
            while ((p = (const char*)TM_MEMCHR(reader->data, '*', reader->size)) != TM_NULL) {
                tm_size_t diff = (tm_size_t)(p - reader->data);
                reader->current.size += diff;
                reader->size -= diff + 1;
                reader->data = p + 1;
                if (!reader->size) {
                    setError(reader, JERR_UNEXPECTED_EOF);
                    return JTOK_ERROR;
                }
                if (*(p + 1) == '/') {
                    reader->data += 1;
                    reader->size -= 1;
                    break;
                }
                reader->current.size++;
            }
            jsonCountNewlines(reader);
            reader->column += 2;  // count */ too
            // do not alter reader->lastToken, comments have no follow
            return JTOK_COMMENT;
        }
    }
    setError(reader, JERR_UNEXPECTED_TOKEN);
    return JTOK_ERROR;
}
static JsonTokenType jsonParseComment(JsonReader* reader) {
    if ((reader->flags & JSON_READER_SINGLE_LINE_COMMENTS) || (reader->flags & JSON_READER_BLOCK_COMMENTS)) {
        if (reader->size < 2) {
            setError(reader, JERR_UNEXPECTED_EOF);
            return JTOK_ERROR;
        }
        switch (reader->data[1]) {
            case '/': {
                return jsonParseSingleLineComment(reader);
            }
            case '*': {
                return jsonParseBlockComment(reader);
            }
        }
    }
    setError(reader, JERR_UNEXPECTED_TOKEN);
    return JTOK_ERROR;
}
static JsonTokenType jsonParseQuotedString(JsonReader* reader, JsonContext currentContext) {
    if (!readQuotedString(reader)) {
        return JTOK_ERROR;
    }
    JsonTokenType result = JTOK_VALUE;
    reader->valueType = JVAL_STRING;
    if (currentContext == JSON_CONTEXT_NULL) {
        reader->lastToken = result;
        return result;
    }
    if (currentContext == JSON_CONTEXT_OBJECT &&
        (reader->lastToken == JTOK_OBJECT_START || reader->lastToken == JTOK_COMMA)) {
        result = JTOK_PROPERTYNAME;
    } else {
        if (!jsonCanValueFollowLastToken(reader, currentContext)) {
            setError(reader, JERR_UNEXPECTED_TOKEN);
            return JTOK_ERROR;
        }
        result = JTOK_VALUE;
    }
    reader->lastToken = result;
    return result;
}
static JsonTokenType jsonParseContextStart(JsonReader* reader, JsonContext currentContext, tm_bool isObject) {
    if (currentContext != JSON_CONTEXT_NULL) {
        if (!jsonCanValueFollowLastToken(reader, currentContext)) {
            setError(reader, JERR_UNEXPECTED_TOKEN);
            return JTOK_ERROR;
        }
    } else {
        if (reader->flags & TMJ_ROOT_ENTERED) {
            setError(reader, JERR_UNEXPECTED_TOKEN);
            return JTOK_ERROR;
        }
        reader->flags |= TMJ_ROOT_ENTERED;
    }
    JsonTokenType token = (isObject) ? (JTOK_OBJECT_START) : (JTOK_ARRAY_START);
    jsonAdvance(reader);
    reader->lastToken = token;
    return token;
}
static JsonTokenType jsonParseContextEnd(JsonReader* reader, JsonContext currentContext, tm_bool isObject) {
    JsonContext context = (isObject) ? (JSON_CONTEXT_OBJECT) : (JSON_CONTEXT_ARRAY);
    if (!jsonCanEndBracketFollowLastToken(reader, currentContext, context)) {
        setError(reader, JERR_UNEXPECTED_TOKEN);
        return JTOK_ERROR;
    }
    JsonTokenType token = (isObject) ? (JTOK_OBJECT_END) : (JTOK_ARRAY_END);
    jsonAdvance(reader);
    reader->lastToken = token;
    return token;
}

TMJ_DEF JsonTokenType jsonNextTokenImplicit(JsonReader* reader, JsonContext currentContext) {
    TM_ASSERT(reader);
    TM_ASSERT(reader->data);

    for (;;) {
        if (!skipWhitespace(reader)) {
            if (currentContext != JSON_CONTEXT_NULL) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return JTOK_ERROR;
            }
            reader->lastToken = JTOK_EOF;
            return JTOK_EOF;
        }
        char c = reader->data[0];
        switch (c) {
            case '/': {
                return jsonParseSingleLineComment(reader);
            }
            case '\'': {
                if (!(reader->flags & JSON_READER_SINGLE_QUOTED_STRINGS)) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }
                // fallthrough into '"' case
            }
            // fallthrough
            case '"': {
                return jsonParseQuotedString(reader, currentContext);
            }
            case '{':
            case '[': {
                return jsonParseContextStart(reader, currentContext, c == '{');
            }
            case '}':
            case ']': {
                return jsonParseContextEnd(reader, currentContext, c == '}');
            }
            case ':':
            case '=': {
                if (currentContext != JSON_CONTEXT_OBJECT || reader->lastToken != JTOK_PROPERTYNAME ||
                    (c == '=' && !(reader->flags & JSON_READER_ALLOW_EQUAL))) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }
                jsonAdvance(reader);
                reader->lastToken = JTOK_PROPERTYCOLON;
                break;
            }
            case ',': {
                if (currentContext == JSON_CONTEXT_NULL) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }
                switch (reader->lastToken) {
                    case JTOK_OBJECT_END:
                    case JTOK_ARRAY_END:
                    case JTOK_VALUE: {
                        break;
                    }
                    default: {
                        setError(reader, JERR_UNEXPECTED_TOKEN);
                        return JTOK_ERROR;
                    }
                }
                jsonAdvance(reader);
                reader->lastToken = JTOK_COMMA;
                break;
            }
            default: {
                // json value case
                if (!jsonCanValueFollowLastToken(reader, currentContext)) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }

                reader->flags |= TMJ_ROOT_ENTERED;
                return readValue(reader);
            }
        }
    }
}

TMJ_DEF tm_bool jsonIsValidUntilEof(JsonReader* reader) {
    if (!reader->size || reader->lastToken == JTOK_EOF) {
        return TM_TRUE;
    }
    JsonTokenType token;
    for (;;) {
        const char* start_data = reader->data;
        tm_size_t start_size = reader->size;
        token = jsonNextTokenImplicit(reader, JSON_CONTEXT_NULL);
        switch (token) {
            case JTOK_COMMENT: {
                break;
            }
            case JTOK_EOF: {
                return TM_TRUE;
            }
            default: {
                if (reader->errorType == JSON_OK) {
                    reader->data = start_data;
                    reader->size = start_size;
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                }
                return TM_FALSE;
            }
        }
    }
}

TMJ_DEF tm_bool jsonIsValidUntilEofEx(JsonReader* reader) {
    if (!reader->size || reader->lastToken == JTOK_EOF) {
        return TM_TRUE;
    }
    JsonTokenType token;
    for (;;) {
        token = jsonNextTokenImplicitEx(reader, JSON_CONTEXT_NULL);
        switch (token) {
            case JTOK_COMMENT: {
                break;
            }
            case JTOK_EOF: {
                return TM_TRUE;
            }
            default: {
                return TM_FALSE;
            }
        }
    }
}

TMJ_DEF JsonTokenType jsonNextToken(JsonReader* reader) {
    JsonContext current;
    if (reader->contextStack.size) {
        current = (JsonContext)reader->contextStack.data[reader->contextStack.size - 1].context;
    } else {
        current = JSON_CONTEXT_NULL;
    }
    JsonTokenType token = jsonNextTokenImplicit(reader, current);
    switch (token) {
        case JTOK_OBJECT_START:
        case JTOK_ARRAY_START: {
            JsonContext context = (JsonContext)(token - JTOK_OBJECT_START + JSON_CONTEXT_OBJECT);
            if (!jsonPushContext(reader, context)) {
                return JTOK_ERROR;
            }
            break;
        }
        case JTOK_OBJECT_END:
        case JTOK_ARRAY_END: {
            JsonContext context = (JsonContext)(token - JTOK_OBJECT_END + JSON_CONTEXT_OBJECT);
            if (!jsonPopContext(reader, context)) {
                return JTOK_ERROR;
            }
            break;
        }
        default: {
            break;
        }
    }
    return token;
}

static tm_bool jsonParseUnquotedPropertyName(JsonReader* reader, JsonContext currentContext) {
    if (currentContext == JSON_CONTEXT_OBJECT && (reader->flags & JSON_READER_UNQUOTED_PROPERTY_NAMES) &&
        reader->lastToken != JTOK_PROPERTYCOLON) {
        switch (reader->lastToken) {
            case JTOK_OBJECT_START:
            case JTOK_COMMA: {
                break;
            }
            default: {
                setError(reader, JERR_UNEXPECTED_TOKEN);
                return TM_FALSE;
            }
        }
        // parse identifier
        reader->current.data = reader->data;
        reader->current.size = 0;
        if (TM_ISDIGIT((unsigned char)reader->data[0]) ||
            (!TM_ISALPHA((unsigned char)reader->data[0]) && reader->data[0] != '_' && reader->data[0] != '$')) {
            return TM_FALSE;
        }
        jsonAdvance(reader);
        while (reader->size &&
               (TM_ISDIGIT((unsigned char)reader->data[0]) || TM_ISALPHA((unsigned char)reader->data[0]) ||
                reader->data[0] == '_' || reader->data[0] == '$')) {
            jsonAdvance(reader);
        }
        reader->current.size = (tm_size_t)(reader->data - reader->current.data);
        reader->lastToken = JTOK_PROPERTYNAME;
        return TM_TRUE;
    }
    return TM_FALSE;
}

static tm_bool jsonParsePythonRawString(JsonReader* reader, JsonContext currentContext) {
    if ((currentContext == JSON_CONTEXT_ARRAY || reader->lastToken == JTOK_PROPERTYCOLON) &&
        (reader->flags & JSON_READER_PYTHON_RAW_STRINGS)) {
        if (reader->data[0] == 'r' || reader->data[0] == 'R') {
            jsonAdvance(reader);
            if (!reader->size) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return TM_FALSE;
            }
            if ((reader->data[0] == '"' || reader->data[0] == '\'') && readQuotedString(reader)) {
                reader->lastToken = JTOK_VALUE;
                reader->valueType = JVAL_RAW_STRING;
                return TM_TRUE;
            } else {
                setError(reader, JERR_UNEXPECTED_TOKEN);
                return TM_FALSE;
            }
        }
    }
    return TM_FALSE;
}
inline static tm_bool jsonIsValidDelimChar(char c) {
    return c != ')' && c != '(' && c != '\\' && !TM_ISSPACE((unsigned char)c);
}
static tm_bool jsonParseCppRawString(JsonReader* reader, JsonContext currentContext) {
    if ((currentContext == JSON_CONTEXT_ARRAY || reader->lastToken == JTOK_PROPERTYCOLON) &&
        (reader->flags & JSON_READER_CPP_RAW_STRINGS)) {
        JsonReader stateGuard = *reader;

        if (reader->data[0] == 'r' || reader->data[0] == 'R') {
            jsonAdvance(reader);
            if (!reader->size) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return TM_FALSE;
            }
            if (reader->data[0] == '"') {
                jsonAdvance(reader);
                if (!reader->size) {
                    setError(reader, JERR_UNEXPECTED_EOF);
                    return TM_FALSE;
                }
                // get delimiter
                char delim[17];
                tm_size_t delimSize = 0;
                while (reader->size && jsonIsValidDelimChar(reader->data[0]) && delimSize < 16) {
                    jsonAdvance(reader);
                    delim[delimSize] = reader->data[0];
                    ++delimSize;
                }
                if (reader->size && reader->data[0] == '(') {
                    // append '"' to delimiter so that we do not need to check ending quotation mark
                    delim[delimSize] = '"';
                    ++delimSize;

                    jsonAdvance(reader);
                    if (!reader->size) {
                        setError(reader, JERR_UNEXPECTED_EOF);
                        return TM_FALSE;
                    }

                    reader->current.data = reader->data;
                    reader->current.size = 0;
                    const char* p;
                    while ((p = (const char*)TM_MEMCHR(reader->data, ')', reader->size)) != TM_NULL) {
                        reader->size -= (tm_size_t)(p - reader->data + 1);
                        reader->data = p + 1;
                        if (reader->size < delimSize) {
                            setError(reader, JERR_UNEXPECTED_EOF);
                            return TM_FALSE;
                        }
                        if (stringEquals(p + 1, delimSize, delim, delimSize)) {
                            reader->current.size = (tm_size_t)(p - reader->current.data);
                            reader->size -= delimSize;
                            reader->data += delimSize;
                            break;
                        }
                    }
                    if (!p) {
                        setError(reader, JERR_UNEXPECTED_EOF);
                        return TM_FALSE;
                    }
                    // parse raw string
                    reader->lastToken = JTOK_VALUE;
                    reader->valueType = JVAL_RAW_STRING;
                    return TM_TRUE;
                }
            }
        }

        // undo any advances we did
        *reader = stateGuard;
    }
    // string isn't cpp style raw string
    return TM_FALSE;
}

static JsonTokenType jsonParseQuotedStringEx(JsonReader* reader, JsonContext currentContext) {
    char quot = reader->data[0];
    JsonTokenType token = jsonParseQuotedString(reader, currentContext);
    JsonStringView current = reader->current;
    tm_bool allowConcatenated =
        (reader->flags & JSON_READER_CONCATENATED_STRINGS) && currentContext == JSON_CONTEXT_OBJECT;
    tm_bool allowConcatenatedArray =
        (reader->flags & JSON_READER_CONCATENATED_STRINGS_IN_ARRAYS) && currentContext == JSON_CONTEXT_ARRAY;
    if (token == JTOK_VALUE && (allowConcatenated || allowConcatenatedArray)) {
        do {
            if (!skipWhitespaceEx(reader, /*ex=*/TM_TRUE)) break;
            TM_ASSERT(reader->size);
            JsonReader stateGuard = *reader;
            if (reader->data[0] == quot && readQuotedString(reader)) {
                current.size = (tm_size_t)(reader->current.data + reader->current.size - current.data);
                reader->valueType = JVAL_CONCAT_STRING;
            } else {
                *reader = stateGuard;
                break;
            }
        } while (reader->data[0] == quot);
        if (reader->valueType == JVAL_CONCAT_STRING) {
            // we include the first quotation mark for concatenated strings, so that
            // jsonCopyConcatenatedString knows which quotation mark to use when concatenating
            --current.data;
            ++current.size;
        }
        reader->current = current;
    }
    return token;
}

TMJ_DEF JsonTokenType jsonNextTokenImplicitEx(JsonReader* reader, JsonContext currentContext) {
    TM_ASSERT(reader);
    TM_ASSERT(reader->data);

    for (;;) {
        if (!skipWhitespaceEx(reader, /*ex=*/TM_TRUE)) {
            if (currentContext != JSON_CONTEXT_NULL) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return JTOK_ERROR;
            }
            reader->lastToken = JTOK_EOF;
            return JTOK_EOF;
        }
        char c = reader->data[0];
        switch (c) {
            case '/': {
                return jsonParseComment(reader);
            }
            case '#': {
                return jsonParsePythonComment(reader);
            }
            case '\'': {
                if (!(reader->flags & JSON_READER_SINGLE_QUOTED_STRINGS)) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }
                // fallthrough into '"' case
            }
            // fallthrough
            case '"': {
                return jsonParseQuotedStringEx(reader, currentContext);
            }
            case '{':
            case '[': {
                return jsonParseContextStart(reader, currentContext, c == '{');
            }
            case '}':
            case ']': {
                return jsonParseContextEnd(reader, currentContext, c == '}');
            }
            case ':':
            case '=': {
                if (currentContext != JSON_CONTEXT_OBJECT || reader->lastToken != JTOK_PROPERTYNAME ||
                    (c == '=' && !(reader->flags & JSON_READER_ALLOW_EQUAL))) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }
                jsonAdvance(reader);
                reader->lastToken = JTOK_PROPERTYCOLON;
                break;
            }
            case ',': {
                if (currentContext == JSON_CONTEXT_NULL) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }
                switch (reader->lastToken) {
                    case JTOK_OBJECT_END:
                    case JTOK_ARRAY_END:
                    case JTOK_VALUE: {
                        break;
                    }
                    default: {
                        setError(reader, JERR_UNEXPECTED_TOKEN);
                        return JTOK_ERROR;
                    }
                }
                jsonAdvance(reader);
                reader->lastToken = JTOK_COMMA;
                break;
            }
            default: {
                // Unqoted property name case.
                if (jsonParseUnquotedPropertyName(reader, currentContext)) return JTOK_PROPERTYNAME;
                // Check raw string cases.
                switch (c) {
                    case 'r':
                    case 'R': {
                        if (jsonParseCppRawString(reader, currentContext)) return reader->lastToken;
                        if (reader->errorType != JSON_OK) return JTOK_ERROR;
                        if (jsonParsePythonRawString(reader, currentContext)) return reader->lastToken;
                        if (reader->errorType != JSON_OK) return JTOK_ERROR;
                        break;
                    }
                }
                if (reader->errorType != JSON_OK) return JTOK_ERROR;

                // Json value case.
                if (!jsonCanValueFollowLastToken(reader, currentContext)) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }
                reader->flags |= TMJ_ROOT_ENTERED;
                return readValueEx(reader);
            }
        }
    }
}
TMJ_DEF JsonTokenType jsonNextTokenEx(JsonReader* reader) {
    JsonContext current;
    if (reader->contextStack.size) {
        current = (JsonContext)reader->contextStack.data[reader->contextStack.size - 1].context;
    } else {
        current = JSON_CONTEXT_NULL;
    }
    JsonTokenType token = jsonNextTokenImplicitEx(reader, current);
    switch (token) {
        case JTOK_OBJECT_START:
        case JTOK_ARRAY_START: {
            JsonContext context = (JsonContext)(token - JTOK_OBJECT_START + JSON_CONTEXT_OBJECT);
            if (!jsonPushContext(reader, context)) return JTOK_ERROR;
            break;
        }
        case JTOK_OBJECT_END:
        case JTOK_ARRAY_END: {
            JsonContext context = (JsonContext)(token - JTOK_OBJECT_END + JSON_CONTEXT_OBJECT);
            if (!jsonPopContext(reader, context)) return JTOK_ERROR;
            break;
        }
        default: {
            break;
        }
    }
    return token;
}

TMJ_DEF JsonTokenType jsonReadRootType(JsonReader* reader, tm_bool ex) {
    for (;;) {
        JsonTokenType token;
        if (ex) {
            token = jsonNextTokenImplicitEx(reader, JSON_CONTEXT_NULL);
        } else {
            token = jsonNextTokenImplicit(reader, JSON_CONTEXT_NULL);
        }
        switch (token) {
            case JTOK_OBJECT_START:
            case JTOK_ARRAY_START:
            case JTOK_VALUE: {
                return token;
            }
            case JTOK_COMMENT: {
                continue;
            }
            default: {
                setError(reader, JERR_UNEXPECTED_TOKEN);
                return JTOK_ERROR;
            }
        }
    }
}

TMJ_DEF tm_bool jsonSkipCurrent(JsonReader* reader, JsonContext currentContext, tm_bool ex) {
    if (reader->errorType != JSON_OK) return TM_FALSE;
    if (!reader->size) return TM_FALSE;
    if (reader->lastToken != JTOK_OBJECT_START && reader->lastToken != JTOK_ARRAY_START &&
        reader->lastToken != JTOK_PROPERTYNAME) {
        return TM_FALSE;
    }

    if (reader->lastToken == JTOK_PROPERTYNAME) {
        // skip ahead to content
        int run = 1;
        do {
            JsonTokenType token;
            if (ex) {
                token = jsonNextTokenImplicitEx(reader, currentContext);
            } else {
                token = jsonNextTokenImplicit(reader, currentContext);
            }
            switch (token) {
                case JTOK_OBJECT_START: {
                    run = 0;
                    currentContext = JSON_CONTEXT_OBJECT;
                    break;
                }
                case JTOK_ARRAY_START: {
                    run = 0;
                    currentContext = JSON_CONTEXT_ARRAY;
                    break;
                }
                case JTOK_VALUE: {
                    return TM_TRUE;
                }
                case JTOK_COMMENT: {
                    break;
                }
                default: {
                    if (reader->errorType != JSON_OK) {
                        return TM_FALSE;
                    }
                    TM_ASSERT(0 && "invalid code path");
                    return TM_FALSE;
                }
            }
        } while (run);
        if (reader->errorType != JSON_OK) return TM_FALSE;
    }
    TM_ASSERT(reader->lastToken == JTOK_OBJECT_START || reader->lastToken == JTOK_ARRAY_START);
    JsonTokenType skipping = reader->lastToken;

    unsigned int depth = 1;

    while (reader->size) {
        if (!skipWhitespaceEx(reader, ex)) {
            if (currentContext != JSON_CONTEXT_NULL) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return TM_FALSE;
            }
            reader->lastToken = JTOK_EOF;
            return JTOK_EOF;
        }
        char c = reader->data[0];
        jsonAdvance(reader);
        switch (c) {
            case '{': {
                if (skipping == JTOK_OBJECT_START) ++depth;
                break;
            }
            case '}': {
                if (skipping == JTOK_OBJECT_START) {
                    --depth;
                    if (depth == 0) {
                        reader->lastToken = JTOK_OBJECT_END;
                        return TM_TRUE;
                    }
                }
                break;
            }
            case '[': {
                if (skipping == JTOK_ARRAY_START) ++depth;
                break;
            }
            case ']': {
                if (skipping == JTOK_ARRAY_START) {
                    --depth;
                    if (depth == 0) {
                        reader->lastToken = JTOK_ARRAY_END;
                        return TM_TRUE;
                    }
                }
                break;
            }
            case '"': {
                --reader->data;
                ++reader->size;
                if (!readQuotedString(reader)) return TM_FALSE;
                break;
            }
        }
    }

    return TM_FALSE;
}

TMJ_DEF JsonReader jsonMakeReader(const char* data, tm_size_t size, JsonContextEntry* contextStackMemory,
                                  tm_size_t contextStackSize, unsigned int flags) {
    JsonReader reader;
    TM_MEMSET(&reader, 0, sizeof(JsonReader));

    reader.data = data;
    reader.size = size;
    reader.line = 1;
    reader.flags = flags;
    reader.flags &= ~TMJ_ROOT_ENTERED;
    reader.contextStack.data = contextStackMemory;
    reader.contextStack.size = 0;
    reader.contextStack.capacity = contextStackSize;
    return reader;
}

tm_size_t jsonCopyUnescapedString(JsonStringView str, char* buffer, tm_size_t size) {
    if (!str.data || str.size <= 0 || size <= 0) return 0;
    TM_ASSERT(buffer);
    const char* first = str.data;
    const char* last = str.data + str.size;
    const char* cur = first;
    char* buffer_start = buffer;
    tm_size_t buffer_remaining = size;

    while ((cur = (const char*)TM_MEMCHR(first, (unsigned char)'\\', (size_t)(last - first))) != TM_NULL) {
        if (buffer_remaining <= 0) break;

        tm_size_t range = (tm_size_t)(cur - first);
        if (range > buffer_remaining) range = buffer_remaining;
        TM_MEMCPY(buffer, first, range * sizeof(char));
        buffer += range;
        buffer_remaining -= range;
        first = cur + 1;
        if (first == last || first + 1 == last) break;
        ++first;
        switch (*(cur + 1)) {
            case '\n': {
                // escaped multiline string, ignore newline
                continue;
            }
            case 'b': {
                *buffer++ = '\b';
                break;
            }
            case 'f': {
                *buffer++ = '\f';
                break;
            }
            case 'n': {
                *buffer++ = '\n';
                break;
            }
            case 'r': {
                *buffer++ = '\r';
                break;
            }
            case 't': {
                *buffer++ = '\t';
                break;
            }
            case 'u': {
                tm_size_t remaining = (tm_size_t)(last - first);
                if (remaining < 4) return (tm_size_t)(buffer - buffer_start);

                uint32_t codepoint = 0;
                tm_size_t advance = tmj_get_codepoint(first, remaining, &codepoint);
                if (advance <= 0) return (tm_size_t)(buffer - buffer_start);

                tm_size_t written = tmj_utf8_encode(codepoint, buffer, buffer_remaining);
                if (written > buffer_remaining) return (tm_size_t)(buffer - buffer_start);

                buffer += written;
                buffer_remaining -= written;
                first += advance;
                break;
            }
            default: {
                // Default behavior, just copy char after the escape as it is.
                *buffer++ = *(cur + 1);
                break;
            }
        }
    }
    if (first != last && buffer_remaining > 0) {
        tm_size_t range = (tm_size_t)(last - first);
        if (range > buffer_remaining) range = buffer_remaining;
        TM_MEMCPY(buffer, first, range * sizeof(char));
        buffer += range;
        buffer_remaining -= range;
    }
    return (tm_size_t)(buffer - buffer_start);
}
tm_size_t jsonCopyConcatenatedString(JsonStringView str, char* buffer, tm_size_t size) {
    if (!str.data || str.size <= 1 || size <= 0) return 0;
    char quot = str.data[0];
    ++str.data;
    --str.size;
    TM_ASSERT(buffer);
    tm_size_t sz = TM_MIN(size, str.size);
    const char* p = str.data;
    const char* next = TM_NULL;
    const char* start = buffer;
    tm_bool add = TM_TRUE;
    while ((next = (const char*)TM_MEMCHR(p, quot, sz)) != TM_NULL) {
        sz -= (tm_size_t)(next - p + 1);
        if (add) {
            tm_bool skip = TM_FALSE;
            if (next > p && !tmj_is_char_unescaped(p, next)) {
                // include escaped quotation when copying unescaped string
                ++next;
                skip = TM_TRUE;
            }
            JsonStringView current;
            current.data = p;
            current.size = (tm_size_t)(next - p);
            tm_size_t len = jsonCopyUnescapedString(current, buffer, size);
            buffer += len;
            size -= len;
            if (skip) {
                // skip to next part of string, since we haven't found an unescaped quot yet
                p = next;
                continue;
            }
        }
        p = next + 1;
        add = !add;
    }
    if (sz) {
        TM_MEMCPY(buffer, p, sz);
        buffer += sz;
    }
    tm_size_t result = (tm_size_t)(buffer - start);
    if (result < size) {
        // make resulting string nullterminated
        buffer[result] = 0;
    }
    return result;
}

JsonStringView jsonAllocateUnescapedString(JsonStackAllocator* allocator, JsonStringView str) {
    JsonStringView result = {TM_NULL, 0};
    char* buffer = (char*)jsonAllocate(allocator, str.size, sizeof(char));
    if (buffer) {
        result.size = jsonCopyUnescapedString(str, buffer, str.size);
        result.data = buffer;
        if (result.size < str.size) {
            // give back unused size to allocator
            tm_size_t diff = (tm_size_t)(str.size - result.size);
            allocator->ptr -= diff;
            allocator->size += diff;
        }
    }
    return result;
}
JsonStringView jsonAllocateConcatenatedString(JsonStackAllocator* allocator, JsonStringView str) {
    JsonStringView result = {TM_NULL, 0};
    char* buffer = (char*)jsonAllocate(allocator, str.size, sizeof(char));
    if (buffer) {
        result.size = jsonCopyConcatenatedString(str, buffer, str.size);
        result.data = buffer;
        if (result.size < str.size) {
            // give back unused size to allocator
            tm_size_t diff = (tm_size_t)(str.size - result.size);
            allocator->ptr -= diff;
            allocator->size += diff;
        }
    }
    return result;
}

inline static int jsonIsPowerOfTwo(unsigned int x) { return x && !(x & (x - 1)); }
TMJ_DEF unsigned int jsonGetAlignmentOffset(const void* ptr, unsigned int alignment) {
    TM_ASSERT(alignment != 0 && jsonIsPowerOfTwo(alignment));
    unsigned int alignmentOffset = (alignment - ((uintptr_t)ptr)) & (alignment - 1);
    TM_ASSERT((((uintptr_t)((char*)ptr + alignmentOffset)) % (alignment)) == 0);
    return alignmentOffset;
}
TMJ_DEF void* jsonAllocate(JsonStackAllocator* allocator, size_t size, unsigned int alignment) {
    unsigned int offset = jsonGetAlignmentOffset(allocator->ptr + allocator->size, alignment);
    if (allocator->size + offset + size > allocator->capacity) {
        return TM_NULL;
    }

    char* result = allocator->ptr + allocator->size + offset;
    allocator->size += offset + size;
    TM_ASSERT((((uintptr_t)((char*)result)) % (alignment)) == 0);
    return result;
}

#define JSON_ALIGNMENT_OBJECT sizeof(void*)
#define JSON_ALIGNMENT_ARRAY sizeof(void*)
#define JSON_ALIGNMENT_NODE sizeof(void*)
#define JSON_ALIGNMENT_VALUE JSON_ALIGNMENT_OBJECT

#ifdef __cplusplus
static_assert(JSON_ALIGNMENT_OBJECT == alignof(JsonObject), "wrong object alignment");
static_assert(JSON_ALIGNMENT_ARRAY == alignof(JsonArray), "wrong array alignment");
static_assert(JSON_ALIGNMENT_NODE == alignof(JsonNode), "wrong node alignment");
static_assert(JSON_ALIGNMENT_VALUE == alignof(JsonValue), "wrong value alignment");
#endif

static JsonErrorType jsonReadArray(JsonReader* reader, JsonStackAllocator* allocator, JsonArray* out);

static JsonErrorType jsonReadObject(JsonReader* reader, JsonStackAllocator* allocator, JsonObject* out) {
    memset(out, 0, sizeof(JsonObject));
    JsonReader readerState = *reader;

    // count number of members
    tm_size_t memberCount = 0;
    int run = 1;
    do {
        JsonTokenType token = jsonNextTokenImplicit(reader, JSON_CONTEXT_OBJECT);
        switch (token) {
            case JTOK_PROPERTYNAME: {
                ++memberCount;
                jsonSkipCurrent(reader, JSON_CONTEXT_OBJECT, TM_FALSE);
                break;
            }
            case JTOK_OBJECT_END: {
                run = 0;
                break;
            }
            case JTOK_COMMENT: {
                break;
            }
            default: {
                if (reader->errorType != JSON_OK) {
                    return reader->errorType;
                }
                TM_ASSERT(0 && "invalid code path");
                return JERR_INTERNAL_ERROR;
            }
        }
    } while (run);

    *reader = readerState;
    out->nodes = (JsonNode*)jsonAllocate(allocator, memberCount * sizeof(JsonNode), JSON_ALIGNMENT_NODE);
    if (!out->nodes) return JERR_OUT_OF_MEMORY;
    out->count = memberCount;

    tm_size_t nodesIndex = 0;
    run = 1;
    do {
        JsonTokenType token = jsonNextTokenImplicit(reader, JSON_CONTEXT_OBJECT);
        switch (token) {
            case JTOK_PROPERTYNAME: {
                JsonNode* current = &out->nodes[nodesIndex++];
                current->name = reader->current;
                int searchContent = 1;
                do {
                    switch (jsonNextTokenImplicit(reader, JSON_CONTEXT_OBJECT)) {
                        case JTOK_OBJECT_START: {
                            searchContent = 0;
                            current->value.type = JVAL_OBJECT;
                            JsonErrorType result = jsonReadObject(reader, allocator, &current->value.data.object);
                            if (result != JSON_OK) {
                                return result;
                            }
                            break;
                        }
                        case JTOK_ARRAY_START: {
                            searchContent = 0;
                            current->value.type = JVAL_ARRAY;
                            JsonErrorType result = jsonReadArray(reader, allocator, &current->value.data.array);
                            if (result != JSON_OK) {
                                return result;
                            }
                            break;
                        }
                        case JTOK_VALUE: {
                            searchContent = 0;
                            current->value.type = reader->valueType;
                            if (current->value.type == JVAL_STRING) {
                                current->value.data.content = jsonAllocateUnescapedString(allocator, reader->current);
                            } else {
                                current->value.data.content = reader->current;
                            }
                            break;
                        }
                        case JTOK_COMMENT: {
                            break;
                        }
                        default: {
                            if (reader->errorType != JSON_OK) {
                                return reader->errorType;
                            }
                            TM_ASSERT(0 && "invalid code path");
                            return JERR_INTERNAL_ERROR;
                        }
                    }
                } while (searchContent);
                break;
            }
            case JTOK_COMMENT: {
                break;
            }
            case JTOK_OBJECT_END: {
                run = 0;
                break;
            }
            default: {
                if (reader->errorType != JSON_OK) {
                    return reader->errorType;
                }
                TM_ASSERT(0 && "invalid code path");
                return JERR_INTERNAL_ERROR;
            }
        }
    } while (run);

    return reader->errorType;
}
static JsonErrorType jsonReadArray(JsonReader* reader, JsonStackAllocator* allocator, JsonArray* out) {
    JsonReader readerState = *reader;

    // count number of members
    int memberCount = 0;
    int run = 1;
    do {
        JsonTokenType token = jsonNextTokenImplicit(reader, JSON_CONTEXT_ARRAY);
        switch (token) {
            case JTOK_OBJECT_START:
            case JTOK_ARRAY_START: {
                ++memberCount;
                jsonSkipCurrent(reader, JSON_CONTEXT_ARRAY, TM_FALSE);
                break;
            }
            case JTOK_VALUE: {
                ++memberCount;
                break;
            }
            case JTOK_COMMENT: {
                break;
            }
            case JTOK_ARRAY_END: {
                run = 0;
                break;
            }
            default: {
                if (reader->errorType != JSON_OK) {
                    return reader->errorType;
                }
                TM_ASSERT(0 && "invalid code path");
                return JERR_INTERNAL_ERROR;
            }
        }
    } while (run);

    *reader = readerState;
    out->values = (JsonValue*)jsonAllocate(allocator, memberCount * sizeof(JsonValue), JSON_ALIGNMENT_VALUE);
    if (!out->values) return JERR_OUT_OF_MEMORY;
    out->count = memberCount;

    int valuesIndex = 0;
    run = 1;
    do {
        JsonTokenType token = jsonNextTokenImplicit(reader, JSON_CONTEXT_ARRAY);
        switch (token) {
            case JTOK_OBJECT_START: {
                JsonValue* value = &out->values[valuesIndex++];
                value->type = JVAL_OBJECT;
                JsonErrorType result = jsonReadObject(reader, allocator, &value->data.object);
                if (result != JSON_OK) {
                    return result;
                }
                break;
            }
            case JTOK_ARRAY_START: {
                JsonValue* value = &out->values[valuesIndex++];
                value->type = JVAL_ARRAY;
                JsonErrorType result = jsonReadArray(reader, allocator, &value->data.array);
                if (result != JSON_OK) {
                    return result;
                }
                break;
            }
            case JTOK_VALUE: {
                JsonValue* value = &out->values[valuesIndex++];
                value->type = reader->valueType;
                if (value->type == JVAL_STRING) {
                    value->data.content = jsonAllocateUnescapedString(allocator, reader->current);
                } else {
                    value->data.content = reader->current;
                }
                break;
            }
            case JTOK_COMMENT: {
                break;
            }
            case JTOK_ARRAY_END: {
                run = 0;
                break;
            }
            default: {
                if (reader->errorType != JSON_OK) {
                    return reader->errorType;
                }
                TM_ASSERT(0 && "invalid code path");
                return JERR_INTERNAL_ERROR;
            }
        }
    } while (run);

    return reader->errorType;
}

TMJ_DEF JsonAllocatedDocument jsonAllocateDocument(const char* data, tm_size_t size, unsigned int flags) {
    size_t poolSize = size * sizeof(JsonValue);
    char* pool = (char*)TMJ_ALLOCATE(poolSize, JSON_ALIGNMENT_VALUE);
    JsonStackAllocator allocator;
    allocator.ptr = pool;
    allocator.size = 0;
    allocator.capacity = size * sizeof(JsonValue);
    JsonAllocatedDocument result;
    result.document = jsonMakeDocument(&allocator, data, size, flags);
    result.pool = pool;
    result.poolSize = poolSize;
    return result;
}
TMJ_DEF JsonDocument jsonMakeDocument(JsonStackAllocator* allocator, const char* data, tm_size_t size,
                                      unsigned int flags) {
    JsonDocument result;
    TM_MEMSET(&result, 0, sizeof(JsonDocument));

    JsonReader reader = jsonMakeReader(data, size, TM_NULL, 0, flags);
    JsonTokenType rootType = jsonReadRootType(&reader, /*ex=*/TM_FALSE);
    switch (rootType) {
        case JTOK_ERROR: {
            break;
        }
        case JTOK_OBJECT_START: {
            result.root.type = JVAL_OBJECT;
            result.error.type = jsonReadObject(&reader, allocator, &result.root.data.object);
            break;
        }
        case JTOK_ARRAY_START: {
            result.root.type = JVAL_ARRAY;
            result.error.type = jsonReadArray(&reader, allocator, &result.root.data.array);
            break;
        }
        case JTOK_VALUE: {
            result.root.type = reader.valueType;
            if (result.root.type == JVAL_STRING) {
                result.root.data.content = jsonAllocateUnescapedString(allocator, reader.current);
            } else {
                result.root.data.content = reader.current;
            }
            break;
        }
        default: {
            TM_ASSERT(0 && "invalid code path");
            result.error.type = JERR_INTERNAL_ERROR;
            break;
        }
    }
    if (reader.errorType == JSON_OK) jsonIsValidUntilEof(&reader);
    if (reader.errorType != JSON_OK) {
        TM_MEMSET(&result, 0, sizeof(JsonDocument));
        result.error.type = reader.errorType;
        result.error.line = reader.line;
        result.error.column = reader.column;
        result.error.offset = (tm_size_t)(reader.current.data - data);
        result.error.length = reader.current.size;
    }
    return result;
}

static JsonErrorType jsonReadArrayEx(JsonReader* reader, JsonStackAllocator* allocator, JsonArray* out);
static JsonValue jsonAllocateValueEx(JsonStackAllocator* allocator, JsonReader* reader) {
    JsonValue result;
    switch (reader->valueType) {
        case JVAL_STRING: {
            result.type = JVAL_STRING;
            result.data.content = jsonAllocateUnescapedString(allocator, reader->current);
            break;
        }
        case JVAL_CONCAT_STRING: {
            result.type = JVAL_STRING;
            result.data.content = jsonAllocateConcatenatedString(allocator, reader->current);
            break;
        }
        case JVAL_RAW_STRING: {
            result.type = JVAL_STRING;
            result.data.content = reader->current;
            break;
        }
        default: {
            result.type = reader->valueType;
            result.data.content = reader->current;
            break;
        }
    }
    return result;
}
static JsonErrorType jsonReadObjectEx(JsonReader* reader, JsonStackAllocator* allocator, JsonObject* out) {
    memset(out, 0, sizeof(JsonObject));
    JsonReader readerState = *reader;

    // count number of members
    tm_size_t memberCount = 0;
    int run = 1;
    do {
        JsonTokenType token = jsonNextTokenImplicitEx(reader, JSON_CONTEXT_OBJECT);
        switch (token) {
            case JTOK_PROPERTYNAME: {
                ++memberCount;
                jsonSkipCurrent(reader, JSON_CONTEXT_OBJECT, TM_TRUE);
                break;
            }
            case JTOK_OBJECT_END: {
                run = 0;
                break;
            }
            case JTOK_COMMENT: {
                break;
            }
            default: {
                if (reader->errorType != JSON_OK) {
                    return reader->errorType;
                }
                TM_ASSERT(0 && "invalid code path");
                return JERR_INTERNAL_ERROR;
            }
        }
    } while (run);

    *reader = readerState;
    out->nodes = (JsonNode*)jsonAllocate(allocator, memberCount * sizeof(JsonNode), JSON_ALIGNMENT_NODE);
    if (!out->nodes) return JERR_OUT_OF_MEMORY;
    out->count = memberCount;

    tm_size_t nodesIndex = 0;
    run = 1;
    do {
        JsonTokenType token = jsonNextTokenImplicitEx(reader, JSON_CONTEXT_OBJECT);
        switch (token) {
            case JTOK_PROPERTYNAME: {
                JsonNode* current = &out->nodes[nodesIndex++];
                current->name = reader->current;
                int searchContent = 1;
                do {
                    switch (jsonNextTokenImplicitEx(reader, JSON_CONTEXT_OBJECT)) {
                        case JTOK_OBJECT_START: {
                            searchContent = 0;
                            current->value.type = JVAL_OBJECT;
                            JsonErrorType result = jsonReadObjectEx(reader, allocator, &current->value.data.object);
                            if (result != JSON_OK) {
                                return result;
                            }
                            break;
                        }
                        case JTOK_ARRAY_START: {
                            searchContent = 0;
                            current->value.type = JVAL_ARRAY;
                            JsonErrorType result = jsonReadArrayEx(reader, allocator, &current->value.data.array);
                            if (result != JSON_OK) {
                                return result;
                            }
                            break;
                        }
                        case JTOK_VALUE: {
                            searchContent = 0;
                            current->value = jsonAllocateValueEx(allocator, reader);
                            break;
                        }
                        case JTOK_COMMENT: {
                            break;
                        }
                        default: {
                            if (reader->errorType != JSON_OK) {
                                return reader->errorType;
                            }
                            TM_ASSERT(0 && "invalid code path");
                            return JERR_INTERNAL_ERROR;
                        }
                    }
                } while (searchContent);
                break;
            }
            case JTOK_COMMENT: {
                break;
            }
            case JTOK_OBJECT_END: {
                run = 0;
                break;
            }
            default: {
                if (reader->errorType != JSON_OK) {
                    return reader->errorType;
                }
                TM_ASSERT(0 && "invalid code path");
                return JERR_INTERNAL_ERROR;
            }
        }
    } while (run);

    return reader->errorType;
}
static JsonErrorType jsonReadArrayEx(JsonReader* reader, JsonStackAllocator* allocator, JsonArray* out) {
    JsonReader readerState = *reader;

    // count number of members
    int memberCount = 0;
    int run = 1;
    do {
        JsonTokenType token = jsonNextTokenImplicitEx(reader, JSON_CONTEXT_ARRAY);
        switch (token) {
            case JTOK_OBJECT_START:
            case JTOK_ARRAY_START: {
                ++memberCount;
                jsonSkipCurrent(reader, JSON_CONTEXT_ARRAY, TM_TRUE);
                break;
            }
            case JTOK_VALUE: {
                ++memberCount;
                break;
            }
            case JTOK_COMMENT: {
                break;
            }
            case JTOK_ARRAY_END: {
                run = 0;
                break;
            }
            default: {
                if (reader->errorType != JSON_OK) {
                    return reader->errorType;
                }
                TM_ASSERT(0 && "invalid code path");
                return JERR_INTERNAL_ERROR;
            }
        }
    } while (run);

    *reader = readerState;
    out->values = (JsonValue*)jsonAllocate(allocator, memberCount * sizeof(JsonValue), JSON_ALIGNMENT_VALUE);
    if (!out->values) return JERR_OUT_OF_MEMORY;
    out->count = memberCount;

    int valuesIndex = 0;
    run = 1;
    do {
        JsonTokenType token = jsonNextTokenImplicitEx(reader, JSON_CONTEXT_ARRAY);
        switch (token) {
            case JTOK_OBJECT_START: {
                JsonValue* value = &out->values[valuesIndex++];
                value->type = JVAL_OBJECT;
                JsonErrorType result = jsonReadObjectEx(reader, allocator, &value->data.object);
                if (result != JSON_OK) {
                    return result;
                }
                break;
            }
            case JTOK_ARRAY_START: {
                JsonValue* value = &out->values[valuesIndex++];
                value->type = JVAL_ARRAY;
                JsonErrorType result = jsonReadArrayEx(reader, allocator, &value->data.array);
                if (result != JSON_OK) {
                    return result;
                }
                break;
            }
            case JTOK_VALUE: {
                JsonValue* value = &out->values[valuesIndex++];
                *value = jsonAllocateValueEx(allocator, reader);
                break;
            }
            case JTOK_COMMENT: {
                break;
            }
            case JTOK_ARRAY_END: {
                run = 0;
                break;
            }
            default: {
                if (reader->errorType != JSON_OK) {
                    return reader->errorType;
                }
                TM_ASSERT(0 && "invalid code path");
                return JERR_INTERNAL_ERROR;
            }
        }
    } while (run);

    return reader->errorType;
}

TMJ_DEF JsonAllocatedDocument jsonAllocateDocumentEx(const char* data, tm_size_t size, unsigned int flags) {
    size_t poolSize = size * sizeof(JsonValue);
    char* pool = (char*)TMJ_ALLOCATE(poolSize, JSON_ALIGNMENT_VALUE);
    JsonStackAllocator allocator;
    allocator.ptr = pool;
    allocator.size = 0;
    allocator.capacity = size * sizeof(JsonValue);
    JsonAllocatedDocument result;
    result.document = jsonMakeDocumentEx(&allocator, data, size, flags);
    result.pool = pool;
    result.poolSize = poolSize;
    return result;
}
TMJ_DEF JsonDocument jsonMakeDocumentEx(JsonStackAllocator* allocator, const char* data, tm_size_t size,
                                        unsigned int flags) {
    JsonDocument result;
    TM_MEMSET(&result, 0, sizeof(JsonDocument));

    JsonReader reader = jsonMakeReader(data, size, TM_NULL, 0, flags);
    JsonTokenType rootType = jsonReadRootType(&reader, /*ex=*/TM_TRUE);
    switch (rootType) {
        case JTOK_ERROR: {
            break;
        }
        case JTOK_OBJECT_START: {
            result.root.type = JVAL_OBJECT;
            result.error.type = jsonReadObjectEx(&reader, allocator, &result.root.data.object);
            break;
        }
        case JTOK_ARRAY_START: {
            result.root.type = JVAL_ARRAY;
            result.error.type = jsonReadArrayEx(&reader, allocator, &result.root.data.array);
            break;
        }
        case JTOK_VALUE: {
            result.root = jsonAllocateValueEx(allocator, &reader);
            break;
        }
        default: {
            TM_ASSERT(0 && "invalid code path");
            result.error.type = JERR_INTERNAL_ERROR;
            break;
        }
    }
    if (reader.errorType == JSON_OK) jsonIsValidUntilEofEx(&reader);
    if (reader.errorType != JSON_OK) {
        TM_MEMSET(&result, 0, sizeof(JsonDocument));
        result.error.type = reader.errorType;
        result.error.line = reader.line;
        result.error.column = reader.column;
        result.error.offset = (tm_size_t)(reader.current.data - data);
        result.error.length = reader.current.size;
    }
    return result;
}
TMJ_DEF void jsonFreeDocument(JsonAllocatedDocument* doc) {
    if (doc->pool) {
        TMJ_FREE(doc->pool, doc->poolSize);
        doc->pool = TM_NULL;
        doc->poolSize = 0;
    }
}

TMJ_DEF tm_bool jsonIsNull(const JsonValue* value) {
    TM_ASSERT(value);
    return value->type == JVAL_NULL && value->data.content.data != TM_NULL;
}
TMJ_DEF tm_bool jsonIsIntegral(const JsonValue* value) { return value->type >= JVAL_INT; }
TMJ_DEF tm_bool jsonIsString(const JsonValue* value) { return value->type == JVAL_STRING; }
TMJ_DEF JsonObject jsonGetObject(const JsonValue* value) {
#ifdef __cplusplus
    JsonObject result = {TM_NULL, 0, 0};
#else
    JsonObject result = {TM_NULL, 0};
#endif
    if (value->type == JVAL_OBJECT) {
        result = value->data.object;
    }
    return result;
}
TMJ_DEF JsonArray jsonGetArray(const JsonValue* value) {
    JsonArray result = {TM_NULL, 0};
    if (value->type == JVAL_ARRAY) {
        result = value->data.array;
    }
    return result;
}
TMJ_DEF JsonObjectArray jsonGetObjectArray(const JsonValue* value) {
    JsonObjectArray result = {TM_NULL, 0};
    if (value->type == JVAL_ARRAY) {
        result.values = value->data.array.values;
        result.count = value->data.array.count;
    }
    return result;
}

TMJ_DEF JsonValue jsonGetMember(const JsonObject* object, const char* name) {
    TM_ASSERT(object);
#ifdef __cplusplus
    return jsonGetMemberCached(object, name, &object->lastAccess);
#else
    JsonValue result = {0};
    JsonNode* it = object->nodes;
    JsonNode* last = object->nodes + object->count;
    size_t len = TM_STRLEN(name);
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, name, len)) {
            result = it->value;
            break;
        }
    }
    return result;
#endif
}
TMJ_DEF JsonValue* jsonQueryMember(const JsonObject* object, const char* name) {
    TM_ASSERT(object);
#ifdef __cplusplus
    return jsonQueryMemberCached(object, name, &object->lastAccess);
#else
    JsonNode* it = object->nodes;
    JsonNode* last = object->nodes + object->count;
    size_t len = TM_STRLEN(name);
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, name, len)) {
            return &it->value;
        }
    }
    return TM_NULL;
#endif
}
TMJ_DEF JsonValue jsonGetMemberCached(const JsonObject* object, const char* name, tm_size_t* lastAccess) {
    TM_ASSERT(object);
    JsonValue result = {JVAL_NULL, {{TM_NULL, 0}}};
    JsonValue* value = jsonQueryMemberCached(object, name, lastAccess);
    if (value) {
        result = *value;
    }
    return result;
}
TMJ_DEF JsonValue* jsonQueryMemberCached(const JsonObject* object, const char* name, tm_size_t* lastAccess) {
    TM_ASSERT(object);
    TM_ASSERT(lastAccess);
    JsonValue* result = TM_NULL;
    tm_bool found = TM_FALSE;
    JsonNode* it = object->nodes + *lastAccess;
    JsonNode* last = object->nodes + object->count;
    size_t len = TM_STRLEN(name);
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, name, len)) {
            result = &it->value;
            *lastAccess = (tm_size_t)(it - object->nodes);
            found = TM_TRUE;
            break;
        }
    }
    if (!found) {
        it = object->nodes;
        last = object->nodes + *lastAccess;
        for (; it < last; ++it) {
            if (stringEquals(it->name.data, it->name.size, name, len)) {
                result = &it->value;
                *lastAccess = (tm_size_t)(it - object->nodes);
                break;
            }
        }
    }
    return result;
}

#if defined(__cplusplus) && defined(TM_STRING_VIEW)
TMJ_DEF JsonValue jsonGetMember(const JsonObject* object, TM_STRING_VIEW name) {
    TM_ASSERT(object);
#ifdef __cplusplus
    return jsonGetMemberCached(object, name, &object->lastAccess);
#else
    JsonValue result = {0};
    JsonNode* it = object->nodes;
    JsonNode* last = object->nodes + object->count;
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, TM_STRING_VIEW_DATA(name), TM_STRING_VIEW_SIZE(name))) {
            result = it->value;
            break;
        }
    }
    return result;
#endif
}
TMJ_DEF JsonValue* jsonQueryMember(const JsonObject* object, TM_STRING_VIEW name) {
    TM_ASSERT(object);
#ifdef __cplusplus
    return jsonQueryMemberCached(object, name, &object->lastAccess);
#else
    JsonNode* it = object->nodes;
    JsonNode* last = object->nodes + object->count;
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, TM_STRING_VIEW_DATA(name), TM_STRING_VIEW_SIZE(name))) {
            return &it->value;
        }
    }
    return TM_NULL;
#endif
}
TMJ_DEF JsonValue jsonGetMemberCached(const JsonObject* object, TM_STRING_VIEW name, tm_size_t* lastAccess) {
    TM_ASSERT(object);
    JsonValue result = {JVAL_NULL, {{TM_NULL, 0}}};
    if (JsonValue* value = jsonQueryMemberCached(object, name, lastAccess)) {
        result = *value;
    }
    return result;
}
TMJ_DEF JsonValue* jsonQueryMemberCached(const JsonObject* object, TM_STRING_VIEW name, tm_size_t* lastAccess) {
    TM_ASSERT(object);
    TM_ASSERT(lastAccess);
    JsonValue* result = TM_NULL;
    tm_bool found = TM_FALSE;
    JsonNode* it = object->nodes + *lastAccess;
    JsonNode* last = object->nodes + object->count;
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, TM_STRING_VIEW_DATA(name), TM_STRING_VIEW_SIZE(name))) {
            result = &it->value;
            *lastAccess = (tm_size_t)(it - object->nodes);
            found = TM_TRUE;
            break;
        }
    }
    if (!found) {
        it = object->nodes;
        last = object->nodes + *lastAccess;
        for (; it < last; ++it) {
            if (stringEquals(it->name.data, it->name.size, TM_STRING_VIEW_DATA(name), TM_STRING_VIEW_SIZE(name))) {
                result = &it->value;
                *lastAccess = (tm_size_t)(it - object->nodes);
                break;
            }
        }
    }
    return result;
}
#endif  // defined( __cplusplus ) && defined( TM_STRING_VIEW )

TMJ_DEF JsonValue jsonGetEntry(const JsonArray* array, tm_size_t index) {
    TM_ASSERT(array);
    TM_ASSERT(tmj_valid_index(index, array->count));
    return array->values[index];
}

TMJ_DEF tm_bool jsonIsValidObject(const JsonObject* object) {
    TM_ASSERT(object);
    return object->nodes != TM_NULL;
}
TMJ_DEF tm_bool jsonIsValidArray(const JsonArray* array) {
    TM_ASSERT(array);
    return array->values != TM_NULL;
}
TMJ_DEF tm_bool jsonIsValidValue(const JsonValue* value) {
    TM_ASSERT(value);
    return value->type != JVAL_NULL;
}

TMJ_DEF int32_t jsonGetInt(const JsonValue* value, int32_t def) {
    switch (value->type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToInt(value->data.content, def);
        }
    }
}
TMJ_DEF uint32_t jsonGetUInt(const JsonValue* value, uint32_t def) {
    switch (value->type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToUInt(value->data.content, def);
        }
    }
}
TMJ_DEF float jsonGetFloat(const JsonValue* value, float def) {
    switch (value->type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToFloat(value->data.content, def);
        }
    }
}
TMJ_DEF double jsonGetDouble(const JsonValue* value, double def) {
    switch (value->type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToDouble(value->data.content, def);
        }
    }
}
TMJ_DEF tm_bool jsonGetBool(const JsonValue* value, tm_bool def) {
    switch (value->type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToBool(value->data.content, def);
        }
    }
}
#ifndef TMJ_NO_INT64
TMJ_DEF int64_t jsonGetInt64(const JsonValue* value, int64_t def) {
    switch (value->type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToInt64(value->data.content, def);
        }
    }
}
TMJ_DEF uint64_t jsonGetUInt64(const JsonValue* value, uint64_t def) {
    switch (value->type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToUInt64(value->data.content, def);
        }
    }
}
#endif

#endif  // TM_JSON_IMPLEMENTATION

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
