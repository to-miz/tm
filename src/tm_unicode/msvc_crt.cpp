typedef struct tmu_platform_path_struct {
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
    out->sbo[0] = 0;
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
            TM_ASSERT(conv_result.size + 2 <= result.contents.capacity);
            result.contents.size = conv_result.size;
            tmu_to_tmu_path(&result.contents, /*is_dir=*/TM_TRUE);
        }
    } else {
        result.ec = conv_result.ec;
    }
    free(dir); /* _wgetcwd calls specifically malloc, we need to directly use free instead of TMU_FREE.*/
    return result;
}

struct tmu_internal_find_data {
    intptr_t handle;
    struct _wfinddata64_t data;
    tm_bool has_data;
    tm_errc next_ec;
};

TMU_DEF tmu_opened_dir tmu_open_directory_t(tmu_platform_path* dir) {
    tmu_opened_dir result;
    memset(&result, 0, sizeof(tmu_opened_dir));

    const tmu_char16* path = TM_NULL;
    if (!tmu_internal_append_wildcard(dir, &path)) {
        result.ec = TM_ENOMEM;
        return result;
    }

    struct tmu_internal_find_data* find_data
        = (struct tmu_internal_find_data*)TMU_MALLOC(sizeof(struct tmu_internal_find_data), sizeof(void*));
    if (!find_data) {
        result.ec = TM_ENOMEM;
        return result;
    }

    memset(find_data, 0, sizeof(struct tmu_internal_find_data));
    find_data->handle = _wfindfirst64(path, &find_data->data);
    if (find_data->handle == -1) {
        result.ec = (tm_errc)errno;
        TMU_FREE(find_data, sizeof(struct tmu_internal_find_data), sizeof(void*));
        return result;
    }
    find_data->has_data = TM_TRUE;
    result.internal = find_data;
    return result;
}

TMU_DEF void tmu_close_directory(tmu_opened_dir* dir) {
    if (!dir) return;
    if (dir->internal) {
        struct tmu_internal_find_data* find_data = (struct tmu_internal_find_data*)dir->internal;
        if (find_data->handle != -1) {
            _findclose(find_data->handle);
        }
        TMU_FREE(find_data, sizeof(struct tmu_internal_find_data), sizeof(void*));
    }
    tmu_destroy_contents(&dir->internal_buffer);
    memset(dir, 0, sizeof(tmu_opened_dir));
}

TMU_DEF const tmu_read_directory_result* tmu_read_directory(tmu_opened_dir* dir) {
    if (!dir) return TM_NULL;
    if (dir->ec != TM_OK) return TM_NULL;
    if (!dir->internal) return TM_NULL;

    struct tmu_internal_find_data* find_data = (struct tmu_internal_find_data*)dir->internal;
    if (find_data->handle == -1) {
        dir->ec = TM_EPERM;
        return TM_NULL;
    }
    if (!find_data->has_data) {
        dir->ec = find_data->next_ec;
        return TM_NULL;
    }

    /* Skip "." and ".." entries. */
    while (find_data->has_data
           && ((find_data->data.name[0] == '.' && find_data->data.name[1] == 0)
               || (find_data->data.name[0] == '.' && find_data->data.name[1] == '.' && find_data->data.name[2] == 0))) {
        find_data->has_data = (_wfindnext64(find_data->handle, &find_data->data) == 0);
        if (!find_data->has_data) {
            int last_error = errno;
            if (last_error != ENOENT) dir->ec = (tm_errc)last_error;
            return TM_NULL;
        }
    }

    memset(&dir->internal_result, 0, sizeof(tmu_read_directory_result));
    tmu_utf16_stream stream = tmu_utf16_make_stream(find_data->data.name);
    tmu_conversion_result conv_result
        = tmu_utf8_from_utf16_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL, 0, /*nullterminate=*/TM_TRUE,
                                 dir->internal_buffer.data, dir->internal_buffer.capacity);
    if (conv_result.ec == TM_ERANGE) {
        if (conv_result.size < 260) conv_result.size = 260;
        if (!dir->internal_buffer.data) {
            dir->internal_buffer.data = (char*)TMU_MALLOC((size_t)conv_result.size, sizeof(char));
            if (!dir->internal_buffer.data) {
                dir->ec = TM_ENOMEM;
                return TM_NULL;
            }
            dir->internal_buffer.capacity = conv_result.size;
        } else if (dir->internal_buffer.capacity < conv_result.size) {
            dir->internal_buffer.size = 0;
            if (!tmu_grow_by(&dir->internal_buffer, conv_result.size)) {
                dir->ec = TM_ENOMEM;
                return TM_NULL;
            }
        }
        TM_ASSERT(dir->internal_buffer.data);
        conv_result
            = tmu_utf8_from_utf16_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL, 0, /*nullterminate=*/TM_TRUE,
                                     dir->internal_buffer.data, dir->internal_buffer.capacity);
    }
    if (conv_result.ec != TM_OK) {
        dir->ec = conv_result.ec;
        return TM_NULL;
    }

    TM_ASSERT(conv_result.size < dir->internal_buffer.capacity);
    dir->internal_buffer.data[conv_result.size] = 0; /* Always nullterminate. */
    dir->internal_buffer.size = conv_result.size;

    dir->internal_result.is_file = (find_data->data.attrib & _A_SUBDIR) == 0;
    dir->internal_result.name = dir->internal_buffer.data;

    find_data->has_data = (_wfindnext64(find_data->handle, &find_data->data) == 0);
    if (!find_data->has_data) {
        int last_error = errno;
        if (last_error != ENOENT) find_data->next_ec = (tm_errc)last_error;
    }
    return &dir->internal_result;
}

