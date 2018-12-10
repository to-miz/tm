/* Use malloc if provided, otherwise fall back to process heap. */
#ifdef TM_MALLOC
	#define TMF_MALLOC TM_MALLOC
	#define TMF_FREE TM_FREE
#else
	#define TMF_MALLOC(type, size, alignment) (type*)HeapAlloc(GetProcessHeap(), 0, (size) * sizeof(type))
	#define TMF_FREE(ptr, size, alignment) HeapFree(GetProcessHeap(), 0, (ptr))
#endif

#ifdef TM_MEMMOVE
	#define TMF_MEMMOVE TM_MEMMOVE
#else
	#define TMF_MEMMOVE(dest, src, size) MoveMemory((dest), (src), (size))
#endif

typedef WCHAR tmf_tchar;

/* String functions, use shlwapi if provided, otherwise use fallback version. */
#ifdef TMF_USE_SHLWAPI_H
#define TMF_STRCHRW StrChrW
#elif defined(TM_WCSCHR)
#define TMF_STRCHRW TM_WCSCHR
#else
static WCHAR* tmf_strchrw(WCHAR* str, WCHAR c) {
    TM_ASSERT(str);
    while (*str && *str != c) ++str;
    if (!*str) return TM_NULL;
    return str;
}
#define TMF_STRCHRW tmf_strchrw
#endif

typedef struct {
    tmf_tchar* path;
    tm_size_t size;
} tmf_platform_path;

tm_errc tmf_winerror_to_errc(DWORD error, tm_errc def) {
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
            return TM_EINVAL;

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

        default:
            return def;

#if 0
        case ERROR_NOT_READY:
            return std::errc::resource_unavailable_try_again;
        case ERROR_RETRY:
            return std::errc::resource_unavailable_try_again;
        case ERROR_DIR_NOT_EMPTY:
            return std::errc::directory_not_empty;
        case ERROR_INVALID_FUNCTION:
            return std::errc::function_not_supported;
        case ERROR_LOCK_VIOLATION:
            return std::errc::no_lock_available;
        case ERROR_LOCKED:
            return std::errc::no_lock_available;
        case ERROR_NOT_SAME_DEVICE:
            return std::errc::cross_device_link;
        case ERROR_OPERATION_ABORTED:
            return std::errc::operation_canceled;
        case ERROR_TOO_MANY_OPEN_FILES:
            return std::errc::too_many_files_open;
#endif
    }
}

tmf_platform_path tmf_to_platform_path_n(const char* path, tm_size_t len) {
    tmf_platform_path result = {TM_NULL, 0};
    if (!len) return result;

    result.path = TMF_MALLOC(WCHAR, len, sizeof(WCHAR));
    if (!result.path) return result;
    result.size = len;

    /* Convert to UTF16. */
    int converted_size = MultiByteToWideChar(CP_UTF8, 0, path, (int)len, result.path, (int)len);
    /* Always nullterminate */
    if (len) {
        if (!converted_size) {
            result.path[0] = 0;
        } else {
            if (converted_size < len) {
                result.path[converted_size] = 0;
            } else {
                result.path[len - 1] = 0;
            }
        }
    }

    /* Turn path to win32 path. */
    for (WCHAR* cur = result.path;; ++cur) {
        switch (*cur) {
            case 0: {
                return result;
            }
            case L'/': {
                *cur = L'\\';
                break;
            }
        }
    }
}

tmf_platform_path tmf_to_platform_path(const char* path) {
    return tmf_to_platform_path_n(path, (tm_size_t)lstrlenA(path));
}

void tmf_to_tmf_path(char* path) {
    TM_ASSERT(path);

    for (;; ++path) {
        switch (*path) {
            case 0:
                return;
            case '\\': {
                *path = '/';
                break;
            }
        }
    }
}

void tmf_destroy_platform_path(tmf_platform_path* platform_path) {
    if (platform_path) {
        if (platform_path->path) {
            TMF_FREE(platform_path->path, platform_path->size, sizeof(tmf_tchar));
        }
        *platform_path = {TM_NULL, 0};
    }
}

tmf_exists_result tmf_file_exists_t(const WCHAR* filename) {
    tmf_exists_result result = {false, TM_OK};
    DWORD attributes = GetFileAttributesW(filename);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        result.ec = tmf_winerror_to_errc(GetLastError(), TM_EIO);
    } else {
        result.exists = (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }
    return result;
}

