/*
References:
ERRNO:
    http://www.virtsync.com/c-error-codes-include-errno
    https://en.cppreference.com/w/cpp/error/errc

GNU: https://www.gnu.org/software/libc/manual/html_node/File-System-Interface.html#File-System-Interface

WINAPI:
    FILE: https://docs.microsoft.com/en-us/windows/desktop/fileio/file-management-functions
    DIRECTORY: https://docs.microsoft.com/en-us/windows/desktop/FileIO/directory-management-functions

WINDOWS CRT:
    FILE: https://docs.microsoft.com/en-us/cpp/c-runtime-library/file-handling?view=vs-2017
    DIRECTORY: https://docs.microsoft.com/en-us/cpp/c-runtime-library/directory-control?view=vs-2017
*/

#include "../common/GENERATED_WARNING.inc"

#ifdef TM_FILE_IMPLEMENTATION
    #include "../common/tm_assert.inc"
#endif /* defined(TM_FILE_IMPLEMENTATION) */

#ifndef _TM_FILE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765
#define _TM_FILE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765

#include "../common/stdint.inc"

#include "../common/tm_size_t.inc"

#include "../common/tm_bool.inc"

#include "../common/tm_errc.inc"
#define TMF_NO_SUCH_FILE_OR_DIRECTORY TM_ENOENT

typedef struct {
    char* data;
    tm_size_t size;
    tm_size_t capacity;

#if defined(__cplusplus) && defined(TM_STRING_VIEW)
    operator TM_STRING_VIEW() const;
#endif
} tmf_contents;

typedef struct {
    tmf_contents contents;
    tm_errc ec;
} tmf_contents_result;

typedef uint64_t tmf_file_time;
typedef struct {
    tmf_file_time file_time;
    tm_errc ec;
} tmf_file_timestamp_result;

typedef struct {
    tm_bool exists;
    tm_errc ec;
} tmf_exists_result;

tmf_exists_result tmf_file_exists(const char* filename);
tmf_exists_result tmf_directory_exists(const char* dir);
tmf_file_timestamp_result tmf_file_timestamp(const char* filename);
int tmf_compare_file_time(tmf_file_time a, tmf_file_time b);

tmf_contents_result tmf_read_file(const char* filename);
tmf_contents_result tmf_read_file_as_utf8(const char* filename, tm_bool validate);
void tmf_destroy_contents(tmf_contents* contents);

typedef struct {
    tm_size_t written;
    tm_errc ec;
} tmf_write_file_result;

tmf_write_file_result tmf_write_file(const char* filename, const void* data, tm_size_t size);
tmf_write_file_result tmf_write_file_as_utf8(const char* filename, const char* data, tm_size_t size);

enum {
    tmf_create_directory_tree = (1u << 0u),
    tmf_overwrite = (1u << 1u),
    tmf_write_byte_order_marker = (1u << 2u),

    /* Write file by first writing to a temporary file, then move file into destination. */
    tmf_atomic_write = (1u << 4u),
};
tmf_write_file_result tmf_write_file_ex(const char* filename, const void* data, tm_size_t size, uint32_t flags);
tmf_write_file_result tmf_write_file_as_utf8_ex(const char* filename, const char* data, tm_size_t size, uint32_t flags);

tm_errc tmf_rename_file(const char* from, const char* to);
tm_errc tmf_rename_file_ex(const char* from, const char* to, uint32_t flags);

tm_errc tmf_delete_file(const char* filename);

tm_errc tmf_create_directory(const char* dir);
tm_errc tmf_delete_directory(const char* dir);

tmf_contents_result tmf_current_working_directory(tm_size_t extra_size);

#if defined(__cplusplus)

struct tmf_contents_managed : tmf_contents {
    tmf_contents_managed();
    tmf_contents_managed(tmf_contents_managed&& other);
    tmf_contents_managed& operator=(tmf_contents_managed&& other);
    ~tmf_contents_managed();
};

struct tmf_read_file_managed_result {
    tmf_contents_managed contents;
    tm_errc ec;
};
tmf_read_file_managed_result tmf_read_file_managed(const char* filename);
tmf_read_file_managed_result tmf_read_file_as_utf8_managed(const char* filename, tm_bool validate);

