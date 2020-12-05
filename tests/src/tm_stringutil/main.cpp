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

TEST_CASE("find") {
    auto find_index = [](const char* str, const char* find_str) {
        return (tm_size_t)(tmsu_find(str, find_str) - str);
    };
    auto find_index_v = [](const char* str, const char* find_str) {
        return (tm_size_t)(tmsu_find_v(tmsu_view(str), tmsu_view(find_str)) - str);
    };
    auto find_last_index = [](const char* str, const char* find_str) {
        return (tm_size_t)(tmsu_find_last(str, find_str) - str);
    };
    auto find_last_index_v = [](const char* str, const char* find_str) {
        return (tm_size_t)(tmsu_find_last_v(tmsu_view(str), tmsu_view(find_str)) - str);
    };
    auto find_index_ignore_case = [](const char* str, const char* find_str) {
        return (tm_size_t)(tmsu_find_ignore_case_ansi(str, find_str) - str);
    };
    auto find_index_ignore_case_v = [](const char* str, const char* find_str) {
        return (tm_size_t)(tmsu_find_ignore_case_ansi_v(tmsu_view(str), tmsu_view(find_str)) - str);
    };

    typedef tm_size_t find_func_t(const char*, const char*);
    struct func_name {
        find_func_t* func;
        const char* name;
    };

    func_name test_funcs[] = {
        {find_index, "tmsu_find"},
        {find_index_v, "tmsu_find_v"},
        {find_last_index, "tmsu_find_last"},
        {find_last_index_v, "tmsu_find_last_v"},
        {find_index_ignore_case, "tmsu_find_ignore_case_ansi"},
        {find_index_ignore_case_v, "tmsu_find_ignore_case_ansi_v"},
    };

    for (auto entry : test_funcs) {
        CAPTURE(entry.name);
        CHECK(entry.func("test", "test") == 0);
        CHECK(entry.func("test", "not_test") == 4);
        CHECK(entry.func("test", "www") == 4);

        CHECK(entry.func("test", "") == 0);
        CHECK(entry.func("", "asd") == 0);

        CHECK(entry.func("testinfixtest", "infix") == 4);
        CHECK(entry.func("infiinfixtest", "infix") == 4);
        CHECK(entry.func("infiinfix", "infix") == 4);
        CHECK(entry.func("testinfix", "infix") == 4);
        CHECK(entry.func("testinfi", "infix") == 8);
    }

    func_name test_funcs_forward[] = {
        {find_index, "tmsu_find"},
        {find_index_v, "tmsu_find_v"},
        {find_index_ignore_case, "tmsu_find_ignore_case_ansi"},
        {find_index_ignore_case_v, "tmsu_find_ignore_case_ansi_v"},
    };
    for (auto entry : test_funcs_forward) {
        CAPTURE(entry.name);
        CHECK(entry.func("test", "t") == 0);
        CHECK(entry.func("testinfixtestinfix", "infix") == 4);
    }

    func_name test_funcs_backward[] = {
        {find_last_index, "tmsu_find_last"},
        {find_last_index_v, "tmsu_find_last_v"},
    };
    for (auto entry : test_funcs_backward) {
        CAPTURE(entry.name);
        CHECK(entry.func("test", "t") == 3);
        CHECK(entry.func("testinfixtestinfix", "infix") == 13);
    }

    func_name caseless[] = {
        {find_index_ignore_case, "tmsu_find_ignore_case_ansi"},
        {find_index_ignore_case_v, "tmsu_find_ignore_case_ansi_v"},
    };

    for (auto entry : caseless) {
        CAPTURE(entry.name);
        CHECK(entry.func("TEST", "t") == 0);
        CHECK(entry.func("test", "T") == 0);
        CHECK(entry.func("TEST", "test") == 0);
        CHECK(entry.func("test", "TEST") == 0);
        CHECK(entry.func("TEST", "not_test") == 4);
        CHECK(entry.func("test", "NOT_TEST") == 4);
        CHECK(entry.func("TEST", "www") == 4);
        CHECK(entry.func("test", "WWW") == 4);

        CHECK(entry.func("TEST", "") == 0);
        CHECK(entry.func("", "ASD") == 0);

        CHECK(entry.func("TESTINFIXTEST", "infix") == 4);
        CHECK(entry.func("testinfixtest", "INFIX") == 4);
        CHECK(entry.func("INFIINFIXTEST", "infix") == 4);
        CHECK(entry.func("infiinfixtest", "INFIX") == 4);
        CHECK(entry.func("INFIINFIX", "infix") == 4);
        CHECK(entry.func("infiinfix", "INFIX") == 4);
        CHECK(entry.func("TESTINFIX", "infix") == 4);
        CHECK(entry.func("testinfix", "INFIX") == 4);
        CHECK(entry.func("TESTINFI", "infix") == 8);
        CHECK(entry.func("testinfi", "INFIX") == 8);
        CHECK(entry.func("TESTINFIXTESTINFIX", "infix") == 4);
        CHECK(entry.func("testinfixtestinfix", "INFIX") == 4);
    }
}

