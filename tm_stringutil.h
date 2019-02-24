/*
tm_stringutil.h v0.3.1 - public domain - https://github.com/to-miz/tm
author: Tolga Mizrak 2018

no warranty; use at your own risk

First release not hit yet. Probably very buggy.

LICENSE
    see license notes at end of file

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_STRINGUTIL_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

PURPOSE
    Some string based utility functions for searching and tokenizing.
    Most functions have versions that work on nullterminated and length based strings.

HISTORY
    v0.3.1  15.01.19 Fixed some warnings in msvc with signed size_t and string_view.
    v0.3    15.10.18 Added more string_view overloads.
    v0.2    14.10.18 Fixed an tmsu_compare functions to do proper lexicographical comparison.
                     Added tmsu_equals.
                     Fixed MSVC warning about unreachable code.
    v0.1    07.10.18 Initial commit.
*/

/* This is a generated file, do not modify directly. You can find the generator files in the src directory. */

/* clang-format off */
#ifdef TM_STRINGUTIL_IMPLEMENTATION
    /* Define these to get rid of crt dependency. */

    /* assert */
    #ifndef TM_ASSERT
        #include <assert.h>
        #define TM_ASSERT assert
    #endif /* !defined(TM_ASSERT) */

    #if !defined(TM_MEMCHR) || !defined(TM_STRCHR) || !defined(TM_STRLEN) || !defined(TM_STRNCMP) || !defined(TM_MEMCMP)
        #include <string.h>
        #ifndef TM_MEMCHR
            #define TM_MEMCHR memchr
        #endif
        #ifndef TM_STRCHR
            #define TM_STRCHR strchr
        #endif
        #ifndef TM_STRLEN
            #define TM_STRLEN strlen
        #endif
        #ifndef TM_STRNCMP
            #define TM_STRNCMP strncmp
        #endif
        #ifndef TM_MEMCMP
            #define TM_MEMCMP memcmp
        #endif

        /* Optional defines. */
        #ifndef TM_STRSTR
            #define TM_STRSTR strstr
        #endif
        #ifndef TM_STRCMP
            #define TM_STRCMP strcmp
        #endif
        #ifndef TM_STRICMP
            #ifdef WIN32
                #define TM_STRICMP _stricmp
            #endif
        #endif
    #endif

    #if !defined(TM_TOUPPER) || !defined(TM_TOLOWER) || !defined(TM_ISDIGIT) || !defined(TM_ISSPACE)
        #include <ctype.h>
        #ifndef TM_TOUPPER
            #define TM_TOUPPER toupper
        #endif
        #ifndef TM_TOLOWER
            #define TM_TOLOWER tolower
        #endif
        #ifndef TM_ISDIGIT
            #define TM_ISDIGIT isdigit
        #endif
        #ifndef TM_ISSPACE
            #define TM_ISSPACE isspace
        #endif
    #endif
#endif /* defined(TM_STRINGUTIL_IMPLEMENTATION) */

#ifndef _TM_STRINGUTIL_INCLUDED_49458961_DD38_441D_B888_A589548CA6F5_
#define _TM_STRINGUTIL_INCLUDED_49458961_DD38_441D_B888_A589548CA6F5_

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
    #include <stddef.h> /* include C version so identifiers are in global namespace */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

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

/* Linkage defaults to extern, to override define TMSU_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMSU_DEF
    #define TMSU_DEF extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* C++ string_view support. If TM_STRING_VIEW is defined, so must be TM_STRING_VIEW_DATA, TM_STRING_VIEW_SIZE
   and TM_STRING_VIEW_MAKE.
   Example:
        #include <string_view>
        #define TM_STRING_VIEW std::string_view
        #define TM_STRING_VIEW_DATA(str) (str).data()
        #define TM_STRING_VIEW_SIZE(str) ((tm_size_t)(str).size())
        #define TM_STRING_VIEW_MAKE(data, size) std::string_view{(data), (size_t)(size)}
*/
#ifdef TM_STRING_VIEW
    #if !defined(TM_STRING_VIEW_DATA) || !defined(TM_STRING_VIEW_SIZE) || !defined(TM_STRING_VIEW_MAKE)
        #error Invalid TM_STRINV_VIEW. If TM_STRING_VIEW is defined, so must be TM_STRING_VIEW_DATA, \
TM_STRING_VIEW_SIZE and TM_STRING_VIEW_MAKE.
    #endif
#endif

/*
Use the C++ string_view type if it is available. This will pull out any string_view returning function
out of C-linkage, since they cannot be defined in C. If C interop is needed, #undef TM_STRING_VIEW before
including this file, so both C and C++ use the same data types.
*/
#if defined(TM_STRING_VIEW) && defined(__cplusplus)
    typedef TM_STRING_VIEW tmsu_string_view;
    #define TMSU_STRING_VIEW_DATA(x) TM_STRING_VIEW_DATA(x)
    #define TMSU_STRING_VIEW_SIZE(x) TM_STRING_VIEW_SIZE(x)
    #define TMSU_STRING_VIEW_MAKE(str, size) TM_STRING_VIEW_MAKE((str), (size))
