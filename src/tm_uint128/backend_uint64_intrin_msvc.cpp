#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(__popcnt)

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
    #pragma intrinsic(_umul128)
    #pragma intrinsic(__shiftleft128)
    #pragma intrinsic(__shiftright128)
    #pragma intrinsic(_BitScanForward64)
    #pragma intrinsic(_BitScanReverse64)
    #pragma intrinsic(__popcnt64)

    static tmi_uint128_t tmi_internal_mul64(uint64_t lhs, uint64_t rhs) {
        tmi_uint128_t result;
        result.low = _umul128(lhs, rhs, &result.high);
        return result;
    }

    TMI_DEF tmi_uint128_t tmi_shl_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
        TM_ASSERT(shift_amount < 128);
        if (shift_amount == 0) return lhs;

        tmi_uint128_t result;
        if (shift_amount >= 64) {
            shift_amount -= 64;
            result.low = 0;
            result.high = TMI_LOW(lhs) << shift_amount;
        } else {
            result.low = (TMI_LOW(lhs) << shift_amount);
            result.high = __shiftleft128(TMI_LOW(lhs), TMI_HIGH(lhs), (unsigned char)shift_amount);
        }
        return result;
    }
    TMI_DEF tmi_uint128_t tmi_shr_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
        TM_ASSERT(shift_amount < 128);
        if (shift_amount == 0) return lhs;

        tmi_uint128_t result;
        if (shift_amount >= 64) {
            shift_amount -= 64;
            result.low = TMI_HIGH(lhs) >> shift_amount;
            result.high = 0;
        } else {
            result.high = (TMI_HIGH(lhs) >> shift_amount);
            result.low = __shiftright128(TMI_LOW(lhs), TMI_HIGH(lhs), (unsigned char)shift_amount);
        }
        return result;
    }

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
    #define TMI_NO_MUL128
    #define TMI_NO_SHIFT128
#endif /* defined(_WIN64) */
