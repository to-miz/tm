#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <iterator>

#include <assert_throws.h>
#include <assert_throws.cpp>

#include <cstring>
using std::strlen;
using std::strncmp;

// clang-format off
#ifdef USE_STRING_VIEW
    #include <string_view>
    #define TM_STRING_VIEW std::string_view
    #define TM_STRING_VIEW_DATA(x) (x).data()
    #define TM_STRING_VIEW_SIZE(x) ((tm_size_t)(x).size())
    #define TM_STRING_VIEW_MAKE(data, size) std::string_view((data), (size_t)(size))

    bool str_equal(std::string_view a, const char* b) { return a == std::string_view{b}; }
#endif

#ifdef SIGNED_SIZE_T
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 1
    typedef int tm_size_t;
#endif

#ifdef USE_CHARCONV
    #include <charconv>
    #define TMJ_TO_INT(first, last, out_ptr, base) std::from_chars((first), (last), *(out_ptr), (base))
    #define TMJ_TO_UINT(first, last, out_ptr, base) std::from_chars((first), (last), *(out_ptr), (base))
    #define TMJ_TO_INT64(first, last, out_ptr, base) std::from_chars((first), (last), *(out_ptr), (base))
    #define TMJ_TO_UINT64(first, last, out_ptr, base) std::from_chars((first), (last), *(out_ptr), (base))
    #define TMJ_TO_FLOAT(first, last, out_ptr) std::from_chars((first), (last), *(out_ptr))
    #define TMJ_TO_DOUBLE(first, last, out_ptr) std::from_chars((first), (last), *(out_ptr))
#endif

#ifdef USE_TM_CONVERSION
    #define TM_CONVERSION_IMPLEMENTATION
    #include <tm_conversion.h>
    #define TMJ_TO_INT(first, last, out_ptr, base) \
        scan_i32_n((first), (tm_size_t)((last) - (first)), (out_ptr), (base))
    #define TMJ_TO_UINT(first, last, out_ptr, base) \
        scan_u32_n((first), (tm_size_t)((last) - (first)), (out_ptr), (base))
    #define TMJ_TO_INT64(first, last, out_ptr, base) \
        scan_i64_n((first), (tm_size_t)((last) - (first)), (out_ptr), (base))
    #define TMJ_TO_UINT64(first, last, out_ptr, base) \
        scan_u64_n((first), (tm_size_t)((last) - (first)), (out_ptr), (base))
    #define TMJ_TO_FLOAT(first, last, out_ptr) scan_float_n((first), (tm_size_t)((last) - (first)), (out_ptr), 0)
    #define TMJ_TO_DOUBLE(first, last, out_ptr) scan_double_n((first), (tm_size_t)((last) - (first)), (out_ptr), 0)
#endif
// clang-format on

#define TM_JSON_IMPLEMENTATION
#define TMJ_DEFINE_INFINITY_AND_NAN
#include <tm_json.h>

struct AllocatedDocument : JsonAllocatedDocument {
    AllocatedDocument(JsonAllocatedDocument doc) { static_cast<JsonAllocatedDocument&>(*this) = doc; }
    ~AllocatedDocument() { jsonFreeDocument(this); };
};

int32_t get_value(JsonValue value, int32_t def) { return value.getInt(def); }
uint32_t get_value(JsonValue value, uint32_t def) { return value.getUInt(def); }
int64_t get_value(JsonValue value, int64_t def) { return value.getInt64(def); }
uint64_t get_value(JsonValue value, uint64_t def) { return value.getUInt64(def); }

bool str_equal(JsonStringView a, const char* b) {
    auto b_size = (tm_size_t)strlen(b);
    if (a.size != b_size) return false;
    return strncmp(a.data, b, a.size) == 0;
}

std::ostream& operator<<(std::ostream& os, const JsonStringView& value) {
    os.write(value.data, value.size);
    return os;
}

template <class A, class B>
void test_int_conversion(const char* json, A* values, size_t values_size, B def) {
    const uint32_t flags = JSON_READER_ALLOW_PLUS_SIGN | JSON_READER_HEXADECIMAL | JSON_READER_EXTENDED_FLOATS;
    AllocatedDocument pool = jsonAllocateDocumentEx(json, (tm_size_t)strlen(json), flags);
    auto& doc = pool.document;
    REQUIRE(doc.error.type == JSON_OK);

    auto ints = doc.root.getArray();

    REQUIRE(ints);
    REQUIRE(ints.size() == values_size);

    for (size_t i = 0; i < values_size; ++i) {
        auto val = ints[(tm_size_t)i].getString();
        CAPTURE(i);
        INFO("value string: " << val);
        CHECK(get_value(ints[(tm_size_t)i], def) == values[i]);
    }
}