#if defined(TMF_USE_STL)
std::vector<char> tmf_read_file_to_vector(const char* filename);
std::vector<char> tmf_read_file_as_utf8_to_vector(const char* filename);
#endif /* defined(TMF_USE_STL) */

#if defined(TM_STRING_VIEW)
tmf_exists_result tmf_file_exists(TM_STRING_VIEW filename);
tmf_exists_result tmf_directory_exists(TM_STRING_VIEW dir);
tmf_file_timestamp_result tmf_file_timestamp(TM_STRING_VIEW filename);
tmf_contents_result tmf_read_file(TM_STRING_VIEW filename);
tmf_write_file_result tmf_write_file(TM_STRING_VIEW filename, const void* data, tm_size_t size);
tmf_write_file_result tmf_write_file_ex(TM_STRING_VIEW filename, const void* data, tm_size_t size, uint32_t flags);

tmf_contents_result tmf_read_file_as_utf8(TM_STRING_VIEW filename, tm_bool validate);
tmf_write_file_result tmf_write_file_as_utf8(TM_STRING_VIEW filename, const char* data, tm_size_t size);
tmf_write_file_result tmf_write_file_as_utf8_ex(TM_STRING_VIEW filename, const char* data, tm_size_t size,
                                                uint32_t flags);

tm_errc tmf_rename_file(TM_STRING_VIEW from, TM_STRING_VIEW to);
tm_errc tmf_rename_file_ex(TM_STRING_VIEW from, TM_STRING_VIEW to, uint32_t flags);
tmf_read_file_managed_result tmf_read_file_managed(TM_STRING_VIEW filename);
tmf_read_file_managed_result tmf_read_file_as_utf8_managed(TM_STRING_VIEW filename, tm_bool validate);

tm_errc tmf_create_directory(TM_STRING_VIEW dir);
tm_errc tmf_delete_directory(TM_STRING_VIEW dir);

#if defined(TMF_USE_STL)
std::vector<char> tmf_read_file_to_vector(TM_STRING_VIEW filename);
std::vector<char> tmf_read_file_as_utf8_to_vector(TM_STRING_VIEW filename);
#endif /* defined(TMF_USE_STL) */
#endif /* defined(TM_STRING_VIEW) */

#endif /* defined(__cplusplus)*/

#endif  // _TM_FILE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765

#ifdef TM_FILE_IMPLEMENTATION

#include "../common/tm_unreferenced_param.inc"

#include "../common/tm_static_assert.inc"

#include "../common/tm_assert_valid_size.inc"

#include "../common/tm_null.inc"

/* Byte order marks for all encodings we can decode. */
static const unsigned char tmf_utf8_bom[3] = {0xEF, 0xBB, 0xBF};
static const unsigned char tmf_utf16_be_bom[2] = {0xFE, 0xFF};
static const unsigned char tmf_utf16_le_bom[2] = {0xFF, 0xFE};
static const unsigned char tmf_utf32_be_bom[4] = {0x00, 0x00, 0xFE, 0xFF};
static const unsigned char tmf_utf32_le_bom[4] = {0xFF, 0xFE, 0x00, 0x00};

void tmf_to_tmf_path(tmf_contents* path, tm_bool is_dir) {
    TM_ASSERT(path);
    TM_ASSERT((path->data && path->capacity > 0) || (!path->data && path->capacity == 0));

    for (char* str = path->data; *str; ++str) {
        if (*str == '\\') *str = '/';
    }

    if (is_dir && path->size > 0) {
        /* Append '/' at the end of the directory path if it doesn't exist. */
        if (path->data[path->size - 1] != '/') {
            path->data[path->size++] = '/';
            TM_ASSERT(path->size <= path->capacity);
        }
    }
}
static tm_errc tmf_create_directory_internal(const WCHAR* dir, tm_size_t dir_len);

#ifdef TMF_USE_WINDOWS_H
#include "windows_implementation.cpp"
#endif

#ifdef TMF_USE_CRT
/* Needs wchar.h on MSVC. */
#include "msvc_crt.cpp"
#endif

#include "utf_conversion.cpp"

#include "common_implementation.cpp"

#undef TMF_MALLOC
#undef TMF_FREE
#undef TMF_STRCHRW

#endif

#include "../common/LICENSE.inc"