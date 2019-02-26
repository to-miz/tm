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
	#define _MSC_VER 1
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

struct file_io_mock {
    int stat_should_fail_with;
    int mkdir_should_fail_with;
    int rmdir_should_fail_with;
    int remove_should_fail_with;
    int rename_should_fail_with;
    int getcwd_should_fail_with;
    int fopen_should_fail_with;
    int freopen_should_fail_with;
    int fwrite_should_fail_with;
    int fread_should_fail_with;

    std::vector<file_io_entry> entries;
    open_file file;

    void clear() {
        stat_should_fail_with = 0;
        mkdir_should_fail_with = 0;
        rmdir_should_fail_with = 0;
        remove_should_fail_with = 0;
        rename_should_fail_with = 0;
        getcwd_should_fail_with = 0;
        fopen_should_fail_with = 0;
        freopen_should_fail_with = 0;
        fwrite_should_fail_with = 0;
        fread_should_fail_with = 0;
        file = {};
        file.file_io_entry_index = -1;

        entries.clear();
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
int internal_errno = 0;

open_file* test_fopen(const tchar* filename, const tchar* mode) {
    TM_ASSERT(filename);
    TM_ASSERT(mode);

    TM_ASSERT(!mock.file.is_open);
    TM_ASSERT(!mock.file.is_writable);

    internal_errno = 0;

    if (mock.fopen_should_fail_with) {
        internal_errno = mock.fopen_should_fail_with;
        mock.fopen_should_fail_with = 0;
        return nullptr;
    }

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
	    internal_errno = ENOENT;
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

    internal_errno = 0;
    if (mock.freopen_should_fail_with) {
        internal_errno = mock.freopen_should_fail_with;
        mock.freopen_should_fail_with = 0;
        current->has_error = true;
        return nullptr;
    }

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

    if (mock.fread_should_fail_with) {
        internal_errno = mock.fread_should_fail_with;
        mock.fread_should_fail_with = 0;
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

    if (mock.fwrite_should_fail_with) {
        internal_errno = mock.fwrite_should_fail_with;
        mock.fwrite_should_fail_with = 0;
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

    internal_errno = 0;
    if (mock.stat_should_fail_with != 0) {
        internal_errno = mock.stat_should_fail_with;
        mock.stat_should_fail_with = 0;
        return -1;
    }

    auto entry_index = mock.find(filename);
    if (entry_index < 0) {
        internal_errno = ENOENT;
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

    internal_errno = 0;
    if (mock.mkdir_should_fail_with != 0) {
        internal_errno = mock.mkdir_should_fail_with;
        mock.mkdir_should_fail_with = 0;
        return -1;
    }

    if (!*path) {
        internal_errno = ENOENT;
        return -1;
    }

    if (mock.find_dir(path) >= 0) {
        internal_errno = EEXIST;
        return -1;
    }
    if (mock.find_file(path) >= 0) {
        internal_errno = ENOTDIR;
        return -1;
    }

    auto parent_size = get_parent_path_size(path);
    if (parent_size > 0 && mock.find_dir(path, parent_size) < 0) {
    	internal_errno = ENOENT;
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

    internal_errno = 0;
    if (mock.rmdir_should_fail_with != 0) {
        internal_errno = mock.rmdir_should_fail_with;
        mock.rmdir_should_fail_with = 0;
        return -1;
    }

    if (!*path) {
        internal_errno = ENOENT;
        return -1;
    }

    auto index = mock.find_dir(path);
    if (index < 0) {
    	internal_errno = ENOENT;
    	return -1;
    }

    mock.entries.erase(mock.entries.begin() + index);
    return 0;
}

int test_remove(const tchar* path) {
    TM_ASSERT(path);

    internal_errno = 0;
    if (mock.remove_should_fail_with != 0) {
        internal_errno = mock.remove_should_fail_with;
        mock.remove_should_fail_with = 0;
        return -1;
    }

    auto index = mock.find_file(path);
    if (index < 0) {
    	internal_errno = ENOENT;
    	return -1;
    }

    mock.entries.erase(mock.entries.begin() + index);
    return 0;
}

int test_rename(const tchar* from, const tchar* to) {
    TM_ASSERT(from);
    TM_ASSERT(to);

    internal_errno = 0;

    if (mock.rename_should_fail_with != 0) {
        internal_errno = mock.rename_should_fail_with;
        mock.rename_should_fail_with = 0;
        return -1;
    }

    auto from_index = mock.find(from);
    if (from_index < 0) {
        internal_errno = ENOENT;
        return -1;
    }

    auto to_index = mock.find(to);
    if (to_index >= 0) {
        internal_errno = EACCES;
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

    if (mock.getcwd_should_fail_with != 0) {
        internal_errno = mock.getcwd_should_fail_with;
        mock.getcwd_should_fail_with = 0;
        return nullptr;
    }

    const size_t cwd_size = 1050;

#ifdef USE_MSVC_CRT
    /* MSVC CRTs _wgetcwd will allocate memory when buf is NULL. */
    if (!buf) {
        buf = (tchar*)malloc(cwd_size * sizeof(tchar));
        if (!buf) {
            internal_errno = ENOMEM;
            return nullptr;
        }
        size = cwd_size;
    }
#endif

    if (size < cwd_size) {
        internal_errno = ERANGE;
        return nullptr;
    }

    for (size_t i = 0; i < cwd_size - 1; ++i) {
        buf[i] = (tchar)'X';
    }
    buf[cwd_size - 1] = 0;
    return buf;
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
#define errno internal_errno
#define fread test_fread
#define fwrite test_fwrite
#define fclose test_fclose
#define ferror test_ferror
#define strchr test_strchr
#define wcslen test_strlen
#define wcschr test_strchr

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