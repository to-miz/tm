#define TMSU_SV_BEGIN(str) TM_STRING_VIEW_DATA(str)
#define TMSU_SV_END(str) (TM_STRING_VIEW_DATA(str) + TM_STRING_VIEW_SIZE(str))

const char* tmsu_find_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_last_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                const char* not_found) {
    return tmsu_find_last_n_ex(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str), not_found);
}
const char* tmsu_find_first_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_first_of_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_first_not_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_first_not_of_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_last_of_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_of_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                   const char* not_found) {
    return tmsu_find_last_of_n_ex(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str), not_found);
}
const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_last_not_of_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_last_not_of_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                       const char* not_found) {
    return tmsu_find_last_not_of_n_ex(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str), not_found);
}
const char* tmsu_find_ignore_case_ansi_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str) {
    return tmsu_find_ignore_case_ansi_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
const char* tmsu_find_first_of_unescaped_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                           char escape_char) {
    return tmsu_find_first_of_unescaped_n(str_first, str_last, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str),
                                          escape_char);
}

TM_STRING_VIEW tmsu_trim_left(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    first = tmsu_trim_left_n(first, last);
    return TM_STRING_VIEW_MAKE(first, tmsu_distance_sz(first, last));
}
TM_STRING_VIEW tmsu_trim_right(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    last = tmsu_trim_right_n(first, last);
    return TM_STRING_VIEW_MAKE(first, tmsu_distance_sz(first, last));
}
TM_STRING_VIEW tmsu_trim(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    first = tmsu_trim_left_n(first, last);
    last = tmsu_trim_right_n(first, last);
    return TM_STRING_VIEW_MAKE(first, tmsu_distance_sz(first, last));
}

int tmsu_compare(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_compare_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}
int tmsu_compare_ignore_case_ansi(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_compare_ignore_case_ansi_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}
int tmsu_human_compare_ansi(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_human_compare_ansi_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}

tm_bool tmsu_equals(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_equals_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}
tm_bool tmsu_equals_ignore_case_ansi(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_equals_ignore_case_ansi_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}

tm_bool tmsu_starts_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_starts_with_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
tm_bool tmsu_ends_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_ends_with_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
tm_bool tmsu_starts_with_ignore_case_ansi(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_starts_with_ignore_case_ansi_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str),
                                                TMSU_SV_END(find_str));
}

tm_bool tmsu_ends_with_ignore_case_ansi(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_ends_with_ignore_case_ansi_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str),
                                              TMSU_SV_END(find_str));
}

tm_bool tmsu_next_token_n(tmsu_tokenizer_n* tokenizer, TM_STRING_VIEW find_str, tmsu_string_view* out) {
    return tmsu_next_token_n(tokenizer, TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str), out);
}

#undef TMSU_SV_BEGIN
#undef TMSU_SV_END