tmf_exists_result tmf_directory_exists_t(const WCHAR* dir) {
    tmf_exists_result result = {false, TM_OK};
    DWORD attributes = GetFileAttributesW(dir);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        result.ec = tmf_winerror_to_errc(GetLastError(), TM_EIO);
    } else {
        result.exists = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    return result;
}

tmf_file_timestamp_result tmf_file_timestamp_t(const WCHAR* filename) {
    tmf_file_timestamp_result result = {0, TM_OK};

    WIN32_FILE_ATTRIBUTE_DATA fileAttr = {0};
    if (GetFileAttributesExW(filename, GetFileExInfoStandard, &fileAttr) == 0) {
        result.ec = TMF_NO_SUCH_FILE_OR_DIRECTORY;
        return result;
    }

    TM_STATIC_ASSERT(sizeof(result.file_time) == sizeof(fileAttr.ftLastWriteTime), invalid_file_time_size);
    CopyMemory(&result.file_time, &fileAttr.ftLastWriteTime, sizeof(fileAttr.ftLastWriteTime));
    return result;
}

tmf_contents_result tmf_read_file_t(const WCHAR* filename) {
    tmf_contents_result result = {{TM_NULL, 0}, TM_OK};

    HANDLE file =
        CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, TM_NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, TM_NULL);

    if (file == INVALID_HANDLE_VALUE) {
        result.ec = tmf_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(file, &size)) {
        result.ec = tmf_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }
    if (size.QuadPart > INT32_MAX) {
        /* TODO: ReadFile can only read 2^32 bytes at a time, implement reading in chunks. */
        result.ec = TM_EFBIG;
        return result;
    }

    char* data = TMF_MALLOC(char, (tm_size_t)size.QuadPart, sizeof(char));
    if (!data) {
        result.ec = TM_ENOMEM;
        return result;
    }

    DWORD bytes_to_read = (DWORD)size.QuadPart;
    DWORD bytes_read = 0;
    BOOL read_file_result = ReadFile(file, data, bytes_to_read, &bytes_read, TM_NULL);
    if (!read_file_result || bytes_read != bytes_to_read) {
        result.ec = tmf_winerror_to_errc(GetLastError(), TM_EIO);
        TMF_FREE(data, (tm_size_t)size.QuadPart, sizeof(char));
        return result;
    }
    CloseHandle(file);

    result.contents.data = data;
    result.contents.size = (tm_size_t)size.QuadPart;
    return result;
}

static tm_errc tmf_create_directory_internal(const WCHAR* dir, tm_size_t dir_len) {
    if (dir_len <= 0) return TM_OK;
    if (dir_len == 1 && dir[0] == L'\\') return TM_OK;
    if (dir_len == 2 && (dir[0] == L'.' || dir[0] == L'~') && dir[1] == L'\\') return TM_OK;

    tm_size_t dir_str_len = dir_len + 1;
    WCHAR* dir_str = TMF_MALLOC(WCHAR, dir_str_len, sizeof(WCHAR));
    if (!dir_str) return TM_ENOMEM;
    CopyMemory(dir_str, dir, dir_len);
    dir_str[dir_str_len] = 0;

    if (tmf_directory_exists_t(dir_str).exists) {
        TMF_FREE(dir_str, dir_str_len, sizeof(WCHAR));
        return TM_OK;
    }

    /* Create directory tree recursively. */
    tm_errc result = TM_OK;
    WCHAR* end = TMF_STRCHRW(dir_str, L'\\');
    for (;;) {
        tm_bool was_null = (end == TM_NULL);
        if (!was_null) *end = 0;

        if (!CreateDirectoryW(dir_str, TM_NULL)) {
            DWORD error = GetLastError();
            if (error != ERROR_ALREADY_EXISTS) {
                result = tmf_winerror_to_errc(error, TM_EIO);
                break;
            }
        }

        if (was_null || *(end + 1) == 0) break;
        *end = L'\\';
        end = TMF_STRCHRW(end + 1, L'\\');
    }
    TMF_FREE(dir_str, dir_str_len, sizeof(WCHAR));
    return result;
}

