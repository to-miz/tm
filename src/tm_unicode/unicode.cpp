#define TMU_WIDEN(x) ((uint32_t)((uint8_t)(x)))
#define TMU_MAX_UTF32 0x10FFFFu
#define TMU_LEAD_SURROGATE_MIN 0xD800u
#define TMU_LEAD_SURROGATE_MAX 0xDBFFu
#define TMU_TRAILING_SURROGATE_MIN 0xDC00u
#define TMU_TRAILING_SURROGATE_MAX 0xDFFFu
#define TMU_SURROGATE_OFFSET (0x10000u - (0xD800u << 10u) - 0xDC00u)
#define TMU_INVALID_CODEPOINT 0xFFFFFFFFu

/* Byte order marks for all encodings we can decode. */
static const unsigned char tmu_utf8_bom[3] = {0xEF, 0xBB, 0xBF};
static const unsigned char tmu_utf16_be_bom[2] = {0xFE, 0xFF};
static const unsigned char tmu_utf16_le_bom[2] = {0xFF, 0xFE};
static const unsigned char tmu_utf32_be_bom[4] = {0x00, 0x00, 0xFE, 0xFF};
static const unsigned char tmu_utf32_le_bom[4] = {0xFF, 0xFE, 0x00, 0x00};

TMU_DEF tmu_utf8_stream tmu_utf8_make_stream(const char* str) {
    TM_ASSERT(str);
    tmu_utf8_stream result = {TM_NULL, TM_NULL};
    result.cur = str;
    result.end = str + TMU_STRLEN(str);
    return result;
}
TMU_DEF tmu_utf8_stream tmu_utf8_make_stream_n(const char* str, tm_size_t len) {
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len == 0);
    tmu_utf8_stream result = {TM_NULL, TM_NULL};
    result.cur = str;
    result.end = str + len;
    return result;
}

TMU_DEF tmu_utf8_output_stream tmu_utf8_make_output_stream(char* data, tm_size_t capacity) {
    TM_ASSERT_VALID_SIZE(capacity);
    TM_ASSERT(data || capacity == 0);

    tmu_utf8_output_stream stream = {TM_NULL, 0, 0, 0, TM_OK};
    stream.data = data;
    stream.capacity = capacity;
    return stream;
}
TMU_DEF tmu_utf8_output_stream tmu_utf8_make_output_stream_n(char* data, tm_size_t capacity, tm_size_t size) {
    TM_ASSERT_VALID_SIZE(size);
    TM_ASSERT_VALID_SIZE(capacity);
    TM_ASSERT(data || capacity == 0);
    TM_ASSERT(size <= capacity);

    tmu_utf8_output_stream stream = {TM_NULL, 0, 0, 0, TM_OK};
    stream.data = data;
    stream.size = size;
    stream.capacity = capacity;
    return stream;
}

TMU_DEF tmu_utf16_stream tmu_utf16_make_stream(const tmu_char16* str) {
    TM_ASSERT(str);
    tmu_utf16_stream result = {TM_NULL, TM_NULL};
    result.cur = str;
    result.end = str + TMU_CHAR16LEN(str);
    return result;
}

TMU_DEF tmu_utf16_stream tmu_utf16_make_stream_n(const tmu_char16* str, tm_size_t len) {
    TM_ASSERT(str || len == 0);
    tmu_utf16_stream result = {TM_NULL, TM_NULL};
    result.cur = str;
    result.end = str + len;
    return result;
}

