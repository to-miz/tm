#define TMF_WIDEN(x) ((uint32_t)((uint8_t)(x)))

enum {
    TMF_LEAD_SURROGATE_MIN = 0xD800u,
    TMF_LEAD_SURROGATE_MAX = 0xDBFFu,
    TMF_TRAILING_SURROGATE_MIN = 0xDC00u,
    TMF_TRAILING_SURROGATE_MAX = 0xDFFFu,
    TMF_SURROGATE_OFFSET = 0x10000u - (0xD800u << 10u) - 0xDC00u,
};

uint16_t tmf_extract_u16_be(tmf_contents* contents) {
    TM_ASSERT(contents->size >= sizeof(uint16_t));
    const char* data = contents->data;
    uint16_t result = (uint16_t)(TMF_WIDEN(data[1]) << 0) | (TMF_WIDEN(data[0]) << 8);
    contents->data += sizeof(uint16_t);
    contents->size -= sizeof(uint16_t);
    return result;
}

uint16_t tmf_extract_u16_le(tmf_contents* contents) {
    TM_ASSERT(contents->size >= sizeof(uint16_t));
    const char* data = contents->data;
    uint16_t result = (uint16_t)(TMF_WIDEN(data[0]) << 0) | (TMF_WIDEN(data[1]) << 8);
    contents->data += sizeof(uint16_t);
    contents->size -= sizeof(uint16_t);
    return result;
}

uint32_t tmf_extract_u32_be(tmf_contents* contents) {
    TM_ASSERT(contents->size >= sizeof(uint32_t));
    const char* data = contents->data;
    uint32_t result =
        (TMF_WIDEN(data[3]) << 0) | (TMF_WIDEN(data[2]) << 8) | (TMF_WIDEN(data[1]) << 16) | (TMF_WIDEN(data[0]) << 24);
    contents->data += sizeof(uint32_t);
    contents->size -= sizeof(uint32_t);
    return result;
}

uint32_t tmf_extract_u32_le(tmf_contents* contents) {
    TM_ASSERT(contents->size >= sizeof(uint32_t));
    const char* data = contents->data;
    uint32_t result =
        (TMF_WIDEN(data[0]) << 0) | (TMF_WIDEN(data[1]) << 8) | (TMF_WIDEN(data[2]) << 16) | (TMF_WIDEN(data[3]) << 24);
    contents->data += sizeof(uint32_t);
    contents->size -= sizeof(uint32_t);
    return result;
}

#undef TMF_WIDEN

tm_bool tmf_grow_by(tmf_contents* contents, tm_size_t amount) {
    TM_ASSERT(contents);
    TM_ASSERT_VALID_SIZE(contents->size);
    TM_ASSERT(contents->capacity >= contents->size);

    if ((contents->capacity - contents->size) >= amount) return TM_TRUE;

    tm_size_t new_capacity = contents->capacity + (contents->capacity / 2);
    char* new_data = TMF_REALLOC(char, contents->data, contents->capacity, sizeof(char), new_capacity, sizeof(char));
    if (!new_data) return TM_FALSE;

    contents->data = new_data;
    contents->capacity = new_capacity;
    return TM_TRUE;
}

#define TMF_MAX_UTF32 0x10FFFFu

