#include<stdarg.h> /* Needed for tmu_printf and tmu_fprintf */

// Cleanup global macros.
// clang-format off
#ifdef _MSC_VER
    #undef _MSC_VER
#endif
#ifdef errno
	#undef errno
#endif
#ifdef FILE
	#undef FILE
#endif
#ifdef S_ISDIR
	#undef S_ISDIR
#endif
#ifdef S_ISREG
	#undef S_ISREG
#endif
#ifdef stat
	#undef stat
#endif
#ifdef wchar_t
	#undef wchar_t
#endif
#ifdef WCHAR
	#undef WCHAR
#endif

#ifdef __stat64
	#undef __stat64
#endif

#ifdef wcslen
    #undef wcslen
#endif
#ifdef wcschr
    #undef wcschr
#endif
#ifdef DIR
    #undef DIR
#endif
#ifdef opendir
    #undef opendir
#endif
#ifdef readdir
    #undef readdir
#endif
#ifdef closedir
    #undef closedir
#endif
#ifdef dirent
    #undef dirent
#endif
#ifdef DT_DIR
    #undef DT_DIR
#endif

#ifdef readlink
    #undef readlink
#endif
#ifdef ssize_t
    #undef ssize_t
#endif

#ifdef __argv
    #undef __argv
#endif
#ifdef __wargv
    #undef __wargv
#endif
#ifdef _wfindfirst64
    #undef _wfindfirst64
#endif
#ifdef _findclose
    #undef _findclose
#endif
#ifdef _wfindnext64
    #undef _wfindnext64
#endif
#ifdef _wfinddata64_t
    #undef _wfinddata64_t
#endif
#ifdef _A_SUBDIR
    #undef _A_SUBDIR
#endif

#ifdef _vscprintf
    #undef _vscprintf
#endif
#ifdef _vsnprintf
    #undef _vsnprintf
#endif
#ifdef vsnprintf
    #undef vsnprintf
#endif
#ifdef vfprintf
    #undef vfprintf
#endif
#ifdef vprintf
    #undef vprintf
#endif
#ifdef fwprintf
    #undef fwprintf
#endif
#ifdef _isatty
    #undef _isatty
#endif
#ifdef _fileno
    #undef _fileno
#endif
#ifdef stdin
    #undef stdin
#endif
#ifdef stdout
    #undef stdout
#endif
#ifdef stderr
    #undef stderr
#endif
#ifdef _O_U16TEXT
    #undef _O_U16TEXT
#endif
// clang-format on

#define TMU_TESTING_CHAR16_DEFINED
typedef uint16_t tmu_char16;

#if !defined(USE_WINDOWS_H) && !defined(USE_MSVC_CRT)

typedef char tchar;
typedef std::vector<tchar> tstring;
std::vector<tchar> convert_path(const char* str) { return std::vector<tchar>(str, str + strlen(str)); }
const char path_delim = '/';

#else /* defined(USE_WINDOWS_H) || defined(USE_MSVC_CRT) */

#ifndef _WIN32
    #define _WIN32 1
#endif
#ifndef _MSC_VER
    #ifdef TMU_TESTING_OLD_MSC
        #define _MSC_VER 1
    #else
        #define _MSC_VER 2000
    #endif
#endif

typedef uint16_t tchar;

// Clang doesn't like keyword macros, so we disable the warning here.
// We knowingly redefine wchar_t to a different type for mock purposes.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif

#define wchar_t uint16_t

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#define WCHAR uint16_t
typedef std::vector<uint16_t> tstring;
const char path_delim = '\\';
std::vector<uint16_t> convert_path(const char* str) {
    size_t len = strlen(str);
    std::vector<uint16_t> result;
    result.resize(len);
    for (size_t i = 0; i < len; i++) {
        /* Assuming ascii. */
        TM_ASSERT(str[i] >= 0);
        if (str[i] == '/') {
            result[i] = (uint16_t)'\\';
        } else {
            result[i] = (uint16_t)str[i];
        }
    }
    return result;
}
#endif /* !defined(USE_WINDOWS_H) && !defined(USE_MSVC_CRT) */

#define TMU_TESTING_TCHAR_DEFINED
typedef tchar tmu_tchar;

size_t test_strlen(const char* str) { return strlen(str); }
size_t test_strlen(const uint16_t* str) {
    const uint16_t* ptr = str;
    while (*ptr) ++ptr;
    return (size_t)(ptr - str);
}

