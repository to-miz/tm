/*
tm_json.h v0.1.7 - public domain - https://github.com/to-miz/tm
written by Tolga Mizrak 2016

no warranty; use at your own risk

LICENSE
    see license notes at end of file

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_JSON_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

    See SWITCHES for more options.

PURPOSE
    A validating utf8 json parser, that is extensible using a lot of different flags.
    Can be used as both a simple and lightweight non validating parser, as well as a strict
    validating parser. Using the functions with the Ex extension, you can make the parser
    accept Json5 files.
    If you want to use this parser with non utf8 input, convert your input to utf8 first.

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
        TMJ_TO_INT(str, len, base, def):
        TMJ_TO_UINT(str, len, base, def):
        TMJ_TO_INT64(str, len, base, def):
        TMJ_TO_UINT64(str, len, base, def):
        TMJ_TO_FLOAT(str, len, def):
        TMJ_TO_DOUBLE(str, len, def):
            define these macros if you have string conversion functions that accept non
            nullterminated strings.
            These macros represent functions with these signatures:
                int to_int(const char* data, size_t size, int base, int def)
                unsigned int to_uint(const char* data, size_t size, int base, unsigned int def)
                double to_double(const char* data, size_t size, double def)
                float to_float(const char* data, size_t size, float def)
                long long to_int64(const char* data, size_t size, int base, long long def)
                unsigned long long to_uint64(const char* data, size_t size, int base,
                                             unsigned long long def)
            Arguments:
                str: a non nullterminated string
                len: the length of str
                base: base to use when converting str into the return value
                def: the default value to use if conversion fails
            If you do not define these, the implementation will use stdlib.h to implement them.
            Since the crt string conversion functions do not allow non nullterminated strings, the
            implementation has to copy the strings into a temporary buffer and nullterminate it
            before passing them into the crt string conversion functions.
        TMJ_DEFINE_INFINITY_AND_NAN:
            define this if you want to use the float extension flag and the math.h definitions of
            INFINITY and NAN. If you do not wish to use the float extension flag
            when parsing (JSON_READER_EXTENDED_FLOATS), you do not need to define these.
            Otherwise define TM_INFINITY and TM_NAN yourself with the inf and nan representations
            on your platform.
        TM_INFINITY:
        TM_NAN:
            these are automatically defined if you define TMJ_DEFINE_INFINITY_AND_NAN and if math.h
            has definitions for INFINITY and NAN. If you do not wish to use the float extension flag
            when parsing (JSON_READER_EXTENDED_FLOATS), you do not need to define these.
            Otherwise define TM_INFINITY and TM_NAN yourself with the inf and nan representations
            on your platform.

ISSUES
    - \u isn't implemented in strings, it will just be ignored currently
    - mismatched brackets will be reported as JERR_UNEXPECTED_EOF instead of JERR_MISMATCHED_BRACKETS
      when using jsonMakeDocument, because jsonSkipCurrent skips until eof on mismatched brackets.
    - missing documentation and example usage code
    - json objects aren't implemented as dictionaries/hashmaps, instead they use linear lookup
      json objects should be turned into hashmaps manually when applicable

HISTORY
    v0.1.7  31.12.18  added more error info to jsonDocument
                      fixed GCC unused-function warning when building in release builds
                      improved default implementation of string conversion functions
    v0.1.6  29.12.18  fixed GCC warnings for multi-line comment, missing-field-initializers and implicit-fallthrough
                      fixed an error in compareString
                      renamed compareString functions to stringEquals, since they only check equality
                      removed tmj_bool and TMJ_NULL, using tm_bool and TM_NULL instead
                      reformatted and switched to indentation using spaces
    v0.1.5  06.10.18  refactored preprocessor
                      changed tmj_size_t to tm_size_t
                      changed TMJ_STRING_VIEW to TM_STRING_VIEW
    v0.1.4b 25.08.18  added repository link
    v0.1.4a 22.07.18  fixed some warnings on C99 compilation
    v0.1.4  22.07.18  removed overloads of C functions for C++, updated licensing
    v0.1.3  10.02.17  string view operators are now preferred to const char* operators in JsonObject
    v0.1.2  28.01.17  added jsonObjectArray for usage with C++11 range based loops
    v0.1.1d 10.01.17  minor change from static const char* to static const char* const in some places
    v0.1.1c 07.11.16  minor edits, no runtime changes
    v0.1.1b 10.10.16  fixed some warnings when tm_size_t is signed
    v0.1.1a 07.10.16  removed usage of unsigned arithmetic when tm_size_t is signed
    v0.1.1  13.09.16  changed JsonValue interface to have operator[] overloads for convenience
                      added more string_view support through TM_STRING_VIEW
    v0.1    11.09.16  initial commit
*/

