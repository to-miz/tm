#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <assert_throws.h>
#include <assert_throws.cpp>

#include <string_view>
#include <type_traits>

// clang-format off
#ifdef USE_SIGNED_SIZE_T
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 1
    typedef int tm_size_t;
#endif

#ifdef USE_STRING_VIEW
    #define TM_STRING_VIEW std::string_view
    #define TM_STRING_VIEW_DATA(x) (x).data()
    #define TM_STRING_VIEW_SIZE(x) ((tm_size_t)(x).size())
    #define TM_STRING_VIEW_MAKE(data, size) std::string_view{(data), (size_t)(size)}
#endif
// clang-format on

// make stringutil use custom implementations of stricmp and strstr
#include <cstring>
#define TM_MEMCHR std::memchr
#define TM_STRCHR std::strchr
#define TM_STRLEN std::strlen
#define TM_STRNCMP std::strncmp
#define TM_MEMCMP std::memcmp

#define TM_STRINGUTIL_IMPLEMENTATION
#include <tm_stringutil.h>

// Testing code starts here.

using std::string_view;

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

#include "unescaped_tests.cpp"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

TEST_CASE("Test ordering") {
    auto test_ordering = [](auto func, auto nz_func, string_view a, string_view b) {
        INFO("a is " << a << " and b is " << b);
        int expected = nz_func(a.data(), b.data());
        if (expected > 0) {
            CHECK(func(a.data(), a.data() + a.size(), b.data(), b.data() + b.size()) > 0);
            CHECK(func(b.data(), b.data() + b.size(), a.data(), a.data() + a.size()) < 0);
        } else if (expected < 0) {
            CHECK(func(a.data(), a.data() + a.size(), b.data(), b.data() + b.size()) < 0);
            CHECK(func(b.data(), b.data() + b.size(), a.data(), a.data() + a.size()) > 0);
        } else {
            CHECK(func(a.data(), a.data() + a.size(), b.data(), b.data() + b.size()) == 0);
            CHECK(func(b.data(), b.data() + b.size(), a.data(), a.data() + a.size()) == 0);
        }
    };
    test_ordering(tmsu_compare_n, strcmp, "Apple", "Cat");
    test_ordering(tmsu_compare_n, strcmp, "", "Cat");
    test_ordering(tmsu_compare_n, strcmp, "Ca", "Cat");
    test_ordering(tmsu_compare_n, strcmp, "Cat", "Cat");
    test_ordering(tmsu_compare_n, strcmp, "10", "2");

    test_ordering(tmsu_compare_ignore_case_ansi_n, strcasecmp, "Apple", "Cat");
    test_ordering(tmsu_compare_ignore_case_ansi_n, strcasecmp, "", "Cat");
    test_ordering(tmsu_compare_ignore_case_ansi_n, strcasecmp, "Ca", "Cat");
    test_ordering(tmsu_compare_ignore_case_ansi_n, strcasecmp, "Cat", "Cat");
    test_ordering(tmsu_compare_ignore_case_ansi_n, strcasecmp, "cAT", "Cat");
    test_ordering(tmsu_compare_ignore_case_ansi_n, strcasecmp, "10", "2");
}