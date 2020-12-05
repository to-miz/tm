#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <iterator>

#include <assert_throws.h>
#include <assert_throws.cpp>

#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <unordered_map>

#include <string_view>
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(str) (str).data()
#define TM_STRING_VIEW_SIZE(str) ((tm_size_t)(str).size())
#define TM_STRING_VIEW_MAKE(data, size) \
    std::string_view { (data), (size_t)(size) }

// clang-format off
#ifdef USE_SIGNED_SIZE_T
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 1
    typedef int tm_size_t;
#endif

#define TMU_USE_CRT
#ifdef USE_WINDOWS_H
    #define TMU_USE_WINDOWS_H
#endif
#ifdef USE_MSVC_CRT
    #define TMU_TESTING_MSVC_CRT
#elif !defined(USE_WINDOWS_H)
    #define TMU_TESTING_UNIX
#endif
// clang-format on
#define TMU_USE_CONSOLE

// Simple read file implementation instead of using the one in the library itself.
// We can't use the implementation in the library, since we are mocking underlying CRT/system calls.
std::vector<char> read_whole_file(const char* filename) {
    std::vector<char> v;
    if (FILE* f = fopen(filename, "rb")) {
        char buf[1024];
        while (size_t len = fread(buf, 1, sizeof(buf), f)) v.insert(v.end(), buf, buf + len);
        REQUIRE(ferror(f) == 0);
        fclose(f);
    }
    return v;
}

#include "../redirected_malloc.cpp"

#define TM_UNICODE_IMPLEMENTATION
#define TMU_SBO_SIZE 15  // Make allocations more common.
#define TMU_TESTING      // tm_unicode will not be including any headers.

#ifndef TMU_NO_FILE_IO
#include "file_io_mockups.cpp"
#endif

// Use Unicode data that defines even more data.
#ifdef USE_TESTS_UCD
#define TMU_NO_UCD
#define TMU_UCD_DEF extern
#include "generated/tests_unicode_data.h"
#include "generated/tests_unicode_data.c"
#endif /* defined(USE_TESTS_UCD) */

#define TMU_USE_STL

#define TM_USE_RESOURCE_PTR
#include <tm_unicode.h>
#include <tm_resource_ptr.h>

#if defined(USE_WINDOWS_H) && !defined(TMU_NO_FILE_IO)
#include "windows_mockups.cpp"
#endif

using namespace std;

/* Test strings in raw bytes, spells out byte order mark + "test_string-ﬆﬃÅỞᵫɶᨠ-end" */
const uint8_t utf8_bom[] = {0xEF, 0xBB, 0xBF, 0x74, 0x65, 0x73, 0x74, 0x5F, 0x73, 0x74, 0x72, 0x69, 0x6E,
                            0x67, 0x2D, 0xEF, 0xAC, 0x86, 0xEF, 0xAC, 0x83, 0xE2, 0x84, 0xAB, 0xE1, 0xBB,
                            0x9E, 0xE1, 0xB5, 0xAB, 0xC9, 0xB6, 0xE1, 0xA8, 0xA0, 0x2D, 0x65, 0x6E, 0x64};
const uint8_t utf16le_bom[] = {0xFF, 0xFE, 0x74, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x5F, 0x00,
                               0x73, 0x00, 0x74, 0x00, 0x72, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00,
                               0x2D, 0x00, 0x06, 0xFB, 0x03, 0xFB, 0x2B, 0x21, 0xDE, 0x1E, 0x6B, 0x1D,
                               0x76, 0x02, 0x20, 0x1A, 0x2D, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x64, 0x00};
const uint8_t utf16be_bom[] = {0xFE, 0xFF, 0x00, 0x74, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x5F,
                               0x00, 0x73, 0x00, 0x74, 0x00, 0x72, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67,
                               0x00, 0x2D, 0xFB, 0x06, 0xFB, 0x03, 0x21, 0x2B, 0x1E, 0xDE, 0x1D, 0x6B,
                               0x02, 0x76, 0x1A, 0x20, 0x00, 0x2D, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x64};

