#if 0
#include <string_view>
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) (x).size()
#endif

#define TMU_USE_CONSOLE

#if defined(_WIN32) && 0
#define TMU_USE_WINDOWS_H
#define TMU_NO_SHELLAPI
#else
#define TMU_USE_CRT
#endif

#ifdef TMU_USE_WINDOWS_H
#include <windows.h>
#endif

#if 0
#define TM_SIZE_T_DEFINED
#define TM_SIZE_T_IS_SIGNED 1
typedef int tm_size_t;
#endif

#define TMU_USE_STL
#define TM_UNICODE_IMPLEMENTATION
#include "main.cpp"

int main() {
    auto exe_path = tmu_module_filename();
    if (exe_path.ec == TM_OK) {
        printf("%s\n", exe_path.contents.data);
    } else {
        printf("ERROR\n");
    }
    tmu_destroy_contents(&exe_path.contents);

    auto exe_dir = tmu_module_directory();
    if (exe_dir.ec == TM_OK) {
        printf("%s\n", exe_dir.contents.data);
    } else {
        printf("ERROR\n");
    }
    tmu_destroy_contents(&exe_dir.contents);

    auto dir = tmu_open_directory(nullptr);
    while (auto entry = tmu_read_directory(&dir)) {
        printf("%s\n", entry->name);
    }
    tmu_close_directory(&dir);
    return 0;
}