TEST_CASE("int conversion" * doctest::description("Test the fallback string to int conversions.")) {
    const int32_t int32_def = -1000;
    const uint32_t uint32_def = 1000;
    const int64_t int64_def = -1000;
    const uint64_t uint64_def = 1000;

    SUBCASE("int32_t") {
        const char* json = "[0, 1, -1, -0, 100, 32767, 65535, 2147483647, -2147483647, +0, +1, +100]";
        const int32_t values[] = {0, 1, -1, -0, 100, 32767, 65535, 2147483647, -2147483647, +0, +1, +100};
        test_int_conversion(json, values, std::size(values), int32_def);
    }

    SUBCASE("uint32_t") {
        const char* json = "[0, 1, -1, -0, 100, 32767,65535, 2147483647, -2147483647, 4294967295, +0, +1, +100]";
        const uint32_t values[] = {0,          1,          uint32_def, 0,  100, 32767, 65535,
                                   2147483647, uint32_def, 4294967295, +0, +1,  +100};
        test_int_conversion(json, values, std::size(values), uint32_def);
    }

    SUBCASE("int64_t") {
        const char* json = R"([
            0, 1, -1, -0, 100, 32767,65535, 2147483647, -2147483647, 4294967295,
            +0, +1, +100,9223372036854775807, -9223372036854775808
        ])";
        const int64_t values[] = {0,
                                  1,
                                  -1,
                                  -0,
                                  100,
                                  32767,
                                  65535,
                                  2147483647,
                                  -2147483647,
                                  4294967295,
                                  +0,
                                  +1,
                                  +100,
                                  9223372036854775807,
                                  -9223372036854775807 - 1};
        test_int_conversion(json, values, std::size(values), int64_def);
    }

    SUBCASE("uint64_t") {
        const char* json = R"([
            0, 1, -1, -0, 100, 32767,
            65535, 2147483647, -2147483647, 4294967295,
            +0, +1, +100,
            9223372036854775807, -9223372036854775808,
            18446744073709551615
        ])";
        const uint64_t values[] = {0,          1,
                                   uint64_def, 0,
                                   100,        32767,
                                   65535,      2147483647,
                                   uint64_def, 4294967295,
                                   +0,         +1,
                                   +100,       9223372036854775807,
                                   uint64_def, 18446744073709551615ull};
        test_int_conversion(json, values, std::size(values), uint64_def);
    }
}

TEST_CASE("int conversion overflow" *
          doctest::description("Test int conversion with values that should overflow. "
                               "May fail depending on the implementation of strtol "
                               "and the way it reports ERANGE.") *
          doctest::may_fail(true)) {
    const int32_t int32_def = -1000;
    const uint32_t uint32_def = 1000;
    const int64_t int64_def = -1000;
    const uint64_t uint64_def = 1000;

    const char* json = R"([
        4294967295, 18446744073709551615,
        11111111111111111111111111111111111111111111111,
        11111111111111111111111111111111111111111111111111111111111111111111111
    ])";

    const int32_t int32_values[] = {int32_def, int32_def, int32_def, int32_def};
    const uint32_t uint32_values[] = {4294967295, uint32_def, uint32_def, uint32_def};
    const int64_t int64_values[] = {4294967295, int64_def, int64_def, int64_def};
    const uint64_t uint64_values[] = {4294967295, 18446744073709551615ull, uint64_def, uint64_def};

    test_int_conversion(json, int32_values, std::size(int32_values), int32_def);
    test_int_conversion(json, uint32_values, std::size(uint32_values), uint32_def);
    test_int_conversion(json, int64_values, std::size(int64_values), int64_def);
    test_int_conversion(json, uint64_values, std::size(uint64_values), uint64_def);
}