#else
    typedef struct {
        const char* data;
        tm_size_t size;
    } tmsu_string_view;
    #define TMSU_STRING_VIEW_DATA(x) (x).data
    #define TMSU_STRING_VIEW_SIZE(x) (x).size
    #define TMSU_STRING_VIEW_MAKE(str, size) tmsu_make_string_view(str, (tm_size_t)(size))
#endif

/* clang-format on */

/*
Find functions for nullterminated strings.
Params:
    str:      String to search in. Must not be NULL.
    find_str: String to search for. Must not be NULL.
Return pointer to first character of the match, or pointer to the nullterminator if no match is found.
*/
TMSU_DEF const char* tmsu_find_char(const char* str, char c);
TMSU_DEF const char* tmsu_find_char2(const char* str, char a, char b);
TMSU_DEF const char* tmsu_find_last_char(const char* str, char c);
TMSU_DEF const char* tmsu_find(const char* str, const char* find_str);
TMSU_DEF const char* tmsu_find_last(const char* str, const char* find_str);
TMSU_DEF const char* tmsu_find_first_not_of(const char* str, const char* find_str);
TMSU_DEF const char* tmsu_find_first_of(const char* str, const char* find_str);
TMSU_DEF const char* tmsu_find_last_not_of(const char* str, const char* find_str);
TMSU_DEF const char* tmsu_find_last_of(const char* str, const char* find_str);

/*
Find functions for length based strings/iterators. They return str_last if no match is found (even the *_last versions).
Params:
    str_first, str_last:           Iterator pair to search in. They can be equal for empty ranges.
    find_str_first, find_str_last: Iterator pair to search for. They can be equal for empty ranges.
    not_found:                     Iterator to return when no match was found. Defaults to str_last on functions
                                   that don't take a not_found parameter.
Return pointer to first character of the match, or str_last or not_found if no match is found.
*/
TMSU_DEF const char* tmsu_find_char_n(const char* str_first, const char* str_last, char c);
TMSU_DEF const char* tmsu_find_char2_n(const char* str_first, const char* str_last, char a, char b);
TMSU_DEF const char* tmsu_find_last_char_n(const char* str_first, const char* str_last, char c);
TMSU_DEF const char* tmsu_find_last_char_n_ex(const char* str_first, const char* str_last, char c,
                                              const char* not_found);
TMSU_DEF const char* tmsu_find_n(const char* str_first, const char* str_last, const char* find_str_first,
                                 const char* find_str_last);
TMSU_DEF const char* tmsu_find_last_n(const char* str_first, const char* str_last, const char* find_str_first,
                                      const char* find_str_last);
TMSU_DEF const char* tmsu_find_last_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                         const char* find_str_last, const char* not_found);
TMSU_DEF const char* tmsu_find_first_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                          const char* find_str_last);
TMSU_DEF const char* tmsu_find_first_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                              const char* find_str_last);
TMSU_DEF const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                         const char* find_str_last);
TMSU_DEF const char* tmsu_find_last_of_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                            const char* find_str_last, const char* not_found);
TMSU_DEF const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                             const char* find_str_last);
TMSU_DEF const char* tmsu_find_last_not_of_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                                const char* find_str_last, const char* not_found);

/* Ignores case for ansi characters. */
TMSU_DEF const char* tmsu_find_char_ignore_case_ansi_n(const char* str_first, const char* str_last, char c);
TMSU_DEF const char* tmsu_find_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                  const char* find_str_first, const char* find_str_last);

/*
Find functions that allow escaping of the character to look for. Useful for parsing.
Note that an escape character escapes the character immediatelly following it. These functions assume that the
escape character also escapes itself.
*/
TMSU_DEF const char* tmsu_find_char_unescaped(const char* str, char c, char escape_char);
TMSU_DEF const char* tmsu_find_first_of_unescaped(const char* str, const char* find_str, char escape_char);
TMSU_DEF const char* tmsu_find_char_unescaped_n(const char* str_first, const char* str_last, char c, char escape_char);
TMSU_DEF const char* tmsu_find_first_of_unescaped_n(const char* str_first, const char* str_last,
                                                    const char* find_str_first, const char* find_str_last,
                                                    char escape_char);

/* Ignores case for ansi characters. */
TMSU_DEF const char* tmsu_find_char_ignore_case_ansi(const char* str, char c);
TMSU_DEF const char* tmsu_find_ignore_case_ansi(const char* str, const char* find_str);

typedef struct {
    const char* current;
} tmsu_tokenizer;

/* Tokenizer for nullterminated strings. Alternative to strtok. */
TMSU_DEF tmsu_tokenizer tmsu_make_tokenizer(const char* str);
/*
Returns true if a token could be extracted. Delimeters can be different between calls.
The start and length of the token is then stored into the output parameter out.
*/
TMSU_DEF tm_bool tmsu_next_token(tmsu_tokenizer* tokenizer, const char* delimiters, tmsu_string_view* out);

typedef struct {
    const char* first;
    const char* last;
} tmsu_tokenizer_n;

/* Tokenizer for length based strings/iterators. Alternative to strtok. */
TMSU_DEF tmsu_tokenizer_n tmsu_make_tokenizer_n(const char* first, const char* last);
/*
Returns true if a token could be extracted. Delimeters can be different between calls.
The start and length of the token is then stored into the output parameter out.
*/
TMSU_DEF tm_bool tmsu_next_token_n(tmsu_tokenizer_n* tokenizer, const char* delimiters_first,
                                   const char* delimiters_last, tmsu_string_view* out);

