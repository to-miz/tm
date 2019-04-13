/* See SWITCHES for how to redefine this. */
#ifndef TMP_STRING_WIDTH
#define TMP_STRING_WIDTH(str, str_len) (int)(str_len)
#endif

static PrintFormattedResult tmp_get_print_size(char*, tm_size_t, bool value, const PrintFormat& format) {
    // '0' or '1' are just a single byte.
    if (format.flags & PrintFlags::BoolAsNumber) return {1, TM_OK};
    // The strings "true" and "false" are 4 and 5 bytes.
    return {(tm_size_t)((value) ? 4 : 5), TM_OK};
}

template <class T>
static PrintFormattedResult tmp_get_print_size(char* dest, tm_size_t maxlen, T value, const PrintFormat& format) {
    // FIXME: This will fail for floating point numbers that need more than TMP_SBO_SIZE characters when supplied dest
    // buffer isn't big enough. Should be very rare, but might be unexpected and lead to bugs.
    // Should we allocate memory in that case?

    char buffer[(TMP_SBO_SIZE)];
    if (maxlen < (tm_size_t)(TMP_SBO_SIZE)) {
        dest = buffer;
        maxlen = (tm_size_t)(TMP_SBO_SIZE);
    }
    return tmp_print_formatted(dest, maxlen, format, value);
}

struct tmp_memory_printer {
    char* data;
    tm_size_t size = 0;
    tm_size_t necessary = 0;
    tm_size_t capacity;
    bool can_grow;
    bool owns = false;
    tm_errc ec = TM_OK;

    tmp_allocator_context allocator;

    tmp_memory_printer(char* buffer, tm_size_t capacity, tmp_allocator_context allocator)
        : data(buffer), capacity(capacity), can_grow(true), allocator(allocator) {
        TM_ASSERT(allocator.reallocate);
        TM_ASSERT(allocator.destroy);
    }
    tmp_memory_printer(char* buffer, tm_size_t capacity) : data(buffer), capacity(capacity), can_grow(false) {}
    ~tmp_memory_printer() {
        if (owns) allocator.destroy(allocator.context, data, capacity);
    }
    bool grow(tm_size_t by_at_least = 0) {
        TM_ASSERT(can_grow);
        tm_size_t new_capacity = 3 * ((capacity + 2) / 2);
        if (new_capacity < capacity + by_at_least) new_capacity = capacity + by_at_least;

        char* new_data = nullptr;
        if (owns) {
            auto reallocate_result = allocator.reallocate(allocator.context, data, capacity, new_capacity);
            new_data = reallocate_result.ptr;
            new_capacity = reallocate_result.size;
        } else {
            auto reallocate_result = allocator.reallocate(allocator.context, nullptr, 0, new_capacity);
            new_data = reallocate_result.ptr;
            new_capacity = reallocate_result.size;

            if (new_data && data && size > 0) {
                TM_MEMCPY(new_data, data, size * sizeof(char));
            }
        }
        if (!new_data) {
            ec = TM_ENOMEM;
            return false;
        }
        data = new_data;
        capacity = new_capacity;
        owns = true;
        return true;
    }
    tm_size_t remaining() {
        TM_ASSERT(size <= capacity);
        return capacity - size;
    }
    char* end() { return data + size; }