TEST_CASE("convert raw bytes") {
    allocation_guard alloc_guard;

    auto do_test = [](const void* in, size_t in_len, tmu_encoding encoding) {
        const tm_size_t buffer_len = 1024;
        char buffer[1024];

        auto overflow = tmu_utf8_convert_from_bytes(in, (tm_size_t)in_len, encoding, tmu_validate_error,
                                                    /*replace_str=*/nullptr, /*replace_str_len=*/0,
                                                    /*nullterminate=*/false, /*out=*/nullptr, /*out_len=*/0);
        REQUIRE(overflow.ec == TM_ERANGE);

        auto success = tmu_utf8_convert_from_bytes(in, (tm_size_t)in_len, encoding, tmu_validate_error,
                                                   /*replace_str=*/nullptr, /*replace_str_len=*/0,
                                                   /*nullterminate=*/false, buffer, buffer_len);
        REQUIRE(success.ec == TM_OK);
        REQUIRE(success.size == sizeof(utf8_bom) - 3);
    };

    do_test(utf8_bom, size(utf8_bom), tmu_encoding_unknown);
    do_test(utf8_bom, size(utf8_bom), tmu_encoding_utf8_bom);

    do_test(utf8_bom + 3, size(utf8_bom) - 3, tmu_encoding_unknown);
    do_test(utf8_bom + 3, size(utf8_bom) - 3, tmu_encoding_utf8);

    do_test(utf16le_bom, size(utf16le_bom), tmu_encoding_unknown);
    do_test(utf16le_bom, size(utf16le_bom), tmu_encoding_utf16le_bom);

    // Can't detect utf16 without bom.
    // do_test(utf16le_bom + 2, size(utf16le_bom) - 2, tmu_encoding_unknown);
    do_test(utf16le_bom + 2, size(utf16le_bom) - 2, tmu_encoding_utf16le);

    do_test(utf16be_bom, size(utf16be_bom), tmu_encoding_unknown);
    do_test(utf16be_bom, size(utf16be_bom), tmu_encoding_utf16be_bom);

    // do_test(utf16be_bom + 2, size(utf16be_bom) - 2, tmu_encoding_unknown);
    do_test(utf16be_bom + 2, size(utf16be_bom) - 2, tmu_encoding_utf16be);
}

TEST_CASE("nulltermination") {
    {
        const char* empty = "";
        char buffer[1] = {1};

        auto required_size =
            tmu_utf8_convert_from_bytes(empty, 0, tmu_encoding_utf8, tmu_validate_error, /*replace_str=*/nullptr,
                                        /*replace_str_len=*/0, /*nullterminate=*/true, /*out=*/nullptr, /*out_len=*/0);
        REQUIRE(required_size.ec == TM_ERANGE);
        REQUIRE(required_size.size == 1);

        auto result =
            tmu_utf8_convert_from_bytes(empty, 0, tmu_encoding_utf8, tmu_validate_error, /*replace_str=*/nullptr,
                                        /*replace_str_len=*/0, /*nullterminate=*/true, buffer, 1);
        REQUIRE(result.ec == TM_OK);
        REQUIRE(result.size == 0);
        REQUIRE(buffer[0] == 0);
    }

    {
        const tmu_char16 empty = 0;
        auto utf16_stream = tmu_utf16_make_stream_n(&empty, 0);
        char buffer[1] = {1};

        auto required_size =
            tmu_utf8_from_utf16_ex(utf16_stream, tmu_validate_error, /*replace_str=*/nullptr,
                                   /*replace_str_len=*/0, /*nullterminate=*/true, /*out=*/nullptr, /*out_len=*/0);
        REQUIRE(required_size.ec == TM_ERANGE);
        REQUIRE(required_size.size == 1);

        auto result = tmu_utf8_from_utf16_ex(utf16_stream, tmu_validate_error, /*replace_str=*/nullptr,
                                             /*replace_str_len=*/0, /*nullterminate=*/true, buffer, 1);
        REQUIRE(result.ec == TM_OK);
        REQUIRE(result.size == 0);
        REQUIRE(buffer[0] == 0);
    }
}