static int tmf_get_path_len_internal(const WCHAR* filename, int filename_len) {
    int dir_len = (filename_len == 0) ? lstrlenW(filename) : filename_len;
    while (dir_len > 0 && filename[dir_len] != '/' && filename[dir_len] != '\\' && filename[dir_len] != ':' &&
           filename[dir_len] != '~') {
        --dir_len;
    }
    return dir_len;
}

tmf_write_file_result tmf_write_file_ex_internal(const WCHAR* filename, const void* data, tm_size_t size,
                                                 uint32_t flags) {
    TM_ASSERT_VALID_SIZE(size);
    tmf_write_file_result result = {0, TM_OK};

    if (flags & tmf_create_directory_tree) {
        tm_errc ec = tmf_create_directory_internal(filename, tmf_get_path_len_internal(filename, /*filename_len=*/0));
        if (ec != TM_OK) {
            result.ec = ec;
            return result;
        }
    }

    DWORD creation_flags = 0;
    if (flags & tmf_overwrite) {
        creation_flags = CREATE_ALWAYS;
    } else {
        creation_flags = CREATE_NEW;
    }
    HANDLE file = CreateFileW(filename, GENERIC_WRITE, 0, TM_NULL, creation_flags, FILE_ATTRIBUTE_NORMAL, TM_NULL);
    if (file == INVALID_HANDLE_VALUE) {
        result.ec = tmf_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }
    if ((size_t)size > (size_t)UINT32_MAX) {
        /* TODO: Implement writing in chunks so there is no UINT32_MAX limit. */
        result.ec = TM_EOVERFLOW;
        return result;
    }

    DWORD total_bytes_written = 0;
    DWORD bytes_written = 0;

    if (flags & tmf_write_byte_order_marker) {
        if (!WriteFile(file, tmf_utf8_bom, sizeof(tmf_utf8_bom), &bytes_written, TM_NULL)) {
            result.ec = TM_EIO;
            return result;
        }
        total_bytes_written += bytes_written;
    }

    DWORD truncated_buffer_size = (DWORD)size;
    if (!WriteFile(file, data, truncated_buffer_size, &bytes_written, TM_NULL)) {
        result.ec = TM_EIO;
        return result;
    }
    total_bytes_written += bytes_written;

    CloseHandle(file);

    result.written = (tm_size_t)total_bytes_written;
    return result;
}

tmf_write_file_result tmf_write_file_ex_t(const WCHAR* filename, const void* data, tm_size_t size, uint32_t flags) {
    if (!(flags & tmf_atomic_write)) {
        return tmf_write_file_ex_internal(filename, data, size, flags);
    }

    tmf_write_file_result result = {0, TM_OK};

    int filename_len = lstrlenW(filename);
    int dir_len = tmf_get_path_len_internal(filename, filename_len);
    if (flags & tmf_create_directory_tree) {
        tm_errc ec = tmf_create_directory_internal(filename, dir_len);
        if (ec != TM_OK) {
            result.ec = ec;
            return result;
        }
    }

    WCHAR* temp_file = TM_NULL;
    WCHAR temp_file_buffer[MAX_PATH];
    const int temp_file_len = filename_len + 9;

    /* Limitation of GetTempFileNameW, dir_len cannot be bigger than MAX_PATH - 14. */
    if (dir_len < MAX_PATH - 14) {
        WCHAR temp_dir[MAX_PATH];
        CopyMemory(temp_dir, filename, dir_len);
        temp_dir[dir_len] = 0;
        if (GetTempFileNameW(temp_dir, L"tmf", 0, temp_file_buffer) == 0) {
            result.ec = TM_EIO;
            return result;
        }

        temp_file = temp_file_buffer;
    } else {
        /* Fallback to using a simple temp_file. */
        temp_file = TMF_MALLOC(WCHAR, temp_file_len, sizeof(WCHAR));
        if (!temp_file) {
            result.ec = TM_ENOMEM;
            return result;
        }
        CopyMemory(temp_file, filename, filename_len * sizeof(WCHAR));
        /* Copy filename ending + null terminator. */
        CopyMemory(temp_file + filename_len, ".tmf_tmp", sizeof(WCHAR) * 9);
    }

    TM_ASSERT(temp_file);

    tmf_write_file_result temp_write_result = tmf_write_file_ex_internal(temp_file, data, size, 0);
    if (temp_write_result.ec != TM_OK) {
        result.ec = temp_write_result.ec;
        if (temp_file != temp_file_buffer) TMF_FREE(temp_file, temp_file_len, sizeof(WCHAR));
        return result;
    }

    DWORD move_flags = MOVEFILE_COPY_ALLOWED;
    if (flags & tmf_overwrite) move_flags |= MOVEFILE_REPLACE_EXISTING;
    if (!MoveFileExW(temp_file, filename, move_flags)) {
        result.ec = tmf_winerror_to_errc(GetLastError(), TM_EIO);
        if (temp_file != temp_file_buffer) TMF_FREE(temp_file, temp_file_len, sizeof(WCHAR));
        return result;
    }
    return result;
}

