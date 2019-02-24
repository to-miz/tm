#if defined(_WIN32) && 0
#define TMF_USE_WINDOWS_H
#endif

#ifdef TMF_USE_WINDOWS_H
#include <windows.h>
#endif

#define TM_FILE_IMPLEMENTATION
#define TMF_USE_CRT
#include "main.cpp"

int main() { return 0; }