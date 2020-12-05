/*
tm_xml.h v0.1.0 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2020

No warranty; Use at your own risk.

First release not hit yet. Probably very buggy.

LICENSE
    See license notes at end of file.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_XML_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

DESCRIPTION
    A simple xml reader with no extra validation. Reads an xml document token by token.
    It does lexical validation, but will not make sure, that closed-tags correspond to opened-tags.
    That should be done by the code that consumes an xml document using this reader.

NOTES
    Although this reader was written using the specifications from https://www.w3.org/TR/xml/
    it is non-conforming in the following categories:
        - Namespaces have to be resolved by the code that consumes an xml document using this reader.
        - No support for builtin attributes.
        - No support for <!DOCTYPE ...>, <!ELEMENT ..> etc. tags. They will just be discarded without any validity
          checks (not even checks for valid unicode encoding).
        - Conforms only to XML 1.1, not XML 1.0.
        - Processing instructions <?...?> are not validated and passed as is. Only unicode validity is checked.
        - When an unknown entity in the form of "&custom_entity;" is encoutered, it is left as is by the unescaping
          routines. This can happen when custom entities are used with <!ENTITY ...>, which we don't support.

ISSUES
    - Error reporting is not very robust. You get the location of the error, but no error description currently.
    - Only utf-8 xml documents are supported.
    - Unicode whitespace that is not ASCII is not supported, especially Next Line, U+0085 is not treated as Whitespace.
    - Unicode validation is only done according to XML 1.1 spec, meaning that some
      "invalid" XML 1.0 documents will be accepted, since XML 1.1 is more lenient.

HISTORY      (DD.MM.YY)
    v0.1.0    07.10.18 Initial commit.
*/

/* This is a generated file, do not modify directly. You can find the generator files in the src directory. */

// clang-format off

/* dependencies */
#ifdef TM_XML_IMPLEMENTATION
    #ifndef TM_ASSERT
        #include <assert.h>
        #define TM_ASSERT assert
    #endif

    #if !defined(TM_MEMSET) || !defined(TM_STRCHR) || !defined(TM_STRCSPN) || !defined(TM_STRSPN) \
        || !defined(TM_STRNCMP) || !defined(TM_STRSTR) || !defined(TM_MEMCHR) || !defined(TM_MEMCMP) \
        || !defined(TM_MEMCPY)

        #include <string.h>
        #ifndef TM_MEMSET
            #define TM_MEMSET memset
        #endif
        #ifndef TM_MEMCMP
            #define TM_MEMCMP memcmp
        #endif
        #ifndef TM_MEMCPY
            #define TM_MEMCPY memcpy
        #endif
        #ifndef TM_STRCHR
            #define TM_STRCHR strchr
        #endif
        #ifndef TM_STRCSPN
            #define TM_STRCSPN strcspn
        #endif
        #ifndef TM_STRSPN
            #define TM_STRSPN strspn
        #endif
        #ifndef TM_STRNCMP
            #define TM_STRNCMP strncmp
        #endif
        #ifndef TM_STRSTR
            #define TM_STRSTR strstr
        #endif
        #ifndef TM_MEMCHR
            #define TM_MEMCHR memchr
        #endif
    #endif

    #if !defined(TM_STRTOUL)
        #include <stdlib.h>
        #ifndef TM_STRTOUL
            #define TM_STRTOUL strtoul
        #endif
    #endif

    /* Global allocation functions to use. */
    #if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
        // Either all or none have to be defined.
        #include <stdlib.h>
        #define TM_MALLOC(size, alignment) malloc((size))
        #define TM_REALLOC(ptr, new_size, new_alignment) realloc((ptr), (new_size))
        // #define TM_REALLOC_IN_PLACE(ptr, new_size, new_alignment) // Optional
        #define TM_FREE(ptr) free((ptr))
        // Define as 1 if alignment parameter is actually respected.
        #define TM_MALLOC_ALIGNMENT_AWARE 0
    #endif
#endif
// clang-format on

#ifndef TM_XML_H_INLCUDED_46276729_6E96_4F9F_8DAF_FF70E27D3A34
#define TM_XML_H_INLCUDED_46276729_6E96_4F9F_8DAF_FF70E27D3A34

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

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

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* Define to 1 if tm_size_t is signed. */
    #include <stddef.h> /* Include C version so identifiers are in global namespace. */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

#ifndef TMXML_DEF
#define TMXML_DEF extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief A stringview structure that keeps a non nullterminated view into a string.
 */
typedef struct {
    const char* data;  //!< Can be NULL if size == 0.
    tm_size_t size;
} tmxml_stringview;

typedef struct {
    uint32_t line;    //!< Current line in the document, zero-based.
    uint32_t column;  //!< Current column in the document, zero-based.
    uint32_t offset;  //!< Byte-offset from the beginning of the document, zero-based.
    uint32_t length;  //!< Length of the current token that this location corresponds to.
} tmxml_location;

struct tmxml_tokenizer {
    const char* current;      //!< Nullterminated xml document.
    tmxml_location location;  //!< Current location inside the xml document.
};

typedef enum {
    tmxml_tok_eof,    //!< End of file token.
    tmxml_tok_error,  //!< Denotes that an error occured.

    tmxml_tok_tag_open,   //!< Tag open ie "<tag_name ...", the token contents will have the name of the tag.
    tmxml_tok_tag_close,  //!< Tag close ie "</tag_name>" or "<tag_name />". Tags without content will generate both
                          //!< tmxml_tok_tag_open and tmxml_tok_tag_close. For reading purposes "<tag_name />" is the
                          //!< same as "<tag_name></tagname>".
    tmxml_tok_attribute_name,           //!< Attribute name ie "<tag_name attribute_name='attribute_value'".
    tmxml_tok_attribute_value,          //!< Attribute value ie "<tag_name attribute_name='attribute_value'".
    tmxml_tok_contents,                 //!< Contents of a tag ie "<tag_name>contants</tag_name>".
    tmxml_tok_contents_cdata,           //!< Contents of a cdata section ie "<![CDATA[ unescaped data ]]>".
    tmxml_tok_comment,                  //!< Contents of a comment, ie "<!-- comment -->".
    tmxml_tok_processing_instructions,  //!< The token contents will have all the contents between <? and ?>
                                        //!< unchanged/unparsed.
} tmxml_token_type;

/*!
 * @brief The xml token to be inspected.
 */
typedef struct {
    tmxml_token_type type;
    tmxml_stringview contents;  //!< The memory of this stringview is managed by tmxml_reader. Do not keep references to
                                //!< this, use tmxml_copy_string instead.
    tmxml_location location;    //!< Location of the token in the xml document.
} tmxml_token;

typedef enum {
    tmxml_rc_none,
    tmxml_rc_eof,
    tmxml_rc_error,
    tmxml_rc_inside_tag,
    tmxml_rc_inside_tag_attribute_name,
    tmxml_rc_content,
    tmxml_rc_post_root,
} tmxml_read_context;

/*!
 * @brief The xml reader. Create using tmxml_make_reader and destroy using tmxml_destroy_reader.
 * To read an xml document, first call tmxml_read_prolog to read the <?xml version="..."?> prolog.
 * Then call tmxml_next_token in a loop as long as it returns @c true and inspect tmxml_reader::current.
 */
