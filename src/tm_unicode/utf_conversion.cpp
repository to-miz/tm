#define TMF_WIDEN(x) ((uint32_t)((uint8_t)(x)))

uint16_t tmf_extract_u16_be(tmf_contents* contents) {
    TM_ASSERT(contents->size >= (tm_size_t)sizeof(uint16_t));
    const char* data = contents->data;
    uint16_t result = (uint16_t)(TMF_WIDEN(data[1]) << 0) | (TMF_WIDEN(data[0]) << 8);
    contents->data += sizeof(uint16_t);
    contents->size -= sizeof(uint16_t);
    return result;
}

uint16_t tmf_extract_u16_le(tmf_contents* contents) {
    TM_ASSERT(contents->size >= (tm_size_t)sizeof(uint16_t));
    const char* data = contents->data;
    uint16_t result = (uint16_t)(TMF_WIDEN(data[0]) << 0) | (TMF_WIDEN(data[1]) << 8);
    contents->data += sizeof(uint16_t);
    contents->size -= sizeof(uint16_t);
    return result;
}

uint32_t tmf_extract_u32_be(tmf_contents* contents) {
    TM_ASSERT(contents->size >= (tm_size_t)sizeof(uint32_t));
    const char* data = contents->data;
    uint32_t result =
        (TMF_WIDEN(data[3]) << 0) | (TMF_WIDEN(data[2]) << 8) | (TMF_WIDEN(data[1]) << 16) | (TMF_WIDEN(data[0]) << 24);
    contents->data += sizeof(uint32_t);
    contents->size -= sizeof(uint32_t);
    return result;
}

uint32_t tmf_extract_u32_le(tmf_contents* contents) {
    TM_ASSERT(contents->size >= (tm_size_t)sizeof(uint32_t));
    const char* data = contents->data;
    uint32_t result =
        (TMF_WIDEN(data[0]) << 0) | (TMF_WIDEN(data[1]) << 8) | (TMF_WIDEN(data[2]) << 16) | (TMF_WIDEN(data[3]) << 24);
    contents->data += sizeof(uint32_t);
    contents->size -= sizeof(uint32_t);
    return result;
}

#undef TMF_WIDEN

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

tm_bool tmf_utf8_append_str(tmf_contents* contents, const char* str, tm_size_t len) {
    TM_ASSERT(contents);
    TM_ASSERT_VALID_SIZE(len);
    if (!len) return TM_TRUE;
    if (!tmf_grow_by(contents, len)) return TM_FALSE;

    TMF_MEMCPY(contents->data + contents->size, str, len * sizeof(char));
    contents->size += len;
    return TM_TRUE;
}

tm_bool tmf_utf8_replace_pos(tmf_contents* contents, tm_size_t at, const char* str, tm_size_t len) {
    TM_ASSERT(contents);
    TM_ASSERT(str);
    TM_ASSERT(len > 0);
    TM_ASSERT(at <= contents->size);
    if (at == contents->size) return tmf_utf8_append_str(contents, str, len);

    if (!tmf_grow_by(contents, len - 1)) return TM_FALSE;

    /* Make room for replacement. */
    TMF_MEMMOVE(contents->data + at + 1, contents->data + at + len, (contents->size - at + 1) * sizeof(char));
    TMF_MEMCPY(contents->data + at, str, len * sizeof(char));
    contents->size += len - 1;
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
        tm_size_t range = tmf_valid_utf8_range(cur, remaining);

        if (cur != str + i) TMF_MEMMOVE(cur, str + i, range);
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    return (tm_size_t)(cur - str);
}

