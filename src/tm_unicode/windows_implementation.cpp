typedef struct tmu_platform_path_struct {
    tmu_tchar* path;
    tmu_tchar sbo[TMU_SBO_SIZE];
    tm_size_t allocated_size;
} tmu_platform_path;

// WC_ERR_INVALID_CHARS exists only since Vista.
#if (defined(WINVER) && WINVER >= 0x0600) || (defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0600)
    #define TMU_TO_UTF8_FLAGS WC_ERR_INVALID_CHARS
    #define TMU_FROM_UTF8_FLAGS MB_ERR_INVALID_CHARS
#else
    #define TMU_TO_UTF8_FLAGS 0
    #define TMU_FROM_UTF8_FLAGS 0
#endif

static tm_errc tmu_winerror_to_errc(DWORD error, tm_errc def) {
    switch (error) {
        case ERROR_ACCESS_DENIED:
        case ERROR_CANNOT_MAKE:
        case ERROR_CURRENT_DIRECTORY:
        case ERROR_INVALID_ACCESS:
        case ERROR_NOACCESS:
        case ERROR_SHARING_VIOLATION:
        case ERROR_WRITE_PROTECT:
            return TM_EACCES;

        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS:
            return TM_EEXIST;

        case ERROR_CANTOPEN:
        case ERROR_CANTREAD:
        case ERROR_CANTWRITE:
        case ERROR_OPEN_FAILED:
        case ERROR_READ_FAULT:
        case ERROR_SEEK:
        case ERROR_WRITE_FAULT:
            return TM_EIO;

        case ERROR_DIRECTORY:
        case ERROR_INVALID_HANDLE:
        case ERROR_INVALID_NAME:
        case ERROR_NEGATIVE_SEEK:
        case ERROR_NO_UNICODE_TRANSLATION:
        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_FLAGS:
            return TM_EINVAL;

        case ERROR_INSUFFICIENT_BUFFER:
            return TM_ERANGE;

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            return TM_ENOENT;

        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY:
            return TM_ENOMEM;

        case ERROR_BAD_UNIT:
        case ERROR_DEV_NOT_EXIST:
        case ERROR_INVALID_DRIVE:
            return TM_ENODEV;

        case ERROR_BUSY:
        case ERROR_BUSY_DRIVE:
        case ERROR_DEVICE_IN_USE:
        case ERROR_OPEN_FILES:
            return TM_EBUSY;

        case ERROR_DISK_FULL:
        case ERROR_HANDLE_DISK_FULL:
            return TM_ENOSPC;

        case ERROR_BUFFER_OVERFLOW:
            return TM_ENAMETOOLONG;

        case ERROR_DIR_NOT_EMPTY:
            return TM_ENOTEMPTY;

        case ERROR_NOT_SAME_DEVICE:
            return TM_EXDEV;

        case ERROR_TOO_MANY_OPEN_FILES:
            return TM_EMFILE;

#if 0
        case ERROR_NOT_READY:
        case ERROR_RETRY:
            return TM_EAGAIN;

        case ERROR_INVALID_FUNCTION:
            return TM_ENOSYS;

        case ERROR_LOCK_VIOLATION:
        case ERROR_LOCKED:
            return TM_ENOLCK;

        case ERROR_OPERATION_ABORTED:
            return TM_ECANCELED;
#endif

        default:
            return def;
    }
}

