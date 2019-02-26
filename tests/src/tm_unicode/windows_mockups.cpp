int test_MultiByteToWideChar(int codepage, DWORD flags, const char* multibyte, int multibyte_len,
                             tchar* widechar, int widechar_len) {
    TM_ASSERT(codepage == CP_UTF8);
    TM_ASSERT(flags == MB_ERR_INVALID_CHARS);
    TM_ASSERT(multibyte_len >= -1);
    TM_ASSERT(widechar_len >= 0);
    TM_ASSERT(multibyte || multibyte_len == 0);

    if (win_mock.test_fail(fail_MultiByteToWideChar)) return 0;

    if (multibyte_len == -1) {
        multibyte_len = (int)(strlen(multibyte) + 1);  // Include null-terminator.
    }
    TM_ASSERT(multibyte_len >= 0);

    if (widechar_len == 0) {
        widechar = TM_NULL;
    }

    auto result = tmu_utf16_from_utf8(tmu_utf8_make_stream_n(multibyte, (tm_size_t)multibyte_len), widechar,
                                      (tm_size_t)widechar_len);
    TM_ASSERT(result.ec == TM_OK || result.ec == TM_ERANGE);
    return (int)result.size;
}

int test_WideCharToMultiByte(int codepage, DWORD flags, const tchar* widechar, int widechar_len, char* multibyte,
                             int multibyte_len, const char* default_char, bool* default_char_used) {
    TM_ASSERT(codepage == CP_UTF8);
    TM_ASSERT(flags == MB_ERR_INVALID_CHARS);

    TM_ASSERT(default_char == nullptr);
    TM_ASSERT(default_char_used == nullptr);

    TM_ASSERT(widechar || widechar_len == 0);

    if (win_mock.test_fail(fail_WideCharToMultiByte)) return 0;

    if (widechar_len == -1) {
        widechar_len = (int)(test_strlen(widechar) + 1);  // Include null-terminator.
    }
    TM_ASSERT(widechar_len >= 0);

    if (multibyte_len == 0) {
        multibyte = TM_NULL;
    }

    auto result = tmu_utf8_from_utf16(tmu_utf16_make_stream_n(widechar, (tm_size_t)widechar_len), multibyte,
                                      (tm_size_t)multibyte_len);
    TM_ASSERT(result.ec == TM_OK || result.ec == TM_ERANGE);
    return (int)result.size;
}