TEST_CASE("tokenizing") {
    const char* comma_separated = "This,is,a,,test";
    const char* delimiters = ",";

    {
        auto tokenizer = tmsu_tokenizer(comma_separated);
        tmsu_view_t value = {};
        tmsu_next_token(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("This")));
        tmsu_next_token(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("is")));
        tmsu_next_token(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("a")));
        tmsu_next_token(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("")));
        tmsu_next_token(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("test")));
        CHECK(!tmsu_next_token(&tokenizer, delimiters, &value));
    }

    {
        auto tokenizer = tmsu_view(comma_separated);
        auto delims = tmsu_view(delimiters);
        tmsu_view_t value = {};
        tmsu_next_token_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("This")));
        tmsu_next_token_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("is")));
        tmsu_next_token_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("a")));
        tmsu_next_token_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("")));
        tmsu_next_token_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("test")));
        CHECK(!tmsu_next_token_v(&tokenizer, delims, &value));
    }

    {
        auto tokenizer = tmsu_tokenizer(comma_separated);
        tmsu_view_t value = {};
        tmsu_next_token_skip_empty(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("This")));
        tmsu_next_token_skip_empty(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("is")));
        tmsu_next_token_skip_empty(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("a")));
        tmsu_next_token_skip_empty(&tokenizer, delimiters, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("test")));
        CHECK(!tmsu_next_token_skip_empty(&tokenizer, delimiters, &value));
    }

    {
        auto tokenizer = tmsu_view(comma_separated);
        auto delims = tmsu_view(delimiters);
        tmsu_view_t value = {};
        tmsu_next_token_skip_empty_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("This")));
        tmsu_next_token_skip_empty_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("is")));
        tmsu_next_token_skip_empty_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("a")));
        tmsu_next_token_skip_empty_v(&tokenizer, delims, &value);
        CHECK(tmsu_equals_v(value, tmsu_view("test")));
        CHECK(!tmsu_next_token_skip_empty_v(&tokenizer, delims, &value));
    }
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

TEST_CASE("url encoding") {
    {
        const tm_size_t buffer_size = 256;
        char buffer[buffer_size];
        char decode_buffer[buffer_size];

        {
            std::string_view test = "Hello World";
            std::string_view expected = "Hello%20World";
            REQUIRE(tmsu_url_encode(test.data(), (tm_size_t)test.size(), buffer, buffer_size) ==
                    (tm_size_t)expected.size());
            REQUIRE(expected == std::string_view(buffer, expected.size()));
            REQUIRE(tmsu_url_decode(buffer, (tm_size_t)expected.size(), decode_buffer, buffer_size) ==
                    (tm_size_t)test.size());
            REQUIRE(test == std::string_view(decode_buffer, test.size()));
        }

        {
            std::string_view test = "-!\"_haushd.a#~-+/&\"§Ha.ha__";
            std::string_view expected = "-%21%22_haushd.a%23%7E-%2B%2F%26%22%C2%A7Ha.ha__";
            REQUIRE(tmsu_url_encode(test.data(), (tm_size_t)test.size(), buffer, buffer_size) ==
                    (tm_size_t)expected.size());
            REQUIRE(expected == std::string_view(buffer, expected.size()));
            REQUIRE(tmsu_url_decode(buffer, (tm_size_t)expected.size(), decode_buffer, buffer_size) ==
                    (tm_size_t)test.size());
            REQUIRE(test == std::string_view(decode_buffer, test.size()));
        }

        {
            std::string_view invalid = "%%";
            REQUIRE(tmsu_url_decode(invalid.data(), (tm_size_t)invalid.size(), decode_buffer, buffer_size) == 0);
        }
        {
            std::string_view invalid = "%1";
            REQUIRE(tmsu_url_decode(invalid.data(), (tm_size_t)invalid.size(), decode_buffer, buffer_size) == 0);
        }
        {
            std::string_view invalid = "%1P";
            REQUIRE(tmsu_url_decode(invalid.data(), (tm_size_t)invalid.size(), decode_buffer, buffer_size) == 0);
        }
        {
            std::string_view invalid = "%P";
            REQUIRE(tmsu_url_decode(invalid.data(), (tm_size_t)invalid.size(), decode_buffer, buffer_size) == 0);
        }
        {
            std::string_view invalid = "%";
            REQUIRE(tmsu_url_decode(invalid.data(), (tm_size_t)invalid.size(), decode_buffer, buffer_size) == 0);
        }
        {
            std::string_view valid = "";
            REQUIRE(tmsu_url_decode(valid.data(), (tm_size_t)valid.size(), decode_buffer, buffer_size) == 0);
        }
        {
            std::string_view valid = "%41";
            REQUIRE(tmsu_url_decode(valid.data(), (tm_size_t)valid.size(), decode_buffer, buffer_size) == 1);
            REQUIRE(std::string_view(decode_buffer, 1) == "A");
        }

        CHECK_ASSERTION_FAILURE(tmsu_url_decode(nullptr, 10, decode_buffer, buffer_size));
        CHECK_ASSERTION_FAILURE(tmsu_url_decode("a", 1, nullptr, buffer_size));
        CHECK_NOASSERT(tmsu_url_decode(nullptr, 0, nullptr, 0));
        REQUIRE(tmsu_url_decode(nullptr, 0, nullptr, 0) == 0);
    }
}

