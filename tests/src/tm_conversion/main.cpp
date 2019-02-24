#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <exception>
#include <string>

#include <assert_throws.h>
#include <assert_throws.cpp>

#include <string_view>

#define TM_CONVERSION_IMPLEMENTATION
#define TMC_CHECKED_WIDTH
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(str) (str).data()
#define TM_STRING_VIEW_SIZE(str) ((tm_size_t)(str).size())
#define TM_STRING_VIEW_MAKE(data, size) std::string_view{(data), (size_t)(size)}
#define TMC_CPP_OVERLOADS
#include <tm_conversion.h>

// helpers

void compare_results(const char* dest, tmc_conv_result result, tm_errc expected_ec, const char* expected_string) {
    CHECK(result.ec == expected_ec);
    if (result.ec != expected_ec) {
        return;
    }
    if (result.ec == TM_OK) {
        std::string_view printed = {dest, result.size};
        std::string_view expected = {expected_string};
        CHECK(printed == expected);
    }
}

template <class T, class func>
void check_float_printing(func f, char* dest, tm_size_t maxlen, T value, uint32_t flags, int32_t precision,
                         tm_errc expected_ec, const char* expected_string) {
    auto result = f(dest, maxlen, value, flags, precision);
    compare_results(dest, result, expected_ec, expected_string);
}

template <class T, class func>
void check_int_printing(func f, char* dest, tm_size_t maxlen, T value, int32_t base, tm_bool lowercase,
                       tm_errc expected_ec, const char* expected_string) {
    auto result = f(dest, maxlen, value, base, lowercase);
    compare_results(dest, result, expected_ec, expected_string);
}

TEST_CASE("Test scan invariants") {
    CHECK(scan_i32(nullptr, nullptr, 10).ec == TM_EINVAL);
    CHECK(scan_u32(nullptr, nullptr, 10).ec == TM_EINVAL);
    CHECK(scan_i64(nullptr, nullptr, 10).ec == TM_EINVAL);
    CHECK(scan_u64(nullptr, nullptr, 10).ec == TM_EINVAL);
    CHECK(scan_float(nullptr, nullptr, 0).ec == TM_EINVAL);
    CHECK(scan_double(nullptr, nullptr, 0).ec == TM_EINVAL);
    CHECK(scan_bool(nullptr, nullptr).ec == TM_EINVAL);

    CHECK(scan_i32_n(nullptr, 0, nullptr, 10).ec == TM_EINVAL);
    CHECK(scan_u32_n(nullptr, 0, nullptr, 10).ec == TM_EINVAL);
    CHECK(scan_i64_n(nullptr, 0, nullptr, 10).ec == TM_EINVAL);
    CHECK(scan_u64_n(nullptr, 0, nullptr, 10).ec == TM_EINVAL);
    CHECK(scan_float_n(nullptr, 0, nullptr, 0).ec == TM_EINVAL);
    CHECK(scan_double_n(nullptr, 0, nullptr, 0).ec == TM_EINVAL);
    CHECK(scan_bool_n(nullptr, 0, nullptr).ec == TM_EINVAL);

    // Assertions should fail because str is nullptr while maxlen is 1
    CHECK_ASSERTION_FAILURE(scan_i32_n(nullptr, 1, nullptr, 10));
    CHECK_ASSERTION_FAILURE(scan_u32_n(nullptr, 1, nullptr, 10));
    CHECK_ASSERTION_FAILURE(scan_i64_n(nullptr, 1, nullptr, 10));
    CHECK_ASSERTION_FAILURE(scan_u64_n(nullptr, 1, nullptr, 10));
    CHECK_ASSERTION_FAILURE(scan_float_n(nullptr, 1, nullptr, 0));
    CHECK_ASSERTION_FAILURE(scan_double_n(nullptr, 1, nullptr, 0));
    CHECK_ASSERTION_FAILURE(scan_bool_n(nullptr, 1, nullptr));

    const char* num = "1";

    // Scanning should not try to write to a nullptr output parameter
    CHECK(scan_i32(num, nullptr, 10).ec == TM_OK);
    CHECK(scan_u32(num, nullptr, 10).ec == TM_OK);
    CHECK(scan_i64(num, nullptr, 10).ec == TM_OK);
    CHECK(scan_u64(num, nullptr, 10).ec == TM_OK);
    CHECK(scan_float(num, nullptr, 0).ec == TM_OK);
    CHECK(scan_double(num, nullptr, 0).ec == TM_OK);
    CHECK(scan_bool(num, nullptr).ec == TM_OK);

    CHECK(scan_i32_n(num, 1, nullptr, 10).ec == TM_OK);
    CHECK(scan_u32_n(num, 1, nullptr, 10).ec == TM_OK);
    CHECK(scan_i64_n(num, 1, nullptr, 10).ec == TM_OK);
    CHECK(scan_u64_n(num, 1, nullptr, 10).ec == TM_OK);
    CHECK(scan_float_n(num, 1, nullptr, 0).ec == TM_OK);
    CHECK(scan_double_n(num, 1, nullptr, 0).ec == TM_OK);
    CHECK(scan_bool_n(num, 1, nullptr).ec == TM_OK);
}

