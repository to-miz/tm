/*
tm_stringutil.h v0.9.3 - public domain - https://github.com/to-miz/tm
author: Tolga Mizrak 2020

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

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* Define to 1 if tm_size_t is signed. */
    #include <stddef.h> /* Include C version so identifiers are in global namespace. */
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

#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

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
        if (*str == c) break;
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
    if (str_first == str_last) return str_last;
    TM_ASSERT(str_first);
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
    --find_str_len;
    const char* search_last = str_last - find_str_len;
    const char* cur = str_first;
    char find_firs_char = *find_str_first;
    ++find_str_first;
    while ((cur = tmsu_find_char_n(cur, search_last, find_firs_char)) != search_last) {
        if (TM_MEMCMP(cur + 1, find_str_first, find_str_len) == 0) {
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
    --find_str_len;
    const char* search_last = str_last - find_str_len;
    const char* prev = search_last;
    const char* cur = search_last;
    char find_firs_char = *find_str_first;
    ++find_str_first;
    while ((cur = tmsu_find_last_char_n(str_first, prev, find_firs_char)) != prev) {
        if (TM_MEMCMP(cur + 1, find_str_first, find_str_len) == 0) {
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

    if (!*str) return str;
    char first_char = *find_str;
    if (!first_char) return str;
    ++find_str;
    size_t find_str_len = TM_STRLEN(find_str);
    do {
        str = tmsu_find_char(str, first_char);
        if (!*str) return str;
        if (TM_STRNCMP(str + 1, find_str, find_str_len) == 0) return str;
        ++str;
    } while (*str);
    return str;
}

TMSU_DEF const char* tmsu_find_last(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    return tmsu_find_last_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF const char* tmsu_find_first_not_of(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
#ifdef TM_STRSPN
    return str + TM_STRSPN(str, find_str);
#else
    while (*str && TM_STRCHR(find_str, TMSU_C2I(*str))) {
        ++str;
    }
    return str;
#endif
}

TMSU_DEF const char* tmsu_find_first_of(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
#ifdef TM_STRCSPN
    return str + TM_STRCSPN(str, find_str);
#else
    while (*str && TM_STRCHR(find_str, TMSU_C2I(*str)) == TM_NULL) {
        ++str;
    }
    return str;
#endif
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
    if (str_first == str_last) return not_found;
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_last != str_first && TM_MEMCHR(find_str_first, TMSU_C2I(*(str_last - 1)), find_str_len) == TM_NULL) {
        --str_last;
    }
    if (str_first == str_last) return not_found;
    return str_last - 1;
}

TMSU_DEF const char* tmsu_find_last_not_of_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                                const char* find_str_last, const char* not_found) {
    if (str_first == str_last) return not_found;
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_last != str_first && TM_MEMCHR(find_str_first, TMSU_C2I(*(str_last - 1)), find_str_len)) {
        --str_last;
    }
    if (str_first == str_last) return not_found;
    return str_last - 1;
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
    --find_str_len;
    const char* search_last = str_last - find_str_len;
    const char* cur = str_first;
    char find_firs_char = *find_str_first;
    ++find_str_first;
    while ((cur = tmsu_find_char_ignore_case_ansi_n(cur, search_last, find_firs_char)) != search_last) {
        if (tmsu_compare_ignore_case_ansi_n(cur + 1, cur + 1 + find_str_len, find_str_first, find_str_last) == 0) {
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

TMSU_DEF tmsu_tokenizer_t tmsu_tokenizer(const char* str) {
    TM_ASSERT(str);
    tmsu_tokenizer_t result;
    result.current = str;
    return result;
}

TMSU_DEF tm_bool tmsu_next_token(tmsu_tokenizer_t* tokenizer, const char* delimiters, tmsu_view_t* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->current);
    TM_ASSERT(delimiters);

    /* Skip everything until we find other delimiters or the end of the string. */
    const char* next = tmsu_find_first_of(tokenizer->current, delimiters);
    if (out) {
        out->first = tokenizer->current;
        out->last = next;
    }
    tm_bool result = (*next != 0);
    tokenizer->current = next + result;
    return result;
}

TMSU_DEF tm_bool tmsu_next_token_skip_empty(tmsu_tokenizer_t* tokenizer, const char* delimiters, tmsu_view_t* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->current);
    TM_ASSERT(delimiters);

    /* Skip delimiters at the beginning. */
    tokenizer->current = tmsu_find_first_not_of(tokenizer->current, delimiters);
    if (!*tokenizer->current) return TM_FALSE;
    /* Skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of(tokenizer->current, delimiters);
    if (out) {
        out->first = tokenizer->current;
        out->last = next;
    }
    tokenizer->current = next;
    return TM_TRUE;
}

TMSU_DEF tm_bool tmsu_next_token_n(tmsu_view_t* tokenizer, const char* delimiters_first,
                                   const char* delimiters_last, tmsu_view_t* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->first && tokenizer->first <= tokenizer->last);
    TM_ASSERT(delimiters_first && delimiters_first <= delimiters_last);

    /* Skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (out) *out = tmsu_view_n(tokenizer->first, next);
    tm_bool result = (next != tokenizer->last);
    tokenizer->first = next + result;
    return result;
}

TMSU_DEF tm_bool tmsu_next_token_skip_empty_n(tmsu_view_t* tokenizer, const char* delimiters_first,
                                              const char* delimiters_last, tmsu_view_t* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->first && tokenizer->first <= tokenizer->last);
    TM_ASSERT(delimiters_first && delimiters_first <= delimiters_last);

    /* Skip delimiters at the beginning. */
    tokenizer->first = tmsu_find_first_not_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (tokenizer->first == tokenizer->last) return TM_FALSE;
    /* Skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (out) *out = tmsu_view_n(tokenizer->first, next);
    tokenizer->first = next;
    return TM_TRUE;
}

/* Word tokenizing. */
static const char TMSU_WORD_SEPERATORS[] = " \t\n\v\f\r./\\()\"'-:,.;<>~!@#$%^&*|+=[]{}`~?";
TMSU_DEF const char* tmsu_find_word_end(const char* str) {
    TM_ASSERT(str);
    return tmsu_find_word_end_n_ex(str, str + TM_STRLEN(str), TMSU_WORD_SEPERATORS,
                                   TMSU_WORD_SEPERATORS + (sizeof(TMSU_WORD_SEPERATORS) - 1));
}
TMSU_DEF const char* tmsu_find_word_end_ex(const char* str, const char* word_seperators) {
    TM_ASSERT(str);
    TM_ASSERT(word_seperators);
    return tmsu_find_word_end_n_ex(str, str + TM_STRLEN(str), word_seperators,
                                   word_seperators + TM_STRLEN(word_seperators));
}
TMSU_DEF const char* tmsu_find_word_end_n(const char* first, const char* last) {
    return tmsu_find_word_end_n_ex(first, last, TMSU_WORD_SEPERATORS,
                                   TMSU_WORD_SEPERATORS + (sizeof(TMSU_WORD_SEPERATORS) - 1));
}
TMSU_DEF const char* tmsu_find_word_end_n_ex(const char* first, const char* last, const char* word_seperators_first,
                                             const char* word_seperators_last) {
    first = tmsu_trim_left_n(first, last);
    return tmsu_find_first_of_n(first, last, word_seperators_first, word_seperators_last);
}

TMSU_DEF const char* tmsu_find_word_start_n(const char* first, const char* last) {
    return tmsu_find_word_start_n_ex(first, last, TMSU_WORD_SEPERATORS,
                                     TMSU_WORD_SEPERATORS + (sizeof(TMSU_WORD_SEPERATORS) - 1));
}
TMSU_DEF const char* tmsu_find_word_start_n_ex(const char* first, const char* last, const char* word_seperators_first,
                                               const char* word_seperators_last) {
    last = tmsu_trim_right_n(first, last);
    const char* word_start = first;
    if (first != last) {
        word_start = tmsu_find_last_of_n_ex(first, last, word_seperators_first, word_seperators_last, TM_NULL);
        if (word_start) {
            // We found a word seperator, skip it, so that word_start actually points to the beginning of a word.
            ++word_start;
        } else {
            word_start = first;
        }
    }
    return word_start;
}

/* Whitespace trimming */

static const int TMSU_WHITESPACE_COUNT = 6;
static char const* const TMSU_WHITESPACE = " \t\n\v\f\r";

TMSU_DEF const char* tmsu_trim_left(const char* str) { return tmsu_find_first_not_of(str, TMSU_WHITESPACE); }

TMSU_DEF const char* tmsu_trim_left_n(const char* first, const char* last) {
    return tmsu_find_first_not_of_n(first, last, TMSU_WHITESPACE, TMSU_WHITESPACE + TMSU_WHITESPACE_COUNT);
}
TMSU_DEF const char* tmsu_trim_right_n(const char* first, const char* last) {
    const char* result = tmsu_find_last_not_of_n_ex(first, last, TMSU_WHITESPACE, TMSU_WHITESPACE + TMSU_WHITESPACE_COUNT, TM_NULL);
    if (result == TM_NULL) {
        /* No non whitespace found, point to first. */
        result = first;
    } else {
        /* Result points to non whitespace character, advance past it. */
        ++result;
    }
    return result;
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
TMSU_DEF tm_bool tmsu_isdigit_c(char c) { return c >= '0' && c <= '9'; }

TMSU_DEF const char* tmsu_stristr(const char* str, const char* find_str) {
    // Strstr returns NULL if find_str was not found, so we replicate this behavior here too.
    const char* result = tmsu_find_ignore_case_ansi(str, find_str);
    return (*result) ? result : TM_NULL;
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
    TM_ASSERT(str || count == 0);
    if (count == 0) return str;
    for (size_t i = 0, j = count - 1; i < j; ++i, --j) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
    return str;
}
TMSU_DEF const void* tmsu_memrchr(const void* ptr, int value, size_t len) {
    TM_ASSERT(ptr || len == 0);
    TM_ASSERT(value >= 0 && value <= 0xFF);
    if (!len) return TM_NULL;
    const char* p = (const char*)ptr + len;
    do {
        --len;
        --p;
        if ((unsigned char)*p == value) {
            return (const void*)p;
        }
    } while (len);
    return TM_NULL;
}

static char const* const tmsu_base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static char const* const tmsu_base64url_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789-_";

static tm_size_t tmsu_base64_encode_chars(char const* const chars, tm_bool pad, const void* input_bytes,
                                          tm_size_t input_size, char* out, tm_size_t out_size) {
    TM_ASSERT((chars == tmsu_base64_chars) || (chars == tmsu_base64url_chars));

    tm_size_t out_index = 0;
    const char* in = (const char*)input_bytes;
    unsigned int char_index[4];
    tm_size_t i = 0;
    for (; input_size >= 3; i += 3, input_size -= 3) {
        unsigned int a = (unsigned char)in[i];
        unsigned int b = (unsigned char)in[i + 1];
        unsigned int c = (unsigned char)in[i + 2];

        // Divide each three 8 bit blocks into four 6 bit blocks:
        // AAAAAA AABBBB BBBBCC CCCCCC
        char_index[0] = (a >> 2u);
        char_index[1] = ((a & 0x3u) << 4) | (b >> 4u);
        char_index[2] = ((b & 0xFu) << 2) | (c >> 6u);
        char_index[3] = (c & 0x3Fu);

        TM_ASSERT(char_index[0] < 64);
        TM_ASSERT(char_index[1] < 64);
        TM_ASSERT(char_index[2] < 64);
        TM_ASSERT(char_index[3] < 64);

        if (out_index + 4 < out_size) {
            out[out_index + 0] = chars[char_index[0]];
            out[out_index + 1] = chars[char_index[1]];
            out[out_index + 2] = chars[char_index[2]];
            out[out_index + 3] = chars[char_index[3]];
        }
        out_index += 4;
    }

    // The remaining size is either 1 or 2, it can't be 3, since it would have been handeled by the above loop.
    TM_ASSERT(input_size == 0 || input_size == 1 || input_size == 2);
    if (input_size > 0) {
        unsigned int a = (unsigned char)in[i];
        unsigned int b = (input_size > 1) ? (unsigned char)in[i + 1] : 0;

        // Divide each three 8 bit blocks into four 6 bit blocks:
        // AAAAAA AABBBB BBBBCC CCCCCC
        char_index[0] = (a >> 2u);
        char_index[1] = ((a & 0x3u) << 4) | (b >> 4u);
        char_index[2] = ((b & 0xFu) << 2);
        char_index[3] = 0;

        TM_ASSERT(char_index[0] < 64);
        TM_ASSERT(char_index[1] < 64);
        TM_ASSERT(char_index[2] < 64);
        TM_ASSERT(char_index[3] < 64);

        if (out_index + 2 < out_size) {
            out[out_index + 0] = chars[char_index[0]];
            out[out_index + 1] = chars[char_index[1]];
            out_index += 2;
        }
        if (input_size > 1) {
            if (out_index + 1 < out_size) out[out_index] = chars[char_index[2]];
            ++out_index;
        } else if (pad) {
            if (out_index + 1 < out_size) out[out_index] = '=';
            ++out_index;
        }

        if (pad) {
            if (out_index + 1 < out_size) out[out_index] = '=';
            ++out_index;
        }
    }
    return out_index;
}

static tm_size_t tmsu_base64_decode_chars(char const* const chars, tm_bool expect_padding,
                                          const char* base64_in, tm_size_t base64_in_size, void* out,
                                          tm_size_t out_size) {
    TM_ASSERT((chars == tmsu_base64_chars) || (chars == tmsu_base64url_chars));
    TM_ASSERT((base64_in_size % 4 == 0) || !expect_padding);

    tm_size_t out_index = 0;
    char* p = (char*)out;
    unsigned int char_value[4];
    tm_size_t i = 0;
    for (; base64_in_size > 4; i += 4, base64_in_size -= 4) {
        char_value[0] = (unsigned int)(tmsu_find_char(chars, base64_in[i + 0]) - chars);
        char_value[1] = (unsigned int)(tmsu_find_char(chars, base64_in[i + 1]) - chars);
        char_value[2] = (unsigned int)(tmsu_find_char(chars, base64_in[i + 2]) - chars);
        char_value[3] = (unsigned int)(tmsu_find_char(chars, base64_in[i + 3]) - chars);

        if (char_value[0] >= 64) return 0;
        if (char_value[1] >= 64) return 0;
        if (char_value[2] >= 64) return 0;
        if (char_value[3] >= 64) return 0;

        // Decompose each four 6 bit blocks into three 8 bit blocks:
        // 00000011 11112222 22333333
        if (out_index + 3 < out_size) {
            p[out_index + 0] = (char)((char_value[0] << 2) | (char_value[1] >> 4));
            p[out_index + 1] = (char)(((char_value[1] << 4) & 0xF0u) | (char_value[2] >> 2));
            p[out_index + 2] = (char)(((char_value[2] << 6) & 0xC0u) | (char_value[3]));
        }
        out_index += 3;
    }

    // Handle last 4 bytes seperately, since they might have padding '='.
    if (base64_in_size > 0) {
        int padding_count = 0;
        if (expect_padding) {
            char_value[0] = (unsigned int)(tmsu_find_char(chars, base64_in[i + 0]) - chars);
            char_value[1] = (unsigned int)(tmsu_find_char(chars, base64_in[i + 1]) - chars);
            char_value[2] = (unsigned int)(tmsu_find_char(chars, base64_in[i + 2]) - chars);
            char_value[3] = (unsigned int)(tmsu_find_char(chars, base64_in[i + 3]) - chars);

            if (char_value[0] >= 64) return 0;
            if (char_value[1] >= 64) return 0;
            if (char_value[3] >= 64) {
                if (base64_in[i + 3] != '=') return 0;
                char_value[3] = 0;
                padding_count = 1;
            }
            if (char_value[2] >= 64) {
                if (base64_in[i + 2] != '=') return 0;
                char_value[2] = 0;
                padding_count = 2;
            }
        } else {
            char_value[0] = (base64_in_size > 0) ? (unsigned int)(tmsu_find_char(chars, base64_in[i + 0]) - chars) : 0;
            char_value[1] = (base64_in_size > 1) ? (unsigned int)(tmsu_find_char(chars, base64_in[i + 1]) - chars) : 0;
            char_value[2] = (base64_in_size > 2) ? (unsigned int)(tmsu_find_char(chars, base64_in[i + 2]) - chars) : 0;
            char_value[3] = (base64_in_size > 3) ? (unsigned int)(tmsu_find_char(chars, base64_in[i + 3]) - chars) : 0;

            if (char_value[0] >= 64) return 0;
            if (char_value[1] >= 64) return 0;
            if (char_value[2] >= 64) return 0;
            if (char_value[3] >= 64) return 0;

            padding_count = 2 - (base64_in_size > 2) - (base64_in_size > 3);
        }

        // Decompose each four 6 bit blocks into three 8 bit blocks:
        // 00000011 11112222 22333333
        if (out_index + 1 < out_size) p[out_index] = (char)((char_value[0] << 2) | (char_value[1] >> 4));
        ++out_index;
        if (padding_count < 2) {
            if (out_index + 1 < out_size) p[out_index] = (char)(((char_value[1] << 4) & 0xF0u) | (char_value[2] >> 2));
            ++out_index;
        }
        if (padding_count < 1) {
            if (out_index + 1 < out_size) p[out_index] = (char)(((char_value[2] << 6) & 0xC0u) | (char_value[3]));
            ++out_index;
        }
    }
    return out_index;
}

TMSU_DEF tm_size_t tmsu_base64_encode(const void* input_bytes, tm_size_t input_size, char* out, tm_size_t out_size) {
    TM_ASSERT(input_bytes || input_size == 0);
    TM_ASSERT(out || out_size == 0);
    return tmsu_base64_encode_chars(tmsu_base64_chars, /*pad=*/TM_TRUE, input_bytes, input_size, out, out_size);
}
TMSU_DEF tm_size_t tmsu_base64url_encode(const void* input_bytes, tm_size_t input_size, char* out, tm_size_t out_size) {
    TM_ASSERT(input_bytes || input_size == 0);
    TM_ASSERT(out || out_size == 0);
    return tmsu_base64_encode_chars(tmsu_base64url_chars, /*pad=*/TM_FALSE, input_bytes, input_size, out, out_size);
}
TMSU_DEF tm_size_t tmsu_base64_decode(const char* base64_encoded_string, tm_size_t base64_input_size, void* out,
                                      tm_size_t out_size) {
    TM_ASSERT(base64_encoded_string || base64_input_size == 0);
    TM_ASSERT(out || out_size == 0);
    if (base64_input_size == 0) return 0;
    if (base64_input_size % 4 != 0) return 0;
    return tmsu_base64_decode_chars(tmsu_base64_chars, /*expect_padding=*/TM_TRUE, base64_encoded_string,
                                    base64_input_size, out, out_size);
}
TMSU_DEF tm_size_t tmsu_base64url_decode(const char* base64url_encoded_string, tm_size_t base64url_input_size,
                                         void* out, tm_size_t out_size) {
    TM_ASSERT(base64url_encoded_string || base64url_input_size == 0);
    TM_ASSERT(out || out_size == 0);
    if (base64url_input_size < 2) return 0;
    return tmsu_base64_decode_chars(tmsu_base64url_chars, /*expect_padding=*/TM_FALSE, base64url_encoded_string,
                                    base64url_input_size, out, out_size);
}

TMSU_DEF tm_size_t tmsu_url_encode(const void* input, tm_size_t input_size, char* out, tm_size_t out_size) {
    TM_ASSERT(input || input_size == 0);
    TM_ASSERT(out || out_size == 0);

    const char* in = (const char*)input;
    tm_size_t out_index = 0;
    for (tm_size_t i = 0; i < input_size; ++i) {
        unsigned int c = (unsigned char)in[i];
        // Special case characters, that are allowed in uri strings.
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')
            || (c == '-') || (c == '_') || (c == '.')) {
            if (out_index < out_size) out[out_index] = (char)c;
            ++out_index;
            continue;
        }

        // Percent encode any other byte.
        // Byte to hex inplace.
        unsigned int l = (c >> 4u) & 0x0Fu;
        l += (l <= 9) ? '0' : ('A' - 10);
        unsigned int r = c & 0x0Fu;
        r += (r <= 9) ? '0' : ('A' - 10);

        if (out_index < out_size) out[out_index] = '%';
        ++out_index;
        if (out_index < out_size) out[out_index] = (char)l;
        ++out_index;
        if (out_index < out_size) out[out_index] = (char)r;
        ++out_index;
    }
    return out_index;
}

TMSU_DEF tm_size_t tmsu_url_decode(const char* url_encoded_input, tm_size_t input_size, void* out, tm_size_t out_size) {
    TM_ASSERT(url_encoded_input || input_size == 0);
    TM_ASSERT(out || out_size == 0);

    char* p = (char*)out;
    tm_size_t out_index = 0;
    for (tm_size_t i = 0; i < input_size; ++i) {
        unsigned int c = (unsigned char)url_encoded_input[i];
        // Special case characters, that are allowed in uri strings.
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '-') || (c == '_') || (c == '.')) {
            if (out_index < out_size) p[out_index] = (char)c;
            ++out_index;
            continue;
        }

        if (c == '%') {
            // Check for malformed input.
            if (input_size - i < 3) return 0;

            // Percent decode.
            unsigned int l = (unsigned char)url_encoded_input[i + 1];
            unsigned int r = (unsigned char)url_encoded_input[i + 2];

            // Hex to byte inplace.
            if (l >= '0' && l <= '9')
                l -= '0';
            else if (l >= 'A' && l <= 'F')
                l = (l - 'A') + 10;
            else if (l >= 'a' && l <= 'f')
                l = (l - 'a') + 10;
            else
                return 0;

            if (r >= '0' && r <= '9')
                r -= '0';
            else if (r >= 'A' && r <= 'F')
                r = (r - 'A') + 10;
            else if (r >= 'a' && r <= 'f')
                r = (r - 'a') + 10;
            else
                return 0;

            if (out_index < out_size) p[out_index] = (char)((unsigned char)((l << 4u) | r));
            ++out_index;
            i += 2;
            continue;
        }

        // Invalid character encountered.
        return 0;
    }
    return out_index;
}

TMSU_DEF tmsu_view_t tmsu_view(const char* str) {
    tmsu_view_t result;
    result.first = str;
    result.last = (str) ? (str + TM_STRLEN(str)) : str;
    return result;
}

TMSU_DEF tmsu_view_t tmsu_view_n(const char* first, const char* last) {
    TM_ASSERT(first <= last);
    tmsu_view_t result;
    result.first = first;
    result.last = last;
    return result;
}

TMSU_DEF tmsu_view_t tmsu_view_l(const char* str, tm_size_t len) {
    TM_ASSERT(str || len == 0);
    TM_ASSERT_VALID_SIZE(len);
    tmsu_view_t result;
    result.first = str;
    result.last = (str) ? (str + len) : str;
    return result;
}

TMSU_DEF tm_bool tmsu_view_empty(tmsu_view_t str) {
    return str.first == str.last;
}

TMSU_DEF const char* tmsu_view_data(tmsu_view_t str) {
    return str.first;
}

TMSU_DEF tm_size_t tmsu_view_size(tmsu_view_t str) {
    TM_ASSERT(str.first <= str.last);
    return (tm_size_t)(str.last - str.first);
}

TMSU_DEF const char* tmsu_find_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_last_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found) {
    return tmsu_find_last_n_ex(str.first, str.last, find_str.first, find_str.last, not_found);
}

