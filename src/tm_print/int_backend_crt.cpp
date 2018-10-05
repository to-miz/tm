static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, uint32_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    PrintFormattedResult result = {maxlen, TM_EOVERFLOW};
    const char* format_string = "%" PRIu32;
    if (format.base == 16) {
        format_string = (format.flags & PrintFlags::Lowercase) ? "%" PRIx32 : "%" PRIX32;
    } else if (format.base == 8) {
        format_string = "%" PRIo32;
    }
    auto size = TMP_SNPRINTF(dest, (size_t)maxlen, format_string, value);
    if (size > 0 && (tm_size_t)size <= maxlen) {
        result.size = (tm_size_t)size;
        result.ec = TM_OK;
    }
    return result;
}
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, uint64_t value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    PrintFormattedResult result = {};
    const char* format_string = "%" PRIu64;
    if (format.base == 16) {
        format_string = (format.flags & PrintFlags::Lowercase) ? "%" PRIx64 : "%" PRIX64;
    } else if (format.base == 8) {
        format_string = "%" PRIo64;
    }
    auto size = TMP_SNPRINTF(dest, (size_t)maxlen, format_string, value);
    if (size > 0 && (tm_size_t)size <= maxlen) {
        result.size = (tm_size_t)size;
        result.ec = TM_OK;
    }
    return result;
}