/* Whitespace trimming */

TMSU_DEF const char* tmsu_trim_left(const char* str);

/* Trims whitespace from the left. Returns new left/first boundary. */
TMSU_DEF const char* tmsu_trim_left_n(const char* first, const char* last);
/* Trims whitespace from the right. Returns new right/last boundary. */
TMSU_DEF const char* tmsu_trim_right_n(const char* first, const char* last);

/* Comparisons */

/* Lexicographical comparisons. */
TMSU_DEF int tmsu_compare(const char* a, const char* b);
TMSU_DEF int tmsu_compare_ignore_case_ansi(const char* a, const char* b);

TMSU_DEF int tmsu_compare_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);
TMSU_DEF int tmsu_compare_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                             const char* b_last);

/* String comparison for humans. See http://stereopsis.com/strcmp4humans.html. */
TMSU_DEF int tmsu_human_compare_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                       const char* b_last);

/* Equality check, faster than lexicographical compare, since we can check lengths first. */
TMSU_DEF tm_bool tmsu_equals_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);
TMSU_DEF tm_bool tmsu_equals_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                                const char* b_last);

TMSU_DEF tm_bool tmsu_starts_with(const char* str, const char* find_str);
TMSU_DEF tm_bool tmsu_ends_with(const char* str, const char* find_str);

TMSU_DEF tm_bool tmsu_starts_with_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);
TMSU_DEF tm_bool tmsu_ends_with_n(const char* str_first, const char* str_last, const char* find_str_first,
                                  const char* find_str_last);

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi(const char* str, const char* find_str);
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi(const char* str, const char* find_str);

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                                     const char* b_last);
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                   const char* find_str_first, const char* find_str_last);

/* Crt extensions, that are non standard and may not be provided. */

/* Only checks chars from '0' to '9'. */
TMSU_DEF tm_bool tmsu_isdigit(unsigned c);

TMSU_DEF const char* tmsu_stristr(const char* str, const char* find_str);
TMSU_DEF int tmsu_stricmp(const char* a, const char* b);
TMSU_DEF int tmsu_strnicmp(const char* a, const char* b, size_t count);
TMSU_DEF char* tmsu_strrev(char* str);
TMSU_DEF char* tmsu_strnrev(char* str, size_t count);
TMSU_DEF const void* tmsu_memrchr(const void* ptr, int value, size_t len);

#ifdef __cplusplus
}
#endif

/* string_view returning types are only extern C if the string_view isn't a C++ type. */
#if defined(__cplusplus) && !defined(TM_STRING_VIEW)
extern "C" {
#endif /* defined(__cplusplus) && !defined(TM_STRING_VIEW) */

/* Trims whitespace from both sides. */
TMSU_DEF tmsu_string_view tmsu_trim(const char* str);
/* Trims whitespace from both sides. */
TMSU_DEF tmsu_string_view tmsu_trim_n(const char* first, const char* last);

#if defined(__cplusplus) && !defined(TM_STRING_VIEW)
}
#endif /* defined(__cplusplus) && !defined(TM_STRING_VIEW) */

#if defined(__cplusplus) && defined(TM_STRING_VIEW)

const char* tmsu_find_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                const char* not_found);
const char* tmsu_find_first_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_first_not_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_of_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                   const char* not_found);
const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_not_of_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                       const char* not_found);
const char* tmsu_find_ignore_case_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_first_of_unescaped_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                           char escape_char);

TM_STRING_VIEW tmsu_trim_left(TM_STRING_VIEW str);
TM_STRING_VIEW tmsu_trim_right(TM_STRING_VIEW str);
TM_STRING_VIEW tmsu_trim(TM_STRING_VIEW str);

int tmsu_compare(TM_STRING_VIEW a, TM_STRING_VIEW b);
int tmsu_compare_ignore_case(TM_STRING_VIEW a, TM_STRING_VIEW b);
int tmsu_human_compare(TM_STRING_VIEW a, TM_STRING_VIEW b);

tm_bool tmsu_equals(TM_STRING_VIEW a, TM_STRING_VIEW b);
tm_bool tmsu_equals_ignore_case(TM_STRING_VIEW a, TM_STRING_VIEW b);