/* clang-format off */
#ifdef TM_JSON_IMPLEMENTATION
    // define these to avoid crt
    #ifndef TM_MEMCHR
        #include <string.h>
        #define TM_MEMCHR memchr
    #endif
    #ifndef TM_STRLEN
        #include <string.h>
        #define TM_STRLEN strlen
    #endif
    #ifndef TM_ISDIGIT
        #include <ctype.h>
        #define TM_ISDIGIT isdigit
        #define TM_ISALPHA isalpha
        #define TM_ISXDIGIT isxdigit
        #define TM_ISSPACE isspace
        #define TM_TOUPPER toupper
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
        #define TMJ_TO_INT(str, len, base, def) tmj_to_int((str), (len), (base), (def))
        #define TMJ_TO_UINT(str, len, base, def) tmj_to_uint((str), (len), (base), (def))
        #define TMJ_TO_INT64(str, len, base, def) tmj_to_int64((str), (len), (base), (def))
        #define TMJ_TO_UINT64(str, len, base, def) tmj_to_uint64((str), (len), (base), (def))
        #define TMJ_TO_FLOAT(str, len, def) tmj_to_float((str), (len), (def))
        #define TMJ_TO_DOUBLE(str, len, def) tmj_to_double((str), (len), (def))
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
    JTOK_COMMA,

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

    // allow c++0x style raw string literals
    // example
    // "property": R"delim(...)delim"
    // if both cpp style raw strings and python style raw strings are enabled,
    // cpp style raw strings have precedence
    JSON_READER_CPP_RAW_STRINGS = (1u << 9u),

    // allow python style raw string literals
    // example
    // "property": r'...'
    JSON_READER_PYTHON_RAW_STRINGS = (1u << 10u),

    // allow c style concatenated multiline strings after a property name
    // example
    // "property":  "this is a multiline "
    //              "string"
    JSON_READER_CONCATENATED_STRINGS = (1u << 11u),

    // allow c style concatenated multiline strings in arrays, errorprone in case a comma is
    // forgotten mistakenly
    JSON_READER_CONCATENATED_STRINGS_IN_ARRAYS = (1u << 12u),

    // allow '=' and ':' for property names
    JSON_READER_ALLOW_EQUAL = (1u << 13u),

    // allow numbers to begin with an explicit plus sign
    JSON_READER_ALLOW_PLUS_SIGN = (1u << 14u),

    // allow case insensitive true, false, null keywords and infinity etc in case of extended floats
    JSON_READER_IGNORE_CASE_KEYWORDS = (1u << 15u),

    // use these flags to parse json5 files
    JSON_READER_JSON5 =
        (JSON_READER_SINGLE_LINE_COMMENTS | JSON_READER_BLOCK_COMMENTS | JSON_READER_TRAILING_COMMA |
         JSON_READER_UNQUOTED_PROPERTY_NAMES | JSON_READER_SINGLE_QUOTED_STRINGS | JSON_READER_EXTENDED_FLOATS |
         JSON_READER_HEXADECIMAL | JSON_READER_ESCAPED_MULTILINE_STRINGS | JSON_READER_ALLOW_PLUS_SIGN),

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
    JERR_OUT_OF_CONTEXT_STACK_MEMORY,  // not enough contextStackMemory to parse a given json file,
                                       // occurs if there are more alternating context (going from
                                       // { to [ and back) switches than memory
    JERR_NO_ROOT,
    JERR_OUT_OF_MEMORY,
    JERR_INTERNAL_ERROR
} JsonErrorType;

TMJ_DEF const char* jsonGetErrorString(JsonErrorType error);

typedef struct {
    const char* data;
    tm_size_t size;
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

// initializes and returns a JsonReader.
// params:
//  data: the utf8 json file contents
//  size: size of data in bytes
//  contextStackMemory:
//      the stack memory the reader uses for bookkeeping. Only needed if you use jsonNextToken or
//      jsonNextTokenEx. If you plan to use the implicit versions (jsonNextTokenImplicit or
//      jsonNextTokenImplicitEx), this can be NULL.
//  contextStackSize: element count of contextStackMemory.
//  flags: parsing flags used when parsing. See enum JsonReaderFlags for all valid flags.
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
TMJ_DEF JsonContext jsonReadRootType(JsonReader* reader);
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
TMJ_DEF int jsonToInt(JsonStringView str, int def);
TMJ_DEF unsigned int jsonToUInt(JsonStringView str, unsigned int def);
TMJ_DEF float jsonToFloat(JsonStringView str, float def);
TMJ_DEF double jsonToDouble(JsonStringView str, double def);
TMJ_DEF tm_bool jsonToBool(JsonStringView str, tm_bool def);
#ifndef TMJ_NO_INT64
TMJ_DEF long long jsonToInt64(JsonStringView str, long long def);
TMJ_DEF unsigned long long jsonToUInt64(JsonStringView str, unsigned long long def);
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
#else
typedef const char* tmj_string_arg;
typedef JsonStringView tmj_string_view;
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

    int getInt(int def = 0) const;
    unsigned int getUInt(unsigned int def = 0) const;
    float getFloat(float def = 0.0f) const;
    double getDouble(double def = 0.0f) const;
    bool getBool(bool def = false) const;
#ifndef TMJ_NO_INT64
    long long getInt64(long long def = 0) const;
    unsigned long long getUInt64(unsigned long long def = 0) const;
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

TMJ_DEF JsonAllocatedDocument jsonAllocateDocument(const char* data, tm_size_t size, unsigned int flags);
TMJ_DEF void jsonFreeDocument(JsonAllocatedDocument* doc);
TMJ_DEF JsonDocument jsonMakeDocument(JsonStackAllocator* allocator, const char* data, tm_size_t size,
                                      unsigned int flags);

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

#if !defined(TMJ_PASS_BY_POINTER) && defined(__cplusplus)
    typedef const JsonValue& JsonValueArg;
    typedef const JsonObject& JsonObjectArg;
    typedef const JsonArray& JsonArrayArg;
    #define TMJ_ARG(arg) (arg)
    #define TMJ_DEREF(arg) (arg)
#else
    typedef const JsonValue* JsonValueArg;
    typedef const JsonObject* JsonObjectArg;
    typedef const JsonArray* JsonArrayArg;
    #define TMJ_ARG(arg) (&(arg))
    #define TMJ_DEREF(arg) (*(arg))
#endif
/* clang-format on */

TMJ_DEF tm_bool jsonIsNull(JsonValueArg value);
TMJ_DEF tm_bool jsonIsIntegral(JsonValueArg value);
TMJ_DEF tm_bool jsonIsString(JsonValueArg value);
// returns a JsonObject if value contains an object, returns a nil object otherwise
// see documentation at the top of the file for what nil objects are
TMJ_DEF JsonObject jsonGetObject(JsonValueArg value);
// returns a JsonArray if value contains an array, returns a nil array otherwise
// see documentation at the top of the file for what nil arrays are
TMJ_DEF JsonArray jsonGetArray(JsonValueArg value);
TMJ_DEF JsonObjectArray jsonGetObjectArray(JsonValueArg value);

TMJ_DEF JsonValue jsonGetMember(JsonObjectArg object, const char* name);
TMJ_DEF JsonValue* jsonQueryMember(JsonObjectArg object, const char* name);
TMJ_DEF JsonValue jsonGetMemberCached(JsonObjectArg object, const char* name, tm_size_t* lastAccess);
TMJ_DEF JsonValue* jsonQueryMemberCached(JsonObjectArg object, const char* name, tm_size_t* lastAccess);
TMJ_DEF JsonValue jsonGetEntry(JsonArrayArg object, tm_size_t index);

// string_view overloads
#if defined(__cplusplus) && defined(TM_STRING_VIEW)
TMJ_DEF JsonValue jsonGetMember(JsonObjectArg object, TM_STRING_VIEW name);
TMJ_DEF JsonValue* jsonQueryMember(JsonObjectArg object, TM_STRING_VIEW name);
TMJ_DEF JsonValue jsonGetMemberCached(JsonObjectArg object, TM_STRING_VIEW name, tm_size_t* lastAccess);
TMJ_DEF JsonValue* jsonQueryMemberCached(JsonObjectArg object, TM_STRING_VIEW name, tm_size_t* lastAccess);
#endif  // defined( __cplusplus ) && defined( TM_STRING_VIEW )

TMJ_DEF tm_bool jsonIsValidObject(JsonObjectArg object);
TMJ_DEF tm_bool jsonIsValidArray(JsonArrayArg array);
TMJ_DEF tm_bool jsonIsValidValue(JsonValueArg value);

TMJ_DEF int jsonGetInt(JsonValueArg value, int def);
TMJ_DEF unsigned int jsonGetUInt(JsonValueArg value, unsigned int def);
TMJ_DEF float jsonGetFloat(JsonValueArg value, float def);
TMJ_DEF double jsonGetDouble(JsonValueArg value, double def);
TMJ_DEF tm_bool jsonGetBool(JsonValueArg value, tm_bool def);
#ifndef TMJ_NO_INT64
TMJ_DEF long long jsonGetInt64(JsonValueArg value, long long def);
TMJ_DEF unsigned long long jsonGetUInt64(JsonValueArg value, unsigned long long def);
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
    return {data.content.data, data.content.size};
}

