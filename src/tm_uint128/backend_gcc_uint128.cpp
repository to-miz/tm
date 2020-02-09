__extension__ typedef unsigned __int128 tmi_u128;

TMI_DEF tmi_uint128_t tmi_make(uint64_t low, uint64_t high) { return {(tmi_u128)low | (((tmi_u128)high) << 64)}; }

TMI_DEF tmi_uint128_t tmi_make_low(uint64_t low) { return {low}; }
TMI_DEF tmi_uint128_t tmi_make_high(uint64_t high) { return {((tmi_u128)high) << 64}; }
TMI_DEF tmi_uint128_t tmi_make_bitmask(uint64_t set_bit_position) { return {((tmi_u128)1) << set_bit_position}; }

TMI_DEF tmi_uint128_t tmi_add(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value + rhs.value}; }
TMI_DEF tmi_uint128_t tmi_add_u64(tmi_uint128_t_arg lhs, uint64_t rhs) { return {lhs.value + rhs}; }
TMI_DEF tmi_uint128_t tmi_sub(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value - rhs.value}; }
TMI_DEF tmi_uint128_t tmi_sub_u64(tmi_uint128_t_arg lhs, uint64_t rhs) { return {lhs.value - rhs}; }
TMI_DEF tmi_uint128_t tmi_mul(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value * rhs.value}; }
TMI_DEF tmi_uint128_t tmi_mul_u64(tmi_uint128_t_arg lhs, uint64_t rhs) { return {lhs.value * rhs}; }
TMI_DEF tmi_uint128_t tmi_div(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(rhs.value != 0);
    return {lhs.value / rhs.value};
}
TMI_DEF tmi_uint128_t tmi_mod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(rhs.value != 0);
    return {lhs.value % rhs.value};
}
TMI_DEF tmi_uint128_t tmi_and(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value & rhs.value}; }
TMI_DEF tmi_uint128_t tmi_or(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value | rhs.value}; }
TMI_DEF tmi_uint128_t tmi_xor(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return {lhs.value ^ rhs.value}; }
TMI_DEF tmi_uint128_t tmi_not(tmi_uint128_t_arg v) { return {~v.value}; }
TMI_DEF tmi_uint128_t tmi_shl(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(rhs.value < 128);
    return {lhs.value << rhs.value};
}
TMI_DEF tmi_uint128_t tmi_shr(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    TM_ASSERT(rhs.value < 128);
    return {lhs.value >> rhs.value};
}
TMI_DEF tmi_uint128_t tmi_shl_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
    TM_ASSERT(shift_amount < 128);
    return {lhs.value << shift_amount};
}
TMI_DEF tmi_uint128_t tmi_shr_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount) {
    TM_ASSERT(shift_amount < 128);
    return {lhs.value >> shift_amount};
}
TMI_DEF tmi_divmod_result tmi_divmod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    return {{lhs.value / rhs.value}, {lhs.value % rhs.value}};
}
TMI_DEF tmi_divmod_result tmi_divmod_u64(tmi_uint128_t_arg lhs, uint64_t rhs) {
    return {{lhs.value / rhs}, {lhs.value % rhs}};
}

TMI_DEF tmi_uint128_t tmi_inc(tmi_uint128_t_arg v) { return {v.value + 1}; }
TMI_DEF tmi_uint128_t tmi_dec(tmi_uint128_t_arg v) { return {v.value - 1}; }

TMI_DEF tm_bool tmi_lt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value < rhs.value; }
TMI_DEF tm_bool tmi_lte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value <= rhs.value; }
TMI_DEF tm_bool tmi_gt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value > rhs.value; }
TMI_DEF tm_bool tmi_gte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value >= rhs.value; }
TMI_DEF tm_bool tmi_eq(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) { return lhs.value == rhs.value; }
TMI_DEF int tmi_cmp(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs) {
    if (lhs.value < rhs.value) return -1;
    if (lhs.value > rhs.value) return 1;
    return 0;
}

TMI_DEF tm_bool tmi_is_zero(tmi_uint128_t_arg v) { return v.value == 0; }
TMI_DEF tm_bool tmi_is_not_zero(tmi_uint128_t_arg v) { return v.value != 0; }
TMI_DEF tm_bool tmi_is_bit_set(tmi_uint128_t_arg v, uint64_t bit_position) {
    TM_ASSERT(bit_position < 128);
    return (v.value & ((tmi_u128)1 << bit_position)) != 0;
}
TMI_DEF tmi_uint128_t_arg tmi_set_bit(tmi_uint128_t_arg v, uint64_t bit_position) {
    TM_ASSERT(bit_position < 128);
    return {v.value | ((tmi_u128)1 << bit_position)};
}

TMI_DEF uint64_t tmi_low(tmi_uint128_t_arg v) { return (uint64_t)v.value; }
TMI_DEF uint64_t tmi_high(tmi_uint128_t_arg v) { return (uint64_t)(v.value >> 64); }

TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
    TM_ASSERT(v);
    return 31 - __builtin_clz(v);
}
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
    TM_ASSERT(v);
    return __builtin_ctz(v);
}
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) { return __builtin_popcount(v); }
TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
    TM_ASSERT(v);
    return 63 - __builtin_clzll(v);
}
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
    TM_ASSERT(v);
    return __builtin_ctzll(v);
}
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) { return __builtin_popcountll(v); }

TMI_DEF uint64_t tmi_fls(tmi_uint128_t_arg v) {
    uint64_t high = tmi_high(v);
    if (high) return tmi_fls_u64(high) + 64;
    return tmi_fls_u64(tmi_low(v));
}
TMI_DEF uint64_t tmi_ffs(tmi_uint128_t_arg v) {
    uint64_t low = tmi_low(v);
    if (low) return tmi_ffs_u64(low);
    return tmi_ffs_u64(tmi_high(v)) + 64;
}
TMI_DEF uint64_t tmi_popcount(tmi_uint128_t_arg v) {
    return tmi_popcount_u64(tmi_low(v)) + tmi_popcount_u64(tmi_high(v));
}

TMI_DEF tmi_bytes tmi_to_bytes(tmi_uint128_t_arg v) {
    tmi_bytes result;
    result.entries[0] = (uint8_t)((v.value >> 0) & 0xFF);
    result.entries[1] = (uint8_t)((v.value >> 8) & 0xFF);
    result.entries[2] = (uint8_t)((v.value >> 16) & 0xFF);
    result.entries[3] = (uint8_t)((v.value >> 24) & 0xFF);
    result.entries[4] = (uint8_t)((v.value >> 32) & 0xFF);
    result.entries[5] = (uint8_t)((v.value >> 40) & 0xFF);
    result.entries[6] = (uint8_t)((v.value >> 48) & 0xFF);
    result.entries[7] = (uint8_t)((v.value >> 56) & 0xFF);

    result.entries[8] = (uint8_t)((v.value >> 64) & 0xFF);
    result.entries[9] = (uint8_t)((v.value >> 72) & 0xFF);
    result.entries[10] = (uint8_t)((v.value >> 80) & 0xFF);
    result.entries[11] = (uint8_t)((v.value >> 88) & 0xFF);
    result.entries[12] = (uint8_t)((v.value >> 96) & 0xFF);
    result.entries[13] = (uint8_t)((v.value >> 104) & 0xFF);
    result.entries[14] = (uint8_t)((v.value >> 112) & 0xFF);
    result.entries[15] = (uint8_t)((v.value >> 120) & 0xFF);
    return result;
}

const tmi_uint128_t tmi_one = {1};
const tmi_uint128_t tmi_zero = {0};
const tmi_uint128_t tmi_max = {~((tmi_u128)0)};