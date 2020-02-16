#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <assert_throws.h>
#include <doctest/doctest.h>
#include <assert_throws.cpp>

#ifdef SIGNED_SIZE_T
#define TM_SIZE_T_DEFINED
#define TM_SIZE_T_IS_SIGNED 1
typedef int tm_size_t;
#endif  // defined(SIGNED_SIZE_T)

#define TM_UINT128_IMPLEMENTATION
#include <tm_uint128.h>

std::ostream& operator<<(std::ostream& os, const tmi_bytes& value) {
    os << "{";
    for (int i = 0; i < 16; ++i) {
        os << std::hex << (int)value.entries[i];
        if (i != 15) os << ", ";
    }
    os << "}";
    return os;
}

bool operator==(const tmi_bytes& lhs, const tmi_bytes& rhs) {
    for (int i = 0; i < 16; ++i) {
        if (lhs.entries[i] != rhs.entries[i]) return false;
    }
    return true;
}

TEST_CASE("float and double tests") {
    CHECK(tml::to_string(tmi_from_double(0.0), 10) == "0");
    CHECK(tml::to_string(tmi_from_double(0.1), 10) == "0");
    CHECK(tml::to_string(tmi_from_double(0.1256), 10) == "0");
    CHECK(tml::to_string(tmi_from_double(0.9948121), 10) == "0");
    CHECK(tml::to_string(tmi_from_double(1.0), 10) == "1");
    CHECK(tml::to_string(tmi_from_double(1.1), 10) == "1");
    CHECK(tml::to_string(tmi_from_double(1.1256), 10) == "1");
    CHECK(tml::to_string(tmi_from_double(1.9948121), 10) == "1");
    CHECK(tml::to_string(tmi_from_double(2.0), 10) == "2");
    CHECK(tml::to_string(tmi_from_double(2.1), 10) == "2");
    CHECK(tml::to_string(tmi_from_double(2.1256), 10) == "2");
    CHECK(tml::to_string(tmi_from_double(2.9948121), 10) == "2");
    CHECK(tml::to_string(tmi_from_double(365.0), 10) == "365");
    CHECK(tml::to_string(tmi_from_double(365.1), 10) == "365");
    CHECK(tml::to_string(tmi_from_double(365.1256), 10) == "365");
    CHECK(tml::to_string(tmi_from_double(365.9948121), 10) == "365");

    CHECK(tml::to_string(tmi_from_float(0.0f), 10) == "0");
    CHECK(tml::to_string(tmi_from_float(0.1f), 10) == "0");
    CHECK(tml::to_string(tmi_from_float(0.1256f), 10) == "0");
    CHECK(tml::to_string(tmi_from_float(0.9948121f), 10) == "0");
    CHECK(tml::to_string(tmi_from_float(1.0f), 10) == "1");
    CHECK(tml::to_string(tmi_from_float(1.1f), 10) == "1");
    CHECK(tml::to_string(tmi_from_float(1.1256f), 10) == "1");
    CHECK(tml::to_string(tmi_from_float(1.9948121f), 10) == "1");
    CHECK(tml::to_string(tmi_from_float(2.0f), 10) == "2");
    CHECK(tml::to_string(tmi_from_float(2.1f), 10) == "2");
    CHECK(tml::to_string(tmi_from_float(2.1256f), 10) == "2");
    CHECK(tml::to_string(tmi_from_float(2.9948121f), 10) == "2");
    CHECK(tml::to_string(tmi_from_float(365.0f), 10) == "365");
    CHECK(tml::to_string(tmi_from_float(365.1f), 10) == "365");
    CHECK(tml::to_string(tmi_from_float(365.1256f), 10) == "365");
    CHECK(tml::to_string(tmi_from_float(365.9948121f), 10) == "365");
}

#include "generated_tests.cpp"