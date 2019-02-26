/* Windows Mockups */
typedef const void* test_handle;

#define lstrlenA strlen
#define lstrlenW test_strlen
#define DWORD uint32_t
#define HANDLE test_handle
#define BOOL bool

/* Undef all windows definitions */
#include "windows_undefs.h"

#define CopyMemory memcpy
#define MoveMemory memmove
#define MAX_PATH 260


#define INVALID_HANDLE_VALUE ((void*)3007)
const void* test_valid_handle = ((void*)1);

#define CP_UTF8 4000
#define MB_ERR_INVALID_CHARS 4001

/* Windows Errors */
#define ERROR_ACCESS_DENIED 1000
#define ERROR_CANNOT_MAKE 1001
#define ERROR_CURRENT_DIRECTORY 1002
#define ERROR_INVALID_ACCESS 1003
#define ERROR_NOACCESS 1004
#define ERROR_SHARING_VIOLATION 1005
#define ERROR_WRITE_PROTECT 1006
#define ERROR_ALREADY_EXISTS 1007
#define ERROR_FILE_EXISTS 1008
#define ERROR_CANTOPEN 1009
#define ERROR_CANTREAD 1010
#define ERROR_CANTWRITE 1011
#define ERROR_OPEN_FAILED 1012
#define ERROR_READ_FAULT 1013
#define ERROR_SEEK 1014
#define ERROR_WRITE_FAULT 1015
#define ERROR_DIRECTORY 1016
#define ERROR_INVALID_HANDLE 1017
#define ERROR_INVALID_NAME 1018
#define ERROR_NEGATIVE_SEEK 1019
#define ERROR_NO_UNICODE_TRANSLATION 1020
#define ERROR_INVALID_PARAMETER 1021
#define ERROR_INVALID_FLAGS 1022
#define ERROR_INSUFFICIENT_BUFFER 1023
#define ERROR_FILE_NOT_FOUND 1024
#define ERROR_PATH_NOT_FOUND 1025
#define ERROR_NOT_ENOUGH_MEMORY 1026
#define ERROR_OUTOFMEMORY 1027
#define ERROR_BAD_UNIT 1028
#define ERROR_DEV_NOT_EXIST 1029
#define ERROR_INVALID_DRIVE 1030
#define ERROR_BUSY 1031
#define ERROR_BUSY_DRIVE 1032
#define ERROR_DEVICE_IN_USE 1033
#define ERROR_OPEN_FILES 1034
#define ERROR_DISK_FULL 1035
#define ERROR_HANDLE_DISK_FULL 1036
#define ERROR_BUFFER_OVERFLOW 1037
#define ERROR_DIR_NOT_EMPTY 1038
#define ERROR_NOT_SAME_DEVICE 1039
#define ERROR_TOO_MANY_OPEN_FILES 1040

/* Not real error, just for testing. */
#define ERROR_MOCK 8000
#define ERROR_MOCK_INVALID_COMMAND_LINE 8001

#define ERROR_NOT_READY 1041
#define ERROR_RETRY 1042
#define ERROR_INVALID_FUNCTION 1043
#define ERROR_LOCK_VIOLATION 1044
#define ERROR_LOCKED 1045
#define ERROR_OPERATION_ABORTED 1046

/* File Attributes */
#define FILE_ATTRIBUTE_DIRECTORY 2000
#define INVALID_FILE_ATTRIBUTES 2001

enum should_fail_enum {
    fail_none,
    fail_MultiByteToWideChar,
    fail_WideCharToMultiByte,
    fail_GetFileAttributesW,
    fail_GetFileAttributesExW,
    fail_CreateFileW,
    fail_GetFileSizeEx,
    fail_ReadFile,
    fail_CloseHandle,
    fail_CreateDirectoryW,
    fail_WriteFile,
    fail_GetTempFileNameW,
    fail_MoveFileExW,
    fail_DeleteFileW,
    fail_RemoveDirectoryW,
    fail_GetCurrentDirectoryW,
    fail_GetCommandLineW,
    fail_CommandLineToArgvW,
    fail_LocalFree,
};

struct win_mock_struct {
    DWORD internal_error;
    DWORD next_error;
    should_fail_enum should_fail;

    bool test_fail(should_fail_enum current) {
        internal_error = 0;
        if (should_fail == current) {
            should_fail = fail_none;
            next_error = 0;
            internal_error = next_error;
            return true;
        }
        return false;
    }
    void set_fail(should_fail_enum which, uint32_t error) {
        should_fail = which;
        next_error = error;
    }
};

win_mock_struct win_mock;

