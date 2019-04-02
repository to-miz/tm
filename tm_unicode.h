/*
tm_unicode.h v0.1.4 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2019

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_UNICODE_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

PURPOSE
    A full Utf-8 support library for C/C++. It is designed to allow using Utf-8 everywhere.
    There are also optional system call wrappers for windows/linux, so that those can be used
    with Utf-8 strings in a platform independent manner. The wrappers include file IO and
    converting the command line.

SWITCHES
    TMU_NO_UCD:
        If TMU_NO_UCD is defined, no Unicode data tables will be compiled into the binary.
        Unicode data tables consume a lot of memory (29937 bytes or 29.24 kilobytes currently).
        The tables are needed for things like case folding and case insensitive comparisons.
        Not including them will make for a smaller binary size.

        The secondary use of this switch is to enable more/less functionality than is provided
        by default. This requires new tables to be generated.
        For Unicode data table generation see https://github.com/to-miz/tm/tools/unicode_gen.

        Then include the generated header before including this file like this:
            #include <generated_ucd.h>
            #define TMU_NO_UCD
            #include <tm_unicode.h>

        And in a single translation unit:
            #include <generated_ucd.h>
            #include <generated_ucd.c>
            #define TMU_NO_UCD
            #define TM_UNICODE_IMPLEMENTATION
            #include <tm_unicode.h>

    TMU_USE_CRT:
        Allows the implementation to use CRT functions.

    TMU_USE_WINDOWS_H:
        Allows the implementation to use Winapi functions (those defined in windows.h).
        The header file <windows.h> still needs to be included manually like this before
        including this file:
            #include <windows.h>
            #define TMU_USE_WINDOWS_H
            #include <tm_unicode.h>

        Both TMU_USE_CRT and TMU_USE_WINDOWS_H can be defined at the same time.
        This allows CRT file IO to be accessible even with the Winapi backend.

    TMU_NO_FILE_IO
        As the name suggests, if this is defined, no file IO functions are supplied.

ISSUES
    - No locale support for case folding (some locales case fold differently,
      like turkic languages with dotted uppercase I).
    - No conditional special casing support, like for instance FINAL SIGMA
      (sigma character at the end of a word has a different lowercase variant).
    - tmu_atomic_write not implemented yet for CRT backend.

HISTORY
    v0.1.4  02.04.19 Fixed gcc/clang compilation errors.
                     Implemented full case toggling.
    v0.1.3  21.03.19 Fixed tmu_get_ucd_width being used instead of tmu_ucd_get_width.
                     Changed generated unicode data to handle invalid codepoints.
    v0.1.2  10.03.19 Fixed unused function warning when compiling with TMU_NO_UCD.
    v0.1.1  25.02.19 Fixed MSVC compilation errors.
    v0.1.0  24.02.19 Initial commit of the complete rewrite.
*/

/* This is a generated file, do not modify directly. You can find the generator files in the src directory. */

#ifdef TM_UNICODE_IMPLEMENTATION
    /* assert */
    #ifndef TM_ASSERT
        #include <assert.h>
        #define TM_ASSERT assert
    #endif /* !defined(TM_ASSERT) */
#endif /* defined(TM_UNICODE_IMPLEMENTATION) */

#ifndef _TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765
#define _TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

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

/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
    #define TM_ERRC_DEFINED
    enum TM_ERRC_CODES {
        TM_OK           = 0,   /* Same as std::errc() */
        TM_EPERM        = 1,   /* Same as std::errc::operation_not_permitted */
        TM_ENOENT       = 2,   /* Same as std::errc::no_such_file_or_directory */
        TM_EIO          = 5,   /* Same as std::errc::io_error */
        TM_ENOMEM       = 12,  /* Same as std::errc::not_enough_memory */
        TM_EACCES       = 13,  /* Same as std::errc::permission_denied */
        TM_EBUSY        = 16,  /* Same as std::errc::device_or_resource_busy */
        TM_EEXIST       = 17,  /* Same as std::errc::file_exists */
        TM_EEXDEV       = 18,  /* Same as std::errc::cross_device_link */
        TM_ENODEV       = 19,  /* Same as std::errc::no_such_device */
        TM_EINVAL       = 22,  /* Same as std::errc::invalid_argument */
        TM_EMFILE       = 24,  /* Same as std::errc::too_many_files_open */
        TM_EFBIG        = 27,  /* Same as std::errc::file_too_large */
        TM_ENOSPC       = 28,  /* Same as std::errc::no_space_on_device */
        TM_ERANGE       = 34,  /* Same as std::errc::result_out_of_range */
        TM_ENAMETOOLONG = 36,  /* Same as std::errc::filename_too_long */
        TM_ENOTEMPTY    = 39,  /* Same as std::errc::directory_not_empty */
        TM_EOVERFLOW    = 75,  /* Same as std::errc::value_too_large */
    };
    typedef int tm_errc;
#endif
#define TMU_NO_SUCH_FILE_OR_DIRECTORY TM_ENOENT

/* Static assert macro for C11/C++. Second parameter must be an identifier, not a string literal. */
#ifndef TM_STATIC_ASSERT
    #if defined(__cplusplus)
        #ifdef __cpp_static_assert
            #define TM_STATIC_ASSERT(cond, msg) static_assert(cond, #msg)
        #endif
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #define TM_STATIC_ASSERT(cond, msg) _Static_assert(cond, #msg)
    #else
        #define TM_STATIC_ASSERT(cond, msg) typedef char static_assertion_##msg[(cond) ? 1 : -1]
    #endif
#endif /* !defined(TM_STATIC_ASSERT) */

#ifndef TMU_DEF
    #define TMU_DEF extern
#endif

#if defined(TMU_USE_CRT) && !defined(TMU_TESTING)
    #include <stdio.h>
#endif

/* Unicode handling. */
#if !defined(TMU_NO_UCD)
#define TMU_UCD_DEF TMU_DEF
/* This file was generated using tools/unicode_gen from
   https://github.com/to-miz/tm. Do not modify by hand. */
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    tmu_grapheme_break_other,
    tmu_grapheme_break_cr,
    tmu_grapheme_break_lf,
    tmu_grapheme_break_control,
    tmu_grapheme_break_prepend,
    tmu_grapheme_break_extend,
    tmu_grapheme_break_regional_indicator,
    tmu_grapheme_break_spacing_mark,
    tmu_grapheme_break_l,
    tmu_grapheme_break_v,
    tmu_grapheme_break_t,
    tmu_grapheme_break_lv,
    tmu_grapheme_break_lvt,
    tmu_grapheme_break_zwj,
    tmu_grapheme_break_extended_pictographic,

    tmu_grapheme_break_count
} tmu_ucd_grapheme_break_enum;

typedef enum {
    tmu_ucd_case_caseless,
    tmu_ucd_case_upper,
    tmu_ucd_case_lower,
    tmu_ucd_case_title
} tmu_ucd_case_info_enum; 

typedef enum {
    tmu_ucd_category_control,
    tmu_ucd_category_letter,
    tmu_ucd_category_mark,
    tmu_ucd_category_number,
    tmu_ucd_category_punctuation,
    tmu_ucd_category_symbol,
    tmu_ucd_category_separator
} tmu_ucd_category_enum;

typedef struct {
    tmu_ucd_category_enum category;
    tmu_ucd_case_info_enum case_info;
    tmu_ucd_grapheme_break_enum grapheme_break;
    uint32_t simple_case_fold;
    const uint16_t* full_case_fold;
} tmu_ucd_entry;

TMU_UCD_DEF tmu_ucd_entry tmu_ucd_get_entry(uint32_t codepoint);
TMU_UCD_DEF tmu_ucd_category_enum tmu_ucd_get_category(uint32_t codepoint);
TMU_UCD_DEF int tmu_ucd_is_whitespace(uint32_t codepoint);
TMU_UCD_DEF tmu_ucd_case_info_enum tmu_ucd_get_case_info(uint32_t codepoint);

#define TMU_UCD_HAS_CASE_INFO 1
#define TMU_UCD_HAS_CATEGORY 1
#define TMU_UCD_HAS_GRAPHEME_BREAK 1
#define TMU_UCD_HAS_WIDTH 0
#define TMU_UCD_HAS_CANONICAL 0
#define TMU_UCD_HAS_COMPATIBILITY 0
#define TMU_UCD_HAS_FULL_CASE 0
#define TMU_UCD_HAS_FULL_CASE_FOLD 1
#define TMU_UCD_HAS_FULL_CASE_TOGGLE 0
#define TMU_UCD_HAS_SIMPLE_CASE 0
#define TMU_UCD_HAS_SIMPLE_CASE_FOLD 1
#define TMU_UCD_HAS_SIMPLE_CASE_TOGGLE 0

#ifdef __cplusplus
}
#endif

#undef TMU_UCD_DEF
#endif

#if defined(TMU_USE_STL) && defined(__cplusplus)
    #include <vector>
#endif /* defined(TMU_USE_STL) && defined(__cplusplus) */

#if !defined(TMU_TESTING_CHAR16_DEFINED)
	#if defined(TMU_USE_WINDOWS_H)
		typedef WCHAR tmu_char16;
	#else
		#include <wchar.h>

		#if !defined(__linux__) && (defined(_WIN32) || WCHAR_MAX == 0xFFFFu || WCHAR_MAX == 0x7FFF)
			typedef wchar_t tmu_char16;
		#else
			typedef uint16_t tmu_char16;
		#endif
	#endif
#endif

TM_STATIC_ASSERT(sizeof(tmu_char16) == 2, tmu_char16_must_be_2_bytes);

typedef struct tmu_contents_struct {
    char* data;
    tm_size_t size;
    tm_size_t capacity;

#if defined(__cplusplus) && defined(TM_STRING_VIEW)
    operator TM_STRING_VIEW() const;
#endif
} tmu_contents;

/* Encoding enums.
   Variants without bom suffix may have byte order mark.
   Variants with bom suffix must have byte order mark. */
typedef enum {
    tmu_encoding_unknown,
    tmu_encoding_utf8,
    tmu_encoding_utf8_bom,
    tmu_encoding_utf16be,
    tmu_encoding_utf16be_bom,
    tmu_encoding_utf16le,
    tmu_encoding_utf16le_bom,
    tmu_encoding_utf32be,
    tmu_encoding_utf32be_bom,
    tmu_encoding_utf32le,
    tmu_encoding_utf32le_bom,
} tmu_encoding;

typedef struct {
    tmu_contents contents;
    tm_errc ec;
} tmu_contents_result;

/* The resulting contents after conversion never have byte order mark (bom). */
typedef struct {
    tmu_contents contents;
    tm_errc ec;
    tmu_encoding original_encoding;
    tm_bool invalid_codepoints_encountered;
} tmu_utf8_conversion_result;

typedef struct {
    const char* cur;
    const char* end;
} tmu_utf8_stream;

typedef struct {
    char* data;
    tm_size_t size;
    tm_size_t capacity;
    tm_size_t necessary;
    tm_errc ec;
} tmu_utf8_output_stream;

typedef struct {
    const tmu_char16* cur;
    const tmu_char16* end;
} tmu_utf16_stream;

typedef struct {
    tmu_char16* data;
    tm_size_t size;
    tm_size_t capacity;
    tm_size_t necessary;
    tm_errc ec;
} tmu_utf16_output_stream;

typedef struct {
    tmu_char16* data;
    tm_size_t size;
    tm_size_t capacity;
} tmu_utf16_contents;

typedef struct {
    tmu_utf16_contents contents;
    tm_errc ec;
} tmu_utf16_contents_result;

typedef struct {
    tm_size_t size;
    tm_errc ec;
    tmu_encoding original_encoding;
    tm_bool invalid_codepoints_encountered;
} tmu_conversion_result;

typedef struct {
    tm_size_t size;
    tm_errc ec;
} tmu_transform_result;

typedef enum { tmu_validate_skip, tmu_validate_error, tmu_validate_replace } tmu_validate;

TMU_DEF tmu_utf8_stream tmu_utf8_make_stream(const char* str);
TMU_DEF tmu_utf8_stream tmu_utf8_make_stream_n(const char* str, tm_size_t len);

TMU_DEF tmu_utf8_output_stream tmu_utf8_make_output_stream(char* data, tm_size_t capacity);
TMU_DEF tmu_utf8_output_stream tmu_utf8_make_output_stream_n(char* data, tm_size_t capacity, tm_size_t size);

TMU_DEF tmu_utf16_stream tmu_utf16_make_stream(const tmu_char16* str);
TMU_DEF tmu_utf16_stream tmu_utf16_make_stream_n(const tmu_char16* str, tm_size_t len);

TMU_DEF tm_bool tmu_is_valid_codepoint(uint32_t codepoint);

/*
Extract codepoint from encoded stream. Stream will shrink as codepoints are extracted.
Params:
    stream:        Input stream to extract codepoints from.
    codepoint_out: Output parameter that receives the extracted codepoint. Must not be NULL.
Returns:
    Returns true on success.
    Returns false on failure and stream->cur points to invalid position.
*/
TMU_DEF tm_bool tmu_utf8_extract(tmu_utf8_stream* stream, uint32_t* codepoint_out);
TMU_DEF tm_bool tmu_utf16_extract(tmu_utf16_stream* stream, uint32_t* codepoint_out);

/*
Encode codepoint to encoded string.
Params:
    codepoint: Must be a valid codepoint (tmu_is_valid_codepoint returns true).
    out:       Output buffer. Can be NULL iff out_len == 0.
    out_len:   Size of the output buffer.
Returns:
    Returns value <= out_len on success.
    Returns value > out_len if output buffer isn't sufficiently large. In that case return value is the required
size.
*/
TMU_DEF tm_size_t tmu_utf8_encode(uint32_t codepoint, char* out, tm_size_t out_len);
TMU_DEF tm_size_t tmu_utf16_encode(uint32_t codepoint, tmu_char16* out, tm_size_t out_len);

/*
Append codepoint to output stream.
Calling append on a stream without capacity will increase the streams 'necessary' counter while setting
ec to EOVERFLOW. The 'necessary' field can then be used to allocate a sufficiently large buffer.
Params:
    codepoint: Must be a valid codepoint (tmu_is_valid_codepoint returns true).
    stream:    Output stream.
Returns:
    Returns true if stream had enough capacity, false otherwise.
size.
*/
TMU_DEF tm_bool tmu_utf8_append(uint32_t codepoint, tmu_utf8_output_stream* stream);
TMU_DEF tm_bool tmu_utf16_append(uint32_t codepoint, tmu_utf16_output_stream* stream);

/*
Convert raw bytes from an untrusted origin to utf8. This function may take ownership of the buffer pointed to
in the input argument if it already is in utf8 encoding. In that case input will be zeroed out to denote a move
in ownership.
Params:
    input:           Raw input bytes. Contents will be modified on success or failure.
    encoding:        The encoding of the input. If the encoding is unknown and should be detected,
                     pass tmu_encoding_unknown.
    validate:        How to validate the resulting utf8 output.
    replace_str:     String to replace invalid codepoints with. Only used if validate == tmu_validate_replace.
    replace_str_len: Length of replace_str.
    nullterminate:   Whether to nullterminate out.
    out:             Output buffer. Can be NULL iff out_len == 0.
                     If NULL, returned size will denote the required size of the output buffer.
    out_len:         Length of the buffer specified by out parameter.
Return:
    Returns the converted contents, size, error code, original encoding and whether invalid codepoints were encoutered.
    On success size will denote how much of out was consumed (not counting null-terminator).
    If out wasn't big enough, the error code will be TM_ERANGE and size will denote the required size.
*/
TMU_DEF tmu_utf8_conversion_result tmu_utf8_convert_from_bytes_dynamic(tmu_contents* input, tmu_encoding encoding,
                                                                       tmu_validate validate, const char* replace_str,
                                                                       tm_size_t replace_str_len,
                                                                       tm_bool nullterminate);

/*
Convert raw bytes from an untrusted origin to utf8.
Params:
    input:           Raw input bytes.
    input_len:       Length of the input in bytes.
    encoding:        The encoding of the input. If the encoding is unknown and should be detected,
                     pass tmu_encoding_unknown.
    validate:        How to validate the resulting utf8 output.
    replace_str:     String to replace invalid codepoints with. Only used if validate == tmu_validate_replace.
    replace_str_len: Length of replace_str.
    nullterminate:   Whether to nullterminate out.
    out:             Output buffer. Can be NULL iff out_len == 0.
                     If NULL, returned size will denote the required size of the output buffer.
    out_len:         Length of the buffer specified by out parameter.
Return:
    Returns the size, error code, original encoding and whether invalid codepoints were encoutered.
    On success size will denote how much of out was consumed (not counting null-terminator).
    If out wasn't big enough, the error code will be TM_ERANGE and size will denote the required size.
*/
TMU_DEF tmu_conversion_result tmu_utf8_convert_from_bytes(const void* input, tm_size_t input_len, tmu_encoding encoding,
                                                          tmu_validate validate, const char* replace_str,
                                                          tm_size_t replace_str_len, tm_bool nullterminate, char* out,
                                                          tm_size_t out_len);

TMU_DEF tmu_conversion_result tmu_utf8_from_utf16(tmu_utf16_stream stream, char* out, tm_size_t out_len);
TMU_DEF tmu_conversion_result tmu_utf16_from_utf8(tmu_utf8_stream stream, tmu_char16* out, tm_size_t out_len);

TMU_DEF tmu_conversion_result tmu_utf8_from_utf16_ex(tmu_utf16_stream stream, tmu_validate validate,
                                                     const char* replace_str, tm_size_t replace_str_len,
                                                     tm_bool nullterminate, char* out, tm_size_t out_len);
TMU_DEF tmu_conversion_result tmu_utf16_from_utf8_ex(tmu_utf8_stream stream, tmu_validate validate,
                                                     const tmu_char16* replace_str, tm_size_t replace_str_len,
                                                     tm_bool nullterminate, tmu_char16* out, tm_size_t out_len);

TMU_DEF tm_size_t tmu_utf8_valid_range(const char* str, tm_size_t len);
TMU_DEF tm_size_t tmu_utf8_skip_invalid(char* str, tm_size_t len);
/*TMU_DEF void tmu_utf8_replace_invalid(tmu_contents* r, const char* replace_str, tm_size_t replace_str_len);*/

TMU_DEF tm_size_t tmu_utf16_valid_range(const tmu_char16* str, tm_size_t len);
TMU_DEF tm_size_t tmu_utf16_skip_invalid(tmu_char16* str, tm_size_t len);