const tchar* test_to_tchar(const char* str) {
    TM_ASSERT(str);

    static tchar buffer[10];
    for (tchar *p = buffer, *end = buffer + 10; p < end && *str; ++p) {
        *p = (tchar)*str++;
    }
    TM_ASSERT(!*str);
    return buffer;
}
tchar test_to_tchar(char c) { return (tchar)c; }
tchar* test_strchr(tchar* str, tchar c) {
	TM_ASSERT(str);
	while (*str && *str != c) ++str;
	if (*str == c) return str;
	return nullptr;
}

bool equal_file(const tstring& a, const tchar* b, size_t b_len) {
    TM_ASSERT(b);
    if (b_len != a.size()) return false;
    return memcmp(a.data(), b, b_len * sizeof(tchar)) == 0;
}
bool equal_dir(const tstring& a, const tchar* b, size_t b_len) {
    TM_ASSERT(b);
    size_t a_len = a.size();
    if (a_len && a[a_len - 1] == path_delim) --a_len;
    if (b_len && b[b_len - 1] == path_delim) --b_len;

    if (b_len != a_len) return false;
    return memcmp(a.data(), b, b_len * sizeof(tchar)) == 0;
}

struct open_file {
    bool is_open;
    bool is_writable;

    const char* read_contents;
    size_t read_contents_size;
    size_t bytes_read;

    size_t bytes_written;
    char write_contents[1024];
    size_t remaining() { return (size_t)(1024 - bytes_written); }

    int32_t file_io_entry_index;
    bool has_error;
};

enum mock_file_type {
    mock_file,
    mock_dir,
};

struct file_io_entry {
    tstring path;
    std::vector<char> contents;
    mock_file_type type;

    file_io_entry() = default;
    file_io_entry(const char* path, const char* contents, mock_file_type type) : path(convert_path(path)), type(type) {
        if (contents) this->contents.assign(contents, contents + strlen(contents));
    }
    file_io_entry(const char* path, const char* contents, size_t contents_size, mock_file_type type)
        : path(convert_path(path)), type(type) {
        if (contents && contents_size > 0) this->contents.assign(contents, contents + contents_size);
    }
};

enum should_fail_enum {
    fail_none,
    fail_stat,
    fail_mkdir,
    fail_rmdir,
    fail_remove,
    fail_rename,
    fail_getcwd,
    fail_fopen,
    fail_freopen,
    fail_fwrite,
    fail_fread,
    fail_opendir,
    fail_readdir,
    fail_readlink,
    fail_malloc,
    fail_realloc,
    fail_vsnprintf,
    fail__vscprintf,
    fail__vsnprintf,
    fail_vfprintf,
    fail_vprintf,
    fail_fwprintf,
    fail__isatty,
    fail__fileno,
    fail__setmode,

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
    fail_GetModuleFileNameW,
    fail_FindFirstFileW,
    fail_FindClose,
    fail_FindNextFileW,
    fail_wfindfirst64,
    fail_wfindnext64,
    fail_SetConsoleOutputCP,
    fail_SetConsoleCP,
    fail_WriteConsoleW,
    fail_GetStdHandle,
    fail_GetConsoleMode,
};

struct console_data {
    std::vector<tmu_char16> wdata;
    open_file file;
    bool is_file = false;

    void clear() {
        wdata.clear();
        file = {};
        is_file = false;
        file.is_open = true;
        file.is_writable = true;
    }

    bool has_output(std::string_view str) {
        if (!wdata.empty()) {
            if (wdata.size() != str.size()) return false;
            for (size_t i = 0, count = wdata.size(); i < count; ++i) {
                if ((char)wdata[i] != str[i]) return false;
            }
            return true;
        }
        return str == std::string_view(file.write_contents, file.bytes_written);
    }
};

struct file_io_mock {
    int internal_error;
    std::unordered_map<should_fail_enum, int> error_map;

    bool simulate_win_xp;

    std::vector<file_io_entry> entries;
    open_file file;

    std::vector<tchar> module_filename;
    int readdir_index;

    std::vector<char> char_module_filename;
    std::vector<tmu_char16> wchar_module_filename;
    std::vector<const char*> argv;
    std::vector<const tmu_char16*> wargv;

    console_data in;
    console_data out;
    console_data err;