tm_bool tmsu_starts_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_ends_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_starts_with_ignore_case(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_ends_with_ignore_case(TM_STRING_VIEW str, TM_STRING_VIEW find_str);

tm_bool tmsu_next_token_n(tmsu_tokenizer_n* tokenizer, TM_STRING_VIEW find_str, tmsu_string_view* out);

#endif

#endif /* !defined(_TM_STRINGUTIL_INCLUDED_49458961_DD38_441D_B888_A589548CA6F5_) */

#ifdef TM_STRINGUTIL_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__cplusplus) || !defined(TM_STRING_VIEW)
inline static tmsu_string_view tmsu_make_string_view(const char* str, tm_size_t size) {
    tmsu_string_view result;
    result.data = str;
    result.size = size;
    return result;
}
#endif /* !defined(__cplusplus) || !defined(TM_STRING_VIEW) */

/* Use null of the underlying language. */
#ifndef TM_NULL
    #ifdef __cplusplus
        #define TM_NULL nullptr
    #else
        #define TM_NULL NULL
    #endif
#endif

#define TMSU_C2I(x) ((unsigned char)(x))

inline static tm_size_t tmsu_distance(const char* first, const char* last) {
    TM_ASSERT(first <= last);
    return (tm_size_t)(last - first);
}
inline static size_t tmsu_distance_sz(const char* first, const char* last) {
    TM_ASSERT(first <= last);
    return (size_t)(last - first);
}

TMSU_DEF const char* tmsu_find_char(const char* str, char c) {
    TM_ASSERT(str);
    for (; *str; ++str) {
        if (*str == c) return str;
    }
    return str;
}
TMSU_DEF const char* tmsu_find_char2(const char* str, char a, char b) {
    TM_ASSERT(str);
    for (; *str; ++str) {
        if (*str == a || *str == b) return str;
    }
    return str;
}

TMSU_DEF const char* tmsu_find_last_char(const char* str, char c) {
    TM_ASSERT(str);
    return tmsu_find_last_char_n(str, str + TM_STRLEN(str), c);
}

TMSU_DEF const char* tmsu_find_char_n(const char* str_first, const char* str_last, char c) {
    const void* result = TM_MEMCHR(str_first, TMSU_C2I(c), tmsu_distance_sz(str_first, str_last));
    return (result) ? (const char*)result : str_last;
}
TMSU_DEF const char* tmsu_find_char2_n(const char* str_first, const char* str_last, char a, char b) {
    /* TODO: Is it better to call memchr twice, or do a manual loop? */
#if 1
    size_t len = tmsu_distance_sz(str_first, str_last);
    if (!len) return str_last;
    const char* a_match = (const char*)TM_MEMCHR(str_first, TMSU_C2I(a), len);
    const char* b_match = (const char*)TM_MEMCHR(str_first, TMSU_C2I(b), len);
    if (!a_match) a_match = str_last;
    if (!b_match) b_match = str_last;
    return (a_match < b_match) ? a_match : b_match;
#else
    for (; str_first != str_last; ++str_first) {
        char current = TMSU_C2I(*str_first);
        if (current == a || current == b) return str_first;
    }
    return str_first;
#endif
}

TMSU_DEF const char* tmsu_find_last_char_n_ex(const char* str_first, const char* str_last, char c,
                                              const char* not_found) {
    TM_ASSERT(str_first <= str_last);
#ifdef TM_MEMRCHR
    void* result = TM_MEMRCHR(str_first, TMSU_C2I(c), tmsu_distance_sz(str_first, str_last));
    return (result) ? (const char*)result : not_found;
#else
    const char* cur = str_last;
    while (str_first != cur) {
        --cur;
        if (TMSU_C2I(*cur) == TMSU_C2I(c)) {
            return cur;
        }
    };
    return not_found;
#endif
}
TMSU_DEF const char* tmsu_find_last_char_n(const char* str_first, const char* str_last, char c) {
    return tmsu_find_last_char_n_ex(str_first, str_last, c, str_last);
}

TMSU_DEF const char* tmsu_find_n(const char* str_first, const char* str_last, const char* find_str_first,
                                 const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first;                                       /* Empty string always matches */
    if (find_str_len > tmsu_distance_sz(str_first, str_last)) return str_last; /* Not enough room for a match. */

    /* We can reduce str_last by find_str_len, since the remaining size at the end doesn't allow for a match. */
    str_last -= find_str_len;
    const char* cur = str_first;
    while ((cur = tmsu_find_char_n(cur, str_last, *find_str_first)) != str_last) {
        if (TM_MEMCMP(cur, find_str_first, find_str_len) == 0) {
            return cur;
        }
        ++cur;
    }
    return str_last;
}

TMSU_DEF const char* tmsu_find_last_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                         const char* find_str_last, const char* not_found) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first;                                        /* Empty string always matches. */
    if (find_str_len > tmsu_distance_sz(str_first, str_last)) return not_found; /* Not enough room for a match. */

    /* We can reduce str_last by find_str_len, since the remaining size at the end doesn't allow for a match. */
    str_last -= find_str_len;
    const char* prev = str_last;
    const char* cur = str_last;
    while ((cur = tmsu_find_last_char_n(str_first, prev, *find_str_first)) != prev) {
        if (TM_MEMCMP(cur, find_str_first, find_str_len) == 0) {
            return cur;
        }
        prev = cur;
    }
    return not_found;
}

TMSU_DEF const char* tmsu_find_last_n(const char* str_first, const char* str_last, const char* find_str_first,
                                      const char* find_str_last) {
    return tmsu_find_last_n_ex(str_first, str_last, find_str_first, find_str_last, str_last);
}

TMSU_DEF const char* tmsu_find(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

#ifdef TM_STRSTR
    return TM_STRSTR(str, find_str);
#else
    /* TODO: is this better than calling the _n variant with two strlens? */
#if 0
    return tmsu_find_n(str, str + TM_STRLEN(str), find_str, TM_STRLEN(find_str));
#else
    if (!*find_str) return str;
    for (;;) {
        while (*str && *str != *find_str) {
            ++str;
        }
        if (!*str) {
            return str;
        }
        const char* other = find_str;
        while (*str && *other && *str == *other) {
            ++str;
            ++other;
        }
        if (!*str && !*other) {
            return str;
        }
    }
#endif
#endif
}