tm_errc tmf_rename_file_ex_t(const WCHAR* from, const WCHAR* to, uint32_t flags) {
    if (flags & tmf_create_directory_tree) {
        tm_errc ec = tmf_create_directory_internal(to, tmf_get_path_len_internal(to, /*filename_len=*/0));
        if (ec != TM_OK) return ec;
    }

    DWORD move_flags = MOVEFILE_COPY_ALLOWED;
    if (flags & tmf_overwrite) move_flags |= MOVEFILE_REPLACE_EXISTING;
    if (!MoveFileExW(from, to, move_flags)) {
        return (!(flags & tmf_overwrite)) ? TM_EEXIST : TM_EIO;
    }
    return TM_OK;
}

tm_errc tmf_delete_file_t(const WCHAR* filename) {
    if (!DeleteFileW(filename)) return tmf_winerror_to_errc(GetLastError(), TM_EIO);
    return TM_OK;
}

tm_errc tmf_create_directory_t(const WCHAR* dir) {
    int dirlen = lstrlenW(dir);
    return tmf_create_directory_internal(dir, dirlen);
}

tm_errc tmf_delete_directory_t(const WCHAR* dir) {
    if (!RemoveDirectoryW(dir)) return tmf_winerror_to_errc(GetLastError(), TM_EIO);
    return TM_OK;
}

static tmf_contents_result tmf_to_utf8(const WCHAR* str, tm_size_t extra_size, int* written) {
    tmf_contents_result result = {{TM_NULL, 0}, TM_OK};
    if (str && *str) {
        int size = WideCharToMultiByte(CP_UTF8, 0, str, -1, TM_NULL, 0, TM_NULL, TM_NULL);
        if (size) {
        	TM_ASSERT(extra_size < INT32_MAX - size);
            size += (int)extra_size;
            result.contents.data = TMF_MALLOC(char, size, sizeof(char));
            if (!result.contents.data) {
                result.ec = TM_ENOMEM;
            } else {
                int real_size = WideCharToMultiByte(CP_UTF8, 0, str, -1, result.contents.data, size, TM_NULL, TM_NULL);
                result.contents.size = (tm_size_t)size;

                *written = real_size;

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

tmf_contents_result tmf_current_working_directory(tm_size_t extra_size) {
    TM_ASSERT_VALID_SIZE(extra_size);

	tmf_contents_result result = {{TM_NULL, 0}, TM_OK};

    DWORD len = GetCurrentDirectoryW(0, TM_NULL);
    if (!len) {
        result.ec = tmf_winerror_to_errc(GetLastError(), TM_EIO);
        return result;
    }
    WCHAR* dir = TMF_MALLOC(WCHAR, len, sizeof(WCHAR));
    if (!dir) {
        result.ec = TM_ENOMEM;
        return result;
    }

    DWORD written = GetCurrentDirectoryW(len, dir);
    TM_ASSERT(written == len);

    int result_len = 0;
    result = tmf_to_utf8(dir, extra_size + 1, &result_len);
    if (result.ec == TM_OK) {
        tmf_to_tmf_path(result.contents.data);
        /* Append '/' at the end of the current directory if it doesn't exist. */
        if (result_len && result.contents.data[result_len - 1] != '/') {
            result.contents.data[result_len] = '/';
            TM_ASSERT(result_len + 1 < result.contents.size);
            result.contents.data[result_len + 1] = 0;
        }
    }
    TMF_FREE(dir, len, sizeof(WCHAR));
    return result;
}