    void set_module_filename(std::string_view name) {
        module_filename.assign(name.begin(), name.end());
        char_module_filename.assign(name.begin(), name.end());
        char_module_filename.push_back(0);
        wchar_module_filename.assign(name.begin(), name.end());
        wchar_module_filename.push_back(0);
        argv.resize(2);
        argv[0] = char_module_filename.data();
        argv[1] = "";
        wargv.resize(2);
        wargv[0] = wchar_module_filename.data();
        static const tmu_char16 empty[1] = {0};
        wargv[1] = empty;
    }

    void clear() {
        internal_error = 0;
        error_map.clear();
        simulate_win_xp = false;

        file = {};
        file.file_io_entry_index = -1;
        char_module_filename.clear();
        wchar_module_filename.clear();
        argv.clear();
        wargv.clear();
        module_filename.clear();
        readdir_index = 0;

        entries.clear();
        in.clear();
        out.clear();
        err.clear();
    }

    bool test_fail(should_fail_enum current) {
        internal_error = 0;
        auto it = error_map.find(current);
        if (it != error_map.end() && it->second != 0) {
            internal_error = it->second;
            it->second = 0;
            return true;
        }
        return false;
    }
    void set_fail(should_fail_enum which, int error) {
        if (which == fail_malloc) {
            malloc_fail = true;
            return;
        }
        if (which == fail_realloc) {
            realloc_fail = true;
            return;
        }
        error_map[which] = error;
    }

    int32_t find_file(const tchar* path) const {
    	auto path_len = test_strlen(path);
        for (int32_t i = 0, count = (int32_t)entries.size(); i < count; ++i) {
            const auto& entry = entries[i];
            if (entry.type == mock_file && equal_file(entry.path, path, path_len)) {
                return i;
            }
        }
        return -1;
    }
    int32_t find_dir(const tchar* path, size_t path_len) const {
        for (int32_t i = 0, count = (int32_t)entries.size(); i < count; ++i) {
            const auto& entry = entries[i];
            if (entry.type == mock_dir && equal_dir(entry.path, path, path_len)) {
                return i;
            }
        }
        return -1;
    }
    int32_t find_dir(const tchar* path) const { return find_dir(path, test_strlen(path)); }

    int32_t find(const tchar* path) const {
        auto path_len = test_strlen(path);
        for (int32_t i = 0, count = (int32_t)entries.size(); i < count; ++i) {
            const auto& entry = entries[i];
            if (entry.type == mock_file) {
                if (equal_file(entry.path, path, path_len)) return i;
            } else {
                if (equal_dir(entry.path, path, path_len)) return i;
            }
        }
        return -1;
    }
};

file_io_mock mock;

open_file* test_fopen(const tchar* filename, const tchar* mode) {
    TM_ASSERT(filename);
    TM_ASSERT(mode);

    TM_ASSERT(!mock.file.is_open);
    TM_ASSERT(!mock.file.is_writable);

    if (mock.test_fail(fail_fopen)) return nullptr;

    bool is_writable = false;

    if (*mode == 'r') {
        ++mode;
    } else if (*mode == 'w') {
        ++mode;
        is_writable = true;
    }

    if (*mode == 'b') ++mode;

    auto index = mock.find_file(filename);

    if (is_writable) {
    	if (index < 0) {
    		mock.entries.push_back({});
    		auto& added = mock.entries.back();
    		added.path.assign(filename, filename + test_strlen(filename));
    		added.type = mock_file;
    		index = (int32_t)(mock.entries.size() - 1);
    	}
    }

	if (index < 0) {
	    mock.internal_error = ENOENT;
	    return nullptr;
	}

    const auto& entry = mock.entries[index];
    mock.file = {};
    mock.file.is_open = true;
    mock.file.is_writable = is_writable;
    mock.file.read_contents = nullptr;
    mock.file.file_io_entry_index = index;
    mock.file.read_contents_size = 0;
    if (!entry.contents.empty()) {
	    mock.file.read_contents_size = entry.contents.size();
    	mock.file.read_contents = entry.contents.data();
    }
    return &mock.file;
}

int test_ferror(open_file* file) {
	return file->has_error;
}

void test_fclose(open_file* file) {
    TM_ASSERT(file);
    TM_ASSERT(file->is_open);
    TM_ASSERT(file->file_io_entry_index >= 0 && (size_t)file->file_io_entry_index < mock.entries.size());

    if (file->is_writable) {
        mock.entries[file->file_io_entry_index].contents.assign(file->write_contents,
                                                                file->write_contents + file->bytes_written);
    }

    *file = {};
}