TEST_CASE("Test print invariants") {
    SUBCASE("2 <= base <= 36") {
        char buffer[10];
        CHECK_ASSERTION_FAILURE(print_i32(buffer, 10, 0, /*base=*/1, false));
        CHECK_ASSERTION_FAILURE(print_u32(buffer, 10, 0, /*base=*/1, false));
        CHECK_ASSERTION_FAILURE(print_i64(buffer, 10, 0, /*base=*/1, false));
        CHECK_ASSERTION_FAILURE(print_u64(buffer, 10, 0, /*base=*/1, false));

        CHECK_ASSERTION_FAILURE(print_i32(buffer, 10, 0, /*base=*/37, false));
        CHECK_ASSERTION_FAILURE(print_u32(buffer, 10, 0, /*base=*/37, false));
        CHECK_ASSERTION_FAILURE(print_i64(buffer, 10, 0, /*base=*/37, false));
        CHECK_ASSERTION_FAILURE(print_u64(buffer, 10, 0, /*base=*/37, false));

        for (int base = 2; base <= 36; ++base) {
            CHECK_NOASSERT(print_i32(buffer, 10, 0, /*base=*/base, false));
            CHECK_NOASSERT(print_u32(buffer, 10, 0, /*base=*/base, false));
            CHECK_NOASSERT(print_i64(buffer, 10, 0, /*base=*/base, false));
            CHECK_NOASSERT(print_u64(buffer, 10, 0, /*base=*/base, false));
        }
    }

    SUBCASE("dest must be not null if maxlen > 0") {
        CHECK_ASSERTION_FAILURE(print_i32(nullptr, 10, 0, 10, false));
        CHECK_ASSERTION_FAILURE(print_u32(nullptr, 10, 0, 10, false));
        CHECK_ASSERTION_FAILURE(print_i64(nullptr, 10, 0, 10, false));
        CHECK_ASSERTION_FAILURE(print_u64(nullptr, 10, 0, 10, false));
        CHECK_ASSERTION_FAILURE(print_double(nullptr, 10, 0, 0, 0));
        CHECK_ASSERTION_FAILURE(print_float(nullptr, 10, 0, 0, 0));
        CHECK_ASSERTION_FAILURE(print_bool(nullptr, 10, 0, 0));
        CHECK_ASSERTION_FAILURE(print_decimal_i32(nullptr, 10, 0));
        CHECK_ASSERTION_FAILURE(print_decimal_u32(nullptr, 10, 0));
        CHECK_ASSERTION_FAILURE(print_decimal_i64(nullptr, 10, 0));
        CHECK_ASSERTION_FAILURE(print_decimal_u64(nullptr, 10, 0));
        CHECK_ASSERTION_FAILURE(print_hex_i32(nullptr, 10, 0, false));
        CHECK_ASSERTION_FAILURE(print_hex_u32(nullptr, 10, 0, false));
        CHECK_ASSERTION_FAILURE(print_hex_i64(nullptr, 10, 0, false));
        CHECK_ASSERTION_FAILURE(print_hex_u64(nullptr, 10, 0, false));
        CHECK_ASSERTION_FAILURE(print_decimal_u32_w(nullptr, 10, 1, 0));
        CHECK_ASSERTION_FAILURE(print_decimal_u64_w(nullptr, 10, 1, 0));
        CHECK_ASSERTION_FAILURE(print_hex_u32_w(nullptr, 10, 1, 0, false));
        CHECK_ASSERTION_FAILURE(print_hex_u64_w(nullptr, 10, 1, 0, false));
    }

    SUBCASE("Width based prints should assert on width being valid") {
        char buffer[10];
        CHECK_ASSERTION_FAILURE(print_decimal_u32_w(buffer, 10, 0, 100));
        CHECK_ASSERTION_FAILURE(print_decimal_u64_w(buffer, 10, 0, 100));
        CHECK_ASSERTION_FAILURE(print_hex_u32_w(buffer, 10, 0, 100, false));
        CHECK_ASSERTION_FAILURE(print_hex_u64_w(buffer, 10, 0, 100, false));
        CHECK_ASSERTION_FAILURE(print_u32_w(buffer, 10, 0, 100, 8, false));
        CHECK_ASSERTION_FAILURE(print_u64_w(buffer, 10, 0, 100, 8, false));
    }

#ifdef TMC_CHECKED_WIDTH
    SUBCASE("Width based prints should check widths being valid on disabled asserts") {
        enable_asserts(false);
        char buffer[10];
        CHECK(print_decimal_u32_w(buffer, 10, 0, 100).ec == TM_EOVERFLOW);
        CHECK(print_decimal_u64_w(buffer, 10, 0, 100).ec == TM_EOVERFLOW);
        CHECK(print_hex_u32_w(buffer, 10, 0, 0x1ff, false).ec == TM_EOVERFLOW);
        CHECK(print_hex_u64_w(buffer, 10, 0, 0x1ff, false).ec == TM_EOVERFLOW);
        CHECK(print_u32_w(buffer, 10, 0, 0x1ff, 8, false).ec == TM_EOVERFLOW);
        CHECK(print_u64_w(buffer, 10, 0, 0x1ff, 8, false).ec == TM_EOVERFLOW);

        CHECK(print_decimal_u32_w(buffer, 10, 1, 100).ec == TM_EOVERFLOW);
        CHECK(print_decimal_u64_w(buffer, 10, 1, 100).ec == TM_EOVERFLOW);
        CHECK(print_hex_u32_w(buffer, 10, 1, 0x1ff, false).ec == TM_EOVERFLOW);
        CHECK(print_hex_u64_w(buffer, 10, 1, 0x1ff, false).ec == TM_EOVERFLOW);
        CHECK(print_u32_w(buffer, 10, 1, 0x1ff, 8, false).ec == TM_EOVERFLOW);
        CHECK(print_u64_w(buffer, 10, 1, 0x1ff, 8, false).ec == TM_EOVERFLOW);

        CHECK(print_decimal_u32_w(buffer, 10, 2, 100).ec == TM_EOVERFLOW);
        CHECK(print_decimal_u64_w(buffer, 10, 2, 100).ec == TM_EOVERFLOW);
        CHECK(print_hex_u32_w(buffer, 10, 2, 0x1ff, false).ec == TM_EOVERFLOW);
        CHECK(print_hex_u64_w(buffer, 10, 2, 0x1ff, false).ec == TM_EOVERFLOW);
        CHECK(print_u32_w(buffer, 10, 2, 0x1ff, 8, false).ec == TM_EOVERFLOW);
        CHECK(print_u64_w(buffer, 10, 2, 0x1ff, 8, false).ec == TM_EOVERFLOW);
        enable_asserts(true);
    }
#endif
    }