typedef struct {
    struct tmxml_tokenizer tokenizer;
    tmxml_token current;  //!< Current token. You MUST NOT take a reference to the contents of the token, since that
                          //!< memory is managed by this reader and will change upon the next call to tmxml_next_token.
                          //!< Use tmxml_copy_string to copy it to a new buffer allocated using malloc.
    unsigned int flags;   //!< Processing flags of content/attribute values.

    tmxml_read_context context;    //!< The parsing context.
    tm_size_t tags_depth;          //!< How many levels of nested tags we currently are in.
    tmxml_stringview current_tag;  //!< If context is tmxml_rc_inside_tag, which tag we currently are inside.
    tm_bool root_found;            //!< Whether the root tag was found yet.

    // Internal memory to initialize the token. The memory is managed by this reader, which is why you should not keep
    // references to the token.
    char sbo[64];
    char* buffer;
    tm_size_t buffer_size;
} tmxml_reader;

typedef struct {
    tmxml_stringview version;
    tmxml_stringview encoding;     //!< Only utf-8 is supported.
    tm_bool standalone_specified;  //!< Whether the standalone attribute was specified.
    tm_bool standalone;            //!< The value of the standalone attribute, if it was specified.
    tm_bool present;               //!< Whether the prolog was present or not.
    tm_bool skipped_doctype;       //!< Whether DOCTYPE was found and skipped.
} tmxml_prolog;

/*!
 * @brief Processing flags to use with tmxml_make_reader.
 */
enum {
    tmxml_flags_trim_left = (1u << 0),                   //!< Turns string like "   Hello" into "Hello".
    tmxml_flags_trim_right = (1u << 1),                  //!< Turns string like "Hello    " into "Hello".
    tmxml_flags_trim_reduce_spaces = (1u << 2),          //!< Turns strings like "Hello    World!" into "Hello World!".
    tmxml_flags_trim_trim_newlines = (1u << 3),          //!< Turns strings like "Hello\nWorld!" into "Hello World!".
    tmxml_flags_trim_trim_cdata = (1u << 4),             //!< Also trim cdata sections.
    tmxml_flags_trim_trim_attribute_values = (1u << 5),  //!< Also trim of attribute values.

    tmxml_flags_trim_all = tmxml_flags_trim_left | tmxml_flags_trim_right | tmxml_flags_trim_reduce_spaces
                           | tmxml_flags_trim_trim_newlines | tmxml_flags_trim_trim_cdata
                           | tmxml_flags_trim_trim_attribute_values
};

/*!
 * @brief Creates a XML Reader from a UTF-8 nullterminated string.
 * @param nullterminated_data[IN] Pointer to a nullterminated string that containts an xml document.
 * Lifetime must be greater than the returned reader. The reader does not copy the input data.
 * @return The initialized reader.
 */
TMXML_DEF tmxml_reader tmxml_make_reader(const char* nullterminated_data, uint32_t flags);
TMXML_DEF void tmxml_destroy_reader(tmxml_reader* reader);

/*!
 * @brief Reads the xml prolog, if it exists. Check out->illformed to see, if there was an error.
 *
 * Note that a prolog must be the very first thing in an xml document (no whitespace allowed beforehand).
 *
 * @return @c true if the xml prolog was parsed correctly or was not present, @c false if a prolog was present but
 * illformed.
 */
TMXML_DEF tm_bool tmxml_read_prolog(tmxml_reader* reader, tmxml_prolog* out);

/*!
 * @brief Advance the reader to the next token. Call this in a loop until it returns @c false.
 * @param reader[IN,OUT] The xml reader to advance. Afterwards reader->current has the current token.
 * Do not keep references to reader->current, since its internal memory will be invalidated by the next
 * call to tmxml_next_token. Use tmxml_copy_string to copy its content or copy by another means.
 * @return @c true if a token is ready to be inspected, @c false otherwise.
 */
TMXML_DEF tm_bool tmxml_next_token(tmxml_reader* reader);

/*!
 * @brief Copies a stringview into a new nullterminated buffer that was allocated using malloc.
 * Use free to free the memory.
 *
 * Note that empty stringviews will still allocate a single byte (the nullterminator).
 * @return Pointer to the allocated nullterminated string, or @c NULL if out of memory.
 */
TMXML_DEF char* tmxml_copy_string(tmxml_stringview str);

TMXML_DEF tm_bool tmxml_unescape_current_token(tmxml_reader* reader);

/*!
 * @brief Unescapes and processes a string with the given flags. Resolves xml escapes like &quot; into '"' etc.
 * If the given buffer is not big enough, the size in out_written is the needed buffer size including the null
 * terminator.
 *
 * @param str[IN] The escaped input string.
 * @param len[IN] The length of the input string.
 * @param flags[IN] The reader flags.
 * @param out[OUT] The output buffer. Can be NULL if out_len == 0. The output will be nullterminated if the buffer is
 * big enough. In that case out_written will be less than out_len.
 * @param out_len[IN] The size of the output buffer.
 * @param out_written[OUT] The amount of bytes written or the needed buffer size of the buffer was not big enough.
 * Data was successfully written to out, if *out_written < out_len, otherwise buffer was not big enough.
 * Use the value of out_written in that case to allocate a buffer that is big enough.
 * MUST NOT be NULL.
 * @return @c true if there were no errors in the input string, @c false if there were errors.
 */
TMXML_DEF tm_bool tmxml_unescape_string(const char* str, tm_size_t len, unsigned int flags, char* out,
                                        tm_size_t out_len, tm_size_t* out_written);

#ifdef __cplusplus
}
#endif

#endif /* !defined(TM_XML_H_INLCUDED_46276729_6E96_4F9F_8DAF_FF70E27D3A34) */

#ifdef TM_XML_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TM_UNREFERENCED_PARAM
	#define TM_UNREFERENCED_PARAM(x) ((void)(x))
	#define TM_UNREFERENCED(x) ((void)(x))
    #define TM_MAYBE_UNUSED(x) ((void)(x))
#endif

#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

enum {
    // Some things don't need escaping inside comments.
    tmxml_internal_inside_comment = (1 << 28u)
};

TMXML_DEF tmxml_reader tmxml_make_reader(const char* nullterminated_data, uint32_t flags) {
    TM_ASSERT(nullterminated_data);

    tmxml_reader result;
    TM_MEMSET(&result, 0, sizeof(result));
    result.tokenizer.current = nullterminated_data;
    result.flags = flags;
    return result;
}

static tmxml_token tmxml_lex_parse_attribute_value(struct tmxml_tokenizer* tokenizer) {
    tmxml_token result;
    result.type = tmxml_tok_error;
    result.contents.data = tokenizer->current;
    result.contents.size = 0;
    result.location = tokenizer->location;

    if (*tokenizer->current == '"' || *tokenizer->current == '\'') {
        const char* start = tokenizer->current + 1;
        result.contents.data = start;
        result.contents.size = 1;
        result.type = tmxml_tok_attribute_value;

        char delimiters[3] = {'\n', 0, 0};
        delimiters[1] = *tokenizer->current;

        const char* next = start;
        for (;;) {
            const char* current = next;
            next = current + TM_STRCSPN(current, delimiters);
            if (*next == '\n') {
                ++next;
                tokenizer->location.column = 0;
                ++tokenizer->location.line;
                continue;
            }
            if (*next != delimiters[1]) {
                result.type = tmxml_tok_error;
            }

            result.contents.size = (tm_size_t)(next - start);
            result.location.length = (uint32_t)result.contents.size;

            if (*next == delimiters[1]) ++next;
            tokenizer->current = next;
            tokenizer->location.column += (uint32_t)(next - current);
            tokenizer->location.offset += (uint32_t)(next - start);
            break;
        }
    }

    return result;
}