open_file* test_freopen(const tchar* filename, const tchar* mode, open_file* current) {
    TM_ASSERT(filename);
    TM_ASSERT(mode);
    TM_ASSERT(current);

    if (mock.test_fail(fail_freopen)) return nullptr;

    test_fclose(current);
    return test_fopen(filename, mode);
}

size_t test_fread(void* buffer, size_t element_size, size_t num_elements, open_file* file) {
    TM_ASSERT(file);
    TM_ASSERT(file->is_open);
    TM_ASSERT(file->read_contents && file->read_contents_size > 0);
    TM_ASSERT(buffer);
    TM_ASSERT(element_size > 0);
    TM_ASSERT(file->bytes_read <= file->read_contents_size);

    if (mock.test_fail(fail_fread)) {
        file->has_error = true;
        return 0;
    }

    size_t read_size = element_size * num_elements;
    size_t unread = file->read_contents_size - file->bytes_read;

    num_elements = 0;
    if (read_size > unread) read_size = unread;
    if (read_size) {
        num_elements = read_size / element_size;
        memcpy(buffer, file->read_contents + file->bytes_read, num_elements * element_size);
        file->bytes_read += num_elements * element_size;
    }
    return num_elements;
}

size_t test_fwrite(const void* buffer, size_t element_size, size_t num_elements, open_file* file) {
    TM_ASSERT(file->is_open);
    TM_ASSERT(file->is_writable);
    TM_ASSERT(buffer);
    TM_ASSERT(element_size > 0);
    TM_ASSERT(file->bytes_written <= std::size(file->write_contents));

    if (mock.test_fail(fail_fwrite)) {
        file->has_error = true;
        return 0;
    }

    size_t write_size = element_size * num_elements;
    size_t unwritten = std::size(file->write_contents) - file->bytes_written;

    num_elements = 0;
    if (write_size > unwritten) write_size = unwritten;
    if (write_size) {
        num_elements = write_size / element_size;
        memcpy(file->write_contents + file->bytes_written, buffer, num_elements * element_size);
        file->bytes_written += num_elements * element_size;
    }
    return num_elements;
}

struct test_stat {
    mock_file_type st_mode;
    uint64_t st_mtime;
};

int test_stat(const tchar* filename, struct test_stat* out) {
    TM_ASSERT(out);
    TM_ASSERT(filename);

    if (mock.test_fail(fail_stat)) return -1;

    auto entry_index = mock.find(filename);
    if (entry_index < 0) {
        mock.internal_error = ENOENT;
        return -1;
    }

    out->st_mode = mock.entries[entry_index].type;
    out->st_mtime = 0;
    return 0;
}

size_t get_parent_path_size(const tchar* first) {
	TM_ASSERT(first);

	if (!*first || !*(first + 1)) return 0;

	auto last = first + test_strlen(first) - 1;
	if (*last == path_delim) --last;

	while (first != last) {
		if (*last == path_delim) return (size_t)(last - first);
		--last;
	}
	return 0;
}

int test_mkdir(const tchar* path) {
    TM_ASSERT(path);

    if (mock.test_fail(fail_mkdir)) return -1;

    if (!*path) {
        mock.internal_error = ENOENT;
        return -1;
    }

    if (mock.find_dir(path) >= 0) {
        mock.internal_error = EEXIST;
        return -1;
    }
    if (mock.find_file(path) >= 0) {
        mock.internal_error = ENOTDIR;
        return -1;
    }

    auto parent_size = get_parent_path_size(path);
    if (parent_size > 0 && mock.find_dir(path, parent_size) < 0) {
    	mock.internal_error = ENOENT;
    	return -1;
    }

    mock.entries.push_back({});
    auto& entry = mock.entries.back();
    entry.path.assign(path, path + test_strlen(path));
    entry.type = mock_dir;
    return 0;
}

int test_mkdir(const tchar* path, unsigned) {
    return test_mkdir(path);
}

int test_rmdir(const tchar* path) {
	TM_ASSERT(path);

    if (mock.test_fail(fail_rmdir)) return -1;

    if (!*path) {
        mock.internal_error = ENOENT;
        return -1;
    }

    auto index = mock.find_dir(path);
    if (index < 0) {
    	mock.internal_error = ENOENT;
    	return -1;
    }

    mock.entries.erase(mock.entries.begin() + index);
    return 0;
}

