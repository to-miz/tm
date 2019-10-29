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
ec to ERANGE. The 'necessary' field can then be used to allocate a sufficiently large buffer.
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