static tmxml_token tmxml_lex_parse_contents(struct tmxml_tokenizer* tokenizer) {
    tmxml_token result;
    const char* start = tokenizer->current;
    result.type = tmxml_tok_contents;
    result.contents.data = start;
    result.contents.size = 1;
    result.location = tokenizer->location;

    const char* next = start;
    for (;;) {
        const char* current = next;
        next = current + TM_STRCSPN(current, "<\n");
        if (*next == '\n') {
            ++next;
            tokenizer->location.column = 0;
            ++tokenizer->location.line;
            continue;
        }
        if (!*next) result.type = tmxml_tok_error;

        tokenizer->location.column += (uint32_t)(next - current);
        tokenizer->location.offset += (uint32_t)(next - start);
        tokenizer->current = next;
        result.contents.size = (tm_size_t)(next - start);
        result.location.length = (uint32_t)result.contents.size;
        break;
    }
    return result;
}

static tm_bool tmxml_lex_skip_whitespace(struct tmxml_tokenizer* tokenizer) {
    TM_ASSERT(tokenizer);

    const char* start = tokenizer->current;
    const char* next = tokenizer->current;
    for (;;) {
        const char* current = next;
        next = current + TM_STRSPN(current, " \t\r");
        if (*next == '\n') {
            ++next;
            tokenizer->location.column = 0;
            ++tokenizer->location.line;
            continue;
        }

        tokenizer->location.column += (uint32_t)(next - current);
        tokenizer->location.offset += (uint32_t)(next - start);
        tokenizer->current = next;
        break;
    }
    return *tokenizer->current != 0;
}

// Unicode handling

static tm_bool tmxml_is_valid_xml_codepoint(uint32_t cp) {
    return (cp >= 0x1u && cp <= 0xD7FFu) || (cp >= 0xE000u && cp <= 0xFFFDu) || (cp >= 0x10000u && cp <= 0x10FFFFu);
}

// Include <tm_unicode.h> before this header in the implementation to not generate duplicate code.
#ifndef _TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765

typedef struct {
    const char* cur;
    const char* end;
} tmxml_utf8_stream;

tmxml_utf8_stream tmxml_utf8_make_stream(const char* data, tm_size_t len) {
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(data || len == 0);

    tmxml_utf8_stream result = {TM_NULL, TM_NULL};
    result.cur = data;
    result.end = data + len;
    return result;
}

static tm_bool tmxml_is_valid_codepoint(uint32_t codepoint) {
    return codepoint <= 0x10FFFFu && (codepoint < 0xD800u || codepoint > 0xDFFFu);
}
static tm_bool tmxml_utf8_extract(tmxml_utf8_stream* stream, uint32_t* codepoint_out) {
    TM_ASSERT(stream);
    TM_ASSERT(codepoint_out);

    uint32_t codepoint = 0xFFFFFFFFu;
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
    tm_bool result = tmxml_is_valid_codepoint(codepoint);
    /* Advance stream only if we could extract a valid codepoint, otherwise stream points to invalid codepoint. */
    if (result) stream->cur = cur;
    return result;
}

static tm_size_t tmxml_utf8_encode(uint32_t codepoint, char* out, tm_size_t out_len) {
    TM_ASSERT_VALID_SIZE(out_len);
    TM_ASSERT(out || out_len == 0);
    TM_ASSERT(tmxml_is_valid_codepoint(codepoint));

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

#else /* defined(_TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765) */

#define tmxml_utf8_stream tmu_utf8_stream
#define tmxml_utf8_make_stream tmu_utf8_make_stream_n
#define tmxml_is_valid_codepoint tmu_is_valid_codepoint
#define tmxml_utf8_extract tmu_utf8_extract
#define tmxml_utf8_encode tmu_utf8_encode

#endif /* defined(_TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765) */

static tm_bool tmxml_unicode_is_name_start(uint32_t cp) {
    // These are the allowed codepoint ranges for the first character of names.
    // According to https://www.w3.org/TR/xml/#sec-common-syn
    // NameStartChar ::= ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] |
    // [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] |
    // [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
    return (cp == ':') || (cp >= 'A' && cp <= 'Z') || (cp == '_') || (cp >= 'a' && cp <= 'z')
           || (cp >= 0xC0u && cp <= 0xD6u) || (cp >= 0xD8u && cp <= 0xF6u) || (cp >= 0xF8u && cp <= 0x2FFu)
           || (cp >= 0x370u && cp <= 0x37Du) || (cp >= 0x37Fu && cp <= 0x1FFFu) || (cp >= 0x200Cu && cp <= 0x200Du)
           || (cp >= 0x2070u && cp <= 0x218Fu) || (cp >= 0x2C00u && cp <= 0x2FEFu) || (cp >= 0x3001u && cp <= 0xD7FFu)
           || (cp >= 0xF900u && cp <= 0xFDCFu) || (cp >= 0xFDF0u && cp <= 0xFFFDu)
           || (cp >= 0x10000u && cp <= 0xEFFFFu);
}

static tm_bool tmxml_unicode_is_name(uint32_t cp) {
    // 	NameChar ::= NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
    return (cp == '-') || (cp == '.') || (cp >= '0' && cp <= '9') || tmxml_unicode_is_name_start(cp) || (cp == 0xB7u)
           || (cp >= 0x0300u && cp <= 0x036Fu) || (cp >= 0x203F && cp <= 0x2040u);
}

static tm_bool tmxml_is_valid_xml_string(tmxml_stringview str) {
    tmxml_utf8_stream stream = tmxml_utf8_make_stream(str.data, str.size);
    uint32_t cp = 0;
    while (tmxml_utf8_extract(&stream, &cp)) {
        switch (cp) {
            case 0:
                // Null bytes not allowed.
                return TM_FALSE;

            case ']': {
                // Check for "]]>", which is not allowed in content unescaped.
                size_t remaining = (size_t)(stream.end - stream.cur);
                if (remaining >= 2 && *(stream.cur) == ']' && *(stream.cur + 1) == '>') return TM_FALSE;
                break;
            }

            default: {
                // Check for validity of cp.
                if (!tmxml_is_valid_xml_codepoint(cp)) return TM_FALSE;
                break;
            }
        }
    }
    return stream.cur == stream.end;
}

static tmxml_token tmxml_lex_parse_name(struct tmxml_tokenizer* tokenizer) {
    tmxml_token result;
    const char* start = tokenizer->current;
    result.type = tmxml_tok_tag_open;
    result.contents.data = start;
    result.contents.size = 1;
    result.location = tokenizer->location;

    const char* current = tokenizer->current;
    const char* next = current + TM_STRCSPN(current, " \t\r\n=/>?");

    // Validate
    tmxml_utf8_stream stream = tmxml_utf8_make_stream(current, (tm_size_t)(next - current));
    uint32_t cp = 0;
    if (!tmxml_utf8_extract(&stream, &cp) || !tmxml_unicode_is_name_start(cp)) {
        result.type = tmxml_tok_error;
    } else {
        while (tmxml_utf8_extract(&stream, &cp)) {
            if (!tmxml_unicode_is_name(cp)) {
                result.type = tmxml_tok_error;
                break;
            }
        }
        if (stream.cur != stream.end) result.type = tmxml_tok_error;
    }

    if (next == start) result.type = tmxml_tok_error;
    if (next - start == 3) {
        // Check for reserved "xml" name.
        if ((*next == 'x' || *next == 'X') && (*(next + 1) == 'm' || *(next + 1) == 'M')
            && (*(next + 2) == 'l' || *(next + 2) == 'L')) {
            result.type = tmxml_tok_error;
        }
    }
    tokenizer->location.column += (uint32_t)(next - current);
    tokenizer->location.offset += (uint32_t)(next - start);
    tokenizer->current = next;
    result.contents.size = (tm_size_t)(next - start);
    result.location.length = (uint32_t)result.contents.size;
    return result;
}

static tmxml_token tmxml_lex_parse_until(struct tmxml_tokenizer* tokenizer, tmxml_token_type type, const char* find_str,
                                         tm_size_t find_str_len) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(find_str);
    TM_ASSERT_VALID_SIZE(find_str_len);

    tmxml_token result;
    const char* start = tokenizer->current;
    result.type = type;
    result.contents.data = start;
    result.contents.size = 1;
    result.location = tokenizer->location;

    const char* contents_end = TM_STRSTR(start, find_str);
    if (!contents_end) {
        result.type = tmxml_tok_error;
    } else {
        // Count newlines in CDATA section, so that location data is still valid.
        result.contents.size = (tm_size_t)(contents_end - start);
        contents_end += find_str_len;
        const char* newline = start;
        for (;;) {
            const char* current = newline;
            const void* nl = TM_MEMCHR(current, (unsigned char)'\n', (size_t)(contents_end - current));
            if (!nl) {
                tokenizer->location.column = (uint32_t)(contents_end - current);
                break;
            }
            newline = (const char*)nl;
            ++newline;
            ++tokenizer->location.line;
            tokenizer->location.column = 0;
        }
        tokenizer->current = contents_end;
        tokenizer->location.offset += (uint32_t)(contents_end - start);
    }

    result.location.length = (uint32_t)result.contents.size;
    return result;
}