int test_remove(const tchar* path) {
    TM_ASSERT(path);

    if (mock.test_fail(fail_remove)) return -1;

    auto index = mock.find_file(path);
    if (index < 0) {
        mock.internal_error = ENOENT;
        return -1;
    }

    mock.entries.erase(mock.entries.begin() + index);
    return 0;
}

int test_rename(const tchar* from, const tchar* to) {
    TM_ASSERT(from);
    TM_ASSERT(to);

    if (mock.test_fail(fail_rename)) return -1;

    auto from_index = mock.find(from);
    if (from_index < 0) {
        mock.internal_error = ENOENT;
        return -1;
    }

    auto to_index = mock.find(to);
    if (to_index >= 0) {
        mock.internal_error = EACCES;
        return -1;
    }

    mock.entries[from_index].path.assign(to, to + test_strlen(to));
    return 0;
}

tchar* test_getcwd(tchar* buf, size_t size) {
#ifndef USE_MSVC_CRT
    TM_ASSERT(buf);
#endif
    TM_ASSERT(size > 0);

    if (mock.test_fail(fail_getcwd)) return nullptr;

    const size_t cwd_size = 1050;

#ifdef USE_MSVC_CRT
    /* MSVC CRTs _wgetcwd will allocate memory when buf is NULL. */
    if (!buf) {
        buf = (tchar*)malloc(cwd_size * sizeof(tchar));
        if (!buf) {
            mock.internal_error = ENOMEM;
            return nullptr;
        }
        size = cwd_size;
    }
#endif

    if (size < cwd_size) {
        mock.internal_error = ERANGE;
        return nullptr;
    }

    for (size_t i = 0; i < cwd_size - 1; ++i) {
        buf[i] = (tchar)'X';
    }
    buf[cwd_size - 1] = 0;
    return buf;
}

/* Posix Directory Reading */
#define TEST_DT_DIR 1

struct test_dirent {
    unsigned char d_type;
    char d_name[256];
};
test_dirent global_dirent;

file_io_mock* test_opendir(const char* dir) {
    if (mock.test_fail(fail_opendir)) return nullptr;
    if (!dir) {
        mock.internal_error = EINVAL;
        return nullptr;
    }

    if (mock.entries.empty()) {
        mock.internal_error = ENOTDIR;
        return nullptr;
    }

    mock.readdir_index = 0;
    return &mock;
}

test_dirent* test_readdir(file_io_mock* handle) {
    TM_ASSERT(handle);
    TM_ASSERT(handle == &mock);
    TM_ASSERT(!mock.entries.empty());

    if (mock.test_fail(fail_readdir)) return nullptr;
    if (mock.readdir_index >= (int)mock.entries.size()) return nullptr;

    const auto* entry = &mock.entries[mock.readdir_index];
    ++mock.readdir_index;

    global_dirent.d_type = (entry->type == mock_file) ? 0 : TEST_DT_DIR;
    size_t len = std::min((size_t)256, entry->path.size());
    memcpy(global_dirent.d_name, entry->path.data(), len);
    if (len < 256)
        global_dirent.d_name[len] = 0;
    else
        global_dirent.d_name[len - 1] = 0;
    return &global_dirent;
}

void test_closedir(file_io_mock* handle) {
    TM_ASSERT(handle);
    TM_ASSERT(handle == &mock);
    mock.readdir_index = 0;
}

/* Windows Crt Directory Reading */
#define TEST_A_SUBDIR 1

struct test_wfinddata64_t {
    unsigned attrib;
    tmu_char16 name[260];
};

static void helper_copy_next_index(test_wfinddata64_t* data) {
    memset(data, 0, sizeof(test_wfinddata64_t));
    data->attrib = (mock.entries[mock.readdir_index].type == mock_dir) ? TEST_A_SUBDIR : 0;
    const auto& path = mock.entries[mock.readdir_index].path;
    auto len = std::min(path.size(), (size_t)260);
    memcpy(data->name, path.data(), len * sizeof(tmu_char16));
    if (len < 260)
        data->name[len] = 0;
    else
        data->name[len - 1] = 0;
    ++mock.readdir_index;
}