TMU_DEF tm_bool tmu_is_valid_codepoint(uint32_t codepoint) {
    return codepoint <= TMU_MAX_UTF32 && (codepoint < TMU_LEAD_SURROGATE_MIN || codepoint > TMU_TRAILING_SURROGATE_MAX);
}
TMU_DEF tm_bool tmu_utf8_extract(tmu_utf8_stream* stream, uint32_t* codepoint_out) {
    TM_ASSERT(stream);
    TM_ASSERT(codepoint_out);

    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    const char* cur = stream->cur;
    ptrdiff_t remaining = stream->end - cur;
    if (remaining > 0) {
        uint32_t first = (uint8_t)cur[0];
        if (first < 0x80) {
            codepoint = first;
            cur += 1;
        } else if ((first >> 5) == 0x6) { /* 110xxxxx 10xxxxxx */
            /* 2 byte sequence */
            if (remaining >= 2) {
                uint32_t second = (uint8_t)cur[1];
                codepoint = ((first & 0x1F) << 6) | (second & 0x3F);
                cur += 2;
            }
        } else if ((first >> 4) == 0xE) { /* 1110xxxx 10xxxxxx 10xxxxxx */
            /* 3 byte sequence */
            if (remaining >= 3) {
                uint32_t second = (uint8_t)cur[1];
                uint32_t third = (uint8_t)cur[2];
                codepoint = ((first & 0xF) << 12) | ((second & 0x3F) << 6) | (third & 0x3F);
                cur += 3;
            }
        } else if ((first >> 3) == 0x1E) { /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            /* 4 byte sequence */
            if (remaining >= 4) {
                uint32_t second = (uint8_t)cur[1];
                uint32_t third = (uint8_t)cur[2];
                uint32_t fourth = (uint8_t)cur[3];
                codepoint = ((first & 0x7) << 18) | ((second & 0x3F) << 12) | ((third & 0x3F) << 6) | (fourth & 0x3f);
                cur += 4;
            }
        }
    }
    *codepoint_out = codepoint;
    tm_bool result = tmu_is_valid_codepoint(codepoint);
    /* Advance stream only if we could extract a valid codepoint, otherwise stream points to invalid codepoint. */
    if (result) stream->cur = cur;
    return result;
}
TMU_DEF tm_bool tmu_utf16_extract(tmu_utf16_stream* stream, uint32_t* codepoint_out) {
    TM_ASSERT(codepoint_out);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;

    const tmu_char16* cur = stream->cur;
    tmu_char16 const* const end = stream->end;
    if (cur != end) {
        uint32_t lead = (uint16_t)*cur;
        ++cur;

        /* Check for surrogate pair. */
        if (lead >= TMU_LEAD_SURROGATE_MIN && lead <= TMU_LEAD_SURROGATE_MAX) {
            if (cur != end) {
                uint32_t trail = (uint16_t)*cur;
                ++cur;
                if (trail >= TMU_TRAILING_SURROGATE_MIN && trail <= TMU_TRAILING_SURROGATE_MAX) {
                    codepoint = (lead << 10) + trail + TMU_SURROGATE_OFFSET;
                }
            }
        } else {
            codepoint = lead;
        }
    }
    *codepoint_out = codepoint;
    tm_bool result = tmu_is_valid_codepoint(codepoint);
    /* Advance stream only if we could extract a valid codepoint, otherwise stream points to invalid codepoint. */
    if (result) stream->cur = cur;
    return result;
}
TMU_DEF tm_size_t tmu_utf8_encode(uint32_t codepoint, char* out, tm_size_t out_len) {
    TM_ASSERT(out || out_len == 0);
    TM_ASSERT(tmu_is_valid_codepoint(codepoint));

    if (codepoint < 0x80) {
        /* 1 byte sequence */
        if (out_len < 1) return 1;
        out[0] = (char)(codepoint);
        return 1;
    } else if (codepoint < 0x800) {
        /* 2 byte sequence 110xxxxx 10xxxxxx */
        if (out_len < 2) return 2;
        out[0] = (char)(0xC0 | (uint8_t)(codepoint >> 6));
        out[1] = (char)(0x80 | (uint8_t)(codepoint & 0x3F));
        return 2;
    } else if (codepoint < 0x10000) {
        /* 3 byte sequence 1110xxxx 10xxxxxx 10xxxxxx */
        if (out_len < 3) return 3;
        out[0] = (char)(0xE0 | (uint8_t)(codepoint >> 12));
        out[1] = (char)(0x80 | ((uint8_t)(codepoint >> 6) & 0x3F));
        out[2] = (char)(0x80 | ((uint8_t)(codepoint & 0x3F)));
        return 3;
    } else {
        /* 4 byte sequence 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        if (out_len < 4) return 4;
        out[0] = (char)(0xF0 | ((uint8_t)(codepoint >> 18) & 0x7));
        out[1] = (char)(0x80 | ((uint8_t)(codepoint >> 12) & 0x3F));
        out[2] = (char)(0x80 | ((uint8_t)(codepoint >> 6) & 0x3F));
        out[3] = (char)(0x80 | ((uint8_t)(codepoint & 0x3F)));
        return 4;
    }
}
TMU_DEF tm_bool tmu_utf8_append(uint32_t codepoint, tmu_utf8_output_stream* stream) {
    TM_ASSERT(stream);
    TM_ASSERT(stream->data || stream->capacity == 0);
    TM_ASSERT(stream->size <= stream->capacity);

    tm_size_t remaining = stream->capacity - stream->size;
    tm_size_t size = tmu_utf8_encode(codepoint, stream->data + stream->size, stream->capacity - stream->size);
    stream->necessary += size;
    if (size > remaining) {
        stream->ec = TM_ERANGE;
        stream->size = stream->capacity;
    } else {
        stream->size += size;
    }
    return stream->ec == TM_OK;
}

TMU_DEF tm_size_t tmu_utf16_encode(uint32_t codepoint, tmu_char16* out, tm_size_t out_len) {
    TM_ASSERT(out || out_len == 0);
    TM_ASSERT(tmu_is_valid_codepoint(codepoint));
    if (codepoint <= 0xFFFFu) {
        if (out_len < 1) return 1;
        out[0] = (tmu_char16)codepoint;
        return 1;
    } else {
        if (out_len < 2) return 2;
        codepoint -= 0x10000u;
        out[0] = (tmu_char16)(TMU_LEAD_SURROGATE_MIN + (uint16_t)(codepoint >> 10u));
        out[1] = (tmu_char16)(TMU_TRAILING_SURROGATE_MIN + (uint16_t)(codepoint & 0x3FFu));
        return 2;
    }
}
TMU_DEF tm_bool tmu_utf16_append(uint32_t codepoint, tmu_utf16_output_stream* stream) {
    TM_ASSERT(stream);
    TM_ASSERT(stream->data || stream->capacity == 0);
    TM_ASSERT(stream->size <= stream->capacity);

    tm_size_t remaining = stream->capacity - stream->size;
    tm_size_t size = tmu_utf16_encode(codepoint, stream->data + stream->size, stream->capacity - stream->size);
    stream->necessary += size;
    if (size > remaining) {
        stream->ec = TM_ERANGE;
        stream->size = stream->capacity;
    } else {
        stream->size += size;
    }
    return stream->ec == TM_OK;
}

typedef struct {
    const char* cur;
    const char* end;
} tmu_byte_stream;

static uint16_t tmu_extract_u16_be(tmu_byte_stream* stream) {
    TM_ASSERT(stream->end - stream->cur >= (ptrdiff_t)sizeof(uint16_t));
    const char* cur = stream->cur;
    uint16_t result = (uint16_t)(TMU_WIDEN(cur[1]) << 0) | (TMU_WIDEN(cur[0]) << 8);
    stream->cur += sizeof(uint16_t);
    return result;
}
static uint16_t tmu_extract_u16_le(tmu_byte_stream* stream) {
    TM_ASSERT(stream->end - stream->cur >= (ptrdiff_t)sizeof(uint16_t));
    const char* cur = stream->cur;
    uint16_t result = (uint16_t)(TMU_WIDEN(cur[0]) << 0) | (TMU_WIDEN(cur[1]) << 8);
    stream->cur += sizeof(uint16_t);
    return result;
}

static uint32_t tmu_extract_u32_be(tmu_byte_stream* stream) {
    TM_ASSERT(stream->end - stream->cur >= (ptrdiff_t)sizeof(uint32_t));
    const char* cur = stream->cur;
    uint32_t result =
        (TMU_WIDEN(cur[3]) << 0) | (TMU_WIDEN(cur[2]) << 8) | (TMU_WIDEN(cur[1]) << 16) | (TMU_WIDEN(cur[0]) << 24);
    stream->cur += sizeof(uint32_t);
    return result;
}
static uint32_t tmu_extract_u32_le(tmu_byte_stream* stream) {
    TM_ASSERT(stream->end - stream->cur >= (ptrdiff_t)sizeof(uint32_t));
    const char* cur = stream->cur;
    uint32_t result =
        (TMU_WIDEN(cur[0]) << 0) | (TMU_WIDEN(cur[1]) << 8) | (TMU_WIDEN(cur[2]) << 16) | (TMU_WIDEN(cur[3]) << 24);
    stream->cur += sizeof(uint32_t);
    return result;
}

typedef struct {
    tmu_conversion_result conversion;
    char* data;
    tm_size_t necessary;
    tm_size_t capacity;
    tm_bool can_grow;
    tm_bool owns;
} tmu_conversion_output_stream;

static tmu_conversion_output_stream tmu_make_conversion_output_stream(char* buffer, tm_size_t buffer_len,
                                                                      tm_bool can_grow) {
    tmu_conversion_output_stream result;
    result.data = buffer;
    result.conversion.size = 0;
    result.necessary = 0;
    result.capacity = buffer_len;
    result.can_grow = can_grow;
    result.owns = TM_FALSE;
    result.conversion.ec = TM_OK;
    result.conversion.original_encoding = tmu_encoding_unknown;
    result.conversion.invalid_codepoints_encountered = TM_FALSE;
    return result;
}

static tm_bool tmu_alloc_output_stream(tmu_conversion_output_stream* out, tm_size_t capacity) {
    if (out->can_grow && out->capacity < capacity) {
        out->data = (char*)TMU_MALLOC(capacity * sizeof(char), sizeof(char));
        if (!out->data) {
            out->conversion.ec = TM_ENOMEM;
            return TM_FALSE;
        }
        out->conversion.size = 0;
        out->capacity = capacity;
        out->owns = TM_TRUE;
    }
    return TM_TRUE;
}

static void tmu_destroy_output(tmu_conversion_output_stream* stream) {
    if (stream) {
        if (stream->data && stream->owns) {
            TMU_FREE(stream->data, stream->capacity * sizeof(char), sizeof(char));
        }
        stream->data = TM_NULL;
        stream->conversion.size = 0;
        stream->capacity = 0;
        stream->owns = TM_FALSE;
    }
}

static tm_bool tmu_output_grow(tmu_conversion_output_stream* stream, tm_size_t by_at_least) {
    TM_ASSERT(stream->can_grow);
    TM_ASSERT(stream->conversion.size + by_at_least > stream->capacity);
    TM_ASSERT(stream->conversion.ec == TM_OK);

    tm_size_t new_capacity = stream->capacity + (stream->capacity / 2);
    if (new_capacity < stream->capacity + by_at_least) new_capacity = stream->capacity + by_at_least;
    char* new_data = TM_NULL;
    if (stream->data && stream->owns) {
        TM_ASSERT(stream->capacity > 0);
        new_data = (char*)TMU_REALLOC(stream->data, stream->capacity * sizeof(char), sizeof(char),
                                      new_capacity * sizeof(char), sizeof(char));
    } else {
        new_data = (char*)TMU_MALLOC(new_capacity * sizeof(char), sizeof(char));
        if (new_data && !stream->owns && stream->data && stream->conversion.size) {
            TMU_MEMCPY(new_data, stream->data, stream->conversion.size * sizeof(char));
        }
    }
    if (!new_data) {
        stream->conversion.ec = TM_ENOMEM;
        tmu_destroy_output(stream);
        return TM_FALSE;
    }
    stream->data = new_data;
    stream->capacity = new_capacity;
    stream->owns = TM_TRUE;
    return TM_TRUE;
}

static tm_bool tmu_output_append_codepoint(tmu_conversion_output_stream* out, uint32_t codepoint) {
    tm_size_t remaining = out->capacity - out->conversion.size;
    tm_size_t write_size = tmu_utf8_encode(codepoint, out->data + out->conversion.size, remaining);
    out->necessary += write_size;
    if (out->conversion.ec == TM_OK) {
        if (write_size > remaining) {
            /* If output stream can't grow, out of memory is not an error.
               We accumulate how much memory is necessary in that case. */
            if (!out->can_grow) {
                out->conversion.ec = TM_ERANGE;
                return TM_TRUE;
            }

            if (!tmu_output_grow(out, write_size)) return TM_FALSE;

            remaining = out->capacity - out->conversion.size;
            write_size = tmu_utf8_encode(codepoint, out->data + out->conversion.size, remaining);
            TM_ASSERT(write_size <= remaining);
        }
        out->conversion.size += write_size;
    }
    return TM_TRUE;
}

static tm_bool tmu_output_append_str(tmu_conversion_output_stream* out, const char* str, tm_size_t len) {
    TM_ASSERT(str && len > 0);

    tm_size_t remaining = out->capacity - out->conversion.size;
    out->necessary += len;
    if (out->conversion.ec == TM_OK) {
        if (remaining < len) {
            /* If output stream can't grow, out of memory is not an error.
               We accumulate how much memory is necessary in that case. */
            if (!out->can_grow) {
                out->conversion.ec = TM_ERANGE;
                return TM_TRUE;
            }

            if (!tmu_output_grow(out, len)) return TM_FALSE;
        }
        TM_ASSERT(len <= out->capacity - out->conversion.size);
        TMU_MEMCPY(out->data + out->conversion.size, str, len * sizeof(char));
        out->conversion.size += len;
    }
    return TM_TRUE;
}

