TEST_CASE("i_number_double_huge_neg_exp" * doctest::may_fail(true)) {
    const char* json = "[123.456e-789]";
    REQUIRE(check_json(json, 14, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_huge_exp" * doctest::may_fail(true)) {
    const char* json = "[0.4e00669999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999969999999006]";
    REQUIRE(check_json(json, 137, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_neg_int_huge_exp" * doctest::may_fail(true)) {
    const char* json = "[-1e+9999]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_pos_double_huge_exp" * doctest::may_fail(true)) {
    const char* json = "[1.5e+9999]";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_real_neg_overflow" * doctest::may_fail(true)) {
    const char* json = "[-123123e100000]";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_real_pos_overflow" * doctest::may_fail(true)) {
    const char* json = "[123123e100000]";
    REQUIRE(check_json(json, 15, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_real_underflow" * doctest::may_fail(true)) {
    const char* json = "[123e-10000000]";
    REQUIRE(check_json(json, 15, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_too_big_neg_int" * doctest::may_fail(true)) {
    const char* json = "[-123123123123123123123123123123]";
    REQUIRE(check_json(json, 33, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_too_big_pos_int" * doctest::may_fail(true)) {
    const char* json = "[100000000000000000000]";
    REQUIRE(check_json(json, 23, JSON_READER_STRICT) == true);
}

TEST_CASE("i_number_very_big_negative_int" * doctest::may_fail(true)) {
    const char* json = "[-237462374673276894279832749832423479823246327846]";
    REQUIRE(check_json(json, 51, JSON_READER_STRICT) == true);
}

TEST_CASE("i_object_key_lone_2nd_surrogate" * doctest::may_fail(true)) {
    const char* json = "{\"\\uDFAA\":0}";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_1st_surrogate_but_2nd_missing" * doctest::may_fail(true)) {
    const char* json = "[\"\\uDADA\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_1st_valid_surrogate_2nd_invalid" * doctest::may_fail(true)) {
    const char* json = "[\"\\uD888\\u1234\"]";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_incomplete_surrogates_escape_valid" * doctest::may_fail(true)) {
    const char* json = "[\"\\uD800\\uD800\\n\"]";
    REQUIRE(check_json(json, 18, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_incomplete_surrogate_and_escape_valid" * doctest::may_fail(true)) {
    const char* json = "[\"\\uD800\\n\"]";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_incomplete_surrogate_pair" * doctest::may_fail(true)) {
    const char* json = "[\"\\uDd1ea\"]";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_invalid_lonely_surrogate" * doctest::may_fail(true)) {
    const char* json = "[\"\\ud800\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_invalid_surrogate" * doctest::may_fail(true)) {
    const char* json = "[\"\\ud800abc\"]";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_invalid_utf-8" * doctest::may_fail(true)) {
    const char* json = "[\"\xFF""\"]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("i_string_inverted_surrogates_U+1D11E" * doctest::may_fail(true)) {
    const char* json = "[\"\\uDd1e\\uD834\"]";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_iso_latin_1" * doctest::may_fail(true)) {
    const char* json = "[\"\xE9""\"]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("i_string_lone_second_surrogate" * doctest::may_fail(true)) {
    const char* json = "[\"\\uDFAA\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("i_string_lone_utf8_continuation_byte" * doctest::may_fail(true)) {
    const char* json = "[\"\x81""\"]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("i_string_not_in_unicode_range" * doctest::may_fail(true)) {
    const char* json = "[\"\xF4""\xBF""\xBF""\xBF""\"]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == true);
}

TEST_CASE("i_string_overlong_sequence_2_bytes" * doctest::may_fail(true)) {
    const char* json = "[\"\xC0""\xAF""\"]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("i_string_overlong_sequence_6_bytes" * doctest::may_fail(true)) {
    const char* json = "[\"\xFC""\x83""\xBF""\xBF""\xBF""\xBF""\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("i_string_overlong_sequence_6_bytes_null" * doctest::may_fail(true)) {
    const char* json = "[\"\xFC""\x80""\x80""\x80""\x80""\x80""\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("i_string_truncated-utf-8" * doctest::may_fail(true)) {
    const char* json = "[\"\xE0""\xFF""\"]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("i_string_UTF-8_invalid_sequence" * doctest::may_fail(true)) {
    const char* json = "[\"\xE6""\x97""\xA5""\xD1""\x88""\xFA""\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("i_structure_500_nested_arrays" * doctest::may_fail(true)) {
    const char* json = "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]";
    REQUIRE(check_json(json, 1000, JSON_READER_STRICT) == true);
}

TEST_CASE("n_array_1_true_without_comma") {
    const char* json = "[1 true]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_a_invalid_utf8") {
    const char* json = "[a\xE5""]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_colon_instead_of_comma") {
    const char* json = "[\"\": 1]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_comma_after_close") {
    const char* json = "[\"\"],";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_comma_and_number") {
    const char* json = "[,1]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_double_comma") {
    const char* json = "[1,,2]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_double_extra_comma") {
    const char* json = "[\"x\",,]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_extra_close") {
    const char* json = "[\"x\"]]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_extra_comma") {
    const char* json = "[\"\",]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_incomplete") {
    const char* json = "[\"x\"";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_incomplete_invalid_value") {
    const char* json = "[x";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_inner_array_no_comma") {
    const char* json = "[3[4]]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_invalid_utf8") {
    const char* json = "[\xFF""]";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_items_separated_by_semicolon") {
    const char* json = "[1:2]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_just_comma") {
    const char* json = "[,]";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_just_minus") {
    const char* json = "[-]";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_missing_value") {
    const char* json = "[   , \"\"]";
    REQUIRE(check_json(json, 9, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_newlines_unclosed") {
    const char* json = "[\"a\",\n4\n,1,";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_number_and_comma") {
    const char* json = "[1,]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_number_and_several_commas") {
    const char* json = "[1,,]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_spaces_vertical_tab_formfeed") {
    const char* json = "[\"\xB""a\"\\f]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_star_inside") {
    const char* json = "[*]";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_unclosed") {
    const char* json = "[\"\"";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_unclosed_trailing_comma") {
    const char* json = "[1,";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_unclosed_with_new_lines") {
    const char* json = "[1,\n1\n,1";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_array_unclosed_with_object_inside") {
    const char* json = "[{}";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_incomplete_false") {
    const char* json = "[fals]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_incomplete_null") {
    const char* json = "[nul]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_incomplete_true") {
    const char* json = "[tru]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_multidigit_number_then_00") {
    const char* json = "123\x0""";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_++") {
    const char* json = "[++1234]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_+1") {
    const char* json = "[+1]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_+Inf") {
    const char* json = "[+Inf]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_-01") {
    const char* json = "[-01]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_-1.0.") {
    const char* json = "[-1.0.]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_-2.") {
    const char* json = "[-2.]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_-NaN") {
    const char* json = "[-NaN]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_.-1") {
    const char* json = "[.-1]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_.2e-3") {
    const char* json = "[.2e-3]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_0.1.2") {
    const char* json = "[0.1.2]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_0.3e+") {
    const char* json = "[0.3e+]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_0.3e") {
    const char* json = "[0.3e]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_0.e1") {
    const char* json = "[0.e1]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_0e+") {
    const char* json = "[0e+]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_0e") {
    const char* json = "[0e]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_0_capital_E+") {
    const char* json = "[0E+]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_0_capital_E") {
    const char* json = "[0E]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_1.0e+") {
    const char* json = "[1.0e+]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_1.0e-") {
    const char* json = "[1.0e-]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_1.0e") {
    const char* json = "[1.0e]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_1eE2") {
    const char* json = "[1eE2]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_1_000") {
    const char* json = "[1 000.0]";
    REQUIRE(check_json(json, 9, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_2.e+3") {
    const char* json = "[2.e+3]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_2.e-3") {
    const char* json = "[2.e-3]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_2.e3") {
    const char* json = "[2.e3]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_9.e+") {
    const char* json = "[9.e+]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_expression") {
    const char* json = "[1+2]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_hex_1_digit") {
    const char* json = "[0x1]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_hex_2_digits") {
    const char* json = "[0x42]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_Inf") {
    const char* json = "[Inf]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_infinity") {
    const char* json = "[Infinity]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_invalid+-") {
    const char* json = "[0e+-1]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_invalid-negative-real") {
    const char* json = "[-123.123foo]";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_invalid-utf-8-in-bigger-int") {
    const char* json = "[123\xE5""]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_invalid-utf-8-in-exponent") {
    const char* json = "[1e1\xE5""]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_invalid-utf-8-in-int") {
    const char* json = "[0\xE5""]\n";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_minus_infinity") {
    const char* json = "[-Infinity]";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_minus_sign_with_trailing_garbage") {
    const char* json = "[-foo]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_minus_space_1") {
    const char* json = "[- 1]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_NaN") {
    const char* json = "[NaN]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_neg_int_starting_with_zero") {
    const char* json = "[-012]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_neg_real_without_int_part") {
    const char* json = "[-.123]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_neg_with_garbage_at_end") {
    const char* json = "[-1x]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_real_garbage_after_e") {
    const char* json = "[1ea]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_real_without_fractional_part") {
    const char* json = "[1.]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_real_with_invalid_utf8_after_e") {
    const char* json = "[1e\xE5""]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_starting_with_dot") {
    const char* json = "[.123]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_U+FF11_fullwidth_digit_one") {
    const char* json = "[\xEF""\xBC""\x91""]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_with_alpha") {
    const char* json = "[1.2a-3]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_with_alpha_char") {
    const char* json = "[1.8011670033376514H-308]";
    REQUIRE(check_json(json, 25, JSON_READER_STRICT) == false);
}

TEST_CASE("n_number_with_leading_zero") {
    const char* json = "[012]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_bad_value") {
    const char* json = "[\"x\", truth]";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_bracket_key") {
    const char* json = "{[: \"x\"}\n";
    REQUIRE(check_json(json, 9, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_comma_instead_of_colon") {
    const char* json = "{\"x\", null}";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_double_colon") {
    const char* json = "{\"x\"::\"b\"}";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_emoji") {
    const char* json = "{\xF0""\x9F""\x87""\xA8""\xF0""\x9F""\x87""\xAD""}";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_garbage_at_end") {
    const char* json = "{\"a\":\"a\" 123}";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_key_with_single_quotes") {
    const char* json = "{key: 'value'}";
    REQUIRE(check_json(json, 14, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_lone_continuation_byte_in_key_and_trailing_comma") {
    const char* json = "{\"\xB9""\":\"0\",}";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_missing_colon") {
    const char* json = "{\"a\" b}";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_missing_key") {
    const char* json = "{:\"b\"}";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_missing_semicolon") {
    const char* json = "{\"a\" \"b\"}";
    REQUIRE(check_json(json, 9, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_missing_value") {
    const char* json = "{\"a\":";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_no-colon") {
    const char* json = "{\"a\"";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_non_string_key") {
    const char* json = "{1:1}";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_non_string_key_but_huge_number_instead") {
    const char* json = "{9999E9999:1}";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_repeated_null_null") {
    const char* json = "{null:null,null:null}";
    REQUIRE(check_json(json, 21, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_several_trailing_commas") {
    const char* json = "{\"id\":0,,,,,}";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_single_quote") {
    const char* json = "{'a':0}";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_trailing_comma") {
    const char* json = "{\"id\":0,}";
    REQUIRE(check_json(json, 9, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_trailing_comment") {
    const char* json = "{\"a\":\"b\"}/**/";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_trailing_comment_open") {
    const char* json = "{\"a\":\"b\"}/**//";
    REQUIRE(check_json(json, 14, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_trailing_comment_slash_open") {
    const char* json = "{\"a\":\"b\"}//";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_trailing_comment_slash_open_incomplete") {
    const char* json = "{\"a\":\"b\"}/";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_two_commas_in_a_row") {
    const char* json = "{\"a\":\"b\",,\"c\":\"d\"}";
    REQUIRE(check_json(json, 18, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_unquoted_key") {
    const char* json = "{a: \"b\"}";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_unterminated-value") {
    const char* json = "{\"a\":\"a";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_with_single_string") {
    const char* json = "{ \"foo\" : \"bar\", \"a\" }";
    REQUIRE(check_json(json, 22, JSON_READER_STRICT) == false);
}

TEST_CASE("n_object_with_trailing_garbage") {
    const char* json = "{\"a\":\"b\"}#";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("n_single_space") {
    const char* json = " ";
    REQUIRE(check_json(json, 1, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_1_surrogate_then_escape") {
    const char* json = "[\"\\uD800\\\"]";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_1_surrogate_then_escape_u") {
    const char* json = "[\"\\uD800\\u\"]";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_1_surrogate_then_escape_u1") {
    const char* json = "[\"\\uD800\\u1\"]";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_1_surrogate_then_escape_u1x") {
    const char* json = "[\"\\uD800\\u1x\"]";
    REQUIRE(check_json(json, 14, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_accentuated_char_no_quotes") {
    const char* json = "[\xC3""\xA9""]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_backslash_00") {
    const char* json = "[\"\\\x0""\"]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_escaped_backslash_bad") {
    const char* json = "[\"\\\\\\\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_escaped_ctrl_char_tab") {
    const char* json = "[\"\\\x9""\"]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_escaped_emoji") {
    const char* json = "[\"\\\xF0""\x9F""\x8C""\x80""\"]";
    REQUIRE(check_json(json, 9, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_escape_x") {
    const char* json = "[\"\\x00\"]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_incomplete_escape") {
    const char* json = "[\"\\\"]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_incomplete_escaped_character") {
    const char* json = "[\"\\u00A\"]";
    REQUIRE(check_json(json, 9, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_incomplete_surrogate") {
    const char* json = "[\"\\uD834\\uDd\"]";
    REQUIRE(check_json(json, 14, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_incomplete_surrogate_escape_invalid") {
    const char* json = "[\"\\uD800\\uD800\\x\"]";
    REQUIRE(check_json(json, 18, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_invalid-utf-8-in-escape") {
    const char* json = "[\"\\u\xE5""\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_invalid_backslash_esc") {
    const char* json = "[\"\\a\"]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_invalid_unicode_escape") {
    const char* json = "[\"\\uqqqq\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_invalid_utf8_after_escape") {
    const char* json = "[\"\\\xE5""\"]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_leading_uescaped_thinspace") {
    const char* json = "[\\u0020\"asd\"]";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_no_quotes_with_bad_escape") {
    const char* json = "[\\n]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_single_doublequote") {
    const char* json = "\"";
    REQUIRE(check_json(json, 1, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_single_quote") {
    const char* json = "['single quote']";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_single_string_no_double_quotes") {
    const char* json = "abc";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_start_escape_unclosed") {
    const char* json = "[\"\\";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_unescaped_crtl_char") {
    const char* json = "[\"a\x0""a\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_unescaped_newline") {
    const char* json = "[\"new\nline\"]";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_unescaped_tab") {
    const char* json = "[\"\x9""\"]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_unicode_CapitalU") {
    const char* json = "\"\\UA66D\"";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_string_with_trailing_garbage") {
    const char* json = "\"\"x";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_angle_bracket_.") {
    const char* json = "<.>";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_angle_bracket_null") {
    const char* json = "[<null>]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_array_trailing_garbage") {
    const char* json = "[1]x";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_array_with_extra_array_close") {
    const char* json = "[1]]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_array_with_unclosed_string") {
    const char* json = "[\"asd]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_ascii-unicode-identifier") {
    const char* json = "a\xC3""\xA5""";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_capitalized_True") {
    const char* json = "[True]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_close_unopened_array") {
    const char* json = "1]";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_comma_instead_of_closing_brace") {
    const char* json = "{\"x\": true,";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_double_array") {
    const char* json = "[][]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_end_array") {
    const char* json = "]";
    REQUIRE(check_json(json, 1, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_lone-invalid-utf-8") {
    const char* json = "\xE5""";
    REQUIRE(check_json(json, 1, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_lone-open-bracket") {
    const char* json = "[";
    REQUIRE(check_json(json, 1, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_no_data") {
    const char* json = "";
    REQUIRE(check_json(json, 0, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_null-byte-outside-string") {
    const char* json = "[\x0""]";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_number_with_trailing_garbage") {
    const char* json = "2@";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_object_followed_by_closing_object") {
    const char* json = "{}}";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_object_unclosed_no_value") {
    const char* json = "{\"\":";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_object_with_comment") {
    const char* json = "{\"a\":/*comment*/\"b\"}";
    REQUIRE(check_json(json, 20, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_object_with_trailing_garbage") {
    const char* json = "{\"a\": true} \"x\"";
    REQUIRE(check_json(json, 15, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_array_apostrophe") {
    const char* json = "['";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_array_comma") {
    const char* json = "[,";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_array_open_object") {
    const char* json = "[{";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_array_open_string") {
    const char* json = "[\"a";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_array_string") {
    const char* json = "[\"a\"";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_object") {
    const char* json = "{";
    REQUIRE(check_json(json, 1, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_object_close_array") {
    const char* json = "{]";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_object_comma") {
    const char* json = "{,";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_object_open_array") {
    const char* json = "{[";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_object_open_string") {
    const char* json = "{\"a";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_object_string_with_apostrophes") {
    const char* json = "{'a'";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_open_open") {
    const char* json = "[\"\\{[\"\\{[\"\\{[\"\\{";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_single_eacute") {
    const char* json = "\xE9""";
    REQUIRE(check_json(json, 1, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_single_star") {
    const char* json = "*";
    REQUIRE(check_json(json, 1, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_trailing_#") {
    const char* json = "{\"a\":\"b\"}#{}";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_U+2060_word_joined") {
    const char* json = "[\xE2""\x81""\xA0""]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_uescaped_LF_before_string") {
    const char* json = "[\\u000A\"\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_unclosed_array") {
    const char* json = "[1";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_unclosed_array_partial_null") {
    const char* json = "[ false, nul";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_unclosed_array_unfinished_false") {
    const char* json = "[ true, fals";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_unclosed_array_unfinished_true") {
    const char* json = "[ false, tru";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_unclosed_object") {
    const char* json = "{\"asd\":\"asd\"";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_unicode-identifier") {
    const char* json = "\xC3""\xA5""";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_whitespace_formfeed") {
    const char* json = "[\xC""]";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == false);
}

TEST_CASE("n_structure_whitespace_U+2060_word_joiner") {
    const char* json = "[\xE2""\x81""\xA0""]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == false);
}

TEST_CASE("y_array_arraysWithSpaces") {
    const char* json = "[[]   ]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_empty-string") {
    const char* json = "[\"\"]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_empty") {
    const char* json = "[]";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_ending_with_newline") {
    const char* json = "[\"a\"]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_false") {
    const char* json = "[false]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_heterogeneous") {
    const char* json = "[null, 1, \"1\", {}]";
    REQUIRE(check_json(json, 18, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_null") {
    const char* json = "[null]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_with_1_and_newline") {
    const char* json = "[1\n]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_with_leading_space") {
    const char* json = " [1]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_with_several_null") {
    const char* json = "[1,null,null,null,2]";
    REQUIRE(check_json(json, 20, JSON_READER_STRICT) == true);
}

TEST_CASE("y_array_with_trailing_space") {
    const char* json = "[2] ";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number") {
    const char* json = "[123e65]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_0e+1") {
    const char* json = "[0e+1]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_0e1") {
    const char* json = "[0e1]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_after_space") {
    const char* json = "[ 4]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_double_close_to_zero") {
    const char* json = "[-0.000000000000000000000000000000000000000000000000000000000000000000000000000001]\n";
    REQUIRE(check_json(json, 84, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_int_with_exp") {
    const char* json = "[20e1]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_minus_zero") {
    const char* json = "[-0]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_negative_int") {
    const char* json = "[-123]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_negative_one") {
    const char* json = "[-1]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_negative_zero") {
    const char* json = "[-0]";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_real_capital_e") {
    const char* json = "[1E22]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_real_capital_e_neg_exp") {
    const char* json = "[1E-2]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_real_capital_e_pos_exp") {
    const char* json = "[1E+2]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_real_exponent") {
    const char* json = "[123e45]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_real_fraction_exponent") {
    const char* json = "[123.456e78]";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_real_neg_exp") {
    const char* json = "[1e-2]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_real_pos_exponent") {
    const char* json = "[1e+2]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_simple_int") {
    const char* json = "[123]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("y_number_simple_real") {
    const char* json = "[123.456789]";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object") {
    const char* json = "{\"asd\":\"sdf\", \"dfg\":\"fgh\"}";
    REQUIRE(check_json(json, 26, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_basic") {
    const char* json = "{\"asd\":\"sdf\"}";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_duplicated_key") {
    const char* json = "{\"a\":\"b\",\"a\":\"c\"}";
    REQUIRE(check_json(json, 17, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_duplicated_key_and_value") {
    const char* json = "{\"a\":\"b\",\"a\":\"b\"}";
    REQUIRE(check_json(json, 17, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_empty") {
    const char* json = "{}";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_empty_key") {
    const char* json = "{\"\":0}";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_escaped_null_in_key") {
    const char* json = "{\"foo\\u0000bar\": 42}";
    REQUIRE(check_json(json, 20, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_extreme_numbers") {
    const char* json = "{ \"min\": -1.0e+28, \"max\": 1.0e+28 }";
    REQUIRE(check_json(json, 35, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_long_strings") {
    const char* json = "{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}";
    REQUIRE(check_json(json, 108, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_simple") {
    const char* json = "{\"a\":[]}";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_string_unicode") {
    const char* json = "{\"title\":\"\\u041f\\u043e\\u043b\\u0442\\u043e\\u0440\\u0430 \\u0417\\u0435\\u043c\\u043b\\u0435\\u043a\\u043e\\u043f\\u0430\" }";
    REQUIRE(check_json(json, 110, JSON_READER_STRICT) == true);
}

TEST_CASE("y_object_with_newlines") {
    const char* json = "{\n\"a\": \"b\"\n}";
    REQUIRE(check_json(json, 12, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_1_2_3_bytes_UTF-8_sequences") {
    const char* json = "[\"\\u0060\\u012a\\u12AB\"]";
    REQUIRE(check_json(json, 22, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_accepted_surrogate_pair") {
    const char* json = "[\"\\uD801\\udc37\"]";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_accepted_surrogate_pairs") {
    const char* json = "[\"\\ud83d\\ude39\\ud83d\\udc8d\"]";
    REQUIRE(check_json(json, 28, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_allowed_escapes") {
    const char* json = "[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]";
    REQUIRE(check_json(json, 20, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_backslash_and_u_escaped_zero") {
    const char* json = "[\"\\\\u0000\"]";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_backslash_doublequotes") {
    const char* json = "[\"\\\"\"]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_comments") {
    const char* json = "[\"a/*b*/c/*d//e\"]";
    REQUIRE(check_json(json, 17, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_double_escape_a") {
    const char* json = "[\"\\\\a\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_double_escape_n") {
    const char* json = "[\"\\\\n\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_escaped_control_character") {
    const char* json = "[\"\\u0012\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_escaped_noncharacter") {
    const char* json = "[\"\\uFFFF\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_in_array") {
    const char* json = "[\"asd\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_in_array_with_leading_space") {
    const char* json = "[ \"asd\"]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_last_surrogates_1_and_2") {
    const char* json = "[\"\\uDBFF\\uDFFF\"]";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_nbsp_uescaped") {
    const char* json = "[\"new\\u00A0line\"]";
    REQUIRE(check_json(json, 17, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_nonCharacterInUTF-8_U+10FFFF") {
    const char* json = "[\"\xF4""\x8F""\xBF""\xBF""\"]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_nonCharacterInUTF-8_U+FFFF") {
    const char* json = "[\"\xEF""\xBF""\xBF""\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_null_escape") {
    const char* json = "[\"\\u0000\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_one-byte-utf-8") {
    const char* json = "[\"\\u002c\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_pi") {
    const char* json = "[\"\xCF""\x80""\"]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_reservedCharacterInUTF-8_U+1BFFF") {
    const char* json = "[\"\xF0""\x9B""\xBF""\xBF""\"]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_simple_ascii") {
    const char* json = "[\"asd \"]";
    REQUIRE(check_json(json, 8, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_space") {
    const char* json = "\" \"";
    REQUIRE(check_json(json, 3, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF") {
    const char* json = "[\"\\uD834\\uDd1e\"]";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_three-byte-utf-8") {
    const char* json = "[\"\\u0821\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_two-byte-utf-8") {
    const char* json = "[\"\\u0123\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_u+2028_line_sep") {
    const char* json = "[\"\xE2""\x80""\xA8""\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_u+2029_par_sep") {
    const char* json = "[\"\xE2""\x80""\xA9""\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_uEscape") {
    const char* json = "[\"\\u0061\\u30af\\u30EA\\u30b9\"]";
    REQUIRE(check_json(json, 28, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_uescaped_newline") {
    const char* json = "[\"new\\u000Aline\"]";
    REQUIRE(check_json(json, 17, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unescaped_char_delete") {
    const char* json = "[\"\x7F""\"]";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode") {
    const char* json = "[\"\\uA66D\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicodeEscapedBackslash") {
    const char* json = "[\"\\u005C\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode_2") {
    const char* json = "[\"\xE2""\x8D""\x82""\xE3""\x88""\xB4""\xE2""\x8D""\x82""\"]";
    REQUIRE(check_json(json, 13, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode_escaped_double_quote") {
    const char* json = "[\"\\u0022\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode_U+10FFFE_nonchar") {
    const char* json = "[\"\\uDBFF\\uDFFE\"]";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode_U+1FFFE_nonchar") {
    const char* json = "[\"\\uD83F\\uDFFE\"]";
    REQUIRE(check_json(json, 16, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode_U+200B_ZERO_WIDTH_SPACE") {
    const char* json = "[\"\\u200B\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode_U+2064_invisible_plus") {
    const char* json = "[\"\\u2064\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode_U+FDD0_nonchar") {
    const char* json = "[\"\\uFDD0\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_unicode_U+FFFE_nonchar") {
    const char* json = "[\"\\uFFFE\"]";
    REQUIRE(check_json(json, 10, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_utf8") {
    const char* json = "[\"\xE2""\x82""\xAC""\xF0""\x9D""\x84""\x9E""\"]";
    REQUIRE(check_json(json, 11, JSON_READER_STRICT) == true);
}

TEST_CASE("y_string_with_del_character") {
    const char* json = "[\"a\x7F""a\"]";
    REQUIRE(check_json(json, 7, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_lonely_false") {
    const char* json = "false";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_lonely_int") {
    const char* json = "42";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_lonely_negative_real") {
    const char* json = "-0.1";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_lonely_null") {
    const char* json = "null";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_lonely_string") {
    const char* json = "\"asd\"";
    REQUIRE(check_json(json, 5, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_lonely_true") {
    const char* json = "true";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_string_empty") {
    const char* json = "\"\"";
    REQUIRE(check_json(json, 2, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_trailing_newline") {
    const char* json = "[\"a\"]\n";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_true_in_array") {
    const char* json = "[true]";
    REQUIRE(check_json(json, 6, JSON_READER_STRICT) == true);
}

TEST_CASE("y_structure_whitespace_array") {
    const char* json = " [] ";
    REQUIRE(check_json(json, 4, JSON_READER_STRICT) == true);
}

