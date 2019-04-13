static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, uint32_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    auto std_result = std::to_chars(dest, dest + maxlen, value, format.base);
    if(std_result.ec == std::errc{}) {
        if(!(format.flags & PrintFlags::Lowercase)) {
            for(char* p = dest, *end = dest + maxlen; p < end; ++p) {
                *p = (char)TMP_TOUPPER((unsigned char)*p);
            }
        }
        return {(tm_size_t)(std_result.ptr - dest), TM_OK};
    }
    return {maxlen, TM_EOVERFLOW};
}
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, uint64_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    auto std_result = std::to_chars(dest, dest + maxlen, value, format.base);
    if(std_result.ec == std::errc{}) {
        if(!(format.flags & PrintFlags::Lowercase)) {
            for(char* p = dest, *end = dest + maxlen; p < end; ++p) {
                *p = (char)TMP_TOUPPER((unsigned char)*p);
            }
        }
        return {(tm_size_t)(std_result.ptr - dest), TM_OK};
    }
    return {maxlen, TM_EOVERFLOW};
}