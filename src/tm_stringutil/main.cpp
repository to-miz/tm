/*
tm_stringutil.h v0.9.3 - public domain - https://github.com/to-miz/tm
author: Tolga Mizrak MERGE_YEAR

no warranty; use at your own risk

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

HISTORY    (DD.MM.YY)
    v0.9.3  05.12.20  Tokenizer does not skip empty tokens anymore. Instead there is tmsu_next_token_skip_empty.
    v0.9.2  04.12.20  Removed C++ string_view overloads, instead there is tmsu_view_t for C.
                      The C++ string_view apis overloads were sort of useless, since a C++
                      string_view class would already have most of these functions as methods.
                      Fixed some bugs with tmsu_find.
    v0.9.1  20.04.20  Added tmsu_sv_begin and tmsu_sv_end convenience functions.
    v0.9.0  03.04.20  Added tmsu_base64_decode, tmsu_base64url_encode, tmsu_base64url_decode,
                      tmsu_url_encode, tmsu_url_decode.
    v0.3.4  02.05.19  Added tmsu_find_word_end_n and tmsu_find_word_start_n.
    v0.3.3  06.03.19  Added optional defines for TM_STRCSPN and TM_STRSPN to make use
                      of CRT if it is present.
                      Fixed a C compilation error due to use of auto.
    v0.3.2  03.03.19  Fixed a bug in tmsu_find_last_not_of_n_ex and tmsu_trim_right_n.
    v0.3.1  15.01.19  Fixed some warnings in msvc with signed size_t and string_view.
    v0.3    15.10.18  Added more string_view overloads.
    v0.2    14.10.18  Fixed an tmsu_compare functions to do proper lexicographical comparison.
                      Added tmsu_equals.
                      Fixed MSVC warning about unreachable code.
    v0.1    07.10.18  Initial commit.
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
        #ifndef TM_STRNCMP
            #define TM_STRNCMP strncmp
        #endif
        #ifndef TM_MEMCMP
            #define TM_MEMCMP memcmp
        #endif

        /* Optional defines. */
        #ifndef TM_STRCMP
            #define TM_STRCMP strcmp
        #endif
        #ifndef TM_STRICMP
            #ifdef WIN32
                #define TM_STRICMP _stricmp
            #endif
        #endif
        #ifndef TM_STRCSPN
            #define TM_STRCSPN strcspn
        #endif
        #ifndef TM_STRSPN
            #define TM_STRSPN strspn
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

#include "../common/tm_string_view.inc"

/* clang-format on */

typedef struct tmsu_view_struct {
    const char* first;
    const char* last;

#if defined(__cplusplus) && defined(TM_STRING_VIEW)
    inline operator TM_STRING_VIEW() const {
        TM_ASSERT(first <= last);
        tm_size_t len = (tm_size_t)(last - first);
        return TM_STRING_VIEW_MAKE(first, len);
    }
#endif
} tmsu_view_t;

TMSU_DEF tmsu_view_t tmsu_view(const char* str);
TMSU_DEF tmsu_view_t tmsu_view_n(const char* first, const char* last);
TMSU_DEF tmsu_view_t tmsu_view_l(const char* str, tm_size_t len);
TMSU_DEF tm_bool tmsu_view_empty(tmsu_view_t str);
TMSU_DEF const char* tmsu_view_data(tmsu_view_t str);
TMSU_DEF tm_size_t tmsu_view_size(tmsu_view_t str);

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

TMSU_DEF const char* tmsu_find_v(tmsu_view_t str, tmsu_view_t find_str);
TMSU_DEF const char* tmsu_find_last_v(tmsu_view_t str, tmsu_view_t find_str);
TMSU_DEF const char* tmsu_find_last_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found);
TMSU_DEF const char* tmsu_find_first_of_v(tmsu_view_t str, tmsu_view_t find_str);
TMSU_DEF const char* tmsu_find_first_not_of_v(tmsu_view_t str, tmsu_view_t find_str);
TMSU_DEF const char* tmsu_find_last_of_v(tmsu_view_t str, tmsu_view_t find_str);
TMSU_DEF const char* tmsu_find_last_of_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found);
TMSU_DEF const char* tmsu_find_last_not_of_v(tmsu_view_t str, tmsu_view_t find_str);
TMSU_DEF const char* tmsu_find_last_not_of_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found);

