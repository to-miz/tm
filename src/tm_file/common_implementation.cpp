static tm_errc tmf_create_directory_internal(const tmf_tchar* dir, tm_size_t dir_len) {
    if (dir_len <= 0) return TM_OK;
    if (dir_len == 1 && dir[0] == TMF_TEXT('\\')) return TM_OK;
    if (dir_len == 2 && (dir[0] == TMF_TEXT('.') || dir[0] == TMF_TEXT('~')) && dir[1] == TMF_TEXT('\\')) return TM_OK;

    tm_size_t dir_str_len = dir_len + 1;
    tmf_tchar* dir_str = TMF_MALLOC(tmf_tchar, dir_str_len, sizeof(tmf_tchar));
    if (!dir_str) return TM_ENOMEM;
    TMF_MEMCPY(dir_str, dir, dir_len * sizeof(tmf_tchar));
    dir_str[dir_str_len] = 0;

    if (tmf_directory_exists_t(dir_str).exists) {
        TMF_FREE(dir_str, dir_str_len, sizeof(tmf_tchar));
        return TM_OK;
    }

    /* Create directory tree recursively. */
    tm_errc result = TM_OK;
    tmf_tchar* end = TMF_TEXTCHR(dir_str, TMF_TEXT('\\'));
    for (;;) {
        tm_bool was_null = (end == TM_NULL);
        if (!was_null) *end = 0;

        result = tmf_create_single_directory_t(dir_str);
        if (result != TM_OK) break;

        if (was_null || *(end + 1) == 0) break;
        *end = TMF_TEXT('\\';)
        end = TMF_TEXTCHR(end + 1, TMF_TEXT('\\'));
    }
    TMF_FREE(dir_str, dir_str_len, sizeof(tmf_tchar));
    return result;
}

tm_errc tmf_create_directory_t(const tmf_tchar* dir) {
    return tmf_create_directory_internal(dir, (tm_size_t)TMF_TEXTLEN(dir));
}

int tmf_compare_file_time(tmf_file_time a, tmf_file_time b) {
    int64_t cmp = (int64_t)(a - b);
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}

void tmf_destroy_contents(tmf_contents* contents) {
    if (contents) {
        if (contents->data) {
            TM_ASSERT(contents->capacity > 0);
            TMF_FREE(contents->data, contents->capacity, sizeof(char));
        }
        contents->data = TM_NULL;
        contents->size = 0;
        contents->capacity = 0;
    }
}

tmf_contents_result tmf_read_file(const char* filename) {
    tmf_platform_path platform_filename = tmf_to_platform_path(filename);
    tmf_contents_result result = tmf_read_file_t(platform_filename.path);
    tmf_destroy_platform_path(&platform_filename);
    return result;
}

tmf_write_file_result tmf_write_file(const char* filename, const void* data, tm_size_t size) {
    return tmf_write_file_ex(filename, data, size, tmf_overwrite);
}
tmf_write_file_result tmf_write_file_as_utf8(const char* filename, const char* data, tm_size_t size) {
    return tmf_write_file_as_utf8_ex(filename, data, size, tmf_overwrite | tmf_write_byte_order_marker);
}

tmf_write_file_result tmf_write_file_ex(const char* filename, const void* data, tm_size_t size, uint32_t flags) {
    tmf_write_file_result result = {0, TM_ENOMEM};
    tmf_platform_path platform_filename = tmf_to_platform_path(filename);
    if (platform_filename.path) {
        flags &= ~tmf_write_byte_order_marker;
        result = tmf_write_file_ex_t(platform_filename.path, data, size, flags);
        tmf_destroy_platform_path(&platform_filename);
    }
    return result;
}

