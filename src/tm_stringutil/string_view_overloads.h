const char* tmsu_find_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                const char* not_found);
const char* tmsu_find_first_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_first_not_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_of_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                   const char* not_found);
const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_last_not_of_n_ex(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                       const char* not_found);
const char* tmsu_find_ignore_case_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str);
const char* tmsu_find_first_of_unescaped_n(const char* str_first, const char* str_last, TM_STRING_VIEW find_str,
                                           char escape_char);

TM_STRING_VIEW tmsu_trim_left(TM_STRING_VIEW str);
TM_STRING_VIEW tmsu_trim_right(TM_STRING_VIEW str);
TM_STRING_VIEW tmsu_trim(TM_STRING_VIEW str);

int tmsu_compare(TM_STRING_VIEW a, TM_STRING_VIEW b);
int tmsu_compare_ignore_case(TM_STRING_VIEW a, TM_STRING_VIEW b);
int tmsu_human_compare(TM_STRING_VIEW a, TM_STRING_VIEW b);

tm_bool tmsu_equals(TM_STRING_VIEW a, TM_STRING_VIEW b);
tm_bool tmsu_equals_ignore_case(TM_STRING_VIEW a, TM_STRING_VIEW b);

tm_bool tmsu_starts_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_ends_with(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_starts_with_ignore_case(TM_STRING_VIEW str, TM_STRING_VIEW find_str);
tm_bool tmsu_ends_with_ignore_case(TM_STRING_VIEW str, TM_STRING_VIEW find_str);