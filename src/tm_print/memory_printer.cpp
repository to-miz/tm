struct tmp_memory_printer {
    char* data;
    tm_size_t size;
    tm_size_t capacity;
    bool allowResize;
    bool owns;
    tm_errc ec = TM_OK;

    ~tmp_memory_printer() {
        if (owns) {
            TMP_FREE(data);
        }
    }
    bool grow(tm_size_t byAtLeast = 0) {
        TM_ASSERT(allowResize);
        tm_size_t newCapacity = 3 * ((capacity + 2) / 2);
        if (newCapacity < capacity + byAtLeast) {
            newCapacity = capacity + byAtLeast;
        }
        if (owns) {
            data = TMP_REALLOC(char, data, newCapacity);
            TM_ASSERT(data);
            if (data) {
                capacity = newCapacity;
                return true;
            }
        } else {
            char* newData = TMP_MALLOC(char, newCapacity);
            TM_ASSERT(newData);
            if (newData) {
                TM_MEMCPY(newData, data, size);
                data = newData;
                capacity = newCapacity;
                owns = true;
                return true;
            }
        }
        ec = TM_ENOMEM;
        return false;
    }
    tm_size_t remaining() {
        TM_ASSERT(size <= capacity);
        return capacity - size;
    }
    char* end() { return data + size; }

    template <class T>
    bool print_value(T value, PrintFormat& format) {
        PrintFormattedResult result = {};
        do {
            result = print_formatted(end(), remaining(), format, value);
        } while (allowResize && result.ec == TM_EOVERFLOW && grow());
        if (result.ec == TM_OK) {
            size += result.size;
        } else {
            ec = result.ec;
        }
        return result.ec == TM_OK;
    }
    bool print_string_value(const char* str, tm_size_t str_len, PrintFormat& format) {
        tm_size_t len = (format.width > 0 && (tm_size_t)format.width > str_len) ? ((tm_size_t)format.width) : (str_len);

        bool result = true;
        if (len > remaining()) result = grow(len);
        if (result) {
            auto print_result = print_formatted(end(), remaining(), format, str, str_len);
            if (print_result.ec == TM_OK) {
                size += print_result.size;
            } else {
                result = false;
                ec = print_result.ec;
            }
        }
        return result;
    }
#ifdef TMP_INT_BACKEND_TM_CONVERSION
    template <class T>
    bool print_unsigned(T value, PrintFormat& format, bool negative) {
        auto sizes = tmp_get_print_sizes(value, format, negative);
        if (sizes.size > remaining() && !(allowResize && grow(sizes.size))) {
            if (!allowResize) ec = TM_EOVERFLOW;
            return false;
        }
        auto result = print_formatted_unsigned(end(), remaining(), sizes, format, value, negative);
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
                    if (remaining() || (allowResize && grow())) {
                        *end() = value.c;
                        ++size;
                        return true;
                    }
                    return false;
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
                bool result = true;
                auto print_size = value.custom.customPrint(end(), remaining(), format, value.custom.data);
                if (allowResize && print_size >= remaining()) {
                    result = grow();
                    if (result) {
                        print_size = value.custom.customPrint(end(), remaining(), format, value.custom.data);
                    }
                }
                if (print_size <= remaining()) {
                    size += print_size;
                }
                return result;
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
        bool result = true;
        if (len > remaining()) {
            result = grow(len);
        }
        auto rem = remaining();
        auto printSize = TM_MIN(len, rem);
        TM_ASSERT_VALID_SIZE(printSize);
        TM_MEMCPY(end(), str, printSize);
        size += printSize;

        return result;
    }
};