TMI_DEF int tmi_fls_s(tmi_uint128_t_arg v) {
    if (tmi_is_zero(v)) return -1;
    return (int)tmi_fls(v);
}
TMI_DEF int tmi_fls_u32_s(uint32_t v) {
    if (v == 0) return -1;
    return (int)tmi_fls_u32(v);
}
TMI_DEF int tmi_fls_u64_s(uint64_t v) {
    if (v == 0) return -1;
    return (int)tmi_fls_u64(v);
}
TMI_DEF int tmi_ffs_s(tmi_uint128_t_arg v) {
    if (tmi_is_zero(v)) return -1;
    return (int)tmi_ffs(v);
}
TMI_DEF int tmi_ffs_u32_s(uint32_t v) {
    if (v == 0) return -1;
    return (int)tmi_ffs_u32(v);
}
TMI_DEF int tmi_ffs_u64_s(uint64_t v) {
    if (v == 0) return -1;
    return (int)tmi_ffs_u64(v);
}

#ifndef TMI_NO_IEEE_754

#ifndef TMI_BACKEND_GCC_UINT128
TMI_DEF float tmi_to_float(tmi_uint128_t_arg v) { return tmi_to_float_rm(v, TM_FEGETROUND()); }
#else
TMI_DEF float tmi_to_float(tmi_uint128_t_arg v) { return (float)v.value; }
#endif