tm_bool tmf_utf8_append_codepoint(tmf_contents* contents, uint32_t codepoint) {
    TM_ASSERT(contents);
    TM_ASSERT(codepoint <= TMF_MAX_UTF32);
    if (codepoint < 0x80) {
        /* 1 byte sequence */
        if (!tmf_grow_by(contents, 1)) return TM_FALSE;
        char* cur = contents->data + contents->size;
        cur[0] = (char)(codepoint);
        contents->size += 1;
    } else if (codepoint < 0x800) {
        /* 2 byte sequence 110xxxxx 10xxxxxx */
        if (!tmf_grow_by(contents, 2)) return TM_FALSE;
        char* cur = contents->data + contents->size;
        cur[0] = (char)(0xC0 | (uint8_t)(codepoint >> 6));
        cur[1] = (char)(0x80 | (uint8_t)(codepoint & 0x3F));
        contents->size += 2;
    } else if (codepoint < 0x10000) {
        /* 3 byte sequence 1110xxxx 10xxxxxx 10xxxxxx */
        if (!tmf_grow_by(contents, 3)) return TM_FALSE;
        char* cur = contents->data + contents->size;
        cur[0] = (char)(0xE0 | (uint8_t)(codepoint >> 12));
        cur[1] = (char)(0x80 | ((uint8_t)(codepoint >> 6) & 0x3F));
        cur[2] = (char)(0x80 | ((uint8_t)(codepoint & 0x3F)));
        contents->size += 3;
    } else {
        /* 4 byte sequence 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        if (!tmf_grow_by(contents, 4)) return TM_FALSE;
        char* cur = contents->data + contents->size;
        cur[0] = (char)(0xF0 | ((uint8_t)(codepoint >> 18) & 0x7));
        cur[1] = (char)(0x80 | ((uint8_t)(codepoint >> 12) & 0x3F));
        cur[2] = (char)(0x80 | ((uint8_t)(codepoint >> 6) & 0x3F));
        cur[3] = (char)(0x80 | ((uint8_t)(codepoint & 0x3F)));
        contents->size += 4;
    }

    return TM_TRUE;
}

typedef struct {
    uint16_t* data;
    tm_size_t size;
    tm_size_t capacity;
} tmf_utf16_contents;

typedef struct {
    tmf_utf16_contents contents;
    tm_errc ec;
} tmf_utf16_contents_result;

tm_bool tmf_utf16_grow_by(tmf_utf16_contents* contents, tm_size_t amount) {
    TM_ASSERT(contents);
    TM_ASSERT_VALID_SIZE(contents->size);
    TM_ASSERT(contents->capacity >= contents->size);

    if ((contents->capacity - contents->size) >= amount) return TM_TRUE;

    tm_size_t new_capacity = contents->capacity + (contents->capacity / 2);
    uint16_t* new_data =
        TMF_REALLOC(uint16_t, contents->data, contents->capacity, sizeof(uint16_t), new_capacity, sizeof(uint16_t));
    if (!new_data) return TM_FALSE;

    contents->data = new_data;
    contents->capacity = new_capacity;
    return TM_TRUE;
}

tm_bool tmf_utf16_append_codepoint(tmf_utf16_contents* contents, uint32_t codepoint) {
    TM_ASSERT(codepoint <= TMF_MAX_UTF32);
    if (codepoint < 0xD7FF) {
        if (!tmf_utf16_grow_by(contents, 1)) return TM_FALSE;
        uint16_t* cur = contents->data + contents->size;
        cur[0] = (uint16_t)codepoint;
        contents->size += 1;
    } else if (codepoint >= 0xE000 && codepoint <= 0xFFFF) {
        if (!tmf_utf16_grow_by(contents, 1)) return TM_FALSE;
        uint16_t* cur = contents->data + contents->size;
        cur[0] = (uint16_t)codepoint;
        contents->size += 1;
    } else if (codepoint >= 0x10000 && codepoint <= 0x10FFFF) {
        if (!tmf_utf16_grow_by(contents, 2)) return TM_FALSE;
        uint16_t* cur = contents->data + contents->size;
        codepoint -= 0x10000;
        cur[0] = 0xD800 + (uint16_t)(codepoint >> 10);
        cur[1] = 0xDC00 + (uint16_t)(codepoint & 0x3FF);
        contents->size += 2;
    } else {
        return TM_FALSE;
    }
    return TM_TRUE;
}

tm_size_t tmf_valid_utf8_range(const char* str, tm_size_t len) {
    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        uint32_t c0 = (uint32_t)((uint8_t)str[i]);
        if (c0 < 0x80u) {
            /* 0xxxxxxx */
            --remaining;
        } else if ((c0 & 0xE0u) == 0xC0u) {
            /* 110xxxxx 10xxxxxx */
            if (remaining < 2) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            if ((c1 & 0xC0u) != 0x80u) return i; /* Invalid trail. */
            if ((c0 & 0xFEu) == 0xC0u) return i; /* Overlong. */
            remaining -= 2;
        } else if ((c0 & 0xF0u) == 0xE0u) {
            /* 1110xxxx 10xxxxxx 10xxxxxx */
            if (remaining < 3) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            uint32_t c2 = (uint32_t)((uint8_t)str[i + 2]);
            if ((c1 & 0xC0u) != 0x80u) return i;                                 /* Invalid trail. */
            if ((c2 & 0xC0u) != 0x80u) return i;                                 /* Invalid trail. */
            if ((c0 == 0xE0u && (c1 & 0xE0u) == 0x80u)) return i;                /* Overlong. */
            if ((c0 == 0xEDu && (c1 & 0xE0u) == 0xA0u)) return i;                /* Surrogate range. */
            if ((c0 == 0xEFu && c1 == 0xBFu && (c2 & 0xFEu) == 0xBEu)) return i; /* Invalid codepoints. */

            remaining -= 3;
        } else if ((c0 & 0xF8u) == 0xF0u) {
            /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            if (remaining < 4) return i;

            uint32_t c1 = (uint32_t)((uint8_t)str[i + 1]);
            uint32_t c2 = (uint32_t)((uint8_t)str[i + 2]);
            uint32_t c3 = (uint32_t)((uint8_t)str[i + 3]);
            if ((c1 & 0xC0u) != 0x80u) return i;                       /* Invalid trail. */
            if ((c2 & 0xC0u) != 0x80u) return i;                       /* Invalid trail. */
            if ((c3 & 0xC0u) != 0x80u) return i;                       /* Invalid trail. */
            if ((c0 == 0xF0u && (c1 & 0xF0u) == 0x80u)) return i;      /* Overlong. */
            if (((c0 == 0xF4u && c1 > 0x8Fu) || c0 > 0xF4u)) return i; /* Invalid codepoints. */

            remaining -= 4;
        } else {
            return i;
        }
    }
    return len;
}