intptr_t test_wfindfirst64(const tmu_char16* dir, test_wfinddata64_t* data) {
    TM_ASSERT(dir);
    TM_ASSERT(data);

    if (mock.test_fail(fail_wfindfirst64)) return -1;

    size_t len = test_strlen(dir);
    if (!len || dir[len - 1] == L'\\' || dir[len - 1] == L'/') {
        mock.internal_error = EINVAL;
        return -1;
    }

    if (mock.entries.empty()) {
        mock.internal_error = ENOENT;
        return -1;
    }

    mock.readdir_index = 0;
    helper_copy_next_index(data);
    return 1;
}

void test_findclose(intptr_t handle) {
    TM_ASSERT(handle == -1 || handle == 1);
    return;
}

int test_wfindnext64(intptr_t handle, test_wfinddata64_t* data) {
    if (mock.test_fail(fail_wfindnext64)) return -1;
    if (handle == -1) {
        mock.internal_error = EINVAL;
        return -1;
    }
    if (mock.readdir_index >= (int)mock.entries.size()) {
        mock.internal_error = ENOENT;
        return -1;
    }
    helper_copy_next_index(data);
    return 0;
}

intptr_t test_readlink(const char* dir, char* out, size_t out_size) {
    if (mock.test_fail(fail_readlink)) return -1;
    if (!out || out_size == 0) {
        mock.internal_error = EINVAL;
        return -1;
    }
    if (strcmp(dir, "/proc/self/exe") != 0) {
        mock.internal_error = EINVAL;
        return -1;
    }

    size_t min_len = std::min(mock.module_filename.size(), out_size);
    if (min_len) {
        for (size_t i = 0; i < min_len; ++i) {
            out[i] = (char)mock.module_filename[i];
        }
    }

    return (intptr_t)min_len;
}

int test_vsnprintf(char* s, size_t n, const char* format, va_list args) {
    TM_ASSERT(s || n == 0);
    TM_ASSERT(format);
    (void)args;
    if (mock.test_fail(fail_vsnprintf)) return -1;
    size_t required_size = test_strlen(format);
    size_t min_len = std::min(required_size, n);
    if (min_len > 0) {
        memcpy(s, format, min_len * sizeof(char));
    }
    if (n > min_len)
        s[min_len] = 0;
    else if (min_len > 0)
        s[min_len - 1] = 0;
    return (int)required_size;
}

int test__vscprintf(const char *format, va_list args) {
    (void)args;
    if (mock.test_fail(fail__vscprintf)) return -1;
    return (int)test_strlen(format);
}

int test__vsnprintf(char* s, size_t n, const char* format, va_list args) {
    TM_ASSERT(s || n == 0);
    TM_ASSERT(format);
    (void)args;
    if (mock.test_fail(fail__vsnprintf)) return -1;
    size_t required_size = test_strlen(format);
    size_t min_len = std::min(required_size, n);
    if (min_len > 0) {
        memcpy(s, format, min_len * sizeof(char));
    }
    if (n < required_size) return -1;
    if (n > required_size) s[required_size] = 0;
    return (int)required_size;
}

int test_vfprintf(open_file* stream, const char* format, va_list args) {
    TM_ASSERT(stream);
    TM_ASSERT(format);
    (void)args;
    if (mock.test_fail(fail_vfprintf)) return -1;
    if (!stream->is_open) return -1;
    if (!stream->is_writable) return -1;

    size_t required_size = test_strlen(format);
    size_t min_len = std::min(required_size, stream->remaining());
    if (min_len > 0) {
        memcpy(stream->write_contents, format, min_len * sizeof(char));
        stream->bytes_written += min_len;
    }
    return (int)required_size;
}

int test_vprintf(const char* format, va_list args) {
    auto stream = &mock.out.file;

    TM_ASSERT(stream);
    TM_ASSERT(format);
    (void)args;
    if (mock.test_fail(fail_vprintf)) return -1;
    if (!stream->is_open) return -1;
    if (!stream->is_writable) return -1;

    size_t required_size = test_strlen(format);
    size_t min_len = std::min(required_size, stream->remaining());
    if (min_len > 0) {
        memcpy(stream->write_contents + stream->bytes_written, format, min_len * sizeof(char));
        stream->bytes_written += min_len;
    }
    return (int)required_size;
}