TEST_CASE("Simple float printing") {
    auto test = [](auto func, auto v) {
        typedef decltype(v) float_type;
        const int len = 5;
        char buffer[len];

        // positive value
        check_float_printing(func, buffer, len, (float_type)0.0, 0, 0, TM_OK, "0");
        check_float_printing(func, buffer, len, (float_type)1.0, 0, 0, TM_OK, "1");
        check_float_printing(func, buffer, len, (float_type)0.0, 0, -1, TM_OK, "0");
        check_float_printing(func, buffer, len, (float_type)1.0, 0, -1, TM_OK, "1");
        check_float_printing(func, buffer, len, (float_type)0.0, PF_TRAILING_ZEROES, -1, TM_OK, "0.0");
        check_float_printing(func, buffer, len, (float_type)1.0, PF_TRAILING_ZEROES, -1, TM_OK, "1.0");

        check_float_printing(func, buffer, len, (float_type)0.0, PF_TRAILING_ZEROES, 0, TM_OK, "0");
        check_float_printing(func, buffer, len, (float_type)1.0, PF_TRAILING_ZEROES, 0, TM_OK, "1");

        check_float_printing(func, buffer, len, (float_type)0.0, PF_TRAILING_ZEROES, 2, TM_OK, "0.00");
        check_float_printing(func, buffer, len, (float_type)1.0, PF_TRAILING_ZEROES, 2, TM_OK, "1.00");

        // negative value
        check_float_printing(func, buffer, len, (float_type)-0.0, 0, 0, TM_OK, "0");
        check_float_printing(func, buffer, len, (float_type)-0.0, 0, -1, TM_OK, "0");
        check_float_printing(func, buffer, len, (float_type)-1.0, 0, 0, TM_OK, "-1");
        check_float_printing(func, buffer, len, (float_type)-1.0, 0, -1, TM_OK, "-1");

        check_float_printing(func, buffer, len, (float_type)-0.0, PF_SIGNBIT, 0, TM_OK, "-0");
        check_float_printing(func, buffer, len, (float_type)-0.0, PF_SIGNBIT, -1, TM_OK, "-0");

        check_float_printing(func, buffer, len, (float_type)-0.0, PF_TRAILING_ZEROES, -1, TM_OK, "0.0");
        check_float_printing(func, buffer, len, (float_type)-1.0, PF_TRAILING_ZEROES, -1, TM_OK, "-1.0");
        check_float_printing(func, buffer, len, (float_type)-0.0, PF_TRAILING_ZEROES, 0, TM_OK, "0");
        check_float_printing(func, buffer, len, (float_type)-1.0, PF_TRAILING_ZEROES, 0, TM_OK, "-1");

        check_float_printing(func, buffer, len, (float_type)-0.0, PF_TRAILING_ZEROES | PF_SIGNBIT, -1, TM_OK, "-0.0");
        check_float_printing(func, buffer, len, (float_type)-0.0, PF_TRAILING_ZEROES | PF_SIGNBIT, 0, TM_OK, "-0");

        check_float_printing(func, buffer, len, (float_type)-0.0, PF_TRAILING_ZEROES, 2, TM_OK, "0.00");
        check_float_printing(func, buffer, len, (float_type)-1.0, PF_TRAILING_ZEROES, 2, TM_OK, "-1.00");

        check_float_printing(func, buffer, len, (float_type)-0.0, PF_TRAILING_ZEROES | PF_SIGNBIT, 2, TM_OK, "-0.00");

        // inf and nan
        check_float_printing(func, buffer, len, INFINITY, 0, -1, TM_OK, "INF");
        check_float_printing(func, buffer, len, -INFINITY, 0, -1, TM_OK, "-INF");
        check_float_printing(func, buffer, len, NAN, 0, -1, TM_OK, "NAN");
        check_float_printing(func, buffer, len, -NAN, 0, -1, TM_OK, "NAN");

        check_float_printing(func, buffer, len, INFINITY, PF_LOWERCASE, -1, TM_OK, "inf");
        check_float_printing(func, buffer, len, -INFINITY, PF_LOWERCASE, -1, TM_OK, "-inf");
        check_float_printing(func, buffer, len, NAN, PF_LOWERCASE, -1, TM_OK, "nan");
        check_float_printing(func, buffer, len, -NAN, PF_LOWERCASE, -1, TM_OK, "nan");
    };

    test(print_float, 1.0f);
    test(print_double, 1.0);
}

