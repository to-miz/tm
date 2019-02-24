#if 0
#include <string_view>
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) (x).size()
#endif

#if defined(_WIN32) && 0
#define TMU_USE_WINDOWS_H
#else
#define TMU_USE_CRT
#endif

#ifdef TMU_USE_WINDOWS_H
#include <windows.h>
#endif

#define TMU_USE_STL
#define TM_UNICODE_IMPLEMENTATION
#include "main.cpp"

int main() { return 0; }