/*
Copies a stream into out, making sure that out has still valid encoding after truncation.
Returns:
        Returns the amount copied to out.
        If returned value is equal to the size of the stream, the whole stream was copied.
*/
TMU_DEF tm_size_t tmu_utf8_copy_truncated(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
TMU_DEF tm_size_t tmu_utf8_copy_truncated_stream(tmu_utf8_stream stream, char* out, tm_size_t out_len);

TMU_DEF tm_bool tmu_utf8_equals(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len);
TMU_DEF int tmu_utf8_compare(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len);

TMU_DEF tm_size_t tmu_utf8_count_codepoints(const char* str);
TMU_DEF tm_size_t tmu_utf8_count_codepoints_n(const char* str, tm_size_t str_len);
TMU_DEF tm_size_t tmu_utf8_count_codepoints_stream(tmu_utf8_stream stream);

/* The following functions depend on Unicode data being present.
   The default unicode data supplied with this library doesn't supply all of the data supported.
   If more functionality is needed (like transforming strings to uppercase/lowercase etc.), then a different set of
   Unicode data needs to be generated. The Unicode data generator is in tools/unicode_gen.
   To then use the newly generated Unicode data with this library, the following is necessary:
   Define TMU_NO_UCD and include the generated header before including this header and include the generated
   C source file before defining TM_UNICODE_IMPLEMENTATION and including this header in a translation unit. */
#if defined(TMU_UCD_HAS_CASE_INFO)

#if TMU_UCD_HAS_CATEGORY
TMU_DEF tm_bool tmu_is_control(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_letter(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_mark(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_number(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_punctuation(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_symbol(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_separator(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_whitespace(uint32_t codepoint);
#endif /* TMU_UCD_HAS_CATEGORY */

#if TMU_UCD_HAS_CASE_INFO
TMU_DEF tm_bool tmu_is_upper(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_lower(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_title(uint32_t codepoint);
TMU_DEF tm_bool tmu_is_caseless(uint32_t codepoint);
#endif /* TMU_UCD_HAS_CATEGORY */

#if TMU_UCD_HAS_WIDTH
TMU_DEF int tmu_utf8_width(tmu_utf8_stream stream);
TMU_DEF int tmu_utf8_width_n(const char* str, tm_size_t str_len);
#endif

#if TMU_UCD_HAS_SIMPLE_CASE
TMU_DEF tmu_transform_result tmu_utf8_to_upper_simple(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
TMU_DEF tmu_transform_result tmu_utf8_to_title_simple(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
TMU_DEF tmu_transform_result tmu_utf8_to_lower_simple(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
#endif /* TMU_UCD_HAS_SIMPLE_CASE */

#if TMU_UCD_HAS_SIMPLE_CASE_FOLD
TMU_DEF tmu_transform_result tmu_utf8_to_case_fold_simple(const char* str, tm_size_t str_len, char* out,
                                                          tm_size_t out_len);
TMU_DEF tm_bool tmu_utf8_equals_ignore_case_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len);
TMU_DEF int tmu_utf8_compare_ignore_case_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len);
/* String comparison for humans. See http://stereopsis.com/strcmp4humans.html. */
TMU_DEF int tmu_utf8_human_compare_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len);
#endif /* TMU_UCD_HAS_SIMPLE_CASE_FOLD */

#if TMU_UCD_HAS_SIMPLE_CASE_TOGGLE
TMU_DEF tmu_transform_result tmu_utf8_toggle_case_simple(const char* str, tm_size_t str_len, char* out,
                                                         tm_size_t out_len);
#endif /* TMU_UCD_HAS_SIMPLE_CASE_TOGGLE */

#if TMU_UCD_HAS_FULL_CASE
TMU_DEF tmu_transform_result tmu_utf8_to_upper(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
TMU_DEF tmu_transform_result tmu_utf8_to_title(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
TMU_DEF tmu_transform_result tmu_utf8_to_lower(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
#endif /* TMU_UCD_HAS_FULL_CASE */

#if TMU_UCD_HAS_FULL_CASE_TOGGLE
TMU_DEF tmu_transform_result tmu_utf8_toggle_case(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
#endif /* TMU_UCD_HAS_SIMPLE_CASE_TOGGLE */

#if TMU_UCD_HAS_FULL_CASE_FOLD
TMU_DEF tmu_transform_result tmu_utf8_to_case_fold(const char* str, tm_size_t str_len, char* out, tm_size_t out_len);
TMU_DEF tm_bool tmu_utf8_equals_ignore_case(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len);
TMU_DEF int tmu_utf8_compare_ignore_case(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len);
/* String comparison for humans. See http://stereopsis.com/strcmp4humans.html. */
TMU_DEF int tmu_utf8_human_compare(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len);
#endif /* TMU_UCD_HAS_FULL_CASE_FOLD */

#endif /* defined(TMU_UCD_HAS_CASE_INFO) */


#if !defined(TMU_NO_FILE_IO)
typedef uint64_t tmu_file_time;
typedef struct {
    tmu_file_time file_time;
    tm_errc ec;
} tmu_file_timestamp_result;

typedef struct {
    tm_bool exists;
    tm_errc ec;
} tmu_exists_result;

TMU_DEF tmu_exists_result tmu_file_exists(const char* filename);
TMU_DEF tmu_exists_result tmu_directory_exists(const char* dir);
TMU_DEF tmu_file_timestamp_result tmu_file_timestamp(const char* filename);
TMU_DEF int tmu_compare_file_time(tmu_file_time a, tmu_file_time b);

/* Typedef of utf8 contersion. Every function that returns tmu_utf8_contents_result will
   convert the bytes into utf8, removing byte order mark (bom) and nullterminating. */
typedef tmu_utf8_conversion_result tmu_utf8_contents_result;

TMU_DEF tmu_contents_result tmu_read_file(const char* filename);
TMU_DEF tmu_utf8_contents_result tmu_read_file_as_utf8(const char* filename);
TMU_DEF tmu_utf8_contents_result tmu_read_file_as_utf8_ex(const char* filename, tmu_encoding encoding,
                                                          tmu_validate validate, const char* replace_str);
TMU_DEF void tmu_destroy_contents(tmu_contents* contents);

typedef struct {
    tm_size_t written;
    tm_errc ec;
} tmu_write_file_result;

TMU_DEF tmu_write_file_result tmu_write_file(const char* filename, const void* data, tm_size_t size);
TMU_DEF tmu_write_file_result tmu_write_file_as_utf8(const char* filename, const char* data, tm_size_t size);

enum {
    tmu_create_directory_tree = (1u << 0u), /* Create directory tree, if it doesn't exist. */
    tmu_overwrite = (1u << 1u),             /* Overwrite file if it exists. */
    tmu_write_byte_order_mark = (1u << 2u), /* Only has effect on tmu_write_file_as_utf8_ex. */

    /* Write file by first writing to a temporary file, then move file into destination. */
    tmu_atomic_write = (1u << 4u),
};
TMU_DEF tmu_write_file_result tmu_write_file_ex(const char* filename, const void* data, tm_size_t size, uint32_t flags);
TMU_DEF tmu_write_file_result tmu_write_file_as_utf8_ex(const char* filename, const char* data, tm_size_t size,
                                                        uint32_t flags);

TMU_DEF tm_errc tmu_rename_file(const char* from, const char* to);
TMU_DEF tm_errc tmu_rename_file_ex(const char* from, const char* to, uint32_t flags);

TMU_DEF tm_errc tmu_delete_file(const char* filename);

TMU_DEF tm_errc tmu_create_directory(const char* dir);
TMU_DEF tm_errc tmu_delete_directory(const char* dir);

/* Path related functions. */
TMU_DEF tmu_contents_result tmu_current_working_directory(tm_size_t extra_size);

#if 0
typedef enum {
    tmu_path_is_file,
    tmu_path_is_directory,
} tmu_path_type;

/* Both paths must be absolute, parameter "to" must be a path to a directory, not a filename. */
tmu_contents_result tmu_make_relative_path(const char* from, tmu_path_type from_type, const char* to, tmu_path_type to_type);
#endif

typedef struct {
    char const* const* args;
    int args_count;

    /* Internal allocated buffer used to construct the args array. */
    void* internal_buffer;
    tm_size_t internal_allocated_size;
} tmu_utf8_command_line;

typedef struct {
    tmu_utf8_command_line command_line;
    tm_errc ec;
} tmu_utf8_command_line_result;

/*
Convert Utf-16 command line to Utf-8.
On success, args[args_count] is guaranteed to be 0.
*/
TMU_DEF tmu_utf8_command_line_result tmu_utf8_command_line_from_utf16(tmu_char16 const* const* utf16_args,
                                                                      int utf16_args_count);
TMU_DEF void tmu_utf8_destroy_command_line(tmu_utf8_command_line* command_line);

#if defined(TMU_USE_WINDOWS_H)
/*
Winapi only extension, get command line directly without supplying the Utf-16 arguments.
Result must still be destroyed using tmu_utf8_destroy_command_line.
*/
TMU_DEF tmu_utf8_command_line_result tmu_utf8_winapi_get_command_line();
#endif

#if defined(TMU_USE_CRT)
TMU_DEF FILE* tmu_fopen(const char* filename, const char* mode);
TMU_DEF FILE* tmu_freopen(const char* filename, const char* mode, FILE* current);
#endif

#if defined(__cplusplus)

struct tmu_contents_managed : tmu_contents {
    tmu_contents_managed();
    tmu_contents_managed(tmu_contents_managed&& other);
    tmu_contents_managed& operator=(tmu_contents_managed&& other);
    ~tmu_contents_managed();
};

struct tmu_contents_managed_result {
    tmu_contents_managed contents;
    tm_errc ec;
};
TMU_DEF tmu_contents_managed_result tmu_read_file_managed(const char* filename);

struct tmu_utf8_managed_result {
    tmu_contents_managed contents;
    tm_errc ec;
    tmu_encoding original_encoding;
    tm_bool invalid_codepoints_encountered;
};
TMU_DEF tmu_utf8_managed_result tmu_read_file_as_utf8_managed(const char* filename);
TMU_DEF tmu_utf8_managed_result tmu_read_file_as_utf8_managed_ex(const char* filename, tmu_encoding encoding,
                                                                 tmu_validate validate, const char* replace_str);

TMU_DEF tmu_contents_managed_result tmu_current_working_directory_managed(tm_size_t extra_size);

struct tmu_utf8_command_line_managed : tmu_utf8_command_line {
    tmu_utf8_command_line_managed();
    tmu_utf8_command_line_managed(tmu_utf8_command_line_managed&& other);
    tmu_utf8_command_line_managed& operator=(tmu_utf8_command_line_managed&& other);
    ~tmu_utf8_command_line_managed();
};

struct tmu_utf8_command_line_managed_result {
    tmu_utf8_command_line_managed command_line;
    tm_errc ec;
};
TMU_DEF tmu_utf8_command_line_managed_result
tmu_utf8_command_line_from_utf16_managed(tmu_char16 const* const* utf16_args, int utf16_args_count);

#if defined(TMU_USE_WINDOWS_H)
/*
Winapi only extension, get command line directly without supplying the Utf-16 arguments.
Result must still be destroyed using tmu_utf8_destroy_command_line.
Requires to link against Shell32.lib.
*/
TMU_DEF tmu_utf8_command_line_managed_result tmu_utf8_winapi_get_command_line_managed();
#endif

#if defined(TMU_USE_STL)
TMU_DEF std::vector<char> tmu_read_file_to_vector(const char* filename);
TMU_DEF std::vector<char> tmu_read_file_as_utf8_to_vector(const char* filename);
#endif /* defined(TMU_USE_STL) */

#if defined(TM_STRING_VIEW)
TMU_DEF tmu_exists_result tmu_file_exists(TM_STRING_VIEW filename);
TMU_DEF tmu_exists_result tmu_directory_exists(TM_STRING_VIEW dir);
TMU_DEF tmu_file_timestamp_result tmu_file_timestamp(TM_STRING_VIEW filename);
TMU_DEF tmu_contents_result tmu_read_file(TM_STRING_VIEW filename);
TMU_DEF tmu_write_file_result tmu_write_file(TM_STRING_VIEW filename, const void* data, tm_size_t size);
TMU_DEF tmu_write_file_result tmu_write_file_ex(TM_STRING_VIEW filename, const void* data, tm_size_t size,
                                                uint32_t flags);

TMU_DEF tmu_utf8_contents_result tmu_read_file_as_utf8(TM_STRING_VIEW filename);
TMU_DEF tmu_utf8_contents_result tmu_read_file_as_utf8_ex(TM_STRING_VIEW filename, tmu_encoding encoding,
                                                          tmu_validate validate, TM_STRING_VIEW replace_str);
TMU_DEF tmu_write_file_result tmu_write_file_as_utf8(TM_STRING_VIEW filename, const char* data, tm_size_t size);
TMU_DEF tmu_write_file_result tmu_write_file_as_utf8_ex(TM_STRING_VIEW filename, const char* data, tm_size_t size,
                                                        uint32_t flags);

TMU_DEF tm_errc tmu_rename_file(TM_STRING_VIEW from, TM_STRING_VIEW to);
TMU_DEF tm_errc tmu_rename_file_ex(TM_STRING_VIEW from, TM_STRING_VIEW to, uint32_t flags);
TMU_DEF tmu_contents_managed_result tmu_read_file_managed(TM_STRING_VIEW filename);
TMU_DEF tmu_utf8_managed_result tmu_read_file_as_utf8_managed(TM_STRING_VIEW filename);
TMU_DEF tmu_utf8_managed_result tmu_read_file_as_utf8_managed_ex(TM_STRING_VIEW filename, tmu_encoding encoding,
                                                                 tmu_validate validate, TM_STRING_VIEW replace_str);

TMU_DEF tm_errc tmu_create_directory(TM_STRING_VIEW dir);
TMU_DEF tm_errc tmu_delete_directory(TM_STRING_VIEW dir);

#if defined(TMU_USE_STL)
TMU_DEF std::vector<char> tmu_read_file_to_vector(TM_STRING_VIEW filename);
TMU_DEF std::vector<char> tmu_read_file_as_utf8_to_vector(TM_STRING_VIEW filename);
#endif /* defined(TMU_USE_STL) */
#endif /* defined(TM_STRING_VIEW) */

#endif /* defined(__cplusplus) */

#endif /* !defined(TMU_NO_FILE_IO) */

#endif  // _TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765

#ifdef TM_UNICODE_IMPLEMENTATION

/* Small buffer optimization for path string allocations. */
#ifndef TMU_SBO_SIZE
    #define TMU_SBO_SIZE 260u
#endif

#ifndef TM_UNREFERENCED_PARAM
	#define TM_UNREFERENCED_PARAM(x) ((void)(x))
	#define TM_UNREFERENCED(x) ((void)(x))
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

#if defined(TMU_USE_WINDOWS_H)
	#define TMU_CHAR16LEN TMU_WCSLEN
#else
	#if !defined(__linux__) && (defined(_WIN32) || WCHAR_MAX == 0xFFFFu || WCHAR_MAX == 0x7FFF)
		#define TMU_CHAR16LEN TMU_WCSLEN
	#else
		static size_t tmu_char16len(const tmu_char16* str) {
			const tmu_char16* p = str;
			while(*p) ++p;
			return (size_t)(p - str);
		}
		#define TMU_CHAR16LEN tmu_char16len
	#endif
#endif

#if !defined (TMU_TESTING_TCHAR_DEFINED)
	#if (defined(_WIN32) || defined(TMU_TESTING_MSVC_CRT) || defined(TMU_USE_WINDOWS_H))
	    typedef tmu_char16 tmu_tchar;
	#else
	    typedef char tmu_tchar;
	#endif
#endif

#if !defined(TMU_NO_UCD)
#define TMU_UCD_DEF TMU_DEF
/* This file was generated using tools/unicode_gen from
   https://github.com/to-miz/tm. Do not modify by hand.
   Around 29937 bytes (29.24 kilobytes) of data for lookup tables
   are generated. */

#ifdef __cplusplus
extern "C" {
#endif

/* Codepoint runs: 468 bytes. */
static const size_t tmu_full_case_fold_offset = 0;
static const size_t tmu_codepoint_runs_size = 234;
static const uint16_t tmu_codepoint_runs[234] = {
    /* Full case fold entries. */
    0,
    115, 115, 0,
    105, 775, 0,
    700, 110, 0,
    106, 780, 0,
    953, 776, 769, 0,
    965, 776, 769, 0,
    1381, 1410, 0,
    104, 817, 0,
    116, 776, 0,
    119, 778, 0,
    121, 778, 0,
    97, 702, 0,
    965, 787, 0,
    965, 787, 768, 0,
    965, 787, 769, 0,
    965, 787, 834, 0,
    7936, 953, 0,
    7937, 953, 0,
    7938, 953, 0,
    7939, 953, 0,
    7940, 953, 0,
    7941, 953, 0,
    7942, 953, 0,
    7943, 953, 0,
    7968, 953, 0,
    7969, 953, 0,
    7970, 953, 0,
    7971, 953, 0,
    7972, 953, 0,
    7973, 953, 0,
    7974, 953, 0,
    7975, 953, 0,
    8032, 953, 0,
    8033, 953, 0,
    8034, 953, 0,
    8035, 953, 0,
    8036, 953, 0,
    8037, 953, 0,
    8038, 953, 0,
    8039, 953, 0,
    8048, 953, 0,
    945, 953, 0,
    940, 953, 0,
    945, 834, 0,
    945, 834, 953, 0,
    8052, 953, 0,
    951, 953, 0,
    942, 953, 0,
    951, 834, 0,
    951, 834, 953, 0,
    953, 776, 768, 0,
    953, 834, 0,
    953, 776, 834, 0,
    965, 776, 768, 0,
    961, 787, 0,
    965, 834, 0,
    965, 776, 834, 0,
    8060, 953, 0,
    969, 953, 0,
    974, 953, 0,
    969, 834, 0,
    969, 834, 953, 0,
    102, 102, 0,
    102, 105, 0,
    102, 108, 0,
    102, 102, 105, 0,
    102, 102, 108, 0,
    115, 116, 0,
    1396, 1398, 0,
    1396, 1381, 0,
    1396, 1387, 0,
    1406, 1398, 0,
    1396, 1389, 0
};

typedef struct {
    uint8_t bits0;
    uint8_t bits1;
    uint8_t full_case_fold_index;
    int32_t simple_case_fold_offset;
} tmu_ucd_internal;

/* Unicode data entries: 2844 bytes. */
static const size_t tmu_ucd_entries_size = 237;
static const tmu_ucd_internal tmu_ucd_entries[237] = {
    {0, 0, 0, 0},
    {0, 3, 0, 0},
    {0, 2, 0, 0},
    {64, 3, 0, 0},
    {0, 1, 0, 0},
    {70, 0, 0, 0},
    {4, 0, 0, 0},
    {5, 0, 0, 0},
    {3, 0, 0, 0},
    {9, 0, 0, 32},
    {17, 0, 0, 0},
    {6, 0, 0, 0},
    {5, 14, 0, 0},
    {1, 0, 0, 0},
    {17, 0, 0, 775},
    {17, 0, 1, 0},
    {9, 0, 0, 1},
    {9, 0, 4, 0},
    {17, 0, 7, 0},
    {9, 0, 0, -121},
    {17, 0, 0, -268},
    {9, 0, 0, 210},
    {9, 0, 0, 206},
    {9, 0, 0, 205},
    {9, 0, 0, 79},
    {9, 0, 0, 202},
    {9, 0, 0, 203},
    {9, 0, 0, 207},
    {9, 0, 0, 211},
    {9, 0, 0, 209},
    {9, 0, 0, 213},
    {9, 0, 0, 214},
    {9, 0, 0, 218},
    {9, 0, 0, 217},
    {9, 0, 0, 219},
    {9, 0, 0, 2},
    {25, 0, 0, 1},
    {17, 0, 10, 0},
    {9, 0, 0, -97},
    {9, 0, 0, -56},
    {9, 0, 0, -130},
    {9, 0, 0, 10795},
    {9, 0, 0, -163},
    {9, 0, 0, 10792},
    {9, 0, 0, -195},
    {9, 0, 0, 69},
    {9, 0, 0, 71},
    {2, 5, 0, 0},
    {2, 5, 0, 116},
    {9, 0, 0, 116},
    {9, 0, 0, 38},
    {9, 0, 0, 37},
    {9, 0, 0, 64},
    {9, 0, 0, 63},
    {17, 0, 13, 0},
    {17, 0, 17, 0},
    {17, 0, 0, 1},
    {9, 0, 0, 8},
    {17, 0, 0, -30},
    {17, 0, 0, -25},
    {9, 0, 0, 0},
    {17, 0, 0, -15},
    {17, 0, 0, -22},
    {17, 0, 0, -54},
    {17, 0, 0, -48},
    {9, 0, 0, -60},
    {17, 0, 0, -64},
    {9, 0, 0, -7},
    {9, 0, 0, 80},
    {9, 0, 0, 15},
    {9, 0, 0, 48},
    {17, 0, 21, 0},
    {0, 4, 0, 0},
    {2, 7, 0, 0},
    {1, 4, 0, 0},
    {1, 7, 0, 0},
    {2, 0, 0, 0},
    {9, 0, 0, 7264},
    {1, 8, 0, 0},
    {1, 9, 0, 0},
    {1, 10, 0, 0},
    {17, 0, 0, -8},
    {17, 0, 0, -6222},
    {17, 0, 0, -6221},
    {17, 0, 0, -6212},
    {17, 0, 0, -6210},
    {17, 0, 0, -6211},
    {17, 0, 0, -6204},
    {17, 0, 0, -6180},
    {17, 0, 0, 35267},
    {9, 0, 0, -3008},
    {17, 0, 24, 0},
    {17, 0, 27, 0},
    {17, 0, 30, 0},
    {17, 0, 33, 0},
    {17, 0, 36, 0},
    {17, 0, 0, -58},
    {9, 0, 1, -7615},
    {9, 0, 0, -8},
    {17, 0, 39, 0},
    {17, 0, 42, 0},
    {17, 0, 46, 0},
    {17, 0, 50, 0},
    {17, 0, 54, 0},
    {17, 0, 57, 0},
    {17, 0, 60, 0},
    {17, 0, 63, 0},
    {17, 0, 66, 0},
    {17, 0, 69, 0},
    {17, 0, 72, 0},
    {17, 0, 75, 0},
    {25, 0, 54, -8},
    {25, 0, 57, -8},
    {25, 0, 60, -8},
    {25, 0, 63, -8},
    {25, 0, 66, -8},
    {25, 0, 69, -8},
    {25, 0, 72, -8},
    {25, 0, 75, -8},
    {17, 0, 78, 0},
    {17, 0, 81, 0},
    {17, 0, 84, 0},
    {17, 0, 87, 0},
    {17, 0, 90, 0},
    {17, 0, 93, 0},
    {17, 0, 96, 0},
    {17, 0, 99, 0},
    {25, 0, 78, -8},
    {25, 0, 81, -8},
    {25, 0, 84, -8},
    {25, 0, 87, -8},
    {25, 0, 90, -8},
    {25, 0, 93, -8},
    {25, 0, 96, -8},
    {25, 0, 99, -8},
    {17, 0, 102, 0},
    {17, 0, 105, 0},
    {17, 0, 108, 0},
    {17, 0, 111, 0},
    {17, 0, 114, 0},
    {17, 0, 117, 0},
    {17, 0, 120, 0},
    {17, 0, 123, 0},
    {25, 0, 102, -8},
    {25, 0, 105, -8},
    {25, 0, 108, -8},
    {25, 0, 111, -8},
    {25, 0, 114, -8},
    {25, 0, 117, -8},
    {25, 0, 120, -8},
    {25, 0, 123, -8},
    {17, 0, 126, 0},
    {17, 0, 129, 0},
    {17, 0, 132, 0},
    {17, 0, 135, 0},
    {17, 0, 138, 0},
    {9, 0, 0, -74},
    {25, 0, 129, -9},
    {17, 0, 0, -7173},
    {17, 0, 142, 0},
    {17, 0, 145, 0},
    {17, 0, 148, 0},
    {17, 0, 151, 0},
    {17, 0, 154, 0},
    {9, 0, 0, -86},
    {25, 0, 145, -9},
    {17, 0, 158, 0},
    {17, 0, 162, 0},
    {17, 0, 165, 0},
    {9, 0, 0, -100},
    {17, 0, 169, 0},
    {17, 0, 173, 0},
    {17, 0, 176, 0},
    {17, 0, 179, 0},
    {9, 0, 0, -112},
    {17, 0, 183, 0},
    {17, 0, 186, 0},
    {17, 0, 189, 0},
    {17, 0, 192, 0},
    {17, 0, 195, 0},
    {9, 0, 0, -128},
    {9, 0, 0, -126},
    {25, 0, 186, -9},
    {0, 5, 0, 0},
    {0, 13, 0, 0},
    {70, 3, 0, 0},
    {6, 3, 0, 0},
    {4, 14, 0, 0},
    {9, 0, 0, -7517},
    {9, 0, 0, -8383},
    {9, 0, 0, -8262},
    {9, 0, 0, 28},
    {17, 14, 0, 0},
    {3, 0, 0, 16},
    {5, 0, 0, 26},
    {5, 14, 0, 26},
    {9, 0, 0, -10743},
    {9, 0, 0, -3814},
    {9, 0, 0, -10727},
    {9, 0, 0, -10780},
    {9, 0, 0, -10749},
    {9, 0, 0, -10783},
    {9, 0, 0, -10782},
    {9, 0, 0, -10815},
    {9, 0, 0, -35332},
    {9, 0, 0, -42280},
    {9, 0, 0, -42308},
    {9, 0, 0, -42319},
    {9, 0, 0, -42315},
    {9, 0, 0, -42305},
    {9, 0, 0, -42258},
    {9, 0, 0, -42282},
    {9, 0, 0, -42261},
    {9, 0, 0, 928},
    {17, 0, 0, -38864},
    {1, 11, 0, 0},
    {0, 12, 0, 0},
    {0, 11, 0, 0},
    {1, 12, 0, 0},
    {0, 0, 0, 0},
    {17, 0, 199, 0},
    {17, 0, 202, 0},
    {17, 0, 205, 0},
    {17, 0, 208, 0},
    {17, 0, 212, 0},
    {17, 0, 216, 0},
    {17, 0, 219, 0},
    {17, 0, 222, 0},
    {17, 0, 225, 0},
    {17, 0, 228, 0},
    {17, 0, 231, 0},
    {1, 5, 0, 0},
    {9, 0, 0, 40},
    {9, 0, 0, 34},
    {0, 14, 0, 0},
    {5, 6, 0, 0},
    {5, 5, 0, 0}
};

/* Unicode data stage one: 1025 bytes. */
static const size_t tmu_ucd_stage_one_size = 1025;
static const uint8_t tmu_ucd_stage_one[1025] = {
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
     32, 33,199,200, 34, 35, 36, 37,201,202,202,202,203, 38, 39, 40,
     41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
     57, 58, 59, 60,204,204, 61, 62, 63, 64,204, 65,205,206, 66, 67,
    204,204,207, 68,204,204, 69, 70, 71, 72, 73, 74, 75, 76,204, 77,
     78, 79, 80, 81, 82, 83,204,204,208,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198, 84,208,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,209,
    202,202,202,202,202,202,202,202,202, 85,202,202, 86, 87, 88, 89,
     90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,
     99,100,101,102,103,104,105, 99,100,101,102,103,104,105, 99,100,
    101,102,103,104,105, 99,100,101,102,103,104,105, 99,100,101,102,
    103,104,105, 99,100,101,102,103,104,105, 99,100,101,102,103,104,
    105, 99,100,101,102,103,104,105, 99,100,101,102,103,104,105, 99,
    100,101,102,103,104,105, 99,100,101,102,103,104,105, 99,100,106,
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,
    211,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,212,202,202,213,214,107,108,202,202,109,110,111,112,113,114,
    115,215,116,117,198,118,119,120,121,122,123,198,202,202,124,198,
    125,126,127,128,129,130,131,132,216,133,134,198,217,198,135,198,
    136,137,138,139,140,141,142,198,143,144,198,145,146,147,148,198,
    149,150,198,198,151,152,198,198,153,154,155,156,198,157,198,198,
    202,202,202,202,202,202,202,218,158,202,219,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    202,202,202,202,202,202,202,202,220,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,202,202,202,202,221,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    202,202,202,202,159,160,161,222,198,198,198,198,162,163,164,165,
    208,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,223,
    202,202,202,202,202,224,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    202,202,225,202,202,226,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,166,167,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    204,227,168,169,170,228,171,198,172,173,174,175,176,177,178,179,
    204,204,204,204,180,181,198,198,198,198,198,198,198,198,198,198,
    182,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,
    202,183,184,198,198,198,198,198,229,185,198,198,186,187,198,198,
    230,188,189,190,191,231,232,233,232,232,234,232,235,192,236,193,
    194,195,196,197,237,231,231,231,231,231,231,231,231,231,231,238,
    208
};

/* Unicode data stage two: 25344 bytes.*/
static const uint32_t tmu_ucd_block_size = 128;
static const uint32_t tmu_ucd_stage_two_blocks_count = 249;
static const size_t tmu_ucd_stage_two_size = 25344;
static const uint8_t tmu_ucd_stage_two[25344] = {
    /* Block 0 */
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  1,  3,  4,  1,  1,
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
      5,  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,  7,  6,  6,  6,  6,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  6,  7,  7,  7,  6,
      6,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  6,  6,  6,  7,  6,
      7, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6,  7,  6,  7,  1,
    /* Block 1 */
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     11,  6,  7,  7,  7,  7,  7,  6,  7, 12, 13,  6,  7,  1, 12,  7,
      7,  7,  8,  8,  7, 14,  6,  6,  7,  8, 13,  6,  8,  8,  8,  6,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
      9,  9,  9,  9,  9,  9,  9,  7,  9,  9,  9,  9,  9,  9,  9, 15,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10,  7, 10, 10, 10, 10, 10, 10, 10, 10,
    /* Block 2 */
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     17, 10, 16, 10, 16, 10, 16, 10, 10, 16, 10, 16, 10, 16, 10, 16,
     10, 16, 10, 16, 10, 16, 10, 16, 10, 18, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 19, 16, 10, 16, 10, 16, 10, 20,
    /* Block 3 */
     10, 21, 16, 10, 16, 10, 22, 16, 10, 23, 23, 16, 10, 10, 24, 25,
     26, 16, 10, 23, 27, 10, 28, 29, 16, 10, 10, 10, 28, 30, 10, 31,
     16, 10, 16, 10, 16, 10, 32, 16, 10, 32, 10, 10, 16, 10, 32, 16,
     10, 33, 33, 16, 10, 16, 10, 34, 16, 10, 10, 13, 16, 10, 10, 10,
     13, 13, 13, 13, 35, 36, 10, 35, 36, 10, 35, 36, 10, 16, 10, 16,
     10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     37, 35, 36, 10, 16, 10, 38, 39, 16, 10, 16, 10, 16, 10, 16, 10,
    /* Block 4 */
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     40, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 10, 10, 10, 10, 10, 10, 41, 16, 10, 42, 43, 10,
     10, 16, 10, 44, 45, 46, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    /* Block 5 */
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 13, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13,  7,  7,  7,  7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
     13, 13, 13, 13, 13,  7,  7,  7,  7,  7,  7,  7, 13,  7, 13,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 6 */
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 48, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     16, 10, 16, 10, 13,  7, 16, 10,  0,  0, 13, 10, 10, 10,  6, 49,
    /* Block 7 */
      0,  0,  0,  0,  7,  7, 50,  6, 51, 51, 51,  0, 52,  0, 53, 53,
     54,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
      9,  9,  0,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10,
     55, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 56, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 57,
     58, 59, 60, 60, 60, 61, 62, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     63, 64, 10, 10, 65, 66,  7, 16, 10, 67, 16, 10, 10, 40, 40, 40,
    /* Block 8 */
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
    /* Block 9 */
     16, 10,  7, 47, 47, 47, 47, 47, 47, 47, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     69, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
    /* Block 10 */
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
      0, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
     70, 70, 70, 70, 70, 70, 70,  0,  0, 13,  6,  6,  6,  6,  6,  6,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    /* Block 11 */
     10, 10, 10, 10, 10, 10, 10, 71, 10,  6,  6,  0,  0,  7,  7,  7,
      0, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  6, 47,
      6, 47, 47,  6, 47, 47,  6, 47,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0, 13,
     13, 13, 13,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 12 */
     72, 72, 72, 72, 72, 72,  7,  7,  7,  6,  6,  7,  6,  6,  7,  7,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  6,  1,  0,  6,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  6,  6,  6, 13, 13,
     47, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 13 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13,  6, 13, 47, 47, 47, 47, 47, 47, 47, 72,  7, 47,
     47, 47, 47, 47, 47, 13, 13, 47, 47,  7, 47, 47, 47, 47, 13, 13,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 13, 13, 13,  7,  7, 13,
    /* Block 14 */
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0, 72,
     13, 47, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  0,  0, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 15 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 13, 13,  7,  6,  6,  6, 13,  0,  0, 47,  7,  7,
    /* Block 16 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 13, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 13, 47, 47, 47, 13, 47, 47, 47, 47, 47,  0,  0,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 47, 47, 47,  0,  0,  6,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 17 */
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 72, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
    /* Block 18 */
     47, 47, 47, 73, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 47, 73, 47, 13, 73, 73,
     73, 47, 47, 47, 47, 47, 47, 47, 47, 73, 73, 73, 73, 47, 73, 73,
     13, 47, 47, 47, 47, 47, 47, 47, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 47, 47,  6,  6,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      6, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 19 */
     13, 47, 73, 73,  0, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0, 13,
     13,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13,  0, 13,  0,  0,  0, 13, 13, 13, 13,  0,  0, 47, 13, 47, 73,
     73, 47, 47, 47, 47,  0,  0, 73, 73,  0,  0, 73, 73, 47, 13,  0,
      0,  0,  0,  0,  0,  0,  0, 47,  0,  0,  0,  0, 13, 13,  0, 13,
     13, 13, 47, 47,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     13, 13,  7,  7,  8,  8,  8,  8,  8,  8,  7,  7, 13,  6, 47,  0,
    /* Block 20 */
      0, 47, 47, 73,  0, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0, 13,
     13,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13,  0, 13, 13,  0, 13, 13,  0, 13, 13,  0,  0, 47,  0, 73, 73,
     73, 47, 47,  0,  0,  0,  0, 47, 47,  0,  0, 47, 47, 47,  0,  0,
      0, 47,  0,  0,  0,  0,  0,  0,  0, 13, 13, 13, 13,  0, 13,  0,
      0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     47, 47, 13, 13, 13, 47,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 21 */
      0, 47, 47, 73,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13,
     13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13,  0, 13, 13,  0, 13, 13, 13, 13, 13,  0,  0, 47, 13, 73, 73,
     73, 47, 47, 47, 47, 47,  0, 47, 47, 73,  0, 73, 73, 47,  0,  0,
     13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 47, 47,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      6,  7,  0,  0,  0,  0,  0,  0,  0, 13, 47, 47, 47, 47, 47, 47,
    /* Block 22 */
      0, 47, 73, 73,  0, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0, 13,
     13,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13,  0, 13, 13,  0, 13, 13, 13, 13, 13,  0,  0, 47, 13, 47, 47,
     73, 47, 47, 47, 47,  0,  0, 73, 73,  0,  0, 73, 73, 47,  0,  0,
      0,  0,  0,  0,  0,  0, 47, 47,  0,  0,  0,  0, 13, 13,  0, 13,
     13, 13, 47, 47,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      7, 13,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 23 */
      0,  0, 47, 13,  0, 13, 13, 13, 13, 13, 13,  0,  0,  0, 13, 13,
     13,  0, 13, 13, 13, 13,  0,  0,  0, 13, 13,  0, 13,  0, 13, 13,
      0,  0,  0, 13, 13,  0,  0,  0, 13, 13, 13,  0,  0,  0, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0, 47, 73,
     47, 73, 73,  0,  0,  0, 73, 73, 73,  0, 73, 73, 73, 47,  0,  0,
     13,  0,  0,  0,  0,  0,  0, 47,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  7,  7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  0,
    /* Block 24 */
     47, 73, 73, 73, 47, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13,
     13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0, 13, 47, 47,
     47, 73, 73, 73, 73,  0, 47, 47, 47,  0, 47, 47, 47, 47,  0,  0,
      0,  0,  0,  0,  0, 47, 47,  0, 13, 13, 13,  0,  0,  0,  0,  0,
     13, 13, 47, 47,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  7,
    /* Block 25 */
     13, 47, 73, 73,  6, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13,
     13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13,  0, 13, 13, 13, 13, 13,  0,  0, 47, 13, 73, 47,
     73, 73, 47, 73, 73,  0, 47, 73, 73,  0, 73, 73, 47, 47,  0,  0,
      0,  0,  0,  0,  0, 47, 47,  0,  0,  0,  0,  0,  0,  0, 13,  0,
     13, 13, 47, 47,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      0, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 26 */
     47, 47, 73, 73,  0, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13,
     13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 47, 47, 13, 47, 73,
     73, 47, 47, 47, 47,  0, 73, 73, 73,  0, 73, 73, 73, 47, 74,  7,
      0,  0,  0,  0, 13, 13, 13, 47,  8,  8,  8,  8,  8,  8,  8, 13,
     13, 13, 47, 47,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  7, 13, 13, 13, 13, 13, 13,
    /* Block 27 */
      0,  0, 73, 73,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  0,  0,  0, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13,  0,  0,
     13, 13, 13, 13, 13, 13, 13,  0,  0,  0, 47,  0,  0,  0,  0, 47,
     73, 73, 47, 47, 47,  0, 47,  0, 73, 73, 73, 73, 73, 73, 73, 47,
      0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      0,  0, 73, 73,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 28 */
      0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 47, 13, 75, 47, 47, 47, 47, 47, 47, 47,  0,  0,  0,  0,  7,
     13, 13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47, 47, 47, 47,  6,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  6,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 29 */
      0, 13, 13,  0, 13,  0,  0, 13, 13,  0, 13,  0,  0, 13,  0,  0,
      0,  0,  0,  0, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13,
      0, 13, 13, 13,  0, 13,  0, 13,  0,  0, 13, 13,  0, 13, 13, 13,
     13, 47, 13, 75, 47, 47, 47, 47, 47, 47,  0, 47, 47, 13,  0,  0,
     13, 13, 13, 13, 13,  0, 13,  0, 47, 47, 47, 47, 47, 47,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0, 13, 13, 13, 13,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 30 */
     13,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  7,  6,  7,  7,  7, 47, 47,  7,  7,  7,  7,  7,  7,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  7, 47,  7, 47,  7, 47,  6,  6,  6,  6, 73, 73,
     13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,
      0, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 73,
    /* Block 31 */
     47, 47, 47, 47, 47,  6, 47, 47, 13, 13, 13, 13, 13, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47,  0, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  0,  7,  7,
      7,  7,  7,  7,  7,  7, 47,  7,  7,  7,  7,  7,  7,  0,  7,  7,
      6,  6,  6,  6,  6,  7,  7,  7,  7,  6,  6,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 32 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 76, 76, 47, 47, 47,
     47, 73, 47, 47, 47, 47, 47, 47, 76, 47, 47, 73, 73, 47, 47, 13,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  6,  6,  6,  6,  6,
     13, 13, 13, 13, 13, 13, 73, 73, 47, 47, 13, 13, 13, 13, 47, 47,
     47, 13, 76, 76, 76, 13, 13, 76, 76, 76, 76, 76, 76, 76, 13, 13,
     13, 47, 47, 47, 47, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 33 */
     13, 13, 47, 76, 73, 47, 47, 76, 76, 76, 76, 76, 76, 47, 13, 76,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 76, 76, 76, 47,  7,  7,
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
     77, 77, 77, 77, 77, 77,  0, 77,  0,  0,  0,  0,  0, 77,  0,  0,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6, 13, 10, 10, 10,
    /* Block 34 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13,  0,  0,
     13, 13, 13, 13, 13, 13, 13,  0, 13,  0, 13, 13, 13, 13,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 35 */
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13,  0, 13, 13, 13, 13,  0,  0, 13, 13, 13, 13, 13, 13, 13,  0,
     13,  0, 13, 13, 13, 13,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 36 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13,  0, 13, 13, 13, 13,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0, 47, 47, 47,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,
    /* Block 37 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60,  0,  0, 81, 81, 81, 81, 81, 81,  0,  0,
    /* Block 38 */
      5, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  6,  6,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  6,  6,  6,  8,  8,
      8, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,
    /* Block 39 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13,
     13, 13, 47, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 47, 47, 47,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13,
     13,  0, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 40 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 47, 47, 73, 47, 47, 47, 47, 47, 47, 47, 73, 73,
     73, 73, 73, 73, 73, 73, 47, 73, 73, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47,  6,  6,  6, 13,  6,  6,  6,  7, 13, 47,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
    /* Block 41 */
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6, 47, 47, 47,  1,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,
    /* Block 42 */
     13, 13, 13, 13, 13, 47, 47, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 47, 13,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 43 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,
     47, 47, 47, 73, 73, 73, 73, 47, 47, 73, 73, 73,  0,  0,  0,  0,
     73, 73, 47, 73, 73, 73, 73, 73, 73, 47, 47, 47,  0,  0,  0,  0,
      7,  0,  0,  0,  6,  6,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,
     13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 44 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 45 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 47, 47, 73, 73, 47,  0,  0,  6,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 73, 47, 73, 47, 47, 47, 47, 47, 47, 47,  0,
     47, 76, 47, 76, 76, 47, 47, 47, 47, 47, 47, 47, 47, 73, 73, 73,
     73, 73, 73, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  0,  0, 47,
    /* Block 46 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
      6,  6,  6,  6,  6,  6,  6, 13,  6,  6,  6,  6,  6,  6,  0,  0,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 47 */
     47, 47, 47, 47, 73, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 47, 73, 47, 47, 47, 47, 47, 73, 47, 73, 73, 73,
     73, 73, 47, 73, 73, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  6,  6,  6,  6,  6,
      6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 47, 47, 47, 47, 47,
     47, 47, 47, 47,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,  0,  0,
    /* Block 48 */
     47, 47, 73, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 73, 47, 47, 47, 47, 73, 73, 47, 47, 73, 47, 47, 47, 13, 13,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 47, 73, 47, 47, 73, 73, 73, 47, 73, 47,
     47, 47, 73, 73,  0,  0,  0,  0,  0,  0,  0,  0,  6,  6,  6,  6,
    /* Block 49 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 73, 73, 73, 73, 73, 73, 73, 73, 47, 47, 47, 47,
     47, 47, 47, 47, 73, 73, 47, 47,  0,  0,  0,  6,  6,  6,  6,  6,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0, 13, 13, 13,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  6,  6,
    /* Block 50 */
     82, 83, 84, 85, 85, 86, 87, 88, 89,  0,  0,  0,  0,  0,  0,  0,
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  0,  0, 90, 90, 90,
      6,  6,  6,  6,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,
     47, 47, 47,  6, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 73, 47, 47, 47, 47, 47, 47, 47, 13, 13, 13, 13, 47, 13, 13,
     13, 13, 73, 73, 47, 13, 13, 73, 47, 47,  0,  0,  0,  0,  0,  0,
    /* Block 51 */
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 13, 10, 10, 10, 10, 10, 10, 10,
    /* Block 52 */
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  0, 47, 47, 47, 47, 47,
    /* Block 53 */
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
    /* Block 54 */
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 91, 92, 93, 94, 95, 96, 10, 10, 97, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
    /* Block 55 */
     10, 10, 10, 10, 10, 10, 10, 10, 98, 98, 98, 98, 98, 98, 98, 98,
     10, 10, 10, 10, 10, 10,  0,  0, 98, 98, 98, 98, 98, 98,  0,  0,
     10, 10, 10, 10, 10, 10, 10, 10, 98, 98, 98, 98, 98, 98, 98, 98,
     10, 10, 10, 10, 10, 10, 10, 10, 98, 98, 98, 98, 98, 98, 98, 98,
     10, 10, 10, 10, 10, 10,  0,  0, 98, 98, 98, 98, 98, 98,  0,  0,
     99, 10,100, 10,101, 10,102, 10,  0, 98,  0, 98,  0, 98,  0, 98,
     10, 10, 10, 10, 10, 10, 10, 10, 98, 98, 98, 98, 98, 98, 98, 98,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  0,  0,
    /* Block 56 */
    103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,
    119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,
    135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,
     10, 10,151,152,153,  0,154,155, 98, 98,156,156,157,  7,158,  7,
      7,  7,159,160,161,  0,162,163,164,164,164,164,165,  7,  7,  7,
     10, 10,166, 54,  0,  0,167,168, 98, 98,169,169,  0,  7,  7,  7,
     10, 10,170, 55,171, 10,172,173, 98, 98,174,174, 67,  7,  7,  7,
      0,  0,175,176,177,  0,178,179,180,180,181,181,182,  7,  7,  0,
    /* Block 57 */
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  1,183,184,  1,  1,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  6,  6,  6,  6,  6,185,186,  1,  1,  1,  1,  1, 11,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,187,  6,  6,  6,
      6,  6,  6,  6,  7,  6,  6,  6,  6,187,  6,  6,  6,  6,  6,  6,
      6,  6,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
      8, 13,  0,  0,  8,  8,  8,  8,  8,  8,  7,  7,  7,  6,  6, 13,
    /* Block 58 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  6,  6,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 59 */
      7,  7, 60,  7,  7,  7,  7, 60,  7,  7, 10, 60, 60, 60, 10, 10,
     60, 60, 60, 10,  7, 60,  7,  7,  7, 60, 60, 60, 60, 60,  7,  7,
      7,  7, 12,  7, 60,  7,188,  7, 60,  7,189,190, 60, 60,  7, 10,
     60, 60,191, 60, 10, 13, 13, 13, 13,192,  7,  7, 10, 10, 60, 60,
      7,  7,  7,  7,  7, 60, 10, 10, 10, 10,  7,  7,  7,  7, 10,  7,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    /* Block 60 */
      8,  8,  8, 16, 10,  8,  8,  8,  8,  8,  7,  7,  0,  0,  0,  0,
      7,  7,  7,  7, 12, 12, 12, 12, 12, 12,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7, 12, 12,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 61 */
      7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12, 12,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7, 12,  6,  6,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 62 */
      7,  7,  7,  7,  7,  7,  7,  7, 12,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12,  7,  7,  7,  7, 12, 12, 12,  7,  7,  7,  7,  7,
    /* Block 63 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    /* Block 64 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,194,194,194,194,194,194,194,194,194,194,
    194,194,195,194,194,194,194,194,194,194,194,194,194,194,194,194,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    /* Block 65 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12, 12,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,
     12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12, 12, 12, 12,  7,
    /* Block 66 */
     12, 12, 12, 12, 12, 12,  7,  7, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12,  7, 12,  7, 12,  7,  7,  7,  7,  7,  7, 12,  7,  7,
      7, 12,  7,  7,  7,  7,  7,  7, 12,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7, 12, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7, 12,  7,  7, 12,  7,  7,  7,  7, 12,  7, 12,  7,
      7,  7,  7, 12, 12, 12,  7, 12,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7, 12, 12, 12, 12, 12,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  6,  6,  6,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    /* Block 67 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  7, 12, 12, 12,  7,  7,  7,  7,  7,  7,  7,  7,
      7, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
     12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12,
      7,  7,  7,  7,  7,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 68 */
      7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  7,  7,
    /* Block 69 */
      7,  7,  7,  7,  7, 12, 12, 12,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12, 12,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
     12,  7,  7,  7,  7, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  0,  0,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 70 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  0,  0,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  0,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,
    /* Block 71 */
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  0,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  0,
     16, 10,196,197,198, 10, 10, 16, 10, 16, 10, 16, 10,199,200,201,
    202, 10, 16, 10, 10, 16, 10, 10, 10, 10, 10, 10, 13, 13,203,203,
    /* Block 72 */
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 10,  7,  7,  7,  7,  7,  7, 16, 10, 16, 10, 47,
     47, 47, 16, 10,  0,  0,  0,  0,  0,  6,  6,  6,  6,  8,  6,  6,
    /* Block 73 */
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10,  0, 10,  0,  0,  0,  0,  0, 10,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0, 13,
      6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 47,
    /* Block 74 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13,  0,
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13,  0,
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13,  0,
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13,  0,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
    /* Block 75 */
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6, 13,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 76 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 77 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,
    /* Block 78 */
      5,  6,  6,  6,  7, 13, 13,  8,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      7,  8,  8,  8,  8,  8,  8,  8,  8,  8, 47, 47, 47, 47, 47, 47,
    187, 13, 13, 13, 13, 13,  7,  7,  8,  8,  8, 13, 13,187,  7,  7,
      0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 79 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  0,  0, 47, 47,  7,  7, 13, 13, 13,
      6, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  6, 13, 13, 13, 13,
    /* Block 80 */
      0,  0,  0,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 81 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,
      7,  7,  8,  8,  8,  8,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 82 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,
      7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 83 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7, 12,  7, 12,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,
    /* Block 84 */
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 85 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  6,  6,
    /* Block 86 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  6,  6,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 13, 13,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 13, 47,
     47, 47, 47,  6, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  6, 13,
    /* Block 87 */
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 13, 13, 47, 47,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     47, 47,  6,  6,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 88 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      7,  7, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     10, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     13, 10, 10, 10, 10, 10, 10, 10, 10, 16, 10, 16, 10,204, 16, 10,
    /* Block 89 */
     16, 10, 16, 10, 16, 10, 16, 10, 13,  7,  7, 16, 10,205, 10, 13,
     16, 10, 16, 10, 10, 10, 16, 10, 16, 10, 16, 10, 16, 10, 16, 10,
     16, 10, 16, 10, 16, 10, 16, 10, 16, 10,206,207,208,209,206, 10,
    210,211,212,213, 16, 10, 16, 10, 16, 10,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0, 13, 13, 13, 10, 13, 13, 13, 13, 13,
    /* Block 90 */
     13, 13, 47, 13, 13, 13, 47, 13, 13, 13, 13, 47, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 73, 73, 47, 47, 73,  7,  7,  7,  7,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 91 */
     73, 73, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
     73, 73, 73, 73, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,  6,  6,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 13, 13, 13, 13, 13, 13,  6,  6,  6, 13,  6, 13, 13, 47,
    /* Block 92 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47, 47, 47, 47,  6,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 73, 73,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  0,  0,  0,
    /* Block 93 */
     47, 47, 47, 73, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 47, 73, 73, 47, 47, 47, 47, 73, 73, 47, 73, 73, 73,
     73,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0, 13,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  6,  6,
     13, 13, 13, 13, 13, 47, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 13, 13, 13, 13, 13,  0,
    /* Block 94 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47, 47, 73,
     73, 47, 47, 73, 73, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 47, 13, 13, 13, 13, 13, 13, 13, 13, 47, 73,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  6,  6,  6,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  7,  7,  7, 13, 76, 47, 76, 13, 13,
    /* Block 95 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     47, 13, 47, 47, 47, 13, 13, 47, 47, 13, 13, 13, 13, 13, 47, 47,
     13, 47, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13, 13, 13,  6,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 73, 47, 47, 73, 73,
      6,  6, 13, 13, 13, 73, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 96 */
      0, 13, 13, 13, 13, 13, 13,  0,  0, 13, 13, 13, 13, 13, 13,  0,
      0, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13,  0,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  7, 13, 13, 13, 13,
     10, 10, 10, 10, 10, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,
    /* Block 97 */
    214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,
    214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,
    214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,
    214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 73, 73, 47, 73, 73, 47, 73, 73,  6, 73, 47,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
    /* Block 98 */
    215,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    /* Block 99 */
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    /* Block 100 */
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    /* Block 101 */
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    /* Block 102 */
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    /* Block 103 */
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    /* Block 104 */
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    /* Block 105 */
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,217,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,217,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    217,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    /* Block 106 */
    216,216,216,216,216,216,216,216,217,216,216,216,216,216,216,216,
    216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,
    216,216,216,218,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,
     79, 79, 79, 79, 79, 79, 79,  0,  0,  0,  0, 80, 80, 80, 80, 80,
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  0,  0,  0,  0,
    /* Block 107 */
    220,221,222,223,224,225,225,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,226,227,228,229,230,  0,  0,  0,  0,  0, 13, 47, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  7, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13,  0, 13,  0,
     13, 13,  0, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 108 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 109 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  6,  6,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 110 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      0,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  7,  7,  0,  0,
    /* Block 111 */
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  0,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  7,  6,  7,  7,  7,  0,  6,  7,  6,  6,  0,  0,  0,  0,
     13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 112 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  1,
    /* Block 113 */
      0,  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,  7,  6,  6,  6,  6,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  6,  7,  7,  7,  6,
      6,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  6,  6,  6,  7,  6,
      7, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6,  7,  6,  7,  6,
      6,  6,  6,  6,  6,  6, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 114 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,231,231,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,
      0,  0, 13, 13, 13, 13, 13, 13,  0,  0, 13, 13, 13, 13, 13, 13,
      0,  0, 13, 13, 13, 13, 13, 13,  0,  0, 13, 13, 13,  0,  0,  0,
      7,  7,  7,  7,  7,  7,  7,  0,  7,  7,  7,  7,  7,  7,  7,  0,
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  7,  7,  0,  0,
    /* Block 115 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13,  0, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 116 */
      6,  6,  6,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  0,  0,  0,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  7,  7,  7,  7,
    /* Block 117 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  7,  7,  7,  0,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,
      7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 47,  0,  0,
    /* Block 118 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     47,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,
    /* Block 119 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      8,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13,  8, 13, 13, 13, 13, 13, 13, 13, 13,  8,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47,  0,  0,  0,  0,  0,
    /* Block 120 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13,  0,  0,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13,
      6,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 121 */
    232,232,232,232,232,232,232,232,232,232,232,232,232,232,232,232,
    232,232,232,232,232,232,232,232,232,232,232,232,232,232,232,232,
    232,232,232,232,232,232,232,232, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 122 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
    232,232,232,232,232,232,232,232,232,232,232,232,232,232,232,232,
    232,232,232,232,232,232,232,232,232,232,232,232,232,232,232,232,
    232,232,232,232,  0,  0,  0,  0, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  0,  0,  0,  0,
    /* Block 123 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 124 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 125 */
     13, 13, 13, 13, 13, 13,  0,  0, 13,  0, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  0, 13, 13,  0,  0,  0, 13,  0,  0, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  0,  6,  8,  8,  8,  8,  8,  8,  8,  8,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13,  7,  7,  8,  8,  8,  8,  8,  8,  8,
    /* Block 126 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,
      0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13,  0, 13, 13,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,
    /* Block 127 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  8,  8,  8,  8,  8,  8,  0,  0,  0,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  6,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 128 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  8,  8, 13, 13,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    /* Block 129 */
     13, 47, 47, 47,  0, 47, 47,  0,  0,  0,  0,  0, 47, 47, 47, 47,
     13, 13, 13, 13,  0, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  0,  0, 47, 47, 47,  0,  0,  0,  0, 47,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  8,  8,  6,
    /* Block 130 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  8,  8,  8,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13,  7, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 47, 47,  0,  0,  0,  0,  8,  8,  8,  8,  8,
      6,  6,  6,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 131 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  0,  0,  0,  6,  6,  6,  6,  6,  6,  6,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,
    /* Block 132 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13,  0,  0,  0,  0,  0,  0,  0,  6,  6,  6,  6,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 133 */
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,
     52, 52, 52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,
    /* Block 134 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 47, 47, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 135 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8, 13,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47,  8,  8,  8,  8,  6,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 136 */
     73, 47, 73, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47,  6,  6,  6,  6,  6,  6,  6,  0,  0,
      0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 47,
    /* Block 137 */
     47, 47, 73, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     73, 73, 73, 47, 47, 47, 47, 73, 73, 47, 47,  6,  6, 72,  6,  6,
      6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 72,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
    /* Block 138 */
     47, 47, 47, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 47, 47, 47, 47, 47, 73, 47, 47, 47,
     47, 47, 47, 47, 47,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      6,  6,  6,  6, 13, 73, 73,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 47,  6,  6, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 139 */
     47, 47, 73, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 73, 73, 73, 47, 47, 47, 47, 47, 47, 47, 47, 47, 73,
     73, 13, 74, 74, 13,  6,  6,  6,  6, 47, 47, 47, 47,  6,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 13,  6, 13,  6,  6,  6,
      0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 140 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 73, 73, 73, 47,
     47, 47, 73, 73, 47, 73, 47, 47,  6,  6,  6,  6,  6,  6, 47,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 141 */
     13, 13, 13, 13, 13, 13, 13,  0, 13,  0, 13, 13, 13, 13,  0, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  6,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 47,
     73, 73, 73, 47, 47, 47, 47, 47, 47, 47, 47,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
    /* Block 142 */
     47, 47, 73, 73,  0, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0, 13,
     13,  0,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13,  0, 13, 13,  0, 13, 13, 13, 13, 13,  0, 47, 47, 13, 47, 73,
     47, 73, 73, 73, 73,  0,  0, 73, 73,  0,  0, 73, 73, 73,  0,  0,
     13,  0,  0,  0,  0,  0,  0, 47,  0,  0,  0,  0,  0, 13, 13, 13,
     13, 13, 73, 73,  0,  0, 47, 47, 47, 47, 47, 47, 47,  0,  0,  0,
     47, 47, 47, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 143 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 73, 73, 73, 47, 47, 47, 47, 47, 47, 47, 47,
     73, 73, 47, 47, 47, 73, 47, 13, 13, 13, 13,  6,  6,  6,  6,  6,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  6,  0,  6, 47,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 144 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     47, 73, 73, 47, 47, 47, 47, 47, 47, 73, 47, 73, 73, 47, 73, 47,
     47, 73, 47, 47, 13, 13,  6, 13,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 145 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 47,
     73, 73, 47, 47, 47, 47,  0,  0, 73, 73, 73, 73, 47, 47, 73, 47,
     47,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  6,  6,  6,  6,  6, 13, 13, 13, 13, 47, 47,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 146 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     73, 73, 73, 47, 47, 47, 47, 47, 47, 47, 47, 73, 73, 47, 73, 47,
     47,  6,  6,  6, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 147 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 47, 73, 47, 73, 73,
     47, 47, 47, 47, 47, 47, 73, 47,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 148 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0, 47, 47, 47,
     73, 73, 47, 47, 47, 47, 73, 47, 47, 47, 47, 47,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  6,  6,  7,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 149 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 73, 73, 73, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 73, 47, 47,  6,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 150 */
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13,
    /* Block 151 */
     13, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 47, 47, 47, 47, 47, 47, 73, 74, 47, 47, 47, 47,  6,
      6,  6,  6,  6,  6,  6,  6, 47,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 47, 47, 47, 47, 47, 47, 73, 73, 47, 47, 47, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 152 */
     13, 13, 13, 13,  0,  0, 74, 74, 74, 74, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 73, 47, 47,  6,  6,  6, 13,  6,  6,
      6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,
    /* Block 153 */
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 73,
     47, 47, 47, 47, 47, 47, 47,  0, 47, 47, 47, 47, 47, 47, 73, 47,
     13,  6,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,
      6,  6, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 154 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      0,  0, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47,  0, 73, 47, 47, 47, 47, 47, 47,
     47, 73, 47, 47, 73, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 155 */
     13, 13, 13, 13, 13, 13, 13,  0, 13, 13,  0, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 47, 47, 47, 47, 47, 47,  0,  0,  0, 47,  0, 47, 47,  0, 47,
     47, 47, 47, 47, 47, 47, 74, 47,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13,  0, 13, 13,  0, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    /* Block 156 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 73, 73, 73, 73, 73,  0,
     47, 47,  0, 73, 73, 47, 73, 47, 13,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 157 */
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 47, 47, 73, 73,  6,  6,  0,  0,  0,  0,  0,  0,  0,
    /* Block 158 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,
      6,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 159 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  6,  6,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 160 */
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,
     47, 47, 47, 47, 47,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 161 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     47, 47, 47, 47, 47, 47, 47,  6,  6,  6,  6,  6,  7,  7,  7,  7,
     13, 13, 13, 13,  6,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  8,  8,  8,  8,  8,
      8,  8,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0, 13, 13, 13,
    /* Block 162 */
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    /* Block 163 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  6,  6,  6,  6,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 164 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  0,
    /* Block 165 */
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 47,
     47, 47, 47, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     13, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 166 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,
    /* Block 167 */
     13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,  0,  0,  0,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  7, 47, 47,  6,
      1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 168 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  0,  0,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7, 47, 73, 47, 47, 47,  7,  7,  7, 73, 47, 47,
     47, 47, 47,  1,  1,  1,  1,  1,  1,  1,  1, 47, 47, 47, 47, 47,
    /* Block 169 */
     47, 47, 47,  7,  7, 47, 47, 47, 47, 47, 47, 47,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 47, 47, 47, 47,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 170 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7, 47, 47, 47,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 171 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,
    /* Block 172 */
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 10, 10,
     10, 10, 10, 10, 10,  0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    /* Block 173 */
     60, 60, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 60,  0, 60, 60,
      0,  0, 60,  0,  0, 60, 60,  0,  0, 60, 60, 60, 60,  0, 60, 60,
     60, 60, 60, 60, 60, 60, 10, 10, 10, 10,  0, 10,  0, 10, 10, 10,
     10, 10, 10, 10,  0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    /* Block 174 */
     10, 10, 10, 10, 60, 60,  0, 60, 60, 60, 60,  0,  0, 60, 60, 60,
     60, 60, 60, 60, 60,  0, 60, 60, 60, 60, 60, 60, 60,  0, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 60, 60,  0, 60, 60, 60, 60,  0,
     60, 60, 60, 60, 60,  0, 60,  0,  0,  0, 60, 60, 60, 60, 60, 60,
     60,  0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    /* Block 175 */
     60, 60, 60, 60, 60, 60, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    /* Block 176 */
     10, 10, 10, 10, 10, 10, 10, 10, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    /* Block 177 */
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10,  0,  0, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60,  7, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  7, 10, 10, 10, 10,
     10, 10, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  7, 10, 10, 10, 10,
    /* Block 178 */
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10,  7, 10, 10, 10, 10, 10, 10, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60,  7, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  7,
     10, 10, 10, 10, 10, 10, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  7,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    /* Block 179 */
     10, 10, 10, 10, 10, 10, 10, 10, 10,  7, 10, 10, 10, 10, 10, 10,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 60, 60, 60, 60, 60, 60,  7, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10,  7, 10, 10, 10, 10, 10, 10, 60, 10,  0,  0,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    /* Block 180 */
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47,  7,  7,  7,  7, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  7,  7,  7,
      7,  7,  7,  7,  7, 47,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 181 */
      7,  7,  7,  7, 47,  7,  7,  6,  6,  6,  6,  6,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 47, 47, 47, 47, 47,
      0, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 182 */
     47, 47, 47, 47, 47, 47, 47,  0, 47, 47, 47, 47, 47, 47, 47, 47,
     47, 47, 47, 47, 47, 47, 47, 47, 47,  0,  0, 47, 47, 47, 47, 47,
     47, 47,  0, 47, 47,  0, 47, 47, 47, 47, 47,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 183 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     47, 47, 47, 47, 47, 47, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 184 */
    233,233,233,233,233,233,233,233,233,233,233,233,233,233,233,233,
    233,233,233,233,233,233,233,233,233,233,233,233,233,233,233,233,
    233,233, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 47, 47, 47, 47, 47, 47, 47,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0,  0,  0,  6,  6,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 185 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  7,  8,  8,  8,
      7,  8,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 186 */
     13, 13, 13, 13,  0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      0, 13, 13,  0, 13,  0,  0, 13,  0, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13,  0, 13, 13, 13, 13,  0, 13,  0, 13,  0,  0,  0,  0,
      0,  0, 13,  0,  0,  0,  0, 13,  0, 13,  0, 13,  0, 13, 13, 13,
      0, 13, 13,  0, 13,  0,  0, 13,  0, 13,  0, 13,  0, 13,  0, 13,
      0, 13, 13,  0, 13,  0,  0, 13, 13, 13, 13,  0, 13, 13, 13, 13,
     13, 13, 13,  0, 13, 13, 13, 13,  0, 13, 13, 13, 13,  0, 13,  0,
    /* Block 187 */
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,
      0, 13, 13, 13,  0, 13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /* Block 188 */
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12,234,234,234,234,234,234,234,234,234,234,234,234,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,234,
    234, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    234, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    234, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12,234,234,234,234,234,234,234,234,234,234,
    /* Block 189 */
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,234,234,234,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,234,234,234,234,
     12, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12, 12,
    /* Block 190 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12,  7,
      7, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,235,235,235,235,235,235,235,235,235,235,
    235,235,235,235,235,235,235,235,235,235,235,235,235,235,235,235,
    /* Block 191 */
      7, 12, 12,234,234,234,234,234,234,234,234,234,234,234,234,234,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 12,
      7,  7, 12, 12, 12, 12, 12, 12, 12, 12, 12,  7,234,234,234,234,
      7,  7,  7,  7,  7,  7,  7,  7,  7,234,234,234,234,234,234,234,
     12, 12,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
     12, 12, 12, 12, 12, 12,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    /* Block 192 */
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12,234,234,234,234,234,234,234,234,234,234,234,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,234,234,234,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12,234,234,234,234,234,234,
    /* Block 193 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7, 12, 12, 12, 12,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    /* Block 194 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,234,234,234,234,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,234,234,234,234,234,234,234,234,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,234,234,234,234,234,234,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    /* Block 195 */
      7,  7,  7,  7,  7,  7,  7,  7,234,234,234,234,234,234,234,234,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
    /* Block 196 */
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,234,234,234,234,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  7, 12, 12, 12,234,
     12, 12, 12, 12, 12, 12,  7, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12,234,234, 12, 12, 12, 12,234,234,234, 12,234, 12, 12, 12, 12,
    /* Block 197 */
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12,234,234,234,234,234,234,234,234,234,234,234,234,234,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12,234,234,234,234,234,234,
     12, 12, 12,234,234,234,234,234,234,234,234,234,234,234,234,234,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
};

/*
Grapheme cluster break transition table.
The grapheme cluster break rules are embedded in a 16x16 state machine
transition table, denoting whether we can break when going from one grapheme
break type to another. The values are made up of a MUST_BREAK bit (highest bit)
and the state to which the state machine transitions to in case no break is
allowed. The state machine will consume codepoints, until a grapheme cluster
break is found.
See https://unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
for more information.
*/
/* Unicode grapheme cluster break transition table: 256 bytes. */
static const size_t tmu_grapheme_break_transitions_size = 256;
static const uint8_t tmu_grapheme_break_transitions[256] = {
      0,  1,  2,  3,  4,133,  6,135,  8,  9, 10, 11, 12,141, 14, 15, 
      0,  1,130,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
    128,  1,  2,  3,132,133,134,135,136,137,138,139,140,141,142, 15, 
      0,  1,  2,  3,  4,133,  6,135,  8,  9, 10, 11, 12,141, 14, 15, 
      0,  1,  2,  3,  4,133,128,135,  8,  9, 10, 11, 12,141, 14, 15, 
      0,  1,  2,  3,  4,133,  6,135,  8,  9, 10, 11, 12,141, 14, 15, 
      0,  1,  2,  3,  4,133,  6,135,136,137, 10,139,140,141, 14, 15, 
      0,  1,  2,  3,  4,133,  6,135,  8,137,138, 11, 12,141, 14, 15, 
      0,  1,  2,  3,  4,133,  6,135,  8,  9,138, 11, 12,141, 14, 15, 
      0,  1,  2,  3,  4,133,  6,135,  8,137,138, 11, 12,141, 14, 15, 
      0,  1,  2,  3,  4,133,  6,135,  8,  9,138, 11, 12,141, 14, 15, 
      0,  1,  2,  3,  4,133,  6,135,  8,  9, 10, 11, 12,141, 14, 15, 
      0,  1,  2,  3,  4,142,  6,135,  8,  9, 10, 11, 12,143, 14, 15, 
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13,142, 15
};

static uint8_t tmu_get_stage_one_value_internal(uint32_t index) {
    if (index >= 8704) return 198;
    if (index < 1025) return tmu_ucd_stage_one[index];
    switch (index) {
        case 1357: return 239;
        case 1358: return 208;
        case 1390: return 240;
        case 1392: return 241;
        case 1437: return 242;
        case 1495: return 243;
        case 1520: return 202;
        case 1521: return 202;
        case 1522: return 202;
        case 1523: return 202;
        case 1524: return 244;
        case 7168: return 245;
        case 7169: return 210;
        case 7170: return 246;
        case 7171: return 247;
        case 7680: return 211;
        case 8191: return 248;
        case 8192: return 211;
        case 8703: return 248;
    }
    if (index >= 7172 && index < 7200) return 210;
    return 198;
}

static uint8_t tmu_get_stage_two_value_internal(uint32_t block_index,
                uint32_t offset) {
    TM_ASSERT(block_index < tmu_ucd_stage_two_blocks_count);
    TM_ASSERT(offset < 128);
    if(block_index < 198) {
        return tmu_ucd_stage_two[block_index * 128 + offset];
    }
    switch (block_index - 198) {
        case 0: return 0;
        case 1: return (offset < 96) ? 78 : 79;
        case 2: return (offset < 40) ? 79 : 80;
        case 3: return (offset == 0) ? 6 : 13;
        case 4: return 13;
        case 5: return (offset >= 109 && offset < 111) ? 6 : 13;
        case 6: return 7;
        case 7: return (offset == 6 || offset == 19) ? 7 : 12;
        case 8: return (offset >= 6 && offset < 16) ? 7 : 12;
        case 9: return (offset >= 52 && offset < 54) ? 12 : 7;
        case 10: return (offset == 0) ? 13 : 0;
        case 11: return (offset == 111) ? 13 : 0;
        case 12: return 1;
        case 13: return (offset == 0) ? 219 : 0;
        case 14: return (offset < 127) ? 0 : 219;
        case 15: return (offset >= 110 && offset < 112) ? 0 : 13;
        case 16: return (offset < 90) ? 13 : 0;
        case 17: return (offset < 123) ? 13 : 0;
        case 18: return (offset < 73) ? 13 : 0;
        case 19: return (offset >= 96 && offset < 127) ? 8 : 0;
        case 20: return (offset < 26) ? 13 : 0;
        case 21: return (offset < 68) ? 13 : 0;
        case 22: return (offset < 47) ? 13 : 0;
        case 23: return (offset < 71) ? 13 : 0;
        case 24: return (offset < 16) ? 13 : 0;
        case 25: return (offset == 113) ? 13 : 0;
        case 26: return (offset < 115) ? 13 : 0;
        case 27: return (offset >= 31 && offset < 112) ? 0 : 13;
        case 28: return (offset < 124) ? 13 : 0;
        case 29: return (offset < 118) ? 7 : 0;
        case 30: return (offset >= 96 && offset < 116) ? 8 : 0;
        case 31: return (offset < 113) ? 0 : 8;
        case 32: return (offset >= 44 && offset < 48) ? 234 : 12;
        case 33: return 234;
        case 34: return 12;
        case 35: return (offset < 123) ? 12 : 236;
        case 36: return (offset >= 62 && offset < 70) ? 7 : 12;
        case 37: return (offset < 80) ? 12 : 7;
        case 38: return (offset < 116) ? 7 : 234;
        case 39: return (offset >= 96 && offset < 110) ? 12 : 234;
        case 40: return (offset < 126) ? 234 : 0;
        case 41: return (offset == 86) ? 13 : 0;
        case 42: return (offset == 52 || offset == 64) ? 13 : 0;
        case 43: return (offset == 29 || offset == 32) ? 13 : 0;
        case 44: return (offset == 33 || offset == 48) ? 13 : 0;
        case 45: return (offset == 96) ? 13 : 0;
        case 46: return (offset < 30) ? 13 : 0;
        case 47: return (offset < 32) ? 1 : 183;
        case 48: return 47;
        case 49: return (offset < 112) ? 47 : 1;
        case 50: return (offset == 125) ? 219 : 0;
        default: return 0;
    }
}

static const tmu_ucd_internal* tmu_get_ucd_internal(uint32_t cp) {
    TM_ASSERT(cp <= 0x10FFFF);
    uint32_t stage_one_index = cp / 128;
    uint32_t stage_two_index = cp % 128;

    uint8_t block_index = tmu_get_stage_one_value_internal(stage_one_index);
    uint8_t entry_index =
        tmu_get_stage_two_value_internal(block_index, stage_two_index);

    TM_ASSERT(entry_index < tmu_ucd_entries_size);
    return &tmu_ucd_entries[entry_index];
}

TMU_UCD_DEF tmu_ucd_entry tmu_ucd_get_entry(uint32_t codepoint) {
    const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
    tmu_ucd_entry result;
    result.category = (tmu_ucd_category_enum)((internal->bits0 >> 0) & 7);
    result.case_info = (tmu_ucd_case_info_enum)((internal->bits0 >> 3) & 3);
    result.grapheme_break =
        (tmu_ucd_grapheme_break_enum)((internal->bits1 >> 0) & 15);
    result.simple_case_fold = codepoint + internal->simple_case_fold_offset;
    result.full_case_fold = tmu_codepoint_runs + tmu_full_case_fold_offset +
                            internal->full_case_fold_index;
    return result;
}

TMU_UCD_DEF tmu_ucd_category_enum tmu_ucd_get_category(uint32_t codepoint) {
    const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
    return (tmu_ucd_category_enum)((internal->bits0 >> 0) & 7);
}

TMU_UCD_DEF int tmu_ucd_is_whitespace(uint32_t codepoint) {
    const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
    return (internal->bits0 & 64) != 0;
}

TMU_UCD_DEF tmu_ucd_case_info_enum tmu_ucd_get_case_info(uint32_t codepoint) {
    const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
    return (tmu_ucd_case_info_enum)((internal->bits0 >> 3) & 3);
}

#ifdef __cplusplus
}
#endif

#undef TMU_UCD_DEF
#endif /* !defined(TMU_NO_UCD) */

/* clang-format off */
#if defined(TMU_USE_CRT) && (defined(TMU_PLATFORM_UNIX) || !defined(TMU_USE_WINDOWS_H))
	#if !defined(TMU_TESTING)
	    #include <string.h>
	    #include <stdlib.h>
	#endif /* !defined(TMU_TESTING) */

	/* Use malloc if provided, otherwise fall back to heap. */
	#ifdef TM_REALLOC
	    #define TMU_MALLOC TM_MALLOC
	    #define TMU_REALLOC TM_REALLOC
	    #define TMU_FREE TM_FREE
	#else
	    #define TMU_MALLOC(size, alignment) malloc((size))
	    #define TMU_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) realloc((ptr), (new_size))
	    #define TMU_FREE(ptr, size, alignment) free((ptr))
	#endif

	#ifdef TM_MEMMOVE
	    #define TMU_MEMMOVE TM_MEMMOVE
	#else
	    #define TMU_MEMMOVE memmove
	#endif

	#ifdef TM_MEMCPY
	    #define TMU_MEMCPY TM_MEMCPY
	#else
	    #define TMU_MEMCPY memcpy
	#endif

	#ifdef TM_WCSCHR
	    #define TMU_STRCHRW TM_WCSCHR
	#else
	    #define TMU_STRCHRW wcschr
	#endif
	/* clang-format on */

	#define TMU_STRLEN strlen
	#define TMU_WCSLEN wcslen
	#define TMU_MEMCHR memchr
	#define TMU_MEMCMP memcmp

#elif defined(_WIN32) && defined(TMU_USE_WINDOWS_H)

	/* Use malloc if provided, otherwise fall back to process heap. */
	#ifdef TM_REALLOC
	    #define TMU_MALLOC TM_MALLOC
	    #define TMU_REALLOC TM_REALLOC
	    #define TMU_FREE TM_FREE
	#else
	    #define TMU_MALLOC(size, alignment) HeapAlloc(GetProcessHeap(), 0, (size))
	    #define TMU_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) \
	        HeapReAlloc(GetProcessHeap(), 0, (ptr), (new_size))
	    #define TMU_FREE(ptr, size, alignment) HeapFree(GetProcessHeap(), 0, (ptr))
	#endif

	#ifdef TM_MEMMOVE
	    #define TMU_MEMMOVE TM_MEMMOVE
	#else
	    #define TMU_MEMMOVE MoveMemory
	#endif

	#ifdef TM_MEMCPY
	    #define TMU_MEMCPY TM_MEMCPY
	#else
	    #define TMU_MEMCPY CopyMemory
	#endif

	#if defined(TM_MEMCMP)
	    #define TMU_MEMCMP TM_MEMCMP
	#elif defined(TMU_USE_CRT)
	    #ifndef TMU_TESTING
	        #include <string.h>
	    #endif
	    #define TMU_MEMCMP memcmp
	#else
	    /* There is no memcmp, implement a simple one here. */
	    static int tmu_memcmp(const void* first, const void* second, size_t size) {
	        const char* a = (const char*)first;
	        const char* b = (const char*)second;

	        while (size) {
	            int diff = (int)*a++ - (int)*b++;
	            if (diff != 0) return (diff < 0) ? -1 : 1;
	            --size;
	        }
	        return 0;
	    }
	    #define TMU_MEMCMP tmu_memcmp
	#endif

	#ifndef TMU_TEXT
		#define TMU_TEXT(x) L##x
	#endif
	#define TMU_TEXTLEN TMU_WCSLEN
	#define TMU_TEXTCHR TMU_STRCHRW
	#define TMU_DIR_DELIM L'\\'
	#define TMU_STRLEN(x) ((tm_size_t)lstrlenA((x)))

	/* String functions, use shlwapi if provided, otherwise use fallback version. */
	#ifdef TMU_USE_SHLWAPI_H
		#define TMU_STRCHRW StrChrW
	#elif defined(TM_WCSCHR)
		#define TMU_STRCHRW TM_WCSCHR
	#else
		static WCHAR* tmu_strchrw(WCHAR* str, WCHAR c) {
		    TM_ASSERT(str);
		    while (*str && *str != c) ++str;
		    if (!*str) return TM_NULL;
		    return str;
		}
		#define TMU_STRCHRW tmu_strchrw
	#endif

	#define TMU_WCSLEN(str) lstrlenW((str))
#endif /* defined(TMU_USE_WINDOWS_H) */
/* clang-format on */
#define TMU_WIDEN(x) ((uint32_t)((uint8_t)(x)))
#define TMU_MAX_UTF32 0x10FFFFu
#define TMU_LEAD_SURROGATE_MIN 0xD800u
#define TMU_LEAD_SURROGATE_MAX 0xDBFFu
#define TMU_TRAILING_SURROGATE_MIN 0xDC00u
#define TMU_TRAILING_SURROGATE_MAX 0xDFFFu
#define TMU_SURROGATE_OFFSET (0x10000u - (0xD800u << 10u) - 0xDC00u)
#define TMU_INVALID_CODEPOINT 0xFFFFFFFFu

/* Byte order marks for all encodings we can decode. */
static const unsigned char tmu_utf8_bom[3] = {0xEF, 0xBB, 0xBF};
static const unsigned char tmu_utf16_be_bom[2] = {0xFE, 0xFF};
static const unsigned char tmu_utf16_le_bom[2] = {0xFF, 0xFE};
static const unsigned char tmu_utf32_be_bom[4] = {0x00, 0x00, 0xFE, 0xFF};
static const unsigned char tmu_utf32_le_bom[4] = {0xFF, 0xFE, 0x00, 0x00};

TMU_DEF tmu_utf8_stream tmu_utf8_make_stream(const char* str) {
    TM_ASSERT(str);
    tmu_utf8_stream result = {TM_NULL, TM_NULL};
    result.cur = str;
    result.end = str + TMU_STRLEN(str);
    return result;
}
TMU_DEF tmu_utf8_stream tmu_utf8_make_stream_n(const char* str, tm_size_t len) {
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len == 0);
    tmu_utf8_stream result = {TM_NULL, TM_NULL};
    result.cur = str;
    result.end = str + len;
    return result;
}

TMU_DEF tmu_utf8_output_stream tmu_utf8_make_output_stream(char* data, tm_size_t capacity) {
    TM_ASSERT_VALID_SIZE(capacity);
    TM_ASSERT(data || capacity == 0);

    tmu_utf8_output_stream stream = {TM_NULL, 0, 0, 0, TM_OK};
    stream.data = data;
    stream.capacity = capacity;
    return stream;
}
TMU_DEF tmu_utf8_output_stream tmu_utf8_make_output_stream_n(char* data, tm_size_t capacity, tm_size_t size) {
    TM_ASSERT_VALID_SIZE(size);
    TM_ASSERT_VALID_SIZE(capacity);
    TM_ASSERT(data || capacity == 0);
    TM_ASSERT(size <= capacity);

    tmu_utf8_output_stream stream = {TM_NULL, 0, 0, 0, TM_OK};
    stream.data = data;
    stream.size = size;
    stream.capacity = capacity;
    return stream;
}

TMU_DEF tmu_utf16_stream tmu_utf16_make_stream(const tmu_char16* str) {
    TM_ASSERT(str);
    tmu_utf16_stream result = {TM_NULL, TM_NULL};
    result.cur = str;
    result.end = str + TMU_CHAR16LEN(str);
    return result;
}

TMU_DEF tmu_utf16_stream tmu_utf16_make_stream_n(const tmu_char16* str, tm_size_t len) {
    TM_ASSERT(str || len == 0);
    tmu_utf16_stream result = {TM_NULL, TM_NULL};
    result.cur = str;
    result.end = str + len;
    return result;
}

TMU_DEF tm_bool tmu_is_valid_codepoint(uint32_t codepoint) {
    return codepoint <= TMU_MAX_UTF32 && (codepoint < TMU_LEAD_SURROGATE_MIN || codepoint > TMU_TRAILING_SURROGATE_MAX);
}
TMU_DEF tm_bool tmu_utf8_extract(tmu_utf8_stream* stream, uint32_t* codepoint_out) {
    TM_ASSERT(stream);
    TM_ASSERT(codepoint_out);

    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    const char* cur = stream->cur;
    ptrdiff_t remaining = stream->end - cur;
    if (remaining > 0) {
        uint32_t first = (uint8_t)cur[0];
        if (first < 0x80) {
            codepoint = first;
            cur += 1;
        } else if ((first >> 5) == 0x6) { /* 110xxxxx 10xxxxxx */
            /* 2 byte sequence */
            if (remaining >= 2) {
                uint32_t second = (uint8_t)cur[1];
                codepoint = ((first & 0x1F) << 6) | (second & 0x3F);
                cur += 2;
            }
        } else if ((first >> 4) == 0xE) { /* 1110xxxx 10xxxxxx 10xxxxxx */
            /* 3 byte sequence */
            if (remaining >= 3) {
                uint32_t second = (uint8_t)cur[1];
                uint32_t third = (uint8_t)cur[2];
                codepoint = ((first & 0xF) << 12) | ((second & 0x3F) << 6) | (third & 0x3F);
                cur += 3;
            }
        } else if ((first >> 3) == 0x1E) { /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            /* 4 byte sequence */
            if (remaining >= 4) {
                uint32_t second = (uint8_t)cur[1];
                uint32_t third = (uint8_t)cur[2];
                uint32_t fourth = (uint8_t)cur[3];
                codepoint = ((first & 0x7) << 18) | ((second & 0x3F) << 12) | ((third & 0x3F) << 6) | (fourth & 0x3f);
                cur += 4;
            }
        }
    }
    *codepoint_out = codepoint;
    tm_bool result = tmu_is_valid_codepoint(codepoint);
    /* Advance stream only if we could extract a valid codepoint, otherwise stream points to invalid codepoint. */
    if (result) stream->cur = cur;
    return result;
}
TMU_DEF tm_bool tmu_utf16_extract(tmu_utf16_stream* stream, uint32_t* codepoint_out) {
    TM_ASSERT(codepoint_out);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;

    const tmu_char16* cur = stream->cur;
    tmu_char16 const* const end = stream->end;
    if (cur != end) {
        uint32_t lead = (uint16_t)*cur;
        ++cur;

        /* Check for surrogate pair. */
        if (lead >= TMU_LEAD_SURROGATE_MIN && lead <= TMU_LEAD_SURROGATE_MAX) {
            if (cur != end) {
                uint32_t trail = (uint16_t)*cur;
                ++cur;
                if (trail >= TMU_TRAILING_SURROGATE_MIN && trail <= TMU_TRAILING_SURROGATE_MAX) {
                    codepoint = (lead << 10) + trail + TMU_SURROGATE_OFFSET;
                }
            }
        } else {
            codepoint = lead;
        }
    }
    *codepoint_out = codepoint;
    tm_bool result = tmu_is_valid_codepoint(codepoint);
    /* Advance stream only if we could extract a valid codepoint, otherwise stream points to invalid codepoint. */
    if (result) stream->cur = cur;
    return result;
}
TMU_DEF tm_size_t tmu_utf8_encode(uint32_t codepoint, char* out, tm_size_t out_len) {
    TM_ASSERT(out || out_len == 0);
    TM_ASSERT(tmu_is_valid_codepoint(codepoint));

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
TMU_DEF tm_bool tmu_utf8_append(uint32_t codepoint, tmu_utf8_output_stream* stream) {
    TM_ASSERT(stream);
    TM_ASSERT(stream->data || stream->capacity == 0);
    TM_ASSERT(stream->size <= stream->capacity);

    tm_size_t remaining = stream->capacity - stream->size;
    tm_size_t size = tmu_utf8_encode(codepoint, stream->data + stream->size, stream->capacity - stream->size);
    stream->necessary += size;
    if (size > remaining) {
        stream->ec = TM_ERANGE;
        stream->size = stream->capacity;
    } else {
        stream->size += size;
    }
    return stream->ec == TM_OK;
}

TMU_DEF tm_size_t tmu_utf16_encode(uint32_t codepoint, tmu_char16* out, tm_size_t out_len) {
    TM_ASSERT(out || out_len == 0);
    TM_ASSERT(tmu_is_valid_codepoint(codepoint));
    if (codepoint <= 0xFFFFu) {
        if (out_len < 1) return 1;
        out[0] = (tmu_char16)codepoint;
        return 1;
    } else {
        if (out_len < 2) return 2;
        codepoint -= 0x10000u;
        out[0] = (tmu_char16)(TMU_LEAD_SURROGATE_MIN + (uint16_t)(codepoint >> 10u));
        out[1] = (tmu_char16)(TMU_TRAILING_SURROGATE_MIN + (uint16_t)(codepoint & 0x3FFu));
        return 2;
    }
}
TMU_DEF tm_bool tmu_utf16_append(uint32_t codepoint, tmu_utf16_output_stream* stream) {
    TM_ASSERT(stream);
    TM_ASSERT(stream->data || stream->capacity == 0);
    TM_ASSERT(stream->size <= stream->capacity);

    tm_size_t remaining = stream->capacity - stream->size;
    tm_size_t size = tmu_utf16_encode(codepoint, stream->data + stream->size, stream->capacity - stream->size);
    stream->necessary += size;
    if (size > remaining) {
        stream->ec = TM_ERANGE;
        stream->size = stream->capacity;
    } else {
        stream->size += size;
    }
    return stream->ec == TM_OK;
}

typedef struct {
    const char* cur;
    const char* end;
} tmu_byte_stream;

static uint16_t tmu_extract_u16_be(tmu_byte_stream* stream) {
    TM_ASSERT(stream->end - stream->cur >= (ptrdiff_t)sizeof(uint16_t));
    const char* cur = stream->cur;
    uint16_t result = (uint16_t)(TMU_WIDEN(cur[1]) << 0) | (TMU_WIDEN(cur[0]) << 8);
    stream->cur += sizeof(uint16_t);
    return result;
}
static uint16_t tmu_extract_u16_le(tmu_byte_stream* stream) {
    TM_ASSERT(stream->end - stream->cur >= (ptrdiff_t)sizeof(uint16_t));
    const char* cur = stream->cur;
    uint16_t result = (uint16_t)(TMU_WIDEN(cur[0]) << 0) | (TMU_WIDEN(cur[1]) << 8);
    stream->cur += sizeof(uint16_t);
    return result;
}

static uint32_t tmu_extract_u32_be(tmu_byte_stream* stream) {
    TM_ASSERT(stream->end - stream->cur >= (ptrdiff_t)sizeof(uint32_t));
    const char* cur = stream->cur;
    uint32_t result =
        (TMU_WIDEN(cur[3]) << 0) | (TMU_WIDEN(cur[2]) << 8) | (TMU_WIDEN(cur[1]) << 16) | (TMU_WIDEN(cur[0]) << 24);
    stream->cur += sizeof(uint32_t);
    return result;
}
static uint32_t tmu_extract_u32_le(tmu_byte_stream* stream) {
    TM_ASSERT(stream->end - stream->cur >= (ptrdiff_t)sizeof(uint32_t));
    const char* cur = stream->cur;
    uint32_t result =
        (TMU_WIDEN(cur[0]) << 0) | (TMU_WIDEN(cur[1]) << 8) | (TMU_WIDEN(cur[2]) << 16) | (TMU_WIDEN(cur[3]) << 24);
    stream->cur += sizeof(uint32_t);
    return result;
}

typedef struct {
    tmu_conversion_result conversion;
    char* data;
    tm_size_t necessary;
    tm_size_t capacity;
    tm_bool can_grow;
    tm_bool owns;
} tmu_conversion_output_stream;

static tmu_conversion_output_stream tmu_make_conversion_output_stream(char* buffer, tm_size_t buffer_len,
                                                                      tm_bool can_grow) {
    tmu_conversion_output_stream result;
    result.data = buffer;
    result.conversion.size = 0;
    result.necessary = 0;
    result.capacity = buffer_len;
    result.can_grow = can_grow;
    result.owns = TM_FALSE;
    result.conversion.ec = TM_OK;
    result.conversion.original_encoding = tmu_encoding_unknown;
    result.conversion.invalid_codepoints_encountered = TM_FALSE;
    return result;
}

static tm_bool tmu_alloc_output_stream(tmu_conversion_output_stream* out, tm_size_t capacity) {
    if (out->can_grow && out->capacity < capacity) {
        out->data = (char*)TMU_MALLOC(capacity * sizeof(char), sizeof(char));
        if (!out->data) {
            out->conversion.ec = TM_ENOMEM;
            return TM_FALSE;
        }
        out->conversion.size = 0;
        out->capacity = capacity;
        out->owns = TM_TRUE;
    }
    return TM_TRUE;
}

static void tmu_destroy_output(tmu_conversion_output_stream* stream) {
    if (stream) {
        if (stream->data && stream->owns) {
            TMU_FREE(stream->data, stream->capacity * sizeof(char), sizeof(char));
        }
        stream->data = TM_NULL;
        stream->conversion.size = 0;
        stream->capacity = 0;
        stream->owns = TM_FALSE;
    }
}

static tm_bool tmu_output_grow(tmu_conversion_output_stream* stream, tm_size_t by_at_least) {
    TM_ASSERT(stream->can_grow);
    TM_ASSERT(stream->conversion.size + by_at_least > stream->capacity);
    TM_ASSERT(stream->conversion.ec == TM_OK);

    tm_size_t new_capacity = stream->capacity + (stream->capacity / 2);
    if (new_capacity < stream->capacity + by_at_least) new_capacity = stream->capacity + by_at_least;
    char* new_data = TM_NULL;
    if (stream->data && stream->owns) {
        TM_ASSERT(stream->capacity > 0);
        new_data = (char*)TMU_REALLOC(stream->data, stream->capacity * sizeof(char), sizeof(char),
                                      new_capacity * sizeof(char), sizeof(char));
    } else {
        new_data = (char*)TMU_MALLOC(new_capacity * sizeof(char), sizeof(char));
        if (new_data && !stream->owns && stream->data && stream->conversion.size) {
            TMU_MEMCPY(new_data, stream->data, stream->conversion.size * sizeof(char));
        }
    }
    if (!new_data) {
        stream->conversion.ec = TM_ENOMEM;
        tmu_destroy_output(stream);
        return TM_FALSE;
    }
    stream->data = new_data;
    stream->capacity = new_capacity;
    stream->owns = TM_TRUE;
    return TM_TRUE;
}

static tm_bool tmu_output_append_codepoint(tmu_conversion_output_stream* out, uint32_t codepoint) {
    tm_size_t remaining = out->capacity - out->conversion.size;
    tm_size_t write_size = tmu_utf8_encode(codepoint, out->data + out->conversion.size, remaining);
    out->necessary += write_size;
    if (out->conversion.ec == TM_OK) {
        if (write_size > remaining) {
            /* If output stream can't grow, out of memory is not an error.
               We accumulate how much memory is necessary in that case. */
            if (!out->can_grow) {
                out->conversion.ec = TM_ERANGE;
                return TM_TRUE;
            }

            if (!tmu_output_grow(out, write_size)) return TM_FALSE;

            remaining = out->capacity - out->conversion.size;
            write_size = tmu_utf8_encode(codepoint, out->data + out->conversion.size, remaining);
            TM_ASSERT(write_size <= remaining);
        }
        out->conversion.size += write_size;
    }
    return TM_TRUE;
}

static tm_bool tmu_output_append_str(tmu_conversion_output_stream* out, const char* str, tm_size_t len) {
    TM_ASSERT(str && len > 0);

    tm_size_t remaining = out->capacity - out->conversion.size;
    out->necessary += len;
    if (out->conversion.ec == TM_OK) {
        if (remaining < len) {
            /* If output stream can't grow, out of memory is not an error.
               We accumulate how much memory is necessary in that case. */
            if (!out->can_grow) {
                out->conversion.ec = TM_ERANGE;
                return TM_TRUE;
            }

            if (!tmu_output_grow(out, len)) return TM_FALSE;
        }
        TM_ASSERT(len <= out->capacity - out->conversion.size);
        TMU_MEMCPY(out->data + out->conversion.size, str, len * sizeof(char));
        out->conversion.size += len;
    }
    return TM_TRUE;
}

static tm_bool tmu_output_replace_pos(tmu_conversion_output_stream* stream, tm_size_t start, tm_size_t end,
                                      const char* str, tm_size_t str_len) {
    tm_size_t size = stream->conversion.size;

    TM_ASSERT(stream);
    TM_ASSERT(str);
    TM_ASSERT(str_len > 0);
    TM_ASSERT(start <= size);
    TM_ASSERT(end <= size);
    TM_ASSERT_VALID_SIZE(start);
    TM_ASSERT(start <= end);

    if (start == size) return tmu_output_append_str(stream, str, str_len);

    tm_size_t len = start - end;
    ptrdiff_t diff = (ptrdiff_t)str_len - (ptrdiff_t)len;
    if (diff == 0) {
        /* Replacement string is as long as range to replace, just overwrite in that case. */
        TMU_MEMCPY(stream->data + start, str, str_len * sizeof(char));
        return TM_TRUE;
    }
    tm_size_t offset = 0;
    if (diff < 0) {
        /* Range to replace is longer than replacement string, we shrink by replacing. */
        offset = start + str_len;
    } else {
        /* Range to replace is shorter than replacement string, we grow by replacing. */
        tm_size_t grow_amount = (tm_size_t)diff;
        if (stream->capacity - size < grow_amount) {
            if (!stream->can_grow) return TM_FALSE;
            if (!tmu_output_grow(stream, grow_amount)) return TM_FALSE;
        }

        offset = end + grow_amount;
    }
    /* Make room for replacement. */
    if (end != size) {
        TMU_MEMMOVE(stream->data + offset, stream->data + end, (size - offset) * sizeof(char));
    }
    /* Copy replacement */
    TMU_MEMCPY(stream->data + start, str, str_len * sizeof(char));

    /* Adjust size of stream (growing or shrinking). */
    TM_ASSERT((ptrdiff_t)size + diff >= 0);
    stream->conversion.size = (tm_size_t)((ptrdiff_t)size + diff);
    return TM_TRUE;
}

static void tmu_convert_bytes_from_utf16(tmu_byte_stream bytes, uint16_t (*extract)(tmu_byte_stream*),
                                         tmu_validate validate, const char* replace_str, tm_size_t replace_str_len,
                                         tm_bool nullterminate, tmu_conversion_output_stream* out) {
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));
    /* Guessing how many utf8 octets we will need. */
    tm_size_t total_bytes_count = (tm_size_t)(bytes.end - bytes.cur);
    if (!tmu_alloc_output_stream(out, total_bytes_count + (total_bytes_count / 2))) {
        return;
    }

    while (bytes.end - bytes.cur >= (ptrdiff_t)sizeof(tmu_char16)) {
        uint32_t codepoint = TMU_INVALID_CODEPOINT;

        uint32_t lead = extract(&bytes);

        /* Check for surrogate pair. */
        if (lead >= TMU_LEAD_SURROGATE_MIN && lead <= TMU_LEAD_SURROGATE_MAX) {
            if (bytes.end - bytes.cur >= (ptrdiff_t)sizeof(tmu_char16)) {
                uint32_t trail = extract(&bytes);
                if (trail >= TMU_TRAILING_SURROGATE_MIN && trail <= TMU_TRAILING_SURROGATE_MAX) {
                    codepoint = (lead << 10) + trail + TMU_SURROGATE_OFFSET;
                }
            }
        } else {
            codepoint = lead;
        }

        if (!tmu_is_valid_codepoint(codepoint)) {
            out->conversion.invalid_codepoints_encountered = TM_TRUE;
            switch (validate) {
                case tmu_validate_skip: {
                    continue;
                }
                case tmu_validate_error: {
                    out->conversion.ec = TM_EINVAL;
                    tmu_destroy_output(out);
                    return;
                }
                case tmu_validate_replace: {
                    if (!tmu_output_append_str(out, replace_str, replace_str_len)) return;
                    continue;
                }
            }
            return;
        }

        if (!tmu_output_append_codepoint(out, codepoint)) return;
    }

    if (bytes.end - bytes.cur > 0 && out->conversion.ec == TM_OK) {
        /* There are remaining bytes in the byte stream that we couldn't convert. */
        out->conversion.ec = TM_EINVAL;
        tmu_destroy_output(out);
        return;
    }
    if (nullterminate && (out->conversion.ec == TM_OK || out->conversion.ec == TM_ERANGE)) {
        if (tmu_output_append_codepoint(out, 0)) {
            /* Don't count null-terminator towards returned size. */
            --out->conversion.size;
        } else {
            return;
        }
    }
}

static void tmu_convert_bytes_from_utf32(tmu_byte_stream bytes, uint32_t (*extract)(tmu_byte_stream*),
                                         tmu_validate validate, const char* replace_str, tm_size_t replace_str_len,
                                         tm_bool nullterminate, tmu_conversion_output_stream* out) {
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));
    /* Guessing how many utf8 octets we will need. */
    tm_size_t total_bytes_count = (tm_size_t)(bytes.end - bytes.cur);
    if (!tmu_alloc_output_stream(out, total_bytes_count + (total_bytes_count / 2))) {
        return;
    }

    while (bytes.end - bytes.cur >= (ptrdiff_t)sizeof(uint32_t)) {
        uint32_t codepoint = extract(&bytes);
        if (!tmu_is_valid_codepoint(codepoint)) {
            out->conversion.invalid_codepoints_encountered = TM_TRUE;
            switch (validate) {
                case tmu_validate_skip: {
                    continue;
                }
                case tmu_validate_error: {
                    out->conversion.ec = TM_EINVAL;
                    tmu_destroy_output(out);
                    return;
                }
                case tmu_validate_replace: {
                    if (!tmu_output_append_str(out, replace_str, replace_str_len)) return;
                    continue;
                }
            }
            return;
        }

        if (!tmu_output_append_codepoint(out, codepoint)) return;
    }

    if (bytes.end - bytes.cur > 0 && out->conversion.ec == TM_OK) {
        /* There are remaining bytes in the byte stream that we couldn't convert. */
        out->conversion.ec = TM_EINVAL;
        tmu_destroy_output(out);
        return;
    }
    if (nullterminate && (out->conversion.ec == TM_OK || out->conversion.ec == TM_ERANGE)) {
        if (tmu_output_append_codepoint(out, 0)) {
            /* Don't count null-terminator towards returned size. */
            --out->conversion.size;
        } else {
            return;
        }
    }
}

static tm_bool tmu_has_utf8_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 3 && (unsigned char)stream.cur[0] == tmu_utf8_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf8_bom[1] && (unsigned char)stream.cur[2] == tmu_utf8_bom[2];
}
static tm_bool tmu_has_utf32_be_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 4 && (unsigned char)stream.cur[0] == tmu_utf32_be_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf32_be_bom[1] && (unsigned char)stream.cur[2] == tmu_utf32_be_bom[2] &&
           (unsigned char)stream.cur[3] == tmu_utf32_be_bom[3];
}
static tm_bool tmu_has_utf32_le_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 4 && (unsigned char)stream.cur[0] == tmu_utf32_le_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf32_le_bom[1] && (unsigned char)stream.cur[2] == tmu_utf32_le_bom[2] &&
           (unsigned char)stream.cur[3] == tmu_utf32_le_bom[3];
}
static tm_bool tmu_has_utf16_be_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 2 && (unsigned char)stream.cur[0] == tmu_utf16_be_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf16_be_bom[1];
}
static tm_bool tmu_has_utf16_le_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 2 && (unsigned char)stream.cur[0] == tmu_utf16_le_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf16_le_bom[1];
}

static void tmu_output_replace_invalid_utf8(tmu_conversion_output_stream* stream, const char* replace_str,
                                            tm_size_t replace_str_len) {
    tm_size_t cur = 0;
    tm_size_t remaining = stream->conversion.size;
    tm_bool invalid_codepoints_encountered = TM_FALSE;
    while (remaining) {
        tm_size_t range = tmu_utf8_valid_range(stream->data + cur, remaining);

        if (range != remaining) {
            invalid_codepoints_encountered = TM_TRUE;
            if (!tmu_output_replace_pos(stream, cur + range, cur + range + 1, replace_str, replace_str_len)) {
                stream->conversion.ec = TM_ENOMEM;
                tmu_destroy_output(stream);
                break;
            }
            cur += replace_str_len;
        }
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    stream->conversion.invalid_codepoints_encountered = invalid_codepoints_encountered;
}

static void tmu_output_validate_inplace(tmu_conversion_output_stream* stream, tmu_validate validate,
                                        const char* replace_str, tm_size_t replace_str_len) {
    switch (validate) {
        case tmu_validate_skip: {
            tm_size_t new_size = tmu_utf8_skip_invalid(stream->data, stream->conversion.size);
            stream->conversion.invalid_codepoints_encountered = (new_size != stream->conversion.size);
            stream->conversion.size = new_size;
            break;
        }
        case tmu_validate_replace: {
            tmu_output_replace_invalid_utf8(stream, replace_str, replace_str_len);
            break;
        }
        case tmu_validate_error:
        default: {
            tm_size_t valid_range = tmu_utf8_valid_range(stream->data, stream->conversion.size);
            if (valid_range != stream->conversion.size) {
                stream->conversion.invalid_codepoints_encountered = TM_TRUE;
                stream->conversion.ec = TM_EINVAL;
            }
            stream->conversion.size = valid_range;
            break;
        }
    }
}

static tm_bool tmu_convert_bytes_to_utf8_internal(const void* input, tm_size_t input_len, tmu_encoding encoding,
                                                  tmu_validate validate, const char* replace_str,
                                                  tm_size_t replace_str_len, tm_bool nullterminate,
                                                  tmu_conversion_output_stream* out_stream) {
    TM_ASSERT(input || input_len == 0);
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));

    tmu_byte_stream bytes = {TM_NULL, TM_NULL};
    bytes.cur = (const char*)input;
    bytes.end = (const char*)input + input_len;

    if (bytes.cur == bytes.end) {
        if (nullterminate) {
            if (tmu_output_append_codepoint(out_stream, 0)) {
                /* Don't count null-terminator towards returned size. */
                --out_stream->conversion.size;
                return TM_TRUE;
            }

            out_stream->conversion.ec = TM_ERANGE;
            out_stream->necessary = 1;
            return TM_FALSE;
        }
        return TM_TRUE;
    }

    if (encoding == tmu_encoding_unknown) {
        tm_bool converted = TM_TRUE;

        /* Try to detect encoding by inspecting byte order mark. */
        if (tmu_has_utf8_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf8_bom;
            converted = TM_FALSE;
        } else if (tmu_has_utf32_be_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf32be_bom;

            bytes.cur += 4;
            tmu_convert_bytes_from_utf32(bytes, tmu_extract_u32_be, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
        } else if (tmu_has_utf32_le_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf32le_bom;

            bytes.cur += 4;
            tmu_convert_bytes_from_utf32(bytes, tmu_extract_u32_le, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
        } else if (tmu_has_utf16_be_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf16be_bom;

            bytes.cur += 2;
            tmu_convert_bytes_from_utf16(bytes, tmu_extract_u16_be, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
        } else if (tmu_has_utf16_le_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf16le_bom;

            bytes.cur += 2;
            tmu_convert_bytes_from_utf16(bytes, tmu_extract_u16_le, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
        } else {
            /* No encoding detected, assume utf8. */
            out_stream->conversion.original_encoding = tmu_encoding_utf8;
            converted = TM_FALSE;
        }

        return converted;
    }

    switch (encoding) {
        case tmu_encoding_utf8:
        case tmu_encoding_utf8_bom: {
            if (tmu_has_utf8_bom(bytes)) {
                out_stream->conversion.original_encoding = tmu_encoding_utf8_bom;
            } else if (encoding == tmu_encoding_utf8_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            } else {
                out_stream->conversion.original_encoding = tmu_encoding_utf8;
            }
            return TM_FALSE;
        }
        case tmu_encoding_utf32be:
        case tmu_encoding_utf32be_bom: {
            tmu_encoding original_encoding = tmu_encoding_utf32be;
            if (tmu_has_utf32_be_bom(bytes)) {
                /* Skip byte order mark. */
                bytes.cur += 4;
                original_encoding = tmu_encoding_utf32be_bom;
            } else if (encoding == tmu_encoding_utf32be_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            }
            tmu_convert_bytes_from_utf32(bytes, tmu_extract_u32_be, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
            out_stream->conversion.original_encoding = original_encoding;
            return out_stream->conversion.ec == TM_OK;
        }
        case tmu_encoding_utf32le:
        case tmu_encoding_utf32le_bom: {
            tmu_encoding original_encoding = tmu_encoding_utf32le;
            if (tmu_has_utf32_le_bom(bytes)) {
                /* Skip byte order mark. */
                bytes.cur += 4;
                original_encoding = tmu_encoding_utf32le_bom;
            } else if (encoding == tmu_encoding_utf32le_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            }
            tmu_convert_bytes_from_utf32(bytes, tmu_extract_u32_le, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
            out_stream->conversion.original_encoding = original_encoding;
            return out_stream->conversion.ec == TM_OK;
        }
        case tmu_encoding_utf16be:
        case tmu_encoding_utf16be_bom: {
            tmu_encoding original_encoding = tmu_encoding_utf16be;
            if (tmu_has_utf16_be_bom(bytes)) {
                /* Skip byte order mark. */
                bytes.cur += 2;
                original_encoding = tmu_encoding_utf16be_bom;
            } else if (encoding == tmu_encoding_utf16be_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            }
            tmu_convert_bytes_from_utf16(bytes, tmu_extract_u16_be, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
            out_stream->conversion.original_encoding = original_encoding;
            return out_stream->conversion.ec == TM_OK;
        }
        case tmu_encoding_utf16le:
        case tmu_encoding_utf16le_bom: {
            tmu_encoding original_encoding = tmu_encoding_utf16le;
            if (tmu_has_utf16_le_bom(bytes)) {
                /* Skip byte order mark. */
                bytes.cur += 2;
                original_encoding = tmu_encoding_utf16le_bom;
            } else if (encoding == tmu_encoding_utf16le_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            }
            tmu_convert_bytes_from_utf16(bytes, tmu_extract_u16_le, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
            out_stream->conversion.original_encoding = original_encoding;
            return out_stream->conversion.ec == TM_OK;
        }
        default: {
            TM_ASSERT(0 && "Invalid encoding.");
            out_stream->conversion.ec = TM_EINVAL;
            return TM_FALSE;
        }
    }
}

TMU_DEF tmu_utf8_conversion_result tmu_utf8_convert_from_bytes_dynamic(tmu_contents* input, tmu_encoding encoding,
                                                                       tmu_validate validate, const char* replace_str,
                                                                       tm_size_t replace_str_len,
                                                                       tm_bool nullterminate) {
    TM_ASSERT(input);
    TM_ASSERT(input->size <= input->capacity);

    tmu_conversion_output_stream out_stream =
        tmu_make_conversion_output_stream(/*buffer=*/TM_NULL, /*buffer_len=*/0, /*can_grow=*/TM_TRUE);

    tm_bool converted = tmu_convert_bytes_to_utf8_internal(input->data, input->size, encoding, validate, replace_str,
                                                           replace_str_len, nullterminate, &out_stream);

    tmu_utf8_conversion_result result;
    result.contents.data = TM_NULL;
    result.contents.size = 0;
    result.contents.capacity = 0;

    if (out_stream.conversion.ec == TM_OK) {
        if (!converted) {
            /* Take ownership of input, since it already is in the encoding we want. */
            TM_ASSERT(out_stream.conversion.original_encoding == tmu_encoding_utf8 ||
                      out_stream.conversion.original_encoding == tmu_encoding_utf8_bom);
            TM_ASSERT(out_stream.data == TM_NULL);
            TM_ASSERT(out_stream.conversion.size == 0);
            TM_ASSERT(out_stream.capacity == 0);

            out_stream.data = input->data;
            out_stream.conversion.size = input->size;
            out_stream.capacity = input->capacity;
            out_stream.owns = TM_TRUE;

            /* Remove byte order mark if it exists. */
            tmu_byte_stream stream = {TM_NULL, TM_NULL};
            stream.cur = out_stream.data;
            stream.end = out_stream.data + out_stream.conversion.size;
            if (tmu_has_utf8_bom(stream)) {
                TMU_MEMMOVE(out_stream.data, out_stream.data + 3, out_stream.conversion.size - 3);
                out_stream.conversion.size -= 3;
            }

            /* Validate inplace. */
            tmu_output_validate_inplace(&out_stream, validate, replace_str, replace_str_len);

            if (nullterminate && (out_stream.conversion.ec == TM_OK || out_stream.conversion.ec == TM_ERANGE)) {
                if (tmu_output_append_codepoint(&out_stream, 0)) {
                    /* Don't count null-terminator towards size. */
                    --out_stream.conversion.size;
                }
            }

            if (out_stream.conversion.ec == TM_OK) {
                /* Zero out input, since we took ownership of its contents. */
                input->data = TM_NULL;
                input->size = 0;
                input->capacity = 0;
            } else {
                out_stream.data = TM_NULL;
                out_stream.conversion.size = 0;
                out_stream.capacity = 0;
            }
        }

        result.contents.data = out_stream.data;
        result.contents.size = out_stream.conversion.size;
        result.contents.capacity = out_stream.capacity;
    } else {
        TM_ASSERT(out_stream.data == TM_NULL);
        TM_ASSERT(out_stream.conversion.size == 0);
        TM_ASSERT(out_stream.capacity == 0);
    }
    result.ec = out_stream.conversion.ec;
    result.original_encoding = out_stream.conversion.original_encoding;
    result.invalid_codepoints_encountered = out_stream.conversion.invalid_codepoints_encountered;
    return result;
}

TMU_DEF tmu_conversion_result tmu_utf8_convert_from_bytes(const void* input, tm_size_t input_len, tmu_encoding encoding,
                                                          tmu_validate validate, const char* replace_str,
                                                          tm_size_t replace_str_len, tm_bool nullterminate, char* out,
                                                          tm_size_t out_len) {
    TM_ASSERT(input || input_len == 0);

    tmu_conversion_output_stream out_stream = tmu_make_conversion_output_stream(out, out_len, /*can_grow=*/TM_FALSE);

    tm_bool converted = tmu_convert_bytes_to_utf8_internal(input, input_len, encoding, validate, replace_str,
                                                           replace_str_len, nullterminate, &out_stream);

    if (out_stream.conversion.ec == TM_OK && !converted && input && input_len > 0) {
        /* No conversion took place because input is already in the encoding we want. */
        TM_ASSERT(out_stream.conversion.original_encoding == tmu_encoding_utf8 ||
                  out_stream.conversion.original_encoding == tmu_encoding_utf8_bom);

        tmu_byte_stream input_bytes = {TM_NULL, TM_NULL};
        input_bytes.cur = (const char*)input;
        input_bytes.end = (const char*)input + input_len;

        /* Skip byte order mark if it exists. */
        if (tmu_has_utf8_bom(input_bytes)) input_bytes.cur += 3;

        /* Copy input while validating, since it already is in the encoding we want. */
        while (input_bytes.cur != input_bytes.end) {
            tm_size_t remaining_bytes = (tm_size_t)(input_bytes.end - input_bytes.cur);
            tm_size_t valid_range = tmu_utf8_valid_range(input_bytes.cur, remaining_bytes);
            if (valid_range > 0) {
                tmu_output_append_str(&out_stream, input_bytes.cur, valid_range);
            }
            input_bytes.cur += valid_range;
            if (valid_range != remaining_bytes) {
                out_stream.conversion.invalid_codepoints_encountered = TM_TRUE;
                switch (validate) {
                    case tmu_validate_skip: {
                        TM_ASSERT(input_bytes.cur + 1 <= input_bytes.end);
                        ++input_bytes.cur; /* Skip invalid octet. */
                        break;
                    }
                    case tmu_validate_replace: {
                        tmu_output_append_str(&out_stream, replace_str, replace_str_len);
                        ++input_bytes.cur; /* Skip invalid octet. */
                        break;
                    }
                    case tmu_validate_error:
                    default: {
                        out_stream.conversion.ec = TM_EINVAL;
                        input_bytes.cur = input_bytes.end;
                        break;
                    }
                }
            }
        }

        if (nullterminate && (out_stream.conversion.ec == TM_OK || out_stream.conversion.ec == TM_ERANGE)) {
            if (tmu_output_append_codepoint(&out_stream, 0)) {
                /* Don't count null-terminator towards size. */
                --out_stream.conversion.size;
            }
        }
    }

    /* Report necessary size if conversion failed because out wasn't big enough. */
    if (out_stream.conversion.ec == TM_ERANGE) {
        out_stream.conversion.size = out_stream.necessary;
    }
    return out_stream.conversion;
}

TMU_DEF tm_size_t tmu_utf8_valid_range(const char* str, tm_size_t len) {
    /* Checking for legal utf-8 byte sequences according to
       https://www.unicode.org/versions/Unicode11.0.0/ch03.pdf
       Table 3-7.  Well-Formed UTF-8 Byte Sequences */

    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        uint32_t c0 = (uint32_t)((uint8_t)str[i]);
        if (c0 < 0x80u) {
            /* Codepoint: 00000000 0xxxxxxx
               Utf-8:              0xxxxxxx */
            --remaining;
        } else if ((c0 & 0xE0u) == 0xC0u) {
            /* Codepoint: 00000yyy yyxxxxxx
               Utf-8:     110yyyyy 10xxxxxx
               Overlong:  1100000y 10xxxxxx */

            if (remaining < 2) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            if ((c1 & 0xC0u) != 0x80u) return i; /* Invalid trail. */
            if ((c0 & 0xFEu) == 0xC0u) return i; /* Overlong. */
            remaining -= 2;
        } else if ((c0 & 0xF0u) == 0xE0u) {
            /* Codepoint: 00000000 zzzzyyyy yyxxxxxx
               Utf-8:     1110zzzz 10yyyyyy 10xxxxxx
               Overlong:  11100000 100yyyyy 10xxxxxx
               Surrogate: 11101101 101yyyyy 10xxxxxx */
            if (remaining < 3) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            uint32_t c2 = (uint32_t)((uint8_t)str[i + 2]);
            if ((c1 & 0xC0u) != 0x80u) return i;                /* Invalid trail. */
            if ((c2 & 0xC0u) != 0x80u) return i;                /* Invalid trail. */
            if (c0 == 0xE0u && (c1 & 0xE0u) == 0x80u) return i; /* Overlong. */
            if (c0 == 0xEDu && c1 > 0x9Fu) return i;            /* Surrogate. */

            remaining -= 3;
        } else if ((c0 & 0xF8u) == 0xF0u) {
            /* Codepoint: 00000000 000uuuuu zzzzyyyy yyxxxxxx
               Utf-8:     11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
               Overlong:  11110000 1000zzzz 10yyyyyy 10xxxxxx */
            if (remaining < 4) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            uint32_t c2 = (uint32_t)((uint8_t)str[i + 2]);
            uint32_t c3 = (uint32_t)((uint8_t)str[i + 3]);
            if ((c1 & 0xC0u) != 0x80u) return i;     /* Invalid trail. */
            if ((c2 & 0xC0u) != 0x80u) return i;     /* Invalid trail. */
            if ((c3 & 0xC0u) != 0x80u) return i;     /* Invalid trail. */
            if (c0 == 0xF0u && c1 < 0x90u) return i; /* Overlong. */
            if (c0 == 0xF4u && c1 > 0x8Fu) return i; /* Invalid codepoints. */
            if (c0 > 0xF4u) return i;                /* Invalid codepoints. */

            remaining -= 4;
        } else {
            return i;
        }
    }
    return len;
}

TMU_DEF tm_size_t tmu_utf8_skip_invalid(char* str, tm_size_t len) {
    char* cur = str;
    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        tm_size_t range = tmu_utf8_valid_range(cur, remaining);

        if (cur != str + i) TMU_MEMMOVE(cur, str + i, range * sizeof(char));
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    return (tm_size_t)(cur - str);
}

TMU_DEF tm_size_t tmu_utf16_valid_range(const tmu_char16* str, tm_size_t len) {
    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        uint32_t c0 = (uint32_t)str[i];
        if (c0 >= TMU_LEAD_SURROGATE_MIN && c0 <= TMU_LEAD_SURROGATE_MAX) {
            if (remaining < 2) return i;

            uint32_t c1 = (uint32_t)str[i + 1];
            if (c1 < TMU_TRAILING_SURROGATE_MIN || c1 > TMU_TRAILING_SURROGATE_MAX) return i;

            uint32_t codepoint = (c0 << 10) + c1 + TMU_SURROGATE_OFFSET;
            if (!tmu_is_valid_codepoint(codepoint)) return i;

            remaining -= 2;
        } else {
            if (c0 >= TMU_TRAILING_SURROGATE_MIN && c0 <= TMU_TRAILING_SURROGATE_MAX) return i;
            --remaining;
        }
    }
    return len;
}

TMU_DEF tm_size_t tmu_utf16_skip_invalid(tmu_char16* str, tm_size_t len) {
    tmu_char16* cur = str;
    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        tm_size_t range = tmu_utf16_valid_range(cur, remaining);

        if (cur != str + i) TMU_MEMMOVE(cur, str + i, range * sizeof(tmu_char16));
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    return (tm_size_t)(cur - str);
}

TMU_DEF tmu_conversion_result tmu_utf8_from_utf16(tmu_utf16_stream stream, char* out, tm_size_t out_len) {
    return tmu_utf8_from_utf16_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL, /*replace_str_len=*/0,
                                  /*nullterminate=*/TM_FALSE, out, out_len);
}
TMU_DEF tmu_conversion_result tmu_utf16_from_utf8(tmu_utf8_stream stream, tmu_char16* out, tm_size_t out_len) {
    return tmu_utf16_from_utf8_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL, /*replace_str_len=*/0,
                                  /*nullterminate=*/TM_FALSE, out, out_len);
}

TMU_DEF tmu_conversion_result tmu_utf8_from_utf16_ex(tmu_utf16_stream stream, tmu_validate validate,
                                                     const char* replace_str, tm_size_t replace_str_len,
                                                     tm_bool nullterminate, char* out, tm_size_t out_len) {
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));

    tmu_conversion_result result = {0, TM_OK, tmu_encoding_unknown, TM_FALSE};
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (stream.cur != stream.end) {
        if (!tmu_utf16_extract(&stream, &codepoint)) {
            result.invalid_codepoints_encountered = TM_TRUE;
            switch (validate) {
                case tmu_validate_skip: {
                    /* Advance stream once and try again. */
                    TM_ASSERT(stream.cur + 1 <= stream.end);
                    ++stream.cur;
                    continue;
                }
                case tmu_validate_replace: {
                    result.size += replace_str_len;
                    if (out_len < replace_str_len) {
                        result.ec = TM_ERANGE;
                        out = TM_NULL;
                        out_len = 0;
                    } else {
                        TMU_MEMCPY(out, replace_str, replace_str_len * sizeof(char));
                        out += replace_str_len;
                        out_len -= replace_str_len;
                    }
                    continue;
                }
                case tmu_validate_error:
                default: {
                    result.ec = TM_EINVAL;
                    stream.cur = stream.end;
                    continue;
                }
            }
        }

        tm_size_t size = tmu_utf8_encode(codepoint, out, out_len);
        result.size += size;
        if (out_len < size) {
            result.ec = TM_ERANGE;
            out = TM_NULL;
            out_len = 0;
        } else {
            out += size;
            out_len -= size;
        }
    }

    if (nullterminate && (result.ec == TM_OK || result.ec == TM_ERANGE)) {
        tm_size_t size = tmu_utf8_encode(0, out, out_len);
        if (out_len < size) {
            result.size += size; /* Only count null-terminator towards size on overflow. */
            result.ec = TM_ERANGE;
            out = TM_NULL;
            out_len = 0;
        } else {
            out += size;
            out_len -= size;
        }
    }
    return result;
}
TMU_DEF tmu_conversion_result tmu_utf16_from_utf8_ex(tmu_utf8_stream stream, tmu_validate validate,
                                                     const tmu_char16* replace_str, tm_size_t replace_str_len,
                                                     tm_bool nullterminate, tmu_char16* out, tm_size_t out_len) {
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));

    tmu_conversion_result result = {0, TM_OK, tmu_encoding_unknown, TM_FALSE};
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (stream.cur != stream.end) {
        if (!tmu_utf8_extract(&stream, &codepoint)) {
            result.invalid_codepoints_encountered = TM_TRUE;
            switch (validate) {
                case tmu_validate_skip: {
                    /* Advance stream once and try again. */
                    TM_ASSERT(stream.cur + 1 <= stream.end);
                    ++stream.cur;
                    continue;
                }
                case tmu_validate_replace: {
                    result.size += replace_str_len;
                    if (out_len < replace_str_len) {
                        result.ec = TM_ERANGE;
                        out = TM_NULL;
                        out_len = 0;
                    } else {
                        TMU_MEMCPY(out, replace_str, replace_str_len * sizeof(tmu_char16));
                        out += replace_str_len;
                        out_len -= replace_str_len;
                    }
                    continue;
                }
                case tmu_validate_error:
                default: {
                    result.ec = TM_EINVAL;
                    stream.cur = stream.end;
                    continue;
                }
            }
        }

        tm_size_t size = tmu_utf16_encode(codepoint, out, out_len);
        result.size += size;
        if (out_len < size) {
            result.ec = TM_ERANGE;
            out = TM_NULL;
            out_len = 0;
        } else {
            out += size;
            out_len -= size;
        }
    }

    if (nullterminate && (result.ec == TM_OK || result.ec == TM_ERANGE)) {
        tm_size_t size = tmu_utf16_encode(0, out, out_len);
        if (out_len < size) {
            result.size += size; /* Only count null-terminator towards size on overflow. */
            result.ec = TM_ERANGE;
            out = TM_NULL;
            out_len = 0;
        } else {
            out += size;
            out_len -= size;
        }
    }
    return result;
}

TMU_DEF tm_size_t tmu_utf8_copy_truncated(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    TM_ASSERT(str || str_len == 0);
    TM_ASSERT(out || out_len == 0);

    if (str_len <= out_len) {
        /* There is enough room in out, no need to truncate. */
        TMU_MEMCPY(out, str, str_len * sizeof(char));
        return str_len;
    }

    if (out_len > 0) {
        /* Retreat until we find the start of a utf8 sequence. */
        if ((uint8_t)str[out_len - 1] >= 0x80u) {
            tm_size_t cur = out_len - 1;
            while (cur > 0 && ((uint8_t)str[cur] & 0xC0u) == 0x80u) {
                --cur;
            }
            /* We found the start of a uf8 sequence, test if we can extract a valid codepoint. */
            tm_size_t distance = out_len - cur;
            if (tmu_utf8_valid_range(str + cur, distance) != distance) {
                /* We couldn't extract a valid codepoint, truncate so that the utf8 sequence isn't part of out. */
                out_len = cur;
            }
        }
        TMU_MEMCPY(out, str, out_len * sizeof(char));
    }
    return out_len;
}
TMU_DEF tm_size_t tmu_utf8_copy_truncated_stream(tmu_utf8_stream stream, char* out, tm_size_t out_len) {
    TM_ASSERT(stream.cur <= stream.end);
    return tmu_utf8_copy_truncated(stream.cur, (tm_size_t)(stream.end - stream.cur), out, out_len);
}

TMU_DEF tm_bool tmu_utf8_equals(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    /* We assume that both streams are normalized,
       so both an early exit through length comparison and memcmp are valid. */
    if (a_len != b_len) return TM_FALSE;
    if (a == b) return TM_TRUE;

    return TMU_MEMCMP(a, b, a_len * sizeof(char)) == 0;
}
TMU_DEF int tmu_utf8_compare(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    /* We assume that both streams are normalized, so that a memcmp is valid. */
    if (!a_len || !b_len) return !b_len - !a_len;
    if (a == b) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    tm_size_t len = (a_len < b_len) ? a_len : b_len;
    if (len) {
        int common_cmp = TMU_MEMCMP(a, b, len * sizeof(char));
        if (common_cmp != 0) return common_cmp;
    }

    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    return 0;
}

TMU_DEF tm_size_t tmu_utf8_count_codepoints(const char* str) {
    return tmu_utf8_count_codepoints_stream(tmu_utf8_make_stream(str));
}
TMU_DEF tm_size_t tmu_utf8_count_codepoints_n(const char* str, tm_size_t str_len) {
    return tmu_utf8_count_codepoints_stream(tmu_utf8_make_stream_n(str, str_len));
}
TMU_DEF tm_size_t tmu_utf8_count_codepoints_stream(tmu_utf8_stream stream) {
    TM_ASSERT(stream.cur <= stream.end);
    tm_size_t result = 0;
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) ++result;
    return result;
}

#if defined(TMU_UCD_HAS_CASE_INFO)

#if TMU_UCD_HAS_CATEGORY
TMU_DEF tm_bool tmu_is_control(uint32_t codepoint) {
    return tmu_ucd_get_category(codepoint) == tmu_ucd_category_control;
}
TMU_DEF tm_bool tmu_is_letter(uint32_t codepoint) { return tmu_ucd_get_category(codepoint) == tmu_ucd_category_letter; }
TMU_DEF tm_bool tmu_is_mark(uint32_t codepoint) { return tmu_ucd_get_category(codepoint) == tmu_ucd_category_mark; }
TMU_DEF tm_bool tmu_is_number(uint32_t codepoint) { return tmu_ucd_get_category(codepoint) == tmu_ucd_category_number; }
TMU_DEF tm_bool tmu_is_punctuation(uint32_t codepoint) {
    return tmu_ucd_get_category(codepoint) == tmu_ucd_category_punctuation;
}
TMU_DEF tm_bool tmu_is_symbol(uint32_t codepoint) { return tmu_ucd_get_category(codepoint) == tmu_ucd_category_symbol; }
TMU_DEF tm_bool tmu_is_separator(uint32_t codepoint) {
    return tmu_ucd_get_category(codepoint) == tmu_ucd_category_separator;
}
TMU_DEF tm_bool tmu_is_whitespace(uint32_t codepoint) { return tmu_ucd_is_whitespace(codepoint) == 1; }
#endif /* TMU_UCD_HAS_CATEGORY */

#if TMU_UCD_HAS_CASE_INFO
TMU_DEF tm_bool tmu_is_upper(uint32_t codepoint) { return tmu_ucd_get_case_info(codepoint) == tmu_ucd_case_upper; }
TMU_DEF tm_bool tmu_is_lower(uint32_t codepoint) { return tmu_ucd_get_case_info(codepoint) == tmu_ucd_case_lower; }
TMU_DEF tm_bool tmu_is_title(uint32_t codepoint) { return tmu_ucd_get_case_info(codepoint) == tmu_ucd_case_title; }
TMU_DEF tm_bool tmu_is_caseless(uint32_t codepoint) {
    return tmu_ucd_get_case_info(codepoint) == tmu_ucd_case_caseless;
}
#endif /* TMU_UCD_HAS_CATEGORY */

#if TMU_UCD_HAS_WIDTH
TMU_DEF int tmu_utf8_width(tmu_utf8_stream stream) {
    TM_ASSERT(stream.cur <= stream.end);
    int result = 0;
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        result += tmu_ucd_get_width(codepoint);
    }
    return result;
}
TMU_DEF int tmu_utf8_width_n(const char* str, tm_size_t str_len) {
    TM_ASSERT(str || str_len == 0);
    tmu_utf8_stream stream = {TM_NULL, TM_NULL};
    stream.cur = str;
    stream.cur = str + str_len;
    return tmu_utf8_width(stream);
}
#endif

typedef struct {
    char* data;
    tm_size_t size;
    tm_size_t capacity;
    tmu_transform_result result;
} tmu_transform_output_stream;

static void tmu_transform_output_append_codepoint(uint32_t codepoint, tmu_transform_output_stream* out) {
    tm_size_t out_size = out->size;
    tm_size_t remaining = out->capacity - out_size;
    tm_size_t size = tmu_utf8_encode(codepoint, out->data + out_size, remaining);
    out->result.size += size;
    if (size > remaining) {
        out->result.ec = TM_ERANGE;
        out->data = TM_NULL;
        out->size = 0;
        out->capacity = 0;
    } else {
        out->size += size;
    }
}

#if TMU_UCD_HAS_SIMPLE_CASE
TMU_DEF tmu_transform_result tmu_utf8_to_upper_simple(const char* str, tm_size_t str_len, char* out,
                                                      tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_upper_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tmu_transform_result tmu_utf8_to_title_simple(const char* str, tm_size_t str_len, char* out,
                                                      tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_title_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tmu_transform_result tmu_utf8_to_lower_simple(const char* str, tm_size_t str_len, char* out,
                                                      tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_lower_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
#endif /* TMU_UCD_HAS_SIMPLE_CASE */

#if TMU_UCD_HAS_SIMPLE_CASE_FOLD
TMU_DEF tmu_transform_result tmu_utf8_to_case_fold_simple(const char* str, tm_size_t str_len, char* out,
                                                          tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_case_fold_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tm_bool tmu_utf8_equals_ignore_case_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    tmu_utf8_stream a_stream = tmu_utf8_make_stream_n(a, a_len);
    tmu_utf8_stream b_stream = tmu_utf8_make_stream_n(b, b_len);

    TM_ASSERT(a_stream.cur <= a_stream.end);
    TM_ASSERT(b_stream.cur <= b_stream.end);
    /* We can't early exit by comparing sizes, since utf8 is variable length
       and differing cases can have differing lengths, even if they compare equal when case folded. */

    if (a_stream.cur == b_stream.cur) {
        /* If both strings point to same adress, we just compare the string lengths. */
        return a_len == b_len;
    }

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&a_stream, &a_cp) && tmu_utf8_extract(&b_stream, &b_cp)) {
        const tmu_ucd_internal* a_internal = tmu_get_ucd_internal(a_cp);
        const tmu_ucd_internal* b_internal = tmu_get_ucd_internal(b_cp);

        uint32_t a_case_folded = a_cp + a_internal->simple_case_fold_offset;
        uint32_t b_case_folded = b_cp + b_internal->simple_case_fold_offset;

        if (a_case_folded != b_case_folded) return TM_FALSE;
    }
    return (a_stream.cur == a_stream.end) && (b_stream.cur == b_stream.end);
}

TMU_DEF int tmu_utf8_compare_ignore_case_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    tmu_utf8_stream a_stream = tmu_utf8_make_stream_n(a, a_len);
    tmu_utf8_stream b_stream = tmu_utf8_make_stream_n(b, b_len);

    TM_ASSERT(a_stream.cur <= a_stream.end);
    TM_ASSERT(b_stream.cur <= b_stream.end);

    if (!a_len || !b_len) return !b_len - !a_len;
    if (a_stream.cur == b_stream.cur) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&a_stream, &a_cp) && tmu_utf8_extract(&b_stream, &b_cp)) {
        const tmu_ucd_internal* a_internal = tmu_get_ucd_internal(a_cp);
        const tmu_ucd_internal* b_internal = tmu_get_ucd_internal(b_cp);

        uint32_t a_case_folded = a_cp + a_internal->simple_case_fold_offset;
        uint32_t b_case_folded = b_cp + b_internal->simple_case_fold_offset;

        int diff = (int)a_case_folded - (int)b_case_folded;
        if (diff != 0) return (diff < 0) ? -1 : 1;
    }

    tm_bool a_is_empty = (a_stream.cur == a_stream.end);
    tm_bool b_is_empty = (b_stream.cur == b_stream.end);
    return b_is_empty - a_is_empty;
}