/* Windows Mock Functions */
#define GetLastError test_GetLastError
#define MultiByteToWideChar test_MultiByteToWideChar
#define WideCharToMultiByte test_WideCharToMultiByte
#define GetFileAttributesW test_GetFileAttributesW
#define GetFileAttributesExW test_GetFileAttributesExW
#define CreateFileW test_CreateFileW
#define GetFileSizeEx test_GetFileSizeEx
#define ReadFile test_ReadFile
#define CloseHandle test_CloseHandle
#define CreateDirectoryW test_CreateDirectoryW
#define WriteFile test_WriteFile
#define GetTempFileNameW test_GetTempFileNameW
#define MoveFileExW test_MoveFileExW
#define DeleteFileW test_DeleteFileW
#define RemoveDirectoryW test_RemoveDirectoryW
#define GetCurrentDirectoryW test_GetCurrentDirectoryW
#define GetCommandLineW test_GetCommandLineW
#define CommandLineToArgvW test_CommandLineToArgvW
#define LocalFree test_LocalFree

DWORD test_GetLastError() { return win_mock.internal_error; }

int test_MultiByteToWideChar(int codepage, DWORD flags, const char* multibyte, int multibyte_len, tchar* widechar,
                             int widechar_len);
int test_WideCharToMultiByte(int codepage, DWORD flags, const tchar* widechar, int widechar_len, char* multibyte,
                             int multibyte_len, const char* default_char, bool* default_char_used);

DWORD test_GetFileAttributesW(const tchar* filename) {
    if (win_mock.test_fail(fail_GetFileAttributesW)) {
        return INVALID_FILE_ATTRIBUTES;
    }

    auto entry_index = mock.find(filename);
    if (entry_index < 0) {
        win_mock.internal_error = ERROR_FILE_NOT_FOUND;
        return INVALID_FILE_ATTRIBUTES;
    }

    const auto& entry = mock.entries[entry_index];
    return (entry.type == mock_dir) ? FILE_ATTRIBUTE_DIRECTORY : 0;
}

#define GetFileExInfoStandard 2002
#define WIN32_FILE_ATTRIBUTE_DATA test_file_attribute_data

struct test_file_attribute_data {
    uint64_t ftLastWriteTime;
};
int test_GetFileAttributesExW(const tchar* filename, int info_level_id, test_file_attribute_data* file_info_out) {
    TM_ASSERT(info_level_id == GetFileExInfoStandard);
    TM_ASSERT(file_info_out);

    if (win_mock.test_fail(fail_GetFileAttributesExW)) return 0;

    auto entry_index = mock.find(filename);
    if (entry_index < 0) {
        win_mock.internal_error = ERROR_FILE_NOT_FOUND;
        return 0;
    }

    file_info_out->ftLastWriteTime = 0;
    return 1;
}

#define GENERIC_READ 3000
#define GENERIC_WRITE 3001
#define OPEN_EXISTING 3002
#define FILE_ATTRIBUTE_NORMAL 3003
#define CREATE_ALWAYS 3004
#define CREATE_NEW 3005
#define FILE_SHARE_READ 3006

HANDLE test_CreateFileW(const tchar* filename, DWORD desired_access, DWORD share_mode, const void* security_attributes,
                        DWORD creation_disposition, DWORD flags_and_attributes, HANDLE template_file) {
    TM_ASSERT(desired_access == GENERIC_READ || desired_access == GENERIC_WRITE);
    TM_ASSERT(share_mode == 0 || share_mode == FILE_SHARE_READ);
    TM_ASSERT(security_attributes == nullptr);
    TM_ASSERT(creation_disposition == OPEN_EXISTING || creation_disposition == CREATE_ALWAYS ||
              creation_disposition == CREATE_NEW);
    TM_ASSERT(flags_and_attributes == FILE_ATTRIBUTE_NORMAL);
    TM_ASSERT(!mock.file.is_open);
    TM_ASSERT(!template_file);

    if (win_mock.test_fail(fail_CreateFileW)) return INVALID_HANDLE_VALUE;

    auto index = mock.find_file(filename);

    if (desired_access == GENERIC_WRITE) {
        if (index >= 0 && creation_disposition == CREATE_NEW) {
            win_mock.internal_error = ERROR_ALREADY_EXISTS;
            return INVALID_HANDLE_VALUE;
        }
        if (index < 0) {
            mock.entries.push_back({});
            auto& added = mock.entries.back();
            added.path.assign(filename, filename + test_strlen(filename));
            added.type = mock_file;
            index = (int32_t)(mock.entries.size() - 1);
        }
    }

    if (index < 0) {
        win_mock.internal_error = ERROR_FILE_NOT_FOUND;
        return INVALID_HANDLE_VALUE;
    }

    const auto& entry = mock.entries[index];
    mock.file = {};
    mock.file.is_open = true;
    mock.file.is_writable = desired_access == GENERIC_WRITE;
    mock.file.read_contents = nullptr;
    mock.file.file_io_entry_index = index;
    mock.file.read_contents_size = 0;
    if (!entry.contents.empty()) {
        mock.file.read_contents_size = entry.contents.size();
        mock.file.read_contents = entry.contents.data();
    }
    return test_valid_handle;
}

