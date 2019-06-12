// TODO: Test custom printing.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <assert_throws.h>
#include <doctest/doctest.h>
#include <assert_throws.cpp>

#ifdef SIGNED_SIZE_T
#define TM_SIZE_T_DEFINED
#define TM_SIZE_T_IS_SIGNED 1
typedef int tm_size_t;
#endif  // defined(SIGNED_SIZE_T)

#define TM_CONVERSION_IMPLEMENTATION
#define TM_PRINT_IMPLEMENTATION
#define TMP_CUSTOM_PRINTING
#define TMP_USE_STL
#include <tm_print.h>

#include <string_view>

template <class... Args>
void check_output(const char* format, const char* expected_string, const Args&... args) {
    const tm_size_t buffer_size = 100;
    char buffer[buffer_size];

    auto expected = std::string_view(expected_string);

    CAPTURE(format);
    CAPTURE(expected_string);

    // Check whether needed size calculations are correct.
    auto needed_size = tml::snprint(nullptr, 0, format, args...);
    REQUIRE(needed_size >= 0);
    REQUIRE((size_t)needed_size == expected.size());

    // Check whether printing to static buffers work.
    auto result_size = tml::snprint(buffer, buffer_size, format, args...);
    REQUIRE(result_size >= 0);
    REQUIRE((tm_size_t)result_size < buffer_size);
    buffer[result_size] = 0;
    auto printed = std::string_view(buffer);
    CHECK(printed == expected);

    // Check whether std::string printing works.
    auto str = tml::string_format(format, args...);
    CHECK(str == expected);
}

template <class... Args>
void expect_error(const char* format, const Args&... args) {
    const tm_size_t buffer_size = 100;
    char buffer[buffer_size];

    CAPTURE(format);

    CHECK(tml::snprint(buffer, buffer_size, format, args...) < 0);
}

TEST_CASE("Test basic printing") {
    check_output("{}", "1", 1);
    check_output("{}", "10", 10);
    check_output("{:x}", "f", 15);
    check_output("{:X}", "F", 15);
    check_output("{:o}", "10", 8);
}

TEST_CASE("Test arguments count") {
    // Zero arguments
    check_output("test", "test");
}

TEST_CASE("Test format validation") {
    // No arguments but placeholders.
    expect_error("{}");
    expect_error("{0}");

    // Specified index out of range.
    expect_error("{1}", 1);
    expect_error("{2}", 1, 2);
    expect_error("{100}", 1, 2);

    // More format specifiers than arguments.
    expect_error("{}{}", 1);
    expect_error("{0}{}{}", 1);
    expect_error("{}{}{0}", 1);
    expect_error("{}{0}{}", 1);
    expect_error("{0}{0}{}{}", 1);

    // Escaped placeholder tokens.
    auto check_escaped_token = [](const char* format, const char* expected) {
        // Test escaped tokens in various positions in a string.
        const char* changed[] = {"%s",       "%stest",   "test%s",   "test%stest",   "%s%s",
                                 "%stest%s", "test%s%s", "%s%stest", "test%s%stest", "test%stest%stest"};
        const tm_size_t buffer_size = 100;
        char format_buffer[buffer_size];
        char expected_buffer[buffer_size];

        for (auto entry : changed) {
            int new_format_len = snprintf(format_buffer, buffer_size, entry, format, format);
            int new_expected_len = snprintf(expected_buffer, buffer_size, entry, expected, expected);
            REQUIRE(new_format_len >= 0);
            REQUIRE(new_format_len < buffer_size);
            REQUIRE(new_expected_len >= 0);
            REQUIRE(new_expected_len < buffer_size);
            format_buffer[new_format_len] = 0;
            expected_buffer[new_expected_len] = 0;

            check_output(format_buffer, expected_buffer);
            check_output(format_buffer, expected_buffer, 1);
            check_output(format_buffer, expected_buffer, 1, 2);
            check_output(format_buffer, expected_buffer, "{");
            check_output(format_buffer, expected_buffer, "}");
            check_output(format_buffer, expected_buffer, "{", "}");
            check_output(format_buffer, expected_buffer, "}", "{");
        }
    };
    check_escaped_token("{{", "{");
    check_escaped_token("}}", "}");
    check_escaped_token("{{}}", "{}");
    check_escaped_token("}}{{", "}{");
    check_escaped_token("test}}test{{test", "test}test{test");

    // Unescaped placeholder tokens.
    auto expect_error_escaped_token = [](const char* format) {
        // Test escaped tokens in various positions in a string.
        const char* changed[] = {"%s", "%stest", "test%s", "test%stest", "%stest%s", "test%stest%stest"};
        const tm_size_t buffer_size = 100;
        char format_buffer[buffer_size];

        for (auto entry : changed) {
            int new_format_len = snprintf(format_buffer, buffer_size, entry, format, format);
            REQUIRE(new_format_len >= 0);
            REQUIRE(new_format_len < buffer_size);
            format_buffer[new_format_len] = 0;

            expect_error(format_buffer);
            expect_error(format_buffer, 1);
            expect_error(format_buffer, 1, 2);
            expect_error(format_buffer, "{");
            expect_error(format_buffer, "}");
            expect_error(format_buffer, "{", "}");
            expect_error(format_buffer, "}", "{");
        }
    };

    const char* error_format_strings[] = {
        "{", "{{{", "}", "}}}", "{{}", "{{asd}", "{}}", "{asd}}", "{0}}", "\\{", "\\{{{", "\\}", "\\}}}",
    };
    // Test format strings with 0 and some arguments.
    for (auto format_string : error_format_strings) {
        expect_error_escaped_token(format_string);
    }
}