static tm_bool tmu_to_platform_path_n(const char* path, tm_size_t size, tmu_platform_path* out) {
    if (size <= 0) {
        out->path = out->sbo;
        out->sbo[0] = 0;
        out->allocated_size = 0;
        return TM_TRUE;
    }

    int required_size = MultiByteToWideChar(CP_UTF8, TMU_FROM_UTF8_FLAGS, path, (int)size, TM_NULL, 0);
    if (required_size <= 0) return TM_FALSE; /* Size was not zero, so conversion failed. */
    ++required_size;                         /* Extra space for null-terminator. */

    if ((unsigned int)required_size <= TMU_SBO_SIZE) {
        out->path = out->sbo;
        out->allocated_size = 0;
    } else {
        out->path = (WCHAR*)TMU_MALLOC(required_size * sizeof(WCHAR), sizeof(WCHAR));
        if (!out->path) return TM_FALSE;
        out->allocated_size = required_size;
    }

    int converted_size =
        MultiByteToWideChar(CP_UTF8, TMU_FROM_UTF8_FLAGS, path, (int)size, out->path, (int)required_size);
    /* Always nullterminate, since MultiByteToWideChar doesn't null-terminate when the supplying it a length param. */
    if (required_size) {
        if (converted_size <= 0) {
            out->path[0] = 0;
        } else {
            if (converted_size < required_size) {
                out->path[converted_size] = 0;
            } else {
                out->path[required_size - 1] = 0;
            }
        }
    }

    /* Turn path to win32 path. */
    for (WCHAR* cur = out->path;; ++cur) {
        switch (*cur) {
            case 0: {
                return TM_TRUE;
            }
            case L'/': {
                *cur = L'\\';
                break;
            }
        }
    }
}

static tm_bool tmu_to_platform_path(const char* path, tmu_platform_path* out) {
    return tmu_to_platform_path_n(path, (tm_size_t)lstrlenA(path), out);
}

static tmu_exists_result tmu_file_exists_t(const WCHAR* filename) {
    tmu_exists_result result = {TM_FALSE, TM_OK};
    DWORD attributes = GetFileAttributesW(filename);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        DWORD last_error = GetLastError();
        if (last_error == ERROR_FILE_NOT_FOUND || last_error == ERROR_PATH_NOT_FOUND) {
            result.ec = TM_OK;
            result.exists = TM_FALSE;
        } else {
            result.ec = tmu_winerror_to_errc(last_error, TM_EIO);
        }
    } else {
        result.exists = (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }
    return result;
}

static tmu_exists_result tmu_directory_exists_t(const WCHAR* dir) {
    tmu_exists_result result = {TM_FALSE, TM_OK};
    DWORD attributes = GetFileAttributesW(dir);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        DWORD last_error = GetLastError();
        if (last_error == ERROR_FILE_NOT_FOUND || last_error == ERROR_PATH_NOT_FOUND) {
            result.ec = TM_OK;
            result.exists = TM_FALSE;
        } else {
            result.ec = tmu_winerror_to_errc(last_error, TM_EIO);
        }
    } else {
        result.exists = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    return result;
}

TM_STATIC_ASSERT(sizeof(tmu_file_time) == sizeof(FILETIME), invalid_file_time_size);
static tmu_file_timestamp_result tmu_file_timestamp_t(const WCHAR* filename) {
    tmu_file_timestamp_result result = {0, TM_OK};

    WIN32_FILE_ATTRIBUTE_DATA fileAttr = {0};
    if (GetFileAttributesExW(filename, GetFileExInfoStandard, &fileAttr) == 0) {
        result.ec = TMU_NO_SUCH_FILE_OR_DIRECTORY;
        return result;
    }

    TMU_MEMCPY(&result.file_time, &fileAttr.ftLastWriteTime, sizeof(fileAttr.ftLastWriteTime));
    return result;
}

static tmu_contents_result tmu_read_file_t(const WCHAR* filename) {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    HANDLE file =
        CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, TM_NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, TM_NULL);

    if (file == INVALID_HANDLE_VALUE) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(file, &size)) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        CloseHandle(file);
        return result;
    }
    if (size.QuadPart > 0) {
        if (size.QuadPart > INT32_MAX) {
            /* TODO: ReadFile can only read 2^32 bytes at a time, implement reading in chunks. */
            result.ec = TM_EFBIG;
            CloseHandle(file);
            return result;
        }

        tm_size_t data_size = (tm_size_t)size.QuadPart;
        char* data = (char*)TMU_MALLOC(data_size * sizeof(char), sizeof(char));
        if (!data) {
            result.ec = TM_ENOMEM;
            CloseHandle(file);
            return result;
        }

        DWORD bytes_to_read = (DWORD)size.QuadPart;
        DWORD bytes_read = 0;
        BOOL read_file_result = ReadFile(file, data, bytes_to_read, &bytes_read, TM_NULL);
        if (!read_file_result || bytes_read != bytes_to_read) {
            result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
            TMU_FREE(data);
            CloseHandle(file);
            return result;
        }

        result.contents.data = data;
        result.contents.size = data_size;
        result.contents.capacity = data_size;
    }
    CloseHandle(file);
    return result;
}