inline JsonObject JsonValueStruct::getObject() const { return jsonGetObject(TMJ_ARG(*this)); }
inline JsonArray JsonValueStruct::getArray() const { return jsonGetArray(TMJ_ARG(*this)); }
inline JsonObjectArray JsonValueStruct::getObjectArray() const { return jsonGetObjectArray(TMJ_ARG(*this)); }
inline bool JsonValueStruct::isNull() const { return jsonIsNull(TMJ_ARG(*this)); }
inline bool JsonValueStruct::isIntegral() const { return jsonIsIntegral(TMJ_ARG(*this)); }
inline bool JsonValueStruct::isString() const { return jsonIsString(TMJ_ARG(*this)); }

inline tm_size_t JsonArray::size() const { return count; }
inline JsonValue JsonArray::operator[](tm_size_t index) const { return jsonGetEntry(TMJ_ARG(*this), index); }

inline tm_size_t JsonObjectArray::size() const { return count; }
inline JsonObject JsonObjectArray::operator[](tm_size_t index) const {
    TM_ASSERT(tmj_valid_index(index, count));
    return jsonGetObject(values[index]);
}

inline tm_size_t JsonObject::size() const { return count; }
inline JsonValue JsonObject::operator[](tmj_string_arg name) const { return jsonGetMember(*this, name); }
inline bool JsonObject::exists(tmj_string_arg name) const { return jsonQueryMember(TMJ_ARG(*this), name) != TM_NULL; }
inline JsonValue* JsonObject::find(tmj_string_arg name) const { return jsonQueryMember(TMJ_ARG(*this), name); }

inline int JsonValueStruct::getInt(int def) const { return jsonGetInt(TMJ_ARG(*this), def); }
inline unsigned int JsonValueStruct::getUInt(unsigned int def) const { return jsonGetUInt(TMJ_ARG(*this), def); }
inline float JsonValueStruct::getFloat(float def) const { return jsonGetFloat(TMJ_ARG(*this), def); }
inline double JsonValueStruct::getDouble(double def) const { return jsonGetDouble(TMJ_ARG(*this), def); }
inline bool JsonValueStruct::getBool(bool def) const { return jsonGetBool(TMJ_ARG(*this), def); }
#ifndef TMJ_NO_INT64
inline long long JsonValueStruct::getInt64(long long def) const { return jsonGetInt64(TMJ_ARG(*this), def); }
inline unsigned long long JsonValueStruct::getUInt64(unsigned long long def) const {
    return jsonGetUInt64(TMJ_ARG(*this), def);
}
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
        if (TM_TOUPPER((unsigned char)*a) != (unsigned char)*b) return TM_FALSE;
        ++a;
        ++b;
        --bSize;
    } while (bSize);
    return TM_TRUE;
}
// TODO: maybe use strncmp instead if second string is nullterminated?
static tm_bool stringEqualsNull(const char* a, size_t aSize, const char* b) {
    return stringEquals(a, aSize, b, TM_STRLEN(b));
}

// This function is only used in debug builds currently.
#ifdef _DEBUG
// TODO: maybe use strnicmp instead if second string is nullterminated?
static tm_bool stringEqualsIgnoreCaseNull(const char* a, size_t aSize, const char* b) {
    return stringEqualsIgnoreCase(a, aSize, b, TM_STRLEN(b));
}
#endif

#ifdef TMJ_DEFINE_OWN_STRING_CONVERSIONS

#ifndef TM_UNREFERENCED_PARAM
#define TM_UNREFERENCED_PARAM(x) ((void)(x))
#endif
// TODO: inspect strings to return def param if string is illformed?

#include <stdlib.h>
#include <errno.h>
static int tmj_to_int(const char* data, tm_size_t size, int base, int def) {
    TM_ASSERT_VALID_SIZE(size);
    if (size > 32) return def;
    char buffer[33];
    TM_MEMCPY(buffer, data, size);
    buffer[size] = 0;
    errno = 0;
    int result = (int)strtol(buffer, TM_NULL, base);
    if (errno == ERANGE) return def;
    return result;
}
static unsigned int tmj_to_uint(const char* data, tm_size_t size, int base, unsigned int def) {
    TM_ASSERT_VALID_SIZE(size);
    if (size > 32) return def;
    char buffer[33];
    TM_MEMCPY(buffer, data, size);
    buffer[size] = 0;
    return (unsigned int)strtoul(buffer, TM_NULL, base);
}
static double tmj_to_double(const char* data, tm_size_t size, double def) {
    TM_ASSERT_VALID_SIZE(size);
    if (size > 511) return def;
    char buffer[512];
    TM_MEMCPY(buffer, data, size);
    buffer[size] = 0;
    return strtod(buffer, TM_NULL);
}
#ifndef TMJ_NO_INT64
static long long tmj_to_int64(const char* data, tm_size_t size, int base, long long def) {
    TM_ASSERT_VALID_SIZE(size);
    if (size > 64) return def;
    char buffer[65];
    TM_MEMCPY(buffer, data, size);
    buffer[size] = 0;
    return (long long)strtoll(buffer, TM_NULL, base);
}
static unsigned long long tmj_to_uint64(const char* data, tm_size_t size, int base, unsigned long long def) {
    TM_ASSERT_VALID_SIZE(size);
    if (size > 64) return def;
    char buffer[65];
    TM_MEMCPY(buffer, data, size);
    buffer[size] = 0;
    return (unsigned long long)strtoull(buffer, TM_NULL, base);
}
#endif  // !defined(TMJ_NO_INT64)
#endif  // defined(TMJ_DEFINE_OWN_STRING_CONVERSIONS)