TMSU_DEF const char* tmsu_find_last(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    return tmsu_find_last_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF const char* tmsu_find_first_not_of(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    while (*str && TM_STRCHR(find_str, TMSU_C2I(*str))) {
        ++str;
    }
    return str;
}

TMSU_DEF const char* tmsu_find_first_of(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    while (*str && TM_STRCHR(find_str, TMSU_C2I(*str)) == TM_NULL) {
        ++str;
    }
    return str;
}

TMSU_DEF const char* tmsu_find_last_not_of(const char* str, const char* find_str) {
    const char* last = str + TM_STRLEN(str) - 1;
    while (last != str && TM_STRCHR(find_str, TMSU_C2I(*last))) {
        --last;
    }
    return last;
}

TMSU_DEF const char* tmsu_find_last_of(const char* str, const char* find_str) {
    const char* last = str + TM_STRLEN(str) - 1;
    while (last != str && TM_STRCHR(find_str, TMSU_C2I(*last)) == TM_NULL) {
        --last;
    }
    return last;
}

TMSU_DEF const char* tmsu_find_first_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                          const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);
    if (find_str_first == find_str_last) return str_first;

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_first != str_last && TM_MEMCHR(find_str_first, TMSU_C2I(*str_first), find_str_len) == TM_NULL) {
        ++str_first;
    }
    return str_first;
}

TMSU_DEF const char* tmsu_find_first_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                              const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first; /* Empty always matches. */
    while (str_first != str_last && TM_MEMCHR(find_str_first, TMSU_C2I(*str_first), find_str_len)) {
        ++str_first;
    }
    return str_first;
}

TMSU_DEF const char* tmsu_find_last_of_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                            const char* find_str_last, const char* not_found) {
    if (str_first == str_last) {
        return not_found;
    }
    --str_last;
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_last != str_first && TM_MEMCHR(find_str_first, TMSU_C2I(*str_last), find_str_len) == TM_NULL) {
        --str_last;
    }
    return not_found;
}

TMSU_DEF const char* tmsu_find_last_not_of_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                                const char* find_str_last, const char* not_found) {
    if (str_first == str_last) {
        return not_found;
    }
    --str_last;
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_last != str_first && TM_MEMCHR(find_str_first, TMSU_C2I(*str_last), find_str_len)) {
        --str_last;
    }
    return not_found;
}

TMSU_DEF const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                         const char* find_str_last) {
    return tmsu_find_last_of_n_ex(str_first, str_last, find_str_first, find_str_last, str_last);
}

TMSU_DEF const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                             const char* find_str_last) {
    return tmsu_find_last_not_of_n_ex(str_first, str_last, find_str_first, find_str_last, str_last);
}

/* Case insensitive */

TMSU_DEF const char* tmsu_find_char_ignore_case_ansi(const char* str, char c) {
    char upper = (char)TM_TOUPPER(TMSU_C2I(c));
    char lower = (char)TM_TOLOWER(TMSU_C2I(c));
    if (upper == lower) {
        return tmsu_find_char(str, c);
    }
    return tmsu_find_char2(str, (char)upper, (char)lower);
}
TMSU_DEF const char* tmsu_find_ignore_case_ansi(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    return tmsu_find_ignore_case_ansi_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF const char* tmsu_find_char_ignore_case_ansi_n(const char* str_first, const char* str_last, char c) {
    TM_ASSERT(str_first <= str_last);

    size_t len = tmsu_distance_sz(str_first, str_last);
    if (!len) return str_last;
    char upper = (char)(TM_TOUPPER(TMSU_C2I(c)));
    char lower = (char)(TM_TOLOWER(TMSU_C2I(c)));
    if (upper == lower) {
        const char* match = (const char*)TM_MEMCHR(str_first, TMSU_C2I(c), len);
        return (match) ? match : str_last;
    }
    return tmsu_find_char2_n(str_first, str_last, upper, lower);
}
TMSU_DEF const char* tmsu_find_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                  const char* find_str_first, const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first;                                       /* Empty string always matches. */
    if (find_str_len > tmsu_distance_sz(str_first, str_last)) return str_last; /* Not enough room for a match. */

    /* We can reduce str_last by find_str_len, since the remaining size at the end doesn't allow for a match. */
    str_last -= find_str_len;
    const char* cur = str_first;
    while ((cur = tmsu_find_char_ignore_case_ansi_n(cur, str_last, *find_str_first)) != str_last) {
        if (tmsu_compare_ignore_case_ansi_n(cur, cur + find_str_len, find_str_first, find_str_last) == 0) {
            return cur;
        }
        ++cur;
    }
    return str_last;
}

/* Find functions that allow escaping of the character to look for. Useful for parsing. */

static tm_bool tmsu_is_char_unescaped(const char* first, const char* last, char escape_char) {
    /* Count how many escape chars precede last. */
    const char* pos = last;
    while (pos > first && *(pos - 1) == escape_char) --pos;
    TM_ASSERT(last >= pos);
    size_t preceding_escape_chars_count = (size_t)(last - pos);
    /* If there are even number of escape chars before last, they all escaped each other. */
    return (preceding_escape_chars_count & 1) == 0;
}