bool ulps_comparison(float a, float b) {
    if (a == b) return true;

    uint32_t au;
    uint32_t bu;
    memcpy(&au, &a, sizeof(au));
    memcpy(&bu, &b, sizeof(au));

    auto am = (au < bu) ? (au) : (bu);
    auto bm = (au < bu) ? (bu) : (au);
    return (bm - am) <= 1;
}
bool ulps_comparison(double a, double b) {
    if (a == b) return true;

    uint64_t au;
    uint64_t bu;
    memcpy(&au, &a, sizeof(au));
    memcpy(&bu, &b, sizeof(au));

    auto am = (au < bu) ? (au) : (bu);
    auto bm = (au < bu) ? (bu) : (au);
    return (bm - am) <= 1;
}

TEST_CASE("float conversion" * doctest::description("Test the fallback string to float conversions.")) {
    auto do_test = []() {
        const char* json = R"([
            0, -0, 1, -1,
            +0, +1,
            0.01, -0.01, 1.01, -1.01,
            +0.01, +1.01,
            1e0, 1e10, -1e+10, 1e-10, +1e+10,
            1.01e0, 1.01e10, -1.01e+10, 1.01e-10, +1.01e+10,
            Infinity, NaN, -Infinity, -NaN, +Infinity, +NaN
        ])";

        const float float_values[] = {0,          -0,          1,       -1,           +0,       +1,           0.01f,
                                      -0.01f,     1.01f,       -1.01f,  +0.01f,       +1.01f,   1e0f,         1e10f,
                                      -1e+10f,    1e-10f,      +1e+10f, 1.01e0f,      1.01e10f, -1.01e+10f,   1.01e-10f,
                                      +1.01e+10f, TM_INFINITY, TM_NAN,  -TM_INFINITY, -TM_NAN,  +TM_INFINITY, +TM_NAN};

        const double double_values[] = {0,         -0,          1,      -1,           +0,      +1,           0.01,
                                        -0.01,     1.01,        -1.01,  +0.01,        +1.01,   1e0,          1e10,
                                        -1e+10,    1e-10,       +1e+10, 1.01e0,       1.01e10, -1.01e+10,    1.01e-10,
                                        +1.01e+10, TM_INFINITY, TM_NAN, -TM_INFINITY, -TM_NAN, +TM_INFINITY, +TM_NAN};

        static_assert(std::size(float_values) == 28);
        static_assert(std::size(double_values) == 28);

        const uint32_t flags = JSON_READER_ALLOW_PLUS_SIGN | JSON_READER_EXTENDED_FLOATS;
        AllocatedDocument pool = jsonAllocateDocumentEx(json, (tm_size_t)strlen(json), flags);
        auto& doc = pool.document;
        REQUIRE(doc.error.type == JSON_OK);

        auto floats = doc.root.getArray();
        REQUIRE(floats.size() == std::size(float_values));

        for (tm_size_t i = 0; i < (tm_size_t)std::size(float_values); ++i) {
            CAPTURE(i);
            auto val = floats[i].getFloat(-1000.0f);
            if (isnan(float_values[i])) {
                // We check like this since nan == nan is always false.
                CHECK(isnan(val));
            } else {
                CHECK(ulps_comparison(val, float_values[i]));
            }
        }

        for (tm_size_t i = 0; i < (tm_size_t)std::size(double_values); ++i) {
            CAPTURE(i);
            auto val = floats[i].getDouble(-1000.0f);
            if (isnan(double_values[i])) {
                // We check like this since nan == nan is always false.
                CHECK(isnan(val));
            } else {
                CHECK(ulps_comparison(val, double_values[i]));
            }
        }
    };

    SUBCASE("default locale") { do_test(); }

    // The fallback string to float conversion (strtod) is locale dependent, so these tests will fail.
    // There is just no reliable, threadsafe way to do string to float conversion other than implementing it yourself.
#if 0
    SUBCASE("english locale") {
        auto prev = setlocale(LC_ALL, "en");
        REQUIRE(prev);
        do_test();
        setlocale(LC_ALL, prev);
    }

    SUBCASE("german locale") {
        // German locale should use ',' as decimal point.
        auto prev = setlocale(LC_ALL, "de");
        REQUIRE(prev);
        auto lc = localeconv();
        bool has_comma_decimal_point = lc && lc->decimal_point && *lc->decimal_point == ',';
        REQUIRE(has_comma_decimal_point);
        do_test();
        setlocale(LC_ALL, prev);
    }

    SUBCASE("turkish locale") {
        // Turkish has different uppercase behavior.
        auto prev = setlocale(LC_ALL, "tr");
        REQUIRE(prev);
        do_test();
        setlocale(LC_ALL, prev);
    }
