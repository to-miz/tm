#include "../common/tm_min.inc"

// clang-format off
#include "../common/tm_assert_valid_size.inc"

namespace {

struct PrintSizes {
    tm_size_t digits;
    tm_size_t decorated;
    tm_size_t size;
};

#if !defined(TMP_TM_CONVERSION_INCLUDED)
    struct PrintFormattedResult {
        tm_size_t size;
        tm_errc ec;
    };

    static PrintFormattedResult scan_u32_n(const char* str, tm_size_t maxlen, uint32_t* out, int32_t base) {
        TM_ASSERT(out);
        TM_ASSERT(base >= 2 && base <= 36);
        TM_ASSERT_VALID_SIZE(maxlen);

        PrintFormattedResult result = {maxlen, TM_EOVERFLOW};
        if (maxlen <= 0) {
            return result;
        }
        #if defined(TMP_CHARCONV_INCLUDED)
            auto std_result = std::from_chars(str, str + maxlen, *out, base);
            if(std_result.ec == std::errc{}) {
                result.size = (tm_size_t)(std_result.ptr - str);
                result.ec = TM_OK;
            }
        #elif defined(TMP_STRTOUL)
            char* endptr = nullptr;
            auto value = TMP_STRTOUL(str, &endptr, base);
            auto size = (tm_size_t)(endptr - str);
            if (endptr && size > 0 && size <= maxlen) {
                *out = value;
                result.size = size;
                result.ec = TM_OK;
            }
        #else
            #error "TMP_STRTOUL not defined"
        #endif
        return result;
    }
#else
    typedef tmc_conv_result PrintFormattedResult;

