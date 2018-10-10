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

template <class Func, class... T>
int ptr_to_index(Func func, const char* str, T... args) {
    auto result = func(str, args...);
    return (int)(result - str);
}

TEST_CASE("Test escaped find") {
    CHECK(*tmsu_find_char_escaped("a", 'a', 'a') == 'a');
    CHECK(*tmsu_find_char_escaped("aa", 'a', 'a') == 0);
    CHECK(ptr_to_index(tmsu_find_char_escaped, "aaa", 'a', 'a') == 2);

    CHECK(*tmsu_find_first_of_escaped("a", "ab", 'a') == 'a');
    CHECK(*tmsu_find_first_of_escaped("aaab", "ab", 'a') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_escaped, "aaaba", "ab", 'a') == 4);

    CHECK(ptr_to_index(tmsu_find_first_of_escape_chars, "aaab", "ab", "ab") == 3);
    CHECK(*tmsu_find_first_of_escape_chars("aabb", "ab", "ab") == 0);
    CHECK(*tmsu_find_first_of_escape_chars("bbaa", "ab", "ab") == 0);
}