#endif
}

TEST_CASE("keywords") {
    const char* json = R"({
        "array": [
            null,Null,NULL,
            true,True,TRUE,
            false,False,FALSE,
            infinity,Infinity,INFINITY,
            nan,Nan,NAN
        ],
        "object": {
            "null": null, "Null": Null, "NULL": NULL,
            "true": true, "True": True, "TRUE": TRUE,
            "false": false, "False": False, "FALSE": FALSE,
            "infinity": infinity, "Infinity": Infinity, "INFINITY": INFINITY,
            "nan": nan, "Nan": Nan, "NAN": NAN
        }
    })";

    const uint32_t flags = JSON_READER_EXTENDED_FLOATS | JSON_READER_IGNORE_CASE_KEYWORDS;
    AllocatedDocument pool = jsonAllocateDocumentEx(json, (tm_size_t)strlen(json), flags);
    auto& doc = pool.document;
    REQUIRE(doc.error.type == JSON_OK);

    auto root = doc.root.getObject();
    auto array = root["array"].getArray();
    auto obj = root["object"].getObject();
    REQUIRE(array);
    REQUIRE(array.size() == 15);
    REQUIRE(obj);
    REQUIRE(obj.count == 15);

    CHECK(array[0].isNull());
    CHECK(array[1].isNull());
    CHECK(array[2].isNull());
    CHECK(array[3].getBool() == true);
    CHECK(array[4].getBool() == true);
    CHECK(array[5].getBool() == true);
    CHECK(array[6].getBool() == false);
    CHECK(array[7].getBool() == false);
    CHECK(array[8].getBool() == false);
    CHECK(isinf(array[9].getFloat()));
    CHECK(isinf(array[10].getFloat()));
    CHECK(isinf(array[11].getFloat()));
    CHECK(isnan(array[12].getFloat()));
    CHECK(isnan(array[13].getFloat()));
    CHECK(isnan(array[14].getFloat()));

    REQUIRE(obj.exists("null"));
    REQUIRE(obj.exists("Null"));
    REQUIRE(obj.exists("NULL"));
    REQUIRE(obj.exists("true"));
    REQUIRE(obj.exists("True"));
    REQUIRE(obj.exists("TRUE"));
    REQUIRE(obj.exists("false"));
    REQUIRE(obj.exists("False"));
    REQUIRE(obj.exists("FALSE"));
    REQUIRE(obj.exists("infinity"));
    REQUIRE(obj.exists("Infinity"));
    REQUIRE(obj.exists("INFINITY"));
    REQUIRE(obj.exists("nan"));
    REQUIRE(obj.exists("Nan"));
    REQUIRE(obj.exists("NAN"));

    CHECK(obj["null"].isNull());
    CHECK(obj["Null"].isNull());
    CHECK(obj["NULL"].isNull());
    CHECK(obj["true"].getBool() == true);
    CHECK(obj["True"].getBool() == true);
    CHECK(obj["TRUE"].getBool() == true);
    CHECK(obj["false"].getBool() == false);
    CHECK(obj["False"].getBool() == false);
    CHECK(obj["FALSE"].getBool() == false);
    CHECK(isinf(obj["infinity"].getFloat()));
    CHECK(isinf(obj["Infinity"].getFloat()));
    CHECK(isinf(obj["INFINITY"].getFloat()));
    CHECK(isnan(obj["nan"].getFloat()));
    CHECK(isnan(obj["Nan"].getFloat()));
    CHECK(isnan(obj["NAN"].getFloat()));
}

bool check_json(const char* json, tm_size_t len, uint32_t flags) {
    auto json_copy = new char[len + 10];
    memcpy(json_copy, json, len);
    // Set last couple of chars to invalid chars to that buffer overruns are detected.
    memset(json_copy + len, '\xfe', 10);

    AllocatedDocument pool = jsonAllocateDocument(json_copy, len, flags);
    AllocatedDocument ex_pool = jsonAllocateDocumentEx(json_copy, len, flags);
    delete[] json_copy;
    return pool.document.error.type == JSON_OK && ex_pool.document.error.type == JSON_OK;
}
bool check_json(const char* json, uint32_t flags) { return check_json(json, (tm_size_t)strlen(json), flags); }