TMSU_DEF const char* tmsu_find_char_unescaped(const char* str, char c, char escape_char) {
    TM_ASSERT(str);
    if (c != escape_char) {
        if (*str == c) return str;
        const char* start = str;
        for (;;) {
            str = tmsu_find_char(str + 1, c);
            if (!*str) return str;
            if (tmsu_is_char_unescaped(start, str, escape_char)) return str;
        }
    } else {
        /* We are looking for an unescaped escape char.
         * Since we are doing lookahead, we don't need to backtrack like in the other case. */
        for (;;) {
            str = tmsu_find_char(str, c);
            if (!*str) return str;
            if (*(str + 1) == c) {
                str += 2;
                continue;
            }
            return str;
        }
    }
}
TMSU_DEF const char* tmsu_find_first_of_unescaped(const char* str, const char* find_str, char escape_char) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    if (!*find_str) return str; /* Empty always matches. */
    const char* first = str;
    for (;;) {
        str = tmsu_find_first_of(str, find_str);
        if (!*str) return str;
        if (*str == escape_char) {
            if (*(str + 1) == escape_char) {
                str += 2;
                first = str;
                continue;
            }
            return str;
        }
        if (tmsu_is_char_unescaped(first, str, escape_char)) break;
        ++str;
    }
    return str;
}
TMSU_DEF const char* tmsu_find_char_unescaped_n(const char* str_first, const char* str_last, char c, char escape_char) {
    TM_ASSERT(str_first <= str_last);
    if (str_first == str_last) return str_last;

    const char* str = str_first;
    if (c != escape_char) {
        if (*str == c) return str;
        for (;;) {
            str = tmsu_find_char_n(str + 1, str_last, c);
            if (str == str_last) return str;
            if (tmsu_is_char_unescaped(str_first, str, escape_char)) return str;
        }
    } else {
        /* We are looking for an unescaped escape char.
         * Since we are doing lookahead, we don't need to backtrack like in the other case. */
        for (;;) {
            str = tmsu_find_char_n(str, str_last, c);
            if (str == str_last) return str;
            if (str + 1 != str_last && *(str + 1) == c) {
                str += 2;
                continue;
            }
            return str;
        }
    }
}
TMSU_DEF const char* tmsu_find_first_of_unescaped_n(const char* str_first, const char* str_last,
                                                    const char* find_str_first, const char* find_str_last,
                                                    char escape_char) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);
    if (str_first == str_last) return str_last;
    if (find_str_first == find_str_last) return str_first; /* Empty string always matches. */

    const char* str = str_first;
    for (;;) {
        str = tmsu_find_first_of_n(str, str_last, find_str_first, find_str_last);
        if (str == str_last) return str;
        if (*str == escape_char) {
            if (str + 1 != str_last && *(str + 1) == escape_char) {
                str += 2;
                str_first = str;
                continue;
            }
            return str;
        }
        if (tmsu_is_char_unescaped(str_first, str, escape_char)) break;
        ++str;
    }
    return str;
}

/* Tokenizer */

TMSU_DEF tmsu_tokenizer tmsu_make_tokenizer(const char* str) {
    TM_ASSERT(str);
    tmsu_tokenizer result;
    result.current = str;
    return result;
}

TMSU_DEF tm_bool tmsu_next_token(tmsu_tokenizer* tokenizer, const char* delimiters, tmsu_string_view* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->current);
    TM_ASSERT(delimiters);

    /* Skip delimiters at the beginning. */
    tokenizer->current = tmsu_find_first_not_of(tokenizer->current, delimiters);
    if (!*tokenizer->current) return TM_FALSE;
    /* Skip skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of(tokenizer->current, delimiters);
    if (out) {
        *out = TMSU_STRING_VIEW_MAKE(tokenizer->current, tmsu_distance(tokenizer->current, next));
    }
    tokenizer->current = next;
    return TM_TRUE;
}

TMSU_DEF tmsu_tokenizer_n tmsu_make_tokenizer_n(const char* first, const char* last) {
    TM_ASSERT(first && first <= last);

    tmsu_tokenizer_n result;
    result.first = first;
    result.last = last;
    return result;
}

TMSU_DEF tm_bool tmsu_next_token_n(tmsu_tokenizer_n* tokenizer, const char* delimiters_first,
                                   const char* delimiters_last, tmsu_string_view* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->first && tokenizer->first <= tokenizer->last);
    TM_ASSERT(delimiters_first && delimiters_first <= delimiters_last);

    /* Skip delimiters at the beginning. */
    tokenizer->first = tmsu_find_first_not_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (tokenizer->first == tokenizer->last) return TM_FALSE;
    /* Skip skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (out) *out = TMSU_STRING_VIEW_MAKE(tokenizer->first, tmsu_distance(tokenizer->first, next));
    tokenizer->first = next;
    return TM_TRUE;
}

/* Whitespace trimming */

static const int TMSU_WHITESPACE_COUNT = 6;
static char const* const TMSU_WHITESPACE = " \t\n\v\f\r";

TMSU_DEF const char* tmsu_trim_left(const char* str) { return tmsu_find_first_not_of(str, TMSU_WHITESPACE); }

TMSU_DEF const char* tmsu_trim_left_n(const char* first, const char* last) {
    return tmsu_find_first_not_of_n(first, last, TMSU_WHITESPACE, TMSU_WHITESPACE + TMSU_WHITESPACE_COUNT);
}
TMSU_DEF const char* tmsu_trim_right_n(const char* first, const char* last) {
    return tmsu_find_last_not_of_n_ex(first, last, TMSU_WHITESPACE, TMSU_WHITESPACE + TMSU_WHITESPACE_COUNT, first);
}

