#include <string_view>
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) (x).size()

#define TMF_USE_WINDOWS_H
#include <windows.h>

#define TM_FILE_IMPLEMENTATION
#include "main.cpp"

int main() { return 0; }