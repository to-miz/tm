#include "file_io_functions.h"

static tm_errc tmu_create_directory_internal(const tmu_tchar* dir, tm_size_t dir_len);
static tm_size_t tmu_get_path_len_internal(const tmu_tchar* filename, tm_size_t filename_len);
struct tmu_contents_struct;
static void tmu_to_tmu_path(struct tmu_contents_struct* path, tm_bool is_dir);
TMU_DEF tm_bool tmu_grow_by(struct tmu_contents_struct* contents, tm_size_t amount);

#if defined(_WIN32) && !defined(TMU_TESTING_UNIX)
struct tmu_platform_path_struct;
static tm_bool tmu_internal_append_wildcard(struct tmu_platform_path_struct* dir, const tmu_tchar** out);
#endif

/* Platform Tests */
#if defined(_MSC_VER) || defined(TMU_TESTING_MSVC_CRT) || (defined(__MINGW32__) && !defined(TMU_TESTING_UNIX))


#if defined(TMU_USE_CRT)
static FILE* tmu_fopen_t(const tmu_tchar* filename, const tmu_tchar* mode) { return _wfopen(filename, mode); }
static FILE* tmu_freopen_t(const tmu_tchar* filename, const tmu_tchar* mode, FILE* current) {
    return _wfreopen(filename, mode, current);
}
#endif /* defined(TMU_USE_CRT) */

#if defined(TMU_USE_WINDOWS_H)

#include "windows_implementation.cpp"

#elif defined(TMU_USE_CRT)

#define TMU_IMPLEMENT_CRT
#include "msvc_crt.cpp"

#else
#error tm_unicode.h needs either TMU_USE_WINDOWS_H or TMU_USE_CRT to be defined.
#endif /* defined(TMU_USE_WINDOWS_H) || defined(TMU_USE_CRT) */

#elif defined(TMU_PLATFORM_UNIX)

#ifndef TMU_USE_CRT
#error tm_unicode.h needs TMU_USE_CRT on this platform.
#endif

#define TMU_IMPLEMENT_CRT
#include "unix_crt.cpp"

#else
#error Not implemented on this platform.
#endif /* Platform Tests */

#if defined(TMU_IMPLEMENT_CRT)
#undef TMU_IMPLEMENT_CRT

#include "common_crt.cpp"

#endif /* defined(TMU_IMPLEMENT_CRT) */

#if defined(TMU_USE_CONSOLE) && defined(TMU_USE_CRT)
TMU_DEF tmu_console_handle tmu_file_to_console_handle(FILE* f) {
    if (f == stdin) return tmu_console_in;
    if (f == stdout) return tmu_console_out;
    if (f == stderr) return tmu_console_err;
    return tmu_console_invalid;
}

#if defined(TMU_PLATFORM_UNIX)
TMU_DEF int tmu_printf(TMU_FORMAT_STRING(const char* format), ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

TMU_DEF int tmu_vprintf(const char* format, va_list args) {
    return vprintf(format, args);
}

TMU_DEF int tmu_fprintf(FILE* stream, TMU_FORMAT_STRING(const char* format), ...) {
    va_list args;
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);
    return result;
}

TMU_DEF int tmu_vfprintf(FILE* stream, const char* format, va_list args) {
    return vfprintf(stream, format, args);
}

#else

static int tmu_internal_vsprintf(char* sbo, size_t sbo_size, char** out, const char* format, va_list args);

#if (!defined(_MSC_VER) || _MSC_VER >= 1900 || defined(__clang__)) && !defined(TMU_TESTING_OLD_MSC)
#define TMU_ALLOC_OFFSET 1
// Use vsnprint if available
static int tmu_internal_vsprintf(char* sbo, size_t sbo_size, char** out, const char* format, va_list args) {
    va_list args_cp;
    va_copy(args_cp, args);
    int needed_size = vsnprintf(sbo, sbo_size, format, args_cp);
    va_end(args_cp);
    if (needed_size <= 0) return needed_size;
    if ((size_t)needed_size < sbo_size) {
        *out = sbo;
        return needed_size;
    }
    *out = (char*)TMU_MALLOC((size_t)needed_size + 1, sizeof(char));
    if (!*out) {
        errno = ENOMEM;
        return -1;
    }
    return vsnprintf(*out, needed_size + 1, format, args);
}
#else
#define TMU_ALLOC_OFFSET 0
// We are on an old version of MSVC so we need a workaround for non standard vsnprintf.
static int tmu_internal_vsprintf(char* sbo, size_t sbo_size, char** out, const char* format, va_list args) {
    va_list args_cp;
    va_copy(args_cp, args);
    int needed_size = _vscprintf(format, args_cp);
    va_end(args_cp);
    if (needed_size <= 0) return needed_size;
    if ((size_t)needed_size <= sbo_size) {
        *out = sbo;
    } else {
        *out = (char*)TMU_MALLOC((size_t)needed_size, sizeof(char));
        if (!*out) {
            errno = ENOMEM;
            return -1;
        }
    }
    return _vsnprintf(*out, needed_size, format, args);
}
#endif

TMU_DEF int tmu_printf(TMU_FORMAT_STRING(const char* format), ...) {
    va_list args;
    va_start(args, format);
    int result = tmu_vprintf(format, args);
    va_end(args);
    return result;
}

TMU_DEF int tmu_fprintf(FILE* stream, TMU_FORMAT_STRING(const char* format), ...) {
    va_list args;
    va_start(args, format);
    int result = tmu_vfprintf(stream, format, args);
    va_end(args);
    return result;
}

TMU_DEF int tmu_vprintf(const char* format, va_list args) {
    char sbo[512];
    char* str = TM_NULL;
    int result = tmu_internal_vsprintf(sbo, 512, &str, format, args);
    if (result > 0 && str) {
        tmu_console_output_n(tmu_console_out, str, (tm_size_t)result);
    }
    if (str && str != sbo) {
        TMU_FREE(str);
    }
    return result;
}

TMU_DEF int tmu_vfprintf(FILE* stream, const char* format, va_list args) {
    int result = -1;
    tmu_console_handle handle = tmu_file_to_console_handle(stream);
    if (handle == tmu_console_invalid) {
        result = vfprintf(stream, format, args);
    } else {
        char sbo[512];
        char* str = TM_NULL;
        result = tmu_internal_vsprintf(sbo, 512, &str, format, args);
        if (result > 0 && str) {
            tmu_console_output_n(handle, str, (tm_size_t)result);
        }
        if (str && str != sbo) {
            TMU_FREE(str);
        }
    }
    return result;
}

#endif

#endif /* defined(TMU_USE_CONSOLE) && defined(TMU_USE_CRT) */

#include "common_implementation.cpp"