TMJ_DEF int jsonToInt(JsonStringView str, int def) {
    if (str.size <= 0) return def;
    if (str.size >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        if (str.size == 2) return def;
        return TMJ_TO_INT(str.data + 2, str.size - 2, 16, def);
    }
    return TMJ_TO_INT(str.data, str.size, 10, def);
}
TMJ_DEF unsigned int jsonToUInt(JsonStringView str, unsigned int def) {
    if (str.size <= 0) return def;
    if (str.size >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        if (str.size == 2) return def;
        return TMJ_TO_UINT(str.data + 2, str.size - 2, 16, def);
    }
    return TMJ_TO_UINT(str.data, str.size, 10, def);
}
TMJ_DEF float jsonToFloat(JsonStringView str, float def) {
    return (float)jsonToDouble(str, (double)def);
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
        if (stringEqualsIgnoreCase(str_.data, str_.size, "INFINITY", 8)) {
            return (neg) ? (-TM_INFINITY) : (TM_INFINITY);
        }
#endif
#if defined(TM_NAN)
        if (stringEqualsIgnoreCase(str_.data, str_.size, "NAN", 3)) {
            return (neg) ? (-TM_NAN) : (TM_NAN);
        }
#endif
    }
#endif
    return TMJ_TO_DOUBLE(str.data, str.size, def);
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
    if (stringEqualsIgnoreCase(str.data, str.size, "TRUE", 4)) {
        return TM_TRUE;
    } else if (stringEqualsIgnoreCase(str.data, str.size, "FALSE", 5)) {
        return TM_FALSE;
    }
    return def;
}
#ifndef TMJ_NO_INT64
TMJ_DEF long long jsonToInt64(JsonStringView str, long long def) {
    if (str.size <= 0) return def;
    if (str.size >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        if (str.size == 2) return def;
        return TMJ_TO_INT64(str.data + 2, str.size - 2, 16, def);
    }
    return TMJ_TO_INT64(str.data, str.size, 10, def);
}
TMJ_DEF unsigned long long jsonToUInt64(JsonStringView str, unsigned long long def) {
    if (str.size <= 0) return def;
    if (str.size >= 2 && str.data[0] == '0' && (str.data[1] == 'x' || str.data[1] == 'X')) {
        if (str.size == 2) return def;
        return TMJ_TO_UINT64(str.data + 2, str.size - 2, 16, def);
    }
    return TMJ_TO_UINT64(str.data, str.size, 10, def);
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
static tm_size_t skipWhitespace(JsonReader* reader) {
    static const char* const whitespace = " \t\r\f\n\v";
    const char* p;
    while ((p = (const char*)TM_MEMCHR(whitespace, reader->data[0], 6)) != TM_NULL) {
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
static void setError(JsonReader* reader, JsonErrorType error) {
    reader->errorType = error;
    reader->current.data = reader->data;
    reader->current.size = 1;
}
static tm_bool readQuotedString(JsonReader* reader) {
    int quote = (unsigned char)reader->data[0];
    TM_ASSERT(quote == '\'' || quote == '"');
    const char* start = reader->data;
    ++reader->data;
    --reader->size;
    reader->current.data = reader->data;
    reader->current.size = 0;
    // TODO: profile to see which is faster, memchr twice over the string or go byte by byte once
    const char* p;
    while ((p = (const char*)TM_MEMCHR(reader->data, quote, reader->size)) != TM_NULL) {
        reader->data = p + 1;
        if (*(p - 1) != '\\') {
            // unescaped quotation mark, we found the string
            reader->current.size = (tm_size_t)(reader->data - reader->current.data - 1);
            if (reader->flags & JSON_READER_ESCAPED_MULTILINE_STRINGS) {
                const char* last = reader->current.data;
                tm_size_t size = reader->current.size;
                while ((p = (const char*)TM_MEMCHR(last, '\n', size)) != TM_NULL) {
                    if (p == reader->current.data || (*(p - 1) != '\\')) {
                        reader->errorType = JERR_ILLFORMED_STRING;
                        reader->current.data = p;
                        reader->current.size = 1;
                        return TM_FALSE;
                    }
                    reader->column = 0;
                    ++reader->line;
                    size -= (tm_size_t)(p - last + 1);
                    last = p + 1;
                }
                reader->column = size;
            } else {
                if ((p = (const char*)TM_MEMCHR(reader->current.data, '\n', reader->current.size)) != TM_NULL) {
                    reader->errorType = JERR_ILLFORMED_STRING;
                    reader->current.data = p;
                    reader->current.size = 1;
                    return TM_FALSE;
                }
                reader->column += reader->current.size + 1;
            }
            reader->size -= reader->current.size + 1;
            if (reader->size <= 0) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return TM_FALSE;
            }
            return TM_TRUE;
        }
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
    reader->valueType = JVAL_UINT;
    reader->lastToken = JTOK_VALUE;
    reader->current.data = reader->data;
    if (reader->data[0] == '-') {
        reader->valueType = JVAL_INT;
        jsonAdvance(reader);
    } else if ((reader->flags & JSON_READER_ALLOW_PLUS_SIGN) && reader->data[0] == '+') {
        jsonAdvance(reader);
    }

    if (reader->data[0] == '0') {
        jsonAdvance(reader);
        if (!reader->size) {
            setError(reader, JERR_UNEXPECTED_EOF);
            return TM_FALSE;
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
                if (!reader->size) {
                    setError(reader, JERR_UNEXPECTED_EOF);
                    return TM_FALSE;
                }
            } while (TM_ISXDIGIT((unsigned char)reader->data[0]));
            reader->valueType = JVAL_UINT;
            reader->current.size = (tm_size_t)(reader->data - reader->current.data);
            return TM_TRUE;
        }
    } else if (TM_ISDIGIT((unsigned char)reader->data[0])) {
        do {
            jsonAdvance(reader);
            if (!reader->size) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return TM_FALSE;
            }
        } while (TM_ISDIGIT((unsigned char)reader->data[0]));
    } else {
        setError(reader, JERR_UNEXPECTED_TOKEN);
        return TM_FALSE;
    }

    if (reader->data[0] == '.') {
        reader->valueType = JVAL_FLOAT;
        do {
            jsonAdvance(reader);
            if (!reader->size) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return TM_FALSE;
            }
        } while (TM_ISDIGIT((unsigned char)reader->data[0]));
    }

    if (reader->data[0] == 'e' || reader->data[0] == 'E') {
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
            if (!reader->size) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return TM_FALSE;
            }
        } while (TM_ISDIGIT((unsigned char)reader->data[0]));
    }

    if (!reader->size) {
        setError(reader, JERR_UNEXPECTED_EOF);
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
    if (!reader->size) {
        setError(reader, JERR_UNEXPECTED_EOF);
        return JTOK_ERROR;
    }
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
                if (stringEqualsIgnoreCase(start, size, "INFINITY", 8)) {
                    reader->valueType = JVAL_FLOAT;
                    advanceValue(reader, 8 + offset);
                    return TM_TRUE;
                } else if (stringEqualsIgnoreCase(start, size, "NAN", 3)) {
                    reader->valueType = JVAL_FLOAT;
                    advanceValue(reader, 3 + offset);
                    return TM_TRUE;
                }
            } else {
                if (stringEquals(start, size, "infinity", 8)) {
                    reader->valueType = JVAL_FLOAT;
                    advanceValue(reader, 8 + offset);
                    return TM_TRUE;
                } else if (stringEquals(start, size, "nan", 3)) {
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
    if (stringEquals(reader->data, reader->size, "true", 4)) {
        reader->valueType = JVAL_BOOL;
        return advanceValue(reader, 4);
    } else if (stringEquals(reader->data, reader->size, "false", 5)) {
        reader->valueType = JVAL_BOOL;
        return advanceValue(reader, 5);
    } else if (stringEquals(reader->data, reader->size, "null", 4)) {
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
        if (stringEqualsIgnoreCase(reader->data, reader->size, "TRUE", 4)) {
            reader->valueType = JVAL_BOOL;
            return advanceValue(reader, 4);
        } else if (stringEqualsIgnoreCase(reader->data, reader->size, "FALSE", 5)) {
            reader->valueType = JVAL_BOOL;
            return advanceValue(reader, 5);
        } else if (stringEqualsIgnoreCase(reader->data, reader->size, "NULL", 4)) {
            reader->valueType = JVAL_NULL;
            return advanceValue(reader, 4);
        }
    } else {
        if (stringEquals(reader->data, reader->size, "true", 4)) {
            reader->valueType = JVAL_BOOL;
            return advanceValue(reader, 4);
        } else if (stringEquals(reader->data, reader->size, "false", 5)) {
            reader->valueType = JVAL_BOOL;
            return advanceValue(reader, 5);
        } else if (stringEquals(reader->data, reader->size, "null", 4)) {
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
    if (stack->size && stack->data[stack->size - 1].context == context) {
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
    if (!stack->size || stack->data[stack->size - 1].context != context) {
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
    // check whether a value can follow last token in current context
    switch (currentContext) {
        case JSON_CONTEXT_OBJECT: {
            if (reader->lastToken != JTOK_PROPERTYNAME) {
                return TM_FALSE;
            }
            break;
        }
        case JSON_CONTEXT_ARRAY: {
            if (reader->lastToken != JTOK_ARRAY_START && reader->lastToken != JTOK_COMMA) {
                return TM_FALSE;
            }
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
        case JTOK_PROPERTYNAME: {
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
static void jsonReadLine(JsonReader* reader) {
    reader->current.data = reader->data;
    const char* p = (const char*)TM_MEMCHR(reader->data, '\n', reader->size);
    if (!p) {
        p = reader->data + reader->size;
    }
    reader->current.size = (tm_size_t)(p - reader->data);
    reader->column = 0;
    ++reader->line;
    reader->data += reader->current.size + 1;
    reader->size -= reader->current.size + 1;
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
            jsonReadLine(reader);
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
            jsonReadLine(reader);
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
                reader->data += 1;
                reader->size -= 1;
                if (*(p + 1) == '/') {
                    break;
                }
                reader->current.size += 2;
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
static tm_bool jsonParseColon(JsonReader* reader) {
    if (!skipWhitespace(reader)) {
        setError(reader, JERR_UNEXPECTED_EOF);
        return TM_FALSE;
    }
    if (reader->data[0] == ':' || (reader->flags & JSON_READER_ALLOW_EQUAL && reader->data[0] == '=')) {
        jsonAdvance(reader);
        return TM_TRUE;
    }
    setError(reader, JERR_UNEXPECTED_TOKEN);
    return TM_FALSE;
}
static JsonTokenType jsonParseQuotedString(JsonReader* reader, JsonContext currentContext) {
    if (currentContext == JSON_CONTEXT_NULL) {
        setError(reader, JERR_UNEXPECTED_TOKEN);
        return JTOK_ERROR;
    }
    if (!readQuotedString(reader)) {
        return JTOK_ERROR;
    }
    reader->valueType = JVAL_STRING;
    if (!skipWhitespace(reader)) {
        setError(reader, JERR_UNEXPECTED_EOF);
        return JTOK_ERROR;
    }
    if (currentContext == JSON_CONTEXT_OBJECT &&
        (reader->lastToken == JTOK_OBJECT_START || reader->lastToken == JTOK_COMMA)) {
        if (!jsonParseColon(reader)) {
            return JTOK_ERROR;
        }
        reader->lastToken = JTOK_PROPERTYNAME;
    } else {
        if (!jsonCanValueFollowLastToken(reader, currentContext)) {
            setError(reader, JERR_UNEXPECTED_TOKEN);
            return JTOK_ERROR;
        }
        reader->lastToken = JTOK_VALUE;
    }
    return reader->lastToken;
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
            case ',': {
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
                if (currentContext == JSON_CONTEXT_NULL || !jsonCanValueFollowLastToken(reader, currentContext)) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }

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
        token = jsonNextTokenImplicit(reader, JSON_CONTEXT_NULL);
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
        reader->lastToken != JTOK_PROPERTYNAME) {
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
        if (TM_ISDIGIT(reader->data[0]) || (!TM_ISALPHA(reader->data[0]) && reader->data[0] != '_')) {
            return TM_FALSE;
        }
        jsonAdvance(reader);
        while (reader->size && (TM_ISDIGIT(reader->data[0]) || TM_ISALPHA(reader->data[0]) || reader->data[0] == '_')) {
            jsonAdvance(reader);
        }
        reader->current.size = (tm_size_t)(reader->data - reader->current.data);
        reader->lastToken = JTOK_PROPERTYNAME;
        return jsonParseColon(reader);
    }
    return TM_FALSE;
}

static tm_bool jsonParsePythonRawString(JsonReader* reader, JsonContext currentContext) {
    if ((currentContext == JSON_CONTEXT_ARRAY || reader->lastToken == JTOK_PROPERTYNAME) &&
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
    if ((currentContext == JSON_CONTEXT_ARRAY || reader->lastToken == JTOK_PROPERTYNAME) &&
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
                // get delimeter
                char delim[17];
                tm_size_t delimSize = 0;
                while (reader->size && jsonIsValidDelimChar(reader->data[0]) && delimSize < 16) {
                    jsonAdvance(reader);
                    delim[delimSize] = reader->data[0];
                    ++delimSize;
                }
                if (reader->size && reader->data[0] == '(') {
                    // append '"' to delimeter so that we do not need to check ending quotation mark
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
            if (!reader->size) {
                setError(reader, JERR_UNEXPECTED_EOF);
                return JTOK_ERROR;
            }
            JsonReader stateGuard = *reader;
            if (reader->data[0] == quot && readQuotedString(reader)) {
                if (!skipWhitespace(reader)) {
                    setError(reader, JERR_UNEXPECTED_EOF);
                    return JTOK_ERROR;
                }
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
            case ',': {
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
                // check raw string cases
                switch (c) {
                    case 'r':
                    case 'R': {
                        if (reader->lastToken == JTOK_PROPERTYNAME) {
                            if (jsonParseCppRawString(reader, currentContext)) {
                                return reader->lastToken;
                            }
                            if (reader->errorType != JSON_OK) {
                                return JTOK_ERROR;
                            }
                            if (jsonParsePythonRawString(reader, currentContext)) {
                                return reader->lastToken;
                            }
                            if (reader->errorType != JSON_OK) {
                                return JTOK_ERROR;
                            }
                        }
                        break;
                    }
                }
                if (currentContext == JSON_CONTEXT_NULL) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }
                if (jsonParseUnquotedPropertyName(reader, currentContext)) {
                    return JTOK_PROPERTYNAME;
                }
                if (reader->errorType != JSON_OK) {
                    return JTOK_ERROR;
                }
                // json value case
                if (!jsonCanValueFollowLastToken(reader, currentContext)) {
                    setError(reader, JERR_UNEXPECTED_TOKEN);
                    return JTOK_ERROR;
                }

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

TMJ_DEF JsonContext jsonReadRootType(JsonReader* reader) {
    if (reader->contextStack.size) {
        JsonContext current = (JsonContext)reader->contextStack.data[reader->contextStack.size - 1].context;
        assert(current == JSON_CONTEXT_NULL);
        if (current != JSON_CONTEXT_NULL) {
            return JSON_CONTEXT_NULL;
        }

        for (;;) {
            JsonTokenType token = jsonNextToken(reader);
            switch (token) {
                case JTOK_OBJECT_START: {
                    return JSON_CONTEXT_OBJECT;
                }
                case JTOK_ARRAY_START: {
                    return JSON_CONTEXT_ARRAY;
                }
                case JTOK_COMMENT: {
                    continue;
                }
                default: {
                    return JSON_CONTEXT_NULL;
                }
            }
        }
    } else {
        for (;;) {
            JsonTokenType token = jsonNextTokenImplicit(reader, JSON_CONTEXT_NULL);
            switch (token) {
                case JTOK_OBJECT_START: {
                    return JSON_CONTEXT_OBJECT;
                }
                case JTOK_ARRAY_START: {
                    return JSON_CONTEXT_ARRAY;
                }
                case JTOK_COMMENT: {
                    continue;
                }
                default: {
                    return JSON_CONTEXT_NULL;
                }
            }
        }
    }
}

TMJ_DEF tm_bool jsonSkipCurrent(JsonReader* reader, JsonContext currentContext, tm_bool ex) {
    if (reader->errorType != JSON_OK) {
        return TM_FALSE;
    }
    if (!reader->size) {
        return TM_FALSE;
    }
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
        if (reader->errorType != JSON_OK) {
            return TM_FALSE;
        }
    }
    assert(reader->lastToken == JTOK_OBJECT_START || reader->lastToken == JTOK_ARRAY_START);
    JsonTokenType skipping = reader->lastToken;

    unsigned int depth = 1;

    while (reader->size) {
        if (!skipWhitespace(reader)) {
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
                if (skipping == JTOK_OBJECT_START) {
                    ++depth;
                }
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
                if (skipping == JTOK_ARRAY_START) {
                    ++depth;
                }
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
                if (!readQuotedString(reader)) {
                    return TM_FALSE;
                }
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
    if (!str.data || str.size <= 0 || size <= 0) {
        return 0;
    }
    TM_ASSERT(buffer);
    tm_size_t sz = TM_MIN(size, str.size);
    const char* p = str.data;
    const char* next = TM_NULL;
    const char* start = buffer;
    while ((next = (const char*)TM_MEMCHR(p, '\\', sz)) != TM_NULL) {
        TM_MEMCPY(buffer, p, next - p);
        buffer += next - p;
        sz -= (tm_size_t)(next - p + 2);
        p = next + 2;
        switch (*(next + 1)) {
            case '\n': {
                // escaped multiline string, ignore newline
                continue;
            }
            case 'b': {
                buffer[0] = '\b';
                break;
            }
            case 'f': {
                buffer[0] = '\f';
                break;
            }
            case 'n': {
                buffer[0] = '\n';
                break;
            }
            case 'r': {
                buffer[0] = '\r';
                break;
            }
            case 't': {
                buffer[0] = '\t';
                break;
            }
            case 'u': {
                // TODO: use utf16 to convert to utf8
                if (sz < 4) {
                    sz = 0;
                } else {
                    sz -= 4;
                    p += 4;
                }
                continue;
            }
            default: {
                // default behavior, just copy char after the escape as it is
                buffer[0] = *(next + 1);
                break;
            }
        }
        ++buffer;
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
tm_size_t jsonCopyConcatenatedString(JsonStringView str, char* buffer, tm_size_t size) {
    if (!str.data || str.size <= 1 || size <= 0) {
        return 0;
    }
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
            if (next > p && *(next - 1) == '\\') {
                // include escaped quotation when copying unescaped string
                ++next;
                skip = TM_TRUE;
            }
            JsonStringView current = {p, (tm_size_t)(next - p)};
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
    if (!out->nodes) {
        return JERR_OUT_OF_MEMORY;
    }
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
    if (!out->values) {
        return JERR_OUT_OF_MEMORY;
    }
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
    JsonStackAllocator allocator = {pool, 0, size * sizeof(JsonValue)};
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
    JsonContext rootType = jsonReadRootType(&reader);
    switch (rootType) {
        case JSON_CONTEXT_NULL: {
            result.error.type = JERR_NO_ROOT;
            break;
        }
        case JSON_CONTEXT_OBJECT: {
            result.root.type = JVAL_OBJECT;
            result.error.type = jsonReadObject(&reader, allocator, &result.root.data.object);
            break;
        }
        case JSON_CONTEXT_ARRAY: {
            result.root.type = JVAL_ARRAY;
            result.error.type = jsonReadArray(&reader, allocator, &result.root.data.array);
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
    if (!out->nodes) {
        return JERR_OUT_OF_MEMORY;
    }
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
    if (!out->values) {
        return JERR_OUT_OF_MEMORY;
    }
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
    JsonStackAllocator allocator = {pool, 0, size * sizeof(JsonValue)};
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
    JsonContext rootType = jsonReadRootType(&reader);
    switch (rootType) {
        case JSON_CONTEXT_NULL: {
            result.error.type = JERR_NO_ROOT;
            break;
        }
        case JSON_CONTEXT_OBJECT: {
            result.root.type = JVAL_OBJECT;
            result.error.type = jsonReadObjectEx(&reader, allocator, &result.root.data.object);
            break;
        }
        case JSON_CONTEXT_ARRAY: {
            result.root.type = JVAL_ARRAY;
            result.error.type = jsonReadArrayEx(&reader, allocator, &result.root.data.array);
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

TMJ_DEF tm_bool jsonIsNull(JsonValueArg value) {
#ifdef _DEBUG
    if (TMJ_DEREF(value).type == JVAL_NULL) {
        TM_ASSERT(stringEqualsIgnoreCaseNull(TMJ_DEREF(value).data.content.data, TMJ_DEREF(value).data.content.size,
                                              "null"));
    }
#endif  // _DEBUG
    return TMJ_DEREF(value).type == JVAL_NULL && TMJ_DEREF(value).data.content.data != TM_NULL;
}
TMJ_DEF tm_bool jsonIsIntegral(JsonValueArg value) { return TMJ_DEREF(value).type >= JVAL_INT; }
TMJ_DEF tm_bool jsonIsString(JsonValueArg value) { return TMJ_DEREF(value).type == JVAL_STRING; }
TMJ_DEF JsonObject jsonGetObject(JsonValueArg value) {
#ifdef __cplusplus
    JsonObject result = {TM_NULL, 0, 0};
#else
    JsonObject result = {TM_NULL, 0};
#endif
    if (TMJ_DEREF(value).type == JVAL_OBJECT) {
        result = TMJ_DEREF(value).data.object;
    }
    return result;
}
TMJ_DEF JsonArray jsonGetArray(JsonValueArg value) {
    JsonArray result = {TM_NULL, 0};
    if (TMJ_DEREF(value).type == JVAL_ARRAY) {
        result = TMJ_DEREF(value).data.array;
    }
    return result;
}
TMJ_DEF JsonObjectArray jsonGetObjectArray(JsonValueArg value) {
    JsonObjectArray result = {TM_NULL, 0};
    if (TMJ_DEREF(value).type == JVAL_ARRAY) {
        result.values = TMJ_DEREF(value).data.array.values;
        result.count = TMJ_DEREF(value).data.array.count;
    }
    return result;
}

TMJ_DEF JsonValue jsonGetMember(JsonObjectArg object, const char* name) {
#ifdef __cplusplus
    return jsonGetMemberCached(object, name, &object.lastAccess);
#else
    JsonValue result = {0};
    JsonNode* it = TMJ_DEREF(object).nodes;
    JsonNode* last = TMJ_DEREF(object).nodes + TMJ_DEREF(object).count;
    for (; it < last; ++it) {
        if (stringEqualsNull(it->name.data, it->name.size, name)) {
            result = it->value;
            break;
        }
    }
    return result;
#endif
}
TMJ_DEF JsonValue* jsonQueryMember(JsonObjectArg object, const char* name) {
#ifdef __cplusplus
    return jsonQueryMemberCached(object, name, &object.lastAccess);
#else
    JsonNode* it = TMJ_DEREF(object).nodes;
    JsonNode* last = TMJ_DEREF(object).nodes + TMJ_DEREF(object).count;
    for (; it < last; ++it) {
        if (stringEqualsNull(it->name.data, it->name.size, name)) {
            return &it->value;
        }
    }
    return TM_NULL;
#endif
}
TMJ_DEF JsonValue jsonGetMemberCached(JsonObjectArg object, const char* name, tm_size_t* lastAccess) {
    JsonValue result = {JVAL_NULL, {TM_NULL, 0}};
    JsonValue* value = jsonQueryMemberCached(object, name, lastAccess);
    if (value) {
        result = *value;
    }
    return result;
}
TMJ_DEF JsonValue* jsonQueryMemberCached(JsonObjectArg object, const char* name, tm_size_t* lastAccess) {
    TM_ASSERT(lastAccess);
    JsonValue* result = TM_NULL;
    tm_bool found = TM_FALSE;
    JsonNode* it = TMJ_DEREF(object).nodes + *lastAccess;
    JsonNode* last = TMJ_DEREF(object).nodes + TMJ_DEREF(object).count;
    for (; it < last; ++it) {
        if (stringEqualsNull(it->name.data, it->name.size, name)) {
            result = &it->value;
            *lastAccess = (tm_size_t)(it - TMJ_DEREF(object).nodes);
            found = TM_TRUE;
            break;
        }
    }
    if (!found) {
        it = TMJ_DEREF(object).nodes;
        last = TMJ_DEREF(object).nodes + *lastAccess;
        for (; it < last; ++it) {
            if (stringEqualsNull(it->name.data, it->name.size, name)) {
                result = &it->value;
                *lastAccess = (tm_size_t)(it - TMJ_DEREF(object).nodes);
                break;
            }
        }
    }
    return result;
}

#if defined(__cplusplus) && defined(TM_STRING_VIEW)
TMJ_DEF JsonValue jsonGetMember(JsonObjectArg object, TM_STRING_VIEW name) {
#ifdef __cplusplus
    return jsonGetMemberCached(object, name, &object.lastAccess);
#else
    JsonValue result = {0};
    JsonNode* it = TMJ_DEREF(object).nodes;
    JsonNode* last = TMJ_DEREF(object).nodes + TMJ_DEREF(object).count;
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, TM_STRING_VIEW_DATA(name), TM_STRING_VIEW_SIZE(name))) {
            result = it->value;
            break;
        }
    }
    return result;
#endif
}
TMJ_DEF JsonValue* jsonQueryMember(JsonObjectArg object, TM_STRING_VIEW name) {
#ifdef __cplusplus
    return jsonQueryMemberCached(object, name, &object.lastAccess);
#else
    JsonNode* it = TMJ_DEREF(object).nodes;
    JsonNode* last = TMJ_DEREF(object).nodes + TMJ_DEREF(object).count;
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, TM_STRING_VIEW_DATA(name), TM_STRING_VIEW_SIZE(name))) {
            return &it->value;
        }
    }
    return TM_NULL;
#endif
}
TMJ_DEF JsonValue jsonGetMemberCached(JsonObjectArg object, TM_STRING_VIEW name, tm_size_t* lastAccess) {
    JsonValue result = {JVAL_NULL, {TM_NULL, 0}};
    if (JsonValue* value = jsonQueryMemberCached(object, name, lastAccess)) {
        result = *value;
    }
    return result;
}
TMJ_DEF JsonValue* jsonQueryMemberCached(JsonObjectArg object, TM_STRING_VIEW name, tm_size_t* lastAccess) {
    TM_ASSERT(lastAccess);
    JsonValue* result = TM_NULL;
    tm_bool found = TM_FALSE;
    JsonNode* it = TMJ_DEREF(object).nodes + *lastAccess;
    JsonNode* last = TMJ_DEREF(object).nodes + TMJ_DEREF(object).count;
    for (; it < last; ++it) {
        if (stringEquals(it->name.data, it->name.size, TM_STRING_VIEW_DATA(name), TM_STRING_VIEW_SIZE(name))) {
            result = &it->value;
            *lastAccess = (tm_size_t)(it - TMJ_DEREF(object).nodes);
            found = TM_TRUE;
            break;
        }
    }
    if (!found) {
        it = TMJ_DEREF(object).nodes;
        last = TMJ_DEREF(object).nodes + *lastAccess;
        for (; it < last; ++it) {
            if (stringEquals(it->name.data, it->name.size, TM_STRING_VIEW_DATA(name), TM_STRING_VIEW_SIZE(name))) {
                result = &it->value;
                *lastAccess = (tm_size_t)(it - TMJ_DEREF(object).nodes);
                break;
            }
        }
    }
    return result;
}
#endif  // defined( __cplusplus ) && defined( TM_STRING_VIEW )

TMJ_DEF JsonValue jsonGetEntry(JsonArrayArg array, tm_size_t index) {
    TM_ASSERT(tmj_valid_index(index, TMJ_DEREF(array).count));
    return TMJ_DEREF(array).values[index];
}

TMJ_DEF tm_bool jsonIsValidObject(JsonObjectArg object) { return TMJ_DEREF(object).nodes != TM_NULL; }
TMJ_DEF tm_bool jsonIsValidArray(JsonArrayArg array) { return TMJ_DEREF(array).values != TM_NULL; }
TMJ_DEF tm_bool jsonIsValidValue(JsonValueArg value) { return TMJ_DEREF(value).type != JVAL_NULL; }

TMJ_DEF int jsonGetInt(JsonValueArg value, int def) {
    switch (TMJ_DEREF(value).type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToInt(TMJ_DEREF(value).data.content, def);
        }
    }
}
TMJ_DEF unsigned int jsonGetUInt(JsonValueArg value, unsigned int def) {
    switch (TMJ_DEREF(value).type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToUInt(TMJ_DEREF(value).data.content, def);
        }
    }
}
TMJ_DEF float jsonGetFloat(JsonValueArg value, float def) {
    switch (TMJ_DEREF(value).type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToFloat(TMJ_DEREF(value).data.content, def);
        }
    }
}
TMJ_DEF double jsonGetDouble(JsonValueArg value, double def) {
    switch (TMJ_DEREF(value).type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToDouble(TMJ_DEREF(value).data.content, def);
        }
    }
}
TMJ_DEF tm_bool jsonGetBool(JsonValueArg value, tm_bool def) {
    switch (TMJ_DEREF(value).type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToBool(TMJ_DEREF(value).data.content, def);
        }
    }
}
#ifndef TMJ_NO_INT64
TMJ_DEF long long jsonGetInt64(JsonValueArg value, long long def) {
    switch (TMJ_DEREF(value).type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToInt64(TMJ_DEREF(value).data.content, def);
        }
    }
}
TMJ_DEF unsigned long long jsonGetUInt64(JsonValueArg value, unsigned long long def) {
    switch (TMJ_DEREF(value).type) {
        case JVAL_NULL:
        case JVAL_OBJECT:
        case JVAL_ARRAY: {
            return def;
        }
        default: {
            return jsonToUInt64(TMJ_DEREF(value).data.content, def);
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