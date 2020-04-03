#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__cplusplus) || !defined(TM_STRING_VIEW)
inline static tmsu_string_view tmsu_make_string_view(const char* str, tm_size_t size) {
    tmsu_string_view result;
    result.data = str;
    result.size = size;
    return result;
}
#endif /* !defined(__cplusplus) || !defined(TM_STRING_VIEW) */

#include "../common/tm_null.inc"

#define TMSU_C2I(x) ((unsigned char)(x))

inline static tm_size_t tmsu_distance(const char* first, const char* last) {
    TM_ASSERT(first <= last);
    return (tm_size_t)(last - first);
}
inline static size_t tmsu_distance_sz(const char* first, const char* last) {
    TM_ASSERT(first <= last);
    return (size_t)(last - first);
}

TMSU_DEF const char* tmsu_find_char(const char* str, char c) {
    TM_ASSERT(str);
    for (; *str; ++str) {
        if (*str == c) return str;
    }
    return str;
}
TMSU_DEF const char* tmsu_find_char2(const char* str, char a, char b) {
    TM_ASSERT(str);
    for (; *str; ++str) {
        if (*str == a || *str == b) return str;
    }
    return str;
}

TMSU_DEF const char* tmsu_find_last_char(const char* str, char c) {
    TM_ASSERT(str);
    return tmsu_find_last_char_n(str, str + TM_STRLEN(str), c);
}

TMSU_DEF const char* tmsu_find_char_n(const char* str_first, const char* str_last, char c) {
    const void* result = TM_MEMCHR(str_first, TMSU_C2I(c), tmsu_distance_sz(str_first, str_last));
    return (result) ? (const char*)result : str_last;
}
TMSU_DEF const char* tmsu_find_char2_n(const char* str_first, const char* str_last, char a, char b) {
    /* TODO: Is it better to call memchr twice, or do a manual loop? */
#if 1
    size_t len = tmsu_distance_sz(str_first, str_last);
    if (!len) return str_last;
    const char* a_match = (const char*)TM_MEMCHR(str_first, TMSU_C2I(a), len);
    const char* b_match = (const char*)TM_MEMCHR(str_first, TMSU_C2I(b), len);
    if (!a_match) a_match = str_last;
    if (!b_match) b_match = str_last;
    return (a_match < b_match) ? a_match : b_match;
#else
    for (; str_first != str_last; ++str_first) {
        char current = TMSU_C2I(*str_first);
        if (current == a || current == b) return str_first;
    }
    return str_first;
#endif
}

TMSU_DEF const char* tmsu_find_last_char_n_ex(const char* str_first, const char* str_last, char c,
                                              const char* not_found) {
    TM_ASSERT(str_first <= str_last);
#ifdef TM_MEMRCHR
    void* result = TM_MEMRCHR(str_first, TMSU_C2I(c), tmsu_distance_sz(str_first, str_last));
    return (result) ? (const char*)result : not_found;
#else
    const char* cur = str_last;
    while (str_first != cur) {
        --cur;
        if (TMSU_C2I(*cur) == TMSU_C2I(c)) {
            return cur;
        }
    };
    return not_found;
#endif
}
TMSU_DEF const char* tmsu_find_last_char_n(const char* str_first, const char* str_last, char c) {
    return tmsu_find_last_char_n_ex(str_first, str_last, c, str_last);
}

TMSU_DEF const char* tmsu_find_n(const char* str_first, const char* str_last, const char* find_str_first,
                                 const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first;                                       /* Empty string always matches */
    if (find_str_len > tmsu_distance_sz(str_first, str_last)) return str_last; /* Not enough room for a match. */

    /* We can reduce str_last by find_str_len, since the remaining size at the end doesn't allow for a match. */
    str_last -= find_str_len;
    const char* cur = str_first;
    while ((cur = tmsu_find_char_n(cur, str_last, *find_str_first)) != str_last) {
        if (TM_MEMCMP(cur, find_str_first, find_str_len) == 0) {
            return cur;
        }
        ++cur;
    }
    return str_last;
}

TMSU_DEF const char* tmsu_find_last_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                         const char* find_str_last, const char* not_found) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first;                                        /* Empty string always matches. */
    if (find_str_len > tmsu_distance_sz(str_first, str_last)) return not_found; /* Not enough room for a match. */

    /* We can reduce str_last by find_str_len, since the remaining size at the end doesn't allow for a match. */
    str_last -= find_str_len;
    const char* prev = str_last;
    const char* cur = str_last;
    while ((cur = tmsu_find_last_char_n(str_first, prev, *find_str_first)) != prev) {
        if (TM_MEMCMP(cur, find_str_first, find_str_len) == 0) {
            return cur;
        }
        prev = cur;
    }
    return not_found;
}