/*
tmu_utf8_human_compare implementation is based on this gist: https://gist.github.com/pervognsen/733034 by Per Vognsen.
*/
static tm_bool tmu_utf8_extract_human_simple(tmu_utf8_stream* stream, uint32_t* codepoint) {
    const char* cur = stream->cur;
    const char* end = stream->end;

    uint32_t base = TMU_INVALID_CODEPOINT;
    if (!tmu_utf8_extract(stream, &base)) return TM_FALSE;

    if (base < (uint8_t)'0' || base > (uint8_t)'9') {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(base);
        *codepoint = base + internal->simple_case_fold_offset;
        return TM_TRUE;
    }

    /* We can treat the utf8 stream as an ascii stream and go byte by byte, since we are comparing ascii values. */
    uint32_t value = 0;
    while (cur != end && *cur >= '0' && *cur <= '9') {
        value = (value * 10) + (*cur - '0');
        ++cur;
    }
    stream->cur = cur;
    TM_ASSERT(stream->cur <= stream->end);

    /* We return an invalid codepoint value, since we want this "codepoint" to compare greater than any other letter. */
    *codepoint = value + TMU_MAX_UTF32;
    return TM_TRUE;
}

TMU_DEF int tmu_utf8_human_compare_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    if (!a_len || !b_len) return !b_len - !a_len;
    if (a == b) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    tmu_utf8_stream a_stream = tmu_utf8_make_stream_n(a, a_len);
    tmu_utf8_stream b_stream = tmu_utf8_make_stream_n(b, b_len);

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract_human_simple(&a_stream, &a_cp) && tmu_utf8_extract_human_simple(&b_stream, &b_cp)) {
        int diff = (int)a_cp - (int)b_cp;
        if (diff != 0) return (diff < 0) ? -1 : 1;
    }

    tm_bool a_is_empty = (a_stream.cur == a_stream.end);
    tm_bool b_is_empty = (b_stream.cur == b_stream.end);
    return b_is_empty - a_is_empty;
}
#endif /* TMU_UCD_HAS_SIMPLE_CASE_FOLD */

