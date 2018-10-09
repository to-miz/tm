TM_STRING_VIEW tmsu_trim_left(TM_STRING_VIEW str);
TM_STRING_VIEW tmsu_trim_right(TM_STRING_VIEW str);
TM_STRING_VIEW tmsu_trim(TM_STRING_VIEW str);

int tmsu_compare(TM_STRING_VIEW a, TM_STRING_VIEW b);
int tmsu_compare_ignore_case(TM_STRING_VIEW a, TM_STRING_VIEW b);
int tmsu_human_compare(TM_STRING_VIEW a, TM_STRING_VIEW b);

tm_bool tmsu_starts_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_ends_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_starts_with_ignore_case(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_ends_with_ignore_case(TM_STRING_VIEW str, TM_STRING_VIEW find_str);