TMSU_DEF const char* tmsu_find_last_n(const char* str_first, const char* str_last, const char* find_str_first,
                                      const char* find_str_last) {
    return tmsu_find_last_n_ex(str_first, str_last, find_str_first, find_str_last, str_last);
}

TMSU_DEF const char* tmsu_find(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

#ifdef TM_STRSTR
    return TM_STRSTR(str, find_str);
#else
    /* TODO: is this better than calling the _n variant with two strlens? */
#if 0
    return tmsu_find_n(str, str + TM_STRLEN(str), find_str, TM_STRLEN(find_str));
#else
    if (!*find_str) return str;
    for (;;) {
        while (*str && *str != *find_str) {
            ++str;
        }
        if (!*str) {
            return str;
        }
        const char* other = find_str;
        while (*str && *other && *str == *other) {
            ++str;
            ++other;
        }
        if (!*str && !*other) {
            return str;
        }
    }
#endif
#endif
}

TMSU_DEF const char* tmsu_find_last(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    return tmsu_find_last_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF const char* tmsu_find_first_not_of(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
#ifdef TM_STRSPN
    return str + TM_STRSPN(str, find_str);
#else
    while (*str && TM_STRCHR(find_str, TMSU_C2I(*str))) {
        ++str;
    }
    return str;
#endif
}

TMSU_DEF const char* tmsu_find_first_of(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
#ifdef TM_STRCSPN
    return str + TM_STRCSPN(str, find_str);
#else
    while (*str && TM_STRCHR(find_str, TMSU_C2I(*str)) == TM_NULL) {
        ++str;
    }
    return str;
#endif
}

TMSU_DEF const char* tmsu_find_last_not_of(const char* str, const char* find_str) {
    const char* last = str + TM_STRLEN(str) - 1;
    while (last != str && TM_STRCHR(find_str, TMSU_C2I(*last))) {
        --last;
    }
    return last;
}

TMSU_DEF const char* tmsu_find_last_of(const char* str, const char* find_str) {
    const char* last = str + TM_STRLEN(str) - 1;
    while (last != str && TM_STRCHR(find_str, TMSU_C2I(*last)) == TM_NULL) {
        --last;
    }
    return last;
}

TMSU_DEF const char* tmsu_find_first_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                          const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);
    if (find_str_first == find_str_last) return str_first;

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_first != str_last && TM_MEMCHR(find_str_first, TMSU_C2I(*str_first), find_str_len) == TM_NULL) {
        ++str_first;
    }
    return str_first;
}

TMSU_DEF const char* tmsu_find_first_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                              const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first; /* Empty always matches. */
    while (str_first != str_last && TM_MEMCHR(find_str_first, TMSU_C2I(*str_first), find_str_len)) {
        ++str_first;
    }
    return str_first;
}

TMSU_DEF const char* tmsu_find_last_of_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                            const char* find_str_last, const char* not_found) {
    if (str_first == str_last) return not_found;
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_last != str_first && TM_MEMCHR(find_str_first, TMSU_C2I(*(str_last - 1)), find_str_len) == TM_NULL) {
        --str_last;
    }
    if (str_first == str_last) return not_found;
    return str_last - 1;
}

TMSU_DEF const char* tmsu_find_last_not_of_n_ex(const char* str_first, const char* str_last, const char* find_str_first,
                                                const char* find_str_last, const char* not_found) {
    if (str_first == str_last) return not_found;
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_last != str_first && TM_MEMCHR(find_str_first, TMSU_C2I(*(str_last - 1)), find_str_len)) {
        --str_last;
    }
    if (str_first == str_last) return not_found;
    return str_last - 1;
}

TMSU_DEF const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                         const char* find_str_last) {
    return tmsu_find_last_of_n_ex(str_first, str_last, find_str_first, find_str_last, str_last);
}

TMSU_DEF const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                             const char* find_str_last) {
    return tmsu_find_last_not_of_n_ex(str_first, str_last, find_str_first, find_str_last, str_last);
}

/* Case insensitive */

