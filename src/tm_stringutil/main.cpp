/*
tm_stringutil.h v0.1 - public domain - https://github.com/to-miz/tm
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
    v0.1   07.10.18 initial commit
*/

#include "../common/GENERATED_WARNING.inc"

/* clang-format off */
#ifdef TM_STRINGUTIL_IMPLEMENTATION
    /* Define these to get rid of crt dependency. */

    #include "../common/tm_assert.inc"

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
        #ifndef TM_STRSTR
            #define TM_STRSTR strstr
        #endif
        #ifndef TM_STRNCMP
            #define TM_STRNCMP strncmp
        #endif
        #ifndef TM_MEMCMP
            #define TM_MEMCMP memcmp
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

#include "../common/tm_size_t.inc"

#include "../common/tm_bool.inc"

/* Linkage defaults to extern, to override define TMSU_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMSU_DEF
    #define TMSU_DEF extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format on */

/*
String view type for C. For C++ string views, there are special overloads for them if TM_STRINGVIEW is defined with
the string view type.
*/
typedef struct {
    const char* data;
    tm_size_t size;
} tmsu_stringview;
inline static tmsu_stringview tmsu_make_stringview(const char* data, tm_size_t size) {
    tmsu_stringview result;
    result.data = data;
    result.size = size;
    return result;
}
inline static const char* tmsu_stringview_begin(tmsu_stringview v) { return v.data; }
inline static const char* tmsu_stringview_end(tmsu_stringview v) { return v.data + v.size; }

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

/* Ignores case for ascii range utf8 characters. */
TMSU_DEF const char* tmsu_find_char_ignore_case(const char* str, char c);
TMSU_DEF const char* tmsu_find_ignore_case(const char* str, const char* find_str);

/*
Find functions for length based strings/iterators. They return str_last if no match is found (even the *_last versions).
Params:
    str_first, str_last:           Iterator pair to search in. They can be equal for empty ranges.
    find_str_first, find_str_last: Iterator pair to search for. They can be equal for empty ranges.
Return pointer to first character of the match, or str_last if no match is found (even the *_last versions).
*/
TMSU_DEF const char* tmsu_find_char_n(const char* str_first, const char* str_last, char c);
TMSU_DEF const char* tmsu_find_char2_n(const char* str_first, const char* str_last, char a, char b);
TMSU_DEF const char* tmsu_find_last_char_n(const char* str_first, const char* str_last, char c);
TMSU_DEF const char* tmsu_find_n(const char* str_first, const char* str_last, const char* find_str_first,
                                 const char* find_str_last);
TMSU_DEF const char* tmsu_find_last_n(const char* str_first, const char* str_last, const char* find_str_first,
                                      const char* find_str_last);
TMSU_DEF const char* tmsu_find_first_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                          const char* find_str_last);
TMSU_DEF const char* tmsu_find_first_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                              const char* find_str_last);
TMSU_DEF const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                         const char* find_str_last);
TMSU_DEF const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                             const char* find_str_last);

/* Ignores case for ascii range utf8 characters. */
TMSU_DEF const char* tmsu_find_char_ignore_case_n(const char* str_first, const char* str_last, char c);
TMSU_DEF const char* tmsu_find_ignore_case_n(const char* str_first, const char* str_last, const char* find_str_first,
                                             const char* find_str_last);

typedef struct {
    const char* current;
} tmsu_tokenizer;

/* Tokenizer for nullterminated strings. Alternative to strtok. */
TMSU_DEF tmsu_tokenizer tmsu_make_tokenizer(const char* str);
/*
Returns true if a token could be extracted. Delimeters can be different between calls.
The start and length of the token is then stored into the output parameter out.
*/
TMSU_DEF tm_bool tmsu_next_token(tmsu_tokenizer* tokenizer, const char* delimiters, tmsu_stringview* out);

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
                                   const char* delimiters_last, tmsu_stringview* out);

/* Whitespace trimming */

TMSU_DEF const char* tmsu_trim_left(const char* str);
/* Trims whitespace from both sides. */
TMSU_DEF tmsu_stringview tmsu_trim(const char* str);

/* Trims whitespace from the left. Returns new left/first boundary. */
TMSU_DEF const char* tmsu_trim_left_n(const char* first, const char* last);
/* Trims whitespace from the right. Returns new right/last boundary. */
TMSU_DEF const char* tmsu_trim_right_n(const char* first, const char* last);
/* Trims whitespace from both sides. */
TMSU_DEF tmsu_stringview tmsu_trim_n(const char* first, const char* last);

/* Comparisons */

TMSU_DEF int tmsu_compare_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);
TMSU_DEF int tmsu_compare_ignore_case_n(const char* a_first, const char* a_last, const char* b_first,
                                        const char* b_last);
/* String comparison for humans. See http://stereopsis.com/strcmp4humans.html. */
TMSU_DEF int tmsu_human_compare_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);

TMSU_DEF tm_bool tmsu_starts_with(const char* str, const char* find_str);
TMSU_DEF tm_bool tmsu_ends_with(const char* str, const char* find_str);

TMSU_DEF tm_bool tmsu_starts_with_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);
TMSU_DEF tm_bool tmsu_ends_with_n(const char* str_first, const char* str_last, const char* find_str_first,
                                  const char* find_str_last);

TMSU_DEF tm_bool tmsu_starts_with_ignore_case(const char* str, const char* find_str);
TMSU_DEF tm_bool tmsu_ends_with_ignore_case(const char* str, const char* find_str);

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_n(const char* a_first, const char* a_last, const char* b_first,
                                                const char* b_last);
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_n(const char* str_first, const char* str_last, const char* find_str_first,
                                              const char* find_str_last);

/* Crt extensions, that are non standard and may not be provided. */

/* Only checks chars from '0' to '9'. Assumes utf8 codepoint. */
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

#if defined(__cplusplus) && defined(TM_STRING_VIEW)

#include "string_view_overloads.h"

#endif

#endif /* !defined(_TM_STRINGUTIL_INCLUDED_49458961_DD38_441D_B888_A589548CA6F5_) */

#ifdef TM_STRINGUTIL_IMPLEMENTATION

#include "implementation.cpp"

#if defined(__cplusplus) && defined(TM_STRING_VIEW)

#include "string_view_overloads.cpp"

#endif

#endif /* defined(TM_STRINGUTIL_IMPLEMENTATION) */

#include "../common/LICENSE.inc"