tmf_contents_result tmf_read_file_as_utf8(const char* filename, tm_bool validate) {
    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};
    tmf_contents_result file = tmf_read_file(filename);
    if (file.ec == TM_OK) {
        result = tmf_convert_to_utf8(file.contents, validate);
        if (result.contents.data != file.contents.data) tmf_destroy_contents(&file.contents);
    }
    return result;
}
tmf_write_file_result tmf_write_file_as_utf8_ex(const char* filename, const char* data, tm_size_t size,
                                                uint32_t flags) {
    tmf_write_file_result result = {0, TM_ENOMEM};
    tmf_platform_path platform_filename = tmf_to_platform_path(filename);
    if (platform_filename.path) {
        result = tmf_write_file_ex_t(platform_filename.path, data, size, flags);
        tmf_destroy_platform_path(&platform_filename);
    }
    return result;
}

#if defined(__cplusplus)

#if defined(TM_STRING_VIEW)
tmf_contents::operator TM_STRING_VIEW() const { return {data, size}; }

static tmf_platform_path tmf_to_platform_path(TM_STRING_VIEW str) {
    return tmf_to_platform_path_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str));
}

tmf_exists_result tmf_file_exists(TM_STRING_VIEW filename) {
    tmf_exists_result result = {TM_FALSE, TM_ENOMEM};
    tmf_platform_path platform_filename = tmf_to_platform_path(filename);
    if (platform_filename.path) {
        result = tmf_file_exists_t(platform_filename.path);
        tmf_destroy_platform_path(&platform_filename);
    }
    return result;
}
tmf_exists_result tmf_directory_exists(TM_STRING_VIEW dir) {
    tmf_exists_result result = {TM_FALSE, TM_ENOMEM};
    tmf_platform_path platform_dir = tmf_to_platform_path(dir);
    if (platform_dir.path) {
        result = tmf_directory_exists_t(platform_dir.path);
        tmf_destroy_platform_path(&platform_dir);
    }
    return result;
}
tmf_file_timestamp_result tmf_file_timestamp(TM_STRING_VIEW filename) {
    tmf_file_timestamp_result result = {0, TM_ENOMEM};
    tmf_platform_path platform_filename = tmf_to_platform_path(filename);
    if (platform_filename.path) {
        result = tmf_file_timestamp_t(platform_filename.path);
        tmf_destroy_platform_path(&platform_filename);
    }
    return result;
}
tmf_contents_result tmf_read_file(TM_STRING_VIEW filename) {
    tmf_contents_result result = {{TM_NULL, 0}, TM_ENOMEM};
    tmf_platform_path platform_filename = tmf_to_platform_path(filename);
    if (platform_filename.path) {
        result = tmf_read_file_t(platform_filename.path);
        tmf_destroy_platform_path(&platform_filename);
    }
    return result;
}
tmf_write_file_result tmf_write_file(TM_STRING_VIEW filename, const void* data, tm_size_t size) {
    return tmf_write_file_ex(filename, data, size, tmf_overwrite);
}
tmf_write_file_result tmf_write_file_ex(TM_STRING_VIEW filename, const void* data, tm_size_t size, uint32_t flags) {
    tmf_write_file_result result = {0, TM_ENOMEM};
    tmf_platform_path platform_filename = tmf_to_platform_path(filename);
    if (platform_filename.path) {
        flags &= ~tmf_write_byte_order_marker;
        result = tmf_write_file_ex_t(platform_filename.path, data, size, flags);
        tmf_destroy_platform_path(&platform_filename);
    }
    return result;
}

tmf_contents_result tmf_read_file_as_utf8(TM_STRING_VIEW filename, tm_bool validate) {
    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};
    tmf_contents_result file = tmf_read_file(filename);
    if (file.ec == TM_OK) {
        result = tmf_convert_to_utf8(file.contents, validate);
        if (result.contents.data != file.contents.data) tmf_destroy_contents(&file.contents);
    }
    return result;
}
tmf_write_file_result tmf_write_file_as_utf8(TM_STRING_VIEW filename, const char* data, tm_size_t size) {
    return tmf_write_file_as_utf8_ex(filename, data, size, tmf_overwrite | tmf_write_byte_order_marker);
}
tmf_write_file_result tmf_write_file_as_utf8_ex(TM_STRING_VIEW filename, const char* data, tm_size_t size,
                                                uint32_t flags) {
    tmf_write_file_result result = {0, TM_ENOMEM};
    tmf_platform_path platform_filename = tmf_to_platform_path(filename);
    if (platform_filename.path) {
        result = tmf_write_file_ex_t(platform_filename.path, data, size, flags);
        tmf_destroy_platform_path(&platform_filename);
    }
    return result;
}

