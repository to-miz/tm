TMI_DEF tmi_uint128_t tmi_make(uint64_t low, uint64_t high) {
    tmi_uint128_t result;
    result.low = low;
    result.high = high;
    return result;
}

TMI_DEF tmi_uint128_t tmi_make_low(uint64_t low) {
    tmi_uint128_t result;
    result.low = low;
    result.high = 0;
    return result;
}
TMI_DEF tmi_uint128_t tmi_make_high(uint64_t high) {
    tmi_uint128_t result;
    result.low = 0;
    result.high = high;
    return result;
}
TMI_DEF tmi_uint128_t tmi_make_bitmask(uint64_t set_bit_position) {
    TM_ASSERT(set_bit_position < 128);
    tmi_uint128_t result;
    if (set_bit_position >= 64) {
        result.low = 0;
        result.high = 1ull << (set_bit_position - 64);
    } else {
        result.low = 1ull << set_bit_position;
        result.high = 0;
    }
    return result;
}

// Adds two 64 Bit numbers together and returns result.
static tmi_uint128_t tmi_internal_add64(uint64_t lhs, uint64_t rhs) {
    tmi_uint128_t result;
    result.low = lhs + rhs;
    result.high = (result.low < lhs);  // Carry bit is set if result wrapped.
    return result;
}
static tmi_uint128_t tmi_internal_sub64(uint64_t lhs, uint64_t rhs) {
    tmi_uint128_t result;
    result.low = lhs - rhs;
    result.high = (result.low > lhs);  // Carry bit is set if result wrapped.
    return result;
}

TMI_DEF tmi_uint128_t tmi_add(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result = tmi_internal_add64(TMI_LOW(lhs), TMI_LOW(rhs));
    // High is lhs + rhs + carry_bit.
    TMI_HIGH(result) = TMI_HIGH(lhs) + TMI_HIGH(rhs) + TMI_HIGH(result);
    return result;
}
TMI_DEF tmi_uint128_t tmi_sub(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result = tmi_internal_sub64(TMI_LOW(lhs), TMI_LOW(rhs));
    // High is lhs - rhs - carry_bit.
    TMI_HIGH(result) = TMI_HIGH(lhs) - TMI_HIGH(rhs) - TMI_HIGH(result);
    return result;
}

TMI_DEF tmi_uint128_t tmi_add_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    tmi_uint128_t result = tmi_internal_add64(TMI_LOW(lhs), rhs);
    // High is lhs + carry_bit.
    TMI_HIGH(result) = TMI_HIGH(lhs) + TMI_HIGH(result);
    return result;
}
TMI_DEF tmi_uint128_t tmi_sub_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    tmi_uint128_t result = tmi_internal_sub64(TMI_LOW(lhs), rhs);
    // High is lhs - carry_bit.
    TMI_HIGH(result) = TMI_HIGH(lhs) - TMI_HIGH(result);
    return result;
}

static tmi_uint128_t tmi_internal_mul64(uint64_t lhs, uint64_t rhs);

TMI_DEF tmi_uint128_t tmi_mul(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result = tmi_internal_mul64(TMI_LOW(lhs), TMI_LOW(rhs));
    result.high += TMI_LOW(lhs) * TMI_HIGH(rhs);
    result.high += TMI_HIGH(lhs) * TMI_LOW(rhs);
    return result;
}
TMI_DEF tmi_uint128_t tmi_mul_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    tmi_uint128_t result = tmi_internal_mul64(TMI_LOW(lhs), rhs);
    result.high += TMI_HIGH(lhs) * rhs;
    return result;
}

TMI_DEF tmi_uint128_t tmi_div(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return tmi_divmod(lhs, rhs).div; }
TMI_DEF tmi_uint128_t tmi_mod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return tmi_divmod(lhs, rhs).mod; }
TMI_DEF tmi_uint128_t tmi_and(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result;
    result.low = TMI_LOW(lhs) & TMI_LOW(rhs);
    result.high = TMI_HIGH(lhs) & TMI_HIGH(rhs);
    return result;
}
TMI_DEF tmi_uint128_t tmi_or(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result;
    result.low = TMI_LOW(lhs) | TMI_LOW(rhs);
    result.high = TMI_HIGH(lhs) | TMI_HIGH(rhs);
    return result;
}
TMI_DEF tmi_uint128_t tmi_xor(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    tmi_uint128_t result;
    result.low = TMI_LOW(lhs) ^ TMI_LOW(rhs);
    result.high = TMI_HIGH(lhs) ^ TMI_HIGH(rhs);
    return result;
}
TMI_DEF tmi_uint128_t tmi_not(tmi_uint128_t_arg v) {
    tmi_uint128_t result;
    result.low = ~TMI_LOW(v);
    result.high = ~TMI_HIGH(v);
    return result;
}