static tm_bool tmu_output_replace_pos(tmu_conversion_output_stream* stream, tm_size_t start, tm_size_t end,
                                      const char* str, tm_size_t str_len) {
    tm_size_t size = stream->conversion.size;

    TM_ASSERT(stream);
    TM_ASSERT(str);
    TM_ASSERT(str_len > 0);
    TM_ASSERT(start <= size);
    TM_ASSERT(end <= size);
    TM_ASSERT_VALID_SIZE(start);
    TM_ASSERT(start <= end);

    if (start == size) return tmu_output_append_str(stream, str, str_len);

    tm_size_t len = start - end;
    ptrdiff_t diff = (ptrdiff_t)str_len - (ptrdiff_t)len;
    if (diff == 0) {
        /* Replacement string is as long as range to replace, just overwrite in that case. */
        TMU_MEMCPY(stream->data + start, str, str_len * sizeof(char));
        return TM_TRUE;
    }
    tm_size_t offset = 0;
    if (diff < 0) {
        /* Range to replace is longer than replacement string, we shrink by replacing. */
        offset = start + str_len;
    } else {
        /* Range to replace is shorter than replacement string, we grow by replacing. */
        tm_size_t grow_amount = (tm_size_t)diff;
        if (stream->capacity - size < grow_amount) {
            if (!stream->can_grow) return TM_FALSE;
            if (!tmu_output_grow(stream, grow_amount)) return TM_FALSE;
        }

        offset = end + grow_amount;
    }
    /* Make room for replacement. */
    if (end != size) {
        TMU_MEMMOVE(stream->data + offset, stream->data + end, (size - offset) * sizeof(char));
    }
    /* Copy replacement */
    TMU_MEMCPY(stream->data + start, str, str_len * sizeof(char));

    /* Adjust size of stream (growing or shrinking). */
    TM_ASSERT((ptrdiff_t)size + diff >= 0);
    stream->conversion.size = (tm_size_t)((ptrdiff_t)size + diff);
    return TM_TRUE;
}

static void tmu_convert_bytes_from_utf16(tmu_byte_stream bytes, uint16_t (*extract)(tmu_byte_stream*),
                                         tmu_validate validate, const char* replace_str, tm_size_t replace_str_len,
                                         tm_bool nullterminate, tmu_conversion_output_stream* out) {
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));
    /* Guessing how many utf8 octets we will need. */
    tm_size_t total_bytes_count = (tm_size_t)(bytes.end - bytes.cur);
    if (!tmu_alloc_output_stream(out, total_bytes_count + (total_bytes_count / 2))) {
        return;
    }

    while (bytes.end - bytes.cur >= (ptrdiff_t)sizeof(tmu_char16)) {
        uint32_t codepoint = TMU_INVALID_CODEPOINT;

        uint32_t lead = extract(&bytes);

        /* Check for surrogate pair. */
        if (lead >= TMU_LEAD_SURROGATE_MIN && lead <= TMU_LEAD_SURROGATE_MAX) {
            if (bytes.end - bytes.cur >= (ptrdiff_t)sizeof(tmu_char16)) {
                uint32_t trail = extract(&bytes);
                if (trail >= TMU_TRAILING_SURROGATE_MIN && trail <= TMU_TRAILING_SURROGATE_MAX) {
                    codepoint = (lead << 10) + trail + TMU_SURROGATE_OFFSET;
                }
            }
        } else {
            codepoint = lead;
        }

        if (!tmu_is_valid_codepoint(codepoint)) {
            out->conversion.invalid_codepoints_encountered = TM_TRUE;
            switch (validate) {
                case tmu_validate_skip: {
                    continue;
                }
                case tmu_validate_error: {
                    out->conversion.ec = TM_EINVAL;
                    tmu_destroy_output(out);
                    return;
                }
                case tmu_validate_replace: {
                    if (!tmu_output_append_str(out, replace_str, replace_str_len)) return;
                    continue;
                }
            }
            return;
        }

        if (!tmu_output_append_codepoint(out, codepoint)) return;
    }

    if (bytes.end - bytes.cur > 0 && out->conversion.ec == TM_OK) {
        /* There are remaining bytes in the byte stream that we couldn't convert. */
        out->conversion.ec = TM_EINVAL;
        tmu_destroy_output(out);
        return;
    }
    if (nullterminate && (out->conversion.ec == TM_OK || out->conversion.ec == TM_ERANGE)) {
        if (tmu_output_append_codepoint(out, 0)) {
            /* Don't count null-terminator towards returned size. */
            --out->conversion.size;
        } else {
            return;
        }
    }
}

static void tmu_convert_bytes_from_utf32(tmu_byte_stream bytes, uint32_t (*extract)(tmu_byte_stream*),
                                         tmu_validate validate, const char* replace_str, tm_size_t replace_str_len,
                                         tm_bool nullterminate, tmu_conversion_output_stream* out) {
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));
    /* Guessing how many utf8 octets we will need. */
    tm_size_t total_bytes_count = (tm_size_t)(bytes.end - bytes.cur);
    if (!tmu_alloc_output_stream(out, total_bytes_count + (total_bytes_count / 2))) {
        return;
    }

    while (bytes.end - bytes.cur >= (ptrdiff_t)sizeof(uint32_t)) {
        uint32_t codepoint = extract(&bytes);
        if (!tmu_is_valid_codepoint(codepoint)) {
            out->conversion.invalid_codepoints_encountered = TM_TRUE;
            switch (validate) {
                case tmu_validate_skip: {
                    continue;
                }
                case tmu_validate_error: {
                    out->conversion.ec = TM_EINVAL;
                    tmu_destroy_output(out);
                    return;
                }
                case tmu_validate_replace: {
                    if (!tmu_output_append_str(out, replace_str, replace_str_len)) return;
                    continue;
                }
            }
            return;
        }

        if (!tmu_output_append_codepoint(out, codepoint)) return;
    }

    if (bytes.end - bytes.cur > 0 && out->conversion.ec == TM_OK) {
        /* There are remaining bytes in the byte stream that we couldn't convert. */
        out->conversion.ec = TM_EINVAL;
        tmu_destroy_output(out);
        return;
    }
    if (nullterminate && (out->conversion.ec == TM_OK || out->conversion.ec == TM_ERANGE)) {
        if (tmu_output_append_codepoint(out, 0)) {
            /* Don't count null-terminator towards returned size. */
            --out->conversion.size;
        } else {
            return;
        }
    }
}

static tm_bool tmu_has_utf8_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 3 && (unsigned char)stream.cur[0] == tmu_utf8_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf8_bom[1] && (unsigned char)stream.cur[2] == tmu_utf8_bom[2];
}
static tm_bool tmu_has_utf32_be_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 4 && (unsigned char)stream.cur[0] == tmu_utf32_be_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf32_be_bom[1] && (unsigned char)stream.cur[2] == tmu_utf32_be_bom[2] &&
           (unsigned char)stream.cur[3] == tmu_utf32_be_bom[3];
}
static tm_bool tmu_has_utf32_le_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 4 && (unsigned char)stream.cur[0] == tmu_utf32_le_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf32_le_bom[1] && (unsigned char)stream.cur[2] == tmu_utf32_le_bom[2] &&
           (unsigned char)stream.cur[3] == tmu_utf32_le_bom[3];
}
static tm_bool tmu_has_utf16_be_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 2 && (unsigned char)stream.cur[0] == tmu_utf16_be_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf16_be_bom[1];
}
static tm_bool tmu_has_utf16_le_bom(tmu_byte_stream stream) {
    return (stream.end - stream.cur) > 2 && (unsigned char)stream.cur[0] == tmu_utf16_le_bom[0] &&
           (unsigned char)stream.cur[1] == tmu_utf16_le_bom[1];
}