/* Ignores case for ansi characters. */
TMSU_DEF const char* tmsu_find_char_ignore_case_ansi(const char* str, char c);
TMSU_DEF const char* tmsu_find_ignore_case_ansi(const char* str, const char* find_str);

TMSU_DEF const char* tmsu_find_char_ignore_case_ansi_n(const char* str_first, const char* str_last, char c);
TMSU_DEF const char* tmsu_find_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                  const char* find_str_first, const char* find_str_last);

TMSU_DEF const char* tmsu_find_char_ignore_case_ansi_v(tmsu_view_t str, char c);
TMSU_DEF const char* tmsu_find_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str);

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
TMSU_DEF const char* tmsu_find_char_unescaped_v(tmsu_view_t str, char c, char escape_char);
TMSU_DEF const char* tmsu_find_first_of_unescaped_v(tmsu_view_t str, tmsu_view_t find_str, char escape_char);

/* Tokenizing */

typedef struct {
    const char* current;
} tmsu_tokenizer_t;

/* Tokenizer for nullterminated strings. Alternative to strtok. */
TMSU_DEF tmsu_tokenizer_t tmsu_tokenizer(const char* str);
/*
Returns true if a token could be extracted. delimiters can be different between calls.
The view of the token is then stored into the output parameter out.
*/
TMSU_DEF tm_bool tmsu_next_token(tmsu_tokenizer_t* tokenizer, const char* delimiters, tmsu_view_t* out);
TMSU_DEF tm_bool tmsu_next_token_skip_empty(tmsu_tokenizer_t* tokenizer, const char* delimiters, tmsu_view_t* out);
TMSU_DEF tm_bool tmsu_next_token_n(tmsu_view_t* tokenizer, const char* delimiters_first,
                                   const char* delimiters_last, tmsu_view_t* out);
TMSU_DEF tm_bool tmsu_next_token_skip_empty_n(tmsu_view_t* tokenizer, const char* delimiters_first,
                                              const char* delimiters_last, tmsu_view_t* out);
TMSU_DEF tm_bool tmsu_next_token_v(tmsu_view_t* tokenizer, tmsu_view_t delimiters, tmsu_view_t* out);
TMSU_DEF tm_bool tmsu_next_token_skip_empty_v(tmsu_view_t* tokenizer, tmsu_view_t delimiters, tmsu_view_t* out);

/* Word tokenizing. */

/*
Returns the end of a word.
Behavior is similar to pressing Control+Right on most editors.
Default word seperators are: " \t\n\v\f\r./\\()\"'-:,.;<>~!@#$%^&*|+=[]{}`~?".
*/
TMSU_DEF const char* tmsu_find_word_end(const char* str);
TMSU_DEF const char* tmsu_find_word_end_ex(const char* str, const char* word_seperators);
TMSU_DEF const char* tmsu_find_word_end_n(const char* first, const char* last);
TMSU_DEF const char* tmsu_find_word_end_n_ex(const char* first, const char* last, const char* word_seperators_first,
                                             const char* word_seperators_last);
TMSU_DEF const char* tmsu_find_word_end_v(tmsu_view_t str);
TMSU_DEF const char* tmsu_find_word_end_v_ex(tmsu_view_t str, tmsu_view_t word_separators);

/*!
Returns the start of a word by doing a reverse search from the end of the string and moving backwards.
Behavior is similar to pressing Control+Left on most editors.
Default word seperators are: " \t\n\v\f\r./\\()\"'-:,.;<>~!@#$%^&*|+=[]{}`~?".
*/
TMSU_DEF const char* tmsu_find_word_start_n(const char* first, const char* last);
TMSU_DEF const char* tmsu_find_word_start_n_ex(const char* first, const char* last, const char* word_seperators_first,
                                               const char* word_seperators_last);
TMSU_DEF const char* tmsu_find_word_start_v(tmsu_view_t str);
TMSU_DEF const char* tmsu_find_word_start_v_ex(tmsu_view_t str, tmsu_view_t word_separators);

/* Whitespace trimming */

TMSU_DEF const char* tmsu_trim_left(const char* str);

/* Trims whitespace from the left. Returns new left/first boundary. */
TMSU_DEF const char* tmsu_trim_left_n(const char* first, const char* last);
TMSU_DEF const char* tmsu_trim_left_v(tmsu_view_t str);
/* Trims whitespace from the right. Returns new right/last boundary. */
TMSU_DEF const char* tmsu_trim_right_n(const char* first, const char* last);
TMSU_DEF const char* tmsu_trim_right_v(tmsu_view_t str);