static tmxml_token tmxml_lex_parse_cdata(struct tmxml_tokenizer* tokenizer) {
    return tmxml_lex_parse_until(tokenizer, tmxml_tok_contents_cdata, "]]>", 3);
}

static void tmxml_lex_advance(struct tmxml_tokenizer* tokenizer, uint32_t amount) {
    tokenizer->current += amount;
    tokenizer->location.offset += amount;
    tokenizer->location.column += amount;
}

static tmxml_token tmxml_lex_parse_comment(struct tmxml_tokenizer* tokenizer) {
    tmxml_token result = tmxml_lex_parse_until(tokenizer, tmxml_tok_comment, "--", 2);
    if (*tokenizer->current != '>') {
        // No "--" strings allowed in comments.
        result.type = tmxml_tok_error;
    } else {
        tmxml_lex_advance(tokenizer, 1);
    }
    return result;
}

static tmxml_token tmxml_lex_parse_processing_instructions(struct tmxml_tokenizer* tokenizer) {
    tmxml_token result = tmxml_lex_parse_until(tokenizer, tmxml_tok_processing_instructions, "?>", 2);
    if (result.type != tmxml_tok_error) {
        if (result.contents.size == 0) {
            result.type = tmxml_tok_error;
        } else if (result.contents.size >= 3) {
            const char* p = result.contents.data;
            if ((*p == 'x' || *p == 'X') && (*(p + 1) == 'm' || *(p + 1) == 'M')
                && (*(p + 2) == 'l' || *(p + 2) == 'L')) {
                result.type = tmxml_tok_error;
            }
        }
    }
    return result;
}

static tmxml_token tmxml_lex_make_error(struct tmxml_tokenizer* tokenizer) {
    tmxml_token result;
    result.type = tmxml_tok_error;
    result.contents.data = tokenizer->current;
    result.contents.size = 0;
    result.location = tokenizer->location;
    return result;
}

static tmxml_stringview tmxml_trim_left(tmxml_stringview str) {
    tmxml_stringview result = {TM_NULL, 0};
    result.data = str.data;
    if (str.size > 0) {
        const char* str_end = str.data + str.size;
        while (str.data != str_end && TM_MEMCHR(" \t\n\r", (unsigned char)(*str.data), 4)) {
            ++str.data;
        }
        result.data = str.data;
        result.size = (tm_size_t)(str_end - str.data);
    }
    return result;
}

static tmxml_stringview tmxml_trim_right(tmxml_stringview str) {
    tmxml_stringview result = {TM_NULL, 0};
    result.data = str.data;
    if (str.size > 0) {
        const char* str_end = str.data + str.size;
        while (str_end != str.data && TM_MEMCHR(" \t\n\r", (unsigned char)(*(str_end - 1)), 4)) {
            --str_end;
        }
        result.size = (tm_size_t)(str_end - str.data);
    }
    return result;
}

static tm_bool tmxml_process_cdata_reduce_whitespace(tmxml_reader* reader, uint32_t flags, char* out, tm_size_t out_len,
                                                     tm_size_t* out_written) {
    TM_ASSERT(reader);
    TM_ASSERT_VALID_SIZE(out_len);
    TM_ASSERT(out || out_len == 0);
    TM_ASSERT(out_written);

    *out_written = 0;

    tm_bool reduce_whitespace = (flags & tmxml_flags_trim_reduce_spaces) != 0;
    tm_size_t content_size = 0;
    tmxml_utf8_stream stream = tmxml_utf8_make_stream(reader->current.contents.data, reader->current.contents.size);
    uint32_t cp = 0;
    const char* prev = stream.cur;
    tm_bool last_was_whitespace = TM_FALSE;
    while (tmxml_utf8_extract(&stream, &cp)) {
        switch (cp) {
            case 0:
                // Null bytes not allowed.
                return TM_FALSE;

            case ']': {
                // Check for "]]>", which is not allowed in content unescaped.
                size_t remaining = (size_t)(stream.end - stream.cur);
                if (remaining >= 2 && *(stream.cur) == ']' && *(stream.cur + 1) == '>') return TM_FALSE;
                if (out_len) {
                    *out = (char)cp;
                    ++out;
                    --out_len;
                }
                ++content_size;
                break;
            }

            default: {
                // Check for validity of cp.
                if (!tmxml_is_valid_xml_codepoint(cp)) return TM_FALSE;

                if (reduce_whitespace) {
                    if (cp <= ' ') {
                        if (last_was_whitespace) break;
                        last_was_whitespace = TM_TRUE;
                        if ((flags & tmxml_flags_trim_trim_newlines) || (cp != '\r' && cp != '\r')) {
                            if (out_len) {
                                *out = ' ';
                                ++out;
                                --out_len;
                            }
                            ++content_size;
                            break;
                        }
                    } else {
                        last_was_whitespace = TM_FALSE;
                    }
                } else if ((flags & tmxml_flags_trim_trim_newlines) && (cp == '\r' || cp == '\n')) {
                    if (cp == '\r' && (stream.cur != stream.end) && *(stream.cur + 1) == '\n') continue;
                    if (out_len) {
                        *out = ' ';
                        ++out;
                        --out_len;
                    }
                    ++content_size;
                    break;
                }

                TM_ASSERT(stream.cur >= prev);
                tm_size_t consumed_amount = (tm_size_t)(stream.cur - prev);
                if (out_len >= consumed_amount) {
                    TM_MEMCPY(out, prev, consumed_amount * sizeof(char));
                    out += consumed_amount;
                    out_len -= consumed_amount;
                }
                content_size += consumed_amount;
                break;
            }
        }
        prev = stream.cur;
    }

    // Nullterminate.
    if (out_len) {
        *out = 0;
        ++out;
        --out_len;
    } else {
        // Count the nullterminator to the content_size only if buffer was not sufficient.
        // That way the caller will allocate enough memory for the next call to this function,
        // but on success will not treat the nullterminator as part of the content.
        ++content_size;
    }
    *out_written = content_size;
    return stream.cur == stream.end;
}

struct tmxml_buffer {
    char* data;
    tm_size_t size;
};