int test_fwprintf(open_file* stream, const tmu_char16* format, ...) {
    TM_ASSERT(stream);
    TM_ASSERT(format);
    TM_ASSERT(format[0] == L'%');
    TM_ASSERT(format[1] == L'l');
    TM_ASSERT(format[2] == L's');
    TM_ASSERT(format[3] == 0);

    auto impl = [](open_file* stream, const tmu_char16* str) {
        if (mock.test_fail(fail_vprintf)) return -1;
        if (!stream->is_open) return -1;
        if (!stream->is_writable) return -1;

        size_t required_size = test_strlen(str);
        if (stream == &mock.out.file) {
            mock.out.wdata.insert(mock.out.wdata.end(), str, str + required_size);
            return (int)required_size;
        } else if (stream == &mock.err.file) {
            mock.err.wdata.insert(mock.err.wdata.end(), str, str + required_size);
            return (int)required_size;
        }

        size_t min_len = std::min(required_size, stream->remaining());
        for (size_t i = 0; i < min_len; ++i) {
            stream->write_contents[stream->bytes_written + i] = (char)str[i];
        }
        stream->bytes_written += min_len;
        return (int)min_len;
    };

    va_list args;
    va_start(args, format);
    const tmu_char16* str = va_arg(args, const tmu_char16*);
    auto result = impl(stream, str);
    va_end(args);
    return result;
}

int test__isatty(int fd) {
    if (mock.test_fail(fail__isatty)) return 0;
    switch (fd) {
        case 0:
            return !mock.in.is_file;
        case 1:
            return !mock.out.is_file;
        case 2:
            return !mock.err.is_file;
    }
    TM_ASSERT(false);
    mock.internal_error = EBADF;
    return 0;
}

int test__fileno(open_file* stream) {
    if (mock.test_fail(fail__fileno)) return -1;
    if (stream == &mock.in.file) return 0;
    if (stream == &mock.out.file) return 1;
    if (stream == &mock.err.file) return 2;
    TM_ASSERT(false);
    mock.internal_error = EINVAL;
    return -1;
}

#define _O_U16TEXT 1

int test__setmode(int fd, int mode) {
    if (mock.test_fail(fail__setmode)) return -1;
    if (mode != _O_U16TEXT) {
        mock.internal_error = EINVAL;
        return -1;
    }
    if (fd < 0 || fd > 2) {
        mock.internal_error = EBADF;
        return -1;
    }
    return 0;
}

#ifdef USE_WINDOWS_H
#include "windows_mockups.h"
#endif

/* TMU Macro Mockups */
#define TMU_S_ISDIR(x) ((x) == mock_dir)
#define TMU_S_ISREG(x) ((x) == mock_file)
#define TMU_TEXT test_to_tchar

/* General CRT Mockups */
#define FILE open_file
#define errno mock.internal_error
#define fread test_fread
#define fwrite test_fwrite
#define fclose test_fclose
#define ferror test_ferror
#define strchr test_strchr
#define wcslen test_strlen
#define wcschr test_strchr
#define vsnprintf test_vsnprintf
#define vfprintf test_vfprintf
#define vprintf test_vprintf
#define fwprintf test_fwprintf
#define stdin &mock.in.file
#define stdout &mock.out.file
#define stderr &mock.err.file

/* Posix CRT Mockups */
#define fopen test_fopen
#define freopen test_freopen
#define stat test_stat
#define mkdir test_mkdir
#define rmdir test_rmdir
#define remove test_remove
#define remove test_remove
#define rename test_rename
#define getcwd test_getcwd
#define DIR file_io_mock
#define dirent test_dirent
#define opendir test_opendir
#define readdir test_readdir
#define closedir test_closedir
#define DT_DIR TEST_DT_DIR
#define readlink test_readlink
#define ssize_t intptr_t

/* MSVC CRT Mockups */
#define _wfopen test_fopen
#define _wfreopen test_freopen
#define __stat64 test_stat
#define _wstat64 test_stat
#define _wmkdir test_mkdir
#define _wrmdir test_rmdir
#define _wremove test_remove
#define _wremove test_remove
#define _wrename test_rename
#define _wgetcwd test_getcwd
#define __argv mock.argv.data()
#define __wargv mock.wargv.data()
#define _wfindfirst64 test_wfindfirst64
#define _wfindnext64 test_wfindnext64
#define _wfinddata64_t test_wfinddata64_t
#define _findclose test_findclose
#define _A_SUBDIR TEST_A_SUBDIR
#define _vscprintf test__vscprintf
#define _vsnprintf test__vsnprintf
#define _isatty test__isatty
#define _fileno test__fileno
#define _setmode test__setmode