static void tmu_output_replace_invalid_utf8(tmu_conversion_output_stream* stream, const char* replace_str,
                                            tm_size_t replace_str_len) {
    tm_size_t cur = 0;
    tm_size_t remaining = stream->conversion.size;
    tm_bool invalid_codepoints_encountered = TM_FALSE;
    while (remaining) {
        tm_size_t range = tmu_utf8_valid_range(stream->data + cur, remaining);

        if (range != remaining) {
            invalid_codepoints_encountered = TM_TRUE;
            if (!tmu_output_replace_pos(stream, cur + range, cur + range + 1, replace_str, replace_str_len)) {
                stream->conversion.ec = TM_ENOMEM;
                tmu_destroy_output(stream);
                break;
            }
            cur += replace_str_len;
        }
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    stream->conversion.invalid_codepoints_encountered = invalid_codepoints_encountered;
}

static void tmu_output_validate_inplace(tmu_conversion_output_stream* stream, tmu_validate validate,
                                        const char* replace_str, tm_size_t replace_str_len) {
    switch (validate) {
        case tmu_validate_skip: {
            tm_size_t new_size = tmu_utf8_skip_invalid(stream->data, stream->conversion.size);
            stream->conversion.invalid_codepoints_encountered = (new_size != stream->conversion.size);
            stream->conversion.size = new_size;
            break;
        }
        case tmu_validate_replace: {
            tmu_output_replace_invalid_utf8(stream, replace_str, replace_str_len);
            break;
        }
        case tmu_validate_error:
        default: {
            tm_size_t valid_range = tmu_utf8_valid_range(stream->data, stream->conversion.size);
            if (valid_range != stream->conversion.size) {
                stream->conversion.invalid_codepoints_encountered = TM_TRUE;
                stream->conversion.ec = TM_EINVAL;
            }
            stream->conversion.size = valid_range;
            break;
        }
    }
}

static tm_bool tmu_convert_bytes_to_utf8_internal(const void* input, tm_size_t input_len, tmu_encoding encoding,
                                                  tmu_validate validate, const char* replace_str,
                                                  tm_size_t replace_str_len, tm_bool nullterminate,
                                                  tmu_conversion_output_stream* out_stream) {
    TM_ASSERT(input || input_len == 0);
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));

    tmu_byte_stream bytes = {TM_NULL, TM_NULL};
    bytes.cur = (const char*)input;
    bytes.end = (const char*)input + input_len;

    if (bytes.cur == bytes.end) {
        if (nullterminate) {
            if (tmu_output_append_codepoint(out_stream, 0)) {
                /* Don't count null-terminator towards returned size. */
                --out_stream->conversion.size;
                return TM_TRUE;
            }

            out_stream->conversion.ec = TM_ERANGE;
            out_stream->necessary = 1;
            return TM_FALSE;
        }
        return TM_TRUE;
    }

    if (encoding == tmu_encoding_unknown) {
        tm_bool converted = TM_TRUE;

        /* Try to detect encoding by inspecting byte order mark. */
        if (tmu_has_utf8_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf8_bom;
            converted = TM_FALSE;
        } else if (tmu_has_utf32_be_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf32be_bom;

            bytes.cur += 4;
            tmu_convert_bytes_from_utf32(bytes, tmu_extract_u32_be, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
        } else if (tmu_has_utf32_le_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf32le_bom;

            bytes.cur += 4;
            tmu_convert_bytes_from_utf32(bytes, tmu_extract_u32_le, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
        } else if (tmu_has_utf16_be_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf16be_bom;

            bytes.cur += 2;
            tmu_convert_bytes_from_utf16(bytes, tmu_extract_u16_be, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
        } else if (tmu_has_utf16_le_bom(bytes)) {
            out_stream->conversion.original_encoding = tmu_encoding_utf16le_bom;

            bytes.cur += 2;
            tmu_convert_bytes_from_utf16(bytes, tmu_extract_u16_le, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
        } else {
            /* No encoding detected, assume utf8. */
            out_stream->conversion.original_encoding = tmu_encoding_utf8;
            converted = TM_FALSE;
        }

        return converted;
    }

    switch (encoding) {
        case tmu_encoding_utf8:
        case tmu_encoding_utf8_bom: {
            if (tmu_has_utf8_bom(bytes)) {
                out_stream->conversion.original_encoding = tmu_encoding_utf8_bom;
            } else if (encoding == tmu_encoding_utf8_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            } else {
                out_stream->conversion.original_encoding = tmu_encoding_utf8;
            }
            return TM_FALSE;
        }
        case tmu_encoding_utf32be:
        case tmu_encoding_utf32be_bom: {
            tmu_encoding original_encoding = tmu_encoding_utf32be;
            if (tmu_has_utf32_be_bom(bytes)) {
                /* Skip byte order mark. */
                bytes.cur += 4;
                original_encoding = tmu_encoding_utf32be_bom;
            } else if (encoding == tmu_encoding_utf32be_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            }
            tmu_convert_bytes_from_utf32(bytes, tmu_extract_u32_be, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
            out_stream->conversion.original_encoding = original_encoding;
            return out_stream->conversion.ec == TM_OK;
        }
        case tmu_encoding_utf32le:
        case tmu_encoding_utf32le_bom: {
            tmu_encoding original_encoding = tmu_encoding_utf32le;
            if (tmu_has_utf32_le_bom(bytes)) {
                /* Skip byte order mark. */
                bytes.cur += 4;
                original_encoding = tmu_encoding_utf32le_bom;
            } else if (encoding == tmu_encoding_utf32le_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            }
            tmu_convert_bytes_from_utf32(bytes, tmu_extract_u32_le, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
            out_stream->conversion.original_encoding = original_encoding;
            return out_stream->conversion.ec == TM_OK;
        }
        case tmu_encoding_utf16be:
        case tmu_encoding_utf16be_bom: {
            tmu_encoding original_encoding = tmu_encoding_utf16be;
            if (tmu_has_utf16_be_bom(bytes)) {
                /* Skip byte order mark. */
                bytes.cur += 2;
                original_encoding = tmu_encoding_utf16be_bom;
            } else if (encoding == tmu_encoding_utf16be_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            }
            tmu_convert_bytes_from_utf16(bytes, tmu_extract_u16_be, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
            out_stream->conversion.original_encoding = original_encoding;
            return out_stream->conversion.ec == TM_OK;
        }
        case tmu_encoding_utf16le:
        case tmu_encoding_utf16le_bom: {
            tmu_encoding original_encoding = tmu_encoding_utf16le;
            if (tmu_has_utf16_le_bom(bytes)) {
                /* Skip byte order mark. */
                bytes.cur += 2;
                original_encoding = tmu_encoding_utf16le_bom;
            } else if (encoding == tmu_encoding_utf16le_bom) {
                /* Byte order mark expected but not found, error out. */
                out_stream->conversion.ec = TM_EINVAL;
                return TM_FALSE;
            }
            tmu_convert_bytes_from_utf16(bytes, tmu_extract_u16_le, validate, replace_str, replace_str_len,
                                         nullterminate, out_stream);
            out_stream->conversion.original_encoding = original_encoding;
            return out_stream->conversion.ec == TM_OK;
        }
        default: {
            TM_ASSERT(0 && "Invalid encoding.");
            out_stream->conversion.ec = TM_EINVAL;
            return TM_FALSE;
        }
    }
}

TMU_DEF tmu_utf8_conversion_result tmu_utf8_convert_from_bytes_dynamic(tmu_contents* input, tmu_encoding encoding,
                                                                       tmu_validate validate, const char* replace_str,
                                                                       tm_size_t replace_str_len,
                                                                       tm_bool nullterminate) {
    TM_ASSERT(input);
    TM_ASSERT(input->size <= input->capacity);

    tmu_conversion_output_stream out_stream =
        tmu_make_conversion_output_stream(/*buffer=*/TM_NULL, /*buffer_len=*/0, /*can_grow=*/TM_TRUE);

    tm_bool converted = tmu_convert_bytes_to_utf8_internal(input->data, input->size, encoding, validate, replace_str,
                                                           replace_str_len, nullterminate, &out_stream);

    tmu_utf8_conversion_result result;
    result.contents.data = TM_NULL;
    result.contents.size = 0;
    result.contents.capacity = 0;

    if (out_stream.conversion.ec == TM_OK) {
        if (!converted) {
            /* Take ownership of input, since it already is in the encoding we want. */
            TM_ASSERT(out_stream.conversion.original_encoding == tmu_encoding_utf8 ||
                      out_stream.conversion.original_encoding == tmu_encoding_utf8_bom);
            TM_ASSERT(out_stream.data == TM_NULL);
            TM_ASSERT(out_stream.conversion.size == 0);
            TM_ASSERT(out_stream.capacity == 0);

            out_stream.data = input->data;
            out_stream.conversion.size = input->size;
            out_stream.capacity = input->capacity;
            out_stream.owns = TM_TRUE;

            /* Remove byte order mark if it exists. */
            tmu_byte_stream stream = {TM_NULL, TM_NULL};
            stream.cur = out_stream.data;
            stream.end = out_stream.data + out_stream.conversion.size;
            if (tmu_has_utf8_bom(stream)) {
                TMU_MEMMOVE(out_stream.data, out_stream.data + 3, out_stream.conversion.size - 3);
                out_stream.conversion.size -= 3;
            }

            /* Validate inplace. */
            tmu_output_validate_inplace(&out_stream, validate, replace_str, replace_str_len);

            if (nullterminate && (out_stream.conversion.ec == TM_OK || out_stream.conversion.ec == TM_ERANGE)) {
                if (tmu_output_append_codepoint(&out_stream, 0)) {
                    /* Don't count null-terminator towards size. */
                    --out_stream.conversion.size;
                }
            }

            if (out_stream.conversion.ec == TM_OK) {
                /* Zero out input, since we took ownership of its contents. */
                input->data = TM_NULL;
                input->size = 0;
                input->capacity = 0;
            } else {
                out_stream.data = TM_NULL;
                out_stream.conversion.size = 0;
                out_stream.capacity = 0;
            }
        }

        result.contents.data = out_stream.data;
        result.contents.size = out_stream.conversion.size;
        result.contents.capacity = out_stream.capacity;
    } else {
        TM_ASSERT(out_stream.data == TM_NULL);
        TM_ASSERT(out_stream.conversion.size == 0);
        TM_ASSERT(out_stream.capacity == 0);
    }
    result.ec = out_stream.conversion.ec;
    result.original_encoding = out_stream.conversion.original_encoding;
    result.invalid_codepoints_encountered = out_stream.conversion.invalid_codepoints_encountered;
    return result;
}

TMU_DEF tmu_conversion_result tmu_utf8_convert_from_bytes(const void* input, tm_size_t input_len, tmu_encoding encoding,
                                                          tmu_validate validate, const char* replace_str,
                                                          tm_size_t replace_str_len, tm_bool nullterminate, char* out,
                                                          tm_size_t out_len) {
    TM_ASSERT(input || input_len == 0);

    tmu_conversion_output_stream out_stream = tmu_make_conversion_output_stream(out, out_len, /*can_grow=*/TM_FALSE);

    tm_bool converted = tmu_convert_bytes_to_utf8_internal(input, input_len, encoding, validate, replace_str,
                                                           replace_str_len, nullterminate, &out_stream);

    if (out_stream.conversion.ec == TM_OK && !converted && input && input_len > 0) {
        /* No conversion took place because input is already in the encoding we want. */
        TM_ASSERT(out_stream.conversion.original_encoding == tmu_encoding_utf8 ||
                  out_stream.conversion.original_encoding == tmu_encoding_utf8_bom);

        tmu_byte_stream input_bytes = {TM_NULL, TM_NULL};
        input_bytes.cur = (const char*)input;
        input_bytes.end = (const char*)input + input_len;

        /* Skip byte order mark if it exists. */
        if (tmu_has_utf8_bom(input_bytes)) input_bytes.cur += 3;

        /* Copy input while validating, since it already is in the encoding we want. */
        while (input_bytes.cur != input_bytes.end) {
            tm_size_t remaining_bytes = (tm_size_t)(input_bytes.end - input_bytes.cur);
            tm_size_t valid_range = tmu_utf8_valid_range(input_bytes.cur, remaining_bytes);
            if (valid_range > 0) {
                tmu_output_append_str(&out_stream, input_bytes.cur, valid_range);
            }
            input_bytes.cur += valid_range;
            if (valid_range != remaining_bytes) {
                out_stream.conversion.invalid_codepoints_encountered = TM_TRUE;
                switch (validate) {
                    case tmu_validate_skip: {
                        TM_ASSERT(input_bytes.cur + 1 <= input_bytes.end);
                        ++input_bytes.cur; /* Skip invalid octet. */
                        break;
                    }
                    case tmu_validate_replace: {
                        tmu_output_append_str(&out_stream, replace_str, replace_str_len);
                        ++input_bytes.cur; /* Skip invalid octet. */
                        break;
                    }
                    case tmu_validate_error:
                    default: {
                        out_stream.conversion.ec = TM_EINVAL;
                        input_bytes.cur = input_bytes.end;
                        break;
                    }
                }
            }
        }

        if (nullterminate && (out_stream.conversion.ec == TM_OK || out_stream.conversion.ec == TM_ERANGE)) {
            if (tmu_output_append_codepoint(&out_stream, 0)) {
                /* Don't count null-terminator towards size. */
                --out_stream.conversion.size;
            }
        }
    }

    /* Report necessary size if conversion failed because out wasn't big enough. */
    if (out_stream.conversion.ec == TM_ERANGE) {
        out_stream.conversion.size = out_stream.necessary;
    }
    return out_stream.conversion;
}

TMU_DEF tm_size_t tmu_utf8_valid_range(const char* str, tm_size_t len) {
    /* Checking for legal utf-8 byte sequences according to
       https://www.unicode.org/versions/Unicode11.0.0/ch03.pdf
       Table 3-7.  Well-Formed UTF-8 Byte Sequences */

    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        uint32_t c0 = (uint32_t)((uint8_t)str[i]);
        if (c0 < 0x80u) {
            /* Codepoint: 00000000 0xxxxxxx
               Utf-8:              0xxxxxxx */
            --remaining;
        } else if ((c0 & 0xE0u) == 0xC0u) {
            /* Codepoint: 00000yyy yyxxxxxx
               Utf-8:     110yyyyy 10xxxxxx
               Overlong:  1100000y 10xxxxxx */

            if (remaining < 2) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            if ((c1 & 0xC0u) != 0x80u) return i; /* Invalid trail. */
            if ((c0 & 0xFEu) == 0xC0u) return i; /* Overlong. */
            remaining -= 2;
        } else if ((c0 & 0xF0u) == 0xE0u) {
            /* Codepoint: 00000000 zzzzyyyy yyxxxxxx
               Utf-8:     1110zzzz 10yyyyyy 10xxxxxx
               Overlong:  11100000 100yyyyy 10xxxxxx
               Surrogate: 11101101 101yyyyy 10xxxxxx */
            if (remaining < 3) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            uint32_t c2 = (uint32_t)((uint8_t)str[i + 2]);
            if ((c1 & 0xC0u) != 0x80u) return i;                /* Invalid trail. */
            if ((c2 & 0xC0u) != 0x80u) return i;                /* Invalid trail. */
            if (c0 == 0xE0u && (c1 & 0xE0u) == 0x80u) return i; /* Overlong. */
            if (c0 == 0xEDu && c1 > 0x9Fu) return i;            /* Surrogate. */

            remaining -= 3;
        } else if ((c0 & 0xF8u) == 0xF0u) {
            /* Codepoint: 00000000 000uuuuu zzzzyyyy yyxxxxxx
               Utf-8:     11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
               Overlong:  11110000 1000zzzz 10yyyyyy 10xxxxxx */
            if (remaining < 4) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            uint32_t c2 = (uint32_t)((uint8_t)str[i + 2]);
            uint32_t c3 = (uint32_t)((uint8_t)str[i + 3]);
            if ((c1 & 0xC0u) != 0x80u) return i;     /* Invalid trail. */
            if ((c2 & 0xC0u) != 0x80u) return i;     /* Invalid trail. */
            if ((c3 & 0xC0u) != 0x80u) return i;     /* Invalid trail. */
            if (c0 == 0xF0u && c1 < 0x90u) return i; /* Overlong. */
            if (c0 == 0xF4u && c1 > 0x8Fu) return i; /* Invalid codepoints. */
            if (c0 > 0xF4u) return i;                /* Invalid codepoints. */

            remaining -= 4;
        } else {
            return i;
        }
    }
    return len;
}

TMU_DEF tm_size_t tmu_utf8_skip_invalid(char* str, tm_size_t len) {
    char* cur = str;
    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        tm_size_t range = tmu_utf8_valid_range(cur, remaining);

        if (cur != str + i) TMU_MEMMOVE(cur, str + i, range * sizeof(char));
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    return (tm_size_t)(cur - str);
}

TMU_DEF tm_size_t tmu_utf16_valid_range(const tmu_char16* str, tm_size_t len) {
    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        uint32_t c0 = (uint32_t)str[i];
        if (c0 >= TMU_LEAD_SURROGATE_MIN && c0 <= TMU_LEAD_SURROGATE_MAX) {
            if (remaining < 2) return i;

            uint32_t c1 = (uint32_t)str[i + 1];
            if (c1 < TMU_TRAILING_SURROGATE_MIN || c1 > TMU_TRAILING_SURROGATE_MAX) return i;

            uint32_t codepoint = (c0 << 10) + c1 + TMU_SURROGATE_OFFSET;
            if (!tmu_is_valid_codepoint(codepoint)) return i;

            remaining -= 2;
        } else {
            if (c0 >= TMU_TRAILING_SURROGATE_MIN && c0 <= TMU_TRAILING_SURROGATE_MAX) return i;
            --remaining;
        }
    }
    return len;
}

TMU_DEF tm_size_t tmu_utf16_skip_invalid(tmu_char16* str, tm_size_t len) {
    tmu_char16* cur = str;
    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        tm_size_t range = tmu_utf16_valid_range(cur, remaining);

        if (cur != str + i) TMU_MEMMOVE(cur, str + i, range * sizeof(tmu_char16));
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    return (tm_size_t)(cur - str);
}

TMU_DEF tmu_conversion_result tmu_utf8_from_utf16(tmu_utf16_stream stream, char* out, tm_size_t out_len) {
    return tmu_utf8_from_utf16_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL, /*replace_str_len=*/0,
                                  /*nullterminate=*/TM_FALSE, out, out_len);
}
TMU_DEF tmu_conversion_result tmu_utf16_from_utf8(tmu_utf8_stream stream, tmu_char16* out, tm_size_t out_len) {
    return tmu_utf16_from_utf8_ex(stream, tmu_validate_error, /*replace_str=*/TM_NULL, /*replace_str_len=*/0,
                                  /*nullterminate=*/TM_FALSE, out, out_len);
}

TMU_DEF tmu_conversion_result tmu_utf8_from_utf16_ex(tmu_utf16_stream stream, tmu_validate validate,
                                                     const char* replace_str, tm_size_t replace_str_len,
                                                     tm_bool nullterminate, char* out, tm_size_t out_len) {
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));

    tmu_conversion_result result = {0, TM_OK, tmu_encoding_unknown, TM_FALSE};
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (stream.cur != stream.end) {
        if (!tmu_utf16_extract(&stream, &codepoint)) {
            result.invalid_codepoints_encountered = TM_TRUE;
            switch (validate) {
                case tmu_validate_skip: {
                    /* Advance stream once and try again. */
                    TM_ASSERT(stream.cur + 1 <= stream.end);
                    ++stream.cur;
                    continue;
                }
                case tmu_validate_replace: {
                    result.size += replace_str_len;
                    if (out_len < replace_str_len) {
                        result.ec = TM_ERANGE;
                        out = TM_NULL;
                        out_len = 0;
                    } else {
                        TMU_MEMCPY(out, replace_str, replace_str_len * sizeof(char));
                        out += replace_str_len;
                        out_len -= replace_str_len;
                    }
                    continue;
                }
                case tmu_validate_error:
                default: {
                    result.ec = TM_EINVAL;
                    stream.cur = stream.end;
                    continue;
                }
            }
        }

        tm_size_t size = tmu_utf8_encode(codepoint, out, out_len);
        result.size += size;
        if (out_len < size) {
            result.ec = TM_ERANGE;
            out = TM_NULL;
            out_len = 0;
        } else {
            out += size;
            out_len -= size;
        }
    }

    if (nullterminate && (result.ec == TM_OK || result.ec == TM_ERANGE)) {
        tm_size_t size = tmu_utf8_encode(0, out, out_len);
        if (out_len < size) {
            result.size += size; /* Only count null-terminator towards size on overflow. */
            result.ec = TM_ERANGE;
            out = TM_NULL;
            out_len = 0;
        } else {
            out += size;
            out_len -= size;
        }
    }
    return result;
}
TMU_DEF tmu_conversion_result tmu_utf16_from_utf8_ex(tmu_utf8_stream stream, tmu_validate validate,
                                                     const tmu_char16* replace_str, tm_size_t replace_str_len,
                                                     tm_bool nullterminate, tmu_char16* out, tm_size_t out_len) {
    TM_ASSERT(validate != tmu_validate_replace || (replace_str && replace_str_len > 0));

    tmu_conversion_result result = {0, TM_OK, tmu_encoding_unknown, TM_FALSE};
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (stream.cur != stream.end) {
        if (!tmu_utf8_extract(&stream, &codepoint)) {
            result.invalid_codepoints_encountered = TM_TRUE;
            switch (validate) {
                case tmu_validate_skip: {
                    /* Advance stream once and try again. */
                    TM_ASSERT(stream.cur + 1 <= stream.end);
                    ++stream.cur;
                    continue;
                }
                case tmu_validate_replace: {
                    result.size += replace_str_len;
                    if (out_len < replace_str_len) {
                        result.ec = TM_ERANGE;
                        out = TM_NULL;
                        out_len = 0;
                    } else {
                        TMU_MEMCPY(out, replace_str, replace_str_len * sizeof(tmu_char16));
                        out += replace_str_len;
                        out_len -= replace_str_len;
                    }
                    continue;
                }
                case tmu_validate_error:
                default: {
                    result.ec = TM_EINVAL;
                    stream.cur = stream.end;
                    continue;
                }
            }
        }

        tm_size_t size = tmu_utf16_encode(codepoint, out, out_len);
        result.size += size;
        if (out_len < size) {
            result.ec = TM_ERANGE;
            out = TM_NULL;
            out_len = 0;
        } else {
            out += size;
            out_len -= size;
        }
    }

    if (nullterminate && (result.ec == TM_OK || result.ec == TM_ERANGE)) {
        tm_size_t size = tmu_utf16_encode(0, out, out_len);
        if (out_len < size) {
            result.size += size; /* Only count null-terminator towards size on overflow. */
            result.ec = TM_ERANGE;
            out = TM_NULL;
            out_len = 0;
        } else {
            out += size;
            out_len -= size;
        }
    }
    return result;
}

TMU_DEF tm_size_t tmu_utf8_copy_truncated(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    TM_ASSERT(str || str_len == 0);
    TM_ASSERT(out || out_len == 0);

    if (str_len <= out_len) {
        /* There is enough room in out, no need to truncate. */
        TMU_MEMCPY(out, str, str_len * sizeof(char));
        return str_len;
    }

    if (out_len > 0) {
        /* Retreat until we find the start of a utf8 sequence. */
        if ((uint8_t)str[out_len - 1] >= 0x80u) {
            tm_size_t cur = out_len - 1;
            while (cur > 0 && ((uint8_t)str[cur] & 0xC0u) == 0x80u) {
                --cur;
            }
            /* We found the start of a uf8 sequence, test if we can extract a valid codepoint. */
            tm_size_t distance = out_len - cur;
            if (tmu_utf8_valid_range(str + cur, distance) != distance) {
                /* We couldn't extract a valid codepoint, truncate so that the utf8 sequence isn't part of out. */
                out_len = cur;
            }
        }
        TMU_MEMCPY(out, str, out_len * sizeof(char));
    }
    return out_len;
}
TMU_DEF tm_size_t tmu_utf8_copy_truncated_stream(tmu_utf8_stream stream, char* out, tm_size_t out_len) {
    TM_ASSERT(stream.cur <= stream.end);
    return tmu_utf8_copy_truncated(stream.cur, (tm_size_t)(stream.end - stream.cur), out, out_len);
}

TMU_DEF tm_bool tmu_utf8_equals(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    /* We assume that both streams are normalized,
       so both an early exit through length comparison and memcmp are valid. */
    if (a_len != b_len) return TM_FALSE;
    if (a == b) return TM_TRUE;

    return TMU_MEMCMP(a, b, a_len * sizeof(char)) == 0;
}
TMU_DEF int tmu_utf8_compare(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    /* We assume that both streams are normalized, so that a memcmp is valid. */
    if (!a_len || !b_len) return !b_len - !a_len;
    if (a == b) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    tm_size_t len = (a_len < b_len) ? a_len : b_len;
    if (len) {
        int common_cmp = TMU_MEMCMP(a, b, len * sizeof(char));
        if (common_cmp != 0) return common_cmp;
    }

    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    return 0;
}

TMU_DEF tm_size_t tmu_utf8_count_codepoints(const char* str) {
    return tmu_utf8_count_codepoints_stream(tmu_utf8_make_stream(str));
}
TMU_DEF tm_size_t tmu_utf8_count_codepoints_n(const char* str, tm_size_t str_len) {
    return tmu_utf8_count_codepoints_stream(tmu_utf8_make_stream_n(str, str_len));
}
TMU_DEF tm_size_t tmu_utf8_count_codepoints_stream(tmu_utf8_stream stream) {
    TM_ASSERT(stream.cur <= stream.end);
    tm_size_t result = 0;
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) ++result;
    return result;
}

