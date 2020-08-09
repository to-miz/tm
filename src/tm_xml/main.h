/*
tm_xml.h v0.1.0 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak MERGE_YEAR

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

#include "../common/GENERATED_WARNING.inc"

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

    #include "../common/tm_malloc.inc"
#endif
// clang-format on

#ifndef TM_XML_H_INLCUDED_46276729_6E96_4F9F_8DAF_FF70E27D3A34
#define TM_XML_H_INLCUDED_46276729_6E96_4F9F_8DAF_FF70E27D3A34

#include "../common/stdint.inc"

#include "../common/tm_null.inc"

#include "../common/tm_bool.inc"

#include "../common/tm_size_t.inc"

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

#include "../common/tm_unreferenced_param.inc"

#include "../common/tm_assert_valid_size.inc"

#include "main.cpp"

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_XML_IMPLEMENTATION) */

#include "../common/LICENSE.inc"