/* Comparisons */

TMSU_DEF int tmsu_compare(const char* a, const char* b) {
    TM_ASSERT(a);
    TM_ASSERT(b);
#ifdef TM_STRCMP
    return TM_STRCMP(a, b);
#else
    while (*a && *b && *a == *b) {
        ++a;
        ++b;
    }
    return *b - *a;
#endif
}
TMSU_DEF int tmsu_compare_ignore_case_ansi(const char* a, const char* b) {
    TM_ASSERT(a);
    TM_ASSERT(b);
#ifdef TM_STRICMP
    return TM_STRICMP(a, b);
#else
    while (*a && *b && TM_TOUPPER(TMSU_C2I(*a)) == TM_TOUPPER(TMSU_C2I(*b))) {
        ++a;
        ++b;
    }
    return TM_TOUPPER(TMSU_C2I(*b)) - TM_TOUPPER(TMSU_C2I(*a));
#endif
}

TMSU_DEF int tmsu_compare_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !b_len - !a_len;
    if (a_first == b_first) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    size_t len = (a_len < b_len) ? a_len : b_len;
    if (len) {
        int common_cmp = TM_MEMCMP(a_first, b_first, len);
        if (common_cmp != 0) return common_cmp;
    }

    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    return 0;
}
TMSU_DEF int tmsu_compare_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                             const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !b_len - !a_len;
    if (a_first == b_first) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    size_t len = (a_len < b_len) ? a_len : b_len;
    for (size_t i = 0; i < len; ++i) {
        int a = TM_TOUPPER(TMSU_C2I(a_first[i]));
        int b = TM_TOUPPER(TMSU_C2I(b_first[i]));
        int diff = a - b;
        if (diff != 0) return diff;
    }

    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    return 0;
}

/*
tmsu_human_compare_n implementation is based on this gist: https://gist.github.com/pervognsen/733034 by Per Vognsen.
*/
static int tmsu_human_parse_and_advance(const char** str, const char* last) {
    const char* p = *str;
    TM_ASSERT(p != last);
    if (!tmsu_isdigit(TMSU_C2I(*p))) {
        ++*str;
        return TM_TOUPPER(TMSU_C2I(*p));
    }

    int result = 0;
    while (p != last && tmsu_isdigit(*p)) {
        result = (result * 10) + (*p - '0');
        ++p;
    }
    *str = p;
    return result + 256;
}

TMSU_DEF int tmsu_human_compare_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                       const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !b_len - !a_len;
    if (a_first == b_first) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    while (a_first != a_last && b_first != b_last) {
        int a = tmsu_human_parse_and_advance(&a_first, a_last);
        int b = tmsu_human_parse_and_advance(&b_first, b_last);
        int diff = a - b;
        if (diff != 0) return diff;
    }

    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    return 0;
}

TMSU_DEF tm_bool tmsu_equals_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last) {
    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (a_len != b_len) return TM_FALSE;
    return TM_MEMCMP(a_first, b_first, a_len) == 0;
}
TMSU_DEF tm_bool tmsu_equals_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                                const char* b_last) {
    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (a_len != b_len) return TM_FALSE;
    for (size_t i = 0; i < a_len; ++i) {
        int a = TM_TOUPPER(TMSU_C2I(a_first[i]));
        int b = TM_TOUPPER(TMSU_C2I(b_first[i]));
        if (a != b) return TM_FALSE;
    }
    return TM_TRUE;
}

TMSU_DEF tm_bool tmsu_starts_with(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    size_t find_str_len = TM_STRLEN(find_str);
    return TM_STRNCMP(str, find_str, find_str_len) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    return tmsu_ends_with_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF tm_bool tmsu_starts_with_n(const char* str_first, const char* str_last, const char* find_str_first,
                                    const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    return TM_MEMCMP(str_first, find_str_first, find_str_len) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with_n(const char* str_first, const char* str_last, const char* find_str_first,
                                  const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    const char* start = str_last - find_str_len;
    return TM_MEMCMP(start, find_str_first, find_str_len) == 0;
}

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    size_t find_str_len = TM_STRLEN(find_str);
    return tmsu_strnicmp(str, find_str, find_str_len) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    return tmsu_ends_with_ignore_case_ansi_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                     const char* find_str_first, const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    return tmsu_compare_ignore_case_ansi_n(str_first, str_first + find_str_len, find_str_first, find_str_last) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                   const char* find_str_first, const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    const char* start = str_last - find_str_len;
    return tmsu_compare_ignore_case_ansi_n(start, str_last, find_str_first, find_str_last) == 0;
}

/* Crt extensions */

TMSU_DEF tm_bool tmsu_isdigit(unsigned c) { return c >= '0' && c <= '9'; }

TMSU_DEF const char* tmsu_stristr(const char* str, const char* find_str) {
    return tmsu_find_ignore_case_ansi(str, find_str);
}
TMSU_DEF int tmsu_stricmp(const char* a, const char* b) {
    while (*a && *b) {
        int aUpper = TM_TOUPPER(TMSU_C2I(*a));
        int bUpper = TM_TOUPPER(TMSU_C2I(*b));
        if (aUpper != bUpper) {
            break;
        }
        ++a;
        ++b;
    }
    int aUpper = TM_TOUPPER(TMSU_C2I(*a));
    int bUpper = TM_TOUPPER(TMSU_C2I(*b));
    return aUpper - bUpper;
}
TMSU_DEF int tmsu_strnicmp(const char* a, const char* b, size_t count) {
    while (*a && *b && count--) {
        int aUpper = TM_TOUPPER(TMSU_C2I(*a));
        int bUpper = TM_TOUPPER(TMSU_C2I(*b));
        if (aUpper != bUpper) {
            break;
        }
        ++a;
        ++b;
    }
    if (!count) {
        return 0;
    }
    int aUpper = TM_TOUPPER(TMSU_C2I(*a));
    int bUpper = TM_TOUPPER(TMSU_C2I(*b));
    return aUpper - bUpper;
}
TMSU_DEF char* tmsu_strrev(char* str) { return tmsu_strnrev(str, TM_STRLEN(str)); }
TMSU_DEF char* tmsu_strnrev(char* str, size_t count) {
    for (size_t i = 0, j = count - 1; i < j; ++i, --j) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
    return str;
}
TMSU_DEF const void* tmsu_memrchr(const void* ptr, int value, size_t len) {
    const char* p = (const char*)ptr + len;
    while (len) {
        --len;
        --p;
        if ((unsigned char)*p == value) {
            return (const void*)p;
        }
    }
    return TM_NULL;
}

#ifdef __cplusplus
}
#endif

