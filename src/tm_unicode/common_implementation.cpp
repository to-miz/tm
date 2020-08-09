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

#if defined(_WIN32) && !defined(TMU_TESTING_UNIX)
static tm_bool tmu_internal_append_wildcard(tmu_platform_path* dir, const tmu_tchar** out) {
    TM_ASSERT(dir);
    TM_ASSERT(dir->path);
    TM_ASSERT(out);

    size_t len = TMU_TEXTLEN(dir->path);
    if (len == 0) {
        *out = TMU_TEXT("*");
        return TM_TRUE;
    }

    tm_bool ends_in_slash = (dir->path[len - 1] == TMU_TEXT('\\'));
    size_t required_size = len + 3 - ends_in_slash;
    if (dir->path == dir->sbo) {
        if (required_size > TMU_SBO_SIZE) {
            void* new_path = TMU_MALLOC(required_size, sizeof(tmu_tchar));
            if (!new_path) return TM_FALSE;
            TMU_MEMCPY(new_path, dir->path, (len + 1) * sizeof(tmu_tchar));
            dir->path = (tmu_tchar*)new_path;
        }
    } else {
        void* new_path = TMU_REALLOC(dir->path, dir->allocated_size * sizeof(tmu_tchar), sizeof(tmu_tchar),
                                     required_size * sizeof(tmu_tchar), sizeof(tmu_tchar));
        if (!new_path) return TM_FALSE;
        dir->path = (tmu_tchar*)new_path;
    }
    if (dir->path != dir->sbo) dir->allocated_size = (tm_size_t)required_size;
    len -= ends_in_slash;
    dir->path[len] = TMU_TEXT('\\');
    dir->path[len + 1] = TMU_TEXT('*');
    dir->path[len + 2] = 0;
    len += 2;
    *out = dir->path;
    return TM_TRUE;
}
#endif

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

TMU_DEF tmu_contents_result tmu_module_directory() {
    tmu_contents_result result = tmu_module_filename();
    if (result.ec == TM_OK) {
        for (tm_size_t i = result.contents.size; i > 0 && result.contents.data[i - 1] != '/'; --i) {
            --result.contents.size;
        }
        /* Nullterminate */
        if (result.contents.data) result.contents.data[result.contents.size] = 0;
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

TMU_DEF tmu_opened_dir tmu_open_directory(const char* dir) {
    tmu_opened_dir result = {TM_ENOMEM, {TM_NULL, TM_FALSE}, {TM_NULL, 0, 0}, TM_NULL};
    if (!dir) dir = "";
    tmu_platform_path platform_dir;
    if (tmu_to_platform_path(dir, &platform_dir)) {
        result = tmu_open_directory_t(&platform_dir);
        tmu_destroy_platform_path(&platform_dir);
    }
    return result;
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