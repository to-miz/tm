#include <intrin.h>
#pragma intrinsic(_umul128)
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(__popcnt)

static tmi_uint128_t tmi_internal_mul64(uint64_t lhs, uint64_t rhs) {
    tmi_uint128_t result;
    result.low = _umul128(lhs, rhs, &result.high);
    return result;
}

TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
    TM_ASSERT(v != 0);
    unsigned long result;
    _BitScanReverse(&result, (unsigned long)v);
    return (uint32_t)result;
}
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
    TM_ASSERT(v != 0);
    unsigned long result;
    _BitScanForward(&result, (unsigned long)v);
    return (uint32_t)result;
}
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) {
    return __popcnt(v);
}

#if defined(_WIN64)
    #pragma intrinsic(_BitScanForward64)
    #pragma intrinsic(_BitScanReverse64)
    #pragma intrinsic(__popcnt64)

    TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
        TM_ASSERT(v != 0);
        unsigned long result;
        _BitScanReverse64(&result, (unsigned __int64)v);
        return (uint64_t)result;
    }
    TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
        TM_ASSERT(v != 0);
        unsigned long result;
        _BitScanForward64(&result, (unsigned __int64)v);
        return (uint64_t)result;
    }
    TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) {
        return __popcnt64(v);
    }
#else
    #define TMI_NO_FLS64
    #define TMI_NO_FFS64
    #define TMI_NO_POPCOUNT64
#endif /* defined(_WIN64) */