    template <class T>
    bool print_value(T value, PrintFormat& format) {
        PrintFormattedResult result = {};
        if (ec == TM_OK) {
            for (;;) {
                result = tmp_print_formatted(end(), remaining(), format, value);
                if (result.ec == TM_OK) break;
                // TM_EOVERFLOW means there wasn't enough remaining size, but we don't know how much we need.
                // TM_ERANGE means there wasn't enough remaining size, and we know exactly how much we need.
                if (result.ec == TM_EOVERFLOW || result.ec == TM_ERANGE) {
                    if (!can_grow) {
                        if (result.ec == TM_ERANGE) {
                            ec = TM_ERANGE;
                            necessary += result.size;
                            return true;
                        }
                        // We break, so we can calculate the necessary size below.
                        break;
                    }
                    if (!grow((result.ec == TM_ERANGE) ? (result.size - remaining()) : 0)) return false;
                    continue;
                }
                // Any other error code is fatal.
                ec = result.ec;
                return false;
            }
            if (result.ec == TM_OK) {
                size += result.size;
                necessary += result.size;
            } else if (result.ec == TM_EOVERFLOW) {
                ec = TM_ERANGE;
            } else {
                ec = result.ec;
            }
        }

        // Error code might have changed, check again.
        if (ec == TM_ERANGE) {
            // Calculate necessary buffer size.
            auto necessary_size = tmp_get_print_size(data, capacity, value, format);
            if (necessary_size.ec != TM_OK) {
                ec = necessary_size.ec;
                return false;
            }
            necessary += necessary_size.size;
            return true;
        }
        return result.ec == TM_OK;
    }
    bool print_string_value(const char* str, tm_size_t str_len, PrintFormat& format) {
        tm_size_t str_width = str_len;
        tm_size_t bytes_needed = str_len;
        if (format.width > 0) {
            int string_width_result = TMP_STRING_WIDTH(str, str_len);
            if (string_width_result < 0) {
                ec = TM_EINVAL;
                return false;
            }
            str_width = (tm_size_t)string_width_result;
            tm_size_t width = ((tm_size_t)format.width > str_width) ? ((tm_size_t)format.width - str_width) : 0;
            bytes_needed = str_len + width;
        }

        necessary += bytes_needed;
        if (ec == TM_ERANGE) return true;

        if (bytes_needed > remaining()) {
            if (!can_grow) {
                ec = TM_ERANGE;
                return true;
            }
            if (!grow(bytes_needed - remaining())) return false;
        }
        auto print_result = tmp_print_formatted(end(), remaining(), format, str, str_len, str_width);
        if (print_result.ec == TM_OK) {
            size += print_result.size;
        } else {
            ec = print_result.ec;
            return false;
        }
        return true;
    }
#ifdef TMP_INT_BACKEND_TM_CONVERSION
    template <class T>
    bool print_unsigned(T value, PrintFormat& format, bool negative) {
        auto sizes = tmp_get_print_sizes(value, format, negative);
        necessary += sizes.size;
        if (sizes.size > remaining() && !(can_grow && grow(sizes.size))) {
            if (!can_grow) {
                ec = TM_ERANGE;
                return true;
            }
            return ec == TM_OK;
        }
        auto result = tmp_print_formatted_unsigned(end(), remaining(), sizes, format, value, negative);
        TM_ASSERT(result.ec == TM_OK);
        size += result.size;
        return result.ec == TM_OK;
    }
    bool print_value(int32_t value, PrintFormat& format) {
        bool negative = value < 0;
        if (negative) value = -value;
        return print_unsigned((uint32_t)value, format, negative);
    }
    bool print_value(int64_t value, PrintFormat& format) {
        bool negative = value < 0;
        if (negative) value = -value;
        return print_unsigned((uint64_t)value, format, negative);
    }
    bool print_value(uint32_t value, PrintFormat& format) { return print_unsigned(value, format, false); }
    bool print_value(uint64_t value, PrintFormat& format) { return print_unsigned(value, format, false); }
#endif  // defined(TMP_INT_BACKEND_TM_CONVERSION)

    bool operator()(int type, const PrintValue& value, PrintFormat& format) {
        switch (type) {
            case PrintType::Char: {
                if (format.flags & PrintFlags::Char) {
                    ++necessary;
                    if (remaining() <= 0) {
                        if (!can_grow) {
                            ec = TM_ERANGE;
                            return true;
                        }
                        if (!grow()) return false;
                    }
                    *end() = value.c;
                    ++size;
                    return true;
                } else {
                    return print_value((int32_t)value.c, format);
                }
            }
            case PrintType::Bool: {
                return print_value(value.b, format);
            }
            case PrintType::Int32: {
                return print_value(value.i32, format);
            }
            case PrintType::UInt32: {
                return print_value(value.u32, format);
            }
            case PrintType::Int64: {
                return print_value(value.i64, format);
            }
            case PrintType::UInt64: {
                return print_value(value.u64, format);
            }
            case PrintType::Float: {
#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
                auto flags = format.flags;
                format.flags |= PrintFlags::TrailingZeroes;
#endif
                bool result = print_value(value.f, format);
#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
                format.flags = flags;
#endif
                return result;
            }
            case PrintType::Double: {
#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
                auto flags = format.flags;
                format.flags |= PrintFlags::TrailingZeroes;
#endif
                bool result = print_value(value.d, format);
#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
                format.flags = flags;
#endif
                return result;
            }
            case PrintType::String: {
                TM_ASSERT(value.s);
                return print_string_value(value.s, (tm_size_t)TM_STRLEN(value.s), format);
            }
            case PrintType::StringView: {
                return print_string_value(value.v.data, (tm_size_t)value.v.size, format);
            }
#ifdef TMP_CUSTOM_PRINTING
            case PrintType::Custom: {
                auto print_size = value.custom.customPrint(end(), remaining(), format, value.custom.data);
                if (print_size < 0) {
                    ec = TM_EINVAL;
                    return false;
                }
                if (can_grow && (tm_size_t)print_size > remaining()) {
                    if (!grow(print_size)) return false;
                    print_size = value.custom.customPrint(end(), remaining(), format, value.custom.data);
                }
                necessary += (tm_size_t)print_size;
                if ((tm_size_t)print_size <= remaining()) {
                    size += print_size;
                } else {
                    ec = TM_ERANGE;
                }
                return true;
            }
#endif
            default: {
                TM_ASSERT(0 && "invalid code path");
                return false;
            }
        }
    }
    bool operator()(const char* str) { return operator()(str, (tm_size_t)TM_STRLEN(str)); }
    bool operator()(const char* str, tm_size_t len) {
        necessary += len;
        if (len > remaining()) {
            if (!can_grow) {
                ec = TM_ERANGE;
                return true;
            }
            if (!grow(len)) return false;
        }
        auto rem = remaining();
        auto print_size = TM_MIN(len, rem);
        TM_ASSERT_VALID_SIZE(print_size);
        if (str && len > 0) {
            TM_MEMCPY(end(), str, print_size * sizeof(char));
        }
        size += print_size;
        return true;
    }
};