TMI_DEF tmi_uint128_t tmi_shl(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(TMI_HIGH(rhs) == 0 && TMI_LOW(rhs) < 128);
    return tmi_shl_u64(lhs, TMI_LOW(rhs));
}
TMI_DEF tmi_uint128_t tmi_shr(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(TMI_HIGH(rhs) == 0 && TMI_LOW(rhs) < 128);
    return tmi_shr_u64(lhs, TMI_LOW(rhs));
}

#ifdef TMI_NO_SHIFT128
TMI_DEF tmi_uint128_t tmi_shl_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
    TM_ASSERT(shift_amount < 128);
    if (shift_amount == 0) return lhs;

    tmi_uint128_t result;
    if (shift_amount >= 64) {
        shift_amount -= 64;
        result.low = 0;
        result.high = TMI_LOW(lhs) << shift_amount;
    } else {
        uint64_t overflow_mask = TMI_LOW(lhs) >> (64 - shift_amount);
        result.low = (TMI_LOW(lhs) << shift_amount);
        result.high = (TMI_HIGH(lhs) << shift_amount) | overflow_mask;
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
        uint64_t overflow_mask = TMI_HIGH(lhs) << (64 - shift_amount);
        result.low = (TMI_LOW(lhs) >> shift_amount) | overflow_mask;
        result.high = (TMI_HIGH(lhs) >> shift_amount);
    }
    return result;
}
#endif

TMI_DEF tmi_uint128_t_arg tmi_set_bit(tmi_uint128_t_arg v, uint64_t bit_position) {
    tmi_uint128_t_arg result = TMI_DEREF(v);
    if (bit_position >= 64) {
        bit_position -= 64;
        result.high |= (1ull << bit_position);
    } else {
        result.low |= (1ull << bit_position);
    }
    return result;
}

TMI_DEF tmi_divmod_result tmi_divmod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(tmi_is_not_zero(rhs));

    tmi_divmod_result result;
    // Handle some trivial cases.
    if (TMI_HIGH(rhs) == 0 && TMI_LOW(rhs) == 1) {
        // lhs / 1
        result.div = TMI_DEREF(lhs);
        result.mod = tmi_zero;
    }
    if (TMI_HIGH(rhs) == 0 && TMI_LOW(rhs) == 2) {
        // lhs / 2
        result.div = tmi_shr(lhs, TMI_PASS(tmi_one));
        result.mod.low = (uint64_t)tmi_is_bit_set(lhs, 0);
        result.mod.high = 0;
    } else if (tmi_eq(lhs, rhs)) {
        // lhs / lhs
        result.div = tmi_one;
        result.mod = tmi_zero;
    } else if (tmi_lt(lhs, rhs)) {
        // (rhs - c) / rhs
        result.div = tmi_zero;
        result.mod = TMI_DEREF(lhs);
    } else {
        // Used algorithm is binary long division.
        result.div = tmi_zero;
        result.mod = tmi_zero;

        // lhs is guaranteed > 0 since we checked whether lhs <= rhs in the other cases and rhs cannot be 0.
        uint64_t i = tmi_fls(lhs);
        for (;;) {
            result.mod.low |= (uint64_t)tmi_is_bit_set(lhs, i);

            if (tmi_gte(TMI_PASS(result.mod), rhs)) {
                result.mod = tmi_sub(TMI_PASS(result.mod), rhs);
                result.div = tmi_set_bit(result.div, i);
            }
            if (!i) break;
            --i;

            // Inline shift one bit left
            result.mod.high = (result.mod.high << 1) | (result.mod.low >> 63);
            result.mod.low <<= 1;
        }
    }

    return result;
}
TMI_DEF tmi_divmod_result tmi_divmod_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    TM_ASSERT(rhs != 0);

    tmi_divmod_result result;
    // Handle some trivial cases.
    if (rhs == 1) {
        // lhs / 1
        result.div = TMI_DEREF(lhs);
        result.mod = tmi_zero;
    }
    if (rhs == 2) {
        // lhs / 2
        result.div = tmi_shr(lhs, TMI_PASS(tmi_one));
        result.mod.low = (uint64_t)tmi_is_bit_set(lhs, 0);
        result.mod.high = 0;
    } else if (TMI_HIGH(lhs) == 0 && TMI_LOW(lhs) == rhs) {
        // lhs / lhs
        result.div = tmi_one;
        result.mod = tmi_zero;
    } else if (TMI_HIGH(lhs) == 0 && TMI_LOW(lhs) < rhs) {
        // (rhs - c) / rhs
        result.div = tmi_zero;
        result.mod = TMI_DEREF(lhs);
    } else {
        // Used algorithm is binary long division.
        result.div = tmi_zero;
        result.mod = tmi_zero;

        // lhs is guaranteed > 0 since we checked whether lhs <= rhs in the other cases and rhs cannot be 0.
        uint64_t i = tmi_fls(lhs);
        for (;;) {
            result.mod.low |= (uint64_t)tmi_is_bit_set(lhs, i);

            if (TMI_HIGH(result.mod) || TMI_LOW(result.mod) >= rhs) {
                result.mod = tmi_sub_u64(TMI_PASS(result.mod), rhs);
                result.div = tmi_set_bit(result.div, i);
            }
            if (!i) break;
            --i;

            // Inline shift one bit left
            result.mod.high = (result.mod.high << 1) | (result.mod.low >> 63);
            result.mod.low <<= 1;
        }
    }

    return result;
}

