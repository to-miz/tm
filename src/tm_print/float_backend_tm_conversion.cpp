static uint32_t tmp_convert_flags(uint32_t flags) {
    // assuming that flags are 1 to 1 compatible with the ones in tm_conversion.h
    return flags & ((1u << PF_COUNT) - 1);  // mask out flags not defined in tm_conversion
}

static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
    return print_double(dest, maxlen, value, tmp_convert_flags(format.flags), format.precision);
}
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
    return print_double(dest, maxlen, (double)value, tmp_convert_flags(format.flags), format.precision);
}