#if TMU_UCD_HAS_CATEGORY
TMU_DEF tm_bool tmu_is_control(uint32_t codepoint) {
    return tmu_ucd_get_category(codepoint) == tmu_ucd_category_control;
}
TMU_DEF tm_bool tmu_is_letter(uint32_t codepoint) { return tmu_ucd_get_category(codepoint) == tmu_ucd_category_letter; }
TMU_DEF tm_bool tmu_is_mark(uint32_t codepoint) { return tmu_ucd_get_category(codepoint) == tmu_ucd_category_mark; }
TMU_DEF tm_bool tmu_is_number(uint32_t codepoint) { return tmu_ucd_get_category(codepoint) == tmu_ucd_category_number; }
TMU_DEF tm_bool tmu_is_punctuation(uint32_t codepoint) {
    return tmu_ucd_get_category(codepoint) == tmu_ucd_category_punctuation;
}
TMU_DEF tm_bool tmu_is_symbol(uint32_t codepoint) { return tmu_ucd_get_category(codepoint) == tmu_ucd_category_symbol; }
TMU_DEF tm_bool tmu_is_separator(uint32_t codepoint) {
    return tmu_ucd_get_category(codepoint) == tmu_ucd_category_separator;
}
TMU_DEF tm_bool tmu_is_whitespace(uint32_t codepoint) { return tmu_ucd_is_whitespace(codepoint) == 1; }
#endif /* TMU_UCD_HAS_CATEGORY */

