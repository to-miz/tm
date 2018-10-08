#ifdef __cplusplus
extern "C" {
#endif

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

TMSU_DEF const char* tmsu_find_last_char_n(const char* str_first, const char* str_last, char c) {
    TM_ASSERT(str_first <= str_last);
#ifdef TM_MEMRCHR
    void* result = TM_MEMRCHR(str_first, TMSU_C2I(c), tmsu_distance_sz(str_first, str_last));
    return (result) ? (const char*)result : str_last;
#else
    const char* cur = str_last;
    while (str_first != cur) {
        --cur;
        if (TMSU_C2I(*cur) == TMSU_C2I(c)) {
            return cur;
        }
    };
    return str_last;
#endif
}

TMSU_DEF const char* tmsu_find_n(const char* str_first, const char* str_last, const char* find_str_first,
                                 const char* find_str_last) {
    TM_ASSERT(str_first && str_first <= str_last);
    TM_ASSERT(find_str_first && find_str_first <= find_str_last);

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

TMSU_DEF const char* tmsu_find_last_n(const char* str_first, const char* str_last, const char* find_str_first,
                                      const char* find_str_last) {
    TM_ASSERT(str_first && str_first <= str_last);
    TM_ASSERT(find_str_first && find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first;                                       /* Empty string always matches */
    if (find_str_len > tmsu_distance_sz(str_first, str_last)) return str_last; /* Not enough room for a match. */

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
    return str_last;
}

TMSU_DEF const char* tmsu_find(const char* str, const char* find_str) {
    TM_ASSERT(str);

#ifdef TM_STRSTR
    return TM_STRSTR(str, find_str);
#else
    /* TODO: is this better than calling the _n variant with two strlens? */
#if 0
    return tmsu_find_n(str, str + TM_STRLEN(str), find_str, TM_STRLEN(find_str));
#else
    size_t find_str_len = TM_STRLEN(find_str);
    if (!find_str || !*find_str) return str;
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
    while (*str && TM_STRCHR(find_str, TMSU_C2I(*str))) {
        ++str;
    }
    return str;
}

TMSU_DEF const char* tmsu_find_first_of(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    while (*str && TM_STRCHR(find_str, TMSU_C2I(*str)) == TM_NULL) {
        ++str;
    }
    return str;
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
    while (str_first != str_last && TM_MEMCHR(find_str_first, TMSU_C2I(*str_first), find_str_len)) {
        ++str_first;
    }
    return str_first;
}

TMSU_DEF const char* tmsu_find_last_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                         const char* find_str_last) {
    if (str_first == str_last) {
        return str_last;
    }
    --str_last;
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_last != str_first && TM_MEMCHR(find_str_first, TMSU_C2I(*str_last), find_str_len) == TM_NULL) {
        --str_last;
    }
    return str_last;
}

TMSU_DEF const char* tmsu_find_last_not_of_n(const char* str_first, const char* str_last, const char* find_str_first,
                                             const char* find_str_last) {
    if (str_first == str_last) {
        return str_last;
    }
    --str_last;
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    while (str_last != str_first && TM_MEMCHR(find_str_first, TMSU_C2I(*str_last), find_str_len)) {
        --str_last;
    }
    return str_last;
}

/* Case insensitive */

TMSU_DEF const char* tmsu_find_char_ignore_case(const char* str, char c) {
    char upper = (char)TM_TOUPPER(TMSU_C2I(c));
    char lower = (char)TM_TOLOWER(TMSU_C2I(c));
    if (upper == lower) {
        return tmsu_find_char(str, c);
    }
    return tmsu_find_char2(str, (char)upper, (char)lower);
}
TMSU_DEF const char* tmsu_find_ignore_case(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);
    return tmsu_find_ignore_case_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF const char* tmsu_find_char_ignore_case_n(const char* str_first, const char* str_last, char c) {
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
TMSU_DEF const char* tmsu_find_ignore_case_n(const char* str_first, const char* str_last, const char* find_str_first,
                                             const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (!find_str_len) return str_first;                                       /* Empty string always matches */
    if (find_str_len > tmsu_distance_sz(str_first, str_last)) return str_last; /* Not enough room for a match. */

    /* We can reduce str_last by find_str_len, since the remaining size at the end doesn't allow for a match. */
    str_last -= find_str_len;
    const char* cur = str_first;
    while ((cur = tmsu_find_char_ignore_case_n(cur, str_last, *find_str_first)) != str_last) {
        if (tmsu_compare_ignore_case_n(cur, cur + find_str_len, find_str_first, find_str_last) == 0) {
            return cur;
        }
        ++cur;
    }
    return str_last;
}

/* Tokenizer */

TMSU_DEF tmsu_tokenizer tmsu_make_tokenizer(const char* str) {
    TM_ASSERT(str);
    tmsu_tokenizer result;
    result.current = str;
    return result;
}

TMSU_DEF tm_bool tmsu_next_token(tmsu_tokenizer* tokenizer, const char* delimiters, tmsu_stringview* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->current);
    TM_ASSERT(delimiters);

    /* Skip delimiters at the beginning. */
    tokenizer->current = tmsu_find_first_not_of(tokenizer->current, delimiters);

    if (!*tokenizer->current) {
        return TM_FALSE;
    }
    /* Skip skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of(tokenizer->current, delimiters);
    if (out) {
        *out = tmsu_make_stringview(tokenizer->current, tmsu_distance(tokenizer->current, next));
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
                                   const char* delimiters_last, tmsu_stringview* out) {
    TM_ASSERT(tokenizer);
    TM_ASSERT(tokenizer->first && tokenizer->first <= tokenizer->last);
    TM_ASSERT(delimiters_first && delimiters_first <= delimiters_last);

    /* Skip delimiters at the beginning. */
    tokenizer->first = tmsu_find_first_not_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);

    if (tokenizer->first == tokenizer->last) {
        return TM_FALSE;
    }
    /* Skip skip everything until we find other delimiters. */
    const char* next = tmsu_find_first_of_n(tokenizer->first, tokenizer->last, delimiters_first, delimiters_last);
    if (out) {
        *out = tmsu_make_stringview(tokenizer->first, tmsu_distance(tokenizer->first, next));
    }
    tokenizer->first = next;
    return TM_TRUE;
}

