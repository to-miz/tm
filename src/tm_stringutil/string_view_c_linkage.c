TMSU_DEF tmsu_string_view tmsu_trim(const char* str) {
    TM_ASSERT(str);
    return tmsu_trim_n(str, str + TM_STRLEN(str));
}
TMSU_DEF tmsu_string_view tmsu_trim_n(const char* first, const char* last) {
    first = tmsu_trim_left_n(first, last);
    last = tmsu_trim_right_n(first, last);
    return TMSU_STRING_VIEW_MAKE(first, tmsu_distance(first, last));
}