TMSU_DEF const char* tmsu_find_char_ignore_case_ansi(const char* str, char c) {
    char upper = (char)TM_TOUPPER(TMSU_C2I(c));
    char lower = (char)TM_TOLOWER(TMSU_C2I(c));
    if (upper == lower) {
        return tmsu_find_char(str, c);
    }
    return tmsu_find_char2(str, (char)upper, (char)lower);
}
TMSU_DEF const char* tmsu_find_ignore_case_ansi(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    return tmsu_find_ignore_case_ansi_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF const char* tmsu_find_char_ignore_case_ansi_n(const char* str_first, const char* str_last, char c) {
    TM_ASSERT(str_first <= str_last);

    size_t len = tmsu_distance_sz(str_first, str_last);
    if (!len) return str_last;
    char upper = (char)(TM_TOUPPER(TMSU_C2I(c)));
    char lower = (char)(TM_TOLOWER(TMSU_C2I(c)));
    if (upper == lower) {
        const char* match = (const char*)TM_MEMCHR(str_first, TMSU_C2I(c), len);
        return (match) ? match : str_last;
    }
    return tmsu_find_char2_n(str_first, str_last, upper, lower);
}
TMSU_DEF const char* tmsu_find_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                  const char* find_str_first, const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first;                                       /* Empty string always matches. */
    if (find_str_len > tmsu_distance_sz(str_first, str_last)) return str_last; /* Not enough room for a match. */

    /* We can reduce str_last by find_str_len, since the remaining size at the end doesn't allow for a match. */
    str_last -= find_str_len;
    const char* cur = str_first;
    while ((cur = tmsu_find_char_ignore_case_ansi_n(cur, str_last, *find_str_first)) != str_last) {
        if (tmsu_compare_ignore_case_ansi_n(cur, cur + find_str_len, find_str_first, find_str_last) == 0) {
            return cur;
        }
        ++cur;
    }
    return str_last;
}

/* Find functions that allow escaping of the character to look for. Useful for parsing. */

static tm_bool tmsu_is_char_unescaped(const char* first, const char* last, char escape_char) {
    /* Count how many escape chars precede last. */
    const char* pos = last;
    while (pos > first && *(pos - 1) == escape_char) --pos;
    TM_ASSERT(last >= pos);
    size_t preceding_escape_chars_count = (size_t)(last - pos);
    /* If there are even number of escape chars before last, they all escaped each other. */
    return (preceding_escape_chars_count & 1) == 0;
}

TMSU_DEF const char* tmsu_find_char_unescaped(const char* str, char c, char escape_char) {
    TM_ASSERT(str);
    if (c != escape_char) {
        if (*str == c) return str;
        const char* start = str;
        for (;;) {
            str = tmsu_find_char(str + 1, c);
            if (!*str) return str;
            if (tmsu_is_char_unescaped(start, str, escape_char)) return str;
        }
    } else {
        /* We are looking for an unescaped escape char.
         * Since we are doing lookahead, we don't need to backtrack like in the other case. */
        for (;;) {
            str = tmsu_find_char(str, c);
            if (!*str) return str;
            if (*(str + 1) == c) {
                str += 2;
                continue;
            }
            return str;
        }
    }
}
TMSU_DEF const char* tmsu_find_first_of_unescaped(const char* str, const char* find_str, char escape_char) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    if (!*find_str) return str; /* Empty always matches. */
    const char* first = str;
    for (;;) {
        str = tmsu_find_first_of(str, find_str);
        if (!*str) return str;
        if (*str == escape_char) {
            if (*(str + 1) == escape_char) {
                str += 2;
                first = str;
                continue;
            }
            return str;
        }
        if (tmsu_is_char_unescaped(first, str, escape_char)) break;
        ++str;
    }
    return str;
}
TMSU_DEF const char* tmsu_find_char_unescaped_n(const char* str_first, const char* str_last, char c, char escape_char) {
    TM_ASSERT(str_first <= str_last);
    if (str_first == str_last) return str_last;

    const char* str = str_first;
    if (c != escape_char) {
        if (*str == c) return str;
        for (;;) {
            str = tmsu_find_char_n(str + 1, str_last, c);
            if (str == str_last) return str;
            if (tmsu_is_char_unescaped(str_first, str, escape_char)) return str;
        }
    } else {
        /* We are looking for an unescaped escape char.
         * Since we are doing lookahead, we don't need to backtrack like in the other case. */
        for (;;) {
            str = tmsu_find_char_n(str, str_last, c);
            if (str == str_last) return str;
            if (str + 1 != str_last && *(str + 1) == c) {
                str += 2;
                continue;
            }
            return str;
        }
    }
}
TMSU_DEF const char* tmsu_find_first_of_unescaped_n(const char* str_first, const char* str_last,
                                                    const char* find_str_first, const char* find_str_last,
                                                    char escape_char) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);
    if (str_first == str_last) return str_last;
    if (find_str_first == find_str_last) return str_first; /* Empty string always matches. */

    const char* str = str_first;
    for (;;) {
        str = tmsu_find_first_of_n(str, str_last, find_str_first, find_str_last);
        if (str == str_last) return str;
        if (*str == escape_char) {
            if (str + 1 != str_last && *(str + 1) == escape_char) {
                str += 2;
                str_first = str;
                continue;
            }
            return str;
        }
        if (tmsu_is_char_unescaped(str_first, str, escape_char)) break;
        ++str;
    }
    return str;
}

/* Tokenizer */

TMSU_DEF tmsu_tokenizer tmsu_make_tokenizer(const char* str) {
    TM_ASSERT(str);
    tmsu_tokenizer result;
    result.current = str;
    return result;
}

