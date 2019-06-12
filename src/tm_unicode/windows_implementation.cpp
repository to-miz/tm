typedef struct {
    tmu_tchar* path;
    tmu_tchar sbo[TMU_SBO_SIZE];
    tm_size_t allocated_size;
} tmu_platform_path;

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
    if (size <= 0) return TM_TRUE;

    int required_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, (int)size, TM_NULL, 0);
    if (required_size <= 0) return TM_FALSE; /* Size was not zero, so conversion failed. */
    ++required_size;                         /* Extra space for null-terminator. */

    if (required_size <= TMU_SBO_SIZE) {
        out->path = out->sbo;
        out->allocated_size = 0;
    } else {
        out->path = (WCHAR*)TMU_MALLOC(required_size * sizeof(WCHAR), sizeof(WCHAR));
        if (!out->path) return TM_FALSE;
        out->allocated_size = required_size;
    }

    int converted_size =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, (int)size, out->path, (int)required_size);
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

static tmu_file_timestamp_result tmu_file_timestamp_t(const WCHAR* filename) {
    tmu_file_timestamp_result result = {0, TM_OK};

    WIN32_FILE_ATTRIBUTE_DATA fileAttr = {0};
    if (GetFileAttributesExW(filename, GetFileExInfoStandard, &fileAttr) == 0) {
        result.ec = TMU_NO_SUCH_FILE_OR_DIRECTORY;
        return result;
    }

    TM_STATIC_ASSERT(sizeof(result.file_time) == sizeof(fileAttr.ftLastWriteTime), invalid_file_time_size);
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
            TMU_FREE(data, data_size * sizeof(char), sizeof(char));
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
        if (temp_file != temp_file_buffer) TMU_FREE(temp_file, temp_file_len * sizeof(WCHAR), sizeof(WCHAR));
        return result;
    }

    DWORD move_flags = MOVEFILE_COPY_ALLOWED;
    if (flags & tmu_overwrite) move_flags |= MOVEFILE_REPLACE_EXISTING;
    if (!MoveFileExW(temp_file, filename, move_flags)) {
        result.ec = tmu_winerror_to_errc(GetLastError(), TM_EIO);
        if (temp_file != temp_file_buffer) TMU_FREE(temp_file, temp_file_len * sizeof(WCHAR), sizeof(WCHAR));
        return result;
    }
    if (temp_file != temp_file_buffer) TMU_FREE(temp_file, temp_file_len * sizeof(WCHAR), sizeof(WCHAR));
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
        int size = WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, TM_NULL, 0, TM_NULL, TM_NULL);
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

                int real_size = WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, result.contents.data, size,
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
    TMU_FREE(dir, len * sizeof(WCHAR), sizeof(WCHAR));
    return result;
}

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
        result = tmu_utf8_command_line_from_utf16(wide_args, wide_args_count);
    }

    if (wide_args) {
        LocalFree(wide_args);
        wide_args = TM_NULL;
    }
    return result;
}

#if defined(__cplusplus)
TMU_DEF tmu_utf8_command_line_managed_result tmu_utf8_winapi_get_command_line_managed() {
    tmu_utf8_command_line_managed_result result;
    tmu_utf8_command_line_result unmanaged = tmu_utf8_winapi_get_command_line();
    static_cast<tmu_utf8_command_line&>(result.command_line) = unmanaged.command_line;
    result.ec = unmanaged.ec;
    return result;
}
#endif /* defined(__cplusplus) */