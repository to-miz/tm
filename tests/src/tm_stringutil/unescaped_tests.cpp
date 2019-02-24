

TEST_CASE("Test tmsu_find_char_unescaped") {
    // Looking for unescaped chars.
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "a", 'a', '/') == 0);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "/a", 'a', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "//a", 'a', '/') == 2);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "///a", 'a', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "c///a", 'a', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "c////a", 'a', '/') == 5);

    // Looking for unescaped escape chars.
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "/", '/', '/') == 0);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "//", '/', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "///", '/', '/') == 2);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "cc///", '/', '/') == 4);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "aaa", '/', '/') == -1);
    CHECK(ptr_to_index(tmsu_find_char_unescaped, "cccaaa", '/', '/') == -1);
}

TEST_CASE("Test tmsu_find_first_of_unescaped") {
    // Not found.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "d", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "", "", '/') == -1);

    // No escaped chars.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "a", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "b", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "c", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "abc", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "dddddabc", "abc", '/') == 5);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "a", "", '/') == 0);

    // Some escaped chars.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/a", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/b", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/c", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/ab", "abc", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/aa", "abc", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/a/aa", "abc", '/') == 4);

    // Escaped escape chars.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "//b", "abc", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "//b", "abc", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///b", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///ba", "abc", '/') == 4);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "c///a", "abc", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "d///a", "abc", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "d///ac", "abc", '/') == 5);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "d////a", "abc", '/') == 5);

    // Looking for unescaped escape chars.
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "/", "ab/", '/') == 0);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "//", "ab/", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///", "ab/", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///a", "ab/", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "///a", "ab/", '/') == 2);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "cc///", "ab/", '/') == 4);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "////", "ab/", '/') == -1);
    CHECK(ptr_to_index(tmsu_find_first_of_unescaped, "ccc////a", "ab/", '/') == 7);
}

TEST_CASE("Test tmsu_find_char_unescaped_n") {
    // Looking for unescaped chars.
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "a", 'a', '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "/a", 'a', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "//a", 'a', '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "///a", 'a', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "c///a", 'a', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "c////a", 'a', '/') == 5);

    // Looking for unescaped escape chars.
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "/", '/', '/') == 0);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "//", '/', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "///", '/', '/') == 2);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "cc///", '/', '/') == 4);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "aaa", '/', '/') == -1);
    CHECK(ptr_to_index_view(tmsu_find_char_unescaped_n, "cccaaa", '/', '/') == -1);
}

TEST_CASE("Test tmsu_find_first_of_unescaped_n") {
    // tmsu_find_first_of_unescaped_n is overloaded, so we get a specific signature of it.
    auto func = [](const char* str_first, const char* str_last, const char* find_str_first, const char* find_str_last,
                   char escape_char) {
        return tmsu_find_first_of_unescaped_n(str_first, str_last, find_str_first, find_str_last, escape_char);
    };

    // Not found.
    CHECK(ptr_to_index_view(func, "d", "abc", '/') == -1);
    CHECK(ptr_to_index_view(func, "", "abc", '/') == -1);
    CHECK(ptr_to_index_view(func, "", "", '/') == -1);

    // No escaped chars.
    CHECK(ptr_to_index_view(func, "a", "abc", '/') == 0);
    CHECK(ptr_to_index_view(func, "b", "abc", '/') == 0);
    CHECK(ptr_to_index_view(func, "c", "abc", '/') == 0);
    CHECK(ptr_to_index_view(func, "abc", "abc", '/') == 0);
    CHECK(ptr_to_index_view(func, "dddddabc", "abc", '/') == 5);
    CHECK(ptr_to_index_view(func, "a", "", '/') == 0);
    CHECK(ptr_to_index_view(func, "a", "", '/') == 0);

    // Some escaped chars.
    CHECK(ptr_to_index_view(func, "/a", "abc", '/') == -1);
    CHECK(ptr_to_index_view(func, "/b", "abc", '/') == -1);
    CHECK(ptr_to_index_view(func, "/c", "abc", '/') == -1);
    CHECK(ptr_to_index_view(func, "/ab", "abc", '/') == 2);
    CHECK(ptr_to_index_view(func, "/aa", "abc", '/') == 2);
    CHECK(ptr_to_index_view(func, "/a/aa", "abc", '/') == 4);

    // Escaped escape chars.
    CHECK(ptr_to_index_view(func, "//b", "abc", '/') == 2);
    CHECK(ptr_to_index_view(func, "//b", "abc", '/') == 2);
    CHECK(ptr_to_index_view(func, "///b", "abc", '/') == -1);
    CHECK(ptr_to_index_view(func, "///ba", "abc", '/') == 4);
    CHECK(ptr_to_index_view(func, "c///a", "abc", '/') == 0);
    CHECK(ptr_to_index_view(func, "d///a", "abc", '/') == -1);
    CHECK(ptr_to_index_view(func, "d///ac", "abc", '/') == 5);
    CHECK(ptr_to_index_view(func, "d////a", "abc", '/') == 5);

    // Looking for unescaped escape chars.
    CHECK(ptr_to_index_view(func, "/", "ab/", '/') == 0);
    CHECK(ptr_to_index_view(func, "//", "ab/", '/') == -1);
    CHECK(ptr_to_index_view(func, "///", "ab/", '/') == 2);
    CHECK(ptr_to_index_view(func, "///a", "ab/", '/') == 2);
    CHECK(ptr_to_index_view(func, "///a", "ab/", '/') == 2);
    CHECK(ptr_to_index_view(func, "cc///", "ab/", '/') == 4);
    CHECK(ptr_to_index_view(func, "////", "ab/", '/') == -1);
    CHECK(ptr_to_index_view(func, "ccc////a", "ab/", '/') == 7);
}