TMSU_DEF tm_bool tmsu_next_token(tmsu_tokenizer* tokenizer, const char* delimiters, tmsu_string_view* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->current);
    TM_ASSERT(delimiters);

    /* Skip delimiters at the beginning. */
    tokenizer->current = tmsu_find_first_not_of(tokenizer->current, delimiters);
    if (!*tokenizer->current) return TM_FALSE;
    /* Skip skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of(tokenizer->current, delimiters);
    if (out) {
        *out = TMSU_STRING_VIEW_MAKE(tokenizer->current, tmsu_distance(tokenizer->current, next));
    }
    tokenizer->current = next;
    return TM_TRUE;
}

TMSU_DEF tmsu_tokenizer_n tmsu_make_tokenizer_n(const char* first, const char* last) {
    TM_ASSERT(first && first <= last);

    tmsu_tokenizer_n result;
    result.first = first;
    result.last = last;
    return result;
}

TMSU_DEF tm_bool tmsu_next_token_n(tmsu_tokenizer_n* tokenizer, const char* delimiters_first,
                                   const char* delimiters_last, tmsu_string_view* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->first && tokenizer->first <= tokenizer->last);
    TM_ASSERT(delimiters_first && delimiters_first <= delimiters_last);

    /* Skip delimiters at the beginning. */
    tokenizer->first = tmsu_find_first_not_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (tokenizer->first == tokenizer->last) return TM_FALSE;
    /* Skip skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (out) *out = TMSU_STRING_VIEW_MAKE(tokenizer->first, tmsu_distance(tokenizer->first, next));
    tokenizer->first = next;
    return TM_TRUE;
}

/* Word tokenizing. */
static const char TMSU_WORD_SEPERATORS[] = " \t\n\v\f\r./\\()\"'-:,.;<>~!@#$%^&*|+=[]{}`~?";
TMSU_DEF const char* tmsu_find_word_end(const char* str) {
    TM_ASSERT(str);
    return tmsu_find_word_end_n_ex(str, str + TM_STRLEN(str), TMSU_WORD_SEPERATORS,
                                   TMSU_WORD_SEPERATORS + (sizeof(TMSU_WORD_SEPERATORS) - 1));
}
TMSU_DEF const char* tmsu_find_word_end_ex(const char* str, const char* word_seperators) {
    TM_ASSERT(str);
    TM_ASSERT(word_seperators);
    return tmsu_find_word_end_n_ex(str, str + TM_STRLEN(str), word_seperators,
                                   word_seperators + TM_STRLEN(word_seperators));
}
TMSU_DEF const char* tmsu_find_word_end_n(const char* first, const char* last) {
    return tmsu_find_word_end_n_ex(first, last, TMSU_WORD_SEPERATORS,
                                   TMSU_WORD_SEPERATORS + (sizeof(TMSU_WORD_SEPERATORS) - 1));
}
TMSU_DEF const char* tmsu_find_word_end_n_ex(const char* first, const char* last, const char* word_seperators_first,
                                             const char* word_seperators_last) {
    first = tmsu_trim_left_n(first, last);
    return tmsu_find_first_of_n(first, last, word_seperators_first, word_seperators_last);
}

TMSU_DEF const char* tmsu_find_word_start_n(const char* first, const char* last) {
    return tmsu_find_word_start_n_ex(first, last, TMSU_WORD_SEPERATORS,
                                     TMSU_WORD_SEPERATORS + (sizeof(TMSU_WORD_SEPERATORS) - 1));
}
TMSU_DEF const char* tmsu_find_word_start_n_ex(const char* first, const char* last, const char* word_seperators_first,
                                               const char* word_seperators_last) {
    last = tmsu_trim_right_n(first, last);
    const char* word_start = first;
    if (first != last) {
        word_start = tmsu_find_last_of_n_ex(first, last, word_seperators_first, word_seperators_last, TM_NULL);
        if (word_start) {
            // We found a word seperator, skip it, so that word_start actually points to the beginning of a word.
            ++word_start;
        } else {
            word_start = first;
        }
    }
    return word_start;
}

/* Whitespace trimming */

static const int TMSU_WHITESPACE_COUNT = 6;
static char const* const TMSU_WHITESPACE = " \t\n\v\f\r";

TMSU_DEF const char* tmsu_trim_left(const char* str) { return tmsu_find_first_not_of(str, TMSU_WHITESPACE); }