/* Whitespace trimming */

TMSU_DEF const char* tmsu_trim_left(const char* str) {
    TM_ASSERT(str);
    while (*str && TM_ISSPACE(TMSU_C2I(*str))) {
        ++str;
    }
    return str;
}
TMSU_DEF tmsu_stringview tmsu_trim(const char* str) {
    TM_ASSERT(str);
    return tmsu_trim_n(str, str + TM_STRLEN(str));
}

TMSU_DEF const char* tmsu_trim_left_n(const char* first, const char* last) {
    TM_ASSERT(first <= last);

    while (first != last && TM_ISSPACE(TMSU_C2I(*first))) {
        ++first;
    }
    return first;
}
TMSU_DEF const char* tmsu_trim_right_n(const char* first, const char* last) {
    TM_ASSERT(first <= last);
    if (first == last) return last;
    --last;
    while (first != last && TM_ISSPACE(TMSU_C2I(*last))) {
        --last;
    }
    return last;
}
TMSU_DEF tmsu_stringview tmsu_trim_n(const char* first, const char* last) {
    first = tmsu_trim_left_n(first, last);
    last = tmsu_trim_right_n(first, last);
    return tmsu_make_stringview(first, tmsu_distance(first, last));
}

/* Comparisons */

TMSU_DEF int tmsu_compare_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !a_len - !b_len;
    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    if (a_first == b_first) return 0;

    return TM_MEMCMP(a_first, b_first, a_len);
}
TMSU_DEF int tmsu_compare_ignore_case_n(const char* a_first, const char* a_last, const char* b_first,
                                        const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !a_len - !b_len;
    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    if (a_first == b_first) return 0;

    for (size_t i = 0; i < a_len; ++i) {
        int a = TM_TOUPPER(TMSU_C2I(a_first[i]));
        int b = TM_TOUPPER(TMSU_C2I(b_first[i]));
        int diff = a - b;
        if (diff != 0) return diff;
    }
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

TMSU_DEF int tmsu_human_compare_n(const char* a_first, const char* a_last, const char* b_first, const char* b_last) {
    TM_ASSERT(a_first <= a_last);
    TM_ASSERT(b_first <= b_last);

    size_t a_len = tmsu_distance_sz(a_first, a_last);
    size_t b_len = tmsu_distance_sz(b_first, b_last);
    if (!a_len || !b_len) return !a_len - !b_len;
    if (a_first == b_first) {
        /*return (int)(a_len - b_len);*/ /* Might overflow/underflow. */
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
    return (a_first != a_last) - (b_first != b_last);
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

TMSU_DEF tm_bool tmsu_starts_with_ignore_case(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    size_t find_str_len = TM_STRLEN(find_str);
    return tmsu_strnicmp(str, find_str, find_str_len) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with_ignore_case(const char* str, const char* find_str) {
    TM_ASSERT(str);
    TM_ASSERT(find_str);

    return tmsu_ends_with_ignore_case_n(str, str + TM_STRLEN(str), find_str, find_str + TM_STRLEN(find_str));
}

TMSU_DEF tm_bool tmsu_starts_with_ignore_case_n(const char* str_first, const char* str_last, const char* find_str_first,
                                                const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    return tmsu_compare_ignore_case_n(str_first, str_first + find_str_len, find_str_first, find_str_last) == 0;
}
TMSU_DEF tm_bool tmsu_ends_with_ignore_case_n(const char* str_first, const char* str_last, const char* find_str_first,
                                              const char* find_str_last) {
    TM_ASSERT(str_first <= str_last);
    TM_ASSERT(find_str_first <= find_str_last);

    size_t str_len = tmsu_distance_sz(str_first, str_last);
    size_t find_str_len = tmsu_distance_sz(find_str_first, find_str_last);
    if (str_len < find_str_len) return TM_FALSE;

    const char* start = str_last - find_str_len;
    return tmsu_compare_ignore_case_n(start, str_last, find_str_first, find_str_last) == 0;
}

/* Crt extensions */

TMSU_DEF tm_bool tmsu_isdigit(unsigned c) { return c >= '0' && c <= '9'; }

TMSU_DEF const char* tmsu_stristr(const char* str, const char* find_str) {
    return tmsu_find_ignore_case(str, find_str);
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

#ifdef __cplusplus
}
#endif