TEST_CASE("Test print overflow") {
    SUBCASE("negative sign") {
        char buffer[1];
        CHECK(print_i32(buffer, 1, -1, 10, false).ec == TM_EOVERFLOW);
        CHECK(print_i64(buffer, 1, -1, 10, false).ec == TM_EOVERFLOW);
        CHECK(print_float(buffer, 1, -1, 0, 0).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 1, -1, 0, 0).ec == TM_EOVERFLOW);
    }

    SUBCASE("float after dot") {
        char buffer[5];
        CHECK(print_float(buffer, 2, 1, PF_TRAILING_ZEROES, -1).ec == TM_EOVERFLOW);
        CHECK(print_float(buffer, 2, 1.1f, 0, -1).ec == TM_EOVERFLOW);
        CHECK(print_float(buffer, 3, 1, PF_TRAILING_ZEROES, 4).ec == TM_EOVERFLOW);
        CHECK(print_float(buffer, 3, 1.11f, 0, 4).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 2, 1, PF_TRAILING_ZEROES, -1).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 2, 1.1, 0, -1).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 3, 1, PF_TRAILING_ZEROES, 4).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 3, 1.11, 0, 4).ec == TM_EOVERFLOW);
        CHECK(print_float(buffer, 2, 0, PF_TRAILING_ZEROES, -1).ec == TM_EOVERFLOW);
        CHECK(print_float(buffer, 3, 0, PF_TRAILING_ZEROES, 4).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 2, 0, PF_TRAILING_ZEROES, -1).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 3, 0, PF_TRAILING_ZEROES, 4).ec == TM_EOVERFLOW);
    }

    SUBCASE("inf and nan") {
        char buffer[5];
        CHECK(print_float(buffer, 2, INFINITY, 0, -1).ec == TM_EOVERFLOW);
        CHECK(print_float(buffer, 2, -INFINITY, 0, -1).ec == TM_EOVERFLOW);
        CHECK(print_float(buffer, 2, NAN, 0, -1).ec == TM_EOVERFLOW);

        CHECK(print_double(buffer, 2, INFINITY, 0, -1).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 2, -INFINITY, 0, -1).ec == TM_EOVERFLOW);
        CHECK(print_double(buffer, 2, NAN, 0, -1).ec == TM_EOVERFLOW);
    }

    SUBCASE("middle") {
        char buffer[5];
        int32_t i32_d = 1;
        uint32_t u32_d = 1;
        int64_t i64_d = 1;
        uint64_t u64_d = 1;
        int32_t i32_h = 1;
        uint32_t u32_h = 1;
        int64_t i64_h = 1;
        uint64_t u64_h = 1;
        int32_t i32_b = 1;
        uint32_t u32_b = 1;
        int64_t i64_b = 1;
        uint64_t u64_b = 1;
        for (int i = 1; i <= 8; i++) {
            INFO("i is: " << i << "; i32_d is: " << i32_d);
            REQUIRE(i == get_digits_count_decimal_u32(u32_d));
            REQUIRE(i == get_digits_count_hex_u32(u32_h));

            tm_errc expected = (i <= 5) ? TM_OK : TM_EOVERFLOW;
            tm_errc expected_neg = (i <= 4) ? TM_OK : TM_EOVERFLOW;

            CHECK(print_i32(buffer, 5, i32_d, 10, false).ec == expected);
            CHECK(print_i32(buffer, 5, i32_h, 16, false).ec == expected);
            CHECK(print_i32(buffer, 5, i32_b, 2, false).ec == expected);

            CHECK(print_i32(buffer, 5, -i32_d, 10, false).ec == expected_neg);
            CHECK(print_i32(buffer, 5, -i32_h, 16, false).ec == expected_neg);
            CHECK(print_i32(buffer, 5, -i32_b, 2, false).ec == expected_neg);

            CHECK(print_u32(buffer, 5, u32_d, 10, false).ec == expected);
            CHECK(print_u32(buffer, 5, u32_h, 16, false).ec == expected);
            CHECK(print_u32(buffer, 5, u32_b, 2, false).ec == expected);

            CHECK(print_i64(buffer, 5, i64_d, 10, false).ec == expected);
            CHECK(print_i64(buffer, 5, i64_h, 16, false).ec == expected);
            CHECK(print_i64(buffer, 5, i64_b, 2, false).ec == expected);

            CHECK(print_i64(buffer, 5, -i64_d, 10, false).ec == expected_neg);
            CHECK(print_i64(buffer, 5, -i64_h, 16, false).ec == expected_neg);
            CHECK(print_i64(buffer, 5, -i64_b, 2, false).ec == expected_neg);

            CHECK(print_u64(buffer, 5, u64_d, 10, false).ec == expected);
            CHECK(print_u64(buffer, 5, u64_h, 16, false).ec == expected);
            CHECK(print_u64(buffer, 5, u64_b, 2, false).ec == expected);

            tm_errc expected_f = (i <= 3) ? TM_OK : TM_EOVERFLOW;
            tm_errc expected_f_neg = (i <= 2) ? TM_OK : TM_EOVERFLOW;

            CHECK(print_float(buffer, 5, (float)i32_d, 0, 0).ec == expected);
            CHECK(print_float(buffer, 5, (float)-i32_d, 0, 0).ec == expected_neg);
            CHECK(print_float(buffer, 5, (float)i32_d, PF_TRAILING_ZEROES, -1).ec == expected_f);
            CHECK(print_float(buffer, 5, (float)-i32_d, PF_TRAILING_ZEROES, -1).ec == expected_f_neg);

            CHECK(print_double(buffer, 5, (double)i32_d, 0, 0).ec == expected);
            CHECK(print_double(buffer, 5, (double)-i32_d, 0, 0).ec == expected_neg);
            CHECK(print_double(buffer, 5, (double)i32_d, PF_TRAILING_ZEROES, -1).ec == expected_f);
            CHECK(print_double(buffer, 5, (double)-i32_d, PF_TRAILING_ZEROES, -1).ec == expected_f_neg);

            i32_d *= 10;
            u32_d *= 10;
            i64_d *= 10;
            u64_d *= 10;
            i32_h *= 16;
            u32_h *= 16;
            i64_h *= 16;
            u64_h *= 16;
            i32_b *= 2;
            u32_b *= 2;
            i64_b *= 2;
            u64_b *= 2;
        }
    }
}

