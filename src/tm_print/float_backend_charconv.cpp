std::chars_format tmp_to_std_flags(unsigned flags) {
    std::chars_format result = std::chars_format::fixed;
    if ((flags & PrintFlags::General) == PrintFlags::General) {
        result = std::chars_format::fixed | std::chars_format::scientific;
    } else if (flags & PrintFlags::Scientific) {
        result = std::chars_format::scientific;
    } else if (flags & PrintFlags::Hex) {
        result = std::chars_format::hex;
    }
    return result;
}

static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    auto precision = format.precision;
    std::to_chars_result std_result = {};
    if (precision < 0) {
        std_result = std::to_chars(dest, dest + maxlen, value, tmp_to_std_flags(format.flags));
    } else {
        std_result = std::to_chars(dest, dest + maxlen, value, tmp_to_std_flags(format.flags), precision);
    }
    if (std_result.ec == std::errc{}) {
        return {(tm_size_t)(std_result.ptr - dest), TM_OK};
    }
    return {maxlen, TM_EOVERFLOW};
}
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    auto precision = format.precision;
    std::to_chars_result std_result = {};
    if (precision < 0) {
        std_result = std::to_chars(dest, dest + maxlen, value, tmp_to_std_flags(format.flags));
    } else {
        std_result = std::to_chars(dest, dest + maxlen, value, tmp_to_std_flags(format.flags), precision);
    }
    if (std_result.ec == std::errc{}) {
        return {(tm_size_t)(std_result.ptr - dest), TM_OK};
    }
    return {maxlen, TM_EOVERFLOW};
}