TEST_CASE("base64") {
    struct DataSet {
        std::string_view value;
        std::string_view base64;
        std::string_view base64url;
    };

    static const DataSet data[] = {
        {"", "", ""},
        {"Hello World", "SGVsbG8gV29ybGQ=", "SGVsbG8gV29ybGQ"},
        {"Some random test text", "U29tZSByYW5kb20gdGVzdCB0ZXh0", "U29tZSByYW5kb20gdGVzdCB0ZXh0"},
        {"\x08", "CA==", "CA"},
        {"\xFF", "/w==", "_w"},
        {std::string_view("\x00\x10", 2), "ABA=", "ABA"},
        {std::string_view("\xFF\x00\xF0\xFE\xFF\xFE\xA0\xCC", 8), "/wDw/v/+oMw=", "_wDw_v_-oMw"},
    };

    for (auto&& entry : data) {
        const tm_size_t buffer_size = 256;
        char encode_buffer[buffer_size];
        char decode_buffer[buffer_size];

        size_t encode_size =
            (size_t)tmsu_base64_encode(entry.value.data(), (tm_size_t)entry.value.size(), encode_buffer, buffer_size);
        REQUIRE(encode_size == entry.base64.size());
        REQUIRE(std::string_view(encode_buffer, encode_size) == entry.base64);

        size_t decode_size =
            (size_t)tmsu_base64_decode(encode_buffer, (tm_size_t)encode_size, decode_buffer, buffer_size);
        REQUIRE(decode_size == entry.value.size());
        REQUIRE(std::string_view(decode_buffer, decode_size) == entry.value);

        encode_size =
            (size_t)tmsu_base64url_encode(entry.value.data(), (tm_size_t)entry.value.size(), encode_buffer, buffer_size);
        REQUIRE(encode_size == entry.base64url.size());
        REQUIRE(std::string_view(encode_buffer, encode_size) == entry.base64url);

        decode_size = (size_t)tmsu_base64url_decode(encode_buffer, (tm_size_t)encode_size, decode_buffer, buffer_size);
        REQUIRE(decode_size == entry.value.size());
        REQUIRE(std::string_view(decode_buffer, decode_size) == entry.value);
    }

    REQUIRE(tmsu_base64_decode("a", 1, nullptr, 0) == 0);
    REQUIRE(tmsu_base64_decode("aa", 2, nullptr, 0) == 0);
    REQUIRE(tmsu_base64_decode("aaa", 3, nullptr, 0) == 0);
    REQUIRE(tmsu_base64_decode("aaaa", 4, nullptr, 0) == 3);
    REQUIRE(tmsu_base64_decode(nullptr, 0, nullptr, 0) == 0);

    REQUIRE(tmsu_base64url_decode("a", 1, nullptr, 0) == 0);
    REQUIRE(tmsu_base64url_decode("aa", 2, nullptr, 0) == 1);
    REQUIRE(tmsu_base64url_decode("aaa", 3, nullptr, 0) == 2);
    REQUIRE(tmsu_base64url_decode("aaaa", 4, nullptr, 0) == 3);
    REQUIRE(tmsu_base64url_decode(nullptr, 0, nullptr, 0) == 0);

    CHECK_ASSERTION_FAILURE(tmsu_base64_decode(nullptr, 2, nullptr, 0));
    CHECK_ASSERTION_FAILURE(tmsu_base64_decode("aa", 2, nullptr, 1));
    CHECK_ASSERTION_FAILURE(tmsu_base64url_decode(nullptr, 2, nullptr, 0));
    CHECK_ASSERTION_FAILURE(tmsu_base64url_decode("aa", 2, nullptr, 1));
}