#if defined(__cplusplus) && defined(TM_STRING_VIEW)

#define TMSU_SV_BEGIN(str) TM_STRING_VIEW_DATA(str)
#define TMSU_SV_END(str) (TM_STRING_VIEW_DATA(str) + TM_STRING_VIEW_SIZE(str))

const char* tmsu_find_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_last_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                const char* not_found) {
    return tmsu_find_last_n_ex(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str), not_found);
}
const char* tmsu_find_first_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_first_of_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_first_not_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_first_not_of_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_last_of_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_of_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                   const char* not_found) {
    return tmsu_find_last_of_n_ex(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str), not_found);
}
const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_last_not_of_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_not_of_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                       const char* not_found) {
    return tmsu_find_last_not_of_n_ex(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str), not_found);
}
const char* tmsu_find_ignore_case_ansi_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_ignore_case_ansi_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_first_of_unescaped_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                           char escape_char) {
    return tmsu_find_first_of_unescaped_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str),
                                          escape_char);
}

TM_STRING_VIEW tmsu_trim_left(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    first = tmsu_trim_left_n(first, last);
    return TM_STRING_VIEW_MAKE(first, tmsu_distance_sz(first, last));
}
TM_STRING_VIEW tmsu_trim_right(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    last = tmsu_trim_right_n(first, last);
    return TM_STRING_VIEW_MAKE(first, tmsu_distance_sz(first, last));
}
TM_STRING_VIEW tmsu_trim(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    first = tmsu_trim_left_n(first, last);
    last = tmsu_trim_right_n(first, last);
    return TM_STRING_VIEW_MAKE(first, tmsu_distance_sz(first, last));
}

int tmsu_compare(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_compare_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}
int tmsu_compare_ignore_case_ansi(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_compare_ignore_case_ansi_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}
int tmsu_human_compare_ansi(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_human_compare_ansi_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}

tm_bool tmsu_equals(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_equals_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}
tm_bool tmsu_equals_ignore_case_ansi(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_equals_ignore_case_ansi_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}

tm_bool tmsu_starts_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_starts_with_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
tm_bool tmsu_ends_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_ends_with_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
tm_bool tmsu_starts_with_ignore_case_ansi(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_starts_with_ignore_case_ansi_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str),
                                                TMSU_SV_END(find_str));
}

tm_bool tmsu_ends_with_ignore_case_ansi(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_ends_with_ignore_case_ansi_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str),
                                              TMSU_SV_END(find_str));
}

tm_bool tmsu_next_token_n(tmsu_tokenizer_n* tokenizer, TM_STRING_VIEW find_str, tmsu_string_view* out) {
    return tmsu_next_token_n(tokenizer, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str), out);
}

#undef TMSU_SV_BEGIN
#undef TMSU_SV_END

#endif

/* string_view returning types are only extern C if the string_view isn't a C++ type. */
#if defined(__cplusplus) && !defined(TM_STRING_VIEW)
extern "C" {
#endif /* defined(__cplusplus) && !defined(TM_STRING_VIEW) */

TMSU_DEF tmsu_string_view tmsu_trim(const char* str) {
    TM_ASSERT(str);
    return tmsu_trim_n(str, str + TM_STRLEN(str));
}
TMSU_DEF tmsu_string_view tmsu_trim_n(const char* first, const char* last) {
    first = tmsu_trim_left_n(first, last);
    last = tmsu_trim_right_n(first, last);
    return TMSU_STRING_VIEW_MAKE(first, tmsu_distance(first, last));
}

#if defined(__cplusplus) && !defined(TM_STRING_VIEW)
}
#endif /* defined(__cplusplus) && !defined(TM_STRING_VIEW) */

#endif /* defined(TM_STRINGUTIL_IMPLEMENTATION) */

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
