typedef wchar_t tmf_tchar;
#define TMF_TEXT(x) L##x
#define TMF_TEXTLEN TMF_WCSLEN
#define TMF_TEXTCHR TMF_STRCHRW
#define TMF_DIR_DELIM L'\\'

#define TMF_STAT _wstat64
#define TMF_STRUCT_STAT struct __stat64
#define TMF_S_IFDIR S_IFDIR
#define TMF_S_IFREG S_IFREG
#define TMF_MKDIR _wmkdir
#define TMF_RMDIR _wrmdir
#define TMF_REMOVE _wremove
#define TMF_RENAME _wrename
#define TMF_GETCWD _wgetcwd

typedef struct {
    tmf_tchar* path;
    tm_size_t size;
} tmf_platform_path;

tmf_platform_path tmf_to_platform_path(const char* path) {
    TM_UNREFERENCED_PARAM(path);
    tmf_platform_path result = {TM_NULL, 0};
    return result;
}

tmf_platform_path tmf_to_platform_path_n(const char* path, tm_size_t size) {
    TM_UNREFERENCED_PARAM(path);
    TM_UNREFERENCED_PARAM(size);
    tmf_platform_path result = {TM_NULL, 0};
    return result;
}

static tmf_contents_result tmf_to_utf8(const tmf_tchar* str, tm_size_t extra_size) {
    TM_UNREFERENCED_PARAM(str);
    TM_UNREFERENCED_PARAM(extra_size);
    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};
    return result;
}

tmf_contents_result tmf_current_working_directory(tm_size_t extra_size) {
    TM_ASSERT_VALID_SIZE(extra_size);

    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    tmf_tchar* dir = _wgetcwd(TM_NULL, 1);
    if (!dir) {
        result.ec = TM_ENOMEM;
        return result;
    }

    result = tmf_to_utf8(dir, extra_size + 1);
    if (result.ec == TM_OK) tmf_to_tmf_path(&result.contents, /*is_dir=*/TM_TRUE);
    free(dir); /* _wgetcwd calls specifically malloc, we need to directly use free instead of TMF_FREE.*/
    return result;
}