#if TMU_UCD_HAS_CASE_INFO
TMU_DEF tm_bool tmu_is_upper(uint32_t codepoint) { return tmu_ucd_get_case_info(codepoint) == tmu_ucd_case_upper; }
TMU_DEF tm_bool tmu_is_lower(uint32_t codepoint) { return tmu_ucd_get_case_info(codepoint) == tmu_ucd_case_lower; }
TMU_DEF tm_bool tmu_is_title(uint32_t codepoint) { return tmu_ucd_get_case_info(codepoint) == tmu_ucd_case_title; }
TMU_DEF tm_bool tmu_is_caseless(uint32_t codepoint) {
    return tmu_ucd_get_case_info(codepoint) == tmu_ucd_case_caseless;
}
#endif /* TMU_UCD_HAS_CATEGORY */

#if TMU_UCD_HAS_WIDTH
TMU_DEF int tmu_utf8_width(tmu_utf8_stream stream) {
    TM_ASSERT(stream.cur <= stream.end);
    int result = 0;
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        result += tmu_get_ucd_width(codepoint);
    }
    return result;
}
TMU_DEF int tmu_utf8_width_n(const char* str, tm_size_t str_len) {
    TM_ASSERT(str || str_len == 0);
    tmu_utf8_stream stream = {TM_NULL, TM_NULL};
    stream.cur = str;
    stream.cur = str + str_len;
    return tmu_utf8_width(stream);
}
#endif