/* For mingw on Windows. It could be that these are not declared, but since mingw links against microsoft libraries,
we should be able to declare them ourselves.
NOTE: We don't use _get_wpgmptr, because it needs wmain to be used. This solution should work for main() entry points also. */
#if !defined(_MSC_VER) && !defined(__wargv) && !defined(__argv)
    #ifdef __cplusplus
        extern "C" wchar_t** __wargv;
        extern "C" char** __argv;
    #else
        extern char** __argv;
        extern wchar_t** __wargv;
    #endif
#endif

TMU_DEF tmu_contents_result tmu_module_filename() {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_EPERM};

#if 0
    wchar_t* module_filename = TM_NULL;
    result.ec = (tm_errc)_get_wpgmptr(&module_filename);
    if (result.ec == TM_OK && module_filename) {
        tmu_utf16_stream stream = tmu_utf16_make_stream(module_filename);
        tmu_conversion_result conv_result
            = tmu_utf8_from_utf16_dynamic_ex(stream, tmu_validate_error,
                                             /*replace_str=*/TM_NULL, /*replace_str_len=*/0,
                                             /*nullterminate=*/TM_TRUE, /*is_sbo=*/TM_FALSE, &result.contents);
        result.ec = conv_result.ec;
    }
#else
    if (__wargv && __wargv[0]) {
        tmu_utf16_stream stream = tmu_utf16_make_stream(__wargv[0]);
        tmu_conversion_result conv_result
            = tmu_utf8_from_utf16_dynamic_ex(stream, tmu_validate_error,
                                             /*replace_str=*/TM_NULL, /*replace_str_len=*/0,
                                             /*nullterminate=*/TM_TRUE, /*is_sbo=*/TM_FALSE, &result.contents);
        result.ec = conv_result.ec;
    }

    if (result.contents.data == TM_NULL && __argv && __argv[0]) {
        size_t len = TMU_STRLEN(__argv[0]);
        if (len == 0) {
            result.ec = TM_EPERM;
            return result;
        }
        char* data = (char*)TMU_MALLOC((len + 1), sizeof(char));
        if (!data) {
            result.ec = TM_ENOMEM;
            return result;
        }
        TMU_MEMCPY(data, __argv[0], (len + 1) * sizeof(char));
        result.ec = TM_OK;
        result.contents.data = data;
        result.contents.size = (tm_size_t)len;
        result.contents.capacity = (tm_size_t)len + 1;
    }
#endif

    if (result.ec == TM_OK) {
        tmu_to_tmu_path(&result.contents, /*is_dir=*/TM_FALSE);
    }

    return result;
}

#if defined(TMU_USE_CONSOLE)

struct tmu_console_state_t {
    FILE* stream;
    tm_bool is_redirected_to_file;
};

static struct tmu_console_state_t tmu_console_state[3];

#ifndef TMU_TESTING
#include <fcntl.h>
#endif

TMU_DEF void tmu_console_output_init() {
    tmu_console_state[tmu_console_in].stream = stdin;
    tmu_console_state[tmu_console_in].is_redirected_to_file = TM_FALSE;

    tmu_console_state[tmu_console_out].stream = stdout;
    tmu_console_state[tmu_console_out].is_redirected_to_file = !_isatty(_fileno(stdout));

    tmu_console_state[tmu_console_err].stream = stderr;
    tmu_console_state[tmu_console_err].is_redirected_to_file = !_isatty(_fileno(stderr));

    if (!tmu_console_state[tmu_console_out].is_redirected_to_file) {
        _setmode(_fileno(stdout), _O_U16TEXT);
    }
    if (!tmu_console_state[tmu_console_err].is_redirected_to_file) {
        _setmode(_fileno(stderr), _O_U16TEXT);
    }
}

TMU_DEF tm_bool tmu_console_output(tmu_console_handle handle, const char* str) {
    TM_ASSERT(str);
    return tmu_console_output_n(handle, str, (tm_size_t)TMU_STRLEN(str));
}

TMU_DEF tm_bool tmu_console_output_n(tmu_console_handle handle, const char* str, tm_size_t len) {
    TM_ASSERT(str || len == 0);
    if (handle <= tmu_console_in || handle > tmu_console_err) return TM_FALSE;
    if (!len) return TM_TRUE;

    if (tmu_console_state[handle].is_redirected_to_file) {
        return fwrite(str, sizeof(char), (size_t)len, tmu_console_state[handle].stream) == (size_t)len;
    }

    tmu_char16 sbo[TMU_SBO_SIZE];

    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, len);
    tmu_conversion_result conv_result
        = tmu_utf16_from_utf8_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL,
                                 /*replace_str_len=*/0,
                                 /*nullterminate=*/TM_TRUE, /*out=*/sbo, /*out_len=*/TMU_SBO_SIZE);

    tmu_char16* wide = sbo;
    if (conv_result.ec == TM_ERANGE) {
        wide = (tmu_char16*)TMU_MALLOC(conv_result.size * sizeof(tmu_char16), sizeof(tmu_char16));
        if (wide) {
            tmu_conversion_result new_result
                = tmu_utf16_from_utf8_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL,
                                         /*replace_str_len=*/0,
                                         /*nullterminate=*/TM_TRUE, wide, conv_result.size);
            conv_result.ec = new_result.ec;
        } else {
            conv_result.ec = TM_ENOMEM;
        }
    }

    tm_size_t written = 0;
    if (conv_result.ec == TM_OK) {
        int print_result = fwprintf(tmu_console_state[handle].stream, TMU_TEXT("%ls"), wide);
        if (print_result >= 0) written = (tm_size_t)print_result;
    }

    if (wide && wide != sbo) {
        TMU_FREE(wide, conv_result.size * sizeof(tmu_char16), sizeof(tmu_char16));
    }
    return written == conv_result.size;
}

#endif