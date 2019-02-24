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
