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
#include <tm_print.h>

#include <string_view>

template <class... Args>
void check_output(const char* format, const char* expected_string, const Args&... args) {
    const tm_size_t buffer_size = 100;
    char buffer[buffer_size];

    auto result_size = snprint(buffer, buffer_size, format, args...);
    REQUIRE(result_size < buffer_size);
    buffer[result_size] = 0;
    auto printed = std::string_view(buffer);
    auto expected = std::string_view(expected_string);
    CHECK(printed == expected);
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
    check_output("{}", "{}");

    // More format specifiers than arguments.
    check_output("{}{}", "1{}", 1);
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

    CHECK(snprint(nullptr, 0, "{}", 1) == -1);
    CHECK(snprint(buffer, 0, "{}", 1) == -1);
    CHECK(snprint(buffer, 1, "{}", 10) == -1);

    CHECK_ASSERTION_FAILURE(snprint(nullptr, 1, "{}", 1));
#ifdef SIGNED_SIZE_T
    CHECK_ASSERTION_FAILURE(snprint(nullptr, -1, "{}", 1));
    CHECK_ASSERTION_FAILURE(snprint(buffer, -1, "{}", 1));
#endif
}

TEST_CASE("Indexed") {
    check_output("{} {}", "1 2", 1, 2);
    check_output("{0} {1}", "1 2", 1, 2);
    check_output("{1} {0}", "2 1", 1, 2);
}
