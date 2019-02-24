typedef char tmf_tchar;
#define TMF_TEXT(x) x
#define TMF_TEXTLEN strlen
#define TMF_TEXTCHR strchr
#define TMF_DIR_DELIM '/'

#define TMF_STAT stat
#define TMF_STRUCT_STAT struct stat
#define TMF_S_IFDIR S_IFDIR
#define TMF_S_IFREG S_IFREG
#define TMF_MKDIR(dir) mkdir((dir), /*mode=*/0x0777u) /* 0x0777u is read, write, execute permissions for all types. */
#define TMF_RMDIR remove
#define TMF_REMOVE remove
#define TMF_RENAME rename
#define TMF_GETCWD getcwd

typedef struct {
    const tmf_tchar* path;
    tm_size_t size;
} tmf_platform_path;

tmf_platform_path tmf_to_platform_path(const char* path) {
    tmf_platform_path result;
    result.path = path;
    result.size = 0;
    return result;
}

tmf_platform_path tmf_to_platform_path_n(const char* path, tm_size_t size) {
	/* FIXME: This is wrong, path might not be nullterminated. */
    tmf_platform_path result;
    result.path = path;
    result.size = size;
    return result;
}

void tmf_destroy_platform_path(tmf_platform_path*) {}