    template <class T>
    PrintSizes tmp_get_print_sizes(T value, const PrintFormat& format, bool negative);
#endif
// clang-format on

namespace FormatSpecifierFlags {
enum Values : unsigned int {
    LeftJustify = (1u << 8u),
    PoundSpecified = (1u << 9u),
    PrependHex = (1u << 10u),
    PrependBinary = (1u << 11u),
    PrependOctal = (1u << 12u),
    EmitDecimalPoint = (1u << 13u),
    IndexSpecified = (1u << 14u),
    WidthSpecified = (1u << 15u),
    PrecisionSpecified = (1u << 16u),
    PadWithSpaces = (1u << 17u),
};
}

static unsigned int tmp_parse_format_specifiers(const char* p, tm_size_t len, PrintFormat* format,
                                                uint32_t* currentIndex) {
    unsigned int result = 0;
    if (len != 0) {
        auto end = p + len;
        // parse what is inside {}

        auto scan_index_result = scan_u32_n(p, (tm_size_t)(end - p), currentIndex, 10);
        if (scan_index_result.ec == TM_OK) {
            p += scan_index_result.size;
            result |= FormatSpecifierFlags::IndexSpecified;
        }
        if (*p == ':') {
            ++p;
            // custom formatting was defined, so we reject the initial formatting
            *format = defaultPrintFormat();
        }
        // parse flags
        {
            // pad with spaces by default
            format->flags |= FormatSpecifierFlags::PadWithSpaces;

            bool parseFlags = true;
            do {
                switch (*p) {
                    case '-': {
                        result |= FormatSpecifierFlags::LeftJustify;
                        ++p;
                        break;
                    }
                    case '+': {
                        format->flags |= PrintFlags::Sign;
                        ++p;
                        break;
                    }
                    case ' ': {
                        format->flags |= FormatSpecifierFlags::PadWithSpaces;
                        ++p;
                        break;
                    }
                    case '#': {
                        result |= FormatSpecifierFlags::PoundSpecified;
                        ++p;
                        break;
                    }
                    case '0': {
                        format->flags &= ~FormatSpecifierFlags::PadWithSpaces;
                        ++p;
                        break;
                    }
                    default: {
                        parseFlags = false;
                        break;
                    }
                }
            } while (parseFlags);
        }

        // parse width
        unsigned width = 0;
        auto scan_width_result = scan_u32_n(p, (tm_size_t)(end - p), &width, 10);
        if (scan_width_result.ec == TM_OK) {
            p += scan_width_result.size;
            result |= FormatSpecifierFlags::WidthSpecified;
        }
        format->width = (int)width;

        // parse precision
        if (*p == '.') {
            ++p;
            unsigned precision = 0;
            auto scan_precision_result = scan_u32_n(p, (tm_size_t)(end - p), &precision, 10);
            if (scan_precision_result.ec == TM_OK) {
                p += scan_precision_result.size;
                result |= FormatSpecifierFlags::PrecisionSpecified;
            }
            format->precision = (int)precision;
        }

        switch (*p) {
            case 'x': {
                format->flags |= PrintFlags::Lowercase;
                format->base = 16;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependHex;
                }
                break;
            }
            case 'X': {
                format->base = 16;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependHex;
                }
                break;
            }
            case 'o': {
                format->base = 8;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependOctal;
                }
                break;
            }
            case 'b': {
                format->flags |= PrintFlags::Lowercase;
                format->base = 2;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependBinary;
                }
                break;
            }
            case 'B': {
                format->base = 2;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependBinary;
                }
                break;
            }
            case 'c': {
                format->flags |= PrintFlags::Char;
                break;
            }

            // floating point
            case 'e': {
                format->flags |= PrintFlags::Lowercase;
                format->flags |= PrintFlags::Scientific;
                break;
            }
            case 'E': {
                format->flags |= PrintFlags::Scientific;
                break;
            }
            case 'f': {
                format->flags |= PrintFlags::TrailingZeroes | PrintFlags::Fixed;
                break;
            }
            case 'g': {
                format->flags |= PrintFlags::Lowercase;
                format->flags |= PrintFlags::General;
                format->flags |= PrintFlags::Shortest;
                break;
            }
            case 'G': {
                format->flags |= PrintFlags::General;
                format->flags |= PrintFlags::Shortest;
                break;
            }
            case 'a': {
                format->flags |= PrintFlags::Lowercase;
                format->flags |= PrintFlags::Hex;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependHex;
                }
                break;
            }
            case 'A': {
                format->flags |= PrintFlags::Hex;
                if (result & FormatSpecifierFlags::PoundSpecified) {
                    result |= FormatSpecifierFlags::PrependHex;
                }
                break;
            }

            // non printf
            case 'n': {
                result |= PrintFlags::BoolAsNumber;
                break;
            }
        }
    }
    return result;
}

// TODO: this could be done better with dependency to type_traits
template <class T>
struct UnsignedPair {
    T value;
    bool negative;
};

static UnsignedPair<double> tmp_make_unsigned(double v) {
    bool negative = v < 0;
    if (negative) v = -v;
    return {v, negative};
}
static UnsignedPair<float> tmp_make_unsigned(float v) {
    bool negative = v < 0;
    if (negative) v = -v;
    return {v, negative};
}

#if !defined(TMP_INT_BACKEND_TM_CONVERSION)
static UnsignedPair<uint32_t> tmp_make_unsigned(int32_t v) {
    bool negative = v < 0;
    if (negative) v = -v;
    return {(uint32_t)v, negative};
}
static UnsignedPair<uint64_t> tmp_make_unsigned(int64_t v) {
    bool negative = v < 0;
    if (negative) v = -v;
    return {(uint64_t)v, negative};
}

static UnsignedPair<uint32_t> tmp_make_unsigned(uint32_t v) { return {v, false}; }
static UnsignedPair<uint64_t> tmp_make_unsigned(uint64_t v) { return {v, false}; }
#endif  // !defined(TMP_INT_BACKEND_TM_CONVERSION)