TEST_CASE("invalid unicode") {
    auto test_data = read_whole_file("tests/external/data/UTF-8-test.txt");
    REQUIRE(!test_data.empty());

    auto do_test = [&](tmu_validate validate, const char* replace_str, tm_size_t replace_str_len, bool nullterminate) {
        auto required_size =
            tmu_utf8_convert_from_bytes(test_data.data(), (tm_size_t)test_data.size(), tmu_encoding_unknown, validate,
                                        replace_str, replace_str_len, nullterminate, /*out=*/nullptr, /*out_len=*/0);
        REQUIRE(required_size.ec == TM_ERANGE);

        vector<char> converted;
        converted.resize((size_t)required_size.size);

        auto converted_size = tmu_utf8_convert_from_bytes(test_data.data(), (tm_size_t)test_data.size(),
                                                          tmu_encoding_unknown, validate, replace_str, replace_str_len,
                                                          nullterminate, converted.data(), (tm_size_t)converted.size());
        REQUIRE(converted_size.ec == TM_OK);
        if (nullterminate) {
            REQUIRE(converted.back() == 0);
        }
        // Nullterminator is not part of reported size.
        REQUIRE(converted_size.size + nullterminate == required_size.size);

        REQUIRE(tmu_utf8_valid_range(converted.data(), (tm_size_t)converted.size()) == (tm_size_t)converted.size());

        auto stream = tmu_utf8_make_stream_n(converted.data(), (tm_size_t)converted.size());
        uint32_t codepoint = 0;
        while (tmu_utf8_extract(&stream, &codepoint))
            ;
        REQUIRE(stream.cur == stream.end);
    };

    auto error = tmu_utf8_convert_from_bytes(test_data.data(), (tm_size_t)test_data.size(), tmu_encoding_unknown,
                                             tmu_validate_error,
                                             /*replace_str=*/nullptr, /*replace_str_len=*/0,
                                             /*nullterminate=*/false, /*out=*/nullptr, /*out_len=*/0);
    REQUIRE(error.ec == TM_EINVAL);

    auto error_nz = tmu_utf8_convert_from_bytes(test_data.data(), (tm_size_t)test_data.size(), tmu_encoding_unknown,
                                                tmu_validate_error,
                                                /*replace_str=*/nullptr, /*replace_str_len=*/0,
                                                /*nullterminate=*/true, /*out=*/nullptr, /*out_len=*/0);
    REQUIRE(error_nz.ec == TM_EINVAL);

    do_test(tmu_validate_skip, nullptr, 0, false);
    do_test(tmu_validate_skip, nullptr, 0, true);
    do_test(tmu_validate_replace, "_", 1, false);
    do_test(tmu_validate_replace, "_", 1, true);
    do_test(tmu_validate_replace, "XXXXXXXXXXXXXXX", 15, false);
    do_test(tmu_validate_replace, "XXXXXXXXXXXXXXX", 15, true);
}

TEST_CASE("valid unicode bruteforce") {
    auto test_codepoint_utf8 = [](uint32_t codepoint, bool valid) {
        char buffer[10];
        auto size = tmu_utf8_encode(codepoint, buffer, 10);
        REQUIRE(size <= 10);
        tm_size_t comp = ((valid) ? size : 0);
        REQUIRE(tmu_utf8_valid_range(buffer, size) == comp);
    };
    auto test_codepoint_utf16 = [](uint32_t codepoint, bool valid) {
        tmu_char16 buffer[10];
        auto size = tmu_utf16_encode(codepoint, buffer, 10);
        ;
        REQUIRE(size <= 10);
        tm_size_t comp = ((valid) ? size : 0);
        REQUIRE(tmu_utf16_valid_range(buffer, size) == comp);
    };

    for (uint32_t codepoint = 0; codepoint < TMU_LEAD_SURROGATE_MIN; ++codepoint) {
        test_codepoint_utf8(codepoint, true);
        test_codepoint_utf16(codepoint, true);
    }
    // Disable asserts so we can create invalid codepoints.
    enable_asserts(false);
    for (uint32_t codepoint = TMU_LEAD_SURROGATE_MIN; codepoint <= TMU_TRAILING_SURROGATE_MAX; ++codepoint) {
        test_codepoint_utf8(codepoint, false);
        test_codepoint_utf16(codepoint, false);
    }
    enable_asserts(true);
    for (uint32_t codepoint = TMU_TRAILING_SURROGATE_MAX + 1; codepoint <= TMU_MAX_UTF32; ++codepoint) {
        test_codepoint_utf8(codepoint, true);
        test_codepoint_utf16(codepoint, true);
    }
}