TMSU_DEF const char* tmsu_trim_left_n(const char* first, const char* last) {
    return tmsu_find_first_not_of_n(first, last, TMSU_WHITESPACE, TMSU_WHITESPACE + TMSU_WHITESPACE_COUNT);
}
TMSU_DEF const char* tmsu_trim_right_n(const char* first, const char* last) {
    const char* result =
        tmsu_find_last_not_of_n_ex(first, last, TMSU_WHITESPACE, TMSU_WHITESPACE + TMSU_WHITESPACE_COUNT, TM_NULL);
    if (result == TM_NULL) {
        /* No non whitespace found, point to first. */
        result = first;
    } else {
        /* Result points to non whitespace character, advance past it. */
        ++result;
    }
    return result;
}

/* Comparisons */

TMSU_DEF int tmsu_compare(const char* a, const char* b) {
    TM_ASSERT(a);
    TM_ASSERT(b);
#ifdef TM_STRCMP
    return TM_STRCMP(a, b);
#else
    while (*a && *b && *a == *b) {
        ++a;
        ++b;
    }
    return *b - *a;
#endif
}
TMSU_DEF int tmsu_compare_ignore_case_ansi(const char* a, const char* b) {
    TM_ASSERT(a);
    TM_ASSERT(b);
#ifdef TM_STRICMP
    return TM_STRICMP(a, b);
#else
    while (*a && *b && TM_TOUPPER(TMSU_C2I(*a)) == TM_TOUPPER(TMSU_C2I(*b))) {
        ++a;
        ++b;
    }
    return TM_TOUPPER(TMSU_C2I(*b)) - TM_TOUPPER(TMSU_C2I(*a));
#endif
}

TMSU_DEF int tmsu_compare_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !b_len - !a_len;
    if (a_first == b_first) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    size_t len = (a_len < b_len) ? a_len : b_len;
    if (len) {
        int common_cmp = TM_MEMCMP(a_first, b_first, len);
        if (common_cmp != 0) return common_cmp;
    }

    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    return 0;
}
TMSU_DEF int tmsu_compare_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                             const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !b_len - !a_len;
    if (a_first == b_first) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    size_t len = (a_len < b_len) ? a_len : b_len;
    for (size_t i = 0; i < len; ++i) {
        int a = TM_TOUPPER(TMSU_C2I(a_first[i]));
        int b = TM_TOUPPER(TMSU_C2I(b_first[i]));
        int diff = a - b;
        if (diff != 0) return diff;
    }

    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    return 0;
}

/*
tmsu_human_compare_n implementation is based on this gist: https://gist.github.com/pervognsen/733034 by Per Vognsen.
*/
static int tmsu_human_parse_and_advance(const char** str, const char* last) {
    const char* p = *str;
    TM_ASSERT(p != last);
    if (!tmsu_isdigit(TMSU_C2I(*p))) {
        ++*str;
        return TM_TOUPPER(TMSU_C2I(*p));
    }

    int result = 0;
    while (p != last && tmsu_isdigit(*p)) {
        result = (result * 10) + (*p - '0');
        ++p;
    }
    *str = p;
    return result + 256;
}

TMSU_DEF int tmsu_human_compare_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                       const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !b_len - !a_len;
    if (a_first == b_first) {
        if (a_len > b_len) return 1;
        if (a_len < b_len) return -1;
        return 0;
    }

    while (a_first != a_last && b_first != b_last) {
        int a = tmsu_human_parse_and_advance(&a_first, a_last);
        int b = tmsu_human_parse_and_advance(&b_first, b_last);
        int diff = a - b;
        if (diff != 0) return diff;
    }

    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    return 0;
}

TMSU_DEF tm_bool tmsu_equals_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last) {
    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (a_len != b_len) return TM_FALSE;
    return TM_MEMCMP(a_first, b_first, a_len) == 0;
}
TMSU_DEF tm_bool tmsu_equals_ignore_case_ansi_n(const char* a_first, const char* a_last, const char* b_first,
                                                const char* b_last) {
    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (a_len != b_len) return TM_FALSE;
    for (size_t i = 0; i < a_len; ++i) {
        int a = TM_TOUPPER(TMSU_C2I(a_first[i]));
        int b = TM_TOUPPER(TMSU_C2I(b_first[i]));
        if (a != b) return TM_FALSE;
    }
    return TM_TRUE;
}