typedef struct {
    char* data;
    tm_size_t size;
    tm_size_t capacity;
    tmu_transform_result result;
} tmu_transform_output_stream;

static void tmu_transform_output_append_codepoint(uint32_t codepoint, tmu_transform_output_stream* out) {
    tm_size_t out_size = out->size;
    tm_size_t remaining = out->capacity - out_size;
    tm_size_t size = tmu_utf8_encode(codepoint, out->data + out_size, remaining);
    out->result.size += size;
    if (size > remaining) {
        out->result.ec = TM_ERANGE;
        out->data = TM_NULL;
        out->size = 0;
        out->capacity = 0;
    } else {
        out->size += size;
    }
}

#if TMU_UCD_HAS_SIMPLE_CASE
TMU_DEF tmu_transform_result tmu_utf8_to_upper_simple(const char* str, tm_size_t str_len, char* out,
                                                      tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_upper_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tmu_transform_result tmu_utf8_to_title_simple(const char* str, tm_size_t str_len, char* out,
                                                      tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_title_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tmu_transform_result tmu_utf8_to_lower_simple(const char* str, tm_size_t str_len, char* out,
                                                      tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_lower_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
#endif /* TMU_UCD_HAS_SIMPLE_CASE */

#if TMU_UCD_HAS_SIMPLE_CASE_FOLD
TMU_DEF tmu_transform_result tmu_utf8_to_case_fold_simple(const char* str, tm_size_t str_len, char* out,
                                                          tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_case_fold_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tm_bool tmu_utf8_equals_ignore_case_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    tmu_utf8_stream a_stream = tmu_utf8_make_stream_n(a, a_len);
    tmu_utf8_stream b_stream = tmu_utf8_make_stream_n(b, b_len);

    TM_ASSERT(a_stream.cur <= a_stream.end);
    TM_ASSERT(b_stream.cur <= b_stream.end);
    /* We can't early exit by comparing sizes, since utf8 is variable length
       and differing cases can have differing lengths, even if they compare equal when case folded. */

    if (a_stream.cur == b_stream.cur) {
        /* If both strings point to same adress, we just compare the string lengths. */
        return a_len == b_len;
    }

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&a_stream, &a_cp) && tmu_utf8_extract(&b_stream, &b_cp)) {
        const tmu_ucd_internal* a_internal = tmu_get_ucd_internal(a_cp);
        const tmu_ucd_internal* b_internal = tmu_get_ucd_internal(b_cp);

        uint32_t a_case_folded = a_cp + a_internal->simple_case_fold_offset;
        uint32_t b_case_folded = b_cp + b_internal->simple_case_fold_offset;

        if (a_case_folded != b_case_folded) return TM_FALSE;
    }
    return (a_stream.cur == a_stream.end) && (b_stream.cur == b_stream.end);
}

TMU_DEF int tmu_utf8_compare_ignore_case_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    tmu_utf8_stream a_stream = tmu_utf8_make_stream_n(a, a_len);
    tmu_utf8_stream b_stream = tmu_utf8_make_stream_n(b, b_len);

    TM_ASSERT(a_stream.cur <= a_stream.end);
    TM_ASSERT(b_stream.cur <= b_stream.end);

    if (!a_len || !b_len) return !b_len - !a_len;
    if (a_stream.cur == b_stream.cur) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&a_stream, &a_cp) && tmu_utf8_extract(&b_stream, &b_cp)) {
        const tmu_ucd_internal* a_internal = tmu_get_ucd_internal(a_cp);
        const tmu_ucd_internal* b_internal = tmu_get_ucd_internal(b_cp);

        uint32_t a_case_folded = a_cp + a_internal->simple_case_fold_offset;
        uint32_t b_case_folded = b_cp + b_internal->simple_case_fold_offset;

        int diff = (int)a_case_folded - (int)b_case_folded;
        if (diff != 0) return (diff < 0) ? -1 : 1;
    }

    tm_bool a_is_empty = (a_stream.cur == a_stream.end);
    tm_bool b_is_empty = (b_stream.cur == b_stream.end);
    return b_is_empty - a_is_empty;
}

/*
tmu_utf8_human_compare implementation is based on this gist: https://gist.github.com/pervognsen/733034 by Per Vognsen.
*/
static tm_bool tmu_utf8_extract_human_simple(tmu_utf8_stream* stream, uint32_t* codepoint) {
    const char* cur = stream->cur;
    const char* end = stream->end;

    uint32_t base = TMU_INVALID_CODEPOINT;
    if (!tmu_utf8_extract(stream, &base)) return TM_FALSE;

    if (base < (uint8_t)'0' || base > (uint8_t)'9') {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(base);
        *codepoint = base + internal->simple_case_fold_offset;
        return TM_TRUE;
    }

    /* We can treat the utf8 stream as an ascii stream and go byte by byte, since we are comparing ascii values. */
    uint32_t value = 0;
    while (cur != end && *cur >= '0' && *cur <= '9') {
        value = (value * 10) + (*cur - '0');
        ++cur;
    }
    stream->cur = cur;
    TM_ASSERT(stream->cur <= stream->end);

    /* We return an invalid codepoint value, since we want this "codepoint" to compare greater than any other letter. */
    *codepoint = value + TMU_MAX_UTF32;
    return TM_TRUE;
}

TMU_DEF int tmu_utf8_human_compare_simple(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    if (!a_len || !b_len) return !b_len - !a_len;
    if (a == b) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    tmu_utf8_stream a_stream = tmu_utf8_make_stream_n(a, a_len);
    tmu_utf8_stream b_stream = tmu_utf8_make_stream_n(b, b_len);

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract_human_simple(&a_stream, &a_cp) && tmu_utf8_extract_human_simple(&b_stream, &b_cp)) {
        int diff = (int)a_cp - (int)b_cp;
        if (diff != 0) return (diff < 0) ? -1 : 1;
    }

    tm_bool a_is_empty = (a_stream.cur == a_stream.end);
    tm_bool b_is_empty = (b_stream.cur == b_stream.end);
    return b_is_empty - a_is_empty;
}
#endif /* TMU_UCD_HAS_SIMPLE_CASE_FOLD */

