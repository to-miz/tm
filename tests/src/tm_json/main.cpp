#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string_view>
#include <iterator>
#include <locale.h>

#include <assert_throws.h>
#include <assert_throws.cpp>

#define TM_JSON_IMPLEMENTATION
// #define TMJ_NO_AUTODETECT
#define TMJ_DEFINE_INFINITY_AND_NAN
#define TM_STRING_VIEW std::string_view
#define TM_STRING_VIEW_DATA(x) (x).data()
#define TM_STRING_VIEW_SIZE(x) ((tm_size_t)(x).size())
#include <tm_json.h>

struct AllocatedDocument : JsonAllocatedDocument {
    AllocatedDocument(JsonAllocatedDocument doc) { static_cast<JsonAllocatedDocument&>(*this) = doc; }
    ~AllocatedDocument() { jsonFreeDocument(this); };
};

int32_t get_value(JsonValue value, int32_t def) { return value.getInt(def); }
uint32_t get_value(JsonValue value, uint32_t def) { return value.getUInt(def); }
int64_t get_value(JsonValue value, int64_t def) { return value.getInt64(def); }
uint64_t get_value(JsonValue value, uint64_t def) { return value.getUInt64(def); }

template <class A, class B>
void test_int_conversion(std::string_view json, A* values, size_t values_size, B def) {
    const uint32_t flags = JSON_READER_ALLOW_PLUS_SIGN | JSON_READER_HEXADECIMAL | JSON_READER_EXTENDED_FLOATS;
    AllocatedDocument pool = jsonAllocateDocumentEx(json.data(), json.size(), flags);
    auto& doc = pool.document;
    REQUIRE(doc.error.type == JSON_OK);

    auto ints = doc.root.getArray();

    REQUIRE(ints);
    REQUIRE(ints.size() == values_size);

    for (size_t i = 0; i < values_size; ++i) {
        auto val = ints[i].getString();
        CAPTURE(i);
        INFO("value string: " << val);
        CHECK(get_value(ints[i], def) == values[i]);
    }
};

// These tests may fail, because the fallback string conversion functions are locale-dependant
TEST_CASE("int conversion" * doctest::description("Test the fallback string to int conversions.")) {
    const int32_t int32_def = -1000;
    const uint32_t uint32_def = 1000;
    const int64_t int64_def = -1000;
    const uint64_t uint64_def = 1000;

    SUBCASE("int32_t") {
        const std::string_view json = "[0, 1, -1, -0, 100, 32767, 65535, 2147483647, -2147483647, +0, +1, +100]";
        const int32_t values[] = {0, 1, -1, -0, 100, 32767, 65535, 2147483647, -2147483647, +0, +1, +100};
        test_int_conversion(json, values, std::size(values), int32_def);
    }

    SUBCASE("uint32_t") {
        const std::string_view json =
            "[0, 1, -1, -0, 100, 32767,65535, 2147483647, -2147483647, 4294967295, +0, +1, +100]";
        const uint32_t values[] = {0,          1,          uint32_def, 0,  100, 32767, 65535,
                                   2147483647, uint32_def, 4294967295, +0, +1,  +100};
        test_int_conversion(json, values, std::size(values), uint32_def);
    }

    SUBCASE("int64_t") {
        const std::string_view json = R"([
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
        const std::string_view json = R"([
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
                                   uint64_def, 18446744073709551615};
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

    const std::string_view json = R"([
            4294967295, 18446744073709551615,
            11111111111111111111111111111111111111111111111,
            11111111111111111111111111111111111111111111111111111111111111111111111
        ])";

    const int32_t int32_values[] = {int32_def, int32_def, int32_def, int32_def};
    const uint32_t uint32_values[] = {4294967295, uint32_def, uint32_def, uint32_def};
    const int64_t int64_values[] = {4294967295, int64_def, int64_def, int64_def};
    const uint64_t uint64_values[] = {4294967295, 18446744073709551615, uint64_def, uint64_def};

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

TEST_CASE("float conversion" *
          doctest::description("Test the fallback locale-independent string to float conversions.")) {
    auto do_test = []() {
        const std::string_view json = R"([
            0, -0, 1, -1,
            +0, +1,
            0.01, -0.01, 1.01, -1.01,
            +0.01, +1.01,
            1e0, 1e10, -1e+10, 1e-10, +1e+10,
            1.01e0, 1.01e10, -1.01e+10, 1.01e-10, +1.01e+10,
            infinity, nan, -infinity, -nan, +infinity, +nan
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
        AllocatedDocument pool = jsonAllocateDocumentEx(json.data(), json.size(), flags);
        auto& doc = pool.document;
        REQUIRE(doc.error.type == JSON_OK);

        auto floats = doc.root.getArray();
        REQUIRE(floats.size() == std::size(float_values));

        for (size_t i = 0; i < std::size(float_values); ++i) {
            CAPTURE(i);
            auto val = floats[i].getFloat(-1000.0f);
            if (isnan(float_values[i])) {
                // We check like this since nan == nan is always false.
                CHECK(isnan(val));
            } else {
                CHECK(ulps_comparison(val, float_values[i]));
            }
        }

        for (size_t i = 0; i < std::size(double_values); ++i) {
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

    SUBCASE("default locale") {
        do_test();
    }

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
}

TEST_CASE("keywords") {
    const std::string_view json = R"({
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
    AllocatedDocument pool = jsonAllocateDocumentEx(json.data(), json.size(), flags);
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

TEST_CASE("strings") {
    const std::string_view json = R"(["Teststring \"\\\"\r\n\t\f\\n\\\n\\\\n Hello"])";
    const std::string_view comp = "Teststring \"\\\"\r\n\t\f\\n\\\n\\\\n Hello";

    const uint32_t flags = JSON_READER_EXTENDED_FLOATS | JSON_READER_IGNORE_CASE_KEYWORDS;
    AllocatedDocument pool = jsonAllocateDocumentEx(json.data(), json.size(), flags);
    auto& doc = pool.document;
    REQUIRE(doc.error.type == JSON_OK);

    CHECK(std::string_view{doc.root.getArray()[0].getString()} == comp);
}

TEST_CASE("unicode" * doctest::should_fail(true)) {
    const std::string_view json = R"({
        "copyright_symbol": "\u00A9"
    })";

    const uint32_t flags = JSON_READER_EXTENDED_FLOATS | JSON_READER_IGNORE_CASE_KEYWORDS;
    AllocatedDocument pool = jsonAllocateDocumentEx(json.data(), json.size(), flags);
    auto& doc = pool.document;
    REQUIRE(doc.error.type == JSON_OK);

    auto root = doc.root.getObject();
    REQUIRE(root);

    CHECK(root["copyright_symbol"].getString() == std::string_view{u8"©"});
}

TEST_CASE("operator[]") {
    const std::string_view json = R"(
        {
            "aaa": 1,
            "a": 2,

            "b": 1,
            "bbb": 2,

            "array": []
        }
    )";

    const uint32_t flags = 0;
    AllocatedDocument pool = jsonAllocateDocumentEx(json.data(), json.size(), flags);
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
    CHECK(root["ba"].getString() == std::string_view{});
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