TMSU_DEF tm_bool tmsu_starts_with(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    size_t find_str_len = TM_STRLEN(find_str);
    return TM_STRNCMP(str, find_str, find_str_len) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    return tmsu_ends_with_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF tm_bool tmsu_starts_with_n(const char* str_first, const char* str_last, const char* find_str_first,
                                    const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    return TM_MEMCMP(str_first, find_str_first, find_str_len) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with_n(const char* str_first, const char* str_last, const char* find_str_first,
                                  const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    const char* start = str_last - find_str_len;
    return TM_MEMCMP(start, find_str_first, find_str_len) == 0;
}

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    size_t find_str_len = TM_STRLEN(find_str);
    return tmsu_strnicmp(str, find_str, find_str_len) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    return tmsu_ends_with_ignore_case_ansi_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                     const char* find_str_first, const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    return tmsu_compare_ignore_case_ansi_n(str_first, str_first + find_str_len, find_str_first, find_str_last) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_ansi_n(const char* str_first, const char* str_last,
                                                   const char* find_str_first, const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    const char* start = str_last - find_str_len;
    return tmsu_compare_ignore_case_ansi_n(start, str_last, find_str_first, find_str_last) == 0;
}

/* Crt extensions */

TMSU_DEF tm_bool tmsu_isdigit(unsigned c) { return c >= '0' && c <= '9'; }

TMSU_DEF const char* tmsu_stristr(const char* str, const char* find_str) {
    return tmsu_find_ignore_case_ansi(str, find_str);
}
TMSU_DEF int tmsu_stricmp(const char* a, const char* b) {
    while (*a && *b) {
        int aUpper = TM_TOUPPER(TMSU_C2I(*a));
        int bUpper = TM_TOUPPER(TMSU_C2I(*b));
        if (aUpper != bUpper) {
            break;
        }
        ++a;
        ++b;
    }
    int aUpper = TM_TOUPPER(TMSU_C2I(*a));
    int bUpper = TM_TOUPPER(TMSU_C2I(*b));
    return aUpper - bUpper;
}
TMSU_DEF int tmsu_strnicmp(const char* a, const char* b, size_t count) {
    while (*a && *b && count--) {
        int aUpper = TM_TOUPPER(TMSU_C2I(*a));
        int bUpper = TM_TOUPPER(TMSU_C2I(*b));
        if (aUpper != bUpper) {
            break;
        }
        ++a;
        ++b;
    }
    if (!count) {
        return 0;
    }
    int aUpper = TM_TOUPPER(TMSU_C2I(*a));
    int bUpper = TM_TOUPPER(TMSU_C2I(*b));
    return aUpper - bUpper;
}
TMSU_DEF char* tmsu_strrev(char* str) { return tmsu_strnrev(str, TM_STRLEN(str)); }
TMSU_DEF char* tmsu_strnrev(char* str, size_t count) {
    for (size_t i = 0, j = count - 1; i < j; ++i, --j) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
    return str;
}
TMSU_DEF const void* tmsu_memrchr(const void* ptr, int value, size_t len) {
    const char* p = (const char*)ptr + len;
    while (len) {
        --len;
        --p;
        if ((unsigned char)*p == value) {
            return (const void*)p;
        }
    }
    return TM_NULL;
}

static char const* const tmsu_base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static char const* const tmsu_base64url_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789-_";

static tm_size_t tmsu_base64_encode_chars(char const* const chars, tm_bool pad, const void* input_bytes,
                                          tm_size_t input_size, char* out, tm_size_t out_size) {
    TM_ASSERT((chars == tmsu_base64_chars) || (chars == tmsu_base64url_chars));

    tm_size_t out_index = 0;
    const char* in = (const char*)input_bytes;
    unsigned int char_index[4];
    tm_size_t i = 0;
    for (; input_size >= 3; i += 3, input_size -= 3) {
        unsigned int a = (unsigned char)in[i];
        unsigned int b = (unsigned char)in[i + 1];
        unsigned int c = (unsigned char)in[i + 2];

        // Divide each three 8 bit blocks into four 6 bit blocks:
        // AAAAAA AABBBB BBBBCC CCCCCC
        char_index[0] = (a >> 2u);
        char_index[1] = ((a & 0x3u) << 4) | (b >> 4u);
        char_index[2] = ((b & 0xFu) << 2) | (c >> 6u);
        char_index[3] = (c & 0x3Fu);

        TM_ASSERT(char_index[0] < 64);
        TM_ASSERT(char_index[1] < 64);
        TM_ASSERT(char_index[2] < 64);
        TM_ASSERT(char_index[3] < 64);

        if (out_index + 4 < out_size) {
            out[out_index + 0] = chars[char_index[0]];
            out[out_index + 1] = chars[char_index[1]];
            out[out_index + 2] = chars[char_index[2]];
            out[out_index + 3] = chars[char_index[3]];
        }
        out_index += 4;
    }

    // The remaining size is either 1 or 2, it can't be 3, since it would have been handeled by the above loop.
    TM_ASSERT(input_size == 0 || input_size == 1 || input_size == 2);
    if (input_size > 0) {
        unsigned int a = (unsigned char)in[i];
        unsigned int b = (input_size > 1) ? (unsigned char)in[i + 1] : 0;

        // Divide each three 8 bit blocks into four 6 bit blocks:
        // AAAAAA AABBBB BBBBCC CCCCCC
        char_index[0] = (a >> 2u);
        char_index[1] = ((a & 0x3u) << 4) | (b >> 4u);
        char_index[2] = ((b & 0xFu) << 2);
        char_index[3] = 0;

        TM_ASSERT(char_index[0] < 64);
        TM_ASSERT(char_index[1] < 64);
        TM_ASSERT(char_index[2] < 64);
        TM_ASSERT(char_index[3] < 64);

        if (out_index + 2 < out_size) {
            out[out_index + 0] = chars[char_index[0]];
            out[out_index + 1] = chars[char_index[1]];
            out_index += 2;
        }
        if (input_size > 1) {
            if (out_index + 1 < out_size) out[out_index] = chars[char_index[2]];
            ++out_index;
        } else if (pad) {
            if (out_index + 1 < out_size) out[out_index] = '=';
            ++out_index;
        }

        if (pad) {
            if (out_index + 1 < out_size) out[out_index] = '=';
            ++out_index;
        }
    }
    return out_index;
}

static tm_size_t tmsu_base64_decode_chars(char const* const chars, tm_bool expect_padding,
                                          const char* base64_encoded_string, tm_size_t base64_input_size, void* out,
                                          tm_size_t out_size) {
    TM_ASSERT((chars == tmsu_base64_chars) || (chars == tmsu_base64url_chars));
    TM_ASSERT((base64_input_size % 4 == 0) || !expect_padding);

    tm_size_t out_index = 0;
    char* p = (char*)out;
    unsigned int char_value[4];
    tm_size_t i = 0;
    for (; base64_input_size > 4; i += 4, base64_input_size -= 4) {
        char_value[0] = (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 0]) - chars);
        char_value[1] = (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 1]) - chars);
        char_value[2] = (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 2]) - chars);
        char_value[3] = (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 3]) - chars);

        if (char_value[0] >= 64) return 0;
        if (char_value[1] >= 64) return 0;
        if (char_value[2] >= 64) return 0;
        if (char_value[3] >= 64) return 0;

        // Decompose each four 6 bit blocks into three 8 bit blocks:
        // 00000011 11112222 22333333
        if (out_index + 3 < out_size) {
            p[out_index + 0] = (char)((char_value[0] << 2) | (char_value[1] >> 4));
            p[out_index + 1] = (char)(((char_value[1] << 4) & 0xF0u) | (char_value[2] >> 2));
            p[out_index + 2] = (char)(((char_value[2] << 6) & 0xC0u) | (char_value[3]));
        }
        out_index += 3;
    }

    // Handle last 4 bytes seperately, since they might have padding '='.
    if (base64_input_size > 0) {
        int padding_count = 0;
        if (expect_padding) {
            char_value[0] = (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 0]) - chars);
            char_value[1] = (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 1]) - chars);
            char_value[2] = (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 2]) - chars);
            char_value[3] = (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 3]) - chars);

            if (char_value[0] >= 64) return 0;
            if (char_value[1] >= 64) return 0;
            if (char_value[3] >= 64) {
                if (base64_encoded_string[i + 3] != '=') return 0;
                char_value[3] = 0;
                padding_count = 1;
            }
            if (char_value[2] >= 64) {
                if (base64_encoded_string[i + 2] != '=') return 0;
                char_value[2] = 0;
                padding_count = 2;
            }
        } else {
            char_value[0] = (base64_input_size > 0) ? (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 0]) - chars) : 0;
            char_value[1] = (base64_input_size > 1) ? (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 1]) - chars) : 0;
            char_value[2] = (base64_input_size > 2) ? (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 2]) - chars) : 0;
            char_value[3] = (base64_input_size > 3) ? (unsigned int)(tmsu_find_char(chars, base64_encoded_string[i + 3]) - chars) : 0;

            if (char_value[0] >= 64) return 0;
            if (char_value[1] >= 64) return 0;
            if (char_value[2] >= 64) return 0;
            if (char_value[3] >= 64) return 0;

            padding_count = 2 - (base64_input_size > 2) - (base64_input_size > 3);
        }

        // Decompose each four 6 bit blocks into three 8 bit blocks:
        // 00000011 11112222 22333333
        if (out_index + 1 < out_size) p[out_index] = (char)((char_value[0] << 2) | (char_value[1] >> 4));
        ++out_index;
        if (padding_count < 2) {
            if (out_index + 1 < out_size) p[out_index] = (char)(((char_value[1] << 4) & 0xF0u) | (char_value[2] >> 2));
            ++out_index;
        }
        if (padding_count < 1) {
            if (out_index + 1 < out_size)  p[out_index] = (char)(((char_value[2] << 6) & 0xC0u) | (char_value[3]));
            ++out_index;
        }
    }
    return out_index;
}