#define LARGE_INTEGER test_large_int
struct test_large_int {
    uint64_t QuadPart;
};
bool test_GetFileSizeEx(HANDLE handle, LARGE_INTEGER* size) {
    TM_ASSERT(handle == test_valid_handle);
    TM_ASSERT(mock.file.is_open);
    TM_ASSERT(size);

    if (win_mock.test_fail(fail_GetFileSizeEx)) return false;

    size->QuadPart = (uint64_t)mock.file.read_contents_size;
    return true;
}

bool test_ReadFile(HANDLE handle, void* dest, DWORD bytes_to_read, DWORD* bytes_read, void* overlapped) {
    TM_ASSERT(handle == test_valid_handle);
    TM_ASSERT(dest || bytes_to_read == 0);
    TM_ASSERT(bytes_read);
    TM_ASSERT(!overlapped);
    TM_ASSERT(mock.file.is_open);

    if (win_mock.test_fail(fail_ReadFile)) return false;

    auto remaining = mock.file.read_contents_size - mock.file.bytes_read;
    if (bytes_to_read > remaining * sizeof(char)) bytes_to_read = (DWORD)(remaining * sizeof(char));
    if (bytes_to_read) {
        memcpy(dest, mock.file.read_contents + mock.file.bytes_read, bytes_to_read);
        mock.file.bytes_read += bytes_to_read;
    }
    *bytes_read = bytes_to_read;
    return true;
}

bool test_CloseHandle(HANDLE handle) {
    TM_ASSERT(handle == test_valid_handle);
    TM_ASSERT(mock.file.is_open);

    if (win_mock.test_fail(fail_CloseHandle)) return false;

    if (mock.file.is_writable) {
        mock.entries[mock.file.file_io_entry_index].contents.assign(mock.file.write_contents,
                                                                    mock.file.write_contents + mock.file.bytes_written);
    }

    mock.file = {};
    return true;
}

bool test_CreateDirectoryW(const tchar* path, const void* security_attributes) {
    TM_ASSERT(path);
    TM_ASSERT(!security_attributes);

    if (win_mock.test_fail(fail_CreateDirectoryW)) return false;

    if (mock.find_dir(path) >= 0) {
        win_mock.internal_error = ERROR_ALREADY_EXISTS;
        return false;
    }

    auto parent_size = get_parent_path_size(path);
    if (parent_size > 0 && mock.find_dir(path, parent_size) < 0) {
        win_mock.internal_error = ERROR_PATH_NOT_FOUND;
        return false;
    }

    mock.entries.push_back({});
    auto& entry = mock.entries.back();
    entry.path.assign(path, path + test_strlen(path));
    entry.type = mock_dir;
    return true;
}

bool test_WriteFile(HANDLE handle, const void* buffer, DWORD buffer_len, DWORD* bytes_written, const void* overlapped) {
    TM_ASSERT(handle == test_valid_handle);
    TM_ASSERT(buffer || buffer_len == 0);
    TM_ASSERT(bytes_written);
    TM_ASSERT(!overlapped);
    TM_ASSERT(mock.file.is_open);
    TM_ASSERT(mock.file.is_writable);

    if (win_mock.test_fail(fail_WriteFile)) return false;

    DWORD bytes_to_write = buffer_len;
    size_t remaining = std::size(mock.file.write_contents) - mock.file.bytes_written;
    if (bytes_to_write > remaining * sizeof(char)) bytes_to_write = (DWORD)(remaining * sizeof(char));
    if (bytes_to_write) {
        memcpy(mock.file.write_contents + mock.file.bytes_written, buffer, bytes_to_write);
        mock.file.bytes_written += bytes_to_write;
    }
    *bytes_written = bytes_to_write;
    return true;
}

uint32_t test_GetTempFileNameW(const tchar* path, const tchar* prefix, uint32_t unique, tchar* out) {
    TM_ASSERT(path);
    TM_ASSERT(prefix);
    TM_ASSERT(unique == 0);
    TM_ASSERT(out);

    if (win_mock.test_fail(fail_GetTempFileNameW)) return 0;

    auto path_len = test_strlen(path);
    auto prefix_len = test_strlen(prefix);

    if (path_len && (path[path_len - 1] == '\\' || path[path_len - 1] == '/')) --path_len;

    const size_t max_len = MAX_PATH - 14;
    auto result_len = path_len + prefix_len + 1 + 3;
    if (result_len > max_len) {
        win_mock.internal_error = ERROR_BUFFER_OVERFLOW;
        return 0;
    }

    static uint32_t id_val = 1;
    if (unique == 0) {
        unique = id_val++;
    }

    if (path_len) {
        memcpy(out, path, path_len * sizeof(tchar));
        out += path_len;
    }
    *out++ = '/';
    if (prefix_len) {
        memcpy(out, prefix, prefix_len * sizeof(tchar));
        out += prefix_len;
    }

    // Dumb int to string.
    for (auto i = 0; i < 3; ++i) {
        if (unique <= 9) {
            *out++ = (tchar)(unique + '0');
            break;
        } else if (unique >= 10 && unique <= 36) {
            *out++ = (tchar)(unique + 'A');
            break;
        } else {
            *out++ = 'Z';
            unique -= 37;
        }
    }
    // Nullterminate
    *out++ = 0;

    return unique;
}