TEST_CASE("utf8 copy truncated") {
    char buffer[100];
    auto stream = tmu_utf8_make_output_stream(buffer, 100);
    tmu_utf8_append('a', &stream);            // Ascii letter a, encodes to 1 octet.
    tmu_utf8_append(0x0710, &stream);         // SYRIAC LETTER ALAPH, encodes to 2 octets.
    tmu_utf8_append(0x1A20, &stream);         // TAI THAM LETTER HIGH KA, encodes to 3 octets.
    tmu_utf8_append(TMU_MAX_UTF32, &stream);  // Encodes to 4 octets.
    tmu_utf8_append(TMU_MAX_UTF32, &stream);
    tmu_utf8_append(TMU_MAX_UTF32, &stream);

    REQUIRE(stream.size <= stream.capacity);
    REQUIRE(stream.capacity <= 100);

    char truncated[100];

    for (tm_size_t i = 0; i < stream.size; ++i) {
        auto truncated_size = tmu_utf8_copy_truncated(stream.data, stream.size, truncated, i);
        REQUIRE(tmu_utf8_valid_range(truncated, truncated_size) == truncated_size);
        if (truncated_size) {
            REQUIRE(memcmp(stream.data, truncated, truncated_size * sizeof(char)) == 0);
        }
    }
}

TEST_CASE("utf8 skip invalid") {
    char buffer[100];
    auto stream = tmu_utf8_make_output_stream(buffer, 100);
    tmu_utf8_append('a', &stream);            // Ascii letter a, encodes to 1 octet.
    tmu_utf8_append(0x0710, &stream);         // SYRIAC LETTER ALAPH, encodes to 2 octets.
    tmu_utf8_append(0x1A20, &stream);         // TAI THAM LETTER HIGH KA, encodes to 3 octets.
    tmu_utf8_append(TMU_MAX_UTF32, &stream);  // Encodes to 4 octets.
    tmu_utf8_append(TMU_MAX_UTF32, &stream);
    tmu_utf8_append(TMU_MAX_UTF32, &stream);

    REQUIRE(stream.size <= stream.capacity);
    REQUIRE(stream.capacity <= 100);

    char truncated[100];

    for (tm_size_t i = 0; i < stream.size; ++i) {
        memcpy(truncated, stream.data, i * sizeof(char));
        auto truncated_size = tmu_utf8_skip_invalid(truncated, i);
        REQUIRE(tmu_utf8_valid_range(truncated, truncated_size) == truncated_size);
        if (truncated_size) {
            REQUIRE(memcmp(stream.data, truncated, truncated_size * sizeof(char)) == 0);
        }
    }
}

TEST_CASE("codepoints count") {
    CHECK(tmu_utf8_count_codepoints("ascii") == strlen("ascii"));

    char a_buffer[100];
    auto a = tmu_utf8_make_output_stream(a_buffer, 100);
    tmu_utf8_append(0x10020, &a);  // LINEAR B SYLLABLE B039 PI (4 octets).
    tmu_utf8_append(0x2E80, &a);   // CJK RADICAL REPEAT (3 octets).
    tmu_utf8_append(0x01EE, &a);   // LATIN CAPITAL LETTER EZH WITH CARON (2 octets).
    tmu_utf8_append('a', &a);

    uint8_t raw[] = {0xF0, 0x90, 0x80, 0xA0,  // LINEAR B SYLLABLE B039 PI (4 octets).
                     0xE2, 0xBA, 0x80,        // CJK RADICAL REPEAT (3 octets).
                     0xC7, 0xAE,              // LATIN CAPITAL LETTER EZH WITH CARON (2 octets).
                     'a'};

    REQUIRE(a.size == 10);
    REQUIRE((tm_size_t)std::size(raw) == a.size);
    REQUIRE(memcmp(a.data, raw, a.size * sizeof(char)) == 0);
    REQUIRE(tmu_utf8_compare(a.data, a.size, (const char*)raw, a.size) == 0);
    REQUIRE(tmu_utf8_equals(a.data, a.size, (const char*)raw, a.size));
    REQUIRE(tmu_utf8_count_codepoints_n(a.data, a.size) == 4);

    // Nullterminate.
    tmu_utf8_append(0, &a);
    REQUIRE(tmu_utf8_count_codepoints(a.data) == 4);
}