TMI_DEF tmi_uint128_t tmi_inc(tmi_uint128_t_arg v) {
    tmi_uint128_t result = tmi_internal_add64(TMI_LOW(v), 1);
    TMI_HIGH(result) = TMI_HIGH(v) + TMI_HIGH(result);
    return result;
}

TMI_DEF tmi_uint128_t tmi_dec(tmi_uint128_t_arg v) {
    tmi_uint128_t result = tmi_internal_sub64(TMI_LOW(v), 1);
    TMI_HIGH(result) = TMI_HIGH(v) - TMI_HIGH(result);
    return result;
}

TMI_DEF tm_bool tmi_lt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_HIGH(lhs) == TMI_HIGH(rhs)) ? (TMI_LOW(lhs) < TMI_LOW(rhs)) : (TMI_HIGH(lhs) < TMI_HIGH(rhs));
}
TMI_DEF tm_bool tmi_lte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_HIGH(lhs) == TMI_HIGH(rhs)) ? (TMI_LOW(lhs) <= TMI_LOW(rhs)) : (TMI_HIGH(lhs) < TMI_HIGH(rhs));
}
TMI_DEF tm_bool tmi_gt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_HIGH(lhs) == TMI_HIGH(rhs)) ? (TMI_LOW(lhs) > TMI_LOW(rhs)) : (TMI_HIGH(lhs) > TMI_HIGH(rhs));
}
TMI_DEF tm_bool tmi_gte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_HIGH(lhs) == TMI_HIGH(rhs)) ? (TMI_LOW(lhs) >= TMI_LOW(rhs)) : (TMI_HIGH(lhs) > TMI_HIGH(rhs));
}
TMI_DEF tm_bool tmi_eq(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return (TMI_LOW(lhs) == TMI_LOW(rhs)) && (TMI_HIGH(lhs) == TMI_HIGH(rhs));
}
TMI_DEF int tmi_cmp(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    if (TMI_HIGH(lhs) < TMI_HIGH(rhs)) return -1;
    if (TMI_HIGH(lhs) > TMI_HIGH(rhs)) return 1;
    if (TMI_LOW(lhs) < TMI_LOW(rhs)) return -1;
    if (TMI_LOW(lhs) > TMI_LOW(rhs)) return 1;
    return 0;
}

TMI_DEF tm_bool tmi_is_zero(tmi_uint128_t_arg v) { return (TMI_LOW(v) == 0) && (TMI_HIGH(v) == 0); }
TMI_DEF tm_bool tmi_is_not_zero(tmi_uint128_t_arg v) { return (TMI_LOW(v) != 0) || (TMI_HIGH(v) != 0); }
TMI_DEF tm_bool tmi_is_bit_set(tmi_uint128_t_arg v, uint64_t bit_position) {
    TM_ASSERT(bit_position < 128);
    if (bit_position >= 64) return (TMI_HIGH(v) & (1ull << (bit_position - 64))) != 0;
    return (TMI_LOW(v) & (1ull << bit_position)) != 0;
}

