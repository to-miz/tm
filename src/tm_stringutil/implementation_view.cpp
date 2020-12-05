TMSU_DEF tmsu_view_t tmsu_view(const char* str) {
    tmsu_view_t result;
    result.first = str;
    result.last = (str) ? (str + TM_STRLEN(str)) : str;
    return result;
}

TMSU_DEF tmsu_view_t tmsu_view_n(const char* first, const char* last) {
    TM_ASSERT(first <= last);
    tmsu_view_t result;
    result.first = first;
    result.last = last;
    return result;
}

TMSU_DEF tmsu_view_t tmsu_view_l(const char* str, tm_size_t len) {
    TM_ASSERT(str || len == 0);
    TM_ASSERT_VALID_SIZE(len);
    tmsu_view_t result;
    result.first = str;
    result.last = (str) ? (str + len) : str;
    return result;
}

TMSU_DEF tm_bool tmsu_view_empty(tmsu_view_t str) {
    return str.first == str.last;
}

TMSU_DEF const char* tmsu_view_data(tmsu_view_t str) {
    return str.first;
}

TMSU_DEF tm_size_t tmsu_view_size(tmsu_view_t str) {
    TM_ASSERT(str.first <= str.last);
    return (tm_size_t)(str.last - str.first);
}

TMSU_DEF const char* tmsu_find_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_last_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found) {
    return tmsu_find_last_n_ex(str.first, str.last, find_str.first, find_str.last, not_found);
}

TMSU_DEF const char* tmsu_find_first_of_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_first_of_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_first_not_of_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_first_not_of_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_of_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_last_of_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_of_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found) {
    return tmsu_find_last_of_n_ex(str.first, str.last, find_str.first, find_str.last, not_found);
}

TMSU_DEF const char* tmsu_find_last_not_of_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_last_not_of_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_last_not_of_v_ex(tmsu_view_t str, tmsu_view_t find_str, const char* not_found) {
    return tmsu_find_last_not_of_n_ex(str.first, str.last, find_str.first, find_str.last, not_found);
}

TMSU_DEF const char* tmsu_find_char_ignore_case_ansi_v(tmsu_view_t str, char c) {
    return tmsu_find_char_ignore_case_ansi_n(str.first, str.last, c);
}

TMSU_DEF const char* tmsu_find_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_find_ignore_case_ansi_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF const char* tmsu_find_char_unescaped_v(tmsu_view_t str, char c, char escape_char) {
    return tmsu_find_char_unescaped_n(str.first, str.last, c, escape_char);
}

TMSU_DEF const char* tmsu_find_first_of_unescaped_v(tmsu_view_t str, tmsu_view_t find_str, char escape_char) {
    return tmsu_find_first_of_unescaped_n(str.first, str.last, find_str.first, find_str.last, escape_char);
}

TMSU_DEF const char* tmsu_find_word_end_v(tmsu_view_t str) {
    return tmsu_find_word_end_n(str.first, str.last);
}

TMSU_DEF const char* tmsu_find_word_end_v_ex(tmsu_view_t str, tmsu_view_t word_separators) {
    return tmsu_find_word_end_n_ex(str.first, str.last, word_separators.first, word_separators.last);
}

TMSU_DEF const char* tmsu_find_word_start_v(tmsu_view_t str) {
    return tmsu_find_word_start_n(str.first, str.last);
}

TMSU_DEF const char* tmsu_find_word_start_v_ex(tmsu_view_t str, tmsu_view_t word_separators) {
    return tmsu_find_word_start_n_ex(str.first, str.last, word_separators.first, word_separators.last);
}

TMSU_DEF const char* tmsu_trim_left_v(tmsu_view_t str) {
    return tmsu_trim_left_n(str.first, str.last);
}

TMSU_DEF const char* tmsu_trim_right_v(tmsu_view_t str) {
    return tmsu_trim_right_n(str.first, str.last);
}

TMSU_DEF tmsu_view_t tmsu_trim_n(const char* first, const char* last) {
    return tmsu_view_n(tmsu_trim_left_n(first, last), tmsu_trim_right_n(first, last));
}

TMSU_DEF tmsu_view_t tmsu_trim_v(tmsu_view_t str) {
    return tmsu_view_n(tmsu_trim_left_n(str.first, str.last), tmsu_trim_right_n(str.first, str.last));
}

TMSU_DEF int tmsu_compare_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_compare_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF int tmsu_compare_ignore_case_ansi_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_compare_ignore_case_ansi_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF int tmsu_human_compare_ansi_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_human_compare_ansi_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF tm_bool tmsu_equals_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_equals_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF tm_bool tmsu_equals_ignore_case_ansi_v(tmsu_view_t a, tmsu_view_t b) {
    return tmsu_equals_ignore_case_ansi_n(a.first, a.last, b.first, b.last);
}

TMSU_DEF tm_bool tmsu_starts_with_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_starts_with_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF tm_bool tmsu_ends_with_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_ends_with_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_starts_with_ignore_case_ansi_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi_v(tmsu_view_t str, tmsu_view_t find_str) {
    return tmsu_ends_with_ignore_case_ansi_n(str.first, str.last, find_str.first, find_str.last);
}

TMSU_DEF tm_bool tmsu_next_token_v(tmsu_view_t* tokenizer, tmsu_view_t delimiters, tmsu_view_t* out) {
    return tmsu_next_token_n(tokenizer, delimiters.first, delimiters.last, out);
}

TMSU_DEF tm_bool tmsu_next_token_skip_empty_v(tmsu_view_t* tokenizer, tmsu_view_t delimiters, tmsu_view_t* out) {
    return tmsu_next_token_skip_empty_n(tokenizer, delimiters.first, delimiters.last, out);
}