#if TMU_UCD_HAS_SIMPLE_CASE || TMU_UCD_HAS_SIMPLE_CASE_TOGGLE
TEST_CASE("case transform simple") {
    auto do_test = [](auto transform, const char* str, const char* expected) {
        char buffer[100];
        auto result = transform(str, (tm_size_t)strlen(str), buffer, 100);
        REQUIRE(result.size < 100);
        buffer[result.size] = 0;

        CAPTURE(str);
        CAPTURE(expected);
        CAPTURE(buffer);
        REQUIRE(result.ec == TM_OK);
        REQUIRE(result.size == (tm_size_t)strlen(expected));
        REQUIRE(memcmp(expected, buffer, result.size * sizeof(char)) == 0);
    };

#if TMU_UCD_HAS_SIMPLE_CASE
    do_test(tmu_utf8_to_upper_simple, "asd", "ASD");
    do_test(tmu_utf8_to_upper_simple, "\xC3\xA4", "\xC3\x84");  // LATIN SMALL LETTER A WITH DIAERESIS

    do_test(tmu_utf8_to_title_simple, "asd", "ASD");
    do_test(tmu_utf8_to_title_simple, "\xC3\xA4", "\xC3\x84");  // LATIN SMALL LETTER A WITH DIAERESIS

    do_test(tmu_utf8_to_lower_simple, "ASD", "asd");
    do_test(tmu_utf8_to_title_simple, "\xC3\x84", "\xC3\x84");  // LATIN CAPITAL LETTER A WITH DIAERESIS
#endif                                                          /* TMU_UCD_HAS_SIMPLE_CASE */

#if TMU_UCD_HAS_SIMPLE_CASE_TOGGLE
    do_test(tmu_utf8_toggle_case_simple, "asd", "ASD");
    do_test(tmu_utf8_toggle_case_simple, "ASD", "asd");
    do_test(tmu_utf8_toggle_case_simple, "AsD", "aSd");
    do_test(tmu_utf8_toggle_case_simple, "aSd", "AsD");
    do_test(tmu_utf8_toggle_case_simple, "\xC3\xA4", "\xC3\x84");  // LATIN SMALL LETTER A WITH DIAERESIS
    do_test(tmu_utf8_toggle_case_simple, "\xC3\x84", "\xC3\xA4");  // LATIN SMALL LETTER A WITH DIAERESIS
#endif                                                             /* TMU_UCD_HAS_SIMPLE_CASE_TOGGLE */
}
#endif /* TMU_UCD_HAS_SIMPLE_CASE || TMU_UCD_HAS_SIMPLE_CASE_TOGGLE */