#if TMU_UCD_HAS_SIMPLE_CASE_TOGGLE
TMU_DEF tmu_transform_result tmu_utf8_toggle_case_simple(const char* str, tm_size_t str_len, char* out,
                                                         tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_case_toggle_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
#endif /* TMU_UCD_HAS_SIMPLE_CASE_TOGGLE */

#if TMU_UCD_HAS_FULL_CASE
TMU_DEF tmu_transform_result tmu_utf8_to_upper(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_upper_index) {
            const uint16_t* full = tmu_codepoint_runs + tmu_full_upper_offset + internal->full_upper_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_upper_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tmu_transform_result tmu_utf8_to_title(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_title_index) {
            const uint16_t* full = tmu_codepoint_runs + tmu_full_title_offset + internal->full_title_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_title_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tmu_transform_result tmu_utf8_to_lower(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_lower_index) {
            const uint16_t* full = tmu_codepoint_runs + tmu_full_lower_offset + internal->full_lower_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_lower_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
#endif /* TMU_UCD_HAS_FULL_CASE */

#if TMU_UCD_HAS_FULL_CASE_TOGGLE
TMU_DEF tmu_transform_result tmu_utf8_toggle_case(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_case_toggle_index) {
            const uint16_t* full = tmu_codepoint_runs + internal->full_case_toggle_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_case_toggle_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
#endif /* TMU_UCD_HAS_FULL_CASE_TOGGLE */

#if TMU_UCD_HAS_FULL_CASE_FOLD
TMU_DEF tmu_transform_result tmu_utf8_to_case_fold(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {TM_NULL, 0, 0, {0, TM_OK}};
    out_stream.data = out;
    out_stream.capacity = out_len;
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_case_fold_index) {
            const uint16_t* full = tmu_codepoint_runs + tmu_full_case_fold_offset + internal->full_case_fold_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_case_fold_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}

typedef struct {
    tmu_utf8_stream base;
    const uint16_t* full_case_fold;
} tmu_utf8_case_fold_stream;

static tm_bool tmu_utf8_extract_case_folded(tmu_utf8_case_fold_stream* stream, uint32_t* codepoint) {
    TM_ASSERT(stream);
    TM_ASSERT(stream->base.cur <= stream->base.end);
    TM_ASSERT(codepoint);

    if (stream->full_case_fold) {
        TM_ASSERT(*stream->full_case_fold);
        *codepoint = *stream->full_case_fold;
        ++stream->full_case_fold;
        if (!*stream->full_case_fold) stream->full_case_fold = TM_NULL;
        return TM_TRUE;
    }
    uint32_t base_codepoint = TMU_INVALID_CODEPOINT;
    if (!tmu_utf8_extract(&stream->base, &base_codepoint)) return TM_FALSE;
    const tmu_ucd_internal* internal = tmu_get_ucd_internal(base_codepoint);
    if (internal->full_case_fold_index) {
        stream->full_case_fold = tmu_codepoint_runs + tmu_full_case_fold_offset + internal->full_case_fold_index;
        TM_ASSERT(*stream->full_case_fold);
        *codepoint = *stream->full_case_fold;
        ++stream->full_case_fold;
        if (!*stream->full_case_fold) stream->full_case_fold = TM_NULL;
        return TM_TRUE;
    }
    *codepoint = base_codepoint + internal->simple_case_fold_offset;
    return TM_TRUE;
}

TMU_DEF tm_bool tmu_utf8_equals_ignore_case(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);
    /* We can't early exit by comparing sizes, since utf8 is variable length
       and differing cases can have differing lengths, even if they compare equal when case folded. */

    if (a == b) {
        /* If both strings point to same adress, we just compare the string lengths. */
        return a_len == b_len;
    }

    tmu_utf8_case_fold_stream a_cf = {{TM_NULL, TM_NULL}, TM_NULL};
    tmu_utf8_case_fold_stream b_cf = {{TM_NULL, TM_NULL}, TM_NULL};

    a_cf.base = tmu_utf8_make_stream_n(a, a_len);
    b_cf.base = tmu_utf8_make_stream_n(b, b_len);

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract_case_folded(&a_cf, &a_cp) && tmu_utf8_extract_case_folded(&b_cf, &b_cp)) {
        if (a_cp != b_cp) return TM_FALSE;
    }
    return (a_cf.base.cur == a_cf.base.end) && (!a_cf.full_case_fold) && (b_cf.base.cur == b_cf.base.end) &&
           (!b_cf.full_case_fold);
}

TMU_DEF int tmu_utf8_compare_ignore_case(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    if (!a_len || !b_len) return !b_len - !a_len;
    if (a == b) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    tmu_utf8_case_fold_stream a_cf = {{TM_NULL, TM_NULL}, TM_NULL};
    tmu_utf8_case_fold_stream b_cf = {{TM_NULL, TM_NULL}, TM_NULL};

    a_cf.base = tmu_utf8_make_stream_n(a, a_len);
    b_cf.base = tmu_utf8_make_stream_n(b, b_len);

    uint32_t a_cp = 0;
    uint32_t b_cp = 0;
    while (tmu_utf8_extract_case_folded(&a_cf, &a_cp) && tmu_utf8_extract_case_folded(&b_cf, &b_cp)) {
        int diff = (int)a_cp - (int)b_cp;
        if (diff != 0) return (diff < 0) ? -1 : 1;
    }

    tm_bool a_is_empty = (a_cf.base.cur == a_cf.base.end) && (!a_cf.full_case_fold);
    tm_bool b_is_empty = (b_cf.base.cur == b_cf.base.end) && (!b_cf.full_case_fold);
    return b_is_empty - a_is_empty;
}

/*
tmu_utf8_human_compare implementation is based on this gist: https://gist.github.com/pervognsen/733034 by Per Vognsen.
*/
static tm_bool tmu_utf8_extract_human(tmu_utf8_case_fold_stream* stream, uint32_t* codepoint) {
    const char* cur = stream->base.cur;
    const char* end = stream->base.end;

    uint32_t base = TMU_INVALID_CODEPOINT;
    if (!tmu_utf8_extract_case_folded(stream, &base)) return TM_FALSE;

    if (base < (uint8_t)'0' || base > (uint8_t)'9') {
        *codepoint = base;
        return TM_TRUE;
    }

    /* We can treat the utf8 stream as an ascii stream and go byte by byte, since we are comparing ascii values. */
    uint32_t value = 0;
    while (cur != end && *cur >= '0' && *cur <= '9') {
        value = (value * 10) + (*cur - '0');
        ++cur;
    }
    stream->base.cur = cur;
    TM_ASSERT(stream->base.cur <= stream->base.end);

    /* We return an invalid codepoint value, since we want this "codepoint" to compare greater than any other letter. */
    *codepoint = value + TMU_MAX_UTF32;
    return TM_TRUE;
}

TMU_DEF int tmu_utf8_human_compare(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    if (!a_len || !b_len) return !b_len - !a_len;
    if (a == b) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    tmu_utf8_case_fold_stream a_cf = {{TM_NULL, TM_NULL}, TM_NULL};
    tmu_utf8_case_fold_stream b_cf = {{TM_NULL, TM_NULL}, TM_NULL};

    a_cf.base = tmu_utf8_make_stream_n(a, a_len);
    b_cf.base = tmu_utf8_make_stream_n(b, b_len);

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract_human(&a_cf, &a_cp) && tmu_utf8_extract_human(&b_cf, &b_cp)) {
        int diff = (int)a_cp - (int)b_cp;
        if (diff != 0) return (diff < 0) ? -1 : 1;
    }

    tm_bool a_is_empty = (a_cf.base.cur == a_cf.base.end) && (!a_cf.full_case_fold);
    tm_bool b_is_empty = (b_cf.base.cur == b_cf.base.end) && (!b_cf.full_case_fold);
    return b_is_empty - a_is_empty;
}
#endif /* TMU_UCD_HAS_FULL_CASE_FOLD */

#endif /* defined(TMU_UCD_HAS_CASE_INFO) */

#if !defined(TMU_NO_FILE_IO)

/* clang-format off */
#if (!defined(_WIN32) && !defined(TMU_TESTING_MSVC_CRT)) || defined(TMU_TESTING_UNIX)
	#if defined(__GNUC__) || defined(__clang__) || defined(__linux__) || defined(TMU_TESTING_UNIX)
		#define TMU_PLATFORM_UNIX
	#endif
#endif

/* Headers */
#if defined(TMU_USE_CRT) && !defined(TMU_USE_WINDOWS_H)
	#if !defined(TMU_TESTING)
	    #ifdef _MSC_VER
	        #include <sys/types.h> /* Required on msvc so that sys/stat.h and wchar.h define additional functions. */
	    #endif
	    #include <sys/stat.h> /* stat function */
	    #include <errno.h> /* errno */

	    #include <wchar.h>
	#endif /* !defined(TMU_TESTING) */
#endif /* defined(TMU_USE_CRT) && !defined(TMU_USE_WINDOWS_H) */

#if defined(_MSC_VER) || defined(TMU_TESTING_MSVC_CRT)
	#ifndef TMU_TEXT
	    #define TMU_TEXT(x) L##x
	#endif
	#define TMU_TEXTLEN TMU_WCSLEN
	#define TMU_TEXTCHR TMU_STRCHRW
	#define TMU_DIR_DELIM L'\\'

	#define TMU_STAT _wstat64
	#define TMU_STRUCT_STAT struct __stat64
	#ifndef TMU_S_ISDIR
	    #define TMU_S_ISDIR(mode) (((mode) & _S_IFDIR) != 0)
	    #define TMU_S_ISREG(mode) (((mode) & _S_IFREG) != 0)
	#endif
	#define TMU_MKDIR _wmkdir
	#define TMU_RMDIR _wrmdir
	#define TMU_REMOVE _wremove
	#define TMU_RENAME _wrename
	#define TMU_GETCWD _wgetcwd

#elif defined(TMU_PLATFORM_UNIX)
	#ifndef TMU_TESTING
	    #include <unistd.h> /* getcwd */
	#endif /* !defined(TMU_TESTING) */

	#ifndef TMU_TEXT
	    #define TMU_TEXT(x) x
	#endif
	#define TMU_TEXTLEN strlen
	#define TMU_TEXTCHR strchr
	#define TMU_DIR_DELIM '/'

	#define TMU_STAT stat
	#define TMU_STRUCT_STAT struct stat
	#ifndef TMU_S_ISDIR
	    #define TMU_S_ISDIR(x) S_ISDIR(x)
	    #define TMU_S_ISREG(x) S_ISREG(x)
	#endif
	#define TMU_MKDIR(dir) mkdir((dir), /*mode=*/0777u) /* 0777u is read, write, execute permissions for all types. */
	#define TMU_RMDIR rmdir
	#define TMU_REMOVE remove
	#define TMU_RENAME rename
	#define TMU_GETCWD getcwd

#endif /* defined(__GNUC__) || defined(__clang__) || defined(__linux__) || defined(TMU_TESTING_UNIX) */
/* clang-format on */


static tm_errc tmu_create_directory_internal(const tmu_tchar* dir, tm_size_t dir_len);
static tm_size_t tmu_get_path_len_internal(const tmu_tchar* filename, tm_size_t filename_len);
struct tmu_contents_struct;
static void tmu_to_tmu_path(struct tmu_contents_struct* path, tm_bool is_dir);
TMU_DEF tm_bool tmu_grow_by(struct tmu_contents_struct* contents, tm_size_t amount);

/* Platform Tests */
#if defined(_MSC_VER) || defined(TMU_TESTING_MSVC_CRT)

#if defined(TMU_USE_CRT)
static FILE* tmu_fopen_t(const tmu_tchar* filename, const tmu_tchar* mode) { return _wfopen(filename, mode); }
static FILE* tmu_freopen_t(const tmu_tchar* filename, const tmu_tchar* mode, FILE* current) {
    return _wfreopen(filename, mode, current);
}
#endif /* defined(TMU_USE_CRT) */

#if defined(TMU_USE_WINDOWS_H)

typedef struct {
    tmu_tchar* path;
    tmu_tchar sbo[TMU_SBO_SIZE];
    tm_size_t allocated_size;
} tmu_platform_path;

static tm_errc tmu_winerror_to_errc(DWORD error, tm_errc def) {
    switch (error) {
        case ERROR_ACCESS_DENIED:
        case ERROR_CANNOT_MAKE:
        case ERROR_CURRENT_DIRECTORY:
        case ERROR_INVALID_ACCESS:
        case ERROR_NOACCESS:
        case ERROR_SHARING_VIOLATION:
        case ERROR_WRITE_PROTECT:
            return TM_EACCES;

        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS:
            return TM_EEXIST;

        case ERROR_CANTOPEN:
        case ERROR_CANTREAD:
        case ERROR_CANTWRITE:
        case ERROR_OPEN_FAILED:
        case ERROR_READ_FAULT:
        case ERROR_SEEK:
        case ERROR_WRITE_FAULT:
            return TM_EIO;

        case ERROR_DIRECTORY:
        case ERROR_INVALID_HANDLE:
        case ERROR_INVALID_NAME:
        case ERROR_NEGATIVE_SEEK:
        case ERROR_NO_UNICODE_TRANSLATION:
        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_FLAGS:
            return TM_EINVAL;

        case ERROR_INSUFFICIENT_BUFFER:
            return TM_ERANGE;

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            return TM_ENOENT;

        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY:
            return TM_ENOMEM;

        case ERROR_BAD_UNIT:
        case ERROR_DEV_NOT_EXIST:
        case ERROR_INVALID_DRIVE:
            return TM_ENODEV;

        case ERROR_BUSY:
        case ERROR_BUSY_DRIVE:
        case ERROR_DEVICE_IN_USE:
        case ERROR_OPEN_FILES:
            return TM_EBUSY;

        case ERROR_DISK_FULL:
        case ERROR_HANDLE_DISK_FULL:
            return TM_ENOSPC;

        case ERROR_BUFFER_OVERFLOW:
            return TM_ENAMETOOLONG;

        case ERROR_DIR_NOT_EMPTY:
            return TM_ENOTEMPTY;

        case ERROR_NOT_SAME_DEVICE:
            return TM_EEXDEV;

        case ERROR_TOO_MANY_OPEN_FILES:
            return TM_EMFILE;

#if 0
        case ERROR_NOT_READY:
        case ERROR_RETRY:
            return TM_EAGAIN;

        case ERROR_INVALID_FUNCTION:
            return TM_ENOSYS;

        case ERROR_LOCK_VIOLATION:
        case ERROR_LOCKED:
            return TM_ENOLCK;

        case ERROR_OPERATION_ABORTED:
            return TM_ECANCELED;
#endif

        default:
            return def;
    }
}

static tm_bool tmu_to_platform_path_n(const char* path, tm_size_t size, tmu_platform_path* out) {
    if (size <= 0) return TM_TRUE;

    int required_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, (int)size, TM_NULL, 0);
    if (required_size <= 0) return TM_FALSE; /* Size was not zero, so conversion failed. */
    ++required_size;                         /* Extra space for null-terminator. */

    if (required_size <= TMU_SBO_SIZE) {
        out->path = out->sbo;
        out->allocated_size = 0;
    } else {
        out->path = (WCHAR*)TMU_MALLOC(required_size * sizeof(WCHAR), sizeof(WCHAR));
        if (!out->path) return TM_FALSE;
        out->allocated_size = required_size;
    }

    int converted_size =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, (int)size, out->path, (int)required_size);
    /* Always nullterminate, since MultiByteToWideChar doesn't null-terminate when the supplying it a length param. */
    if (required_size) {
        if (converted_size <= 0) {
            out->path[0] = 0;
        } else {
            if (converted_size < required_size) {
                out->path[converted_size] = 0;
            } else {
                out->path[required_size - 1] = 0;
            }
        }
    }

    /* Turn path to win32 path. */
    for (WCHAR* cur = out->path;; ++cur) {
        switch (*cur) {
            case 0: {
                return TM_TRUE;
            }
            case L'/': {
                *cur = L'\\';
                break;
            }
        }
    }
}