TEST_CASE("Test digits count") {
    SUBCASE("decimal") {
        SUBCASE("u32") {
            CHECK(get_digits_count_decimal_u32(0) == 1);
            CHECK(get_digits_count_decimal_u32(UINT32_MAX) == 10);

            uint32_t prev = 1;
            uint32_t val = 1;
            for (uint32_t i = 1; i <= 10; i++) {
                REQUIRE(prev <= val);
                CHECK(get_digits_count_decimal_u32(val) == i);
                val *= 10;
            }
        }
        SUBCASE("u64") {
            CHECK(get_digits_count_decimal_u64(0) == 1);
            CHECK(get_digits_count_decimal_u64(UINT64_MAX) == 20);

            uint64_t prev = 1;
            uint64_t val = 1;
            for (uint32_t i = 1; i <= 20; i++) {
                REQUIRE(prev <= val);
                CHECK(get_digits_count_decimal_u64(val) == i);
                val *= 10;
            }
        }
    }
    SUBCASE("hex") {
        SUBCASE("u32") {
            CHECK(get_digits_count_hex_u32(0) == 1);
            CHECK(get_digits_count_hex_u32(UINT32_MAX) == 8);

            uint32_t prev = 1;
            uint32_t val = 1;
            for (uint32_t i = 1; i <= 8; i++) {
                REQUIRE(prev <= val);
                CHECK(get_digits_count_hex_u32(val) == i);
                val *= 16;
            }
        }
        SUBCASE("u64") {
            CHECK(get_digits_count_hex_u64(0) == 1);
            CHECK(get_digits_count_hex_u64(UINT64_MAX) == 16);

            uint64_t prev = 1;
            uint64_t val = 1;
            for (uint32_t i = 1; i <= 16; i++) {
                REQUIRE(prev <= val);
                CHECK(get_digits_count_hex_u64(val) == i);
                val *= 16;
            }
        }
    }
    SUBCASE("invariants") {
        CHECK_ASSERTION_FAILURE(get_digits_count_u32(0, 0));
        CHECK_ASSERTION_FAILURE(get_digits_count_u64(0, 0));
        CHECK_ASSERTION_FAILURE(get_digits_count_u32(0, 37));
        CHECK_ASSERTION_FAILURE(get_digits_count_u64(0, 37));

        CHECK_NOASSERT(get_digits_count_u32(0, 2));
        CHECK_NOASSERT(get_digits_count_u32(0, 36));
        CHECK_NOASSERT(get_digits_count_u32(0, 10));
        CHECK_NOASSERT(get_digits_count_u64(0, 2));
        CHECK_NOASSERT(get_digits_count_u64(0, 36));
        CHECK_NOASSERT(get_digits_count_u64(0, 10));
    }
    SUBCASE("generic") {
        auto test = [](auto func, const auto start) {
            // test for all possible bases that get_digits gives right result
            for(int base = 2; base <= 36; ++base) {
                REQUIRE(func(0, base) == 1);

                auto value = start;
                for(auto i = 1;; ++i) {
                    REQUIRE(func(value, base) == i);
                    const auto next = value * base;
                    if(next / base != value) {
                        // multiplication overflow
                        break;
                    }
                    value = next;
                }
            }
        };

        test(get_digits_count_u32, (uint32_t)1);
        test(get_digits_count_u64, (uint64_t)1);
    }
}

