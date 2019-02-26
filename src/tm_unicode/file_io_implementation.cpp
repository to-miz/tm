#include "file_io_functions.h"

static tm_errc tmu_create_directory_internal(const tmu_tchar* dir, tm_size_t dir_len);
static tm_size_t tmu_get_path_len_internal(const tmu_tchar* filename, tm_size_t filename_len);
struct tmu_contents_struct;
static void tmu_to_tmu_path(struct tmu_contents_struct* path, tm_bool is_dir);
TMU_DEF tm_bool tmu_grow_by(struct tmu_contents_struct* contents, tm_size_t amount);

/* Platform Tests */
#if defined(_MSC_VER) || defined(TMU_TESTING_MSVC_CRT)

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
#undef TMU_PLATFORM_UNIX

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

#include "common_implementation.cpp"