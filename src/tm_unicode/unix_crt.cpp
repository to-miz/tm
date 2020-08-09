typedef struct tmu_platform_path_struct {
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
        result.contents.size = (tm_size_t)TMU_STRLEN(result.contents.data);

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

TMU_DEF tmu_contents_result tmu_module_filename() {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    char sbo[260];

    char* filename = sbo;
    ssize_t filename_size = 260;

    ssize_t size = readlink("/proc/self/exe", filename, filename_size);
    int last_error = errno;
    if (size >= filename_size) {
        filename_size *= 2;
        char* new_filename = (char*)TMU_MALLOC(filename_size * sizeof(char), sizeof(char));
        if (!new_filename) {
            result.ec = TM_ENOMEM;
            return result;
        }
        filename = new_filename;

        for (;;) {
            size = readlink("/proc/self/exe", filename, filename_size);
            last_error = errno;
            if (size < 0) break;
            if (size >= filename_size) {
                new_filename = (char*)TMU_REALLOC(filename, filename_size * sizeof(char), sizeof(char),
                                                  filename_size * sizeof(char) * 2, sizeof(char));
                if (!new_filename) {
                    result.ec = TM_ENOMEM;
                    break;
                }
                filename = new_filename;
                filename_size *= 2;
                continue;
            }
            break;
        }
    }

    if (size < 0) result.ec = (tm_errc)last_error;

    if (result.ec == TM_OK) {
        if (filename == sbo) {
            filename_size = size + 1;
            result.contents.data = (char*)TMU_MALLOC(filename_size * sizeof(char), sizeof(char));
            if (!result.contents.data) {
                result.ec = TM_ENOMEM;
                return result;
            }
            TMU_MEMCPY(result.contents.data, filename, size * sizeof(char));
            result.contents.data[size] = 0;  // Force nulltermination.
        } else {
            TM_ASSERT(size < filename_size);
            filename[size] = 0;  // Force nulltermination.
            result.contents.data = filename;
        }
        result.contents.size = (tm_size_t)size;
        result.contents.capacity = (tm_size_t)filename_size;
    } else {
        TMU_FREE(filename, filename_size * sizeof(char), sizeof(char));
    }
    return result;
}

#if 0
struct tmu_internal_find_data {
    DIR* handle;
    char* dir;
    char* prefix;
    char* suffix;
};
#endif

TMU_DEF tmu_opened_dir tmu_open_directory_t(tmu_platform_path* dir) {
    TM_ASSERT(dir);
    TM_ASSERT(dir->path);

    tmu_opened_dir result;
    memset(&result, 0, sizeof(tmu_opened_dir));

    const char* path = dir->path;
    if (!path || *path == 0) path = ".";

    DIR* handle = opendir(path);
    if (!handle) {
        result.ec = errno;
        return result;
    }

    result.internal = handle;
    return result;
}

TMU_DEF void tmu_close_directory(tmu_opened_dir* dir) {
    if (!dir) return;
    if (dir->internal) {
        closedir((DIR*)dir->internal);
    }
    memset(dir, 0, sizeof(tmu_opened_dir));
}

TMU_DEF const tmu_read_directory_result* tmu_read_directory(tmu_opened_dir* dir) {
    if (!dir) return TM_NULL;
    if (!dir->internal) return TM_NULL;
    DIR* handle = (DIR*)dir->internal;

    struct dirent* entry = TM_NULL;
    for (;;) {
        errno = 0;
        entry = readdir(handle);
        if (!entry) {
            int last_error = errno;
            if (last_error != 0) dir->ec = last_error;
            memset(&dir->internal_result, 0, sizeof(tmu_read_directory_result));
            return TM_NULL;
        }

        /* Skip "." and ".." entries. */
        if ((entry->d_name[0] == '.' && entry->d_name[1] == 0)
            || (entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == 0))
            continue;
        break;
    }

    dir->internal_result.name = entry->d_name;
    dir->internal_result.is_file = ((entry->d_type & DT_DIR) == 0);
    return &dir->internal_result;
}

#if defined(TMU_USE_CONSOLE)

TMU_DEF void tmu_console_output_init() {}
TMU_DEF tm_bool tmu_console_output(tmu_console_handle handle, const char* str) {
    TM_ASSERT(str);
    return tmu_console_output_n(handle, str, (tm_size_t)TMU_STRLEN(str));
}
TMU_DEF tm_bool tmu_console_output_n(tmu_console_handle handle, const char* str, tm_size_t len) {
    TM_ASSERT(str || len == 0);
    if (handle <= tmu_console_in || handle > tmu_console_err) return TM_FALSE;
    if (!len) return TM_TRUE;

    FILE* files[3] = {stdin, stdout, stderr};
    return fwrite(str, sizeof(char), (size_t)len, files[handle]) == (size_t)len;
}

#endif