static tm_bool tmu_to_platform_path(const char* path, tmu_platform_path* out) {
    return tmu_to_platform_path_n(path, (tm_size_t)lstrlenA(path), out);
}

static tmu_exists_result tmu_file_exists_t(const WCHAR* filename) {
    tmu_exists_result result = {TM_FALSE, TM_OK};
    DWORD attributes = GetFileAttributesW(filename);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        DWORD last_error = GetLastError();
        if (last_error == ERROR_FILE_NOT_FOUND || last_error == ERROR_PATH_NOT_FOUND) {
            result.ec = TM_OK;
            result.exists = TM_FALSE;
        } else {
            result.ec = tmu_winerror_to_errc(last_error, TM_EIO);
        }
    } else {
        result.exists = (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }
    return result;
}

static tmu_exists_result tmu_directory_exists_t(const WCHAR* dir) {
    tmu_exists_result result = {TM_FALSE, TM_OK};
    DWORD attributes = GetFileAttributesW(dir);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        DWORD last_error = GetLastError();
        if (last_error == ERROR_FILE_NOT_FOUND || last_error == ERROR_PATH_NOT_FOUND) {
            result.ec = TM_OK;
            result.exists = TM_FALSE;
        } else {
            result.ec = tmu_winerror_to_errc(last_error, TM_EIO);
        }
    } else {
        result.exists = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    return result;
}