#if TMU_UCD_HAS_FULL_CASE
TEST_CASE("case transform") {
    auto do_test = [](auto transform, const char* str, const char* expected) {
        char buffer[100];
        auto result = transform(str, (tm_size_t)strlen(str), buffer, 100);
        REQUIRE(result.size < 100);
        buffer[result.size] = 0;

        CAPTURE(str);
        CAPTURE(expected);
        CAPTURE(buffer);
        REQUIRE(result.ec == TM_OK);
        REQUIRE(result.size == (tm_size_t)strlen(expected));
        REQUIRE(memcmp(expected, buffer, result.size * sizeof(char)) == 0);
    };

    do_test(tmu_utf8_to_upper, "asd", "ASD");
    do_test(tmu_utf8_to_upper, "\xC3\xA4", "\xC3\x84");  // LATIN SMALL LETTER A WITH DIAERESIS

    do_test(tmu_utf8_to_title, "asd", "ASD");
    do_test(tmu_utf8_to_title, "\xC3\xA4", "\xC3\x84");  // LATIN SMALL LETTER A WITH DIAERESIS

    do_test(tmu_utf8_to_lower, "ASD", "asd");
    do_test(tmu_utf8_to_title, "\xC3\x84", "\xC3\x84");  // LATIN CAPITAL LETTER A WITH DIAERESIS

    // GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    do_test(tmu_utf8_to_upper, "\xE1\xBD\x92", "\xCE\xA5\xCC\x93\xCC\x80");
    do_test(tmu_utf8_to_title, "\xE1\xBD\x92", "\xCE\xA5\xCC\x93\xCC\x80");

    // LATIN SMALL LIGATURE FF
    do_test(tmu_utf8_to_title, "\xEF\xAC\x80", "Ff");
    do_test(tmu_utf8_to_upper, "\xEF\xAC\x80", "FF");
    // LATIN SMALL LIGATURE FFI
    do_test(tmu_utf8_to_title, "\xEF\xAC\x83", "Ffi");
    do_test(tmu_utf8_to_upper, "\xEF\xAC\x83", "FFI");

#if TMU_UCD_HAS_FULL_CASE_TOGGLE
    do_test(tmu_utf8_toggle_case, "asd", "ASD");
    do_test(tmu_utf8_toggle_case, "ASD", "asd");
    do_test(tmu_utf8_toggle_case, "AsD", "aSd");
    do_test(tmu_utf8_toggle_case, "aSd", "AsD");
    do_test(tmu_utf8_toggle_case, "\xC3\xA4", "\xC3\x84");  // LATIN SMALL LETTER A WITH DIAERESIS
    do_test(tmu_utf8_toggle_case, "\xC3\x84", "\xC3\xA4");  // LATIN SMALL LETTER A WITH DIAERESIS
    // GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    do_test(tmu_utf8_toggle_case, "\xE1\xBD\x92", "\xCE\xA5\xCC\x93\xCC\x80");
#endif /* TMU_UCD_HAS_FULL_CASE_TOGGLE */
}
#endif /* TMU_UCD_HAS_FULL_CASE */

#if TMU_UCD_HAS_SIMPLE_CASE_FOLD
TEST_CASE("case fold simple") {
    const tm_size_t buffer_len = 100;

    char a_buffer[buffer_len];
    auto a = tmu_utf8_make_output_stream(a_buffer, buffer_len);
    tmu_utf8_append('a', &a);
    tmu_utf8_append(0x00E4, &a);  // ä LATIN SMALL LETTER A WITH DIAERESIS
    tmu_utf8_append(0x047B, &a);  // CYRILLIC SMALL LETTER ROUND OMEGA

    char b_buffer[buffer_len];
    auto b = tmu_utf8_make_output_stream(b_buffer, buffer_len);
    tmu_utf8_append('A', &b);
    tmu_utf8_append(0x00C4, &b);  // Ä LATIN SMALL LETTER A WITH DIAERESIS
    tmu_utf8_append(0x047A, &b);  // CYRILLIC CAPITAL LETTER ROUND OMEGA

    REQUIRE(a.ec == TM_OK);
    REQUIRE(b.ec == TM_OK);

    REQUIRE(tmu_utf8_compare_ignore_case_simple(a.data, a.size, b.data, b.size) == 0);
    REQUIRE(tmu_utf8_equals_ignore_case_simple(a.data, a.size, b.data, b.size));
    REQUIRE(tmu_utf8_human_compare_simple(a.data, a.size, b.data, b.size) == 0);

    char a_fold[buffer_len];
    char b_fold[buffer_len];

    auto a_transform = tmu_utf8_to_case_fold_simple(a.data, a.size, a_fold, buffer_len);
    auto b_transform = tmu_utf8_to_case_fold_simple(b.data, b.size, b_fold, buffer_len);

    REQUIRE(a_transform.ec == TM_OK);
    REQUIRE(b_transform.ec == TM_OK);
    REQUIRE(a_transform.size == b_transform.size);
    REQUIRE(memcmp(a_fold, b_fold, a_transform.size * sizeof(char)) == 0);
    REQUIRE(tmu_utf8_equals(a_fold, a_transform.size, b_fold, b_transform.size));
    REQUIRE(tmu_utf8_equals_ignore_case_simple(a_fold, a_transform.size, b_fold, b_transform.size));
    REQUIRE(tmu_utf8_compare(a_fold, a_transform.size, b_fold, b_transform.size) == 0);
    REQUIRE(tmu_utf8_compare_ignore_case_simple(a_fold, a_transform.size, b_fold, b_transform.size) == 0);
    REQUIRE(tmu_utf8_human_compare_simple(a_fold, a_transform.size, b_fold, b_transform.size) == 0);
}

