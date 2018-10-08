#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <assert_throws.h>
#include <assert_throws.cpp>

#include <string_view>
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) ((tm_size_t)(x).size())

#define TM_STRINGUTIL_IMPLEMENTATION
#include <tm_stringutil.h>
