#include <iostream>
#include <string_view>

#define TM_JSON_IMPLEMENTATION
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) ((tm_size_t)(x).size())
#include <tm_json.h>

int main() {}