bool check_json_ex(const char* json, tm_size_t len, uint32_t flags) {
    auto json_copy = new char[len + 10];
    memcpy(json_copy, json, len);
    // Set last couple of chars to invalid chars to that buffer overruns are detected.
    memset(json_copy + len, '\xfe', 10);
    AllocatedDocument ex_pool = jsonAllocateDocumentEx(json_copy, len, flags);
    delete[] json_copy;
    return ex_pool.document.error.type == JSON_OK;
}
bool check_json_ex(const char* json, uint32_t flags) { return check_json_ex(json, (tm_size_t)strlen(json), flags); }

AllocatedDocument json_doc(const char* json, uint32_t flags) {
    return jsonAllocateDocumentEx(json, (tm_size_t)strlen(json), flags);
}

TEST_CASE("strings") {
    SUBCASE("escapes") {
        {
            const char* json = R"(["Teststring \"\\\"\r\n\t\f\\n\\\n\\\\n Hello"])";
            const char* comp = "Teststring \"\\\"\r\n\t\f\\n\\\n\\\\n Hello";

            const uint32_t flags = JSON_READER_EXTENDED_FLOATS | JSON_READER_IGNORE_CASE_KEYWORDS;
            AllocatedDocument pool = jsonAllocateDocumentEx(json, (tm_size_t)strlen(json), flags);
            auto& doc = pool.document;
            REQUIRE(doc.error.type == JSON_OK);

            CHECK(str_equal(doc.root.getArray()[0].getString(), comp));
        }
        REQUIRE(check_json(R"(["Teststring\\"])", JSON_READER_STRICT) == true);
        const char* raw = R"(["Teststring\"])";
        REQUIRE(check_json(raw, JSON_READER_STRICT) == false);
    }

    SUBCASE("unicode escape") {
        REQUIRE(check_json(R"(["Invalid string \updog"])", JSON_READER_STRICT) == false);
        REQUIRE(check_json(R"(["Valid string \\updog"])", JSON_READER_STRICT) == true);
        REQUIRE(check_json(R"(["Invalid string \u"])", JSON_READER_STRICT) == false);
        REQUIRE(check_json(R"(["Valid string \\u"])", JSON_READER_STRICT) == true);
    }

    SUBCASE("escaped multiline") {
        REQUIRE(check_json("[\"Valid string \\\nescaped newline\"]", JSON_READER_ESCAPED_MULTILINE_STRINGS) == true);
        REQUIRE(check_json("[\"Invalid string \nunescaped newline\"]", JSON_READER_ESCAPED_MULTILINE_STRINGS) == false);
        REQUIRE(check_json("[\"No newline\"]", JSON_READER_ESCAPED_MULTILINE_STRINGS) == true);
        REQUIRE(check_json("[\"Escaped at the end \\\n\"]", JSON_READER_ESCAPED_MULTILINE_STRINGS) == true);
        REQUIRE(check_json("[\"Invalid \\\\\n\"]", JSON_READER_ESCAPED_MULTILINE_STRINGS) == false);
    }

    SUBCASE("concatenated strings") {
        const uint32_t flags = JSON_READER_CONCATENATED_STRINGS | JSON_READER_CONCATENATED_STRINGS_IN_ARRAYS;

        REQUIRE(check_json(R"(["Valid string" invalid "another valid string"])", flags) == false);

        // With escaped quotation marks
        // Repeated assignments to json are workaround for MSVC raw string parsing when escape sequences are present.
        const char* json = R"(["Valid string\"" \" "another \"valid string"])";
        REQUIRE(check_json(json, flags) == false);
        json = R"(["Valid string\\\" "another \"valid string"])";
        REQUIRE(check_json(json, flags) == false);
        json = R"(["Valid string\\"" "another \"valid string"])";
        REQUIRE(check_json(json, flags) == false);

        // With newlines
        json = R"(["Valid string\"" \"
                   "another \"valid string"])";
        REQUIRE(check_json(json, flags) == false);
        json = R"(["Valid string\\\"
                   "another \"valid string"])";
        REQUIRE(check_json(json, flags) == false);
        json = R"(["Valid string\\""
                   "another \"valid string"])";
        REQUIRE(check_json(json, flags) == false);

        auto check_and_compare = [&](const char* json, const char* cmp) {
            auto pool = json_doc(json, flags);
            REQUIRE(pool.document.error.type == JSON_OK);
            REQUIRE(str_equal(pool.document.root.getArray()[0].getString(), cmp));
        };

        check_and_compare(R"(["Valid string""another valid string"])", "Valid stringanother valid string");
        check_and_compare(R"(["Valid string""another valid string""another another"])",
                          "Valid stringanother valid stringanother another");

        // With escaped quotation marks
        check_and_compare(R"(["Valid string\""  "another \"valid string"])", R"(Valid string"another "valid string)");

        // With newlines
        check_and_compare(R"(["Valid string\""
                               "another \"valid string"])",
                          R"(Valid string"another "valid string)");
        check_and_compare(R"(["Valid string\\"
                               "another \"valid string"])",
                          R"(Valid string\another "valid string)");
    }
}

