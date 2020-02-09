TMI_DEF tmi_conv_result tmi_scan_u128(const char* nullterminated, tmi_uint128_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    typedef tmi_uint128_t utype;
    const utype UMAX_VAL = tmi_max;

    if (!nullterminated) {
        tmi_conv_result result = {0, TM_EINVAL};
        return result;
    }
    const char* start = nullterminated;
    const char* p = nullterminated;

    const uint64_t ubase = (uint64_t)base;
    int rangeError = 0;
    const tmi_divmod_result divmod = tmi_divmod_u64(TMI_PASS(UMAX_VAL), ubase);
    const utype maxValue = divmod.div;
    const uint64_t maxDigit = tmi_low(TMI_PASS(divmod.mod));
    utype value = tmi_zero;
    for (; *p; ++p) {
        int32_t cp = (uint8_t)(*p);
        uint64_t digit;
        if (TM_ISDIGIT(cp)) {
            digit = (uint64_t)(cp - '0');
        } else if (TM_ISUPPER(cp)) {
            digit = (uint64_t)(cp - 'A' + 10);
        } else if (TM_ISLOWER(cp)) {
            digit = (uint64_t)(cp - 'a' + 10);
        } else {
            break;
        }
        if (digit >= ubase) break;
        if (rangeError || tmi_gt(TMI_PASS(value), TMI_PASS(maxValue)) ||
            (tmi_eq(TMI_PASS(value), TMI_PASS(maxValue)) && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = tmi_mul_u64(TMI_PASS(value), ubase);
            value = tmi_add_u64(TMI_PASS(value), digit);
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);
    tmi_conv_result result = {0, TM_EINVAL};

    /* Did we scan anything? */
    if (dist == 0) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = UMAX_VAL;
#endif
        } else {
            *out = value;
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMI_DEF tmi_conv_result tmi_scan_u128_n(const char* str, tm_size_t len, tmi_uint128_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    typedef tmi_uint128_t utype;
    const utype UMAX_VAL = tmi_max;

    tmi_conv_result result = {0, TM_EINVAL};
    if (len <= 0) return result;

    const char* start = str;
    const char* p = str;

    const uint64_t ubase = (uint64_t)base;
    int rangeError = 0;
    const tmi_divmod_result divmod = tmi_divmod_u64(TMI_PASS(UMAX_VAL), ubase);
    const utype maxValue = divmod.div;
    const uint64_t maxDigit = tmi_low(TMI_PASS(divmod.mod));
    utype value = tmi_zero;
    for (; len; ++p, --len) {
        int32_t cp = (uint8_t)(*p);
        uint64_t digit;
        if (TM_ISDIGIT(cp)) {
            digit = (uint64_t)(cp - '0');
        } else if (TM_ISUPPER(cp)) {
            digit = (uint64_t)(cp - 'A' + 10);
        } else if (TM_ISLOWER(cp)) {
            digit = (uint64_t)(cp - 'a' + 10);
        } else {
            break;
        }
        if (digit >= ubase) break;
        if (rangeError || tmi_gt(TMI_PASS(value), TMI_PASS(maxValue)) ||
            (tmi_eq(TMI_PASS(value), TMI_PASS(maxValue)) && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = tmi_mul_u64(TMI_PASS(value), ubase);
            value = tmi_add_u64(TMI_PASS(value), digit);
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);

    /* Did we scan anything? */
    if (dist == 0) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = UMAX_VAL;
#endif
        } else {
            *out = value;
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}

TMI_DEF tmi_conv_result tmi_print_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value, int32_t base,
                                       tm_bool lowercase) {
    return tmi_print_u128_w(dest, maxlen, tmi_get_digits_count_u128(value, base), value, base, lowercase);
}
TMI_DEF tm_size_t tmi_get_digits_count_u128(tmi_uint128_t number, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    tm_size_t result = 1;

    const uint64_t ubase = (uint64_t)base;
    const uint64_t base_pow_1 = ubase;
    const uint64_t base_pow_2 = base_pow_1 * ubase;
    const uint64_t base_pow_3 = base_pow_2 * ubase;
    const uint64_t base_pow_4 = base_pow_3 * ubase;

    for (;;) {
        if (tmi_high(number) == 0 && tmi_low(number) < base_pow_4) {
            uint64_t number_low = tmi_low(number);
            return result + (number_low >= base_pow_1) + (number_low >= base_pow_2) + (number_low >= base_pow_3);
        }
        number = tmi_divmod_u64(TMI_PASS(number), base_pow_4).div;
        result += 4;
    }
}

static const char print_NumberToCharTableUpper[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
                                                    'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                                    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
static const char print_NumberToCharTableLower[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
                                                    'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                                    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

TMI_DEF tmi_conv_result tmi_print_u128_w(char* dest, tm_size_t maxlen, tm_size_t width, tmi_uint128_t value,
                                         int32_t base, tm_bool lowercase) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen == 0);
    /* See comment on declaration for why width has to equal a specific value. */
    TM_ASSERT(width > 0);
    TM_ASSERT(width == tmi_get_digits_count_u128(value, base));

    tmi_conv_result result = {0, TM_OK};
    if (width > maxlen || width <= 0 || (tmi_is_zero(TMI_PASS(value)) && width != 1)) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }

    result.size = width;
    char* p = dest + width - 1;

    uint64_t ubase = (uint64_t)base;

    /* string conversion */
    const char* table = (lowercase) ? print_NumberToCharTableLower : print_NumberToCharTableUpper;
    if (tmi_is_zero(TMI_PASS(value))) {
        TM_ASSERT(width == 1);
        *p = '0';
    } else {
        TM_ASSERT(width > 0);
        do {
            tmi_divmod_result divmod = tmi_divmod_u64(TMI_PASS(value), ubase);
            TM_ASSERT(tmi_low(divmod.mod) <
                      sizeof(print_NumberToCharTableLower) / sizeof(print_NumberToCharTableLower[0]));
            *p-- = table[tmi_low(divmod.mod)];
            value = divmod.div;
            --width;
        } while (tmi_is_not_zero(TMI_PASS(value)) && width > 0);

        if (tmi_is_not_zero(value) && width <= 0) {
            result.size = maxlen;
            result.ec = TM_EOVERFLOW;
            return result;
        }
    }
    return result;
}

TMI_DEF tmi_uint128_t tmi_from_string(const char* nullterminated, int32_t base) {
    tmi_uint128_t result = tmi_zero;
    tmi_scan_u128(nullterminated, &result, base);
    return result;
}
TMI_DEF tmi_uint128_t tmi_from_string_n(const char* str, tm_size_t maxlen, int32_t base) {
    tmi_uint128_t result = tmi_zero;
    tmi_scan_u128_n(str, maxlen, &result, base);
    return result;
}