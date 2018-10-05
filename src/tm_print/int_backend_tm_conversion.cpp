static tm_size_t tmp_get_digits_count_decimal(uint64_t value) { return get_digits_count_decimal_u64(value); }
static tm_size_t tmp_get_digits_count_decimal(uint32_t value) { return get_digits_count_decimal_u32(value); }
static tm_size_t tmp_get_digits_count_hex(uint32_t value) { return get_digits_count_hex_u32(value); }
static tm_size_t tmp_get_digits_count_hex(uint64_t value) { return get_digits_count_hex_u64(value); }
static tm_size_t tmp_get_digits_count(uint32_t value, int32_t base) { return get_digits_count_u32(value, base); }
static tm_size_t tmp_get_digits_count(uint64_t value, int32_t base) { return get_digits_count_u64(value, base); }

template <class T>
PrintSizes tmp_get_print_sizes(T value, const PrintFormat& format, bool negative) {
    static_assert(std::is_unsigned<T>::value, "T has to be unsigned");

    PrintSizes result = {};

    switch (format.base) {
        case 10: {
            result.digits = tmp_get_digits_count_decimal(value);
            break;
        }
        case 16: {
            result.digits = tmp_get_digits_count_hex(value);
            break;
        }
        default: {
            result.digits = tmp_get_digits_count(value, format.base);
            break;
        }
    }

    result.decorated = tmp_get_decorated_size(result.digits, format, negative);
    result.size = result.decorated;
    if (format.width > 0 && result.size < (tm_size_t)format.width) {
        result.size = (tm_size_t)format.width;
    }

    TM_ASSERT(result.size >= result.decorated);
    TM_ASSERT(result.size >= result.digits);
    TM_ASSERT(result.decorated >= result.digits);
    return result;
}

static PrintFormattedResult tmp_print_decimal_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value) {
    return print_decimal_u32_w(dest, maxlen, width, value);
}
static PrintFormattedResult tmp_print_decimal_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value) {
    return print_decimal_u64_w(dest, maxlen, width, value);
}
static PrintFormattedResult tmp_print_hex_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value,
                                            bool lowercase) {
    return print_hex_u32_w(dest, maxlen, width, value, lowercase);
}
static PrintFormattedResult tmp_print_hex_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value,
                                            bool lowercase) {
    return print_hex_u64_w(dest, maxlen, width, value, lowercase);
}
static PrintFormattedResult tmp_print_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value, int32_t base,
                                        bool lowercase) {
    return print_u32_w(dest, maxlen, width, value, base, lowercase);
}
static PrintFormattedResult tmp_print_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value, int32_t base,
                                        bool lowercase) {
    return print_u64_w(dest, maxlen, width, value, base, lowercase);
}

template <class T>
static PrintFormattedResult print_formatted_unsigned(char* dest, tm_size_t maxlen, const PrintSizes& sizes,
                                                     const PrintFormat& format, T value, bool negative) {
    PrintFormattedResult result = {0, TM_OK};
    if (sizes.size > maxlen) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }
    result.size = sizes.size;

    tmp_print_decoration(dest, maxlen, sizes, format, negative);

    tm_size_t padding = 0;
    auto flags = format.flags;
    if (flags & FormatSpecifierFlags::LeftJustify) {
        padding = sizes.decorated - sizes.digits;
    } else {
        padding = sizes.size - sizes.digits;
    }
    dest += padding;
    auto remaining = maxlen - padding;
    PrintFormattedResult print_result = {};
    switch (format.base) {
        case 10: {
            print_result = tmp_print_decimal_w(dest, remaining, sizes.digits, value);
            break;
        }
        case 16: {
            bool lowercase = (flags & PrintFlags::Lowercase) != 0;
            print_result = tmp_print_hex_w(dest, remaining, sizes.digits, value, lowercase);
            break;
        }
        default: {
            bool lowercase = (flags & PrintFlags::Lowercase) != 0;
            print_result = tmp_print_w(dest, remaining, sizes.digits, value, format.base, lowercase);
            break;
        }
    }
    if (print_result.ec != TM_OK) {
        result.size = maxlen;
        result.ec = print_result.ec;
        return result;
    }

    return result;
}