TEST_CASE("Test integer printing") {
    SUBCASE("Generic") {
        auto positive = [](auto func) {
            char buffer[16];
            check_int_printing(func, buffer, 16, 0, 10, false, TM_OK, "0");
            check_int_printing(func, buffer, 16, 1, 10, false, TM_OK, "1");
            check_int_printing(func, buffer, 16, 32767, 10, false, TM_OK, "32767");
            check_int_printing(func, buffer, 16, 65535, 10, false, TM_OK, "65535");
            check_int_printing(func, buffer, 16, 2147483647, 10, false, TM_OK, "2147483647");

            check_int_printing(func, buffer, 16, 0, 16, false, TM_OK, "0");
            check_int_printing(func, buffer, 16, 1, 16, false, TM_OK, "1");
            check_int_printing(func, buffer, 16, 32767, 16, false, TM_OK, "7FFF");
            check_int_printing(func, buffer, 16, 65535, 16, false, TM_OK, "FFFF");
            check_int_printing(func, buffer, 16, 2147483647, 16, false, TM_OK, "7FFFFFFF");
            check_int_printing(func, buffer, 16, 2147483647, 16, true, TM_OK, "7fffffff");

            check_int_printing(func, buffer, 16, 3829, 2, false, TM_OK, "111011110101");
            check_int_printing(func, buffer, 16, 3829, 36, false, TM_OK, "2YD");
            check_int_printing(func, buffer, 16, 2147483647, 36, true, TM_OK, "zik0zj");
        };

        auto negative = [](auto func) {
            char buffer[16];
            check_int_printing(func, buffer, 16, -1, 10, false, TM_OK, "-1");
            check_int_printing(func, buffer, 16, -32767, 10, false, TM_OK, "-32767");
            check_int_printing(func, buffer, 16, -65535, 10, false, TM_OK, "-65535");
            check_int_printing(func, buffer, 16, -2147483647, 10, false, TM_OK, "-2147483647");
            check_int_printing(func, buffer, 16, -2147483647 - 1, 10, false, TM_OK, "-2147483648");

            check_int_printing(func, buffer, 16, -1, 16, false, TM_OK, "-1");
            check_int_printing(func, buffer, 16, -32767, 16, false, TM_OK, "-7FFF");
            check_int_printing(func, buffer, 16, -65535, 16, false, TM_OK, "-FFFF");
            check_int_printing(func, buffer, 16, -2147483647, 16, false, TM_OK, "-7FFFFFFF");
            check_int_printing(func, buffer, 16, -2147483647 - 1, 16, false, TM_OK, "-80000000");
            check_int_printing(func, buffer, 16, -2147483647, 16, true, TM_OK, "-7fffffff");

            check_int_printing(func, buffer, 16, -3829, 2, false, TM_OK, "-111011110101");
            check_int_printing(func, buffer, 16, -3829, 36, false, TM_OK, "-2YD");
            check_int_printing(func, buffer, 16, -2147483647, 36, true, TM_OK, "-zik0zj");
        };

        auto unsigned_int = [](auto func) {
            char buffer[16];
            check_int_printing(func, buffer, 16, 0xdeadbeef, 16, true, TM_OK, "deadbeef");
            check_int_printing(func, buffer, 16, 0xdeadbeef, 36, true, TM_OK, "1ps9wxb");
        };

        positive(print_i32);
        positive(print_u32);
        positive(print_i64);
        positive(print_u64);

        char buffer[16];
        check_int_printing(print_i32, buffer, 16, 0xdeadbeef, 16, true, TM_OK, "-21524111");
        check_int_printing(print_i32, buffer, 16, 0xdeadbeef, 36, true, TM_OK, "-98u51t");

        negative(print_i32);
        negative(print_i64);

        unsigned_int(print_u32);
        unsigned_int(print_i64);
        unsigned_int(print_u64);
    }
    SUBCASE("limits") {
        char buffer[64];

        check_int_printing(print_i32, buffer, 64, INT32_MIN, 8, false, TM_OK, "-20000000000");
        check_int_printing(print_i32, buffer, 64, INT32_MIN, 10, false, TM_OK, "-2147483648");
        check_int_printing(print_i32, buffer, 64, INT32_MIN, 16, false, TM_OK, "-80000000");

        check_int_printing(print_i32, buffer, 64, INT32_MAX, 8, false, TM_OK, "17777777777");
        check_int_printing(print_i32, buffer, 64, INT32_MAX, 10, false, TM_OK, "2147483647");
        check_int_printing(print_i32, buffer, 64, INT32_MAX, 16, false, TM_OK, "7FFFFFFF");

        check_int_printing(print_u32, buffer, 64, UINT32_MAX, 8, false, TM_OK, "37777777777");
        check_int_printing(print_u32, buffer, 64, UINT32_MAX, 10, false, TM_OK, "4294967295");
        check_int_printing(print_u32, buffer, 64, UINT32_MAX, 16, false, TM_OK, "FFFFFFFF");

        check_int_printing(print_i64, buffer, 64, INT64_MIN, 8, false, TM_OK, "-1000000000000000000000");
        check_int_printing(print_i64, buffer, 64, INT64_MIN, 10, false, TM_OK, "-9223372036854775808");
        check_int_printing(print_i64, buffer, 64, INT64_MIN, 16, false, TM_OK, "-8000000000000000");

        check_int_printing(print_i64, buffer, 64, INT64_MAX, 8, false, TM_OK, "777777777777777777777");
        check_int_printing(print_i64, buffer, 64, INT64_MAX, 10, false, TM_OK, "9223372036854775807");
        check_int_printing(print_i64, buffer, 64, INT64_MAX, 16, false, TM_OK, "7FFFFFFFFFFFFFFF");

        check_int_printing(print_u64, buffer, 64, UINT64_MAX, 8, false, TM_OK, "1777777777777777777777");
        check_int_printing(print_u64, buffer, 64, UINT64_MAX, 10, false, TM_OK, "18446744073709551615");
        check_int_printing(print_u64, buffer, 64, UINT64_MAX, 16, false, TM_OK, "FFFFFFFFFFFFFFFF");
    }
}

