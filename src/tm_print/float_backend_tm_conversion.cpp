
static uint32_t tmp_convert_flags(uint32_t flags) {

#define tmp_version_mismatch_error              \
    "Internal Error: Incompatible print flags." \
    " Are tm_conversion.h and tm_print.h versions mismatched?"

    // Make sure that print flags are 1 to 1 compatible with the ones in tm_conversion.h.
    static_assert((uint32_t)PrintFlags::Fixed == (uint32_t)PF_FIXED, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Scientific == (uint32_t)PF_SCIENTIFIC, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Hex == (uint32_t)PF_HEX, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Shortest == (uint32_t)PF_SHORTEST, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::TrailingZeroes == (uint32_t)PF_TRAILING_ZEROES, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::BoolAsNumber == (uint32_t)PF_BOOL_AS_NUMBER, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Lowercase == (uint32_t)PF_LOWERCASE, tmp_version_mismatch_error);
    static_assert((uint32_t)PrintFlags::Sign == (uint32_t)PF_SIGNBIT, tmp_version_mismatch_error);
    static_assert(PF_COUNT == 8, tmp_version_mismatch_error);
#undef tmp_version_mismatch_error

    // Assuming that flags are 1 to 1 compatible with the ones in tm_conversion.h.
    return flags & ((1u << PF_COUNT) - 1);  // Mask out flags not defined in tm_conversion.
}

static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
    return print_double(dest, maxlen, value, tmp_convert_flags(format.flags), format.precision);
}
static PrintFormattedResult tmp_print_value(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
    return print_double(dest, maxlen, (double)value, tmp_convert_flags(format.flags), format.precision);
}