#define TMSU_SV_BEGIN(str) TM_STRING_VIEW_DATA(str)
#define TMSU_SV_END(str) (TM_STRING_VIEW_DATA(str) + TM_STRING_VIEW_SIZE(str))

tm_bool tmsu_next_token(tmsu_tokenizer* tokenizer, const char* delimiters, TM_STRING_VIEW* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->current);
    TM_ASSERT(delimiters);

    /* Skip delimiters at the beginning. */
    tokenizer->current = tmsu_find_first_not_of(tokenizer->current, delimiters);
    if (!*tokenizer->current) return TM_FALSE;
    /* Skip skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of(tokenizer->current, delimiters);
    if (out) *out = TM_STRING_VIEW{tokenizer->current, tmsu_distance(tokenizer->current, next)};
    tokenizer->current = next;
    return TM_TRUE;
}
tm_bool tmsu_next_token_n(tmsu_tokenizer_n* tokenizer, const char* delimiters_first, const char* delimiters_last,
                          TM_STRING_VIEW* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->first && tokenizer->first <= tokenizer->last);
    TM_ASSERT(delimiters_first && delimiters_first <= delimiters_last);

    /* Skip delimiters at the beginning. */
    tokenizer->first = tmsu_find_first_not_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (tokenizer->first == tokenizer->last) return TM_FALSE;
    /* Skip skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (out) *out = TM_STRING_VIEW{tokenizer->first, tmsu_distance(tokenizer->first, next)};
    tokenizer->first = next;
    return TM_TRUE;
}
TM_STRING_VIEW tmsu_trim_left(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    first = tmsu_trim_left_n(first, last);
    return TM_STRING_VIEW{first, tmsu_distance(first, last)};
}
TM_STRING_VIEW tmsu_trim_right(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    last = tmsu_trim_right_n(first, last);
    return TM_STRING_VIEW{first, tmsu_distance(first, last)};
}
TM_STRING_VIEW tmsu_trim(TM_STRING_VIEW str) {
    const char* first = TMSU_SV_BEGIN(str);
    const char* last = TMSU_SV_END(str);
    first = tmsu_trim_left_n(first, last);
    last = tmsu_trim_right_n(first, last);
    return TM_STRING_VIEW{first, tmsu_distance(first, last)};
}

int tmsu_compare(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_compare_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}
int tmsu_compare_ignore_case(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_compare_ignore_case_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}
int tmsu_human_compare(TM_STRING_VIEW a, TM_STRING_VIEW b) {
    return tmsu_human_compare_n(TMSU_SV_BEGIN(a), TMSU_SV_END(a), TMSU_SV_BEGIN(b), TMSU_SV_END(b));
}

tm_bool tmsu_starts_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_starts_with_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
tm_bool tmsu_ends_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_ends_with_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str), TMSU_SV_END(find_str));
}
tm_bool tmsu_starts_with_ignore_case(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_starts_with_ignore_case_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str),
                                          TMSU_SV_END(find_str));
}
tm_bool tmsu_ends_with_ignore_case(TM_STRING_VIEW str, TM_STRING_VIEW find_str) {
    return tmsu_ends_with_ignore_case_n(TMSU_SV_BEGIN(str), TMSU_SV_END(str), TMSU_SV_BEGIN(find_str),
                                        TMSU_SV_END(find_str));
}

#undef TMSU_SV_BEGIN
#undef TMSU_SV_END