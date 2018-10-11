#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <assert_throws.h>
#include <assert_throws.cpp>

#include <string_view>
#include <type_traits>
using std::string_view;
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) ((tm_size_t)(x).size())

#define TM_STRINGUTIL_IMPLEMENTATION
#include <tm_stringutil.h>

// Calls func with supplied arguments and returns index to match.
template <class Func, class... T>
int ptr_to_index(Func func, const char* str, T... args) {
    auto result = func(str, args...);
    if (!*result) return -1;
    return (int)(result - str);
}

// Calls func with const char* iterators and returns index of match.
template <class Func, class T>
int ptr_to_index_view(Func func, const char* str, T find_str, char escape_char) {
    auto str_view = string_view{str};
    const char* result = 0;
    if constexpr (std::is_same<T, char>::value) {
        result = func(str_view.data(), str_view.data() + str_view.size(), find_str, escape_char);
    } else {
        auto find_view = string_view(find_str);
        result = func(str_view.data(), str_view.data() + str_view.size(), find_view.data(),
                      find_view.data() + find_view.size(), escape_char);
    }
    if (!*result) return -1;
    return (int)(result - str);
}

TEST_CASE("Test tmsu_find_char_unescaped") {
    // Looking for unescaped chars.
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "a", 'a', '/') == 0);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "/a", 'a', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "//a", 'a', '/') == 2);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "///a", 'a', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "c///a", 'a', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "c////a", 'a', '/') == 5);

    // Looking for unescaped escape chars.
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "/", '/', '/') == 0);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "//", '/', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "///", '/', '/') == 2);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "cc///", '/', '/') == 4);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "aaa", '/', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "cccaaa", '/', '/') == -1);
}

TEST_CASE("Test tmsu_find_first_of_unescaped") {
    // Not found.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "d", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "", "", '/') == -1);

    // No escaped chars.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "a", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "b", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "c", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "abc", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "dddddabc", "abc", '/') == 5);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "a", "", '/') == 0);

    // Some escaped chars.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/a", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/b", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/c", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/ab", "abc", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/aa", "abc", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/a/aa", "abc", '/') == 4);

    // Escaped escape chars.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "//b", "abc", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "//b", "abc", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///b", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///ba", "abc", '/') == 4);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "c///a", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "d///a", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "d///ac", "abc", '/') == 5);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "d////a", "abc", '/') == 5);

    // Looking for unescaped escape chars.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/", "ab/", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "//", "ab/", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///", "ab/", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///a", "ab/", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///a", "ab/", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "cc///", "ab/", '/') == 4);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "////", "ab/", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "ccc////a", "ab/", '/') == 7);
}

TEST_CASE("Test tmsu_find_char_unescaped_n") {
    // Looking for unescaped chars.
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "a", 'a', '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "/a", 'a', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "//a", 'a', '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "///a", 'a', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "c///a", 'a', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "c////a", 'a', '/') == 5);

    // Looking for unescaped escape chars.
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "/", '/', '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "//", '/', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "///", '/', '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "cc///", '/', '/') == 4);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "aaa", '/', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "cccaaa", '/', '/') == -1);
}

TEST_CASE("Test tmsu_find_first_of_unescaped_n") {
    // Not found.
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "d", "abc", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "", "abc", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "", "", '/') == -1);

    // No escaped chars.
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "a", "abc", '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "b", "abc", '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "c", "abc", '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "abc", "abc", '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "dddddabc", "abc", '/') == 5);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "a", "", '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "a", "", '/') == 0);

    // Some escaped chars.
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "/a", "abc", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "/b", "abc", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "/c", "abc", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "/ab", "abc", '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "/aa", "abc", '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "/a/aa", "abc", '/') == 4);

    // Escaped escape chars.
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "//b", "abc", '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "//b", "abc", '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "///b", "abc", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "///ba", "abc", '/') == 4);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "c///a", "abc", '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "d///a", "abc", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "d///ac", "abc", '/') == 5);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "d////a", "abc", '/') == 5);

    // Looking for unescaped escape chars.
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "/", "ab/", '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "//", "ab/", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "///", "ab/", '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "///a", "ab/", '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "///a", "ab/", '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "cc///", "ab/", '/') == 4);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "////", "ab/", '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_first_of_unescaped_n, "ccc////a", "ab/", '/') == 7);
}