TEST_CASE("human compare simple") {
    std::string_view strings[] = {"ASD10",    "asd",
                                  "\xC7\xAE",  // 01EE LATIN CAPITAL LETTER EZH WITH CARON
                                  "asd10",    "ASD2", "b", "asd1", "asd9"};

    // Human compare is case insensitive, so both cases could occur.
    const std::string_view expected0[] = {
        "asd",      "asd1", "ASD2", "asd9", "ASD10", "asd10", "b",
        "\xC7\xAE",  // 01EE LATIN CAPITAL LETTER EZH WITH CARON
    };
    const std::string_view expected1[] = {
        "asd",      "asd1", "ASD2", "asd9", "asd10", "ASD10", "b",
        "\xC7\xAE",  // 01EE LATIN CAPITAL LETTER EZH WITH CARON
    };

    std::sort(std::begin(strings), std::end(strings), [](std::string_view a, std::string_view b) {
        return tmu_utf8_human_compare_simple(a.data(), (tm_size_t)a.size(), b.data(), (tm_size_t)b.size()) < 0;
    });

    bool case0 = std::equal(std::begin(strings), std::end(strings), std::begin(expected0));
    bool case1 = std::equal(std::begin(strings), std::end(strings), std::begin(expected1));
    REQUIRE((case0 || case1));
}
#endif /* TMU_UCD_HAS_SIMPLE_CASE_FOLD */

#if TMU_UCD_HAS_FULL_CASE_FOLD
TEST_CASE("case fold") {
    const tm_size_t buffer_len = 100;
    char a_buffer[buffer_len];
    auto a = tmu_utf8_make_output_stream(a_buffer, buffer_len);
    tmu_utf8_append('a', &a);
    tmu_utf8_append(0x00E4, &a);  // ä LATIN SMALL LETTER A WITH DIAERESIS
    tmu_utf8_append(0x047B, &a);  // CYRILLIC SMALL LETTER ROUND OMEGA

    tmu_utf8_append(0x1F56, &a);  // GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI

    char b_buffer[buffer_len];
    auto b = tmu_utf8_make_output_stream(b_buffer, buffer_len);
    tmu_utf8_append('A', &b);
    tmu_utf8_append(0x00C4, &b);  // Ä LATIN SMALL LETTER A WITH DIAERESIS
    tmu_utf8_append(0x047A, &b);  // CYRILLIC CAPITAL LETTER ROUND OMEGA

    // GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI casefolds into three seperate codepoints.
    tmu_utf8_append(0x03C5, &b);  // GREEK SMALL LETTER UPSILON
    tmu_utf8_append(0x0313, &b);  // COMBINING COMMA ABOVE
    tmu_utf8_append(0x0342, &b);  // COMBINING GREEK PERISPOMENI

    REQUIRE(a.ec == TM_OK);
    REQUIRE(b.ec == TM_OK);

    REQUIRE(tmu_utf8_compare_ignore_case(a.data, a.size, b.data, b.size) == 0);
    REQUIRE(tmu_utf8_equals_ignore_case(a.data, a.size, b.data, b.size));
    REQUIRE(tmu_utf8_human_compare(a.data, a.size, b.data, b.size) == 0);

    char a_fold[buffer_len];
    char b_fold[buffer_len];

    auto a_transform = tmu_utf8_to_case_fold(a.data, a.size, a_fold, buffer_len);
    auto b_transform = tmu_utf8_to_case_fold(b.data, b.size, b_fold, buffer_len);

    REQUIRE(a_transform.ec == TM_OK);
    REQUIRE(b_transform.ec == TM_OK);
    REQUIRE(a_transform.size == b_transform.size);
    REQUIRE(memcmp(a_fold, b_fold, a_transform.size * sizeof(char)) == 0);
    REQUIRE(tmu_utf8_equals(a_fold, a_transform.size, b_fold, b_transform.size));
    REQUIRE(tmu_utf8_equals_ignore_case(a_fold, a_transform.size, b_fold, b_transform.size));
    REQUIRE(tmu_utf8_compare(a_fold, a_transform.size, b_fold, b_transform.size) == 0);
    REQUIRE(tmu_utf8_compare_ignore_case(a_fold, a_transform.size, b_fold, b_transform.size) == 0);
    REQUIRE(tmu_utf8_human_compare(a_fold, a_transform.size, b_fold, b_transform.size) == 0);
}