static tm_errc tmu_create_single_directory_t(const WCHAR* dir) {
    tm_errc result = TM_OK;
    if (!CreateDirectoryW(dir, TM_NULL)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            result = tmu_winerror_to_errc(error, TM_EIO);
        }
    }
    return result;
}

static tmu_write_file_result tmu_write_file_ex_internal(const WCHAR* filename, const void* data, tm_size_t size,
                                                        uint32_t flags) {
    TM_ASSERT_VALID_SIZE(size);
    tmu_write_file_result result = {0, TM_OK};

    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(filename, tmu_get_path_len_internal(filename, /*filename_len=*/0));
        if (ec != TM_OK) {
            result.ec = ec;
            return result;
        }
    }

    DWORD creation_flags = 0;
    if (flags & tmu_overwrite) {
        creation_flags = CREATE_ALWAYS;
    } else {
        creation_flags = CREATE_NEW;
    }
    HANDLE file = CreateFileW(filename, GENERIC_WRITE, 0, TM_NULL, creation_flags, FILE_ATTRIBUTE_NORMAL, TM_NULL);
    if (file == INVALID_HANDLE_VALUE) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }
    if ((size_t)size > (size_t)UINT32_MAX) {
        /* TODO: Implement writing in chunks so there is no UINT32_MAX limit. */
        result.ec = TM_EOVERFLOW;
        CloseHandle(file);
        return result;
    }

    DWORD total_bytes_written = 0;
    DWORD bytes_written = 0;

    if (flags & tmu_write_byte_order_mark) {
        if (!WriteFile(file, tmu_utf8_bom, sizeof(tmu_utf8_bom), &bytes_written, TM_NULL)) {
            result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
            CloseHandle(file);
            return result;
        }
        total_bytes_written += bytes_written;
    }

    DWORD truncated_buffer_size = (DWORD)size;
    if (!WriteFile(file, data, truncated_buffer_size, &bytes_written, TM_NULL)) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        CloseHandle(file);
        return result;
    }
    total_bytes_written += bytes_written;

    CloseHandle(file);
    result.written = (tm_size_t)total_bytes_written;
    return result;
}

