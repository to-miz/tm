static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, double value, const PrintFormat& format) {
    TM_ASSERT(!dest || maxlen > 0);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(format.base >= 2 && format.base <= 36);

    PrintFormattedResult result = {maxlen, TM_EOVERFLOW};
    auto precision = format.precision;
    char fmt_buffer[5];
    char* p = fmt_buffer;
    *p++ = '%';
    if (precision >= 0) {
        *p++ = '.';
        *p++ = '*';
    }
    *p = 'f';
    if ((format.flags & PrintFlags::General) == PrintFlags::General) {
        *p = (format.flags & PrintFlags::Lowercase) ? 'g' : 'G';
    } else if (format.flags & PrintFlags::Scientific) {
        *p = (format.flags & PrintFlags::Lowercase) ? 'e' : 'E';
    } else if (format.flags & PrintFlags::Hex) {
        *p = (format.flags & PrintFlags::Lowercase) ? 'a' : 'A';
    }
    ++p;
    *p = 0;
    TM_ASSERT(p < fmt_buffer + 5);

    int size = -1;
    if (precision < 0) {
        size = TMP_SNPRINTF(dest, maxlen, fmt_buffer, value);
    } else {
        size = TMP_SNPRINTF(dest, maxlen, fmt_buffer, precision, value);
    }
    if (size > 0 && (tm_size_t)size <= maxlen) {
        if (format.flags & PrintFlags::Hex) {
            // remove 0x prefix to make output same as other backends
            if (size > 2) {
                TM_ASSERT(dest[0] == '0');
                TM_ASSERT(dest[1] == 'x' || dest[1] == 'X');
                TM_MEMMOVE(dest, dest + 2, size - 2);
                size -= 2;
            }
        }
        result.size = (tm_size_t)size;
        result.ec = TM_OK;
    }
    return result;
}
static PrintFormattedResult tmp_print(char* dest, tm_size_t maxlen, float value, const PrintFormat& format) {
    return tmp_print(dest, maxlen, (double)value, format);
}