TMSU_DEF tm_size_t tmsu_base64_encode(const void* input_bytes, tm_size_t input_size, char* out, tm_size_t out_size) {
    TM_ASSERT(input_bytes || input_size == 0);
    TM_ASSERT(out || out_size == 0);
    return tmsu_base64_encode_chars(tmsu_base64_chars, /*pad=*/TM_TRUE, input_bytes, input_size, out, out_size);
}
TMSU_DEF tm_size_t tmsu_base64url_encode(const void* input_bytes, tm_size_t input_size, char* out, tm_size_t out_size) {
    TM_ASSERT(input_bytes || input_size == 0);
    TM_ASSERT(out || out_size == 0);
    return tmsu_base64_encode_chars(tmsu_base64url_chars, /*pad=*/TM_FALSE, input_bytes, input_size, out, out_size);
}
TMSU_DEF tm_size_t tmsu_base64_decode(const char* base64_encoded_string, tm_size_t base64_input_size, void* out,
                                      tm_size_t out_size) {
    TM_ASSERT(base64_encoded_string || base64_input_size == 0);
    TM_ASSERT(out || out_size == 0);
    if (base64_input_size == 0) return 0;
    if (base64_input_size % 4 != 0) return 0;
    return tmsu_base64_decode_chars(tmsu_base64_chars, /*expect_padding=*/TM_TRUE, base64_encoded_string,
                                    base64_input_size, out, out_size);
}
TMSU_DEF tm_size_t tmsu_base64url_decode(const char* base64url_encoded_string, tm_size_t base64url_input_size,
                                         void* out, tm_size_t out_size) {
    TM_ASSERT(base64url_encoded_string || base64url_input_size == 0);
    TM_ASSERT(out || out_size == 0);
    if (base64url_input_size < 2) return 0;
    return tmsu_base64_decode_chars(tmsu_base64url_chars, /*expect_padding=*/TM_FALSE, base64url_encoded_string,
                                    base64url_input_size, out, out_size);
}