static tmu_write_file_result tmu_write_file_ex_t(const WCHAR* filename, const void* data, tm_size_t size,
                                                 uint32_t flags) {
    if (!(flags & tmu_atomic_write)) {
        return tmu_write_file_ex_internal(filename, data, size, flags);
    }

    tmu_write_file_result result = {0, TM_OK};

    tm_size_t filename_len = (tm_size_t)lstrlenW(filename);
    tm_size_t dir_len = tmu_get_path_len_internal(filename, filename_len);
    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(filename, dir_len);
        if (ec != TM_OK) {
            result.ec = ec;
            return result;
        }
    }

    WCHAR* temp_file = TM_NULL;
    WCHAR temp_file_buffer[MAX_PATH];
    const tm_size_t temp_file_len = filename_len + 9;

    /* Limitation of GetTempFileNameW, dir_len cannot be bigger than MAX_PATH - 14. */
    if (dir_len < MAX_PATH - 14) {
        WCHAR temp_dir[MAX_PATH];
        TMU_MEMCPY(temp_dir, filename, dir_len * sizeof(WCHAR));
        temp_dir[dir_len] = 0;
        if (GetTempFileNameW(temp_dir, TMU_TEXT("tmf"), 0, temp_file_buffer) == 0) {
            result.ec = TM_EIO;
            return result;
        }

        temp_file = temp_file_buffer;
    } else {
        /* Fallback to using a simple temp_file. */
        temp_file = (WCHAR*)TMU_MALLOC(temp_file_len * sizeof(WCHAR), sizeof(WCHAR));
        if (!temp_file) {
            result.ec = TM_ENOMEM;
            return result;
        }
        TMU_MEMCPY(temp_file, filename, filename_len * sizeof(WCHAR));
        /* Copy filename ending + null terminator. */
        TMU_MEMCPY(temp_file + filename_len, TMU_TEXT(".tmu_tmp"), sizeof(WCHAR) * 9);
    }

    TM_ASSERT(temp_file);

    tmu_write_file_result temp_write_result = tmu_write_file_ex_internal(temp_file, data, size, 0);
    if (temp_write_result.ec != TM_OK) {
        result.ec = temp_write_result.ec;
        if (temp_file != temp_file_buffer) {
            TMU_FREE(temp_file);
        }
        return result;
    }

    DWORD move_flags = MOVEFILE_COPY_ALLOWED;
    if (flags & tmu_overwrite) move_flags |= MOVEFILE_REPLACE_EXISTING;
    if (!MoveFileExW(temp_file, filename, move_flags)) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        if (temp_file != temp_file_buffer) {
            TMU_FREE(temp_file);
        }
        return result;
    }
    if (temp_file != temp_file_buffer) {
        TMU_FREE(temp_file);
    }
    return result;
}

static tm_errc tmu_rename_file_ex_t(const WCHAR* from, const WCHAR* to, uint32_t flags) {
    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(to, tmu_get_path_len_internal(to, /*filename_len=*/0));
        if (ec != TM_OK) return ec;
    }

    DWORD move_flags = MOVEFILE_COPY_ALLOWED;
    if (flags & tmu_overwrite) move_flags |= MOVEFILE_REPLACE_EXISTING;
    if (!MoveFileExW(from, to, move_flags)) return tmu_winerror_to_errc(GetLastError(), TM_EIO);
    return TM_OK;
}

static tm_errc tmu_delete_file_t(const WCHAR* filename) {
    if (!DeleteFileW(filename)) return tmu_winerror_to_errc(GetLastError(), TM_EIO);
    return TM_OK;
}

static tm_errc tmu_delete_directory_t(const WCHAR* dir) {
    if (!RemoveDirectoryW(dir)) return tmu_winerror_to_errc(GetLastError(), TM_EIO);
    return TM_OK;
}

static tmu_contents_result tmu_to_utf8(const WCHAR* str, tm_size_t extra_size) {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};
    if (str && *str) {
        int size = WideCharToMultiByte(CP_UTF8, TMU_TO_UTF8_FLAGS, str, -1, TM_NULL, 0, TM_NULL, TM_NULL);
        if (size <= 0) {
            result.ec = tmu_winerror_to_errc(GetLastError(), TM_EINVAL);
        } else {
            TM_ASSERT((tm_size_t)extra_size < (tm_size_t)(INT32_MAX - size));
            size += (int)extra_size + 1;
            result.contents.data = (char*)TMU_MALLOC(size * sizeof(char), sizeof(char));
            if (!result.contents.data) {
                result.ec = TM_ENOMEM;
            } else {
                result.contents.capacity = size;

                int real_size = WideCharToMultiByte(CP_UTF8, TMU_TO_UTF8_FLAGS, str, -1, result.contents.data, size,
                                                    TM_NULL, TM_NULL);
                result.contents.size = (tm_size_t)real_size;

                /* Always nullterminate. */
                if (!real_size) {
                    result.contents.data[0] = 0;
                } else {
                    if (real_size < size) {
                        result.contents.data[real_size] = 0;
                    } else {
                        result.contents.data[size - 1] = 0;
                    }
                }
            }
        }
    }
    return result;
}

