typedef struct {
    const tmu_tchar* path;
    tmu_tchar sbo[TMU_SBO_SIZE];
    /* Not the length of the path string, but the number of bytes allocated, if path is malloced. */
    tm_size_t allocated_size;
} tmu_platform_path;

static tm_bool tmu_to_platform_path(const char* path, tmu_platform_path* out) {
    TM_ASSERT(out);
    out->path = path;
    out->allocated_size = 0;
    return TM_TRUE;
}

static tmu_tchar* tmu_to_platform_path_t(const tmu_tchar* path, tm_size_t size, tmu_platform_path* out);

#if defined(TM_STRING_VIEW)
static tmu_tchar* tmu_to_platform_path_n(const tmu_tchar* path, tm_size_t size, tmu_platform_path* out) {
    return tmu_to_platform_path_t(path, size, out);
}
#endif /* defined(TM_STRING_VIEW) */

static FILE* tmu_fopen_t(const tmu_tchar* filename, const tmu_tchar* mode) { return fopen(filename, mode); }
static FILE* tmu_freopen_t(const tmu_tchar* filename, const tmu_tchar* mode, FILE* current) {
    return freopen(filename, mode, current);
}

TMU_DEF tmu_contents_result tmu_current_working_directory(tm_size_t extra_size) {
    TM_ASSERT_VALID_SIZE(extra_size);

    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    const tm_size_t increment_size = 200;
    result.contents.data = (char*)TMU_MALLOC(increment_size * sizeof(char), sizeof(char));
    if (!result.contents.data) {
        result.ec = TM_ENOMEM;
    } else {
        result.contents.capacity = increment_size;
        while (getcwd(result.contents.data, result.contents.capacity) == TM_NULL) {
            if (errno == ERANGE) {
                tm_size_t new_size = result.contents.capacity + increment_size;
                char* new_data = (char*)TMU_REALLOC(result.contents.data, result.contents.capacity * sizeof(char),
                                                    sizeof(char), new_size * sizeof(char), sizeof(char));
                if (!new_data) {
                    tmu_destroy_contents(&result.contents);
                    result.ec = TM_ENOMEM;
                    break;
                }
                result.contents.data = new_data;
                result.contents.capacity = new_size;
            } else {
                result.ec = errno;
                if (result.ec == TM_OK) {
                    /* No error even though getcwd returned NULL, there shouldn't be any useful data. */
                    result.ec = TM_EIO;
                }
                break;
            }
        }
    }

    if (result.ec == TM_OK) {
        result.contents.size = TMU_STRLEN(result.contents.data);

        /* Pad by extra_size and room for trailing '/'. */
        tm_size_t new_size = result.contents.size + extra_size + 1;
        if (new_size > result.contents.capacity) {
            char* new_data = (char*)TMU_REALLOC(result.contents.data, result.contents.capacity * sizeof(char),
                                                sizeof(char), new_size * sizeof(char), sizeof(char));
            if (!new_data) {
                tmu_destroy_contents(&result.contents);
                result.ec = TM_ENOMEM;
            } else {
                result.contents.data = new_data;
                result.contents.capacity = new_size;
            }
        }
    }
    if (result.ec == TM_OK) tmu_to_tmu_path(&result.contents, /*is_dir=*/TM_TRUE);
    return result;
}