static tmu_file_timestamp_result tmu_file_timestamp_t(const WCHAR* filename) {
    tmu_file_timestamp_result result = {0, TM_OK};

    WIN32_FILE_ATTRIBUTE_DATA fileAttr = {0};
    if (GetFileAttributesExW(filename, GetFileExInfoStandard, &fileAttr) == 0) {
        result.ec = TMU_NO_SUCH_FILE_OR_DIRECTORY;
        return result;
    }

    TM_STATIC_ASSERT(sizeof(result.file_time) == sizeof(fileAttr.ftLastWriteTime), invalid_file_time_size);
    TMU_MEMCPY(&result.file_time, &fileAttr.ftLastWriteTime, sizeof(fileAttr.ftLastWriteTime));
    return result;
}

static tmu_contents_result tmu_read_file_t(const WCHAR* filename) {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    HANDLE file =
        CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, TM_NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, TM_NULL);

    if (file == INVALID_HANDLE_VALUE) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(file, &size)) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        CloseHandle(file);
        return result;
    }
    if (size.QuadPart > 0) {
        if (size.QuadPart > INT32_MAX) {
            /* TODO: ReadFile can only read 2^32 bytes at a time, implement reading in chunks. */
            result.ec = TM_EFBIG;
            CloseHandle(file);
            return result;
        }

        tm_size_t data_size = (tm_size_t)size.QuadPart;
        char* data = (char*)TMU_MALLOC(data_size * sizeof(char), sizeof(char));
        if (!data) {
            result.ec = TM_ENOMEM;
            CloseHandle(file);
            return result;
        }

        DWORD bytes_to_read = (DWORD)size.QuadPart;
        DWORD bytes_read = 0;
        BOOL read_file_result = ReadFile(file, data, bytes_to_read, &bytes_read, TM_NULL);
        if (!read_file_result || bytes_read != bytes_to_read) {
            result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
            TMU_FREE(data, data_size * sizeof(char), sizeof(char));
            CloseHandle(file);
            return result;
        }

        result.contents.data = data;
        result.contents.size = data_size;
        result.contents.capacity = data_size;
    }
    CloseHandle(file);
    return result;
}

