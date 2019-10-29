TEST_CASE("i_number_double_huge_neg_exp" * doctest::may_fail(true)) {
    const char* json = "[123.456e-789]";
    CHECK(check_json(json, 14, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 14, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_huge_exp" * doctest::may_fail(true)) {
    const char* json = "[0.4e00669999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999969999999006]";
    CHECK(check_json(json, 137, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 137, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_neg_int_huge_exp" * doctest::may_fail(true)) {
    const char* json = "[-1e+9999]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_pos_double_huge_exp" * doctest::may_fail(true)) {
    const char* json = "[1.5e+9999]";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 11, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_real_neg_overflow" * doctest::may_fail(true)) {
    const char* json = "[-123123e100000]";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 16, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_real_pos_overflow" * doctest::may_fail(true)) {
    const char* json = "[123123e100000]";
    CHECK(check_json(json, 15, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 15, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_real_underflow" * doctest::may_fail(true)) {
    const char* json = "[123e-10000000]";
    CHECK(check_json(json, 15, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 15, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_too_big_neg_int" * doctest::may_fail(true)) {
    const char* json = "[-123123123123123123123123123123]";
    CHECK(check_json(json, 33, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 33, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_too_big_pos_int" * doctest::may_fail(true)) {
    const char* json = "[100000000000000000000]";
    CHECK(check_json(json, 23, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 23, JSON_READER_JSON5) == true);
}
TEST_CASE("i_number_very_big_negative_int" * doctest::may_fail(true)) {
    const char* json = "[-237462374673276894279832749832423479823246327846]";
    CHECK(check_json(json, 51, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 51, JSON_READER_JSON5) == true);
}
TEST_CASE("i_object_key_lone_2nd_surrogate" * doctest::may_fail(true)) {
    const char* json = "{\"\\uDFAA\":0}";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_1st_surrogate_but_2nd_missing" * doctest::may_fail(true)) {
    const char* json = "[\"\\uDADA\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_1st_valid_surrogate_2nd_invalid" * doctest::may_fail(true)) {
    const char* json = "[\"\\uD888\\u1234\"]";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_incomplete_surrogates_escape_valid" * doctest::may_fail(true)) {
    const char* json = "[\"\\uD800\\uD800\\n\"]";
    CHECK(check_json(json, 18, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_incomplete_surrogate_and_escape_valid" * doctest::may_fail(true)) {
    const char* json = "[\"\\uD800\\n\"]";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_incomplete_surrogate_pair" * doctest::may_fail(true)) {
    const char* json = "[\"\\uDd1ea\"]";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_invalid_lonely_surrogate" * doctest::may_fail(true)) {
    const char* json = "[\"\\ud800\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_invalid_surrogate" * doctest::may_fail(true)) {
    const char* json = "[\"\\ud800abc\"]";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_invalid_utf-8" * doctest::may_fail(true)) {
    const char* json = "[\"\xFF""\"]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("i_string_inverted_surrogates_U+1D11E" * doctest::may_fail(true)) {
    const char* json = "[\"\\uDd1e\\uD834\"]";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_iso_latin_1" * doctest::may_fail(true)) {
    const char* json = "[\"\xE9""\"]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("i_string_lone_second_surrogate" * doctest::may_fail(true)) {
    const char* json = "[\"\\uDFAA\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("i_string_lone_utf8_continuation_byte" * doctest::may_fail(true)) {
    const char* json = "[\"\x81""\"]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("i_string_not_in_unicode_range" * doctest::may_fail(true)) {
    const char* json = "[\"\xF4""\xBF""\xBF""\xBF""\"]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 8, JSON_READER_JSON5) == true);
}
TEST_CASE("i_string_overlong_sequence_2_bytes" * doctest::may_fail(true)) {
    const char* json = "[\"\xC0""\xAF""\"]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("i_string_overlong_sequence_6_bytes" * doctest::may_fail(true)) {
    const char* json = "[\"\xFC""\x83""\xBF""\xBF""\xBF""\xBF""\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("i_string_overlong_sequence_6_bytes_null" * doctest::may_fail(true)) {
    const char* json = "[\"\xFC""\x80""\x80""\x80""\x80""\x80""\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("i_string_truncated-utf-8" * doctest::may_fail(true)) {
    const char* json = "[\"\xE0""\xFF""\"]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("i_string_UTF-8_invalid_sequence" * doctest::may_fail(true)) {
    const char* json = "[\"\xE6""\x97""\xA5""\xD1""\x88""\xFA""\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("i_structure_500_nested_arrays" * doctest::may_fail(true)) {
    const char* json = "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]";
    CHECK(check_json(json, 1000, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 1000, JSON_READER_JSON5) == true);
}
TEST_CASE("n_array_1_true_without_comma") {
    const char* json = "[1 true]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_a_invalid_utf8") {
    const char* json = "[a\xE5""]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_colon_instead_of_comma") {
    const char* json = "[\"\": 1]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_comma_after_close") {
    const char* json = "[\"\"],";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_comma_and_number") {
    const char* json = "[,1]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_double_comma") {
    const char* json = "[1,,2]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_double_extra_comma") {
    const char* json = "[\"x\",,]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_extra_close") {
    const char* json = "[\"x\"]]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_extra_comma") {
    const char* json = "[\"\",]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_incomplete") {
    const char* json = "[\"x\"";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_incomplete_invalid_value") {
    const char* json = "[x";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_inner_array_no_comma") {
    const char* json = "[3[4]]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_invalid_utf8") {
    const char* json = "[\xFF""]";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_items_separated_by_semicolon") {
    const char* json = "[1:2]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_just_comma") {
    const char* json = "[,]";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_just_minus") {
    const char* json = "[-]";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_missing_value") {
    const char* json = "[   , \"\"]";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_newlines_unclosed") {
    const char* json = "[\"a\",\xD""\n4\xD""\n,1,";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_number_and_comma") {
    const char* json = "[1,]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_number_and_several_commas") {
    const char* json = "[1,,]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_spaces_vertical_tab_formfeed") {
    const char* json = "[\"\xB""a\"\\f]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_star_inside") {
    const char* json = "[*]";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_unclosed") {
    const char* json = "[\"\"";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_unclosed_trailing_comma") {
    const char* json = "[1,";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_unclosed_with_new_lines") {
    const char* json = "[1,\xD""\n1\xD""\n,1";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_array_unclosed_with_object_inside") {
    const char* json = "[{}";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_incomplete_false") {
    const char* json = "[fals]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_incomplete_null") {
    const char* json = "[nul]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_incomplete_true") {
    const char* json = "[tru]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_multidigit_number_then_00") {
    const char* json = "123\x0""";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_++") {
    const char* json = "[++1234]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_+1") {
    const char* json = "[+1]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_+Inf") {
    const char* json = "[+Inf]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_-01") {
    const char* json = "[-01]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_-1.0.") {
    const char* json = "[-1.0.]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_-2.") {
    const char* json = "[-2.]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_-NaN") {
    const char* json = "[-NaN]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_.-1") {
    const char* json = "[.-1]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_.2e-3") {
    const char* json = "[.2e-3]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_0.1.2") {
    const char* json = "[0.1.2]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_0.3e+") {
    const char* json = "[0.3e+]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_0.3e") {
    const char* json = "[0.3e]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_0.e1") {
    const char* json = "[0.e1]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_0e+") {
    const char* json = "[0e+]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_0e") {
    const char* json = "[0e]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_0_capital_E+") {
    const char* json = "[0E+]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_0_capital_E") {
    const char* json = "[0E]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_1.0e+") {
    const char* json = "[1.0e+]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_1.0e-") {
    const char* json = "[1.0e-]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_1.0e") {
    const char* json = "[1.0e]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_1eE2") {
    const char* json = "[1eE2]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_1_000") {
    const char* json = "[1 000.0]";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_2.e+3") {
    const char* json = "[2.e+3]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_2.e-3") {
    const char* json = "[2.e-3]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_2.e3") {
    const char* json = "[2.e3]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_9.e+") {
    const char* json = "[9.e+]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_expression") {
    const char* json = "[1+2]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_hex_1_digit") {
    const char* json = "[0x1]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_hex_2_digits") {
    const char* json = "[0x42]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_Inf") {
    const char* json = "[Inf]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_infinity") {
    const char* json = "[Infinity]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_invalid+-") {
    const char* json = "[0e+-1]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_invalid-negative-real") {
    const char* json = "[-123.123foo]";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_invalid-utf-8-in-bigger-int") {
    const char* json = "[123\xE5""]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_invalid-utf-8-in-exponent") {
    const char* json = "[1e1\xE5""]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_invalid-utf-8-in-int") {
    const char* json = "[0\xE5""]\xD""\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_minus_infinity") {
    const char* json = "[-Infinity]";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_minus_sign_with_trailing_garbage") {
    const char* json = "[-foo]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_minus_space_1") {
    const char* json = "[- 1]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_NaN") {
    const char* json = "[NaN]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_neg_int_starting_with_zero") {
    const char* json = "[-012]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_neg_real_without_int_part") {
    const char* json = "[-.123]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_neg_with_garbage_at_end") {
    const char* json = "[-1x]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_real_garbage_after_e") {
    const char* json = "[1ea]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_real_without_fractional_part") {
    const char* json = "[1.]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_real_with_invalid_utf8_after_e") {
    const char* json = "[1e\xE5""]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_starting_with_dot") {
    const char* json = "[.123]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_U+FF11_fullwidth_digit_one") {
    const char* json = "[\xEF""\xBC""\x91""]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_with_alpha") {
    const char* json = "[1.2a-3]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_with_alpha_char") {
    const char* json = "[1.8011670033376514H-308]";
    CHECK(check_json(json, 25, JSON_READER_STRICT) == false);
}
TEST_CASE("n_number_with_leading_zero") {
    const char* json = "[012]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_bad_value") {
    const char* json = "[\"x\", truth]";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_bracket_key") {
    const char* json = "{[: \"x\"}\xD""\n";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_comma_instead_of_colon") {
    const char* json = "{\"x\", null}";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_double_colon") {
    const char* json = "{\"x\"::\"b\"}";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_emoji") {
    const char* json = "{\xF0""\x9F""\x87""\xA8""\xF0""\x9F""\x87""\xAD""}";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_garbage_at_end") {
    const char* json = "{\"a\":\"a\" 123}";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_key_with_single_quotes") {
    const char* json = "{key: 'value'}";
    CHECK(check_json(json, 14, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_lone_continuation_byte_in_key_and_trailing_comma") {
    const char* json = "{\"\xB9""\":\"0\",}";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_missing_colon") {
    const char* json = "{\"a\" b}";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_missing_key") {
    const char* json = "{:\"b\"}";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_missing_semicolon") {
    const char* json = "{\"a\" \"b\"}";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_missing_value") {
    const char* json = "{\"a\":";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_no-colon") {
    const char* json = "{\"a\"";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_non_string_key") {
    const char* json = "{1:1}";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_non_string_key_but_huge_number_instead") {
    const char* json = "{9999E9999:1}";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_repeated_null_null") {
    const char* json = "{null:null,null:null}";
    CHECK(check_json(json, 21, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_several_trailing_commas") {
    const char* json = "{\"id\":0,,,,,}";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_single_quote") {
    const char* json = "{'a':0}";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_trailing_comma") {
    const char* json = "{\"id\":0,}";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_trailing_comment") {
    const char* json = "{\"a\":\"b\"}/**/";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_trailing_comment_open") {
    const char* json = "{\"a\":\"b\"}/**//";
    CHECK(check_json(json, 14, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_trailing_comment_slash_open") {
    const char* json = "{\"a\":\"b\"}//";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_trailing_comment_slash_open_incomplete") {
    const char* json = "{\"a\":\"b\"}/";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_two_commas_in_a_row") {
    const char* json = "{\"a\":\"b\",,\"c\":\"d\"}";
    CHECK(check_json(json, 18, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_unquoted_key") {
    const char* json = "{a: \"b\"}";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_unterminated-value") {
    const char* json = "{\"a\":\"a";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_with_single_string") {
    const char* json = "{ \"foo\" : \"bar\", \"a\" }";
    CHECK(check_json(json, 22, JSON_READER_STRICT) == false);
}
TEST_CASE("n_object_with_trailing_garbage") {
    const char* json = "{\"a\":\"b\"}#";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_single_space") {
    const char* json = " ";
    CHECK(check_json(json, 1, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_1_surrogate_then_escape") {
    const char* json = "[\"\\uD800\\\"]";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_1_surrogate_then_escape_u") {
    const char* json = "[\"\\uD800\\u\"]";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_1_surrogate_then_escape_u1") {
    const char* json = "[\"\\uD800\\u1\"]";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_1_surrogate_then_escape_u1x") {
    const char* json = "[\"\\uD800\\u1x\"]";
    CHECK(check_json(json, 14, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_accentuated_char_no_quotes") {
    const char* json = "[\xC3""\xA9""]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_backslash_00") {
    const char* json = "[\"\\\x0""\"]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_escaped_backslash_bad") {
    const char* json = "[\"\\\\\\\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_escaped_ctrl_char_tab") {
    const char* json = "[\"\\\x9""\"]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_escaped_emoji") {
    const char* json = "[\"\\\xF0""\x9F""\x8C""\x80""\"]";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_escape_x") {
    const char* json = "[\"\\x00\"]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_incomplete_escape") {
    const char* json = "[\"\\\"]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_incomplete_escaped_character") {
    const char* json = "[\"\\u00A\"]";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_incomplete_surrogate") {
    const char* json = "[\"\\uD834\\uDd\"]";
    CHECK(check_json(json, 14, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_incomplete_surrogate_escape_invalid") {
    const char* json = "[\"\\uD800\\uD800\\x\"]";
    CHECK(check_json(json, 18, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_invalid-utf-8-in-escape") {
    const char* json = "[\"\\u\xE5""\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_invalid_backslash_esc") {
    const char* json = "[\"\\a\"]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_invalid_unicode_escape") {
    const char* json = "[\"\\uqqqq\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_invalid_utf8_after_escape") {
    const char* json = "[\"\\\xE5""\"]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_leading_uescaped_thinspace") {
    const char* json = "[\\u0020\"asd\"]";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_no_quotes_with_bad_escape") {
    const char* json = "[\\n]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_single_doublequote") {
    const char* json = "\"";
    CHECK(check_json(json, 1, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_single_quote") {
    const char* json = "['single quote']";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_single_string_no_double_quotes") {
    const char* json = "abc";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_start_escape_unclosed") {
    const char* json = "[\"\\";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_unescaped_crtl_char") {
    const char* json = "[\"a\x0""a\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_unescaped_newline") {
    const char* json = "[\"new\xD""\nline\"]";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_unescaped_tab") {
    const char* json = "[\"\x9""\"]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_unicode_CapitalU") {
    const char* json = "\"\\UA66D\"";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == false);
}
TEST_CASE("n_string_with_trailing_garbage") {
    const char* json = "\"\"x";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_angle_bracket_.") {
    const char* json = "<.>";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_angle_bracket_null") {
    const char* json = "[<null>]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_array_trailing_garbage") {
    const char* json = "[1]x";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_array_with_extra_array_close") {
    const char* json = "[1]]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_array_with_unclosed_string") {
    const char* json = "[\"asd]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_ascii-unicode-identifier") {
    const char* json = "a\xC3""\xA5""";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_capitalized_True") {
    const char* json = "[True]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_close_unopened_array") {
    const char* json = "1]";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_comma_instead_of_closing_brace") {
    const char* json = "{\"x\": true,";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_double_array") {
    const char* json = "[][]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_end_array") {
    const char* json = "]";
    CHECK(check_json(json, 1, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_lone-invalid-utf-8") {
    const char* json = "\xE5""";
    CHECK(check_json(json, 1, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_lone-open-bracket") {
    const char* json = "[";
    CHECK(check_json(json, 1, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_no_data") {
    const char* json = "";
    CHECK(check_json(json, 0, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_null-byte-outside-string") {
    const char* json = "[\x0""]";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_number_with_trailing_garbage") {
    const char* json = "2@";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_object_followed_by_closing_object") {
    const char* json = "{}}";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_object_unclosed_no_value") {
    const char* json = "{\"\":";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_object_with_comment") {
    const char* json = "{\"a\":/*comment*/\"b\"}";
    CHECK(check_json(json, 20, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_object_with_trailing_garbage") {
    const char* json = "{\"a\": true} \"x\"";
    CHECK(check_json(json, 15, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_array_apostrophe") {
    const char* json = "['";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_array_comma") {
    const char* json = "[,";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_array_open_object") {
    const char* json = "[{";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_array_open_string") {
    const char* json = "[\"a";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_array_string") {
    const char* json = "[\"a\"";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_object") {
    const char* json = "{";
    CHECK(check_json(json, 1, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_object_close_array") {
    const char* json = "{]";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_object_comma") {
    const char* json = "{,";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_object_open_array") {
    const char* json = "{[";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_object_open_string") {
    const char* json = "{\"a";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_object_string_with_apostrophes") {
    const char* json = "{'a'";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_open_open") {
    const char* json = "[\"\\{[\"\\{[\"\\{[\"\\{";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_single_eacute") {
    const char* json = "\xE9""";
    CHECK(check_json(json, 1, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_single_star") {
    const char* json = "*";
    CHECK(check_json(json, 1, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_trailing_#") {
    const char* json = "{\"a\":\"b\"}#{}";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_U+2060_word_joined") {
    const char* json = "[\xE2""\x81""\xA0""]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_uescaped_LF_before_string") {
    const char* json = "[\\u000A\"\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_unclosed_array") {
    const char* json = "[1";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_unclosed_array_partial_null") {
    const char* json = "[ false, nul";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_unclosed_array_unfinished_false") {
    const char* json = "[ true, fals";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_unclosed_array_unfinished_true") {
    const char* json = "[ false, tru";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_unclosed_object") {
    const char* json = "{\"asd\":\"asd\"";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_unicode-identifier") {
    const char* json = "\xC3""\xA5""";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_whitespace_formfeed") {
    const char* json = "[\xC""]";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
}
TEST_CASE("n_structure_whitespace_U+2060_word_joiner") {
    const char* json = "[\xE2""\x81""\xA0""]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
}
TEST_CASE("y_array_arraysWithSpaces") {
    const char* json = "[[]   ]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_empty-string") {
    const char* json = "[\"\"]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_empty") {
    const char* json = "[]";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 2, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_ending_with_newline") {
    const char* json = "[\"a\"]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_false") {
    const char* json = "[false]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_heterogeneous") {
    const char* json = "[null, 1, \"1\", {}]";
    CHECK(check_json(json, 18, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 18, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_null") {
    const char* json = "[null]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_with_1_and_newline") {
    const char* json = "[1\xD""\n]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_with_leading_space") {
    const char* json = " [1]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_with_several_null") {
    const char* json = "[1,null,null,null,2]";
    CHECK(check_json(json, 20, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 20, JSON_READER_JSON5) == true);
}
TEST_CASE("y_array_with_trailing_space") {
    const char* json = "[2] ";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number") {
    const char* json = "[123e65]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 8, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_0e+1") {
    const char* json = "[0e+1]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_0e1") {
    const char* json = "[0e1]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_after_space") {
    const char* json = "[ 4]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_double_close_to_zero") {
    const char* json = "[-0.000000000000000000000000000000000000000000000000000000000000000000000000000001]\xD""\n";
    CHECK(check_json(json, 85, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 85, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_int_with_exp") {
    const char* json = "[20e1]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_minus_zero") {
    const char* json = "[-0]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_negative_int") {
    const char* json = "[-123]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_negative_one") {
    const char* json = "[-1]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_negative_zero") {
    const char* json = "[-0]";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_real_capital_e") {
    const char* json = "[1E22]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_real_capital_e_neg_exp") {
    const char* json = "[1E-2]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_real_capital_e_pos_exp") {
    const char* json = "[1E+2]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_real_exponent") {
    const char* json = "[123e45]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 8, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_real_fraction_exponent") {
    const char* json = "[123.456e78]";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 12, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_real_neg_exp") {
    const char* json = "[1e-2]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_real_pos_exponent") {
    const char* json = "[1e+2]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_simple_int") {
    const char* json = "[123]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("y_number_simple_real") {
    const char* json = "[123.456789]";
    CHECK(check_json(json, 12, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 12, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object") {
    const char* json = "{\"asd\":\"sdf\", \"dfg\":\"fgh\"}";
    CHECK(check_json(json, 26, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 26, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_basic") {
    const char* json = "{\"asd\":\"sdf\"}";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 13, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_duplicated_key") {
    const char* json = "{\"a\":\"b\",\"a\":\"c\"}";
    CHECK(check_json(json, 17, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 17, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_duplicated_key_and_value") {
    const char* json = "{\"a\":\"b\",\"a\":\"b\"}";
    CHECK(check_json(json, 17, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 17, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_empty") {
    const char* json = "{}";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 2, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_empty_key") {
    const char* json = "{\"\":0}";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_escaped_null_in_key") {
    const char* json = "{\"foo\\u0000bar\": 42}";
    CHECK(check_json(json, 20, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 20, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_extreme_numbers") {
    const char* json = "{ \"min\": -1.0e+28, \"max\": 1.0e+28 }";
    CHECK(check_json(json, 35, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 35, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_long_strings") {
    const char* json = "{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}";
    CHECK(check_json(json, 108, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 108, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_simple") {
    const char* json = "{\"a\":[]}";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 8, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_string_unicode") {
    const char* json = "{\"title\":\"\\u041f\\u043e\\u043b\\u0442\\u043e\\u0440\\u0430 \\u0417\\u0435\\u043c\\u043b\\u0435\\u043a\\u043e\\u043f\\u0430\" }";
    CHECK(check_json(json, 110, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 110, JSON_READER_JSON5) == true);
}
TEST_CASE("y_object_with_newlines") {
    const char* json = "{\xD""\n\"a\": \"b\"\xD""\n}";
    CHECK(check_json(json, 14, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 14, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_1_2_3_bytes_UTF-8_sequences") {
    const char* json = "[\"\\u0060\\u012a\\u12AB\"]";
    CHECK(check_json(json, 22, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 22, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_accepted_surrogate_pair") {
    const char* json = "[\"\\uD801\\udc37\"]";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 16, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_accepted_surrogate_pairs") {
    const char* json = "[\"\\ud83d\\ude39\\ud83d\\udc8d\"]";
    CHECK(check_json(json, 28, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 28, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_allowed_escapes") {
    const char* json = "[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]";
    CHECK(check_json(json, 20, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 20, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_backslash_and_u_escaped_zero") {
    const char* json = "[\"\\\\u0000\"]";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 11, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_backslash_doublequotes") {
    const char* json = "[\"\\\"\"]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_comments") {
    const char* json = "[\"a/*b*/c/*d//e\"]";
    CHECK(check_json(json, 17, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 17, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_double_escape_a") {
    const char* json = "[\"\\\\a\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_double_escape_n") {
    const char* json = "[\"\\\\n\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_escaped_control_character") {
    const char* json = "[\"\\u0012\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_escaped_noncharacter") {
    const char* json = "[\"\\uFFFF\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_in_array") {
    const char* json = "[\"asd\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_in_array_with_leading_space") {
    const char* json = "[ \"asd\"]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 8, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_last_surrogates_1_and_2") {
    const char* json = "[\"\\uDBFF\\uDFFF\"]";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 16, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_nbsp_uescaped") {
    const char* json = "[\"new\\u00A0line\"]";
    CHECK(check_json(json, 17, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 17, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_nonCharacterInUTF-8_U+10FFFF") {
    const char* json = "[\"\xF4""\x8F""\xBF""\xBF""\"]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 8, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_nonCharacterInUTF-8_U+FFFF") {
    const char* json = "[\"\xEF""\xBF""\xBF""\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_null_escape") {
    const char* json = "[\"\\u0000\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_one-byte-utf-8") {
    const char* json = "[\"\\u002c\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_pi") {
    const char* json = "[\"\xCF""\x80""\"]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_reservedCharacterInUTF-8_U+1BFFF") {
    const char* json = "[\"\xF0""\x9B""\xBF""\xBF""\"]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 8, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_simple_ascii") {
    const char* json = "[\"asd \"]";
    CHECK(check_json(json, 8, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 8, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_space") {
    const char* json = "\" \"";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 3, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF") {
    const char* json = "[\"\\uD834\\uDd1e\"]";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 16, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_three-byte-utf-8") {
    const char* json = "[\"\\u0821\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_two-byte-utf-8") {
    const char* json = "[\"\\u0123\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_u+2028_line_sep") {
    const char* json = "[\"\xE2""\x80""\xA8""\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_u+2029_par_sep") {
    const char* json = "[\"\xE2""\x80""\xA9""\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_uEscape") {
    const char* json = "[\"\\u0061\\u30af\\u30EA\\u30b9\"]";
    CHECK(check_json(json, 28, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 28, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_uescaped_newline") {
    const char* json = "[\"new\\u000Aline\"]";
    CHECK(check_json(json, 17, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 17, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unescaped_char_delete") {
    const char* json = "[\"\x7F""\"]";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode") {
    const char* json = "[\"\\uA66D\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicodeEscapedBackslash") {
    const char* json = "[\"\\u005C\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode_2") {
    const char* json = "[\"\xE2""\x8D""\x82""\xE3""\x88""\xB4""\xE2""\x8D""\x82""\"]";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 13, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode_escaped_double_quote") {
    const char* json = "[\"\\u0022\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode_U+10FFFE_nonchar") {
    const char* json = "[\"\\uDBFF\\uDFFE\"]";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 16, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode_U+1FFFE_nonchar") {
    const char* json = "[\"\\uD83F\\uDFFE\"]";
    CHECK(check_json(json, 16, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 16, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode_U+200B_ZERO_WIDTH_SPACE") {
    const char* json = "[\"\\u200B\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode_U+2064_invisible_plus") {
    const char* json = "[\"\\u2064\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode_U+FDD0_nonchar") {
    const char* json = "[\"\\uFDD0\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_unicode_U+FFFE_nonchar") {
    const char* json = "[\"\\uFFFE\"]";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_utf8") {
    const char* json = "[\"\xE2""\x82""\xAC""\xF0""\x9D""\x84""\x9E""\"]";
    CHECK(check_json(json, 11, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 11, JSON_READER_JSON5) == true);
}
TEST_CASE("y_string_with_del_character") {
    const char* json = "[\"a\x7F""a\"]";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_lonely_false") {
    const char* json = "false";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_lonely_int") {
    const char* json = "42";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 2, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_lonely_negative_real") {
    const char* json = "-0.1";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_lonely_null") {
    const char* json = "null";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_lonely_string") {
    const char* json = "\"asd\"";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_lonely_true") {
    const char* json = "true";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_string_empty") {
    const char* json = "\"\"";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 2, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_trailing_newline") {
    const char* json = "[\"a\"]\xD""\n";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_true_in_array") {
    const char* json = "[true]";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("y_structure_whitespace_array") {
    const char* json = " [] ";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("empty-array") {
    const char* json = "[]";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 2, JSON_READER_JSON5) == true);
}
TEST_CASE("leading-comma-array") {
    const char* json = "[\n    ,null\n]";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 13, JSON_READER_JSON5) == false);
}
TEST_CASE("lone-trailing-comma-array") {
    const char* json = "[\n    ,\n]";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 9, JSON_READER_JSON5) == false);
}
TEST_CASE("no-comma-array") {
    const char* json = "[\n    true\n    false\n]";
    CHECK(check_json(json, 22, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 22, JSON_READER_JSON5) == false);
}
TEST_CASE("regular-array") {
    const char* json = "[\n    true,\n    false,\n    null\n]";
    CHECK(check_json(json, 33, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 33, JSON_READER_JSON5) == true);
}
TEST_CASE("trailing-comma-array") {
    const char* json = "[\n    null,\n]";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 13, JSON_READER_JSON5) == true);
}
TEST_CASE("block-comment-following-array-element") {
    const char* json = "[\n    false\n    /*\n        true\n    */\n]";
    CHECK(check_json(json, 40, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 40, JSON_READER_JSON5) == true);
}
TEST_CASE("block-comment-following-top-level-value") {
    const char* json = "null\n/*\n    Some non-comment top-level value is needed;\n    we use null above.\n*/";
    CHECK(check_json(json, 81, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 81, JSON_READER_JSON5) == true);
}
TEST_CASE("block-comment-in-string") {
    const char* json = "\"This /* block comment */ isn't really a block comment.\"";
    CHECK(check_json(json, 56, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 56, JSON_READER_JSON5) == true);
}
TEST_CASE("block-comment-preceding-top-level-value") {
    const char* json = "/*\n    Some non-comment top-level value is needed;\n    we use null below.\n*/\nnull";
    CHECK(check_json(json, 81, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 81, JSON_READER_JSON5) == true);
}
TEST_CASE("block-comment-with-asterisks") {
    const char* json = "/**\n * This is a JavaDoc-like block comment.\n * It contains asterisks inside of it.\n * It might also be closed with multiple asterisks.\n * Like this:\n **/\ntrue";
    CHECK(check_json(json, 159, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 159, JSON_READER_JSON5) == true);
}
TEST_CASE("inline-comment-following-array-element") {
    const char* json = "[\n    false   // true\n]";
    CHECK(check_json(json, 23, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 23, JSON_READER_JSON5) == true);
}
TEST_CASE("inline-comment-following-top-level-value") {
    const char* json = "null // Some non-comment top-level value is needed; we use null here.";
    CHECK(check_json(json, 69, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 69, JSON_READER_JSON5) == true);
}
TEST_CASE("inline-comment-in-string") {
    const char* json = "\"This inline comment // isn't really an inline comment.\"";
    CHECK(check_json(json, 56, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 56, JSON_READER_JSON5) == true);
}
TEST_CASE("inline-comment-preceding-top-level-value") {
    const char* json = "// Some non-comment top-level value is needed; we use null below.\nnull";
    CHECK(check_json(json, 70, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 70, JSON_READER_JSON5) == true);
}
TEST_CASE("top-level-block-comment") {
    const char* json = "/*\n    This should fail;\n    comments cannot be the only top-level value.\n*/";
    CHECK(check_json(json, 76, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 76, JSON_READER_JSON5) == false);
}
TEST_CASE("top-level-inline-comment") {
    const char* json = "// This should fail; comments cannot be the only top-level value.";
    CHECK(check_json(json, 65, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 65, JSON_READER_JSON5) == false);
}
TEST_CASE("unterminated-block-comment") {
    const char* json = "true\n/*\n    This block comment doesn't terminate.\n    There was a legitimate value before this,\n    but this is still invalid JS/JSON5.\n";
    CHECK(check_json(json, 136, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 136, JSON_READER_JSON5) == false);
}
TEST_CASE("empty") {
    const char* json = "";
    CHECK(check_json(json, 0, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 0, JSON_READER_JSON5) == false);
}
TEST_CASE("npm-package") {
    const char* json = "{\n  \"name\": \"npm\",\n  \"publishConfig\": {\n    \"proprietary-attribs\": false\n  },\n  \"description\": \"A package manager for node\",\n  \"keywords\": [\n    \"package manager\",\n    \"modules\",\n    \"install\",\n    \"package.json\"\n  ],\n  \"version\": \"1.1.22\",\n  \"preferGlobal\": true,\n  \"config\": {\n    \"publishtest\": false\n  },\n  \"homepage\": \"http://npmjs.org/\",\n  \"author\": \"Isaac Z. Schlueter <i@izs.me> (http://blog.izs.me)\",\n  \"repository\": {\n    \"type\": \"git\",\n    \"url\": \"https://github.com/isaacs/npm\"\n  },\n  \"bugs\": {\n    \"email\": \"npm-@googlegroups.com\",\n    \"url\": \"http://github.com/isaacs/npm/issues\"\n  },\n  \"directories\": {\n    \"doc\": \"./doc\",\n    \"man\": \"./man\",\n    \"lib\": \"./lib\",\n    \"bin\": \"./bin\"\n  },\n  \"main\": \"./lib/npm.js\",\n  \"bin\": \"./bin/npm-cli.js\",\n  \"dependencies\": {\n    \"semver\": \"~1.0.14\",\n    \"ini\": \"1\",\n    \"slide\": \"1\",\n    \"abbrev\": \"1\",\n    \"graceful-fs\": \"~1.1.1\",\n    \"minimatch\": \"~0.2\",\n    \"nopt\": \"1\",\n    \"node-uuid\": \"~1.3\",\n    \"proto-list\": \"1\",\n    \"rimraf\": \"2\",\n    \"request\": \"~2.9\",\n    \"which\": \"1\",\n    \"tar\": \"~0.1.12\",\n    \"fstream\": \"~0.1.17\",\n    \"block-stream\": \"*\",\n    \"inherits\": \"1\",\n    \"mkdirp\": \"0.3\",\n    \"read\": \"0\",\n    \"lru-cache\": \"1\",\n    \"node-gyp\": \"~0.4.1\",\n    \"fstream-npm\": \"0 >=0.0.5\",\n    \"uid-number\": \"0\",\n    \"archy\": \"0\",\n    \"chownr\": \"0\"\n  },\n  \"bundleDependencies\": [\n    \"slide\",\n    \"ini\",\n    \"semver\",\n    \"abbrev\",\n    \"graceful-fs\",\n    \"minimatch\",\n    \"nopt\",\n    \"node-uuid\",\n    \"rimraf\",\n    \"request\",\n    \"proto-list\",\n    \"which\",\n    \"tar\",\n    \"fstream\",\n    \"block-stream\",\n    \"inherits\",\n    \"mkdirp\",\n    \"read\",\n    \"lru-cache\",\n    \"node-gyp\",\n    \"fstream-npm\",\n    \"uid-number\",\n    \"archy\",\n    \"chownr\"\n  ],\n  \"devDependencies\": {\n    \"ronn\": \"https://github.com/isaacs/ronnjs/tarball/master\"\n  },\n  \"engines\": {\n    \"node\": \"0.6 || 0.7 || 0.8\",\n    \"npm\": \"1\"\n  },\n  \"scripts\": {\n    \"test\": \"node ./test/run.js\",\n    \"prepublish\": \"npm prune; rm -rf node_modules/*/{test,example,bench}*; make -j4 doc\",\n    \"dumpconf\": \"env | grep npm | sort | uniq\"\n  },\n  \"licenses\": [\n    {\n      \"type\": \"MIT +no-false-attribs\",\n      \"url\": \"http://github.com/isaacs/npm/raw/master/LICENSE\"\n    }\n  ]\n}\n";
    CHECK(check_json(json, 2187, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 2187, JSON_READER_JSON5) == true);
}
TEST_CASE("npm-package") {
    const char* json = "{\n  name: 'npm',\n  publishConfig: {\n    'proprietary-attribs': false,\n  },\n  description: 'A package manager for node',\n  keywords: [\n    'package manager',\n    'modules',\n    'install',\n    'package.json',\n  ],\n  version: '1.1.22',\n  preferGlobal: true,\n  config: {\n    publishtest: false,\n  },\n  homepage: 'http://npmjs.org/',\n  author: 'Isaac Z. Schlueter <i@izs.me> (http://blog.izs.me)',\n  repository: {\n    type: 'git',\n    url: 'https://github.com/isaacs/npm',\n  },\n  bugs: {\n    email: 'npm-@googlegroups.com',\n    url: 'http://github.com/isaacs/npm/issues',\n  },\n  directories: {\n    doc: './doc',\n    man: './man',\n    lib: './lib',\n    bin: './bin',\n  },\n  main: './lib/npm.js',\n  bin: './bin/npm-cli.js',\n  dependencies: {\n    semver: '~1.0.14',\n    ini: '1',\n    slide: '1',\n    abbrev: '1',\n    'graceful-fs': '~1.1.1',\n    minimatch: '~0.2',\n    nopt: '1',\n    'node-uuid': '~1.3',\n    'proto-list': '1',\n    rimraf: '2',\n    request: '~2.9',\n    which: '1',\n    tar: '~0.1.12',\n    fstream: '~0.1.17',\n    'block-stream': '*',\n    inherits: '1',\n    mkdirp: '0.3',\n    read: '0',\n    'lru-cache': '1',\n    'node-gyp': '~0.4.1',\n    'fstream-npm': '0 >=0.0.5',\n    'uid-number': '0',\n    archy: '0',\n    chownr: '0',\n  },\n  bundleDependencies: [\n    'slide',\n    'ini',\n    'semver',\n    'abbrev',\n    'graceful-fs',\n    'minimatch',\n    'nopt',\n    'node-uuid',\n    'rimraf',\n    'request',\n    'proto-list',\n    'which',\n    'tar',\n    'fstream',\n    'block-stream',\n    'inherits',\n    'mkdirp',\n    'read',\n    'lru-cache',\n    'node-gyp',\n    'fstream-npm',\n    'uid-number',\n    'archy',\n    'chownr',\n  ],\n  devDependencies: {\n    ronn: 'https://github.com/isaacs/ronnjs/tarball/master',\n  },\n  engines: {\n    node: '0.6 || 0.7 || 0.8',\n    npm: '1',\n  },\n  scripts: {\n    test: 'node ./test/run.js',\n    prepublish: 'npm prune; rm -rf node_modules/*/{test,example,bench}*; make -j4 doc',\n    dumpconf: 'env | grep npm | sort | uniq',\n  },\n  licenses: [\n    {\n      type: 'MIT +no-false-attribs',\n      url: 'http://github.com/isaacs/npm/raw/master/LICENSE',\n    },\n  ],\n}\n";
    CHECK(check_json(json, 2095, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 2095, JSON_READER_JSON5) == true);
}
TEST_CASE("readme-example") {
    const char* json = "{\n    foo: 'bar',\n    while: true,\n\n    this: 'is a \\\nmulti-line string',\n\n    // this is an inline comment\n    here: 'is another', // inline comment\n\n    /* this is a block comment\n       that continues on another line */\n\n    hex: 0xDEADbeef,\n    half: .5,\n    delta: +10,\n    to: Infinity,   // and beyond!\n\n    finally: 'a trailing comma',\n    oh: [\n        \"we shouldn't forget\",\n        'arrays can have',\n        'trailing commas too',\n    ],\n}\n";
    CHECK(check_json(json, 452, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 452, JSON_READER_JSON5) == true);
}
TEST_CASE("valid-whitespace") {
    const char* json = "{\n \xC""   // An invalid form feed character (\\x0c) has been entered before this comment.\n    // Be careful not to delete it.\n  \"a\": true\n}\n";
    CHECK(check_json(json, 136, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 136, JSON_READER_JSON5) == true);
}
TEST_CASE("comment-cr") {
    const char* json = "{\xD""    // This comment is terminated with `\\r`.\xD""}\xD""";
    CHECK(check_json(json, 49, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 49, JSON_READER_JSON5) == true);
}
TEST_CASE("comment-crlf") {
    const char* json = "{\xD""\n    // This comment is terminated with `\\r\\n`.\xD""\n}\xD""\n";
    CHECK(check_json(json, 54, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 54, JSON_READER_JSON5) == true);
}
TEST_CASE("comment-lf") {
    const char* json = "{\n    // This comment is terminated with `\\n`.\n}\n";
    CHECK(check_json(json, 49, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 49, JSON_READER_JSON5) == true);
}
TEST_CASE("escaped-cr") {
    const char* json = "{\xD""    // the following string contains an escaped `\\r`\xD""    a: 'line 1 \\\xD""line 2'\xD""}\xD""";
    CHECK(check_json(json, 82, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 82, JSON_READER_JSON5) == true);
}
TEST_CASE("escaped-crlf") {
    const char* json = "{\xD""\n    // the following string contains an escaped `\\r\\n`\xD""\n    a: 'line 1 \\\xD""\nline 2'\xD""\n}\xD""\n";
    CHECK(check_json(json, 89, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 89, JSON_READER_JSON5) == true);
}
TEST_CASE("escaped-lf") {
    const char* json = "{\n    // the following string contains an escaped `\\n`\n    a: 'line 1 \\\nline 2'\n}\n";
    CHECK(check_json(json, 82, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 82, JSON_READER_JSON5) == true);
}
TEST_CASE("float-leading-decimal-point") {
    const char* json = ".5\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 3, JSON_READER_JSON5) == true);
}
TEST_CASE("float-leading-zero") {
    const char* json = "0.5\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("float-trailing-decimal-point-with-integer-exponent") {
    const char* json = "5.e4\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("float-trailing-decimal-point") {
    const char* json = "5.\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 3, JSON_READER_JSON5) == true);
}
TEST_CASE("float-with-integer-exponent") {
    const char* json = "1.2e3\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("float") {
    const char* json = "1.2\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("hexadecimal-empty") {
    const char* json = "0x\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 3, JSON_READER_JSON5) == false);
}
TEST_CASE("hexadecimal-lowercase-letter") {
    const char* json = "0xc8\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("hexadecimal-uppercase-x") {
    const char* json = "0XC8\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("hexadecimal-with-integer-exponent") {
    const char* json = "0xc8e4\n";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 7, JSON_READER_JSON5) == true);
}
TEST_CASE("hexadecimal") {
    const char* json = "0xC8\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("infinity") {
    const char* json = "Infinity\n";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 9, JSON_READER_JSON5) == true);
}
TEST_CASE("integer-with-float-exponent") {
    const char* json = "1e2.3\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == false);
}
TEST_CASE("integer-with-hexadecimal-exponent") {
    const char* json = "1e0x4\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == false);
}
TEST_CASE("integer-with-integer-exponent") {
    const char* json = "2e23\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("integer-with-negative-float-exponent") {
    const char* json = "1e-2.3\n";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == false);
}
TEST_CASE("integer-with-negative-hexadecimal-exponent") {
    const char* json = "1e-0x4\n";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == false);
}
TEST_CASE("integer-with-negative-integer-exponent") {
    const char* json = "2e-23\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("integer-with-negative-zero-integer-exponent") {
    const char* json = "5e-0\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("integer-with-positive-float-exponent") {
    const char* json = "1e+2.3\n";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == false);
}
TEST_CASE("integer-with-positive-hexadecimal-exponent") {
    const char* json = "1e+0x4\n";
    CHECK(check_json(json, 7, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 7, JSON_READER_JSON5) == false);
}
TEST_CASE("integer-with-positive-integer-exponent") {
    const char* json = "1e+2\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("integer-with-positive-zero-integer-exponent") {
    const char* json = "5e+0\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("integer-with-zero-integer-exponent") {
    const char* json = "5e0\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("integer") {
    const char* json = "15\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 3, JSON_READER_JSON5) == true);
}
TEST_CASE("lone-decimal-point") {
    const char* json = ".\n";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 2, JSON_READER_JSON5) == false);
}
TEST_CASE("nan") {
    const char* json = "NaN\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-float-leading-decimal-point") {
    const char* json = "-.5\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-float-leading-zero") {
    const char* json = "-0.5\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-float-trailing-decimal-point") {
    const char* json = "-5.\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-float") {
    const char* json = "-1.2\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-hexadecimal") {
    const char* json = "-0xC8\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-infinity") {
    const char* json = "-Infinity\n";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-integer") {
    const char* json = "-15\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-noctal") {
    const char* json = "-098\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == false);
}
TEST_CASE("negative-octal") {
    const char* json = "-0123\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == false);
}
TEST_CASE("negative-zero-float-leading-decimal-point") {
    const char* json = "-.0\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-zero-float-trailing-decimal-point") {
    const char* json = "-0.\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-zero-float") {
    const char* json = "-0.0\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-zero-hexadecimal") {
    const char* json = "-0x0\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-zero-integer") {
    const char* json = "-0\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 3, JSON_READER_JSON5) == true);
}
TEST_CASE("negative-zero-octal") {
    const char* json = "-00\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == false);
}
TEST_CASE("noctal-with-leading-octal-digit") {
    const char* json = "0780\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == false);
}
TEST_CASE("noctal") {
    const char* json = "080\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == false);
}
TEST_CASE("octal") {
    const char* json = "010\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == false);
}
TEST_CASE("positive-float-leading-decimal-point") {
    const char* json = "+.5\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-float-leading-zero") {
    const char* json = "+0.5\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-float-trailing-decimal-point") {
    const char* json = "+5.\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-float") {
    const char* json = "+1.2\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-hexadecimal") {
    const char* json = "+0xC8\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 6, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-infinity") {
    const char* json = "+Infinity\n";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 10, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-integer") {
    const char* json = "+15\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-noctal") {
    const char* json = "+098\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 5, JSON_READER_JSON5) == false);
}
TEST_CASE("positive-octal") {
    const char* json = "+0123\n";
    CHECK(check_json(json, 6, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 6, JSON_READER_JSON5) == false);
}
TEST_CASE("positive-zero-float-leading-decimal-point") {
    const char* json = "+.0\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-zero-float-trailing-decimal-point") {
    const char* json = "+0.\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-zero-float") {
    const char* json = "+0.0\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-zero-hexadecimal") {
    const char* json = "+0x0\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-zero-integer") {
    const char* json = "+0\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 3, JSON_READER_JSON5) == true);
}
TEST_CASE("positive-zero-octal") {
    const char* json = "+00\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 4, JSON_READER_JSON5) == false);
}
TEST_CASE("zero-float-leading-decimal-point") {
    const char* json = ".0\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 3, JSON_READER_JSON5) == true);
}
TEST_CASE("zero-float-trailing-decimal-point") {
    const char* json = "0.\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 3, JSON_READER_JSON5) == true);
}
TEST_CASE("zero-float") {
    const char* json = "0.0\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("zero-hexadecimal") {
    const char* json = "0x0\n";
    CHECK(check_json(json, 4, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 4, JSON_READER_JSON5) == true);
}
TEST_CASE("zero-integer-with-integer-exponent") {
    const char* json = "0e23\n";
    CHECK(check_json(json, 5, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 5, JSON_READER_JSON5) == true);
}
TEST_CASE("zero-integer") {
    const char* json = "0\n";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 2, JSON_READER_JSON5) == true);
}
TEST_CASE("zero-octal") {
    const char* json = "00\n";
    CHECK(check_json(json, 3, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 3, JSON_READER_JSON5) == false);
}
TEST_CASE("duplicate-keys") {
    const char* json = "{\n    \"a\": true,\n    \"a\": false\n}\n";
    CHECK(check_json(json, 34, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 34, JSON_READER_JSON5) == true);
}
TEST_CASE("empty-object") {
    const char* json = "{}";
    CHECK(check_json(json, 2, JSON_READER_STRICT) == true);
    CHECK(check_json_ex(json, 2, JSON_READER_JSON5) == true);
}
TEST_CASE("illegal-unquoted-key-number") {
    const char* json = "{\n    10twenty: \"ten twenty\"\n}";
    CHECK(check_json(json, 30, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 30, JSON_READER_JSON5) == false);
}
TEST_CASE("illegal-unquoted-key-symbol") {
    const char* json = "{\n    multi-word: \"multi-word\"\n}";
    CHECK(check_json(json, 32, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 32, JSON_READER_JSON5) == false);
}
TEST_CASE("leading-comma-object") {
    const char* json = "{\n    ,\"foo\": \"bar\"\n}";
    CHECK(check_json(json, 21, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 21, JSON_READER_JSON5) == false);
}
TEST_CASE("lone-trailing-comma-object") {
    const char* json = "{\n    ,\n}";
    CHECK(check_json(json, 9, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 9, JSON_READER_JSON5) == false);
}
TEST_CASE("no-comma-object") {
    const char* json = "{\n    \"foo\": \"bar\"\n    \"hello\": \"world\"\n}";
    CHECK(check_json(json, 41, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 41, JSON_READER_JSON5) == false);
}
TEST_CASE("reserved-unquoted-key") {
    const char* json = "{\n    while: true\n}";
    CHECK(check_json(json, 19, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 19, JSON_READER_JSON5) == true);
}
TEST_CASE("single-quoted-key") {
    const char* json = "{\n    'hello': \"world\"\n}";
    CHECK(check_json(json, 24, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 24, JSON_READER_JSON5) == true);
}
TEST_CASE("trailing-comma-object") {
    const char* json = "{\n    \"foo\": \"bar\",\n}";
    CHECK(check_json(json, 21, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 21, JSON_READER_JSON5) == true);
}
TEST_CASE("unquoted-keys") {
    const char* json = "{\n    hello: \"world\",\n    _: \"underscore\",\n    $: \"dollar sign\",\n    one1: \"numerals\",\n    _$_: \"multiple symbols\",\n    $_$hello123world_$_: \"mixed\"\n}";
    CHECK(check_json(json, 150, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 150, JSON_READER_JSON5) == true);
}
TEST_CASE("escaped-single-quoted-string") {
    const char* json = "'I can\\'t wait'";
    CHECK(check_json(json, 15, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 15, JSON_READER_JSON5) == true);
}
TEST_CASE("multi-line-string") {
    const char* json = "'hello\\\n world'";
    CHECK(check_json(json, 15, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 15, JSON_READER_JSON5) == true);
}
TEST_CASE("single-quoted-string") {
    const char* json = "'hello world'";
    CHECK(check_json(json, 13, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 13, JSON_READER_JSON5) == true);
}
TEST_CASE("unescaped-multi-line-string") {
    const char* json = "\"foo\nbar\"\n";
    CHECK(check_json(json, 10, JSON_READER_STRICT) == false);
    CHECK(check_json(json, 10, JSON_READER_JSON5) == false);
}
TEST_CASE("unicode-escaped-unquoted-key" * doctest::may_fail(true)) {
    const char* json = "{\n    sig\\u03A3ma: \"the sum of all things\"\n}";
    CHECK(check_json(json, 44, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 44, JSON_READER_JSON5) == true);
}
TEST_CASE("unicode-unquoted-key" * doctest::may_fail(true)) {
    const char* json = "{\n    \xC3""\xBC""ml\xC3""\xA5""\xC3""\xBB""t: \"that's not really an \xC3""\xBC""mla\xC3""\xBC""t, but this is\"\n}";
    CHECK(check_json(json, 63, JSON_READER_STRICT) == false);
    CHECK(check_json_ex(json, 63, JSON_READER_JSON5) == true);
}