static struct tmxml_buffer tmxml_get_buffer(tmxml_reader* reader) {
    struct tmxml_buffer result = {TM_NULL, 0};
    if (reader->buffer) {
        result.data = reader->buffer;
        result.size = reader->buffer_size;
    } else {
        result.data = reader->sbo;
        result.size = (tm_size_t)(sizeof(reader->sbo) / sizeof(char));
    }
    return result;
}

static tm_bool tmxml_grow_buffer(tmxml_reader* reader, tm_size_t content_size, struct tmxml_buffer* out) {
    TM_ASSERT(reader);
    TM_ASSERT_VALID_SIZE(content_size);
    TM_ASSERT(out);
    TM_ASSERT(out->size < content_size);

    void* new_buffer = TM_NULL;
    if (out->data == reader->sbo) {
        new_buffer = TM_MALLOC(content_size * sizeof(char), sizeof(char));
    } else {
        new_buffer = TM_REALLOC(out->data, content_size * sizeof(char), sizeof(char));
    }
    if (!new_buffer) {
        reader->current.type = tmxml_tok_error;
        return TM_FALSE;
    }

    out->data = (char*)new_buffer;
    out->size = content_size;
    reader->buffer = out->data;
    reader->buffer_size = out->size;
    return TM_TRUE;
}

static tm_bool tmxml_process_cdata(tmxml_reader* reader) {
    uint32_t flags = reader->flags;
    if (!(flags & tmxml_flags_trim_trim_cdata)) {
        // If we are in an attribute value, but trimming for it isn't enabled, clear the flags.
        flags &= ~((uint32_t)(tmxml_flags_trim_left | tmxml_flags_trim_right | tmxml_flags_trim_reduce_spaces
                              | tmxml_flags_trim_trim_newlines));
    }
    if (flags & tmxml_flags_trim_left) reader->current.contents = tmxml_trim_left(reader->current.contents);
    if (flags & tmxml_flags_trim_right) reader->current.contents = tmxml_trim_right(reader->current.contents);
    if (reader->current.contents.size == 0) return TM_TRUE;

    tm_bool reduce_whitespace = (flags & tmxml_flags_trim_reduce_spaces) != 0;
    if (reduce_whitespace) {
        struct tmxml_buffer buffer = tmxml_get_buffer(reader);

        tm_size_t content_size = 0;
        if (!tmxml_process_cdata_reduce_whitespace(reader, flags, buffer.data, buffer.size, &content_size)) {
            reader->current.type = tmxml_tok_error;
            return TM_FALSE;
        }

        if (content_size >= buffer.size) {
            if (!tmxml_grow_buffer(reader, content_size, &buffer)) return TM_FALSE;

            // Try again with new buffer.
            if (!tmxml_process_cdata_reduce_whitespace(reader, flags, buffer.data, buffer.size, &content_size)) {
                reader->current.type = tmxml_tok_error;
                return TM_FALSE;
            }
            TM_ASSERT(content_size < buffer.size);
        }

        reader->current.contents.data = buffer.data;
        reader->current.contents.size = content_size;
        return TM_TRUE;
    }

    return tmxml_is_valid_xml_string(reader->current.contents);
}