static tm_errc tmu_create_single_directory_t(const WCHAR* dir) {
    tm_errc result = TM_OK;
    if (!CreateDirectoryW(dir, TM_NULL)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            result = tmu_winerror_to_errc(error, TM_EIO);
        }
    }
    return result;
}

static tmu_write_file_result tmu_write_file_ex_internal(const WCHAR* filename, const void* data, tm_size_t size,
                                                        uint32_t flags) {
    TM_ASSERT_VALID_SIZE(size);
    tmu_write_file_result result = {0, TM_OK};

    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(filename, tmu_get_path_len_internal(filename, /*filename_len=*/0));
        if (ec != TM_OK) {
            result.ec = ec;
            return result;
        }
    }

    DWORD creation_flags = 0;
    if (flags & tmu_overwrite) {
        creation_flags = CREATE_ALWAYS;
    } else {
        creation_flags = CREATE_NEW;
    }
    HANDLE file = CreateFileW(filename, GENERIC_WRITE, 0, TM_NULL, creation_flags, FILE_ATTRIBUTE_NORMAL, TM_NULL);
    if (file == INVALID_HANDLE_VALUE) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }
    if ((size_t)size > (size_t)UINT32_MAX) {
        /* TODO: Implement writing in chunks so there is no UINT32_MAX limit. */
        result.ec = TM_EOVERFLOW;
        CloseHandle(file);
        return result;
    }

    DWORD total_bytes_written = 0;
    DWORD bytes_written = 0;

    if (flags & tmu_write_byte_order_mark) {
        if (!WriteFile(file, tmu_utf8_bom, sizeof(tmu_utf8_bom), &bytes_written, TM_NULL)) {
            result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
            CloseHandle(file);
            return result;
        }
        total_bytes_written += bytes_written;
    }

    DWORD truncated_buffer_size = (DWORD)size;
    if (!WriteFile(file, data, truncated_buffer_size, &bytes_written, TM_NULL)) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        CloseHandle(file);
        return result;
    }
    total_bytes_written += bytes_written;

    CloseHandle(file);
    result.written = (tm_size_t)total_bytes_written;
    return result;
}

static tmu_write_file_result tmu_write_file_ex_t(const WCHAR* filename, const void* data, tm_size_t size,
                                                 uint32_t flags) {
    if (!(flags & tmu_atomic_write)) {
        return tmu_write_file_ex_internal(filename, data, size, flags);
    }

    tmu_write_file_result result = {0, TM_OK};

    tm_size_t filename_len = (tm_size_t)lstrlenW(filename);
    tm_size_t dir_len = tmu_get_path_len_internal(filename, filename_len);
    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(filename, dir_len);
        if (ec != TM_OK) {
            result.ec = ec;
            return result;
        }
    }

    WCHAR* temp_file = TM_NULL;
    WCHAR temp_file_buffer[MAX_PATH];
    const tm_size_t temp_file_len = filename_len + 9;

    /* Limitation of GetTempFileNameW, dir_len cannot be bigger than MAX_PATH - 14. */
    if (dir_len < MAX_PATH - 14) {
        WCHAR temp_dir[MAX_PATH];
        TMU_MEMCPY(temp_dir, filename, dir_len * sizeof(WCHAR));
        temp_dir[dir_len] = 0;
        if (GetTempFileNameW(temp_dir, TMU_TEXT("tmf"), 0, temp_file_buffer) == 0) {
            result.ec = TM_EIO;
            return result;
        }

        temp_file = temp_file_buffer;
    } else {
        /* Fallback to using a simple temp_file. */
        temp_file = (WCHAR*)TMU_MALLOC(temp_file_len * sizeof(WCHAR), sizeof(WCHAR));
        if (!temp_file) {
            result.ec = TM_ENOMEM;
            return result;
        }
        TMU_MEMCPY(temp_file, filename, filename_len * sizeof(WCHAR));
        /* Copy filename ending + null terminator. */
        TMU_MEMCPY(temp_file + filename_len, TMU_TEXT(".tmu_tmp"), sizeof(WCHAR) * 9);
    }

    TM_ASSERT(temp_file);

    tmu_write_file_result temp_write_result = tmu_write_file_ex_internal(temp_file, data, size, 0);
    if (temp_write_result.ec != TM_OK) {
        result.ec = temp_write_result.ec;
        if (temp_file != temp_file_buffer) TMU_FREE(temp_file, temp_file_len * sizeof(WCHAR), sizeof(WCHAR));
        return result;
    }

    DWORD move_flags = MOVEFILE_COPY_ALLOWED;
    if (flags & tmu_overwrite) move_flags |= MOVEFILE_REPLACE_EXISTING;
    if (!MoveFileExW(temp_file, filename, move_flags)) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        if (temp_file != temp_file_buffer) TMU_FREE(temp_file, temp_file_len * sizeof(WCHAR), sizeof(WCHAR));
        return result;
    }
    if (temp_file != temp_file_buffer) TMU_FREE(temp_file, temp_file_len * sizeof(WCHAR), sizeof(WCHAR));
    return result;
}

static tm_errc tmu_rename_file_ex_t(const WCHAR* from, const WCHAR* to, uint32_t flags) {
    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(to, tmu_get_path_len_internal(to, /*filename_len=*/0));
        if (ec != TM_OK) return ec;
    }

    DWORD move_flags = MOVEFILE_COPY_ALLOWED;
    if (flags & tmu_overwrite) move_flags |= MOVEFILE_REPLACE_EXISTING;
    if (!MoveFileExW(from, to, move_flags)) return tmu_winerror_to_errc(GetLastError(), TM_EIO);
    return TM_OK;
}

static tm_errc tmu_delete_file_t(const WCHAR* filename) {
    if (!DeleteFileW(filename)) return tmu_winerror_to_errc(GetLastError(), TM_EIO);
    return TM_OK;
}

static tm_errc tmu_delete_directory_t(const WCHAR* dir) {
    if (!RemoveDirectoryW(dir)) return tmu_winerror_to_errc(GetLastError(), TM_EIO);
    return TM_OK;
}

static tmu_contents_result tmu_to_utf8(const WCHAR* str, tm_size_t extra_size) {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};
    if (str && *str) {
        int size = WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, TM_NULL, 0, TM_NULL, TM_NULL);
        if (size <= 0) {
            result.ec = tmu_winerror_to_errc(GetLastError(), TM_EINVAL);
        } else {
            TM_ASSERT((tm_size_t)extra_size < (tm_size_t)(INT32_MAX - size));
            size += (int)extra_size + 1;
            result.contents.data = (char*)TMU_MALLOC(size * sizeof(char), sizeof(char));
            if (!result.contents.data) {
                result.ec = TM_ENOMEM;
            } else {
                result.contents.capacity = size;

                int real_size = WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, result.contents.data, size,
                                                    TM_NULL, TM_NULL);
                result.contents.size = (tm_size_t)real_size;

                /* Always nullterminate. */
                if (!real_size) {
                    result.contents.data[0] = 0;
                } else {
                    if (real_size < size) {
                        result.contents.data[real_size] = 0;
                    } else {
                        result.contents.data[size - 1] = 0;
                    }
                }
            }
        }
    }
    return result;
}

TMU_DEF tmu_contents_result tmu_current_working_directory(tm_size_t extra_size) {
    TM_ASSERT_VALID_SIZE(extra_size);

    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    /* This will return the size necessary including null-terminator. */
    DWORD len = GetCurrentDirectoryW(0, TM_NULL);
    if (!len) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }
    WCHAR* dir = (WCHAR*)TMU_MALLOC(len * sizeof(WCHAR), sizeof(WCHAR));
    if (!dir) {
        result.ec = TM_ENOMEM;
        return result;
    }

    /* This will return the size written without null-terminator. */
    DWORD written = GetCurrentDirectoryW(len, dir);
    TM_UNREFERENCED(written);
    TM_ASSERT(written + 1 == len);

    result = tmu_to_utf8(dir, extra_size + 1);
    if (result.ec == TM_OK) tmu_to_tmu_path(&result.contents, /*is_dir=*/TM_TRUE);
    TMU_FREE(dir, len * sizeof(WCHAR), sizeof(WCHAR));
    return result;
}

TMU_DEF tmu_utf8_command_line_result tmu_utf8_winapi_get_command_line() {
    tmu_utf8_command_line_result result = {{TM_NULL, 0, TM_NULL, 0}, TM_OK};

    const WCHAR* wide_command_line = GetCommandLineW();
    if (!wide_command_line) result.ec = TM_EPERM;

    int wide_args_count = 0;
    WCHAR** wide_args = TM_NULL;
    if (result.ec == TM_OK) {
        wide_args = CommandLineToArgvW(wide_command_line, &wide_args_count);
        if (!wide_args) result.ec = tmu_winerror_to_errc(GetLastError(), TM_EPERM);
    }

    if (result.ec == TM_OK) {
        result = tmu_utf8_command_line_from_utf16(wide_args, wide_args_count);
    }

    if (wide_args) {
        LocalFree(wide_args);
        wide_args = TM_NULL;
    }
    return result;
}

#if defined(__cplusplus)
TMU_DEF tmu_utf8_command_line_managed_result tmu_utf8_winapi_get_command_line_managed() {
    tmu_utf8_command_line_managed_result result;
    tmu_utf8_command_line_result unmanaged = tmu_utf8_winapi_get_command_line();
    static_cast<tmu_utf8_command_line&>(result.command_line) = unmanaged.command_line;
    result.ec = unmanaged.ec;
    return result;
}
#endif /* defined(__cplusplus) */

#elif defined(TMU_USE_CRT)

#define TMU_IMPLEMENT_CRT
typedef struct {
    tmu_tchar* path;
    tmu_tchar sbo[TMU_SBO_SIZE];
    tm_size_t allocated_size;
} tmu_platform_path;

static void tmu_translate_path_delims(tmu_char16* buffer) {
    for (tmu_char16* cur = buffer;; ++cur) {
        switch (*cur) {
            case 0: {
                return;
            }
            case L'/': {
                *cur = L'\\';
                break;
            }
        }
    }
}

static void tmu_destroy_platform_path(tmu_platform_path* path);

static tm_bool tmu_to_platform_path_internal(tmu_utf8_stream path_stream, tmu_platform_path* out) {
    TM_ASSERT(out);
    out->path = out->sbo;
    out->allocated_size = 0;

    tm_size_t buffer_size = TMU_SBO_SIZE;

    tmu_conversion_result conv_result =
        tmu_utf16_from_utf8_ex(path_stream, tmu_validate_error, /*replace_str=*/TM_NULL, /*replace_str_len=*/0,
                               /*nullterminate=*/TM_TRUE, out->sbo, buffer_size);
    if (conv_result.ec == TM_ERANGE) {
        buffer_size = conv_result.size;
        TM_ASSERT_VALID_SIZE(conv_result.size);
        out->path = (tmu_tchar*)TMU_MALLOC(buffer_size * sizeof(tmu_tchar), sizeof(tmu_tchar));
        if (!out->path) return TM_FALSE;
        out->allocated_size = buffer_size;
        conv_result =
            tmu_utf16_from_utf8_ex(path_stream, tmu_validate_error, /*replace_str=*/TM_NULL, /*replace_str_len=*/0,
                                   /*nullterminate=*/TM_TRUE, out->path, out->allocated_size);
    }
    if (conv_result.ec != TM_OK) {
        tmu_destroy_platform_path(out);
        return TM_FALSE;
    }
    /* Must be nullterminated. */
    TM_ASSERT_VALID_SIZE(conv_result.size);
    TM_ASSERT(conv_result.size < buffer_size);
    TM_ASSERT(out->path[conv_result.size] == 0);
    return TM_TRUE;
}

static tm_bool tmu_to_platform_path(const char* path, tmu_platform_path* out) {
    if (!tmu_to_platform_path_internal(tmu_utf8_make_stream(path), out)) return TM_FALSE;
    tmu_translate_path_delims(out->path);
    return TM_TRUE;
}

#if defined(TM_STRING_VIEW) && defined(__cplusplus)
static tm_bool tmu_to_platform_path_n(const char* path, tm_size_t size, tmu_platform_path* out) {
    if (!tmu_to_platform_path_internal(tmu_utf8_make_stream_n(path, size), out)) return TM_FALSE;
    tmu_translate_path_delims(out->path);
    return TM_TRUE;
}
#endif

TMU_DEF tmu_contents_result tmu_current_working_directory(tm_size_t extra_size) {
    TM_UNREFERENCED_PARAM(extra_size);
    TM_ASSERT_VALID_SIZE(extra_size);

    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    errno = 0;
    tmu_char16* dir = _wgetcwd(TM_NULL, 1);
    if (!dir) {
        result.ec = (errno != 0) ? errno : TM_ENOMEM;
        return result;
    }

    tmu_utf16_stream dir_stream = tmu_utf16_make_stream(dir);
    tmu_conversion_result conv_result =
        tmu_utf8_from_utf16_ex(dir_stream, tmu_validate_error, /*replace_str=*/TM_NULL,
                               /*replace_str_len=*/0,
                               /*nullterminate=*/TM_TRUE, /*out=*/TM_NULL, /*out_len=*/0);
    if (conv_result.ec == TM_ERANGE) {
        /* Extra size for trailing '/'. */
        tm_size_t capacity = conv_result.size + extra_size + 1;
        result.contents.data = (char*)TMU_MALLOC(capacity * sizeof(char), sizeof(char));
        if (!result.contents.data) {
            result.ec = TM_ENOMEM;
        } else {
            result.contents.capacity = capacity;
            conv_result =
                tmu_utf8_from_utf16_ex(dir_stream, tmu_validate_error, /*replace_str=*/TM_NULL,
                                       /*replace_str_len=*/0,
                                       /*nullterminate=*/TM_TRUE, result.contents.data, result.contents.capacity);
            TM_ASSERT(conv_result.ec == TM_OK);
            TM_ASSERT(conv_result.size + 2 < result.contents.capacity);
            result.contents.size = conv_result.size;
            tmu_to_tmu_path(&result.contents, /*is_dir=*/TM_TRUE);
        }
    } else {
        result.ec = conv_result.ec;
    }
    free(dir); /* _wgetcwd calls specifically malloc, we need to directly use free instead of TMU_FREE.*/
    return result;
}

#else
#error tm_unicode.h needs either TMU_USE_WINDOWS_H or TMU_USE_CRT to be defined.
#endif /* defined(TMU_USE_WINDOWS_H) || defined(TMU_USE_CRT) */

#elif defined(TMU_PLATFORM_UNIX)
#undef TMU_PLATFORM_UNIX

#ifndef TMU_USE_CRT
#error tm_unicode.h needs TMU_USE_CRT on this platform.
#endif

#define TMU_IMPLEMENT_CRT
typedef struct {
    const tmu_tchar* path;
    tmu_tchar sbo[TMU_SBO_SIZE];
    /* Not the length of the path string, but the number of bytes allocated, if path is malloced. */
    tm_size_t allocated_size;
} tmu_platform_path;

static tm_bool tmu_to_platform_path(const char* path, tmu_platform_path* out) {
    TM_ASSERT(out);
    out->path = path;
    out->allocated_size = 0;
    return TM_TRUE;
}

static tmu_tchar* tmu_to_platform_path_t(const tmu_tchar* path, tm_size_t size, tmu_platform_path* out);

#if defined(TM_STRING_VIEW)
static tmu_tchar* tmu_to_platform_path_n(const tmu_tchar* path, tm_size_t size, tmu_platform_path* out) {
    return tmu_to_platform_path_t(path, size, out);
}
#endif /* defined(TM_STRING_VIEW) */

static FILE* tmu_fopen_t(const tmu_tchar* filename, const tmu_tchar* mode) { return fopen(filename, mode); }
static FILE* tmu_freopen_t(const tmu_tchar* filename, const tmu_tchar* mode, FILE* current) {
    return freopen(filename, mode, current);
}

TMU_DEF tmu_contents_result tmu_current_working_directory(tm_size_t extra_size) {
    TM_ASSERT_VALID_SIZE(extra_size);

    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    const tm_size_t increment_size = 200;
    result.contents.data = (char*)TMU_MALLOC(increment_size * sizeof(char), sizeof(char));
    if (!result.contents.data) {
        result.ec = TM_ENOMEM;
    } else {
        result.contents.capacity = increment_size;
        while (getcwd(result.contents.data, result.contents.capacity) == TM_NULL) {
            if (errno == ERANGE) {
                tm_size_t new_size = result.contents.capacity + increment_size;
                char* new_data = (char*)TMU_REALLOC(result.contents.data, result.contents.capacity * sizeof(char),
                                                    sizeof(char), new_size * sizeof(char), sizeof(char));
                if (!new_data) {
                    tmu_destroy_contents(&result.contents);
                    result.ec = TM_ENOMEM;
                    break;
                }
                result.contents.data = new_data;
                result.contents.capacity = new_size;
            } else {
                result.ec = errno;
                if (result.ec == TM_OK) {
                    /* No error even though getcwd returned NULL, there shouldn't be any useful data. */
                    result.ec = TM_EIO;
                }
                break;
            }
        }
    }

    if (result.ec == TM_OK) {
        result.contents.size = (tm_size_t)TMU_STRLEN(result.contents.data);

        /* Pad by extra_size and room for trailing '/'. */
        tm_size_t new_size = result.contents.size + extra_size + 1;
        if (new_size > result.contents.capacity) {
            char* new_data = (char*)TMU_REALLOC(result.contents.data, result.contents.capacity * sizeof(char),
                                                sizeof(char), new_size * sizeof(char), sizeof(char));
            if (!new_data) {
                tmu_destroy_contents(&result.contents);
                result.ec = TM_ENOMEM;
            } else {
                result.contents.data = new_data;
                result.contents.capacity = new_size;
            }
        }
    }
    if (result.ec == TM_OK) tmu_to_tmu_path(&result.contents, /*is_dir=*/TM_TRUE);
    return result;
}

#else
#error Not implemented on this platform.
#endif /* Platform Tests */

#if defined(TMU_IMPLEMENT_CRT)
#undef TMU_IMPLEMENT_CRT

static tmu_exists_result tmu_file_exists_t(const tmu_tchar* filename) {
    TM_ASSERT(filename);

    tmu_exists_result result = {TM_FALSE, TM_OK};

    TMU_STRUCT_STAT buffer;
    int stat_result = TMU_STAT(filename, &buffer);

    if (stat_result == 0) {
        result.exists = TMU_S_ISREG(buffer.st_mode);
    } else if (stat_result == -1) {
        /* TODO: Which version to use?
           One is picky about the error to report, the other is picky about saying anything about the existence.
           It depends on the implementation details of stat (Unix vs Windows report different errors etc.). */
#if 1
        if (errno == ENOENT || errno == ENOTDIR) {
            result.exists = TM_FALSE;
        } else {
            result.ec = errno;
        }
#else
        if (errno == EINVAL || errno == EACCES || errno == EOVERFLOW) {
            result.ec = errno;
        } else {
            result.exists = TM_FALSE;
        }
#endif
    } else {
        /* On Windows stat doesn't return -1 on EINVAL. */
        result.ec = TM_EINVAL;
    }

    return result;
}

static tmu_exists_result tmu_directory_exists_t(const tmu_tchar* dir) {
    TM_ASSERT(dir);

    tmu_exists_result result = {TM_FALSE, TM_OK};

    TMU_STRUCT_STAT buffer;
    int stat_result = TMU_STAT(dir, &buffer);

    if (stat_result == 0) {
        result.exists = TMU_S_ISDIR(buffer.st_mode);
    } else if (stat_result == -1) {
        /* TODO: Which version to use?
           One is picky about the error to report, the other is picky about saying anything about the existence.
           It depends on the implementation details of stat (Unix vs Windows report different errors etc.). */
#if 1
        if (errno == ENOENT || errno == ENOTDIR) {
            result.exists = TM_FALSE;
        } else {
            result.ec = errno;
        }
#else
        if (errno == EINVAL || errno == EACCES || errno == EOVERFLOW) {
            result.ec = errno;
        } else {
            result.exists = TM_FALSE;
        }
#endif
    } else {
        /* On Windows stat doesn't return -1 on EINVAL. */
        result.ec = TM_EINVAL;
    }

    return result;
}

static tmu_contents_result tmu_read_file_t(const tmu_tchar* filename) {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    errno = 0;
    FILE* f = tmu_fopen_t(filename, TMU_TEXT("rb"));
    if (!f) {
        result.ec = (errno != 0) ? errno : TM_EIO;
    } else {
        enum { BUFFER_SIZE = 1024 };
        char buffer[BUFFER_SIZE];
        size_t size = fread(buffer, sizeof(char), sizeof(buffer), f);
        if (size) {
            result.contents.capacity = ((tm_size_t)size < BUFFER_SIZE) ? ((tm_size_t)size) : (BUFFER_SIZE * 2);
            result.contents.data = (char*)TMU_MALLOC(result.contents.capacity * sizeof(char), sizeof(char));
            if (!result.contents.data) {
                result.ec = TM_ENOMEM;
            } else {
                TMU_MEMCPY(result.contents.data, buffer, size * sizeof(char));
                result.contents.size = (tm_size_t)size;
                while ((size = fread(buffer, sizeof(char), sizeof(buffer), f)) != 0) {
                    if (!tmu_grow_by(&result.contents, (tm_size_t)size)) {
                        tmu_destroy_contents(&result.contents);
                        result.ec = TM_ENOMEM;
                        break;
                    }
                    TMU_MEMCPY(result.contents.data + result.contents.size, buffer, size * sizeof(char));
                    result.contents.size += (tm_size_t)size;
                }
            }
        }
        if (errno != 0 || ferror(f)) {
            result.ec = (errno != 0) ? errno : TM_EIO;
        }
        fclose(f);
    }

    return result;
}

static tmu_write_file_result tmu_write_file_ex_internal(const tmu_tchar* filename, const void* data, tm_size_t size,
                                                        uint32_t flags) {
    TM_ASSERT_VALID_SIZE(size);
    tmu_write_file_result result = {0, TM_OK};

    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(filename, tmu_get_path_len_internal(filename, /*filename_len=*/0));
        if (ec != TM_OK) {
            result.ec = ec;
            return result;
        }
    }

    if (!(flags & tmu_overwrite)) {
        tmu_exists_result exists = tmu_file_exists_t(filename);
        if (exists.ec != TM_OK) {
            result.ec = exists.ec;
            return result;
        }
        if (exists.exists) {
            result.ec = TM_EEXIST;
            return result;
        }
    }

    FILE* f = tmu_fopen_t(filename, TMU_TEXT("wb"));
    if (!f) {
        result.ec = (errno != 0) ? errno : TM_EIO;
        return result;
    }

    errno = 0;
    result.written = (tm_size_t)fwrite(data, 1, size, f);
    if (result.written != size || errno != 0 || ferror(f)) {
        result.ec = (errno != 0) ? errno : TM_EIO;
    }

    fclose(f);
    return result;
}

static tm_errc tmu_delete_file_t(const tmu_tchar* filename) {
    tm_errc result = TM_OK;
    if (TMU_REMOVE(filename) != 0) result = (errno != 0) ? errno : TM_EIO;
    return result;
}

static tm_errc tmu_rename_file_ex_t(const tmu_tchar* from, const tmu_tchar* to, uint32_t flags) {
    tm_errc result = TM_OK;

    /* We check if 'to' exists first, because some implementations of rename overwrite by default. */
    if (!(flags & tmu_overwrite)) {
        tmu_exists_result to_exists = tmu_file_exists_t(to);
        if (to_exists.ec != TM_OK) return to_exists.ec;
        if (to_exists.exists) return TM_EEXIST;
    }

    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(to, tmu_get_path_len_internal(to, /*filename_len=*/0));
        if (ec != TM_OK) return ec;
    }

    errno = 0;
    int err = TMU_RENAME(from, to);
    if (err != 0) {
        result = (errno != 0) ? errno : TM_EIO;
        if ((flags & tmu_overwrite) && tmu_file_exists_t(from).exists && tmu_file_exists_t(to).exists) {
            /* Destination probably exists, try to delete file since tmu_overwrite is specified and try again. */
            result = tmu_delete_file_t(to);
            if (result == TM_OK) {
                err = TMU_RENAME(from, to);
                if (err != 0) result = (errno != 0) ? errno : TM_EIO;
            }
        }
    }
    return result;
}

static tmu_write_file_result tmu_write_file_ex_t(const tmu_tchar* filename, const void* data, tm_size_t size,
                                                 uint32_t flags) {
    if (!(flags & tmu_atomic_write)) {
        return tmu_write_file_ex_internal(filename, data, size, flags);
    }

    tmu_write_file_result result = {0, TM_EIO};

#if 0
    /* FIXME: Not implemented. */
    /* Get a temp filename*/
    char temp_filename_buffer[L_tmpnam + 1];
    char* temp_filename = TM_NULL;
    for (int attempts = 0; attempts < 10; ++attempts) {
        // Try to open up a temporary file. If filename was not unique or another process got it, try again.
        temp_filename = tmpnam(temp_filename_buffer);
        if (!temp_filename) {
            result.ec = TM_EEXIST;
            break;
        }
        result = tmu_write_file_ex_internal(temp_filename, data, size, 0);
        if (result.ec == TM_EEXIST) {
            continue;
        } else {
            break;
        }
    }
    if (result.ec == TM_OK) {
        TM_ASSERT(temp_filename);
        result.ec = tmu_rename_file_ex_t(temp_filename, filename, flags);
    }
#endif
    return result;
}

static tmu_file_timestamp_result tmu_file_timestamp_t(const tmu_tchar* dir) {
    TM_ASSERT(dir);

    tmu_file_timestamp_result result = {0, TM_OK};

    TMU_STRUCT_STAT buffer;
    int stat_result = TMU_STAT(dir, &buffer);

    if (stat_result == 0) {
        result.file_time = (tmu_file_time)buffer.st_mtime;
    } else {
        if (errno != 0) {
            result.ec = errno;
        } else {
            result.ec = (stat_result == TM_EINVAL) ? TM_EINVAL : TM_ENOENT;
        }
    }

    return result;
}

static tm_errc tmu_create_single_directory_t(const tmu_tchar* dir) {
    tm_errc result = TM_OK;

    int err = TMU_MKDIR(dir);
    if (err != 0) {
        if (errno != EEXIST) {
            result = errno;
        }
    }
    return result;
}

static tm_errc tmu_delete_directory_t(const tmu_tchar* dir) {
    tm_errc result = TM_OK;
    if (TMU_RMDIR(dir) != 0) result = errno;
    return result;
}

#undef TMU_STAT
#undef TMU_STRUCT_STAT
#undef TMU_S_IFDIR
#undef TMU_S_IFREG
#undef TMU_MKDIR
#undef TMU_RMDIR
#undef TMU_REMOVE
#undef TMU_RENAME
#undef TMU_GETCWD
#undef TMU_FOPEN_READ
#undef TMU_FOPEN_WRITE

#endif /* defined(TMU_IMPLEMENT_CRT) */

static void tmu_to_tmu_path(struct tmu_contents_struct* path, tm_bool is_dir) {
    TM_ASSERT(path);
    TM_ASSERT((path->data && path->capacity > 0) || (!path->data && path->capacity == 0));

#ifdef _WIN32
    /* On Windows we want to replace \ by / to stay consistent with Unix. */
    for (char* str = path->data; *str; ++str) {
        if (*str == '\\') *str = '/';
    }
#endif

    if (is_dir && path->size > 0) {
        /* Append '/' at the end of the directory path if it doesn't exist. */
        if (path->data[path->size - 1] != '/') {
            TM_ASSERT(path->size + 1 < path->capacity);
            path->data[path->size++] = '/';
        }
    }

    /* Nullterminate. */
    TM_ASSERT(path->size < path->capacity);
    path->data[path->size] = 0;
}

void tmu_destroy_platform_path(tmu_platform_path* path) {
    if (path) {
        if (path->path && path->allocated_size > 0) {
            TM_ASSERT(path->path != path->sbo);
            // Cast away const-ness, since we know that path was allocated.
            TMU_FREE(((void*)path->path), path->allocated_size * sizeof(tmu_tchar), sizeof(tmu_tchar));
        }
        path->path = TM_NULL;
        path->allocated_size = 0;
    }
}