TMI_DEF float tmi_to_float_rm(tmi_uint128_t_arg v, int rounding_mode) {
    if (tmi_is_zero(v)) return 0.0f;

    uint64_t shift_amount = tmi_fls(v);
    tmi_uint128_t normalized = tmi_shl_u64(v, 127 - shift_amount);
    uint32_t exponent = 127 + (uint32_t)shift_amount;
    if (exponent >= 256) return FLT_MAX;  // TODO: FLT_MAX or INF?

    uint32_t mantissa = (uint32_t)(tmi_high(TMI_PASS(normalized)) >> 32ull);
    uint32_t round_up = 0;
    if (rounding_mode == TM_FE_TONEAREST) {
        /*
        Rounding.
        Rounding algorithm from https://www.exploringbinary.com/decimal-to-floating-point-needs-arbitrary-precision/

        If bit 25 is 0, round down
        If bit 25 is 1
            If any bit beyond bit 25 is 1, round up
            If all bits beyond bit 25 are 0 (meaning the number is halfway between two floating-point numbers)
                If bit 53 is 0, round down
                If bit 53 is 1, round up
        */
        // If bit 25 is 0, round down
        // If bit 25 is 1
        if (mantissa & (1 << 7u)) {
            // If any bit beyond bit 25 is 1, round up
            // If all bits beyond bit 25 are 0 (meaning the number is halfway between two floating-point numbers)
            round_up =
                (tmi_low(TMI_PASS(normalized)) != 0) | ((tmi_high(TMI_PASS(normalized)) & 0x17FFFFFFFFFull) != 0);
        }
    } else if (rounding_mode == TM_FE_UPWARD) {
        // Upward means that if any bit is 1 we round up.
        round_up = (tmi_low(TMI_PASS(normalized)) != 0) | ((tmi_high(TMI_PASS(normalized)) & 0x1FFFFFFFFFFull) != 0);
    } else {
        // TM_FE_TOWARDZERO and TM_FE_DOWNWARD are the same thing for unsigned values.
        // We just truncate in this case.
    }
    mantissa >>= 8u;        // Shift mantissa into right position
    mantissa += round_up;   // Apply rounding.
    mantissa &= 0x7FFFFFu;  // Mask off leading 1 bit.
    uint32_t bits = (exponent << 23u) | mantissa;

    // Type pruning via safe memcpy. This is defined behavior.
    float result = 0.0f;
    TM_ASSERT(sizeof(result) == sizeof(bits));
    TM_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

#ifndef TMI_BACKEND_GCC_UINT128
TMI_DEF double tmi_to_double(tmi_uint128_t_arg v) { return tmi_to_double_rm(v, TM_FEGETROUND()); }
#else
TMI_DEF double tmi_to_double(tmi_uint128_t_arg v) { return (double)v.value; }
#endif

TMI_DEF double tmi_to_double_rm(tmi_uint128_t_arg v, int rounding_mode) {
    if (tmi_is_zero(v)) return 0.0;

    uint64_t shift_amount = tmi_fls(v);
    tmi_uint128_t normalized = tmi_shl_u64(v, 127 - shift_amount);
    uint64_t exponent = 1023 + shift_amount;
    if (exponent >= 2048) return DBL_MAX;  // TODO: DBL_MAX or INF?

    uint64_t mantissa = tmi_high(TMI_PASS(normalized));
    uint64_t round_up = 0;
    if (rounding_mode == TM_FE_TONEAREST) {
        /*
        Rounding.
        Rounding algorithm from https://www.exploringbinary.com/decimal-to-floating-point-needs-arbitrary-precision/

        If bit 54 is 0, round down
        If bit 54 is 1
            If any bit beyond bit 54 is 1, round up
            If all bits beyond bit 54 are 0 (meaning the number is halfway between two floating-point numbers)
                If bit 53 is 0, round down
                If bit 53 is 1, round up
        */
        // If bit 54 is 0, round down
        // If bit 54 is 1
        if (mantissa & (1 << 10u)) {
            // If any bit beyond bit 54 is 1, round up
            // If all bits beyond bit 54 are 0 (meaning the number is halfway between two floating-point numbers)
            round_up = (tmi_low(TMI_PASS(normalized)) != 0) | ((mantissa & 0xBFFull) != 0);
        }
    } else if (rounding_mode == TM_FE_UPWARD) {
        // Upward means that if any bit is 1 we round up.
        round_up = (tmi_low(TMI_PASS(normalized)) != 0) | ((mantissa & 0x7FFull) != 0);
    } else {
        // TM_FE_TOWARDZERO and TM_FE_DOWNWARD are the same thing for unsigned values.
        // We just truncate in this case.
    }
    mantissa >>= 11u;                // Shift mantissa into right position
    mantissa += round_up;            // Apply rounding.
    mantissa &= 0xFFFFFFFFFFFFFull;  // Mask off leading 1 bit.
    uint64_t bits = (exponent << 52u) | mantissa;

    // Type pruning via safe memcpy. This is defined behavior.
    double result = 0.0;
    TM_ASSERT(sizeof(result) == sizeof(bits));
    TM_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

TMI_DEF tmi_uint128_t tmi_from_float(float v) {
    // Type pruning via safe memcpy. This is defined behavior.
    uint32_t bits = 0;
    TM_ASSERT(sizeof(v) == sizeof(bits));
    TM_MEMCPY(&bits, &v, sizeof(bits));

    uint32_t sign = (bits >> 31u);
    int32_t exponent = (int32_t)((bits >> 23) & 0x7FFull) - 127 - 23;
    if (exponent < -23) return tmi_zero;
    tmi_uint128_t result = tmi_make_low(bits & 0x7FFFFFu);
    result = tmi_set_bit(result, 23); // Set implicit 1 bit.
    if (exponent < 0) {
        result = tmi_shr_u64(TMI_PASS(result), (uint64_t)(-exponent));
    } else {
        result = tmi_shl_u64(TMI_PASS(result), (uint64_t)exponent);
    }
    if (sign) result = tmi_sub(TMI_PASS(tmi_max), TMI_PASS(result));
    return result;
}
TMI_DEF tmi_uint128_t tmi_from_double(double v) {
    // Type pruning via safe memcpy. This is defined behavior.
    uint64_t bits = 0;
    TM_ASSERT(sizeof(v) == sizeof(bits));
    TM_MEMCPY(&bits, &v, sizeof(bits));

    uint64_t sign = (bits >> 63u);
    int64_t exponent = (int64_t)((bits >> 52) & 0x7FFull) - 1023 - 52;
    if (exponent < -52) return tmi_zero;
    tmi_uint128_t result = tmi_make_low(bits & 0xFFFFFFFFFFFFFull);
    result = tmi_set_bit(result, 52); // Set implicit 1 bit.
    if (exponent < 0) {
        result = tmi_shr_u64(TMI_PASS(result), (uint64_t)(-exponent));
    } else {
        result = tmi_shl_u64(TMI_PASS(result), (uint64_t)exponent);
    }
    if (sign) result = tmi_sub(TMI_PASS(tmi_max), TMI_PASS(result));
    return result;
}

TMI_DEF tm_bool tmi_from_float_s(float v, tmi_uint128_t* out) {
    TM_ASSERT(out);
    if (TM_ISFINITE(v)) {
        *out = tmi_from_float(v);
        return TM_TRUE;
    }
    return TM_FALSE;
}
TMI_DEF tm_bool tmi_from_double_s(double v, tmi_uint128_t* out) {
    TM_ASSERT(out);
    if (TM_ISFINITE(v)) {
        *out = tmi_from_double(v);
        return TM_TRUE;
    }
    return TM_FALSE;
}

#endif