TEST_CASE("human compare") {
    std::string_view strings[] = {"ASD10",    "asd",
                                  "\xC7\xAE",  // 01EE LATIN CAPITAL LETTER EZH WITH CARON
                                  "asd10",    "ASD2", "b", "asd1", "asd9"};

    // Human compare is case insensitive, so both cases could occur.
    const std::string_view expected0[] = {
        "asd",      "asd1", "ASD2", "asd9", "ASD10", "asd10", "b",
        "\xC7\xAE",  // 01EE LATIN CAPITAL LETTER EZH WITH CARON
    };
    const std::string_view expected1[] = {
        "asd",      "asd1", "ASD2", "asd9", "asd10", "ASD10", "b",
        "\xC7\xAE",  // 01EE LATIN CAPITAL LETTER EZH WITH CARON
    };

    std::sort(std::begin(strings), std::end(strings), [](std::string_view a, std::string_view b) {
        return tmu_utf8_human_compare(a.data(), (tm_size_t)a.size(), b.data(), (tm_size_t)b.size()) < 0;
    });

    bool case0 = std::equal(std::begin(strings), std::end(strings), std::begin(expected0));
    bool case1 = std::equal(std::begin(strings), std::end(strings), std::begin(expected1));
    REQUIRE((case0 || case1));
}
#endif /* TMU_UCD_HAS_FULL_CASE_FOLD */

#if TMU_UCD_HAS_CATEGORY
TEST_CASE("ucd category") {
    CHECK(tmu_is_letter('a') == true);
    CHECK(tmu_is_letter(0x1101F) == true);   // BRAHMI LETTER DDA
    CHECK(tmu_is_letter(0x11052) == false);  // BRAHMI NUMBER ONE

    CHECK(tmu_is_lower('a') == true);
    CHECK(tmu_is_upper('A') == true);
    CHECK(tmu_is_upper(0xFF21) == true);     // FULLWIDTH LATIN CAPITAL LETTER A
    CHECK(tmu_is_lower(0xFF41) == true);     // FULLWIDTH LATIN SMALL LETTER A
    CHECK(tmu_is_title(0x1F88) == true);     // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    CHECK(tmu_is_caseless(0x2135) == true);  // ALEF SYMBOL

    CHECK(tmu_is_number('1') == true);
    CHECK(tmu_is_number(0x11052) == true);  // BRAHMI NUMBER ONE
    CHECK(tmu_is_number(0x11053) == true);  // BRAHMI NUMBER TWO

    CHECK(tmu_is_whitespace(' ') == true);

    CHECK(tmu_is_control('\n') == true);
    CHECK(tmu_is_control(0x200D) == true);  // ZERO WIDTH JOINER
    CHECK(tmu_is_control(0x009D) == true);  // OPERATING SYSTEM COMMAND

    CHECK(tmu_is_punctuation('.') == true);
    CHECK(tmu_is_punctuation('-') == true);
    CHECK(tmu_is_punctuation('(') == true);

    CHECK(tmu_is_mark(0x0300) == true);   // COMBINING GRAVE ACCENT
    CHECK(tmu_is_mark(0x0903) == true);   // DEVANAGARI SIGN VISARGA
    CHECK(tmu_is_mark(0x102E0) == true);  // COPTIC EPACT THOUSANDS MARK

    CHECK(tmu_is_symbol(0x03F6) == true);  // GREEK REVERSED LUNATE EPSILON SYMBOL
    CHECK(tmu_is_symbol(0x0608) == true);  // ARABIC RAY
    CHECK(tmu_is_symbol(0x060F) == true);  // ARABIC SIGN MISRA
}
#endif /* TMU_UCD_HAS_CATEGORY */

#ifndef TMU_NO_FILE_IO
#include "file_io_tests.cpp"
#endif