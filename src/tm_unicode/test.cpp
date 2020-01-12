#if 0
#include <string_view>
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) (x).size()
#endif

#define TMU_USE_CONSOLE

#if defined(_WIN32) && 0
#define TMU_USE_WINDOWS_H
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

int main() { return 0; }