TMXML_DEF tm_bool tmxml_next_token(tmxml_reader* reader) {
    TM_ASSERT(reader);
    TM_ASSERT(reader->tokenizer.current);

    tmxml_read_context context = reader->context;
    struct tmxml_tokenizer tokenizer = reader->tokenizer;
    const uint32_t flags = reader->flags;

    reader->context = tmxml_rc_error;
    reader->current = tmxml_lex_make_error(&tokenizer);

    switch (context) {
        case tmxml_rc_eof:
            reader->context = tmxml_rc_eof;
            // Fallthrough
        case tmxml_rc_error:
            return TM_FALSE;

        case tmxml_rc_none:
            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_TRUE;
            if (*tokenizer.current != '<') return TM_TRUE;
            // Fallthrough

        case tmxml_rc_content: {
            for (;;) {
                switch (*tokenizer.current) {
                    case 0:
                        if (reader->root_found && reader->tags_depth == 0) {
                            reader->context = tmxml_rc_eof;
                            reader->current.type = tmxml_tok_eof;
                        }
                        return TM_TRUE;

                    case '<': {
                        tmxml_lex_advance(&tokenizer, 1);
                        switch (*tokenizer.current) {
                            case 0:
                                return TM_TRUE;

                            case '!': {
                                tmxml_lex_advance(&tokenizer, 1);
                                if (TM_STRNCMP(tokenizer.current, "--", 2) == 0) {
                                    // Comment.
                                    tmxml_lex_advance(&tokenizer, 2);
                                    reader->current = tmxml_lex_parse_comment(&tokenizer);
                                    if (reader->current.type == tmxml_tok_error) return TM_TRUE;
                                    if (!tmxml_unescape_current_token(reader)) return TM_TRUE;
                                    reader->context = tmxml_rc_content;
                                } else if (TM_STRNCMP(tokenizer.current, "[CDATA[", 7) == 0) {
                                    // CDATA.
                                    tmxml_lex_advance(&tokenizer, 7);
                                    if (reader->tags_depth > 0) {
                                        reader->current = tmxml_lex_parse_cdata(&tokenizer);
                                        if (reader->current.type == tmxml_tok_error) return TM_TRUE;
                                        if (!tmxml_process_cdata(reader)) return TM_TRUE;
                                        reader->context = tmxml_rc_content;
                                    }
                                } else {
                                    // Error, unknown tag.
                                    return TM_TRUE;
                                }
                                break;
                            }

                            case '?': {
                                tmxml_lex_advance(&tokenizer, 1);
                                reader->current = tmxml_lex_parse_processing_instructions(&tokenizer);
                                if (reader->current.type == tmxml_tok_error) return TM_TRUE;
                                if (!tmxml_is_valid_xml_string(reader->current.contents)) {
                                    reader->current.type = tmxml_tok_error;
                                    return TM_TRUE;
                                }
                                reader->context = tmxml_rc_content;
                                break;
                            }

                            case '/': {
                                tmxml_lex_advance(&tokenizer, 1);
                                reader->current = tmxml_lex_parse_name(&tokenizer);
                                reader->current.type = tmxml_tok_tag_close;
                                if (reader->current.type != tmxml_tok_error) {
                                    if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_TRUE;
                                    if (*tokenizer.current != '>') {
                                        reader->current = tmxml_lex_make_error(&tokenizer);
                                        return TM_TRUE;
                                    }
                                    tmxml_lex_advance(&tokenizer, 1);
                                    reader->context = tmxml_rc_content;
                                }
                                if (reader->tags_depth == 0) return TM_TRUE;
                                --reader->tags_depth;
                                if (reader->tags_depth == 0) {
                                    // We consumed the root tag, now only whitespace and comments are allowed.
                                    reader->context = tmxml_rc_post_root;
                                }
                                break;
                            }

                            default:
                                reader->current = tmxml_lex_parse_name(&tokenizer);
                                if (reader->current.type != tmxml_tok_error) {
                                    if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_TRUE;
                                    if (*tokenizer.current == '>') {
                                        tmxml_lex_advance(&tokenizer, 1);
                                        reader->context = tmxml_rc_content;
                                        ++reader->tags_depth;
                                    } else if (*tokenizer.current == '/') {
                                        // Leave it for next call.
                                        reader->context = tmxml_rc_inside_tag;
                                        reader->current_tag = reader->current.contents;
                                    } else {
                                        reader->context = tmxml_rc_inside_tag;
                                    }
                                    reader->root_found = TM_TRUE;
                                }
                                break;
                        }
                        break;
                    }

                    default: {
                        if (!reader->root_found || reader->tags_depth == 0) {
                            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_TRUE;
                            reader->tokenizer = tokenizer;
                            if (*tokenizer.current != '<') return TM_TRUE;
                            continue;
                        }
                        reader->current = tmxml_lex_parse_contents(&tokenizer);
                        if (reader->current.type == tmxml_tok_error) return TM_TRUE;
                        if (!tmxml_unescape_current_token(reader)) return TM_TRUE;
                        if ((flags & (tmxml_flags_trim_left | tmxml_flags_trim_right))
                            && reader->current.contents.size == 0) {
                            reader->current = tmxml_lex_make_error(&tokenizer);
                            continue;
                        }
                        reader->context = tmxml_rc_content;
                        break;
                    }
                }
                break;
            }
            reader->tokenizer = tokenizer;
            return TM_TRUE;
        }

        case tmxml_rc_inside_tag: {
            // Expecting to parse an attribute now or '/>'.
            if (*tokenizer.current == '/') {
                tmxml_lex_advance(&tokenizer, 1);
                if (*tokenizer.current != '>') {
                    reader->current = tmxml_lex_make_error(&tokenizer);
                    return TM_TRUE;
                }
                if (reader->tags_depth == 0) {
                    // We consumed the root tag, now only whitespace and comments are allowed.
                    reader->context = tmxml_rc_post_root;
                } else {
                    reader->context = tmxml_rc_content;
                }
                tmxml_lex_advance(&tokenizer, 1);
                reader->current.type = tmxml_tok_tag_close;
                reader->current.contents = reader->current_tag;
                reader->current_tag.data = TM_NULL;
                reader->current_tag.size = 0;
                reader->tokenizer = tokenizer;
                return TM_TRUE;
            }

            reader->current = tmxml_lex_parse_name(&tokenizer);
            if (reader->current.type != tmxml_tok_error) {
                reader->current.type = tmxml_tok_attribute_name;
                if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_TRUE;
                if (*tokenizer.current != '=') {
                    reader->current = tmxml_lex_make_error(&tokenizer);
                    return TM_TRUE;
                }
                tmxml_lex_advance(&tokenizer, 1);
                if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_TRUE;
                if (*tokenizer.current != '"' && *tokenizer.current != '\'') {
                    reader->current = tmxml_lex_make_error(&tokenizer);
                    return TM_TRUE;
                }
                reader->context = tmxml_rc_inside_tag_attribute_name;
            }
            reader->tokenizer = tokenizer;
            return TM_TRUE;
        }

        case tmxml_rc_inside_tag_attribute_name: {
            // Expecting to parse an attribute value now.
            reader->current = tmxml_lex_parse_attribute_value(&tokenizer);
            if (reader->current.type != tmxml_tok_error) {
                if (!tmxml_unescape_current_token(reader)) return TM_TRUE;
                // At least one whitespace or '>' has to follow after an attribute value.
                if (!TM_STRCHR(" \t\r\n>", (unsigned char)*tokenizer.current)) return TM_TRUE;
                if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_TRUE;
                if (*tokenizer.current == '>') {
                    tmxml_lex_advance(&tokenizer, 1);
                    reader->context = tmxml_rc_content;
                    ++reader->tags_depth;
                } else if (*tokenizer.current == '/') {
                    // Leave it for next call.
                    reader->context = tmxml_rc_inside_tag;
                    reader->current_tag = reader->current.contents;
                } else {
                    reader->context = tmxml_rc_inside_tag;
                }
            }
            reader->tokenizer = tokenizer;
            return TM_TRUE;
        }

        case tmxml_rc_post_root: {
            // Only whitespace and comments allowed from this point on.
            if (tmxml_lex_skip_whitespace(&tokenizer)) {
                reader->current = tmxml_lex_make_error(&tokenizer);
                if (TM_STRNCMP(tokenizer.current, "<!--", 4) == 0) {
                    tmxml_lex_advance(&tokenizer, 4);
                    reader->current = tmxml_lex_parse_comment(&tokenizer);
                    if (reader->current.type != tmxml_tok_error) reader->context = tmxml_rc_post_root;
                    if (!tmxml_unescape_current_token(reader)) return TM_TRUE;
                } else if (TM_STRNCMP(tokenizer.current, "<?", 2) == 0) {
                    tmxml_lex_advance(&tokenizer, 2);
                    reader->current = tmxml_lex_parse_processing_instructions(&tokenizer);
                    if (reader->current.type == tmxml_tok_error) return TM_TRUE;
                    if (!tmxml_is_valid_xml_string(reader->current.contents)) {
                        reader->current.type = tmxml_tok_error;
                        return TM_TRUE;
                    }
                    reader->context = tmxml_rc_post_root;
                }
            } else {
                reader->current.type = tmxml_tok_eof;
                reader->current.contents.data = tokenizer.current;
                reader->current.contents.size = 0;
                reader->current.location = tokenizer.location;
                reader->context = tmxml_rc_eof;
            }
            reader->tokenizer = tokenizer;
            return TM_TRUE;
        }
    }

    TM_ASSERT(0 && "Invalid context, forgot to initialize tmxml_reader?");
    return TM_FALSE;
}

