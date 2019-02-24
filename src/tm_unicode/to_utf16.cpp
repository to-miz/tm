typedef struct {
    const char* cur;
    const char* end;
} tmf_utf8_stream;

tmf_codepoint_result tmf_extract_from_utf8_stream(tmf_utf8_stream* stream) {
    tmf_codepoint_result result = {0xFFFFFFFFu, TM_EINVAL};
    const char* cur = stream->cur;
    tm_size_t remaining = (tm_size_t)(stream->end - cur);
    if (remaining > 0) {
        uint32_t first = (uint8_t)cur[0];
        if (first < 0x80) {
            result.codepoint = first;
            cur += 1;
        } else if ((first >> 5) == 0x6) {  // 110xxxxx 10xxxxxx
            // 2 byte sequence
            if (remaining >= 2) {
                uint32_t second = (uint8_t)cur[1];
                result.codepoint = ((first & 0x1F) << 6) | (second & 0x3F);
                cur += 2;
            }
        } else if ((first >> 4) == 0xE) {  // 1110xxxx 10xxxxxx 10xxxxxx
            // 3 byte sequence
            if (remaining >= 3) {
                uint32_t second = (uint8_t)cur[1];
                uint32_t third = (uint8_t)cur[2];
                result.codepoint = ((first & 0xF) << 12) | ((second & 0x3F) << 6) | (third & 0x3F);
                cur += 3;
            }
        } else if ((first >> 3) == 0x1E) {  // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            // 4 byte sequence
            if (remaining >= 4) {
                uint32_t second = (uint8_t)cur[1];
                uint32_t third = (uint8_t)cur[2];
                uint32_t fourth = (uint8_t)cur[3];
                result.codepoint =
                    ((first & 0x7) << 18) | ((second & 0x3F) << 12) | ((third & 0x3F) << 6) | (fourth & 0x3f);
                cur += 4;
            }
        }

        result.ec = (result.codepoint > TMF_MAX_UTF32) ? TM_EINVAL : TM_OK;
        stream->cur = cur;
    }
    return result;
}

tm_bool tmf_utf16_grow_by(tmf_utf16_contents* contents, tm_size_t amount, tm_bool owns) {
    TM_ASSERT(contents);
    TM_ASSERT_VALID_SIZE(contents->size);
    TM_ASSERT(contents->capacity >= contents->size);

    if ((contents->capacity - contents->size) >= amount) return TM_TRUE;

    tm_size_t new_capacity = contents->capacity + (contents->capacity / 2);
    if (new_capacity - contents->capacity < amount) new_capacity = contents->capacity + amount;
    TM_ASSERT(new_capacity > 0);

    tmf_char16* new_data = TM_NULL;
    if (owns) {
        new_data = (tmf_char16*)TMF_REALLOC(contents->data, contents->capacity * sizeof(tmf_char16), sizeof(tmf_char16),
                                            new_capacity * sizeof(tmf_char16), sizeof(tmf_char16));
        if (!new_data) return TM_FALSE;
    } else {
        new_data = (tmf_char16*)TMF_MALLOC(new_capacity * sizeof(tmf_char16), sizeof(tmf_char16));
        if (!new_data) return TM_FALSE;
        TMF_MEMCPY(new_data, contents->data, contents->size * sizeof(tmf_char16));
    }

    TM_ASSERT(new_data);
    contents->data = new_data;
    contents->capacity = new_capacity;
    return TM_TRUE;
}

tm_bool tmf_utf16_append_codepoint(tmf_utf16_contents* contents, uint32_t codepoint, tm_bool owns) {
    TM_ASSERT(codepoint <= TMF_MAX_UTF32);
    if (codepoint < TMF_LEAD_SURROGATE_MIN || (codepoint >= 0xE000u && codepoint <= 0xFFFFu)) {
        if (!tmf_utf16_grow_by(contents, 1, owns)) return TM_FALSE;
        tmf_char16* cur = contents->data + contents->size;
        cur[0] = (tmf_char16)codepoint;
        contents->size += 1;
    } else if (codepoint >= 0x10000u && codepoint <= 0x10FFFFu) {
        if (!tmf_utf16_grow_by(contents, 2, owns)) return TM_FALSE;
        tmf_char16* cur = contents->data + contents->size;
        codepoint -= 0x10000u;
        cur[0] = (tmf_char16)(TMF_LEAD_SURROGATE_MIN + (uint16_t)(codepoint >> 10u));
        cur[1] = (tmf_char16)(TMF_TRAILING_SURROGATE_MIN + (uint16_t)(codepoint & 0x3FFu));
        contents->size += 2;
    } else {
        return TM_FALSE;
    }
    return TM_TRUE;
}

tmf_utf16_contents_result tmf_utf8_stream_to_utf16(tmf_utf8_stream stream, tmf_char16* sbo, tm_size_t sbo_size) {
    TM_ASSERT(stream.cur <= stream.end);

    tmf_utf16_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    tm_size_t len = (tm_size_t)(stream.end - stream.cur);

    /* Init result. */
    if (sbo) {
        result.contents.data = sbo;
        result.contents.capacity = sbo_size;
    } else {
        result.contents.data = (tmf_char16*)TMF_MALLOC(len * sizeof(tmf_char16), sizeof(tmf_char16));
        if (!result.contents.data) {
            result.ec = TM_ENOMEM;
        } else {
            result.contents.capacity = len;
        }
    }

    if (result.ec == TM_OK) {
        while (stream.cur < stream.end) {
            tmf_codepoint_result cp = tmf_extract_from_utf8_stream(&stream);
            if (cp.ec != TM_OK) {
                result.ec = TM_EINVAL;
                break;
            }

            if (!tmf_utf16_append_codepoint(&result.contents, cp.codepoint, result.contents.data != sbo)) {
                tmf_utf16_destroy_contents(&result.contents, result.contents.data != sbo);
                result.ec = TM_ENOMEM;
                break;
            }
        }

        // Nullterminate.
        if (!tmf_utf16_grow_by(&result.contents, 1, result.contents.data != sbo)) {
            tmf_utf16_destroy_contents(&result.contents, result.contents.data != sbo);
            result.ec = TM_ENOMEM;
        } else {
            TM_ASSERT(result.contents.size < result.contents.capacity);
            result.contents.data[result.contents.size] = 0;
        }
    }

    return result;
}

tmf_utf16_contents_result tmf_utf8_to_utf16(const char* str, tmf_char16* sbo, tm_size_t sbo_size) {
    TM_ASSERT(str);

    tmf_utf8_stream stream;
    stream.cur = str;
    stream.end = str + TMF_STRLEN(str);

    return tmf_utf8_stream_to_utf16(stream, sbo, sbo_size);
}

tmf_utf16_contents_result tmf_utf8_n_to_utf16(const char* str, tm_size_t len, tmf_char16* sbo, tm_size_t sbo_size) {
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || !len);

    tmf_utf8_stream stream;
    stream.cur = str;
    stream.end = str + len;

    return tmf_utf8_stream_to_utf16(stream, sbo, sbo_size);
}

tmf_contents_result tmf_utf16_to_utf8(const tmf_char16* str) {
    TM_ASSERT(str);

    tmf_utf16_stream stream;
    stream.cur = str;
    stream.end = str + TMF_CHAR16LEN(str);

    return tmf_utf16_stream_to_utf8(stream);
}