TEST_CASE("unicode") {
    const char* json = R"({
        "copyright_symbol": "\u00A9",
        "Utf-16 surrogate pair": "\uD800\uDD40"
    })";

    const uint32_t flags = JSON_READER_EXTENDED_FLOATS | JSON_READER_IGNORE_CASE_KEYWORDS;
    AllocatedDocument pool = jsonAllocateDocumentEx(json, (tm_size_t)strlen(json), flags);
    auto& doc = pool.document;
    REQUIRE(doc.error.type == JSON_OK);

    auto root = doc.root.getObject();
    REQUIRE(root);

    // COPYRIGHT SIGN "©"
    CHECK(str_equal(root["copyright_symbol"].getString(), "\xC2\xA9"));

    // GREEK ACROPHONIC ATTIC ONE QUARTER "𐅀"
    CHECK(str_equal(root["Utf-16 surrogate pair"].getString(), "\xF0\x90\x85\x80"));

    // Unpaired surrogate pairs.
    // Repeated assignments to json are workaround for MSVC raw string parsing when escape sequences are present.
    json = R"(["Unpaired surrogate pair \uD800"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["Unpaired surrogate pair \uDD40"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["Unpaired surrogate pair \uDD40\uD800"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["Unpaired surrogate pair \uD800\uD800"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["Unpaired surrogate pair \uD800\uD800"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);

    json = R"(["\uD800 Unpaired surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["\uDD40 Unpaired surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["\uDD40\uD800 Unpaired surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["\uD800\uD800 Unpaired surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["\uD800\uD800 Unpaired surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);

    json = R"(["Unpaired \uD800 surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["Unpaired \uDD40 surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["Unpaired \uDD40\uD800 surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["Unpaired \uD800\uD800 surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
    json = R"(["Unpaired \uD800\uD800 surrogate pair"])";
    REQUIRE(check_json(json, JSON_READER_STRICT) == false);
}

TEST_CASE("operator[]") {
    const char* json = R"({
        "aaa": 1,
        "a": 2,

        "b": 1,
        "bbb": 2,

        "array": []
    })";

    const uint32_t flags = 0;
    AllocatedDocument pool = jsonAllocateDocumentEx(json, (tm_size_t)strlen(json), flags);
    auto& doc = pool.document;
    REQUIRE(doc.error.type == JSON_OK);

    auto root = doc.root.getObject();
    REQUIRE(root);

    CHECK(root.exists("c") == false);
    CHECK(root.exists("ab") == false);
    CHECK(root.exists("bb") == false);
    CHECK(root.exists("ba") == false);
    CHECK(root.exists("aa") == false);

    CHECK(root["c"].isNull() == false);
    CHECK(root["ab"].getInt(-1) == -1);
    CHECK(root["bb"].getFloat(-1.0f) == -1.0f);
#ifndef USE_STRING_VIEW
    CHECK(root["ba"].getString().size == 0);
#else
    CHECK(root["ba"].getString().empty());
#endif
    CHECK(root["aa"].getBool(true) == true);
    CHECK(root["aa"].getArray().count == 0);
    CHECK(root["aa"].getObject().count == 0);
    CHECK(root["aa"].isString() == false);
    CHECK(root["aa"].isIntegral() == false);
    CHECK(root["aa"].type == JVAL_NULL);

    CHECK(root.exists("aaa") == true);
    CHECK(root.exists("a") == true);
    CHECK(root.exists("b") == true);
    CHECK(root.exists("bbb") == true);
    CHECK(root.exists("array") == true);

    CHECK(root["aaa"].getInt() == 1);
    CHECK(root["a"].getInt() == 2);
    CHECK(root["b"].getInt() == 1);
    CHECK(root["bbb"].getInt() == 2);

    auto array = root["array"].getArray();
    CHECK(array.count == 0);
    CHECK_ASSERTION_FAILURE(array[0]);
}

