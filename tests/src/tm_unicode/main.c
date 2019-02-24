#ifdef USE_WINDOWS_H
#define TMU_USE_WINDOWS_H
#include <windows.h>
#else
#define TMU_USE_CRT
#endif

#define TM_UNICODE_IMPLEMENTATION
#include <tm_unicode.h>

int main() { return 0; }