TMU_DEF tmu_contents_result tmu_current_working_directory(tm_size_t extra_size) {
    TM_ASSERT_VALID_SIZE(extra_size);

    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    /* This will return the size necessary including null-terminator. */
    DWORD len = GetCurrentDirectoryW(0, TM_NULL);
    if (!len) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }
    WCHAR* dir = (WCHAR*)TMU_MALLOC(len * sizeof(WCHAR), sizeof(WCHAR));
    if (!dir) {
        result.ec = TM_ENOMEM;
        return result;
    }

    /* This will return the size written without null-terminator. */
    DWORD written = GetCurrentDirectoryW(len, dir);
    TM_UNREFERENCED(written);
    TM_ASSERT(written + 1 == len);

    result = tmu_to_utf8(dir, extra_size + 1);
    if (result.ec == TM_OK) tmu_to_tmu_path(&result.contents, /*is_dir=*/TM_TRUE);
    TMU_FREE(dir);
    return result;
}

TMU_DEF tmu_contents_result tmu_module_filename() {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    WCHAR sbo[MAX_PATH];

    WCHAR* filename = sbo;
    DWORD filename_size = MAX_PATH;

    DWORD size = GetModuleFileNameW(TM_NULL, filename, filename_size);
    DWORD last_error = GetLastError();
    if (size > 0 && size >= filename_size && (last_error == ERROR_INSUFFICIENT_BUFFER || last_error == ERROR_SUCCESS)) {
        WCHAR* new_filename = (WCHAR*)TMU_MALLOC(filename_size * sizeof(WCHAR) * 2, sizeof(WCHAR));
        if (!new_filename) {
            result.ec = TM_ENOMEM;
            return result;
        }
        filename = new_filename;
        filename_size *= 2;

        for (;;) {
            size = GetModuleFileNameW(TM_NULL, filename, filename_size);
            last_error = GetLastError();
            if (last_error != ERROR_INSUFFICIENT_BUFFER && last_error != ERROR_SUCCESS) {
                break;
            }
            if (size >= filename_size) {
                new_filename = (WCHAR*)TMU_REALLOC(filename, filename_size * sizeof(WCHAR) * 2, sizeof(WCHAR));
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

    if (size == 0) result.ec = tmu_winerror_to_errc(last_error, TM_EIO);

    if (result.ec == TM_OK) {
        TM_ASSERT(size < filename_size);
        filename[size] = 0;  // Force nulltermination.
        result = tmu_to_utf8(filename, 0);
    }
    if (result.ec == TM_OK) tmu_to_tmu_path(&result.contents, /*is_dir=*/TM_FALSE);

    if (filename != sbo) {
        TMU_FREE(filename);
    }
    return result;
}

struct tmu_internal_find_data {
    HANDLE handle;
    WIN32_FIND_DATAW data;
    BOOL has_data;
    tm_errc next_ec;
};

static tmu_opened_dir tmu_open_directory_t(tmu_platform_path* dir) {
    TM_ASSERT(dir);

    tmu_opened_dir result;
    ZeroMemory(&result, sizeof(result));

    const WCHAR* path = TM_NULL;
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

    ZeroMemory(find_data, sizeof(struct tmu_internal_find_data));
    find_data->handle = FindFirstFileW(path, &find_data->data);
    if (find_data->handle == INVALID_HANDLE_VALUE) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        TMU_FREE(find_data);
        return result;
    }

    find_data->has_data = 1;
    result.internal = find_data;
    return result;
}

TMU_DEF void tmu_close_directory(tmu_opened_dir* dir) {
    if (!dir) return;
    if (dir->internal) {
        struct tmu_internal_find_data* find_data = (struct tmu_internal_find_data*)dir->internal;
        if (find_data->handle != INVALID_HANDLE_VALUE) {
            FindClose(find_data->handle);
        }
        TMU_FREE(find_data);
    }
    tmu_destroy_contents(&dir->internal_buffer);
    ZeroMemory(dir, sizeof(tmu_opened_dir));
}

TMU_DEF const tmu_read_directory_result* tmu_read_directory(tmu_opened_dir* dir) {
    if (!dir) return TM_NULL;
    if (dir->ec != TM_OK) return TM_NULL;
    if (!dir->internal) return TM_NULL;

    struct tmu_internal_find_data* find_data = (struct tmu_internal_find_data*)dir->internal;
    if (find_data->handle == INVALID_HANDLE_VALUE) {
        dir->ec = TM_EPERM;
        return TM_NULL;
    }
    if (!find_data->has_data) {
        dir->ec = find_data->next_ec;
        return TM_NULL;
    }

    /* Skip "." and ".." entries. */
    while (find_data->has_data
           && ((find_data->data.cFileName[0] == '.' && find_data->data.cFileName[1] == 0)
               || (find_data->data.cFileName[0] == '.' && find_data->data.cFileName[1] == '.'
                   && find_data->data.cFileName[2] == 0))) {
        find_data->has_data = FindNextFileW(find_data->handle, &find_data->data);
        if (!find_data->has_data) {
            DWORD last_error = GetLastError();
            if (last_error != ERROR_NO_MORE_FILES) dir->ec = tmu_winerror_to_errc(last_error, TM_EPERM);
            return TM_NULL;
        }
    }

    ZeroMemory(&dir->internal_result, sizeof(tmu_read_directory_result));
    int required_size = WideCharToMultiByte(CP_UTF8, TMU_TO_UTF8_FLAGS, find_data->data.cFileName, -1, TM_NULL, 0,
                                            TM_NULL, TM_NULL);
    if (required_size <= 0) {
        dir->ec = tmu_winerror_to_errc(GetLastError(), TM_EPERM);
        return TM_NULL;
    }

    // Additional size for trailing slash and nullterminator.
    required_size += 1 + ((find_data->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    if (!dir->internal_buffer.data) {
        dir->internal_buffer.data = (char*)TMU_MALLOC((size_t)required_size, sizeof(char));
        if (!dir->internal_buffer.data) {
            dir->ec = TM_ENOMEM;
            return TM_NULL;
        }
        dir->internal_buffer.capacity = (tm_size_t)required_size;
    } else if (dir->internal_buffer.capacity < (tm_size_t)required_size) {
        dir->internal_buffer.size = 0;
        if (!tmu_grow_by(&dir->internal_buffer, (tm_size_t)required_size)) {
            dir->ec = TM_ENOMEM;
            return TM_NULL;
        }
    }
    TM_ASSERT(dir->internal_buffer.data);
    int real_size = WideCharToMultiByte(CP_UTF8, TMU_TO_UTF8_FLAGS, find_data->data.cFileName, -1,
                                        dir->internal_buffer.data, (int)dir->internal_buffer.capacity, TM_NULL, TM_NULL);
    if (real_size <= 0 || real_size >= required_size) {
        dir->ec = tmu_winerror_to_errc(GetLastError(), TM_EPERM);
        return TM_NULL;
    }

    TM_ASSERT((tm_size_t)real_size < dir->internal_buffer.capacity);
    dir->internal_buffer.data[real_size] = 0; /* Always nullterminate. */
    dir->internal_buffer.size = (tm_size_t)real_size;
    tmu_to_tmu_path(&dir->internal_buffer, /*is_dir=*/((find_data->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0));

    dir->internal_result.is_file = (find_data->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    dir->internal_result.name = dir->internal_buffer.data;

    find_data->has_data = FindNextFileW(find_data->handle, &find_data->data);
    if (!find_data->has_data) {
        DWORD last_error = GetLastError();
        if (last_error != ERROR_NO_MORE_FILES) find_data->next_ec = tmu_winerror_to_errc(last_error, TM_EPERM);
    }
    return &dir->internal_result;
}

#if !defined(TMU_NO_SHELLAPI)
TMU_DEF tmu_utf8_command_line_result tmu_utf8_winapi_get_command_line() {
    tmu_utf8_command_line_result result = {{TM_NULL, 0, TM_NULL, 0}, TM_OK};

    const WCHAR* wide_command_line = GetCommandLineW();
    if (!wide_command_line) result.ec = TM_EPERM;

    int wide_args_count = 0;
    WCHAR** wide_args = TM_NULL;
    if (result.ec == TM_OK) {
        wide_args = CommandLineToArgvW(wide_command_line, &wide_args_count);
        if (!wide_args) result.ec = tmu_winerror_to_errc(GetLastError(), TM_EPERM);
    }

    if (result.ec == TM_OK) {
        // Safe const cast wide_args.
        result = tmu_utf8_command_line_from_utf16((WCHAR const* const*)wide_args, wide_args_count);
    }

    if (wide_args) {
        LocalFree(wide_args);
        wide_args = TM_NULL;
    }
    return result;
}

#endif /* !defined(TMU_NO_SHELLAPI) */

#if defined(TMU_USE_CONSOLE)

struct tmu_console_state_t {
    HANDLE handle;
    tm_bool is_redirected_to_file;
};

static struct tmu_console_state_t tmu_console_state[3];

TMU_DEF void tmu_console_output_init() {
    DWORD mode = 0;
    DWORD handle_ids[3] = {STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE};
    for (int i = tmu_console_in; i <= tmu_console_err; ++i) {
        tmu_console_state[i].handle = GetStdHandle(handle_ids[i]);
        tmu_console_state[i].is_redirected_to_file = !GetConsoleMode(tmu_console_state[i].handle, &mode);
    }

    // See https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
    const DWORD Utf16Codepage = 1200;
    SetConsoleOutputCP(Utf16Codepage);
    SetConsoleCP(Utf16Codepage);
}
TMU_DEF tm_bool tmu_console_output(tmu_console_handle handle, const char* str) {
    TM_ASSERT(str);
    return tmu_console_output_n(handle, str, TMU_STRLEN(str));
}
TMU_DEF tm_bool tmu_console_output_n(tmu_console_handle handle, const char* str, tm_size_t len) {
    TM_ASSERT(str || len == 0);
    if (handle <= tmu_console_in || handle > tmu_console_err) return TM_FALSE;
    if (!len) return TM_TRUE;

    DWORD written = 0;
    if (tmu_console_state[handle].is_redirected_to_file) {
        BOOL result = WriteFile(tmu_console_state[handle].handle, str, (DWORD)len, &written, /*overlapped=*/TM_NULL);
        if (!result) return TM_FALSE;
        return written == (DWORD)len;
    }

    tmu_char16 sbo[TMU_SBO_SIZE];

    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, len);
    tmu_conversion_result conv_result
        = tmu_utf16_from_utf8_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL,
                                 /*replace_str_len=*/0,
                                 /*nullterminate=*/TM_FALSE, /*out=*/sbo, /*out_len=*/TMU_SBO_SIZE);

    tmu_char16* wide = sbo;
    if (conv_result.ec == TM_ERANGE) {
        wide = (tmu_char16*)TMU_MALLOC(conv_result.size * sizeof(tmu_char16), sizeof(tmu_char16));
        if (wide) {
            tmu_conversion_result new_result
                = tmu_utf16_from_utf8_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL,
                                         /*replace_str_len=*/0,
                                         /*nullterminate=*/TM_FALSE, wide, conv_result.size);
            conv_result.ec = new_result.ec;
        } else {
            conv_result.ec = TM_ENOMEM;
        }
    }

    if (conv_result.ec == TM_OK) {
        BOOL result = WriteConsoleW(tmu_console_state[handle].handle, wide, (DWORD)conv_result.size, &written, TM_NULL);
        if (!result) written = 0;
    }

    if (wide && wide != sbo) {
        TMU_FREE(wide);
    }
    return written == (DWORD)conv_result.size;
}

#endif /* defined(TMU_USE_CONSOLE) */