TMI_DEF uint64_t tmi_low(tmi_uint128_t_arg v) { return TMI_LOW(v); }
TMI_DEF uint64_t tmi_high(tmi_uint128_t_arg v) { return TMI_HIGH(v); }

TMI_DEF uint64_t tmi_fls(tmi_uint128_t_arg v) {
    if (TMI_HIGH(v)) return tmi_fls_u64(TMI_HIGH(v)) + 64;
    return tmi_fls_u64(TMI_LOW(v));
}
TMI_DEF uint64_t tmi_ffs(tmi_uint128_t_arg v) {
    if (TMI_LOW(v)) return tmi_ffs_u64(TMI_LOW(v));
    return tmi_ffs_u64(TMI_HIGH(v)) + 64;
}
TMI_DEF uint64_t tmi_popcount(tmi_uint128_t_arg v) {
    return tmi_popcount_u64(TMI_LOW(v)) + tmi_popcount_u64(TMI_HIGH(v));
}

TMI_DEF tmi_bytes tmi_to_bytes(tmi_uint128_t_arg v) {
    tmi_bytes result;
    result.entries[0] = (uint8_t)((TMI_LOW(v) >> 0) & 0xFF);
    result.entries[1] = (uint8_t)((TMI_LOW(v) >> 8) & 0xFF);
    result.entries[2] = (uint8_t)((TMI_LOW(v) >> 16) & 0xFF);
    result.entries[3] = (uint8_t)((TMI_LOW(v) >> 24) & 0xFF);
    result.entries[4] = (uint8_t)((TMI_LOW(v) >> 32) & 0xFF);
    result.entries[5] = (uint8_t)((TMI_LOW(v) >> 40) & 0xFF);
    result.entries[6] = (uint8_t)((TMI_LOW(v) >> 48) & 0xFF);
    result.entries[7] = (uint8_t)((TMI_LOW(v) >> 56) & 0xFF);

    result.entries[8] = (uint8_t)((TMI_HIGH(v) >> 0) & 0xFF);
    result.entries[9] = (uint8_t)((TMI_HIGH(v) >> 8) & 0xFF);
    result.entries[10] = (uint8_t)((TMI_HIGH(v) >> 16) & 0xFF);
    result.entries[11] = (uint8_t)((TMI_HIGH(v) >> 24) & 0xFF);
    result.entries[12] = (uint8_t)((TMI_HIGH(v) >> 32) & 0xFF);
    result.entries[13] = (uint8_t)((TMI_HIGH(v) >> 40) & 0xFF);
    result.entries[14] = (uint8_t)((TMI_HIGH(v) >> 48) & 0xFF);
    result.entries[15] = (uint8_t)((TMI_HIGH(v) >> 56) & 0xFF);
    return result;
}

const tmi_uint128_t tmi_one = {1, 0};
const tmi_uint128_t tmi_zero = {0, 0};
const tmi_uint128_t tmi_max = {0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull};

#ifdef TMI_NO_MUL128
// Multiplies two 32 Bit numbers together and returns result.
static tmi_uint128_t tmi_internal_mul64(uint64_t lhs, uint64_t rhs) {
    uint64_t lhs_low = lhs & 0xFFFFFFFFull;
    uint64_t lhs_high = lhs >> 32ull;
    uint64_t rhs_low = rhs & 0xFFFFFFFFull;
    uint64_t rhs_high = rhs >> 32ull;

    uint64_t low_low = lhs_low * rhs_low;
    uint64_t low_high = lhs_low * rhs_high + (low_low >> 32ull);
    uint64_t high_low = lhs_high * rhs_low;
    uint64_t high_high = lhs_high * rhs_high + (high_low >> 32ull);

    uint64_t a = low_low & 0xFFFFFFFFull;
    uint64_t b = low_high + (high_low & 0xFFFFFFFFull);
    uint64_t c = high_high + (b >> 32ull);
    b <<= 32ull;

    tmi_uint128_t result;
    result.low = a | b;
    result.high = c;
    return result;
}
#endif

