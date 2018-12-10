tmf_contents_result tmf_convert_to_utf8(tmf_contents contents) {
    tmf_contents_result result = {{TM_NULL, 0}, TM_OK};

    if (contents.data) {
        if (contents.size > 3 && contents.data[0] == tmf_utf8_bom[0] && contents.data[1] == tmf_utf8_bom[1] &&
            contents.data[2] == tmf_utf8_bom[2]) {
            /* File is utf8 encoded, move file contents to skip byte order mark. */
            result.contents = contents;
            TMF_MEMMOVE(result.contents.data, result.contents.data + 3, result.contents.size - 3);
            /* Clear out last 3 bytes. */
            result.contents.data[result.contents.size - 3] = 0;
            result.contents.data[result.contents.size - 2] = 0;
            result.contents.data[result.contents.size - 1] = 0;
        } else if (contents.size > 2 && contents.data[0] == tmf_utf16_be_bom[0] &&
                   contents.data[1] == tmf_utf16_be_bom[1]) {
            /* Utf16 big endian encoding. */

        } else if (contents.size > 2 && contents.data[0] == tmf_utf16_le_bom[0] &&
                   contents.data[1] == tmf_utf16_le_bom[1]) {
            /* Utf16 little endian encoding. */

        } else if (contents.size > 4 && contents.data[0] == tmf_utf32_be_bom[0] &&
                   contents.data[1] == tmf_utf32_be_bom[1] && contents.data[2] == tmf_utf32_be_bom[2] &&
                   contents.data[3] == tmf_utf32_be_bom[3]) {
            /* Utf32 big endian encoding. */

        } else if (contents.size > 4 && contents.data[0] == tmf_utf32_le_bom[0] &&
                   contents.data[1] == tmf_utf32_le_bom[1] && contents.data[2] == tmf_utf32_le_bom[2] &&
                   contents.data[3] == tmf_utf32_le_bom[3]) {
            /* Utf32 little endian encoding. */

        } else {
            /* No encoding detected, assume utf8. */
            result.contents = contents;
        }
    }

    return result;
}