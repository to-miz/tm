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

TEST_CASE("trim") {
    auto trim = [](std::string_view str) {
        auto trimmed_left = tmsu_trim_left_n(str.data(), str.data() + str.size());
        auto trimmed_right = tmsu_trim_right_n(str.data(), str.data() + str.size());
        if (trimmed_left > trimmed_right) trimmed_left = trimmed_right;
        return std::string_view{trimmed_left, (size_t)(trimmed_right - trimmed_left)};
    };
    std::string_view test = "    asdasdasd    \n\t   ";
    std::string_view already_trimmed = "asdasdasd";
    CHECK(trim(test) == already_trimmed);
    CHECK(trim(already_trimmed) == already_trimmed);

    CHECK(trim("") == "");
    CHECK(trim("a") == "a");
    CHECK(trim("   ") == "");
    CHECK(trim("   \n    ") == "");
    CHECK(trim("asd") == "asd");
    CHECK(trim("asd    ") == "asd");
    CHECK(trim("   asd") == "asd");
    CHECK(trim("   asd   ") == "asd");

    SUBCASE("trim right") {
        auto trim_right = [](std::string_view str) {
            auto first = str.data();
            auto last = tmsu_trim_right_n(str.data(), str.data() + str.size());
            return std::string_view{first, (size_t)(last - first)};
        };

        CHECK(trim_right("\n") == "");
        CHECK(trim_right("\na") == "\na");
        CHECK(trim_right("a") == "a");
        CHECK(trim_right("  a") == "  a");
        CHECK(trim_right("  a  ") == "  a");
        CHECK(trim_right("    ") == "");
    }
}

TEST_CASE("find last not of") {
    auto find_last_not_of = [](const char* first, const char* last, std::string_view find_str, const char* not_found) {
        return tmsu_find_last_not_of_n_ex(first, last, find_str.data(), find_str.data() + find_str.size(), not_found);
    };

    std::string_view test = "aaaaaaac";
    auto first = test.data();
    auto last = test.data() + test.size();

    CHECK(find_last_not_of(first, last, "def", last) == last - 1);
    CHECK(find_last_not_of(first, last, "def", first) == last - 1);

    CHECK(find_last_not_of(first, first, "def", first) == first);
    CHECK(find_last_not_of(first, first, "def", last) == last);

    CHECK(find_last_not_of(first, last, "ac", last) == last);
    CHECK(find_last_not_of(first, last, "ac", first) == first);
    CHECK(find_last_not_of(first, first, "ac", first) == first);
    CHECK(find_last_not_of(first, first, "ac", last) == last);

    CHECK(find_last_not_of(first, last, "ac", nullptr) == nullptr);
    CHECK(find_last_not_of(first, last, "ac", nullptr) == nullptr);
    CHECK(find_last_not_of(first, first, "ac", nullptr) == nullptr);
    CHECK(find_last_not_of(first, first, "ac", nullptr) == nullptr);

    test = "aaaaaaacaaaaa";
    first = test.data();
    last = test.data() + test.size();

    CHECK(find_last_not_of(first, last, "a", last) == first + 7);

    test = "a";
    first = test.data();
    last = test.data() + test.size();

    CHECK(find_last_not_of(first, last, "b", last) == first);
    CHECK(find_last_not_of(first, last, "b", first) == first);
    CHECK(find_last_not_of(first, last, "b", nullptr) == first);
}

TEST_CASE("word tokenizing") {
    {
        // Check logic behind word seperators array.
        const char* word_seperators_first = TMSU_WORD_SEPERATORS;
        const char* word_seperators_last = TMSU_WORD_SEPERATORS + (sizeof(TMSU_WORD_SEPERATORS) - 1);
        REQUIRE(word_seperators_first < word_seperators_last);
        // word_seperators_last should point to the null terminator.
        REQUIRE(*word_seperators_last == 0);
    }

    {
        const char* str = "First Word";
        CHECK(tmsu_find_word_end(str) == str + 5);       // Returns " Word".
        CHECK(tmsu_find_word_end(str + 5) == str + 10);  // Returns end of string.
    }

    {
        string_view str = "  First Word   ";
        auto str_first = str.data();
        auto str_last = str.data() + str.size();
        CHECK(tmsu_find_word_start_n(str_first, str_last) == str_first + 8);       // Returns "Word   ".
        CHECK(tmsu_find_word_start_n(str_first, str_first + 8) == str_first + 2);  // Returns "  ".
        CHECK(tmsu_find_word_start_n(str_first, str_first + 2) == str_first);      // Returns beginning of string.
    }
}