TMXML_DEF tm_bool tmxml_read_prolog(tmxml_reader* reader, tmxml_prolog* out) {
    TM_ASSERT(reader);
    TM_ASSERT(out);

    TM_MEMSET(out, 0, sizeof(*out));

    struct tmxml_tokenizer tokenizer = reader->tokenizer;
    if (!tokenizer.current) return TM_TRUE;
    if (*tokenizer.current == '<' && *(tokenizer.current + 1) == '?') {
        // Prolog found.
        tmxml_lex_advance(&tokenizer, 2);

        out->present = TM_TRUE;
        out->standalone = TM_TRUE;
        if (TM_STRNCMP(tokenizer.current, "xml", 3) != 0) return TM_FALSE;
        tmxml_lex_advance(&tokenizer, 3);
        if (!TM_STRCHR(" \t\r\n", (unsigned char)*tokenizer.current)) return TM_FALSE;
        if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;

        tmxml_token token = tmxml_lex_parse_name(&tokenizer);
        if (token.type != tmxml_tok_tag_open) return TM_FALSE;

        if (token.contents.size == 7 && TM_MEMCMP(token.contents.data, "version", 7) == 0) {
            if (out->version.size > 0) return TM_FALSE;
            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;
            if (*tokenizer.current != '=') return TM_FALSE;
            tmxml_lex_advance(&tokenizer, 1);
            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;
            tmxml_token version = tmxml_lex_parse_attribute_value(&tokenizer);
            if (version.type != tmxml_tok_attribute_value) return TM_FALSE;
            if (version.contents.size <= 0) return TM_FALSE;
            out->version = version.contents;
        } else {
            return TM_FALSE;
        }

        // Must have at least one whitespace after an attribute except there are no more attributes.
        if (!TM_STRCHR(" \t\r\n?", (unsigned char)*tokenizer.current)) return TM_FALSE;
        if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;
        token = tmxml_lex_parse_name(&tokenizer);

        if (token.type == tmxml_tok_tag_open && token.contents.size == 8
            && TM_MEMCMP(token.contents.data, "encoding", 8) == 0) {
            if (out->encoding.size > 0) return TM_FALSE;
            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;
            if (*tokenizer.current != '=') return TM_FALSE;
            tmxml_lex_advance(&tokenizer, 1);
            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;
            tmxml_token encoding = tmxml_lex_parse_attribute_value(&tokenizer);
            if (encoding.type != tmxml_tok_attribute_value) return TM_FALSE;
            if (encoding.contents.size <= 0) return TM_FALSE;
            out->encoding = encoding.contents;

            if (!TM_STRCHR(" \t\r\n?", (unsigned char)*tokenizer.current)) return TM_FALSE;
            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;
            token = tmxml_lex_parse_name(&tokenizer);
        }

        if (token.type == tmxml_tok_tag_open && token.contents.size == 10
            && TM_MEMCMP(token.contents.data, "standalone", 10) == 0) {
            if (out->standalone_specified) return TM_FALSE;
            out->standalone_specified = TM_TRUE;
            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;
            if (*tokenizer.current != '=') return TM_FALSE;
            tmxml_lex_advance(&tokenizer, 1);
            if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;
            tmxml_token standalone = tmxml_lex_parse_attribute_value(&tokenizer);
            if (standalone.type != tmxml_tok_attribute_value) return TM_FALSE;
            if (standalone.contents.size <= 0) return TM_FALSE;
            if (standalone.contents.size == 3 && TM_MEMCMP(standalone.contents.data, "yes", 3) == 0) {
                out->standalone = TM_TRUE;
            } else if (standalone.contents.size == 2 && TM_MEMCMP(standalone.contents.data, "no", 3) == 0) {
                out->standalone = TM_FALSE;
            } else {
                return TM_FALSE;
            }
            token.type = tmxml_tok_error;
        }

        if (token.type == tmxml_tok_tag_open) return TM_FALSE;

        if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_FALSE;

        if (out->version.size == 0) return TM_FALSE;

        if (*tokenizer.current != '?') return TM_FALSE;
        tmxml_lex_advance(&tokenizer, 1);
        if (*tokenizer.current != '>') return TM_FALSE;
        tmxml_lex_advance(&tokenizer, 1);
    }

    // Try to read and discard DOCTYPE tag.
    struct tmxml_tokenizer tokenizer_rollback = tokenizer;

    for (;;) {
        if (!tmxml_lex_skip_whitespace(&tokenizer)) return TM_TRUE;
        if (*tokenizer.current != '<') break;
        if (*(tokenizer.current + 1) != '!') {
            // We found the root tag, rollback.
            tokenizer = tokenizer_rollback;
            break;
        }
        tmxml_lex_advance(&tokenizer, 2);
        if (*tokenizer.current == '-' && *(tokenizer.current + 1) == '-') {
            tmxml_lex_advance(&tokenizer, 2);
            if (tmxml_lex_parse_comment(&tokenizer).type == tmxml_tok_error) return TM_FALSE;
            continue;
        } else if (TM_STRNCMP(tokenizer.current, "DOCTYPE", 7) == 0) {
            tmxml_lex_advance(&tokenizer, 7);
            out->skipped_doctype = TM_TRUE;

            // Parse until we skipped DOCTYPE.
            size_t tags_depth = 1;
            do {
                const char* next = tokenizer.current + TM_STRCSPN(tokenizer.current, "\n<>");
                char c = *next;
                if (c == '\n') {
                    tokenizer.location.column = 0;
                    ++tokenizer.location.line;
                    ++next;
                } else if (c == '>') {
                    --tags_depth;
                    ++next;
                } else if (*next == '<') {
                    ++tags_depth;
                    ++next;
                } else {
                    // Error: Unexpected end of file, but let the reader handle it.
                    break;
                }

                tokenizer.location.offset += (uint32_t)(next - tokenizer.current);
                tokenizer.current = next;
            } while (tags_depth > 0);
            break;
        } else {
            // Error: invalid tag, but let the reader handle it.
            break;
        }
    }

    reader->tokenizer = tokenizer;
    return TM_TRUE;
}

