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

#include "generated_tests.cpp"