TMSU_DEF const char* tmsu_find_first_of_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_first_of_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_first_not_of_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_first_not_of_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_of_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_last_of_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_of_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found) {
    return tmsu_find_last_of_n_ex(str.first, str.last, find_str.first, find_str.last, not_found);
}

TMSU_DEF const char* tmsu_find_last_not_of_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_last_not_of_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_not_of_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found) {
    return tmsu_find_last_not_of_n_ex(str.first, str.last, find_str.first, find_str.last, not_found);
}

TMSU_DEF const char* tmsu_find_char_ignore_case_ansi_v(tmsu_view_t str, char c) {
    return tmsu_find_char_ignore_case_ansi_n(str.first, str.last, c);
}

TMSU_DEF const char* tmsu_find_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_ignore_case_ansi_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_char_unescaped_v(tmsu_view_t str, char c, char escape_char) {
    return tmsu_find_char_unescaped_n(str.first, str.last, c, escape_char);
}

TMSU_DEF const char* tmsu_find_first_of_unescaped_v(tmsu_view_t str, tmsu_view_t find_str, char escape_char) {
    return tmsu_find_first_of_unescaped_n(str.first, str.last, find_str.first, find_str.last, escape_char);
}

TMSU_DEF const char* tmsu_find_word_end_v(tmsu_view_t str) {
    return tmsu_find_word_end_n(str.first, str.last);
}

