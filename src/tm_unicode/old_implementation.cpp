static tm_bool tmf_grow_by(tmf_contents* contents, tm_size_t amount);

/* Byte order marks for all encodings we can decode. */
static const unsigned char tmf_utf8_bom[3] = {0xEF, 0xBB, 0xBF};
static const unsigned char tmf_utf16_be_bom[2] = {0xFE, 0xFF};
static const unsigned char tmf_utf16_le_bom[2] = {0xFF, 0xFE};
static const unsigned char tmf_utf32_be_bom[4] = {0x00, 0x00, 0xFE, 0xFF};
static const unsigned char tmf_utf32_le_bom[4] = {0xFF, 0xFE, 0x00, 0x00};

static void tmf_to_tmf_path(tmf_contents* path, tm_bool is_dir) {
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
        // Nullterminate.
        TM_ASSERT(path->size < path->capacity);
        path->data[path->size] = 0;
    }
}

#include "char_types.cpp"

static tm_errc tmf_create_directory_internal(const tmf_tchar* dir, tm_size_t dir_len);
static tm_size_t tmf_get_path_len_internal(const tmf_tchar* filename, tm_size_t filename_len);

#ifdef TMF_USE_WINDOWS_H
#include "windows_implementation.cpp"
#endif

#if defined(_MSC_VER) && defined(TMF_USE_CRT)
FILE* tmf_fopen_t(const tmf_char16* filename, const tmf_char16* mode) { return _wfopen(filename, mode); }
FILE* tmf_freopen_t(const tmf_char16* filename, const tmf_char16* mode, FILE* current) {
    return _wfreopen(filename, mode, current);
}
#endif /* defined(_MSC_VER) && defined(TMF_USE_CRT) */

typedef struct {
    tmf_char16* data;
    tm_size_t size;
    tm_size_t capacity;
} tmf_utf16_contents;

typedef struct {
    tmf_utf16_contents contents;
    tm_errc ec;
} tmf_utf16_contents_result;

tmf_utf16_contents_result tmf_utf8_to_utf16(const char* str, tmf_char16* sbo, tm_size_t sbo_size);
tmf_utf16_contents_result tmf_utf8_n_to_utf16(const char* str, tm_size_t len, tmf_char16* sbo, tm_size_t sbo_size);

#if defined(TMF_USE_CRT) && !defined(TMF_USE_WINDOWS_H)
#ifdef _MSC_VER
    #include <sys/types.h> /* Required on msvc so that sys/stat.h and wchar.h define additional functions. */
#endif
#include <sys/stat.h> /* stat function */
#include <errno.h> /* errno */

#include <wchar.h>
#include <stdlib.h>
#include <string.h>

/* Use malloc if provided, otherwise fall back to heap. */
#ifdef TM_REALLOC
    #define TMF_MALLOC TM_MALLOC
    #define TMF_REALLOC TM_REALLOC
    #define TMF_FREE TM_FREE
#else
    #define TMF_MALLOC(size, alignment) malloc((size))
    #define TMF_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) realloc((ptr), (new_size))
    #define TMF_FREE(ptr, size, alignment) free((ptr))
#endif

#ifdef TM_MEMMOVE
    #define TMF_MEMMOVE TM_MEMMOVE
#else
    #define TMF_MEMMOVE memmove
#endif

#ifdef TM_MEMCPY
    #define TMF_MEMCPY TM_MEMCPY
#else
    #define TMF_MEMCPY memcpy
#endif

#ifdef TM_WCSCHR
    #define TMF_STRCHRW TM_WCSCHR
#else
    #define TMF_STRCHRW wcschr
#endif

#define TMF_STRLEN strlen
#define TMF_WCSLEN wcslen
#define TMF_MEMCHR memchr

#if defined(_MSC_VER)

#include "msvc_crt.cpp"

#elif defined(__GNUC__) || defined(__clang__) || defined(__linux__)

#include "unix_crt.cpp"

#else
#error Not implemented on this platform.
#endif

#include "common_crt.cpp"

#endif /* defined(TMF_USE_CRT) && !defined(TMF_USE_WINDOWS_H) */

#define TMF_MAX_UTF32 0x10FFFFu
#define TMF_LEAD_SURROGATE_MIN 0xD800u
#define TMF_LEAD_SURROGATE_MAX 0xDBFFu
#define TMF_TRAILING_SURROGATE_MIN 0xDC00u
#define TMF_TRAILING_SURROGATE_MAX 0xDFFFu
#define TMF_SURROGATE_OFFSET (0x10000u - (0xD800u << 10u) - 0xDC00u)
#define TMU_INVALID_CODEPOINT 0xFFFFFFFFu
#include "unicode.cpp"
#include "utf_conversion.cpp"
#include "to_utf16.cpp"
#undef TMF_MAX_UTF32
#undef TMF_LEAD_SURROGATE_MIN
#undef TMF_LEAD_SURROGATE_MAX
#undef TMF_TRAILING_SURROGATE_MIN
#undef TMF_TRAILING_SURROGATE_MAX
#undef TMF_SURROGATE_OFFSET

#include "common_implementation.cpp"

// #include "path_functions.cpp"

#undef TMF_MALLOC
#undef TMF_FREE
#undef TMF_STRCHRW