// TODO: implement following tests

TEST_CASE("numbers") {}
TEST_CASE("hex conversion") {}
TEST_CASE("extended flags") {}
TEST_CASE("invalid json") {}

// These tests were taken from https://github.com/nst/JSONTestSuite (MIT License see tests/external/LICENSES.txt)
TEST_CASE("n_structure_open_array_object") {
    const char* structure = "[{\"\":";
    char* buffer = new char[strlen(structure) * 50001];
    char* p = buffer;
    for (auto i = 0; i < 50000; ++i, ++p) {
        *p = structure[i % 5];
    }
    *p = 0;
    CHECK(check_json(buffer, JSON_READER_STRICT) == false);
    delete[] buffer;
}
TEST_CASE("n_structure_100000_opening_arrays") {
    char* buffer = new char[100001];
    char* p = buffer;
    for (auto i = 0; i < 100000; ++i, ++p) {
        *p = '[';
    }
    *p = 0;
    CHECK(check_json(buffer, JSON_READER_STRICT) == false);
    delete[] buffer;
}
#include "generated_tests_json5.cpp"
#include "comments_tests.cpp"

TEST_CASE("json5_decimal_point") {
    const char* json = R"(
        [
            1,
            1.,
            .1,
            1e+10,
            .1e+10,
            1.1e+10
        ]
    )";
    CHECK(check_json(json, JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
    CHECK(check_json(".e+10", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
    CHECK(check_json("1.e+10", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
    CHECK(check_json("e+10", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT) == false);

    // Number parsing should also work on single value documents.
    CHECK(check_json("1", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
    CHECK(check_json("1.", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
    CHECK(check_json("{1.", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT) == false);
    CHECK(check_json(".1", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
    CHECK(check_json("1e+10", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
    CHECK(check_json(".1e+10", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
    CHECK(check_json("1.1e+10", JSON_READER_ALLOW_LEADING_AND_TRAILING_DECIMALPOINT));
}

void print_indent(int amount) {
    while (amount > 0) {
        printf("  ");
        --amount;
    }
}
void print_value(JsonValue value, int indent = 0) {
    switch(value.type) {
        case JVAL_NULL:
        case JVAL_STRING:
        case JVAL_INT:
        case JVAL_UINT:
        case JVAL_BOOL:
        case JVAL_FLOAT:
        case JVAL_RAW_STRING:
        case JVAL_CONCAT_STRING: {
            printf("%.*s\n", (int)value.data.content.size, value.data.content.data);
            break;
        }
        case JVAL_OBJECT: {
            print_indent(indent);
            printf("{\n");
            for (auto node : value.getObject()) {
                print_indent(indent + 1);
                printf("\"%.*s\": ", (int)node.name.size, node.name.data);
                print_value(node.value, indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        case JVAL_ARRAY: {
            printf("[\n");
            for (auto val : value.getArray()) {
                print_indent(indent + 1);
                print_value(val, indent + 1);
            }
            print_indent(indent);
            printf("]\n");
            break;
        }
    }
}

TEST_CASE("json5") {
    // Example json5 document taken from https://json5.org/
    const char* json = R"(
        {
          // comments
          unquoted: 'and you can quote me on that',
          singleQuotes: 'I can use "double quotes" here',
          lineBreaks: "Look, Mom! \
No \\n's!",
          hexadecimal: 0xdecaf,
          leadingDecimalPoint: .8675309, andTrailing: 8675309.,
          positiveSign: +1,
          trailingComma: 'in objects', andIn: ['arrays',],
          "backwardsCompatible": "with JSON",
        }
    )";
    CHECK(check_json_ex(json, JSON_READER_JSON5));

    AllocatedDocument ex_pool = jsonAllocateDocumentEx(json, (tm_size_t)strlen(json), JSON_READER_JSON5);
    REQUIRE(ex_pool.document.error.type == JSON_OK);
}