#if defined(__cplusplus) && defined(TM_STRING_VIEW)
tmu_contents::operator TM_STRING_VIEW() const { return TM_STRING_VIEW_MAKE(data, size); }

static tm_bool tmu_to_platform_path(TM_STRING_VIEW str, tmu_platform_path* out) {
    return tmu_to_platform_path_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str), out);
}
#endif /* defined(__cplusplus) && defined(TM_STRING_VIEW) */

#ifdef TMU_USE_CRT
TMU_DEF FILE* tmu_fopen(const char* filename, const char* mode) {
    FILE* f = TM_NULL;
    tmu_platform_path platform_filename;
    platform_filename.path = TM_NULL;
    tmu_platform_path platform_mode;
    platform_mode.path = TM_NULL;
    if (tmu_to_platform_path(filename, &platform_filename) && tmu_to_platform_path(mode, &platform_mode)) {
        f = tmu_fopen_t(platform_filename.path, platform_mode.path);
    }
    tmu_destroy_platform_path(&platform_filename);
    tmu_destroy_platform_path(&platform_mode);
    return f;
}
TMU_DEF FILE* tmu_freopen(const char* filename, const char* mode, FILE* current) {
    FILE* f = TM_NULL;
    tmu_platform_path platform_filename;
    platform_filename.path = TM_NULL;
    tmu_platform_path platform_mode;
    platform_mode.path = TM_NULL;
    if (tmu_to_platform_path(filename, &platform_filename) && tmu_to_platform_path(mode, &platform_mode)) {
        f = tmu_freopen_t(platform_filename.path, platform_mode.path, current);
    } else {
        if (current) fclose(current);
    }
    tmu_destroy_platform_path(&platform_filename);
    tmu_destroy_platform_path(&platform_mode);
    return f;
}
#if defined(__cplusplus) && defined(TM_STRING_VIEW)
TMU_DEF FILE* tmu_fopen(TM_STRING_VIEW filename, TM_STRING_VIEW mode) {
    FILE* f = TM_NULL;
    tmu_platform_path platform_filename;
    platform_filename.path = TM_NULL;
    tmu_platform_path platform_mode;
    platform_mode.path = TM_NULL;
    if (tmu_to_platform_path(filename, &platform_filename) && tmu_to_platform_path(mode, &platform_mode)) {
        f = tmu_fopen_t(platform_filename.path, platform_mode.path);
    }
    tmu_destroy_platform_path(&platform_filename);
    tmu_destroy_platform_path(&platform_mode);
    return f;
}
TMU_DEF FILE* tmu_freopen(TM_STRING_VIEW filename, TM_STRING_VIEW mode, FILE* current) {
    FILE* f = TM_NULL;
    tmu_platform_path platform_filename;
    platform_filename.path = TM_NULL;
    tmu_platform_path platform_mode;
    platform_mode.path = TM_NULL;
    if (tmu_to_platform_path(filename, &platform_filename) && tmu_to_platform_path(mode, &platform_mode)) {
        f = tmu_freopen_t(platform_filename.path, platform_mode.path, current);
    } else {
        if (current) fclose(current);
    }
    tmu_destroy_platform_path(&platform_filename);
    tmu_destroy_platform_path(&platform_mode);
    return f;
}
#endif /* defined(__cplusplus) && defined(TM_STRING_VIEW) */
#endif /*defined(TMU_USE_CRT)*/

static tm_size_t tmu_get_path_len_internal(const tmu_tchar* filename, tm_size_t filename_len) {
    tm_size_t dir_len = (filename_len == 0) ? (tm_size_t)TMU_TEXTLEN(filename) : filename_len;
    while (dir_len > 0 && filename[dir_len] != TMU_TEXT('/') && filename[dir_len] != TMU_TEXT('\\') &&
           filename[dir_len] != TMU_TEXT(':') && filename[dir_len] != TMU_TEXT('~')) {
        --dir_len;
    }
    return dir_len;
}

TMU_DEF tm_bool tmu_grow_by(tmu_contents* contents, tm_size_t amount) {
    TM_ASSERT(contents);
    TM_ASSERT_VALID_SIZE(contents->size);
    TM_ASSERT(contents->size <= contents->capacity);

    if ((contents->capacity - contents->size) >= amount) return TM_TRUE;

    tm_size_t new_capacity = contents->capacity + (contents->capacity / 2);
    if (new_capacity < contents->size + amount) new_capacity = contents->size + amount;
    char* new_data = (char*)TMU_REALLOC(contents->data, contents->capacity * sizeof(char), sizeof(char),
                                        new_capacity * sizeof(char), sizeof(char));
    if (!new_data) return TM_FALSE;

    contents->data = new_data;
    contents->capacity = new_capacity;
    return TM_TRUE;
}

static tmu_tchar* tmu_to_platform_path_t(const tmu_tchar* path, tm_size_t size, tmu_platform_path* out) {
    TM_ASSERT(out);
    tmu_tchar* buffer = TM_NULL;
    if (size < TMU_SBO_SIZE) {
        buffer = out->sbo;
        out->allocated_size = 0;
    } else {
        buffer = (tmu_tchar*)TMU_MALLOC((size + 1) * sizeof(tmu_tchar), sizeof(tmu_tchar));
        if (!buffer) return TM_NULL;
        out->allocated_size = size + 1;
    }

    TMU_MEMCPY(buffer, path, size * sizeof(tmu_tchar));
    buffer[size] = 0;
    out->path = buffer;
    return buffer;
}

static tm_errc tmu_create_directory_internal(const tmu_tchar* dir, tm_size_t dir_len) {
    if (dir_len <= 0) return TM_OK;
    if (dir_len == 1 && dir[0] == TMU_DIR_DELIM) return TM_OK;
    if (dir_len == 2 && (dir[0] == TMU_TEXT('.') || dir[0] == TMU_TEXT('~')) && dir[1] == TMU_DIR_DELIM) return TM_OK;

    tmu_platform_path platform_dir_buffer;
    tmu_tchar* path = tmu_to_platform_path_t(dir, dir_len, &platform_dir_buffer);
    if (!path) return TM_ENOMEM;

    if (tmu_directory_exists_t(path).exists) {
        tmu_destroy_platform_path(&platform_dir_buffer);
        return TM_OK;
    }

    /* Create directory tree recursively. */
    tm_errc result = TM_OK;
    tmu_tchar* end = TMU_TEXTCHR(path, TMU_DIR_DELIM);
    for (;;) {
        tm_bool was_null = (end == TM_NULL);
        if (!was_null) *end = 0;

        result = tmu_create_single_directory_t(path);
        if (result != TM_OK) break;

        if (was_null || *(end + 1) == 0) break;
        *end = TMU_DIR_DELIM;
        end = TMU_TEXTCHR(end + 1, TMU_DIR_DELIM);
    }
    tmu_destroy_platform_path(&platform_dir_buffer);
    return result;
}

static tm_errc tmu_create_directory_t(const tmu_tchar* dir) {
    return tmu_create_directory_internal(dir, (tm_size_t)TMU_TEXTLEN(dir));
}

TMU_DEF int tmu_compare_file_time(tmu_file_time a, tmu_file_time b) {
    int64_t cmp = (int64_t)(a - b);
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}

TMU_DEF void tmu_destroy_contents(tmu_contents* contents) {
    if (contents) {
        if (contents->data) {
            TM_ASSERT(contents->capacity > 0);
            TMU_FREE(contents->data, contents->capacity * sizeof(char), sizeof(char));
        }
        contents->data = TM_NULL;
        contents->size = 0;
        contents->capacity = 0;
    }
}

TMU_DEF tmu_exists_result tmu_file_exists(const char* filename) {
    tmu_exists_result result = {TM_FALSE, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_file_exists_t(platform_filename.path);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}
TMU_DEF tmu_exists_result tmu_directory_exists(const char* dir) {
    tmu_exists_result result = {TM_FALSE, TM_ENOMEM};
    tmu_platform_path platform_dir;
    if (tmu_to_platform_path(dir, &platform_dir)) {
        result = tmu_directory_exists_t(platform_dir.path);
        tmu_destroy_platform_path(&platform_dir);
    }
    return result;
}
TMU_DEF tmu_file_timestamp_result tmu_file_timestamp(const char* filename) {
    tmu_file_timestamp_result result = {0, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_file_timestamp_t(platform_filename.path);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}

TMU_DEF tm_errc tmu_create_directory(const char* dir) {
    tm_errc result = TM_ENOMEM;
    tmu_platform_path platform_dir;
    if (tmu_to_platform_path(dir, &platform_dir)) {
        result = tmu_create_directory_t(platform_dir.path);
        tmu_destroy_platform_path(&platform_dir);
    }
    return result;
}
TMU_DEF tm_errc tmu_delete_directory(const char* dir) {
    tm_errc result = TM_ENOMEM;
    tmu_platform_path platform_dir;
    if (tmu_to_platform_path(dir, &platform_dir)) {
        result = tmu_delete_directory_t(platform_dir.path);
        tmu_destroy_platform_path(&platform_dir);
    }
    return result;
}

TMU_DEF tmu_contents_result tmu_read_file(const char* filename) {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_read_file_t(platform_filename.path);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}

TMU_DEF tmu_write_file_result tmu_write_file(const char* filename, const void* data, tm_size_t size) {
    return tmu_write_file_ex(filename, data, size, tmu_overwrite);
}
TMU_DEF tmu_write_file_result tmu_write_file_as_utf8(const char* filename, const char* data, tm_size_t size) {
    return tmu_write_file_as_utf8_ex(filename, data, size, tmu_overwrite | tmu_write_byte_order_mark);
}

TMU_DEF tmu_write_file_result tmu_write_file_ex(const char* filename, const void* data, tm_size_t size,
                                                uint32_t flags) {
    tmu_write_file_result result = {0, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        flags &= ~tmu_write_byte_order_mark;
        result = tmu_write_file_ex_t(platform_filename.path, data, size, flags);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}

TMU_DEF tmu_utf8_contents_result tmu_read_file_as_utf8(const char* filename) {
    return tmu_read_file_as_utf8_ex(filename, tmu_encoding_unknown, tmu_validate_error, TM_NULL);
}
TMU_DEF tmu_utf8_contents_result tmu_read_file_as_utf8_ex(const char* filename, tmu_encoding encoding,
                                                          tmu_validate validate, const char* replace_str) {
    tmu_utf8_contents_result result = {{TM_NULL, 0, 0}, TM_OK, tmu_encoding_unknown, TM_FALSE};
    tm_size_t replace_str_len = 0;
    if (validate == tmu_validate_replace &&
        (!replace_str || (replace_str_len = (tm_size_t)TMU_STRLEN(replace_str)) == 0)) {
        // Replacing with an empty string is same as skipping.
        validate = tmu_validate_skip;
    }
    tmu_contents_result file = tmu_read_file(filename);
    result.ec = file.ec;
    if (file.ec == TM_OK) {
        result = tmu_utf8_convert_from_bytes_dynamic(&file.contents, encoding, validate, replace_str, replace_str_len,
                                                     /*nullterminate=*/TM_TRUE);
        tmu_destroy_contents(&file.contents);
    }
    return result;
}
TMU_DEF tmu_write_file_result tmu_write_file_as_utf8_ex(const char* filename, const char* data, tm_size_t size,
                                                        uint32_t flags) {
    tmu_write_file_result result = {0, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_write_file_ex_t(platform_filename.path, data, size, flags);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}

TMU_DEF tm_errc tmu_rename_file(const char* from, const char* to) { return tmu_rename_file_ex(from, to, 0); }
TMU_DEF tm_errc tmu_rename_file_ex(const char* from, const char* to, uint32_t flags) {
    tm_errc result = TM_ENOMEM;
    tmu_platform_path platform_from;
    platform_from.path = TM_NULL;
    tmu_platform_path platform_to;
    platform_to.path = TM_NULL;
    if (tmu_to_platform_path(from, &platform_from) && tmu_to_platform_path(to, &platform_to)) {
        result = tmu_rename_file_ex_t(platform_from.path, platform_to.path, flags);
    }
    tmu_destroy_platform_path(&platform_from);
    tmu_destroy_platform_path(&platform_to);
    return result;
}

TMU_DEF tm_errc tmu_delete_file(const char* filename) {
    tm_errc result = TM_ENOMEM;
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_delete_file_t(platform_filename.path);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}

TMU_DEF tmu_utf8_command_line_result tmu_utf8_command_line_from_utf16(tmu_char16 const* const* utf16_args,
                                                                      int utf16_args_count) {
    TM_ASSERT(utf16_args_count >= 0);
    TM_ASSERT(utf16_args || utf16_args_count == 0);

    tmu_utf8_command_line_result result = {{TM_NULL, 0, TM_NULL, 0}, TM_OK};

    /* Calculate necessary buffer size. */
    tm_size_t buffer_size = 0;

    const tm_size_t args_array_size = (tm_size_t)((utf16_args_count + 1) * sizeof(const char*));
    buffer_size += args_array_size;

    for (int i = 0; i < utf16_args_count; ++i) {
        tmu_conversion_result conversion =
            tmu_utf8_from_utf16_ex(tmu_utf16_make_stream(utf16_args[i]), tmu_validate_error, /*replace_str=*/TM_NULL,
                                   /*replace_str_len=*/0, /*nullterminate=*/TM_TRUE, /*out=*/TM_NULL, /*out_len=*/0);
        if (conversion.ec != TM_ERANGE) {
            result.ec = conversion.ec;
            break;
        }
        buffer_size += conversion.size * sizeof(char);
    }
    buffer_size += sizeof(char); /* Final null-terminator, since *args[args_count] has to be guaranteed 0. */

    /* Allocate buffer. */
    char* buffer = TM_NULL;
    if (result.ec == TM_OK) {
        TM_ASSERT(buffer_size > 0);

        buffer = (char*)TMU_MALLOC(buffer_size, sizeof(const char*));
        if (!buffer) result.ec = TM_ENOMEM;
    }

    /* Partition buffer to get an array to strings and a string pool for the individual arguments. */
    char const** args = (char const**)buffer;
    char* string_pool = buffer + args_array_size;
    tm_size_t string_pool_size = buffer_size - args_array_size;

    /* Convert args one by one. */
    if (result.ec == TM_OK) {
        TM_ASSERT(string_pool && string_pool_size > 0);

        char const** current_arg = args;
        char* current = string_pool;
        tm_size_t remaining = string_pool_size;
        for (int i = 0; i < utf16_args_count; ++i) {
            tmu_conversion_result conversion = tmu_utf8_from_utf16_ex(
                tmu_utf16_make_stream(utf16_args[i]), tmu_validate_error, /*replace_str=*/TM_NULL,
                /*replace_str_len=*/0, /*nullterminate=*/TM_TRUE, current, remaining);
            if (conversion.ec != TM_OK) {
                result.ec = conversion.ec;
                break;
            }
            *current_arg++ = current;

            TM_ASSERT(conversion.size + 1 <= remaining);
            current += conversion.size + 1;
            remaining -= conversion.size + 1;
            if (remaining <= 0) {
                result.ec = TM_ERANGE;
                break;
            }
        }

        if (result.ec == TM_OK) {
            if (remaining <= 0) {
                result.ec = TM_ERANGE;
            } else {
                /* Final null-terminator, so that *args[args_count] == 0.*/
                *current_arg = current;
                *current++ = 0;
                --remaining;
                TM_ASSERT(remaining == 0);
            }
        }
    }

    if (result.ec == TM_OK) {
        result.command_line.args = args;
        result.command_line.args_count = utf16_args_count;

        result.command_line.internal_buffer = buffer;
        result.command_line.internal_allocated_size = buffer_size;
    }

    /* Free resources if anything went wrong. */
    if (result.ec != TM_OK) {
        if (buffer) {
            TM_ASSERT(buffer_size > 0);
            TMU_FREE(buffer, buffer_size * sizeof(char), sizeof(char));
            buffer = TM_NULL;
            buffer_size = 0;
        }
    }

    return result;
}
TMU_DEF void tmu_utf8_destroy_command_line(tmu_utf8_command_line* command_line) {
    if (command_line) {
        if (command_line->internal_buffer) {
            TM_ASSERT(command_line->internal_allocated_size > 0);
            TMU_FREE(command_line->internal_buffer, command_line->internal_allocated_size * sizeof(char), sizeof(char));
        }
        command_line->args = TM_NULL;
        command_line->args_count = 0;
        command_line->internal_buffer = TM_NULL;
        command_line->internal_allocated_size = 0;
    }
}

#if defined(__cplusplus)

static tmu_contents_managed_result tmu_to_managed_result(tmu_contents_result in) {
    tmu_contents_managed_result result;
    result.contents.data = in.contents.data;
    result.contents.size = in.contents.size;
    result.contents.capacity = in.contents.capacity;
    result.ec = in.ec;
    return result;
}
static tmu_utf8_managed_result tmu_to_utf8_managed_result(tmu_utf8_contents_result in) {
    tmu_utf8_managed_result result;
    result.contents.data = in.contents.data;
    result.contents.size = in.contents.size;
    result.contents.capacity = in.contents.capacity;
    result.ec = in.ec;
    result.original_encoding = in.original_encoding;
    result.invalid_codepoints_encountered = in.invalid_codepoints_encountered;
    return result;
}

TMU_DEF tmu_contents_managed_result tmu_current_working_directory_managed(tm_size_t extra_size) {
    return tmu_to_managed_result(tmu_current_working_directory(extra_size));
}

#if defined(TM_STRING_VIEW)

TMU_DEF tmu_exists_result tmu_file_exists(TM_STRING_VIEW filename) {
    tmu_exists_result result = {TM_FALSE, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_file_exists_t(platform_filename.path);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}
TMU_DEF tmu_exists_result tmu_directory_exists(TM_STRING_VIEW dir) {
    tmu_exists_result result = {TM_FALSE, TM_ENOMEM};
    tmu_platform_path platform_dir;
    if (tmu_to_platform_path(dir, &platform_dir)) {
        result = tmu_directory_exists_t(platform_dir.path);
        tmu_destroy_platform_path(&platform_dir);
    }
    return result;
}
TMU_DEF tmu_file_timestamp_result tmu_file_timestamp(TM_STRING_VIEW filename) {
    tmu_file_timestamp_result result = {0, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_file_timestamp_t(platform_filename.path);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}
TMU_DEF tmu_contents_result tmu_read_file(TM_STRING_VIEW filename) {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_read_file_t(platform_filename.path);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}
TMU_DEF tmu_write_file_result tmu_write_file(TM_STRING_VIEW filename, const void* data, tm_size_t size) {
    return tmu_write_file_ex(filename, data, size, tmu_overwrite);
}
TMU_DEF tmu_write_file_result tmu_write_file_ex(TM_STRING_VIEW filename, const void* data, tm_size_t size,
                                                uint32_t flags) {
    tmu_write_file_result result = {0, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        flags &= ~tmu_write_byte_order_mark;
        result = tmu_write_file_ex_t(platform_filename.path, data, size, flags);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}

TMU_DEF tmu_utf8_contents_result tmu_read_file_as_utf8(TM_STRING_VIEW filename) {
    return tmu_read_file_as_utf8_ex(filename, tmu_encoding_unknown, tmu_validate_error, TM_STRING_VIEW{});
}
TMU_DEF tmu_utf8_contents_result tmu_read_file_as_utf8_ex(TM_STRING_VIEW filename, tmu_encoding encoding,
                                                          tmu_validate validate, TM_STRING_VIEW replace_str) {
    tmu_utf8_contents_result result = {{TM_NULL, 0, 0}, TM_OK, tmu_encoding_unknown, TM_FALSE};
    if (validate == tmu_validate_replace && TM_STRING_VIEW_SIZE(replace_str) == 0) {
        // Replacing with an empty string is same as skipping.
        validate = tmu_validate_skip;
    }
    tmu_contents_result file = tmu_read_file(filename);
    result.ec = file.ec;
    if (file.ec == TM_OK) {
        result =
            tmu_utf8_convert_from_bytes_dynamic(&file.contents, encoding, validate, TM_STRING_VIEW_DATA(replace_str),
                                                (tm_size_t)TM_STRING_VIEW_SIZE(replace_str), /*nullterminate=*/TM_TRUE);
        tmu_destroy_contents(&file.contents);
    }
    return result;
}
TMU_DEF tmu_write_file_result tmu_write_file_as_utf8(TM_STRING_VIEW filename, const char* data, tm_size_t size) {
    return tmu_write_file_as_utf8_ex(filename, data, size, tmu_overwrite | tmu_write_byte_order_mark);
}
TMU_DEF tmu_write_file_result tmu_write_file_as_utf8_ex(TM_STRING_VIEW filename, const char* data, tm_size_t size,
                                                        uint32_t flags) {
    tmu_write_file_result result = {0, TM_ENOMEM};
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_write_file_ex_t(platform_filename.path, data, size, flags);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}

TMU_DEF tm_errc tmu_rename_file(TM_STRING_VIEW from, TM_STRING_VIEW to) { return tmu_rename_file_ex(from, to, 0); }
TMU_DEF tm_errc tmu_rename_file_ex(TM_STRING_VIEW from, TM_STRING_VIEW to, uint32_t flags) {
    tm_errc result = TM_ENOMEM;
    tmu_platform_path platform_from;
    platform_from.path = TM_NULL;
    tmu_platform_path platform_to;
    platform_to.path = TM_NULL;
    if (tmu_to_platform_path(from, &platform_from) && tmu_to_platform_path(to, &platform_to)) {
        result = tmu_rename_file_ex_t(platform_from.path, platform_to.path, flags);
    }
    tmu_destroy_platform_path(&platform_from);
    tmu_destroy_platform_path(&platform_to);
    return result;
}

TMU_DEF tm_errc tmu_delete_file(TM_STRING_VIEW filename) {
    tm_errc result = TM_ENOMEM;
    tmu_platform_path platform_filename;
    if (tmu_to_platform_path(filename, &platform_filename)) {
        result = tmu_delete_file_t(platform_filename.path);
        tmu_destroy_platform_path(&platform_filename);
    }
    return result;
}

TMU_DEF tmu_contents_managed_result tmu_read_file_managed(TM_STRING_VIEW filename) {
    return tmu_to_managed_result(tmu_read_file(filename));
}

TMU_DEF tmu_utf8_managed_result tmu_read_file_as_utf8_managed(TM_STRING_VIEW filename) {
    return tmu_to_utf8_managed_result(tmu_read_file_as_utf8(filename));
}
TMU_DEF tmu_utf8_managed_result tmu_read_file_as_utf8_managed_ex(TM_STRING_VIEW filename, tmu_encoding encoding,
                                                                 tmu_validate validate, TM_STRING_VIEW replace_str) {
    return tmu_to_utf8_managed_result(tmu_read_file_as_utf8_ex(filename, encoding, validate, replace_str));
}

TMU_DEF tm_errc tmu_create_directory(TM_STRING_VIEW dir) {
    tm_errc result = TM_ENOMEM;
    tmu_platform_path platform_dir;
    if (tmu_to_platform_path(dir, &platform_dir)) {
        result = tmu_create_directory_t(platform_dir.path);
        tmu_destroy_platform_path(&platform_dir);
    }
    return result;
}
TMU_DEF tm_errc tmu_delete_directory(TM_STRING_VIEW dir) {
    tm_errc result = TM_ENOMEM;
    tmu_platform_path platform_dir;
    if (tmu_to_platform_path(dir, &platform_dir)) {
        result = tmu_delete_directory_t(platform_dir.path);
        tmu_destroy_platform_path(&platform_dir);
    }
    return result;
}

#endif /* defined(TM_STRING_VIEW) */

tmu_contents_managed::tmu_contents_managed() {
    data = nullptr;
    size = 0;
    capacity = 0;
}
tmu_contents_managed::tmu_contents_managed(tmu_contents_managed&& other) {
    static_cast<tmu_contents&>(*this) = static_cast<tmu_contents&>(other);
    static_cast<tmu_contents&>(other) = {TM_NULL, 0, 0};
}
tmu_contents_managed& tmu_contents_managed::operator=(tmu_contents_managed&& other) {
    if (this != &other) {
        /* Swap contents and let destructor of other release memory. */
        tmu_contents temp = static_cast<tmu_contents&>(*this);
        static_cast<tmu_contents&>(*this) = static_cast<tmu_contents&>(other);
        static_cast<tmu_contents&>(other) = temp;
    }
    return *this;
}
tmu_contents_managed::~tmu_contents_managed() { tmu_destroy_contents(this); }

TMU_DEF tmu_contents_managed_result tmu_read_file_managed(const char* filename) {
    return tmu_to_managed_result(tmu_read_file(filename));
}

TMU_DEF tmu_utf8_managed_result tmu_read_file_as_utf8_managed(const char* filename) {
    return tmu_to_utf8_managed_result(tmu_read_file_as_utf8(filename));
}
TMU_DEF tmu_utf8_managed_result tmu_read_file_as_utf8_managed_ex(const char* filename, tmu_encoding encoding,
                                                                 tmu_validate validate, const char* replace_str) {
    return tmu_to_utf8_managed_result(tmu_read_file_as_utf8_ex(filename, encoding, validate, replace_str));
}

tmu_utf8_command_line_managed::tmu_utf8_command_line_managed() {
    args = nullptr;
    args_count = 0;
    internal_buffer = nullptr;
    internal_allocated_size = 0;
}
tmu_utf8_command_line_managed::tmu_utf8_command_line_managed(tmu_utf8_command_line_managed&& other) {
    static_cast<tmu_utf8_command_line&>(*this) = static_cast<tmu_utf8_command_line&>(other);
    static_cast<tmu_utf8_command_line&>(other) = {nullptr, 0, nullptr, 0};
}
tmu_utf8_command_line_managed& tmu_utf8_command_line_managed::operator=(tmu_utf8_command_line_managed&& other) {
    if (this != &other) {
        /* Swap contents and let destructor of other release memory. */
        tmu_utf8_command_line temp = static_cast<tmu_utf8_command_line&>(*this);
        static_cast<tmu_utf8_command_line&>(*this) = static_cast<tmu_utf8_command_line&>(other);
        static_cast<tmu_utf8_command_line&>(other) = temp;
    }
    return *this;
}
tmu_utf8_command_line_managed::~tmu_utf8_command_line_managed() { tmu_utf8_destroy_command_line(this); }

tmu_utf8_command_line_managed_result tmu_utf8_command_line_from_utf16_managed(tmu_char16 const* const* utf16_args,
                                                                              int utf16_args_count) {
    tmu_utf8_command_line_managed_result result;
    tmu_utf8_command_line_result unmanaged = tmu_utf8_command_line_from_utf16(utf16_args, utf16_args_count);
    static_cast<tmu_utf8_command_line&>(result.command_line) = unmanaged.command_line;
    result.ec = unmanaged.ec;
    return result;
}

#if defined(TMU_USE_STL)
TMU_DEF std::vector<char> tmu_read_file_to_vector(const char* filename) {
    std::vector<char> result;
    tmu_contents_managed_result file = tmu_read_file_managed(filename);
    if (file.ec == TM_OK && file.contents.size > 0) {
        result.assign(file.contents.data, file.contents.data + file.contents.size);
    }
    return result;
}
TMU_DEF std::vector<char> tmu_read_file_as_utf8_to_vector(const char* filename) {
    std::vector<char> result;
    tmu_utf8_managed_result file = tmu_read_file_as_utf8_managed(filename);
    if (file.ec == TM_OK && file.contents.size > 0) {
        result.assign(file.contents.data, file.contents.data + file.contents.size);
    }
    return result;
}
#if defined(TM_STRING_VIEW)
TMU_DEF std::vector<char> tmu_read_file_to_vector(TM_STRING_VIEW filename) {
    std::vector<char> result;
    tmu_contents_managed_result file = tmu_read_file_managed(filename);
    if (file.ec == TM_OK && file.contents.size > 0) {
        result.assign(file.contents.data, file.contents.data + file.contents.size);
    }
    return result;
}
TMU_DEF std::vector<char> tmu_read_file_as_utf8_to_vector(TM_STRING_VIEW filename) {
    std::vector<char> result;
    tmu_utf8_managed_result file = tmu_read_file_as_utf8_managed(filename);
    if (file.ec == TM_OK && file.contents.size > 0) {
        result.assign(file.contents.data, file.contents.data + file.contents.size);
    }
    return result;
}
#endif /* defined(TM_STRING_VIEW) */

#endif /* defined(TMU_USE_STL) */

#endif /* defined(__cplusplus) */

#undef TMU_TEXT
#undef TMU_DIR_DELIM
#undef TMU_TEXTLEN
#undef TMU_TEXTCHR

#endif /* !defined(TMU_NO_FILE_IO) */

#endif /* defined(TM_UNICODE_IMPLEMENTATION) */

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