TMSU_DEF tmsu_view_t tmsu_trim_n(const char* first, const char* last);
TMSU_DEF tmsu_view_t tmsu_trim_v(tmsu_view_t str);

/* Comparisons */

/* Lexicographical comparisons. */
TMSU_DEF int tmsu_compare(const char* a, const char* b);
TMSU_DEF int tmsu_compare_ignore_case_ansi(const char* a, const char* b);

TMSU_DEF int tmsu_compare_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);
TMSU_DEF int tmsu_compare_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                             const char* b_last);

TMSU_DEF int tmsu_compare_v(tmsu_view_t a, tmsu_view_t b);
TMSU_DEF int tmsu_compare_ignore_case_ansi_v(tmsu_view_t a, tmsu_view_t b);

/* String comparison for humans. See http://stereopsis.com/strcmp4humans.html. */
TMSU_DEF int tmsu_human_compare_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                       const char* b_last);
TMSU_DEF int tmsu_human_compare_ansi_v(tmsu_view_t a, tmsu_view_t b);

/* Equality check, faster than lexicographical compare, since we can check lengths first. */
TMSU_DEF tm_bool tmsu_equals_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);
TMSU_DEF tm_bool tmsu_equals_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                                const char* b_last);

TMSU_DEF tm_bool tmsu_equals_v(tmsu_view_t a, tmsu_view_t b);
TMSU_DEF tm_bool tmsu_equals_ignore_case_ansi_v(tmsu_view_t a, tmsu_view_t b);

TMSU_DEF tm_bool tmsu_starts_with(const char* str, const char* find_str);
TMSU_DEF tm_bool tmsu_ends_with(const char* str, const char* find_str);

TMSU_DEF tm_bool tmsu_starts_with_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last);
TMSU_DEF tm_bool tmsu_ends_with_n(const char* str_first, const char* str_last, const char* find_str_first,
                                  const char* find_str_last);

TMSU_DEF tm_bool tmsu_starts_with_v(tmsu_view_t str, tmsu_view_t find_str);
TMSU_DEF tm_bool tmsu_ends_with_v(tmsu_view_t str, tmsu_view_t find_str);

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi(const char* str, const char* find_str);
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi(const char* str, const char* find_str);

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                                     const char* b_last);
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                   const char* find_str_first, const char* find_str_last);

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str);
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str);

/* Some encoding/decoding functions. */

/*!
 * @brief Base64 encodes input bytes into an output octet stream. The octet stream is not nullterminated.
 *
 * No allocations take place. If the provided output size is not enough, the required size will be returned.
 * Note that base64 encoding is at most 4/3 of the original size, so an output buffer with
 * (input_size + (input_size / 3) + 4) should be enough.
 *
 * @param input_bytes[IN] The input bytes, that are to be base64 encoded.
 * @param input_size[IN] The size (in bytes) of the input.
 * @param out[OUT] The output octet stream.
 * @param out_size[INT] The size of the output (in bytes/octets).
 * @return The number of octets produced on success. If the returned number is greater than @param out_size, then the
 * output buffer was not big enough and it denotes a failure to encode. In this case the returned number is the required
 * output size.
 */
TMSU_DEF tm_size_t tmsu_base64_encode(const void* input_bytes, tm_size_t input_size, char* out, tm_size_t out_size);

/*!
 * @brief Base64 decodes a base64 encoded string to raw bytes.
 *
 * No allocations take place. If the provided output size is not enough, the required size will be returned.
 * Note that base64 encoding is at most 4/3 of the original size, so an output buffer with the same size as the input
 * should be enough.
 *
 * @param base64_encoded_string[IN] The base64 encoded string.
 * @param base64_input_size[IN]
 * @param out[OUT]
 * @param out_size[IN]
 * @return If the returned size is greater than @param out_size, then the output buffer was not big enough and the
 * returned size is the required output size.
 * If the returned size is 0, then @param base64_encoded_string is malformed or empty.
 * If the returned size is greater than 0 and less than or equal to @param out_size, then decoding succeeded.
 */
TMSU_DEF tm_size_t tmsu_base64_decode(const char* base64_encoded_string, tm_size_t base64_input_size, void* out,
                                      tm_size_t out_size);