static PrintFormattedResult print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format, const char* str,
                                            tm_size_t str_len) {
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT_VALID_SIZE(str_len);

    const auto width = format.width;
    tm_size_t size = (width > 0 && (tm_size_t)width > str_len) ? width : str_len;
    if (size > maxlen) {
        return {maxlen, TM_EOVERFLOW};
    }

    auto remaining = maxlen;

    if (width > 0 && (tm_size_t)width > str_len) {
        tm_size_t padding = (tm_size_t)width - str_len;
        if (!(format.flags & FormatSpecifierFlags::LeftJustify)) {
            TM_ASSERT(padding <= remaining);
            TM_MEMSET(dest, ' ', (size_t)padding);
            dest += padding;
            remaining -= padding;
        } else {
            TM_ASSERT(padding + str_len <= maxlen);
            TM_MEMSET(dest + str_len, ' ', (size_t)padding);
        }
    }

    TM_ASSERT(str_len <= remaining);
    TM_MEMCPY(dest, str, (size_t)str_len);
    return {size, TM_OK};
}

static PrintFormattedResult print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format, bool value) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    auto flags = format.flags;
    if (flags & PrintFlags::BoolAsNumber) {
        return print_formatted(dest, maxlen, format, (value) ? "1" : "0", 1);
    }
    const bool lowercase = ((flags & PrintFlags::Lowercase) != 0);
    const char* str = nullptr;
    tm_size_t str_len = 0;
    if (value) {
        str = (lowercase) ? "true" : "TRUE";
        str_len = 4;
    } else {
        str = (lowercase) ? "false" : "FALSE";
        str_len = 5;
    }
    return print_formatted(dest, maxlen, format, str, str_len);
}

static tm_size_t tmp_get_decorated_size(tm_size_t digits, const PrintFormat& format, bool negative) {
    tm_size_t result = digits;
    if (negative || (format.flags & PrintFlags::Sign)) {
        ++result;
    }
    if ((format.flags & FormatSpecifierFlags::PrependHex) || (format.flags & FormatSpecifierFlags::PrependBinary)) {
        result += 2;
    }
    if (format.flags & FormatSpecifierFlags::PrependOctal) {
        result += 1;
    }
    return result;
}

static void tmp_print_decoration(char* dest, tm_size_t maxlen, const PrintSizes& sizes, const PrintFormat& format,
                                 bool negative) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(sizes.size <= maxlen);
    auto remaining = maxlen;

    char* decorate_pos = dest;
    const auto flags = format.flags;

    // padding
    if (format.width > 0 && sizes.size <= (tm_size_t)format.width) {
        if (!(flags & FormatSpecifierFlags::LeftJustify)) {
            auto padding = (tm_size_t)format.width - sizes.decorated;
            TM_ASSERT(padding <= maxlen);
            if (flags & FormatSpecifierFlags::PadWithSpaces) {
                decorate_pos = dest + padding;
            } else {
                TM_ASSERT(sizes.decorated >= sizes.digits);
                dest += sizes.decorated - sizes.digits;
            }
            TM_MEMSET(dest, (flags & FormatSpecifierFlags::PadWithSpaces) ? ' ' : '0', padding);
            dest += padding;
            remaining -= padding;
        } else {
            dest += sizes.decorated;
            TM_ASSERT((tm_size_t)format.width >= sizes.decorated);
            TM_MEMSET(dest, ' ', (tm_size_t)format.width - sizes.decorated);
        }
    } else {
        TM_ASSERT(sizes.decorated >= sizes.digits);
        dest += sizes.decorated - sizes.digits;
    }

    if (negative) {
        TM_ASSERT(remaining > 0);
        *decorate_pos++ = '-';
        --remaining;
    } else if (flags & PrintFlags::Sign) {
        TM_ASSERT(remaining > 0);
        *decorate_pos++ = '+';
        --remaining;
    }

    if (flags & FormatSpecifierFlags::PrependHex) {
        *decorate_pos++ = '0';
        *decorate_pos++ = (flags & PrintFlags::Lowercase) ? 'x' : 'X';
        remaining -= 2;
    } else if (flags & FormatSpecifierFlags::PrependBinary) {
        *decorate_pos++ = '0';
        *decorate_pos++ = (flags & PrintFlags::Lowercase) ? 'b' : 'B';
        remaining -= 2;
    } else if (flags & FormatSpecifierFlags::PrependOctal) {
        *decorate_pos++ = '0';
        --remaining;
    }
}