tm_errc tmf_rename_file(TM_STRING_VIEW from, TM_STRING_VIEW to) { return tmf_rename_file_ex(from, to, 0); }
tm_errc tmf_rename_file_ex(TM_STRING_VIEW from, TM_STRING_VIEW to, uint32_t flags) {
    tm_errc result = TM_ENOMEM;
    tmf_platform_path platform_from = tmf_to_platform_path(from);
    tmf_platform_path platform_to = tmf_to_platform_path(to);
    if (platform_from.path && platform_to.path) {
        result = tmf_rename_file_ex_t(platform_from.path, platform_to.path, flags);
    }
    tmf_destroy_platform_path(&platform_from);
    tmf_destroy_platform_path(&platform_to);
    return result;
}
tmf_read_file_managed_result tmf_read_file_managed(TM_STRING_VIEW filename) {
    tmf_contents_result read_file_result = tmf_read_file(filename);

    tmf_read_file_managed_result result;
    result.contents.data = read_file_result.contents.data;
    result.contents.size = read_file_result.contents.size;
    result.ec = read_file_result.ec;
    return result;
}
tmf_read_file_managed_result tmf_read_file_as_utf8_managed(TM_STRING_VIEW filename, tm_bool validate) {
    tmf_contents_result read_file_result = tmf_read_file_as_utf8(filename, validate);

    tmf_read_file_managed_result result;
    result.contents.data = read_file_result.contents.data;
    result.contents.size = read_file_result.contents.size;
    result.ec = read_file_result.ec;
    return result;
}

tm_errc tmf_create_directory(TM_STRING_VIEW dir) {
    tm_errc result = TM_ENOMEM;
    tmf_platform_path platform_dir = tmf_to_platform_path(dir);
    if (platform_dir.path) {
        result = tmf_create_directory_t(platform_dir.path);
        tmf_destroy_platform_path(&platform_dir);
    }
    return result;
}
tm_errc tmf_delete_directory(TM_STRING_VIEW dir) {
    tm_errc result = TM_ENOMEM;
    tmf_platform_path platform_dir = tmf_to_platform_path(dir);
    if (platform_dir.path) {
        result = tmf_delete_directory_t(platform_dir.path);
        tmf_destroy_platform_path(&platform_dir);
    }
    return result;
}

#endif

tmf_contents_managed::tmf_contents_managed() {
    data = nullptr;
    size = 0;
}
tmf_contents_managed::tmf_contents_managed(tmf_contents_managed&& other) {
    data = other.data;
    size = other.size;
    other.data = nullptr;
    other.size = 0;
}
tmf_contents_managed& tmf_contents_managed::operator=(tmf_contents_managed&& other) {
    if (this != &other) {
        /* Swap contents and let destructor of other release memory. */
        char* temp_data = data;
        data = other.data;
        other.data = temp_data;

        tm_size_t temp_size = size;
        size = other.size;
        other.size = temp_size;
    }
    return *this;
}
tmf_contents_managed::~tmf_contents_managed() { tmf_destroy_contents(this); }

tmf_read_file_managed_result tmf_read_file_managed(const char* filename) {
    tmf_contents_result read_file_result = tmf_read_file(filename);

    tmf_read_file_managed_result result;
    result.contents.data = read_file_result.contents.data;
    result.contents.size = read_file_result.contents.size;
    result.ec = read_file_result.ec;
    return result;
}

tmf_read_file_managed_result tmf_read_file_as_utf8_managed(const char* filename, tm_bool validate) {
    tmf_contents_result read_file_result = tmf_read_file_as_utf8(filename, validate);

    tmf_read_file_managed_result result;
    result.contents.data = read_file_result.contents.data;
    result.contents.size = read_file_result.contents.size;
    result.ec = read_file_result.ec;
    return result;
}

#endif /* defined(__cplusplus)*/