/*!
 * @brief Base64url encodes input bytes into an output octet stream. The octet stream is not nullterminated.
 *
 * No allocations take place. If the provided output size is not enough, the required size will be returned.
 * Note that base64url encoding is at most 4/3 of the original size, so an output buffer with
 * (input_size + (input_size / 3) + 4) should be enough.
 *
 * @param input_bytes[IN] The input bytes, that are to be base64url encoded.
 * @param input_size[IN] The size (in bytes) of the input.
 * @param out[OUT] The output octet stream.
 * @param out_size[INT] The size of the output (in bytes or octets).
 * @return The number of octets produced on success. If the returned number is greater than @param out_size, then the
 * output buffer was not big enough and it denotes a failure to encode. In this case the returned number is the required
 * output size.
 */
TMSU_DEF tm_size_t tmsu_base64url_encode(const void* input_bytes, tm_size_t input_size, char* out, tm_size_t out_size);

/*!
 * @brief Base64url decodes a base64url encoded string to raw bytes.
 *
 * No allocations take place. If the provided output size is not enough, the required size will be returned.
 * Note that base64url encoding is at most 4/3 of the original size, so an output buffer with the same size as the input
 * should be enough.
 *
 * @param base64url_encoded_string[IN] The base64url encoded string.
 * @param base64url_input_size[IN] The size of the string in bytes or octets.
 * @param out[OUT] The output buffer.
 * @param out_size[IN] The size of the output in bytes.
 * @return If the returned size is greater than @param out_size, then the output buffer was not big enough and the
 * returned size is the required output size.
 * If the returned size is 0, then @param base64url_encoded_string is malformed or empty.
 * If the returned size is greater than 0 and less than or equal to @param out_size, then decoding succeeded.
 */
TMSU_DEF tm_size_t tmsu_base64url_decode(const char* base64url_encoded_string, tm_size_t base64url_input_size,
                                         void* out, tm_size_t out_size);

/*!
 * @brief Url encodes an input string. If the output buffer is not big enough, the required size is returned.
 * @param input[IN] The input string. Note that the input can be any raw binary data or string.
 * @param input_size[IN] The size of the input in bytes.
 * @param out[OUT] The output buffer. It will contain a valid ASCII string on success. The output is NOT nullterminated.
 * @param out_size[IN] The size of the output buffer in bytes or octets.
 * @return If the returned size is less than or equal to @param out_size, then encoding succeeded.
 * Otherwise the returned size denotes the required output buffer size and encoding failed.
 */
TMSU_DEF tm_size_t tmsu_url_encode(const void* input, tm_size_t input_size, char* out, tm_size_t out_size);

/*!
 * @brief Url decodes an input string. If the output buffer is not big enough, the required size is returned.
 * @param url_encoded_input[IN] The url encoded input string.
 * @param input_size[IN] The size of the input in bytes or octets.
 * @param out[OUT] The output buffer. Note that the output can be any raw binary data or string.
 * @param out_size[IN] The size of the output buffer in bytes. The output is NOT nullterminated.
 * @return If the returned size is less than or equal to @param out_size, then encoding succeeded.
 * If the returned size is 0, then @param url_encoded_input is malformed or empty.
 * Otherwise the returned size denotes the required output buffer size and encoding failed.
 */
TMSU_DEF tm_size_t tmsu_url_decode(const char* url_encoded_input, tm_size_t input_size, void* out, tm_size_t out_size);

/* Crt extensions, that are non standard and may not be provided. */

/* Only checks chars from '0' to '9'. */
TMSU_DEF tm_bool tmsu_isdigit(unsigned c);
TMSU_DEF tm_bool tmsu_isdigit_c(char c);

TMSU_DEF const char* tmsu_stristr(const char* str, const char* find_str);
TMSU_DEF int tmsu_stricmp(const char* a, const char* b);
TMSU_DEF int tmsu_strnicmp(const char* a, const char* b, size_t count);
TMSU_DEF char* tmsu_strrev(char* str);
TMSU_DEF char* tmsu_strnrev(char* str, size_t count);
TMSU_DEF const void* tmsu_memrchr(const void* ptr, int value, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* !defined(_TM_STRINGUTIL_INCLUDED_49458961_DD38_441D_B888_A589548CA6F5_) */

#ifdef TM_STRINGUTIL_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#include "../common/tm_assert_valid_size.inc"

#include "../common/tm_null.inc"

#include "implementation.cpp"

#include "implementation_view.cpp"

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_STRINGUTIL_IMPLEMENTATION) */

#include "../common/LICENSE.inc"
