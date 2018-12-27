/* Use malloc if provided, otherwise fall back to heap. */
#ifdef TM_REALLOC
	#define TMF_MALLOC TM_MALLOC
	#define TMF_REALLOC TM_REALLOC
	#define TMF_FREE TM_FREE
#else
	#define TMF_MALLOC(type, size, alignment) (type*)malloc((size) * sizeof(type))
	#define TMF_REALLOC(type, ptr, old_size, old_alignment, new_size, new_alignment) \
	    (type*)realloc((ptr), (new_size) * sizeof(type))
	#define TMF_FREE(ptr, size, alignment) free((ptr))
#endif

#ifdef TM_MEMMOVE
	#define TMF_MEMMOVE TM_MEMMOVE
#else
	#define TMF_MEMMOVE memmove
#endif

#ifdef TM_MEMCPY
    #define TMF_MEMCPY TM_MEMCPY
#else
    #define TMF_MEMCPY memcpy
#endif

typedef WCHAR tmf_tchar;

#ifdef TM_WCSCHR
	#define TMF_STRCHRW TM_WCSCHR
#else
	#define TMF_STRCHRW wcschr
#endif

#define TMF_STRLEN strlen
#define TMF_WCSLEN wcslen

typedef struct {
    tmf_tchar* path;
    tm_size_t size;
} tmf_platform_path;

tmf_exists_result tmf_file_exists_t(const WCHAR* filename) {
    TM_ASSERT(filename);

    tmf_exists_result result = {TM_FALSE, TM_OK};

    struct __stat64 buffer;
    int stat_result = _wstat64(filename, &buffer);

    if (stat_result == 0) {
        result.exists = (buffer.st_mode & _S_IFREG) != 0;
    } else if (stat_result == -1) {
    	result.exists = false;
    } else {
        result.ec = TM_EINVAL;
    }

    return result;
}

tmf_exists_result tmf_directory_exists_t(const WCHAR* dir) {
    TM_ASSERT(dir);

    tmf_exists_result result = {TM_FALSE, TM_OK};

    struct __stat64 buffer;
    int stat_result = _wstat64(dir, &buffer);

    if (stat_result == 0) {
        result.exists = (buffer.st_mode & _S_IFDIR) != 0;
    } else if (stat_result == -1) {
    	result.exists = false;
    } else {
        result.ec = TM_EINVAL;
    }

    return result;
}

tmf_file_timestamp_result tmf_file_timestamp_t(const WCHAR* dir) {
    TM_ASSERT(dir);

    tmf_file_timestamp_result result = {0, TM_OK};

    struct __stat64 buffer;
    int stat_result = _wstat64(dir, &buffer);

    if (stat_result == 0) {
        result.file_time = buffer.st_mtime;
    } else {
        result.ec = (stat_result == TM_EINVAL) ? TM_EINVAL : TM_ENOENT;
    }

    return result;
}

static tm_errc tmf_create_single_directory_t(const WCHAR* dir) {
    tm_errc result = TM_OK;

    int err = _wmkdir(dir);
    if (err != 0) {
        if (errno != EEXIST) {
            result = errno;
        }
    }
    return result;
}

tm_errc tmf_delete_file_t(const char* filename) {
    tm_errc result = TM_OK;
    if (_wremove(filename) != 0) result = errno;
    return result;
}

tm_errc tmf_delete_directory_t(const WCHAR* dir) {
    tm_errc result = TM_OK;
    if (_wrmdir(dir) != 0) result = errno;
    return result;
}

tm_errc tmf_rename_file_ex_t(const WCHAR* from, const WCHAR* to, uint32_t flags) {
    tm_errc result = TM_OK;

    int err = _wrename(from, to);
    if (err != 0) {
        if (errno == EACCES && (flags & tmf_overwrite)) {
            /* Destination probably exists, try to delete file since tmf_overwrite is specified and try again. */
            result = tmf_delete_file_t(to);
            if (result == TM_OK) {
                err = _wrename(from, to);
                if (err != 0) result = errno;
            }
        } else {
            result = errno;
        }
    }
    return result;
}

static tmf_contents_result tmf_to_utf8(const WCHAR* str, tm_size_t extra_size) {
    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};
    return result;
}

tmf_contents_result tmf_current_working_directory(tm_size_t extra_size) {
    TM_ASSERT_VALID_SIZE(extra_size);

    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    WCHAR* dir = _wgetcwd(TM_NULL, 1);
    if (!dir) {
        result.ec = TM_ENOMEM;
        return result;
    }

    result = tmf_to_utf8(dir, extra_size + 1);
    if (result.ec == TM_OK) tmf_to_tmf_path(&result.contents, /*is_dir=*/TM_TRUE);
    free(dir); /* _wgetcwd calls specifically malloc, we need to directly use free instead of TMF_FREE.*/
    return result;
}