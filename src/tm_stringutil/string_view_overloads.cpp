#define TMSU_SV_BEGIN(str) TM_STRING_VIEW_DATA(str)
#define TMSU_SV_END(str) (TM_STRING_VIEW_DATA(str) + TM_STRING_VIEW_SIZE(str))

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