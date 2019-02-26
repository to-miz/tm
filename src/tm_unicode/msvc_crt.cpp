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