void tmf_replace_invalid_utf8(tmf_utf8_contents_result* r, const char* replace_str, tm_size_t replace_str_len) {
    tmf_contents* contents = &r->contents;
    tm_size_t cur = 0;
    tm_size_t remaining = contents->size;
    tm_bool invalid_codepoints_encountered = TM_FALSE;
    while (remaining) {
        tm_size_t range = tmf_valid_utf8_range(contents->data + cur, remaining);

        if (range != remaining) {
            invalid_codepoints_encountered = TM_TRUE;
            if (!tmf_utf8_replace_pos(contents, cur + range, replace_str, replace_str_len)) {
                r->ec = TM_ENOMEM;
                tmf_destroy_contents(&r->contents);
                break;
            }
            cur += replace_str_len;
        }
        cur += range;
        if (range == remaining) break;
        TM_ASSERT(remaining >= range + 1);
        remaining -= range + 1;
    }
    r->invalid_codepoints_encountered = invalid_codepoints_encountered;
}

typedef struct {
    const tmf_char16* cur;
    const tmf_char16* end;
} tmf_utf16_stream;

typedef struct {
    uint32_t codepoint;
    tm_errc ec;
} tmf_codepoint_result;

tmf_codepoint_result tmf_extract_from_utf16_stream(tmf_utf16_stream* stream) {
    tmf_codepoint_result result = {0xFFFFFFFFu, TM_EINVAL};
    const tmf_char16* cur = stream->cur;
    tmf_char16 const* const end = stream->end;
    if (cur != end) {
        uint32_t lead = (uint16_t)*cur;
        ++cur;

        /* Check for surrogate pair. */
        if (lead >= TMF_LEAD_SURROGATE_MIN && lead <= TMF_LEAD_SURROGATE_MAX) {
            if (cur != end) {
                uint32_t trail = (uint16_t)*cur;
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

tmf_utf8_contents_result tmf_convert_file_from_utf16(tmf_contents contents, uint16_t (*extract)(tmf_contents*),
                                                     tmf_validate validate, const char* replace_str,
                                                     tm_size_t replace_str_len) {
    TM_ASSERT(validate != tmf_validate_replace || (replace_str && replace_str_len > 0));

    tmf_utf8_contents_result result = {{TM_NULL, 0, 0}, TM_OK, tmf_encoding_unknown, TM_FALSE};

    /* Guessing how many utf8 octets we will need. */
    result.contents.capacity = contents.size + (contents.size / 2);
    result.contents.size = 0;
    result.contents.data = (char*)TMF_MALLOC(result.contents.capacity * sizeof(char), sizeof(char));
    if (!result.contents.data) {
        result.ec = TM_ENOMEM;
    } else {
        while (contents.size >= 2) {
            uint32_t codepoint = 0xFFFFFFFFu;

            uint32_t lead = extract(&contents);

            /* Check for surrogate pair. */
            if (lead >= TMF_LEAD_SURROGATE_MIN && lead <= TMF_LEAD_SURROGATE_MAX) {
                if (contents.size < 2) {
                    result.invalid_codepoints_encountered = TM_TRUE;
                    if (validate == tmf_validate_error) result.ec = TM_EINVAL;
                    break;
                }
                uint32_t trail = extract(&contents);
                if (trail >= TMF_TRAILING_SURROGATE_MIN && trail <= TMF_TRAILING_SURROGATE_MAX) {
                    codepoint = (lead << 10) + trail + TMF_SURROGATE_OFFSET;
                }
            } else {
                codepoint = lead;
            }

            if (codepoint > TMF_MAX_UTF32) {
                result.invalid_codepoints_encountered = TM_TRUE;
                if (validate == tmf_validate_replace) {
                    if (!tmf_utf8_append_str(&result.contents, replace_str, replace_str_len)) {
                        result.ec = TM_ENOMEM;
                        tmf_destroy_contents(&result.contents);
                        break;
                    }
                } else if(validate == tmf_validate_skip) {
                    continue;
                } else {
                    result.ec = TM_EINVAL;
                    break;
                }
            }
            if (!tmf_utf8_append_codepoint(&result.contents, codepoint)) {
                result.ec = TM_ENOMEM;
                tmf_destroy_contents(&result.contents);
                break;
            }
        }

        if (result.ec == TM_OK && (validate == tmf_validate_error) && contents.size != 0) result.ec = TM_EINVAL;
    }

    return result;
}

tmf_utf8_contents_result tmf_convert_file_from_utf32(tmf_contents contents, uint32_t (*extract)(tmf_contents*),
                                                     tmf_validate validate, const char* replace_str,
                                                     tm_size_t replace_str_len) {
    TM_ASSERT(validate != tmf_validate_replace || (replace_str && replace_str_len > 0));
    tmf_utf8_contents_result result = {{TM_NULL, 0, 0}, TM_OK, tmf_encoding_unknown, TM_FALSE};

    /* Guessing how many utf8 octets we will need. */
    result.contents.capacity = contents.size + (contents.size / 2);
    result.contents.size = 0;
    result.contents.data = (char*)TMF_MALLOC(result.contents.size * sizeof(char), sizeof(char));
    if (!result.contents.data) {
        result.ec = TM_ENOMEM;
    } else {
        while (contents.size >= 4) {
            uint32_t codepoint = extract(&contents);
            if (codepoint > TMF_MAX_UTF32) {
                result.invalid_codepoints_encountered = TM_TRUE;
                if (validate == tmf_validate_replace) {
                    if (!tmf_utf8_append_str(&result.contents, replace_str, replace_str_len)) {
                        result.ec = TM_ENOMEM;
                        tmf_destroy_contents(&result.contents);
                        break;
                    }
                } else if(validate == tmf_validate_skip) {
                    continue;
                } else {
                    result.ec = TM_EINVAL;
                    break;
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

tm_bool tmf_has_utf8_bom(tmf_contents contents) {
    return contents.size > 3 && (unsigned char)contents.data[0] == tmf_utf8_bom[0] &&
           (unsigned char)contents.data[1] == tmf_utf8_bom[1] && (unsigned char)contents.data[2] == tmf_utf8_bom[2];
}
tm_bool tmf_has_utf32_be_bom(tmf_contents contents) {
    return contents.size > 4 && (unsigned char)contents.data[0] == tmf_utf32_be_bom[0] &&
           (unsigned char)contents.data[1] == tmf_utf32_be_bom[1] &&
           (unsigned char)contents.data[2] == tmf_utf32_be_bom[2] &&
           (unsigned char)contents.data[3] == tmf_utf32_be_bom[3];
}
tm_bool tmf_has_utf32_le_bom(tmf_contents contents) {
    return contents.size > 4 && (unsigned char)contents.data[0] == tmf_utf32_le_bom[0] &&
           (unsigned char)contents.data[1] == tmf_utf32_le_bom[1] &&
           (unsigned char)contents.data[2] == tmf_utf32_le_bom[2] &&
           (unsigned char)contents.data[3] == tmf_utf32_le_bom[3];
}
tm_bool tmf_has_utf16_be_bom(tmf_contents contents) {
    return contents.size > 2 && (unsigned char)contents.data[0] == tmf_utf16_be_bom[0] &&
           (unsigned char)contents.data[1] == tmf_utf16_be_bom[1];
}
tm_bool tmf_has_utf16_le_bom(tmf_contents contents) {
    return contents.size > 2 && (unsigned char)contents.data[0] == tmf_utf16_le_bom[0] &&
           (unsigned char)contents.data[1] == tmf_utf16_le_bom[1];
}

tmf_utf8_contents_result tmf_convert_file_to_utf8(tmf_contents* input, tmf_encoding encoding, tmf_validate validate,
                                                  const char* replace_str, tm_size_t replace_str_len) {
    TM_ASSERT(input);

    tmf_utf8_contents_result result = {{TM_NULL, 0, 0}, TM_OK, tmf_encoding_unknown, TM_FALSE};
    tmf_contents contents = *input;

    if (contents.data) {
        if (encoding == tmf_encoding_unknown) {
            /* Try to detect encoding by inspecting byte order mark. */
            if (tmf_has_utf8_bom(contents)) {
                encoding = tmf_encoding_utf8_bom;
            } else if (tmf_has_utf32_be_bom(contents)) {
                encoding = tmf_encoding_utf32be_bom;

                contents.data += 4;
                contents.size -= 4;

                result = tmf_convert_file_from_utf32(contents, tmf_extract_u32_be, validate, replace_str, replace_str_len);
            } else if (tmf_has_utf32_le_bom(contents)) {
                encoding = tmf_encoding_utf32le_bom;

                contents.data += 4;
                contents.size -= 4;

                result = tmf_convert_file_from_utf32(contents, tmf_extract_u32_le, validate, replace_str, replace_str_len);
            } else if (tmf_has_utf16_be_bom(contents)) {
                encoding = tmf_encoding_utf16be_bom;

                contents.data += 2;
                contents.size -= 2;

                result = tmf_convert_file_from_utf16(contents, tmf_extract_u16_be, validate, replace_str, replace_str_len);
            } else if (tmf_has_utf16_le_bom(contents)) {
                encoding = tmf_encoding_utf16le_bom;

                contents.data += 2;
                contents.size -= 2;

                result = tmf_convert_file_from_utf16(contents, tmf_extract_u16_le, validate, replace_str, replace_str_len);
            } else {
                /* No encoding detected, assume utf8. */
                encoding = tmf_encoding_utf8;
                result.contents = contents;
            }
        }

        switch (encoding) {
            case tmf_encoding_utf8:
            case tmf_encoding_utf8_bom: {
                if (tmf_has_utf8_bom(contents)) {
                    /* Move file contents to skip byte order mark. */
                    result.contents = contents;
                    TMF_MEMMOVE(result.contents.data, result.contents.data + 3, result.contents.size - 3);
                    result.contents.size -= 3;
                    result.original_encoding = tmf_encoding_utf8_bom;
                } else if (encoding == tmf_encoding_utf8_bom) {
                    /* Byte order mark expected but not found, error out. */
                    result.ec = TM_EINVAL;
                    break;
                } else {
                    result.contents = contents;
                    result.original_encoding = tmf_encoding_utf8;
                }

                switch (validate) {
                    case tmf_validate_skip: {
                        tm_size_t new_size = tmf_skip_invalid_utf8(result.contents.data, result.contents.size);
                        result.invalid_codepoints_encountered = (new_size != result.contents.size);
                        result.contents.size = new_size;
                        break;
                    }
                    case tmf_validate_replace: {
                        tmf_replace_invalid_utf8(&result, replace_str, replace_str_len);
                        break;
                    }
                    case tmf_validate_error:
                    default: {
                        tm_size_t valid_range = tmf_valid_utf8_range(result.contents.data, result.contents.size);
                        if (valid_range != result.contents.size) {
                            result.invalid_codepoints_encountered = TM_TRUE;
                            result.ec = TM_EINVAL;
                        }
                        result.contents.size = valid_range;
                        break;
                    }
                }
                break;
            }
            case tmf_encoding_utf32be:
            case tmf_encoding_utf32be_bom: {
                tmf_encoding original_encoding = tmf_encoding_utf32be;
                if (tmf_has_utf32_be_bom(contents)) {
                    /* Skip byte order mark. */
                    contents.data += 4;
                    contents.size -= 4;
                    original_encoding = tmf_encoding_utf32be_bom;
                } else if(encoding == tmf_encoding_utf32be_bom) {
                    /* Byte order mark expected but not found, error out. */
                    result.ec = TM_EINVAL;
                    break;
                }
                result = tmf_convert_file_from_utf32(contents, tmf_extract_u32_be, validate, replace_str, replace_str_len);
                result.original_encoding = original_encoding;
                break;
            }
            case tmf_encoding_utf32le:
            case tmf_encoding_utf32le_bom: {
                tmf_encoding original_encoding = tmf_encoding_utf32le;
                if (tmf_has_utf32_le_bom(contents)) {
                    /* Skip byte order mark. */
                    contents.data += 4;
                    contents.size -= 4;
                    original_encoding = tmf_encoding_utf32le_bom;
                } else if(encoding == tmf_encoding_utf32le_bom) {
                    /* Byte order mark expected but not found, error out. */
                    result.ec = TM_EINVAL;
                    break;
                }
                result = tmf_convert_file_from_utf32(contents, tmf_extract_u32_le, validate, replace_str, replace_str_len);
                result.original_encoding = original_encoding;
                break;
            }
            case tmf_encoding_utf16be:
            case tmf_encoding_utf16be_bom: {
                tmf_encoding original_encoding = tmf_encoding_utf16be;
                if (tmf_has_utf16_be_bom(contents)) {
                    /* Skip byte order mark. */
                    contents.data += 2;
                    contents.size -= 2;
                    original_encoding = tmf_encoding_utf16be_bom;
                } else if(encoding == tmf_encoding_utf16be_bom) {
                    /* Byte order mark expected but not found, error out. */
                    result.ec = TM_EINVAL;
                    break;
                }
                result = tmf_convert_file_from_utf16(contents, tmf_extract_u16_be, validate, replace_str, replace_str_len);
                result.original_encoding = original_encoding;
                break;
            }
            case tmf_encoding_utf16le:
            case tmf_encoding_utf16le_bom: {
                tmf_encoding original_encoding = tmf_encoding_utf16le;
                if (tmf_has_utf16_le_bom(contents)) {
                    /* Skip byte order mark. */
                    contents.data += 2;
                    contents.size -= 2;
                    original_encoding = tmf_encoding_utf16le_bom;
                } else if(encoding == tmf_encoding_utf16le_bom) {
                    /* Byte order mark expected but not found, error out. */
                    result.ec = TM_EINVAL;
                    break;
                }
                result = tmf_convert_file_from_utf16(contents, tmf_extract_u16_le, validate, replace_str, replace_str_len);
                result.original_encoding = original_encoding;
                break;
            }
            default: {
                TM_ASSERT(0 && "Invalid encoding.");
                result.ec = TM_EINVAL;
                break;
            }
        }
    }

    if (result.ec == TM_OK) {
        if (input->data == result.contents.data) {
            /* We took ownership of input, we need to null its members so the memory doesn't get freed. */
            input->data = TM_NULL;
            input->size = 0;
            input->capacity = 0;
        }

        /* Nullterminate. */
        if (!tmf_grow_by(&result.contents, 1)) {
            tmf_destroy_contents(&result.contents);
            result.ec = TM_ENOMEM;
        } else {
            TM_ASSERT(result.contents.size < result.contents.capacity);
            result.contents.data[result.contents.size] = 0;
        }
    }

    return result;
}

void tmf_utf16_destroy_contents(tmf_utf16_contents* contents) {
    if (contents) {
        if (contents->data) TMF_FREE(contents->data, contents->capacity * sizeof(tmf_char16), sizeof(tmf_char16));
        contents->data = TM_NULL;
        contents->size = 0;
        contents->capacity = 0;
    }
}

tmf_contents_result tmf_utf16_stream_to_utf8(tmf_utf16_stream stream) {
    TM_ASSERT(stream.cur <= stream.end);

    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    tm_size_t len = (tm_size_t)(stream.end - stream.cur);

    result.contents.data = (char*)TMF_MALLOC(len * sizeof(char), sizeof(char));
    if (!result.contents.data) {
        result.ec = TM_ENOMEM;
    } else {
        result.contents.capacity = len;
        while (stream.cur < stream.end) {
            tmf_codepoint_result cp = tmf_extract_from_utf16_stream(&stream);
            if (cp.ec != TM_OK) {
                result.ec = TM_EINVAL;
                break;
            }

            if (!tmf_utf8_append_codepoint(&result.contents, cp.codepoint)) {
                result.ec = TM_ENOMEM;
                tmf_destroy_contents(&result.contents);
                break;
            }
        }

        // Nullterminate.
        if (!tmf_grow_by(&result.contents, 1)) {
            result.ec = TM_ENOMEM;
            tmf_destroy_contents(&result.contents);
        } else {
            TM_ASSERT(result.contents.size < result.contents.capacity);
            result.contents.data[result.contents.size] = 0;
        }
    }

    return result;
}