TMSU_DEF const char* tmsu_find_word_end_v_ex(tmsu_view_t str, tmsu_view_t word_separators) {
    return tmsu_find_word_end_n_ex(str.first, str.last, word_separators.first, word_separators.last);
}

TMSU_DEF const char* tmsu_find_word_start_v(tmsu_view_t str) {
    return tmsu_find_word_start_n(str.first, str.last);
}

TMSU_DEF const char* tmsu_find_word_start_v_ex(tmsu_view_t str, tmsu_view_t word_separators) {
    return tmsu_find_word_start_n_ex(str.first, str.last, word_separators.first, word_separators.last);
}

TMSU_DEF const char* tmsu_trim_left_v(tmsu_view_t str) {
    return tmsu_trim_left_n(str.first, str.last);
}

TMSU_DEF const char* tmsu_trim_right_v(tmsu_view_t str) {
    return tmsu_trim_right_n(str.first, str.last);
}

TMSU_DEF tmsu_view_t tmsu_trim_n(const char* first, const char* last) {
    return tmsu_view_n(tmsu_trim_left_n(first, last), tmsu_trim_right_n(first, last));
}

TMSU_DEF tmsu_view_t tmsu_trim_v(tmsu_view_t str) {
    return tmsu_view_n(tmsu_trim_left_n(str.first, str.last), tmsu_trim_right_n(str.first, str.last));
}

TMSU_DEF int tmsu_compare_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_compare_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF int tmsu_compare_ignore_case_ansi_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_compare_ignore_case_ansi_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF int tmsu_human_compare_ansi_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_human_compare_ansi_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF tm_bool tmsu_equals_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_equals_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF tm_bool tmsu_equals_ignore_case_ansi_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_equals_ignore_case_ansi_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF tm_bool tmsu_starts_with_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_starts_with_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF tm_bool tmsu_ends_with_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_ends_with_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_starts_with_ignore_case_ansi_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_ends_with_ignore_case_ansi_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF tm_bool tmsu_next_token_v(tmsu_view_t* tokenizer, tmsu_view_t delimiters, tmsu_view_t* out) {
    return tmsu_next_token_n(tokenizer, delimiters.first, delimiters.last, out);
}

TMSU_DEF tm_bool tmsu_next_token_skip_empty_v(tmsu_view_t* tokenizer, tmsu_view_t delimiters, tmsu_view_t* out) {
    return tmsu_next_token_skip_empty_n(tokenizer, delimiters.first, delimiters.last, out);
}

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_STRINGUTIL_IMPLEMENTATION) */

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2020 Tolga Mizrak

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
