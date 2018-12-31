#include <string_view>
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) (x).size()

#if defined(_WIN32) && 0
#define TMF_USE_WINDOWS_H
#else
#define TMF_USE_CRT
#endif

#ifdef TMF_USE_WINDOWS_H
#include <windows.h>
#endif

#ifdef TMF_USE_CRT
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <sys/stat.h> /* stat function */
#include <errno.h> /* errno */
#include <stdio.h> /* remove */
#endif
#endif

#define TM_FILE_IMPLEMENTATION
#include "main.cpp"

int main() { return 0; }