TMSU_DEF tm_size_t tmsu_url_encode(const void* input, tm_size_t input_size, char* out, tm_size_t out_size) {
    TM_ASSERT(input || input_size == 0);
    TM_ASSERT(out || out_size == 0);

    const char* in = (const char*)input;
    tm_size_t out_index = 0;
    for (tm_size_t i = 0; i < input_size; ++i) {
        unsigned int c = (unsigned char)in[i];
        // Special case characters, that are allowed in uri strings.
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '-') || (c == '_') ||
            (c == '.')) {
            if (out_index < out_size) out[out_index] = (char)c;
            ++out_index;
            continue;
        }

        // Percent encode any other byte.
        // Byte to hex inplace.
        unsigned int l = (c >> 4u) & 0x0Fu;
        l += (l <= 9) ? '0' : ('A' - 10);
        unsigned int r = c & 0x0Fu;
        r += (r <= 9) ? '0' : ('A' - 10);

        if (out_index < out_size) out[out_index] = '%';
        ++out_index;
        if (out_index < out_size) out[out_index] = (char)l;
        ++out_index;
        if (out_index < out_size) out[out_index] = (char)r;
        ++out_index;
    }
    return out_index;
}

TMSU_DEF tm_size_t tmsu_url_decode(const char* url_encoded_input, tm_size_t input_size, void* out, tm_size_t out_size) {
    TM_ASSERT(url_encoded_input || input_size == 0);
    TM_ASSERT(out || out_size == 0);

    char* p = (char*)out;
    tm_size_t out_index = 0;
    for (tm_size_t i = 0; i < input_size; ++i) {
        unsigned int c = (unsigned char)url_encoded_input[i];
        // Special case characters, that are allowed in uri strings.
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '-') || (c == '_') ||
            (c == '.')) {
            if (out_index < out_size) p[out_index] = (char)c;
            ++out_index;
            continue;
        }

        if (c == '%') {
            // Check for malformed input.
            if (input_size - i < 3) return 0;

            // Percent decode.
            unsigned int l = (unsigned char)url_encoded_input[i + 1];
            unsigned int r = (unsigned char)url_encoded_input[i + 2];

            // Hex to byte inplace.
            if (l >= '0' && l <= '9')
                l -= '0';
            else if (l >= 'A' && l <= 'F')
                l = (l - 'A') + 10;
            else if (l >= 'a' && l <= 'f')
                l = (l - 'a') + 10;
            else
                return 0;

            if (r >= '0' && r <= '9')
                r -= '0';
            else if (r >= 'A' && r <= 'F')
                r = (r - 'A') + 10;
            else if (r >= 'a' && r <= 'f')
                r = (r - 'a') + 10;
            else
                return 0;

            if (out_index < out_size) p[out_index] = (char)((unsigned char)((l << 4u) | r));
            ++out_index;
            i += 2;
            continue;
        }

        // Invalid character encountered.
        return 0;
    }
    return out_index;
}

#ifdef __cplusplus
}
#endif