static float lerp(float t, float a, float b) { return a + t * (b - a); }
static double lerp(double t, double a, double b) { return a + t * (b - a); }

template <class UnsignedType, class PrintFunc, class ScanFunc, class ValueType>
bool test_roundtrip(PrintFunc print_func, ScanFunc scan_func, ValueType min_val, ValueType max_val) {
    int mismatchCount = 0;
    int closeMismatchCount = 0;
    uint64_t totalTests = 0;

    auto print_test = [&](ValueType min, ValueType max, int steps) {
        const int precision = 14;
        const unsigned flags = PF_TRAILING_ZEROES;

        for (auto i = 0; i < steps; ++i) {
            ValueType t = i / (ValueType)(steps - 1);
            ValueType value = lerp(t, min, max);

            char buffer[10000];
            auto print_result = print_func(buffer, 10000, value, flags, precision);
            REQUIRE(print_result.ec == TM_OK);

            ValueType scanned = 0;
            auto scan_result = scan_func(buffer, print_result.size, &scanned, 0);
            REQUIRE(scan_result.ec == TM_OK);

            ++totalTests;

            if (scanned != value) {
                UnsignedType uval = 0;
                UnsignedType uscan = 0;
                memcpy(&uval, &value, sizeof(uval));
                memcpy(&uscan, &scanned, sizeof(uscan));
                auto smallest = (uval < uscan) ? (uval) : (uscan);
                auto biggest = (uval < uscan) ? (uscan) : (uval);
                auto missed = biggest - smallest;
                if (missed != 0) {
                    // printf("mismatch: %.14f %.14f %d\n", value, scanned, (int)missed);
                    if (missed == 1) {
                        ++closeMismatchCount;
                    }
                }
                ++mismatchCount;
            }
        }
    };

#if 1
    const int steps = 100000;
#else
    const int steps = 10000000;
#endif
    print_test(0, 1, steps);
    print_test(0, 100, steps);
    print_test(0, 1000, steps);
    print_test(0, 10000, steps);
    print_test(min_val, max_val, steps);
    printf("mismatches: close: %d; bad: %d; total tests: %zu\n", closeMismatchCount, mismatchCount - closeMismatchCount,
           totalTests);

    return mismatchCount - closeMismatchCount == 0;
}