static PrintFormattedResult tmp_move_printed_value_and_decorate(char* dest, tm_size_t maxlen, const PrintFormat& format,
                                                                PrintFormattedResult printResult, bool negative) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    if (printResult.ec != TM_OK) return printResult;

    PrintSizes sizes = {};
    sizes.digits = printResult.size;
    sizes.decorated = tmp_get_decorated_size(sizes.digits, format, negative);
    sizes.size = sizes.decorated;

    auto width = format.width;
    if (width > 0 && sizes.size < (tm_size_t)width) {
        sizes.size = (tm_size_t)width;
    }
    TM_ASSERT(sizes.size >= sizes.decorated);
    TM_ASSERT(sizes.size >= sizes.digits);
    TM_ASSERT(sizes.decorated >= sizes.digits);

    tm_size_t digits_pos = 0;
    if (width <= 0 || (format.flags & FormatSpecifierFlags::LeftJustify)) {
        digits_pos = sizes.decorated - sizes.digits;
    } else {
        digits_pos = sizes.size - sizes.digits;
    }
    if (digits_pos > 0) {
        tm_size_t sign = negative || ((format.flags & PrintFlags::Sign) != 0);
        if (digits_pos != sign) {
            TM_MEMMOVE(dest + digits_pos, dest + sign, sizes.digits);
        }
    }

    tmp_print_decoration(dest, maxlen, sizes, format, negative);
    printResult.size = sizes.size;
    return printResult;
}

template <class T>
static PrintFormattedResult print_formatted(char* dest, tm_size_t maxlen, const PrintFormat& format, T value) {
    TM_ASSERT(dest || maxlen == 0);
    TM_ASSERT_VALID_SIZE(maxlen);

    if (maxlen <= 0 || (format.width > 0 && (tm_size_t)format.width > maxlen)) {
        return {maxlen, TM_EOVERFLOW};
    }

    auto pair = tmp_make_unsigned(value);
    bool sign = pair.negative || ((format.flags & PrintFlags::Sign) != 0);
    auto result = tmp_print(dest + sign, maxlen - sign, pair.value, format);
    return tmp_move_printed_value_and_decorate(dest, maxlen, format, result, pair.negative);
}

#ifdef TMP_INT_BACKEND_CRT
#include "int_backend_crt.cpp"
#endif  // defined(TMP_INT_BACKEND_CRT)

#ifdef TMP_INT_BACKEND_TM_CONVERSION
#include "int_backend_tm_conversion.cpp"
#endif  // defined(TMP_INT_BACKEND_TM_CONVERSION)

#ifdef TMP_INT_BACKEND_CHARCONV
#include "int_backend_charconv.cpp"
#endif  // defined(TMP_INT_BACKEND_CHARCONV)

#ifdef TMP_FLOAT_BACKEND_CRT
#include "float_backend_crt.cpp"
#endif  // defined(TMP_FLOAT_BACKEND_CRT)

#ifdef TMP_FLOAT_BACKEND_TM_CONVERSION
#include "float_backend_tm_conversion.cpp"
#endif  // defined(TMP_FLOAT_BACKEND_TM_CONVERSION)

#ifdef TMP_FLOAT_BACKEND_CHARCONV
#include "float_backend_charconv.cpp"
#endif  // defined(TMP_FLOAT_BACKEND_CHARCONV)

#include "memory_printer.cpp"

static const char* tmp_find(const char* first, const char* last, char c) {
    return (const char*)TM_MEMCHR(first, c, last - first);
}