TMXML_DEF tm_bool tmxml_unescape_string(const char* str, tm_size_t len, unsigned int flags, char* out,
                                        tm_size_t out_len, tm_size_t* out_written) {
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len == 0);
    TM_ASSERT_VALID_SIZE(out_len);
    TM_ASSERT(out || out_len == 0);
    TM_ASSERT(str != out);
    TM_ASSERT(out_written);

    *out_written = 0;
    tmxml_stringview current;
    current.data = str;
    current.size = len;
    if (flags & tmxml_flags_trim_left) current = tmxml_trim_left(current);
    if (flags & tmxml_flags_trim_right) current = tmxml_trim_right(current);
    if (current.size == 0) return TM_TRUE;

    tm_size_t content_size = 0;
    tmxml_utf8_stream stream = tmxml_utf8_make_stream(current.data, current.size);
    uint32_t cp = 0;
    const char* prev = stream.cur;
    tm_bool reduce_whitespace = (flags & tmxml_flags_trim_reduce_spaces) != 0;
    tm_bool last_was_whitespace = TM_FALSE;
    while (tmxml_utf8_extract(&stream, &cp)) {
        switch (cp) {
            case 0:
                // Null bytes not allowed.
                return TM_FALSE;

            case '&': {
                size_t remaining = (size_t)(stream.end - stream.cur);
                if (remaining <= 0) return TM_FALSE;
                if (*stream.cur == '#') {
                    if (remaining <= 1) return TM_FALSE;
                    ++stream.cur;
                    --remaining;
                    const void* semicolon = TM_MEMCHR(stream.cur, (unsigned int)';', remaining);
                    if (!semicolon) return TM_FALSE;
                    size_t number_len = (size_t)((const char*)semicolon - stream.cur);
                    // Distinguish between decimal and hexadecimal.
                    int base = 10;
                    if (number_len >= 1 && *stream.cur == 'x') {
                        // Hexadecimal
                        --number_len;
                        ++stream.cur;
                        base = 16;
                        if (number_len <= 0 || number_len > 8) return TM_FALSE;
                        // Validate.
                        for (size_t i = 0; i < number_len; ++i) {
                            if (!((stream.cur[i] >= '0' && stream.cur[i] <= '9')
                                  || (stream.cur[i] >= 'a' && stream.cur[i] <= 'f')
                                  || (stream.cur[i] >= 'A' && stream.cur[i] <= 'F')))
                                return TM_FALSE;
                        }
                    } else {
                        if (number_len <= 0 || number_len > 10) return TM_FALSE;
                        // Validate.
                        for (size_t i = 0; i < number_len; ++i) {
                            if (!(stream.cur[i] >= '0' && stream.cur[i] <= '9')) return TM_FALSE;
                        }
                    }

                    // Copy to nullterminated buffer.
                    char buffer[11];
                    TM_MEMCPY(buffer, stream.cur, number_len * sizeof(char));
                    buffer[number_len] = 0;
                    cp = TM_STRTOUL(buffer, TM_NULL, base);
                    stream.cur = ((const char*)semicolon) + 1;

                    // Check for validity of cp.
                    if (!tmxml_is_valid_xml_codepoint(cp)) return TM_FALSE;

                    // REFACTOR: Code duplication with default case and process_cdata.
                    if (reduce_whitespace) {
                        if (cp <= ' ') {
                            if (last_was_whitespace) break;
                            last_was_whitespace = TM_TRUE;
                            if ((flags & tmxml_flags_trim_trim_newlines) || (cp != '\r' && cp != '\r')) {
                                if (out_len) {
                                    *out = ' ';
                                    ++out;
                                    --out_len;
                                }
                                ++content_size;
                                break;
                            }
                        } else {
                            last_was_whitespace = TM_FALSE;
                        }
                    } else if ((flags & tmxml_flags_trim_trim_newlines) && (cp == '\r' || cp == '\n')) {
                        if (cp == '\r' && (stream.cur != stream.end) && *(stream.cur + 1) == '\n') continue;
                        if (out_len) {
                            *out = ' ';
                            ++out;
                            --out_len;
                        }
                        ++content_size;
                        break;
                    }

                    // Turn into utf-8 and copy into out.
                    tm_size_t encoded_size = tmxml_utf8_encode(cp, out, out_len);
                    if (encoded_size <= out_len) {
                        out += encoded_size;
                        out_len -= encoded_size;
                    }
                    content_size += encoded_size;
                } else if (remaining >= 3 && TM_MEMCMP(stream.cur, "lt;", 3 * sizeof(char)) == 0) {
                    if (out_len) {
                        *out = '<';
                        ++out;
                        --out_len;
                    }
                    ++content_size;
                    stream.cur += 3;
                } else if (remaining >= 4 && TM_MEMCMP(stream.cur, "amp;", 4 * sizeof(char)) == 0) {
                    if (out_len) {
                        *out = '&';
                        ++out;
                        --out_len;
                    }
                    ++content_size;
                    stream.cur += 4;
                } else if (remaining >= 5 && TM_MEMCMP(stream.cur, "quot;", 5 * sizeof(char)) == 0) {
                    if (out_len) {
                        *out = '"';
                        ++out;
                        --out_len;
                    }
                    ++content_size;
                    stream.cur += 5;
                } else if (remaining >= 5 && TM_MEMCMP(stream.cur, "apos;", 5 * sizeof(char)) == 0) {
                    if (out_len) {
                        *out = '\'';
                        ++out;
                        --out_len;
                    }
                    ++content_size;
                    stream.cur += 5;
                } else if (remaining >= 3 && TM_MEMCMP(stream.cur, "gt;", 3 * sizeof(char)) == 0) {
                    if (out_len) {
                        *out = '>';
                        ++out;
                        --out_len;
                    }
                    ++content_size;
                    stream.cur += 3;
                } else {
                    const void* semicolon = TM_MEMCHR(stream.cur, (unsigned int)';', remaining);
                    if (!semicolon) return TM_FALSE;

                    // TODO: How to handle this case? We couldn't find the escape entity.
                    // Probably a custom entity, but since we do not support <!ENTITY>, we can't handle this.
                    if (out_len) {
                        *out = '&';
                        ++out;
                        --out_len;
                    }
                    ++content_size;
                }
                break;
            }

            case '<': {
                if (!(flags & tmxml_internal_inside_comment)) return TM_FALSE;
                if (out_len) {
                    *out = (char)cp;
                    ++out;
                    --out_len;
                }
                ++content_size;
                break;
            }

            case ']': {
                // Check for "]]>", which is not allowed in content unescaped.
                size_t remaining = (size_t)(stream.end - stream.cur);
                if (!(flags & tmxml_internal_inside_comment) && remaining >= 2 && *(stream.cur) == ']'
                    && *(stream.cur + 1) == '>') {
                    return TM_FALSE;
                }
                if (out_len) {
                    *out = (char)cp;
                    ++out;
                    --out_len;
                }
                ++content_size;
                break;
            }

            default: {
                // Check for validity of cp.
                if (!tmxml_is_valid_xml_codepoint(cp)) return TM_FALSE;

                if (reduce_whitespace) {
                    if (cp <= ' ') {
                        if (last_was_whitespace) break;
                        last_was_whitespace = TM_TRUE;
                        if ((flags & tmxml_flags_trim_trim_newlines) || (cp != '\r' && cp != '\r')) {
                            if (out_len) {
                                *out = ' ';
                                ++out;
                                --out_len;
                            }
                            ++content_size;
                            break;
                        }
                    } else {
                        last_was_whitespace = TM_FALSE;
                    }
                } else if ((flags & tmxml_flags_trim_trim_newlines) && (cp == '\r' || cp == '\n')) {
                    if (cp == '\r' && (stream.cur != stream.end) && *(stream.cur + 1) == '\n') continue;
                    if (out_len) {
                        *out = ' ';
                        ++out;
                        --out_len;
                    }
                    ++content_size;
                    break;
                }

                tm_size_t consumed_amount = (tm_size_t)(stream.cur - prev);
                if (out_len >= consumed_amount) {
                    TM_MEMCPY(out, prev, consumed_amount * sizeof(char));
                    out += consumed_amount;
                    out_len -= consumed_amount;
                }
                content_size += consumed_amount;
                break;
            }
        }
        prev = stream.cur;
    }

    // Nullterminate.
    if (out_len) {
        *out = 0;
        ++out;
        --out_len;
    } else {
        // Count the nullterminator to the content_size only if buffer was not sufficient.
        // That way the caller will allocate enough memory for the next call to this function,
        // but on success will not treat the nullterminator as part of the content.
        ++content_size;
    }
    *out_written = content_size;
    return stream.cur == stream.end;
}

TMXML_DEF tm_bool tmxml_unescape_current_token(tmxml_reader* reader) {
    uint32_t flags = reader->flags;
    if (reader->current.type == tmxml_tok_attribute_value && !(flags & tmxml_flags_trim_trim_attribute_values)) {
        // If we are in an attribute value, but trimming for it isn't enabled, clear the flags.
        flags &= ~((uint32_t)(tmxml_flags_trim_left | tmxml_flags_trim_right | tmxml_flags_trim_reduce_spaces
                              | tmxml_flags_trim_trim_newlines));
    }
    if (flags & tmxml_flags_trim_left) reader->current.contents = tmxml_trim_left(reader->current.contents);
    if (flags & tmxml_flags_trim_right) reader->current.contents = tmxml_trim_right(reader->current.contents);
    if (reader->current.contents.size == 0) return TM_TRUE;

    if (reader->current.type == tmxml_tok_comment) {
        flags |= tmxml_internal_inside_comment;
    }

    struct tmxml_buffer buffer = tmxml_get_buffer(reader);

    tm_size_t content_size = 0;
    if (!tmxml_unescape_string(reader->current.contents.data, reader->current.contents.size, flags, buffer.data,
                               buffer.size, &content_size)) {
        reader->current.type = tmxml_tok_error;
        return TM_FALSE;
    }

    if (content_size >= buffer.size) {
        if (!tmxml_grow_buffer(reader, content_size, &buffer)) return TM_FALSE;

        // Try again with new buffer.
        if (!tmxml_unescape_string(reader->current.contents.data, reader->current.contents.size, flags, buffer.data,
                                   buffer.size, &content_size)) {
            reader->current.type = tmxml_tok_error;
            return TM_FALSE;
        }
        TM_ASSERT(content_size < buffer.size);
    }

    reader->current.contents.data = buffer.data;
    reader->current.contents.size = content_size;
    return TM_TRUE;
}

TMXML_DEF void tmxml_destroy_reader(tmxml_reader* reader) {
    if (reader && reader->buffer) {
        TM_FREE(reader->buffer);
        reader->buffer = TM_NULL;
        reader->buffer_size = 0;
    }
}

TMXML_DEF char* tmxml_copy_string(tmxml_stringview str) {
    TM_ASSERT(str.data || str.size == 0);

    char* result = TM_NULL;
    if (str.size > 0) {
        void* buffer = TM_MALLOC(str.size * sizeof(char), sizeof(char));
        if (buffer) {
            TM_MEMCPY(buffer, str.data, str.size * sizeof(char));
            result = (char*)buffer;
        }
    } else {
        // Allocate a single byte for an empty string.
        void* buffer = TM_MALLOC(sizeof(char), sizeof(char));
        if (buffer) {
            result = (char*)buffer;
            *result = 0;
        }
    }
    return result;
}

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_XML_IMPLEMENTATION) */

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