TEST_CASE("Test float roundtrip") {
    // CHECK(test_roundtrip<uint32_t>(print_float, scan_float_n, -HUGE_VALF, HUGE_VALF));
    CHECK(test_roundtrip<uint32_t>(print_float, scan_float_n, -1000000000.0f, 1000000000.0f));
    // We currently expect doubles to not survive a roundtrip. The conversion routines are wildy inaccurate for doubles.
    CHECK(test_roundtrip<uint64_t>(print_double, scan_double_n, -1000000000.0, 100000000000.0) == false);
}

TEST_CASE("Test float accuracy") {
    auto print_test = [](float min, float max, int steps) {
        bool success = true;
        int32_t precision = 12;
        uint32_t flags = PF_TRAILING_ZEROES;
        for (auto i = 0; i < steps; ++i) {
            float t = i / (float)(steps - 1);
            float value = lerp(t, min, max);

            char tm_buffer[1000];
            auto tm_result = print_float(tm_buffer, 1000, value, flags, precision);

            // TODO: use a more accurate conversion method than snprintf
            char snprintf_buffer[1000];
            auto snprintf_len = snprintf(snprintf_buffer, 1000, "%.*f", precision, value);

            char snprintf_buffer_r[1000];
            auto snprintf_r_len = snprintf(snprintf_buffer_r, 1000, "%.*f", precision + 2, value);

            if (tm_result.ec != TM_OK || tm_result.size != (tm_size_t)snprintf_len ||
                memcmp(tm_buffer, snprintf_buffer, tm_result.size) != 0) {
                // check whether snprintf actually does any rounding
                bool rounding_error = true;
                // check if snprintf would have printed a 5 if precision was higher
                if (snprintf_buffer_r[snprintf_r_len - 2] >= '5') {
                    if (tm_result.size == (tm_size_t)snprintf_len) {
                        int diff = tm_buffer[tm_result.size - 1] - snprintf_buffer[snprintf_len - 1];
                        if (diff == 1) {
                            // we rounded up, but snprintf didn't, even though it could have
                            rounding_error = false;
                        }
                    }
                }

                if (rounding_error) {
                    printf("%.*s %.*s\n", (int)tm_result.size, tm_buffer, (int)snprintf_r_len, snprintf_buffer_r);
                    success = false;
                }
            }
        }
        return success;
    };
    CHECK(print_test(-10000, 10000, 10000000));
    CHECK(print_test(0, 100, 10000000));
    CHECK(print_test(0, 1, 10000000));
}

TEST_CASE("Test float corner cases" * doctest::may_fail(true)) {
    float value = 0;
    auto result = scan_float("0.0000000000000000000000000000001", &value, 0);
    CHECK(result.ec == TM_OK);
    CHECK(value != 0);
}