tm_size_t tmf_skip_invalid_utf8(char* str, tm_size_t len) {
    char* cur = str;
    tm_size_t remaining = len;
    while (remaining) {
        tm_size_t i = len - remaining;
        tm_size_t range = tmf_valid_utf8_range(str + i, remaining);

        if (cur != str + i) TMF_MEMMOVE(cur, str + i, range);
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    return (tm_size_t)(cur - str);
}

typedef struct {
    const uint16_t* cur;
    const uint16_t* end;
} tmf_utf16_stream;

typedef struct {
    uint32_t codepoint;
    tm_errc ec;
} tmf_codepoint_result;

tmf_codepoint_result tmf_extract_from_utf16_stream(tmf_utf16_stream* stream) {
    tmf_codepoint_result result = {0xFFFFFFFFu, TM_EINVAL};
    const uint16_t* cur = stream->cur;
    uint16_t const* const end = stream->end;
    if (cur != end) {
        uint32_t lead = *cur;
        ++cur;

        /* Check for surrogate pair. */
        if (lead >= TMF_LEAD_SURROGATE_MIN && lead <= TMF_LEAD_SURROGATE_MAX) {
            if (cur != end) {
                uint32_t trail = *cur;
                ++cur;
                if (trail >= TMF_TRAILING_SURROGATE_MIN && trail <= TMF_TRAILING_SURROGATE_MAX) {
                    result.codepoint = (lead << 10) + trail + TMF_SURROGATE_OFFSET;
                }
            }
        } else {
            result.codepoint = lead;
        }

        result.ec = (result.codepoint > TMF_MAX_UTF32) ? TM_EINVAL : TM_OK;
        stream->cur = cur;
    }
    return result;
}

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

tmf_contents_result tmf_convert_file_from_utf16(tmf_contents contents, uint16_t (*extract)(tmf_contents*),
                                           tm_bool validate) {
    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    /* Guessing how many utf8 octets we will need. */
    result.contents.capacity = contents.size + (contents.size / 2);
    result.contents.size = 0;
    result.contents.data = TMF_MALLOC(char, result.contents.capacity, sizeof(char));
    if (!result.contents.data) {
        result.ec = TM_ENOMEM;
    } else {
        while (contents.size >= 2) {
            uint32_t codepoint = 0;

            uint32_t lead = extract(&contents);

            /* Check for surrogate pair. */
            if (lead >= TMF_LEAD_SURROGATE_MIN && lead <= TMF_LEAD_SURROGATE_MAX) {
                if (contents.size < 2) {
                    if (validate) result.ec = TM_EINVAL;
                    break;
                }
                uint32_t trail = extract(&contents);
                if (trail < TMF_TRAILING_SURROGATE_MIN || trail > TMF_TRAILING_SURROGATE_MAX) {
                    if (validate) {
                        result.ec = TM_EINVAL;
                        break;
                    } else {
                        continue;
                    }
                }
                codepoint = (lead << 10) + trail + TMF_SURROGATE_OFFSET;
            } else {
                codepoint = lead;
            }

            if (codepoint > TMF_MAX_UTF32) {
                if (validate) {
                    result.ec = TM_EINVAL;
                    break;
                } else {
                    continue;
                }
            }
            if (!tmf_utf8_append_codepoint(&result.contents, codepoint)) {
                result.ec = TM_ENOMEM;
                tmf_destroy_contents(&result.contents);
                break;
            }
        }

        if (result.ec == TM_OK && validate && contents.size != 0) result.ec = TM_EINVAL;
    }

    return result;
}

tmf_contents_result tmf_convert_file_from_utf32(tmf_contents contents, uint32_t (*extract)(tmf_contents*),
                                           tm_bool validate) {
    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    /* Guessing how many utf8 octets we will need. */
    result.contents.capacity = contents.size + (contents.size / 2);
    result.contents.size = 0;
    result.contents.data = TMF_MALLOC(char, result.contents.size, sizeof(char));
    if (!result.contents.data) {
        result.ec = TM_ENOMEM;
    } else {
        while (contents.size >= 4) {
            uint32_t codepoint = extract(&contents);
            if (codepoint > TMF_MAX_UTF32) {
                if (validate) {
                    result.ec = TM_EINVAL;
                    break;
                } else {
                    continue;
                }
            }
            if (!tmf_utf8_append_codepoint(&result.contents, codepoint)) {
                result.ec = TM_ENOMEM;
                tmf_destroy_contents(&result.contents);
                break;
            }
        }

        if (result.ec == TM_OK && validate && contents.size != 0) result.ec = TM_EINVAL;
    }

    return result;
}

#undef TMF_MAX_UTF32

tmf_contents_result tmf_convert_file_to_utf8(tmf_contents contents, tm_bool validate) {
    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    if (contents.data) {
        tm_bool is_utf8 = false;
        if (contents.size > 3 && contents.data[0] == tmf_utf8_bom[0] && contents.data[1] == tmf_utf8_bom[1] &&
            contents.data[2] == tmf_utf8_bom[2]) {
            /* File is utf8 encoded, move file contents to skip byte order mark. */
            result.contents = contents;
            TMF_MEMMOVE(result.contents.data, result.contents.data + 3, result.contents.size - 3);
            result.contents.size -= 3;
            is_utf8 = TM_TRUE;
        } else if (contents.size > 4 && contents.data[0] == tmf_utf32_be_bom[0] &&
                   contents.data[1] == tmf_utf32_be_bom[1] && contents.data[2] == tmf_utf32_be_bom[2] &&
                   contents.data[3] == tmf_utf32_be_bom[3]) {
            /* Utf32 big endian encoding. */
            contents.data += 4;
            contents.size -= 4;

            result = tmf_convert_file_from_utf32(contents, tmf_extract_u32_be, validate);
        } else if (contents.size > 4 && contents.data[0] == tmf_utf32_le_bom[0] &&
                   contents.data[1] == tmf_utf32_le_bom[1] && contents.data[2] == tmf_utf32_le_bom[2] &&
                   contents.data[3] == tmf_utf32_le_bom[3]) {
            /* Utf32 little endian encoding. */
            contents.data += 4;
            contents.size -= 4;

            result = tmf_convert_file_from_utf32(contents, tmf_extract_u32_le, validate);
        } else if (contents.size > 2 && contents.data[0] == tmf_utf16_be_bom[0] &&
                   contents.data[1] == tmf_utf16_be_bom[1]) {
            /* Utf16 big endian encoding. */
            contents.data += 2;
            contents.size -= 2;

            result = tmf_convert_file_from_utf16(contents, tmf_extract_u16_be, validate);
        } else if (contents.size > 2 && contents.data[0] == tmf_utf16_le_bom[0] &&
                   contents.data[1] == tmf_utf16_le_bom[1]) {
            /* Utf16 little endian encoding. */
            contents.data += 2;
            contents.size -= 2;

            result = tmf_convert_file_from_utf16(contents, tmf_extract_u16_le, validate);
        } else {
            /* No encoding detected, assume utf8. */
            result.contents = contents;
            is_utf8 = TM_TRUE;
        }

        if (is_utf8) {
            if (validate) {
                tm_size_t valid_range = tmf_valid_utf8_range(result.contents.data, result.contents.size);
                if (valid_range != result.contents.size) result.ec = TM_EINVAL;
                result.contents.size = valid_range;
            } else {
                result.contents.size = tmf_skip_invalid_utf8(result.contents.data, result.contents.size);
            }
        }
    }

    return result;
}

tmf_utf16_contents_result tmf_utf8_stream_to_utf16(tmf_utf8_stream stream) {
    TM_UNREFERENCED_PARAM(stream);
    tmf_utf16_contents_result result = {};
    return result;
}

tmf_utf16_contents_result tmf_utf8_to_utf16(const char* str) {
    TM_ASSERT(str);

    tmf_utf8_stream stream;
    stream.cur = str;
    stream.end = str + TMF_STRLEN(str);

    return tmf_utf8_stream_to_utf16(stream);
}

tmf_utf16_contents_result tmf_utf8_n_to_utf16(const char* str, tm_size_t len) {
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || !len);

    tmf_utf8_stream stream;
    stream.cur = str;
    stream.end = str + len;

    return tmf_utf8_stream_to_utf16(stream);
}