TEST_CASE("Test integral type printing") {
    // signed
    check_output("{}", "-1", (char)-1);
    check_output("{}", "-1", (signed char)-1);
    check_output("{}", "-1", (short)-1);
    check_output("{}", "-1", (int)-1);
    check_output("{}", "-1", (long)-1);
    check_output("{}", "-1", (long long)-1);

    // unsigned
    check_output("{}", "1", (unsigned char)1);
    check_output("{}", "1", (unsigned short)1);
    check_output("{}", "1", (unsigned int)1);
    check_output("{}", "1", (unsigned long)1);
    check_output("{}", "1", (unsigned long long)1);

    // float
    check_output("{}", "1.000000", (float)1);
    check_output("{}", "1.000000", (double)1);
}

TEST_CASE("Test integer format specifiers") {
    check_output("{:+}", "+1", 1);
    check_output("{: }", "1", 1);
    check_output("{:0}", "1", 1);
    check_output("{:+}", "+1.000000", 1.0);

    check_output("{:c}", "a", 'a');

    check_output("{:x}", "f", 15);
    check_output("{:X}", "F", 15);
    check_output("{:b}", "1", 1);
    check_output("{:B}", "1", 1);
    check_output("{:o}", "10", 8);

    check_output("{:#x}", "0xf", 15);
    check_output("{:#X}", "0XF", 15);
    check_output("{:#b}", "0b1", 1);
    check_output("{:#B}", "0B1", 1);
    check_output("{:#o}", "010", 8);

    // combinations
    check_output("{:+#x}", "+0xf", 15);
    check_output("{:+#X}", "+0XF", 15);
    check_output("{:+#b}", "+0b1", 1);
    check_output("{:+#B}", "+0B1", 1);
    check_output("{:+#o}", "+010", 8);
}

TEST_CASE("Test float format specifiers") {
    check_output("{:f}", "1.000000", 1.0);
    check_output("{:g}", "1", 1.0);
}

#if defined(TMP_FLOAT_BACKEND_TM_CONVERSION) && !defined(TMC_HAS_SCIENTIFIC)
/* tm_conversion doesn't have support for scientific printing yet. */
const bool scientific_may_fail = true;
#else
const bool scientific_may_fail = false;
#endif
TEST_CASE("Test float scientific format specifiers" * doctest::may_fail(scientific_may_fail)) {
    check_output("{:e}", "1.000000e+00", 1.0);
    check_output("{:E}", "1.000000E+00", 1.0);

    check_output("{:a}", "1.000000p+0", 1.0);
    check_output("{:A}", "1.000000P+0", 1.0);

    check_output("{:#a}", "0x1.000000p+0", 1.0);
    check_output("{:#A}", "0X1.000000P+0", 1.0);
}

TEST_CASE("Test width") {
    check_output("{:10}", "         1", 1);
    check_output("{:10}", "        -1", -1);
    check_output("{:+10}", "        +1", 1);

    check_output("{:-10}", "1         ", 1);
    check_output("{:-10}", "-1        ", -1);
    check_output("{:-+10}", "+1        ", 1);

    check_output("{:010}", "0000000001", 1);
    check_output("{:010}", "-000000001", -1);
    check_output("{:+010}", "+000000001", 1);

    check_output("{:-010}", "1         ", 1);
    check_output("{:-010}", "-1        ", -1);
    check_output("{:-+010}", "+1        ", 1);

    check_output("{:10}", "  1.000000", 1.0);
    check_output("{:10}", " -1.000000", -1.0);
    check_output("{:+10}", " +1.000000", 1.0);

    check_output("{:010}", "001.000000", 1.0);
    check_output("{:010}", "-01.000000", -1.0);
    check_output("{:+010}", "+01.000000", 1.0);

    check_output("{:-010}", "1.000000  ", 1.0);
    check_output("{:-010}", "-1.000000 ", -1.0);
    check_output("{:-+010}", "+1.000000 ", 1.0);
}

TEST_CASE("Test overflow") {
    char buffer[10];

    CHECK(tml::snprint(nullptr, 0, "{}", 1) == 1);
    CHECK(tml::snprint(buffer, 0, "{}", 1) == 1);
    CHECK(tml::snprint(buffer, 1, "{}", 10) == 2);

    CHECK_ASSERTION_FAILURE(tml::snprint(nullptr, 1, "{}", 1));
#ifdef SIGNED_SIZE_T
    CHECK_ASSERTION_FAILURE(tml::snprint(nullptr, -1, "{}", 1));
    CHECK_ASSERTION_FAILURE(tml::snprint(buffer, -1, "{}", 1));
#endif
}

TEST_CASE("Test indexed") {
    check_output("{} {}", "1 2", 1, 2);
    check_output("{} {} {0}", "1 2 1", 1, 2);
    check_output("{} {} {0} {1} {0}", "1 2 1 2 1", 1, 2);

    check_output("{0} {1}", "1 2", 1, 2);
    check_output("{1} {0}", "2 1", 1, 2);

    check_output("{0} {0}", "1 1", 1, 2);
    check_output("{0} {}", "1 1", 1, 2);
    check_output("{1} {}", "2 1", 1, 2);
    check_output("{0} {} {}", "1 1 2", 1, 2);
    check_output("{1} {} {}", "2 1 2", 1, 2);
}

// TODO: What is the correct behavior when specifying shortest representation with a given precision?
#if 0
TEST_CASE("Test precision with shortest") {
    check_output("{:g}", "1", 1.0);
    check_output("{:g}", "1.11", 1.11);
    check_output("{:.2g}", "1.1", 1.11);
    check_output("{:.3g}", "1.11", 1.11);
    check_output("{:.4g}", "1.11", 1.11);
    check_output("{:.2g}", "1.1", 1.11);
    check_output("{:.2g}", "1.2", 1.19);
}
#endif