#if TMU_UCD_HAS_SIMPLE_CASE_TOGGLE
TMU_DEF tmu_transform_result tmu_utf8_toggle_case_simple(const char* str, tm_size_t str_len, char* out,
                                                         tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    tmu_transform_result result = {0, TM_OK};
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        uint32_t transformed = codepoint + internal->simple_case_toggle_offset;
        tmu_transform_output_append_codepoint(transformed, &out_stream);
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
#endif /* TMU_UCD_HAS_SIMPLE_CASE_TOGGLE */

#if TMU_UCD_HAS_FULL_CASE
TMU_DEF tmu_transform_result tmu_utf8_to_upper(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    tmu_transform_result result = {0, TM_OK};
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_upper_index) {
            const uint16_t* full = tmu_codepoint_runs + tmu_full_upper_offset + internal->full_upper_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_upper_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tmu_transform_result tmu_utf8_to_title(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    tmu_transform_result result = {0, TM_OK};
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_title_index) {
            const uint16_t* full = tmu_codepoint_runs + tmu_full_title_offset + internal->full_title_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_title_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
TMU_DEF tmu_transform_result tmu_utf8_to_lower(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    tmu_transform_result result = {0, TM_OK};
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_lower_index) {
            const uint16_t* full = tmu_codepoint_runs + tmu_full_lower_offset + internal->full_lower_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_lower_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}
#endif /* TMU_UCD_HAS_FULL_CASE */

#if TMU_UCD_HAS_FULL_CASE_FOLD
TMU_DEF tmu_transform_result tmu_utf8_to_case_fold(const char* str, tm_size_t str_len, char* out, tm_size_t out_len) {
    tmu_transform_output_stream out_stream = {out, 0, out_len, {0, TM_OK}};
    tmu_utf8_stream stream = tmu_utf8_make_stream_n(str, str_len);
    uint32_t codepoint = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract(&stream, &codepoint)) {
        const tmu_ucd_internal* internal = tmu_get_ucd_internal(codepoint);
        if (internal->full_case_fold_index) {
            const uint16_t* full = tmu_codepoint_runs + tmu_full_case_fold_offset + internal->full_case_fold_index;
            while (*full) {
                tmu_transform_output_append_codepoint(*full, &out_stream);
                ++full;
            }
        } else {
            uint32_t transformed = codepoint + internal->simple_case_fold_offset;
            tmu_transform_output_append_codepoint(transformed, &out_stream);
        }
    }
    if (out_stream.result.ec == TM_OK && stream.cur != stream.end) {
        out_stream.result.ec = TM_EINVAL;
    }
    return out_stream.result;
}

typedef struct {
    tmu_utf8_stream base;
    const uint16_t* full_case_fold;
} tmu_utf8_case_fold_stream;

static tm_bool tmu_utf8_extract_case_folded(tmu_utf8_case_fold_stream* stream, uint32_t* codepoint) {
    TM_ASSERT(stream);
    TM_ASSERT(stream->base.cur <= stream->base.end);
    TM_ASSERT(codepoint);

    if (stream->full_case_fold) {
        TM_ASSERT(*stream->full_case_fold);
        *codepoint = *stream->full_case_fold;
        ++stream->full_case_fold;
        if (!*stream->full_case_fold) stream->full_case_fold = TM_NULL;
        return TM_TRUE;
    }
    uint32_t base_codepoint = TMU_INVALID_CODEPOINT;
    if (!tmu_utf8_extract(&stream->base, &base_codepoint)) return TM_FALSE;
    const tmu_ucd_internal* internal = tmu_get_ucd_internal(base_codepoint);
    if (internal->full_case_fold_index) {
        stream->full_case_fold = tmu_codepoint_runs + tmu_full_case_fold_offset + internal->full_case_fold_index;
        TM_ASSERT(*stream->full_case_fold);
        *codepoint = *stream->full_case_fold;
        ++stream->full_case_fold;
        if (!*stream->full_case_fold) stream->full_case_fold = TM_NULL;
        return TM_TRUE;
    }
    *codepoint = base_codepoint + internal->simple_case_fold_offset;
    return TM_TRUE;
}

TMU_DEF tm_bool tmu_utf8_equals_ignore_case(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);
    /* We can't early exit by comparing sizes, since utf8 is variable length
       and differing cases can have differing lengths, even if they compare equal when case folded. */

    if (a == b) {
        /* If both strings point to same adress, we just compare the string lengths. */
        return a_len == b_len;
    }

    tmu_utf8_case_fold_stream a_cf = {{TM_NULL, TM_NULL}, TM_NULL};
    tmu_utf8_case_fold_stream b_cf = {{TM_NULL, TM_NULL}, TM_NULL};

    a_cf.base = tmu_utf8_make_stream_n(a, a_len);
    b_cf.base = tmu_utf8_make_stream_n(b, b_len);

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract_case_folded(&a_cf, &a_cp) && tmu_utf8_extract_case_folded(&b_cf, &b_cp)) {
        if (a_cp != b_cp) return TM_FALSE;
    }
    return (a_cf.base.cur == a_cf.base.end) && (!a_cf.full_case_fold) && (b_cf.base.cur == b_cf.base.end) &&
           (!b_cf.full_case_fold);
}

TMU_DEF int tmu_utf8_compare_ignore_case(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    if (!a_len || !b_len) return !b_len - !a_len;
    if (a == b) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    tmu_utf8_case_fold_stream a_cf = {{TM_NULL, TM_NULL}, TM_NULL};
    tmu_utf8_case_fold_stream b_cf = {{TM_NULL, TM_NULL}, TM_NULL};

    a_cf.base = tmu_utf8_make_stream_n(a, a_len);
    b_cf.base = tmu_utf8_make_stream_n(b, b_len);

    uint32_t a_cp = 0;
    uint32_t b_cp = 0;
    while (tmu_utf8_extract_case_folded(&a_cf, &a_cp) && tmu_utf8_extract_case_folded(&b_cf, &b_cp)) {
        int diff = (int)a_cp - (int)b_cp;
        if (diff != 0) return (diff < 0) ? -1 : 1;
    }

    tm_bool a_is_empty = (a_cf.base.cur == a_cf.base.end) && (!a_cf.full_case_fold);
    tm_bool b_is_empty = (b_cf.base.cur == b_cf.base.end) && (!b_cf.full_case_fold);
    return b_is_empty - a_is_empty;
}

/*
tmu_utf8_human_compare implementation is based on this gist: https://gist.github.com/pervognsen/733034 by Per Vognsen.
*/
static tm_bool tmu_utf8_extract_human(tmu_utf8_case_fold_stream* stream, uint32_t* codepoint) {
    const char* cur = stream->base.cur;
    const char* end = stream->base.end;

    uint32_t base = TMU_INVALID_CODEPOINT;
    if (!tmu_utf8_extract_case_folded(stream, &base)) return TM_FALSE;

    if (base < (uint8_t)'0' || base > (uint8_t)'9') {
        *codepoint = base;
        return TM_TRUE;
    }

    /* We can treat the utf8 stream as an ascii stream and go byte by byte, since we are comparing ascii values. */
    uint32_t value = 0;
    while (cur != end && *cur >= '0' && *cur <= '9') {
        value = (value * 10) + (*cur - '0');
        ++cur;
    }
    stream->base.cur = cur;
    TM_ASSERT(stream->base.cur <= stream->base.end);

    /* We return an invalid codepoint value, since we want this "codepoint" to compare greater than any other letter. */
    *codepoint = value + TMU_MAX_UTF32;
    return TM_TRUE;
}

TMU_DEF int tmu_utf8_human_compare(const char* a, tm_size_t a_len, const char* b, tm_size_t b_len) {
    TM_ASSERT(a || a_len == 0);
    TM_ASSERT(b || b_len == 0);

    if (!a_len || !b_len) return !b_len - !a_len;
    if (a == b) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    tmu_utf8_case_fold_stream a_cf = {{TM_NULL, TM_NULL}, TM_NULL};
    tmu_utf8_case_fold_stream b_cf = {{TM_NULL, TM_NULL}, TM_NULL};

    a_cf.base = tmu_utf8_make_stream_n(a, a_len);
    b_cf.base = tmu_utf8_make_stream_n(b, b_len);

    uint32_t a_cp = TMU_INVALID_CODEPOINT;
    uint32_t b_cp = TMU_INVALID_CODEPOINT;
    while (tmu_utf8_extract_human(&a_cf, &a_cp) && tmu_utf8_extract_human(&b_cf, &b_cp)) {
        int diff = (int)a_cp - (int)b_cp;
        if (diff != 0) return (diff < 0) ? -1 : 1;
    }

    tm_bool a_is_empty = (a_cf.base.cur == a_cf.base.end) && (!a_cf.full_case_fold);
    tm_bool b_is_empty = (b_cf.base.cur == b_cf.base.end) && (!b_cf.full_case_fold);
    return b_is_empty - a_is_empty;
}
#endif /* TMU_UCD_HAS_FULL_CASE_FOLD */
