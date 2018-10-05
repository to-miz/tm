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

template <class T>
void check_output(const char* format, T value, const char* expected_string) {
    const tm_size_t buffer_size = 100;
    char buffer[buffer_size];

    auto result_size = snprint(buffer, buffer_size, format, value);
    REQUIRE(result_size < buffer_size);
    buffer[result_size] = 0;
    auto printed = std::string_view(buffer);
    auto expected = std::string_view(expected_string);
    CHECK(printed == expected);
}

TEST_CASE("Test basic printing") {
    check_output("{}", 1, "1");
    check_output("{}", 10, "10");
    check_output("{:x}", 15, "f");
    check_output("{:X}", 15, "F");
    check_output("{:o}", 8, "10");
}

TEST_CASE("Test integral type printing") {
    // signed
    check_output("{}", (char)-1, "-1");
    check_output("{}", (signed char)-1, "-1");
    check_output("{}", (short)-1, "-1");
    check_output("{}", (int)-1, "-1");
    check_output("{}", (long)-1, "-1");
    check_output("{}", (long long)-1, "-1");

    // unsigned
    check_output("{}", (unsigned char)1, "1");
    check_output("{}", (unsigned short)1, "1");
    check_output("{}", (unsigned int)1, "1");
    check_output("{}", (unsigned long)1, "1");
    check_output("{}", (unsigned long long)1, "1");

    // float
    check_output("{}", (float)1, "1.000000");
    check_output("{}", (double)1, "1.000000");
}

TEST_CASE("Test integer format specifiers") {
    check_output("{:+}", 1, "+1");
    check_output("{: }", 1, "1");
    check_output("{:0}", 1, "1");
    check_output("{:+}", 1.0, "+1.000000");

    check_output("{:c}", 'a', "a");

    check_output("{:x}", 15, "f");
    check_output("{:X}", 15, "F");
    check_output("{:b}", 1, "1");
    check_output("{:B}", 1, "1");
    check_output("{:o}", 8, "10");

    check_output("{:#x}", 15, "0xf");
    check_output("{:#X}", 15, "0XF");
    check_output("{:#b}", 1, "0b1");
    check_output("{:#B}", 1, "0B1");
    check_output("{:#o}", 8, "010");

    // combinations
    check_output("{:+#x}", 15, "+0xf");
    check_output("{:+#X}", 15, "+0XF");
    check_output("{:+#b}", 1, "+0b1");
    check_output("{:+#B}", 1, "+0B1");
    check_output("{:+#o}", 8, "+010");
}

TEST_CASE("Test float format specifiers") {
    check_output("{:f}", 1.0, "1.000000");
    check_output("{:g}", 1.0, "1");

// check scientific output
#if !defined(TMP_FLOAT_BACKEND_TM_CONVERSION) || (defined(TMC_VERSION)) && TMC_VERSION > 0x00090904u
    check_output("{:e}", 1.0, "1.000000e+00");
    check_output("{:E}", 1.0, "1.000000E+00");

    check_output("{:a}", 1.0, "1.000000p+0");
    check_output("{:A}", 1.0, "1.000000P+0");

    check_output("{:#a}", 1.0, "0x1.000000p+0");
    check_output("{:#A}", 1.0, "0X1.000000P+0");
#endif
}

TEST_CASE("Test width") {
    check_output("{:10}", 1, "         1");
    check_output("{:10}", -1, "        -1");
    check_output("{:+10}", 1, "        +1");

    check_output("{:-10}", 1, "1         ");
    check_output("{:-10}", -1, "-1        ");
    check_output("{:-+10}", 1, "+1        ");

    check_output("{:010}", 1, "0000000001");
    check_output("{:010}", -1, "-000000001");
    check_output("{:+010}", 1, "+000000001");

    check_output("{:-010}", 1, "1         ");
    check_output("{:-010}", -1, "-1        ");
    check_output("{:-+010}", 1, "+1        ");

    check_output("{:10}", 1.0, "  1.000000");
    check_output("{:10}", -1.0, " -1.000000");
    check_output("{:+10}", 1.0, " +1.000000");

    check_output("{:010}", 1.0, "001.000000");
    check_output("{:010}", -1.0, "-01.000000");
    check_output("{:+010}", 1.0, "+01.000000");

    check_output("{:-010}", 1.0, "1.000000  ");
    check_output("{:-010}", -1.0, "-1.000000 ");
    check_output("{:-+010}", 1.0, "+1.000000 ");
}

TEST_CASE("Test overflow") {
    char buffer[10];

    CHECK(snprint(nullptr, 0, "{}", 1) == 0);
    CHECK(snprint(buffer, 0, "{}", 1) == 0);
    CHECK(snprint(buffer, 1, "{}", 10) == 0);

    CHECK_ASSERTION_FAILURE(snprint(nullptr, 1, "{}", 1));
#ifdef SIGNED_SIZE_T
    CHECK_ASSERTION_FAILURE(snprint(nullptr, -1, "{}", 1));
    CHECK_ASSERTION_FAILURE(snprint(buffer, -1, "{}", 1));
#endif
}