static void tmp_print_impl(const char* format, size_t formatLen, const PrintFormat& initialFormatting,
                           const PrintArgList& args, tmp_memory_printer& printout) {
    // sanitize flags
    uint32_t format_flags = initialFormatting.flags & ((1u << PrintFlags::Count) - 1);

    const char* formatFirst = format;
    const char* formatLast = format + formatLen;
    auto index = 0u;
    const char* p = formatFirst;
    auto flags = args.flags;
    while (flags && (p = tmp_find(formatFirst, formatLast, '{')) != nullptr) {
        if (!printout(formatFirst, (tm_size_t)(p - formatFirst))) return;
        ++p;
        if (*p == '{') {
            if (!printout("{", 1)) return;
            ++p;
            formatFirst = p;
            continue;
        }

        // parse until '}'
        auto next = tmp_find(formatFirst, formatLast, '}');
        if (!next) {
            TM_ASSERT(0 && "illformed format");
            break;
        }

        PrintFormat printFormat = initialFormatting;
        printFormat.flags = format_flags;
        auto currentIndex = index;
        auto current = flags & PrintType::Mask;
        auto formatFlags = tmp_parse_format_specifiers(p, (tm_size_t)(next - p), &printFormat, &currentIndex);
        printFormat.flags |= formatFlags;

        if (!(formatFlags & FormatSpecifierFlags::IndexSpecified)) {
            ++index;
            flags >>= PrintType::Bits;
        } else {
            current = (args.flags >> (currentIndex * PrintType::Bits)) & PrintType::Mask;
        }
        formatFirst = next + 1;

        TM_ASSERT(currentIndex < args.size);
        if ((formatFlags & FormatSpecifierFlags::PrecisionSpecified) &&
            (current == PrintType::Int32 || current == PrintType::UInt32 || current == PrintType::Int64 ||
             current == PrintType::UInt64)) {
            if (formatFlags & FormatSpecifierFlags::WidthSpecified) {
                printFormat.width = TM_MIN(printFormat.precision, printFormat.width);
            } else {
                printFormat.width = printFormat.precision;
            }
        }
        if (!printout((int)current, args.args[currentIndex], printFormat)) return;
    }
    if (formatFirst < formatLast) {
        if (!printout(formatFirst, (tm_size_t)(formatLast - formatFirst))) return;
    }
}

}  // anonymous namespace

#ifndef TMP_NO_CRT_FILE_PRINTING
TMP_DEF tm_errc tmp_print(FILE* out, const char* format, const PrintArgList& args) {
    char sbo[TMP_SBO_SIZE];
    tmp_memory_printer printer = {sbo, 0, TMP_SBO_SIZE, true, false};

    tmp_print_impl(format, TM_STRLEN(format), defaultPrintFormat(), args, printer);
    fwrite(printer.data, sizeof(char), printer.size, out);
    return printer.ec;
}
#ifdef TM_STRING_VIEW
TMP_DEF tm_errc tmp_print(FILE* out, TM_STRING_VIEW format, const PrintArgList& args) {
    char sbo[TMP_SBO_SIZE];
    tmp_memory_printer printer = {sbo, 0, TMP_SBO_SIZE, true, false};

    tmp_print_impl(TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), defaultPrintFormat(), args, printer);
    fwrite(printer.data, sizeof(char), printer.size, out);
    return printer.ec;
}
#endif  // defined(TM_STRING_VIEW)
#endif  // !defined(TMP_NO_CRT_FILE_PRINTING)

TMP_DEF tm_size_t tmp_snprint(char* dest, tm_size_t len, const char* format, const PrintFormat& initialFormatting,
                              const PrintArgList& args) {
    tmp_memory_printer mem{dest, 0, len, false, false};
    tmp_print_impl(format, TM_STRLEN(format), initialFormatting, args, mem);
    return mem.size;
}
#ifdef TM_STRING_VIEW
TMP_DEF tm_size_t tmp_snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initialFormatting,
                              const PrintArgList& args) {
    tmp_memory_printer mem{dest, 0, len, false, false};
    tmp_print_impl(TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), initialFormatting, args, mem);
    return mem.size;
}
#endif  // defined(TM_STRING_VIEW)