#define MOVEFILE_COPY_ALLOWED (1 << 28)
#define MOVEFILE_REPLACE_EXISTING (1 << 29)
bool test_MoveFileExW(const tchar* from, const tchar* to, DWORD flags) {
    TM_ASSERT(from);
    TM_ASSERT(to);
    TM_ASSERT(flags == 0 || flags == MOVEFILE_COPY_ALLOWED ||
              flags == (MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING));

    if (win_mock.test_fail(fail_MoveFileExW)) return 0;

    auto from_index = mock.find(from);
    if (from_index < 0) {
        win_mock.internal_error = ERROR_FILE_NOT_FOUND;
        return false;
    }

    auto to_index = mock.find(to);
    if (to_index >= 0) {
        if (!(flags & MOVEFILE_REPLACE_EXISTING)) {
            win_mock.internal_error = ERROR_ALREADY_EXISTS;
            return false;
        }
    }
    mock.entries[from_index].path.assign(to, to + test_strlen(to));
    if (to_index >= 0) {
        mock.entries.erase(mock.entries.begin() + to_index);
    }

    return true;
}

bool test_DeleteFileW(const tchar* path) {
    TM_ASSERT(path);

    if (win_mock.test_fail(fail_DeleteFileW)) return false;

    auto index = mock.find_file(path);
    if (index < 0) {
        win_mock.internal_error = ERROR_FILE_NOT_FOUND;
        return false;
    }

    mock.entries.erase(mock.entries.begin() + index);
    return true;
}

bool test_RemoveDirectoryW(const tchar* path) {
    TM_ASSERT(path);

    if (win_mock.test_fail(fail_RemoveDirectoryW)) return false;

    auto index = mock.find_dir(path);
    if (index < 0) {
        win_mock.internal_error = ERROR_PATH_NOT_FOUND;
        return false;
    }

    mock.entries.erase(mock.entries.begin() + index);
    return true;
}

DWORD test_GetCurrentDirectoryW(DWORD buffer_len, tchar* buffer) {
    TM_ASSERT(buffer || buffer_len == 0);

    if (win_mock.test_fail(fail_GetCurrentDirectoryW)) return 0;

    const size_t cwd_size = 1050;
    if (buffer_len == 0) return cwd_size;

    if (buffer_len < cwd_size) {
        win_mock.internal_error = ERROR_INSUFFICIENT_BUFFER;
        return cwd_size;
    }

    for (size_t i = 0; i < cwd_size - 1; ++i) {
        buffer[i] = (tchar)'X';
    }
    buffer[cwd_size - 1] = 0;
    return cwd_size - 1;
}

const WCHAR* test_GetCommandLineW() {
    bool make_invalid = false;
    if (win_mock.should_fail == fail_GetCommandLineW) {
        if (win_mock.next_error == ERROR_MOCK_INVALID_COMMAND_LINE) {
            make_invalid = true;
        } else {
            if (win_mock.test_fail(fail_GetCommandLineW)) return nullptr;
        }
    }

    static WCHAR buffer[100] = {};
    for (auto& entry : buffer) {
        entry = 'X';
    }
    if (make_invalid) buffer[98] = 0xDC00u; /* Unpaired surrogate. */
    buffer[99] = 0;
    return buffer;
}

WCHAR** test_CommandLineToArgvW(const WCHAR* command_line, int* out_args_count) {
    TM_ASSERT(command_line);
    TM_ASSERT(*command_line);
    TM_ASSERT(out_args_count);

    if (win_mock.test_fail(fail_CommandLineToArgvW)) return nullptr;

    static WCHAR empty = 0;

    /* Allocate memory so we can track unfreed memory. */
    auto args = (WCHAR**)TM_MALLOC(sizeof(WCHAR*) * 2, sizeof(WCHAR*));
    TM_ASSERT(args);
    args[0] = (WCHAR*)command_line;
    args[1] = &empty;
    *out_args_count = 1;
    return args;
}

void test_LocalFree(WCHAR** ptr) {
    TM_ASSERT(ptr);
    TM_FREE(ptr, sizeof(WCHAR*) * 2, sizeof(WCHAR*));
}