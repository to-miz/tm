// Tool to generate unicode data lookup tables for C/C++.

#define VERSION 0x00000200u
#define VERSION_MAJOR (VERSION >> 16u)
#define VERSION_MINOR ((VERSION >> 8u) & 0xFFu)
#define VERSION_PATCH (VERSION & 0xFFu)

#ifdef _MSC_VER
#pragma warning(disable : 4702)  // unreachable code
#endif

#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#include <cstring>
#include <cerrno>
#include <vector>
#include <new>
#include <stdexcept>
#include <algorithm>
#include <string>

// This is a simple tool, this is fine.
using namespace std;

// Helpers.
#define MAYBE_UNUSED(x) ((void)x)
// clang-format off
#ifdef _DEBUG
    #ifdef _MSC_VER
        #define debug_break __debugbreak
        #define break_if(x)            \
            {                          \
                if (x) __debugbreak(); \
            }
    #elif defined(_WIN32)
        #define debug_break DebugBreak
        #define break_if(x)          \
            {                        \
                if (x) DebugBreak(); \
            }
    #else
        #include <signal.h>
        #define debug_break() raise(SIGTRAP)
        #define break_if(x)            \
            {                          \
                if (x) raise(SIGTRAP); \
            }
    #endif

    #define DEBUG_WRAPPER(x) \
        do {                 \
            x;               \
        } while (false)
#else
    #define DEBUG_WRAPPER(x) ((void)0)
    #define break_if(x) ((void)0)
    #define debug_break() ((void)0)
#endif
// clang-format on

#if 1
constexpr const int32_t block_size = 1 << 7;
constexpr const int32_t pruned_size = 1025;
#else
constexpr const int32_t block_size = 1 << 6;
constexpr const int32_t pruned_size = 2049;
#endif


enum generate_flags_enum : uint32_t {
    generate_flags_case_info = (1 << 0),
    generate_flags_category = (1 << 1),
    generate_flags_grapheme_break = (1 << 2),
    generate_flags_width = (1 << 3),

    generate_flags_canonical = (1 << 4),
    generate_flags_compatibility = (1 << 5),

    generate_flags_full_case = (1 << 6),
    generate_flags_full_case_fold = (1 << 7),
    generate_flags_full_case_toggle = (1 << 8),

    generate_flags_simple_case = (1 << 9),
    generate_flags_simple_case_fold = (1 << 10),
    generate_flags_simple_case_toggle = (1 << 11),

    generate_flags_prune_stage_one = (1 << 12),
    generate_flags_prune_stage_two = (1 << 13),
    generate_flags_handle_invalid_codepoints = (1 << 14),

    generate_flags_named_flags_mask = (1 << 20) - 1,

    generate_flags_no_assert = (1 << 20),
    generate_flags_no_header_guard = (1 << 21),

    // generate_flags_binary_search = (1 << 11),

    // These are the default flags when nothing is specified in the command line.
    generate_flags_default = generate_flags_case_info | generate_flags_category | generate_flags_grapheme_break |
                             // generate_flags_canonical |
                             // generate_flags_compatibility |
                             // generate_flags_full_case |
                             // generate_flags_full_case_fold |
                             // generate_flags_full_case_toggle |
                             // generate_flags_simple_case |
                             generate_flags_simple_case_fold |
                             // generate_flags_simple_case_toggle |
                             // generate_flags_prune_stage_one | generate_flags_prune_stage_two |
                             generate_flags_handle_invalid_codepoints
};

#include "cli.cpp"

void fatal_error_impl(const tchar* msg) {
    FPRINTF(stderr, TSTRF, msg);
    debug_break();
    exit(-1);
}
#define fatal_error(x) fatal_error_impl(TEXT(x))

vector<char> read_file(FILE* f) {
    assert(f);
    vector<char> result;
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    size_t size = 0;
    while ((size = fread(buffer, sizeof(char), sizeof(buffer), f)) != 0) {
        result.insert(result.end(), buffer, buffer + size * sizeof(char));
    }

    if (!result.empty()) result.push_back(0);  // Null terminate.

    return result;
}

vector<char> read_file(const tchar* filename) {
    errno = 0;
    FILE* f = FOPEN(filename, TEXT("rb"));
    if (!f) {
        print_error(app_name, filename, TEXT("Failed to open file"));
        exit(-1);
    }
    auto result = read_file(f);
    if (!close_stream(app_name, filename, TEXT("Error reading from"), f)) exit(-1);

    return result;
}

struct string_segment {
    const char* first;
    const char* last;

    size_t size() const {
        assert(first <= last);
        return (size_t)(last - first);
    }
    bool empty() const { return size() == 0; }

    bool equals(const char* other) const {
        auto len = strlen(other);
        if (len != size()) return false;
        return memcmp(first, other, len * sizeof(char)) == 0;
    }

    friend bool operator==(string_segment seg, const char* other) { return seg.equals(other); }
    friend bool operator==(const char* other, string_segment seg) { return seg.equals(other); }
    friend bool operator!=(string_segment seg, const char* other) { return !seg.equals(other); }
    friend bool operator!=(const char* other, string_segment seg) { return !seg.equals(other); }
};

enum language_enum : int8_t {
    language_general,
    language_lt,
    language_tr,
    language_az,

    language_count
};

char const* const language_enum_strings[] = {
    nullptr,
    "lt",
    "tr",
    "az",
};
static_assert(size(language_enum_strings) == language_count, "");
language_enum to_language_enum(string_segment str) {
    for (size_t i = 1; i < size(language_enum_strings); ++i) {
        if (str == language_enum_strings[i]) return (language_enum)i;
    }
    return (language_enum)-1;
}

enum condition_enum : int8_t {
    condition_after_i,
    condition_not_before_dot,
    condition_more_above,
    condition_after_soft_dotted,
    condition_final_sigma,

    condition_count
};
// Strings of the conditions above as they appear in SpecialCasing.txt
char const* const condition_enum_strings[] = {
    "After_I", "Not_Before_Dot", "More_Above", "After_Soft_Dotted", "Final_Sigma",
};
static_assert(size(condition_enum_strings) == condition_count, "");
condition_enum to_condition_enum(string_segment str) {
    for (size_t i = 0; i < size(condition_enum_strings); ++i) {
        if (str == condition_enum_strings[i]) return (condition_enum)i;
    }
    return (condition_enum)-1;
}

enum category_enum : int8_t {
    category_other,                  // Cc | Cf | Cs | Co | Cn.
    category_uppercase_letter,       // An uppercase letter.
    category_lowercase_letter,       // A lowercase letter.
    category_titlecase_letter,       // A digraphic character, with first part uppercase.
    category_cased_letter,           // Lu | Ll | Lt.
    category_modifier_letter,        // A modifier letter.
    category_other_letter,           // Other letters, including syllables and ideographs.
    category_letter,                 // Lu | Ll | Lt | Lm | Lo.
    category_nonspacing_mark,        // A nonspacing combining mark (zero advance width).
    category_spacing_mark,           // A spacing combining mark (positive advance width).
    category_enclosing_mark,         // An enclosing combining mark.
    category_mark,                   // Mn | Mc | Me.
    category_decimal_number,         // A decimal digit.
    category_letter_number,          // A letterlike numeric character.
    category_other_number,           // A numeric character of other type.
    category_number,                 // Nd | Nl | No.
    category_connector_punctuation,  // A connecting punctuation mark, like a tie.
    category_dash_punctuation,       // A dash or hyphen punctuation mark.
    category_open_punctuation,       // An opening punctuation mark (of a pair).
    category_close_punctuation,      // A closing punctuation mark (of a pair).
    category_initial_punctuation,    // An initial quotation mark.
    category_final_punctuation,      // A final quotation mark.
    category_other_punctuation,      // A punctuation mark of other type.
    category_punctuation,            // Pc | Pd | Ps | Pe | Pi | Pf | Po.
    category_math_symbol,            // A symbol of mathematical use.
    category_currency_symbol,        // A currency sign.
    category_modifier_symbol,        // A non-letterlike modifier symbol.
    category_other_symbol,           // A symbol of other type.
    category_symbol,                 // Sm | Sc | Sk | So.
    category_space_separator,        // A space character (of various non-zero widths).
    category_line_separator,         // U+2028 LINE SEPARATOR only.
    category_paragraph_separator,    // U+2029 PARAGRAPH SEPARATOR only.
    category_separator,              // Zs | Zl | Zp.
    category_control,                // A C0 or C1 control code.
    category_control_format,         // A format control character.
    category_control_surrogate,      // A surrogate code point.
    category_control_private_use,    // A private-use character.
    category_control_unassigned,     // A reserved unassigned code point or a noncharacter.

    category_count
};
char const* const category_enum_strings[] = {
    "C",  "Lu", "Ll", "Lt", "LC", "Lm", "Lo", "L",  "Mn", "Mc", "Me", "M",  "Nd", "Nl", "No", "N",  "Pc", "Pd", "Ps",
    "Pe", "Pi", "Pf", "Po", "P",  "Sm", "Sc", "Sk", "So", "S",  "Zs", "Zl", "Zp", "Z",  "Cc", "Cf", "Cs", "Co", "Cn",
};
static_assert(size(category_enum_strings) == category_count, "");

category_enum to_category_enum(string_segment str) {
    for (size_t i = 0; i < size(category_enum_strings); ++i) {
        if (str == category_enum_strings[i]) return (category_enum)i;
    }
    return (category_enum)-1;
}

enum bidirectional_category_enum : int8_t {
    // Strong Types.
    bidirectional_category_left_to_right,  // Any strong left-to-right character.
    bidirectional_category_right_to_left,  // Any strong right-to-left (non-Arabic-type) character.
    bidirectional_category_arabic_letter,  // Any strong right-to-left (Arabic-type) character.
    // Weak Types.
    bidirectional_category_european_number,      // Any ASCII digit or Eastern Arabic-Indic digit.
    bidirectional_category_european_separator,   // Plus and minus signs.
    bidirectional_category_european_terminator,  // A terminator in a numeric format context, includes currency signs.
    bidirectional_category_arabic_number,        // Any Arabic-Indic digit.
    bidirectional_category_common_separator,     // Commas, colons, and slashes.
    bidirectional_category_nonspacing_mark,      // Any nonspacing mark.
    bidirectional_category_boundary_neutral,     // Most format characters, control codes, or noncharacters.
    // Neutral Types.
    bidirectional_category_paragraph_separator,  // Various newline characters.
    bidirectional_category_segment_separator,    // Various segment-related control codes.
    bidirectional_category_whitespace,           // Spaces.
    bidirectional_category_other_neutral,        // Most other symbols and punctuation marks.
    // Explicit Formatting Types.
    bidirectional_category_left_to_right_embedding,  // U+202A: the LR embedding control.
    bidirectional_category_left_to_right_override,   // U+202D: the LR override control.
    bidirectional_category_right_to_left_embedding,  // U+202B: the RL embedding control.
    bidirectional_category_right_to_left_override,   // U+202E: the RL override control.
    bidirectional_category_pop_directional_format,   // U+202C: terminates an embedding or override control.
    bidirectional_category_left_to_right_isolate,    // U+2066: the LR isolate control.
    bidirectional_category_right_to_left_isolate,    // U+2067: the RL isolate control.
    bidirectional_category_first_strong_isolate,     // U+2068: the first strong isolate control.
    bidirectional_category_pop_directional_isolate,  // U+2069: terminates an isolate control.

    bidirectional_category_count
};
char const* const bidirectional_category_enum_strings[] = {
    "L",  "R",  "AL",  "EN",  "ES",  "ET",  "AN",  "CS",  "NSM", "BN",  "B",   "S",
    "WS", "ON", "LRE", "LRO", "RLE", "RLO", "PDF", "LRI", "RLI", "FSI", "PDI",
};
static_assert(size(bidirectional_category_enum_strings) == bidirectional_category_count, "");

bidirectional_category_enum to_bidirectional_category_enum(string_segment str) {
    for (size_t i = 0; i < size(bidirectional_category_enum_strings); ++i) {
        if (str == bidirectional_category_enum_strings[i]) return (bidirectional_category_enum)i;
    }
    return (bidirectional_category_enum)-1;
}

enum grapheme_break_enum : int8_t {
    grapheme_break_other,
    grapheme_break_cr,
    grapheme_break_lf,
    grapheme_break_control,
    grapheme_break_prepend,
    grapheme_break_extend,
    grapheme_break_regional_indicator,
    grapheme_break_spacing_mark,
    grapheme_break_l,
    grapheme_break_v,
    grapheme_break_t,
    grapheme_break_lv,
    grapheme_break_lvt,
    grapheme_break_zwj,
    grapheme_break_extended_pictographic,

    grapheme_break_count,

    // Additional values for state machine transitions.
    grapheme_break_extended_pictographic_zwj = grapheme_break_count,
};
char const* const grapheme_break_enum_strings[] = {
    "Other", "CR", "LF", "Control", "Prepend", "Extend", "Regional_Indicator",   "SpacingMark",
    "L",     "V",  "T",  "LV",      "LVT",     "ZWJ",    "Extended_Pictographic"};
static_assert(size(grapheme_break_enum_strings) == grapheme_break_count, "");
grapheme_break_enum to_grapheme_break_enum(string_segment str) {
    for (size_t i = 0; i < size(grapheme_break_enum_strings); ++i) {
        if (str == grapheme_break_enum_strings[i]) return (grapheme_break_enum)i;
    }
    return (grapheme_break_enum)-1;
}
const char* to_string(grapheme_break_enum gb) {
    static char const* const strings[] = {"grapheme_break_other",
                                          "grapheme_break_cr",
                                          "grapheme_break_lf",
                                          "grapheme_break_control",
                                          "grapheme_break_prepend",
                                          "grapheme_break_extend",
                                          "grapheme_break_regional_indicator",
                                          "grapheme_break_spacing_mark",
                                          "grapheme_break_l",
                                          "grapheme_break_v",
                                          "grapheme_break_t",
                                          "grapheme_break_lv",
                                          "grapheme_break_lvt",
                                          "grapheme_break_zwj",
                                          "grapheme_break_extended_pictographic"};
    assert(gb >= grapheme_break_other && gb <= grapheme_break_extended_pictographic);
    if (gb < grapheme_break_other || gb > grapheme_break_extended_pictographic) fatal_error("Internal error.");
    return strings[gb];
}

enum tag_enum : int8_t {
    tag_canonical,  // Canonical form.
    tag_font,       // Font variant (for example, a blackletter form).
    tag_no_break,   // No-break version of a space or hyphen.
    tag_initial,    // Initial presentation form (Arabic).
    tag_medial,     // Medial presentation form (Arabic).
    tag_final,      // Final presentation form (Arabic).
    tag_isolated,   // Isolated presentation form (Arabic).
    tag_circle,     // Encircled form.
    tag_super,      // Superscript form.
    tag_sub,        // Subscript form.
    tag_vertical,   // Vertical layout presentation form.
    tag_wide,       // Wide (or zenkaku) compatibility character.
    tag_narrow,     // Narrow (or hankaku) compatibility character.
    tag_small,      // Small variant form (CNS compatibility).
    tag_square,     // CJK squared font variant.
    tag_fraction,   // Vulgar fraction form.
    tag_compat,     // Otherwise unspecified compatibility character.

    tag_count
};

char const* const tag_enum_strings[] = {
    nullptr, "<font>",     "<noBreak>", "<initial>", "<medial>", "<final>",  "<isolated>", "<circle>", "<super>",
    "<sub>", "<vertical>", "<wide>",    "<narrow>",  "<small>",  "<square>", "<fraction>", "<compat>",
};
static_assert(size(tag_enum_strings) == tag_count, "");
tag_enum to_tag_enum(string_segment str) {
    // Skip canonical tag, since it doesn't exist as a tag in UnicodeData.txt.
    for (size_t i = 1; i < size(tag_enum_strings); ++i) {
        if (str == tag_enum_strings[i]) return (tag_enum)i;
    }
    return (tag_enum)-1;
}

enum width_enum : int8_t {
    width_neutral,
    width_narrow,
    width_half,
    width_full,
    width_wide,
    width_ambiguous,

    width_count
};

char const* const width_enum_strings[] = {"N", "Na", "H", "F", "W", "A"};
static_assert(size(width_enum_strings) == width_count, "");
width_enum to_width_enum(string_segment str) {
    // Skip canonical tag, since it doesn't exist as a tag in UnicodeData.txt.
    for (size_t i = 1; i < size(width_enum_strings); ++i) {
        if (str == width_enum_strings[i]) return (width_enum)i;
    }
    return (width_enum)-1;
}

constexpr const uint32_t invalid_codepoint = 0xFFFFFFFFu;

struct tag_entry {
    tag_enum type = tag_count;
    uint32_t* values = nullptr;
    int32_t values_count = 0;
};

struct simple_case {
    uint32_t upper = invalid_codepoint;
    uint32_t lower = invalid_codepoint;
    uint32_t title = invalid_codepoint;
    uint32_t case_fold = invalid_codepoint;
    uint32_t turkic_case_fold = invalid_codepoint;
};

struct full_case {
    uint32_t upper[4];
    uint32_t lower[4];
    uint32_t title[4];
    uint32_t case_fold[4];
    int8_t upper_count;
    int8_t lower_count;
    int8_t title_count;
    int8_t case_fold_count;
};

struct conditional_case : full_case {
    int8_t conditions_count;
    language_enum language;
    condition_enum conditions[4];
};

struct unicode_data {
    uint32_t cp;
    category_enum category;
    bidirectional_category_enum bidirectional_category;
    grapheme_break_enum grapheme_break;
    simple_case* simple;
    full_case* full;
    conditional_case* conditionals[3];
    tag_entry* tag;
    int8_t conditionals_count;
    width_enum width;

    string_segment name;
};

struct data_entry {
    uint32_t cp;
    unicode_data* data;
};

struct extracted_mapping {
    uint32_t from;
    uint32_t to;
    string_segment name;

    int shift() const { return (int)to - (int)from; }
};

size_t get_alignment_offset(const void* ptr, size_t alignment) {
    // Alignment must be != 0 and power of two.
    assert(alignment != 0 && !(alignment & (alignment - 1)));

    size_t alignment_offset = (alignment - ((uintptr_t)ptr)) & (alignment - 1);
    assert(((uintptr_t)((const char*)ptr + alignment_offset) % alignment) == 0);
    return alignment_offset;
}

class monotonic_allocator {
    char* ptr;
    size_t sz;
    size_t cap;

   public:
    monotonic_allocator(size_t capacity) {
        ptr = new char[capacity];
        cap = capacity;
    }
    ~monotonic_allocator() { clear(); }

    void* alloc(size_t size) {
        char* result = ptr + sz;
        // Result should be aligned.
        assert(((uintptr_t)result % sizeof(max_align_t)) == 0);

        auto alignment_offset = get_alignment_offset(result + size, sizeof(max_align_t));
        if (sz + size + alignment_offset > cap) fatal_error("Out of memory.");

        sz += size + alignment_offset;
        return result;
    }

    size_t size() const { return sz; }
    size_t capacity() const { return cap; }

    void clear() {
        sz = 0;
        cap = 0;
        delete[] ptr;
    }
};

#define megabytes(x) ((x)*1024 * 1024)
static monotonic_allocator global_allocator(megabytes(100));

template <class T, class... Args>
T* monotonic_new(Args&&... args) {
    void* storage = global_allocator.alloc(sizeof(T));
    return ::new (storage) T(forward<Args>(args)...);
}

template <class T>
T* monotonic_new_array(size_t count) {
    assert(count);
    T* storage = (T*)global_allocator.alloc(sizeof(T) * count);
    auto first = ::new ((void*)storage) T();
    for (size_t i = 1; i < count; ++i) {
        ::new ((void*)&storage[i]) T();
    }
    return first;
}

const char* find_char(const char* str, char c) {
    assert(str);
    for (; *str; ++str) {
        if (*str == c) return str;
    }
    return str;
}
const char* find_char(const char* str_first, const char* str_last, char c) {
    assert(str_first);
    assert(str_last);
    const void* result = memchr(str_first, (unsigned char)c, (size_t)(str_last - str_first));
    return (result) ? (const char*)result : str_last;
}
const char* find_char2(const char* str_first, const char* str_last, char c1, char c2) {
    assert(str_first);
    assert(str_last);
    while (str_first != str_last) {
        if (*str_first == c1 || *str_first == c2) return str_first;
        ++str_first;
    }
    return str_last;
}
string_segment trim_left(string_segment segment) {
    assert(segment.first <= segment.last);
    while (segment.first < segment.last && isspace((unsigned char)(*segment.first))) ++segment.first;
    return segment;
}
string_segment trim(string_segment segment) {
    segment = trim_left(segment);
    assert(segment.first <= segment.last);
    while (segment.last != segment.first && isspace((unsigned char)(*(segment.last - 1)))) --segment.last;
    return segment;
}

string_segment extract_until_delim(string_segment* line, char delim) {
    auto current = line->first;
    auto current_end = find_char(current, line->last, delim);

    line->first = (current_end != line->last) ? current_end + 1 : current_end;
    *line = trim_left(*line);
    return trim({current, current_end});
}
string_segment extract_until_delim2(string_segment* line, char delim1, char delim2) {
    auto current = line->first;
    auto current_end = find_char2(current, line->last, delim1, delim2);

    line->first = (current_end != line->last) ? current_end + 1 : current_end;
    *line = trim_left(*line);
    return trim({current, current_end});
}

string_segment extract_next(string_segment* line) { return extract_until_delim2(line, ';', '#'); }

bool to_cp(string_segment segment, vector<char>* buffer, uint32_t* out) {
    assert(out);
    if (segment.first == segment.last || !isxdigit((unsigned char)segment.first[0])) return false;
    buffer->assign(segment.first, segment.last);
    buffer->push_back(0);
    *out = strtoul(buffer->data(), /*endptr=*/nullptr, /*base=*/16);
    return true;
}
bool get_line(const char** str, string_segment* out) {
    assert(str);
    assert(*str);
    assert(out);

    if (!**str) return false;

    auto line_start = *str;
    auto line_end = find_char(*str, '\n');
    *str = (*line_end) ? (line_end + 1) : (line_end);
    *out = {line_start, line_end};
    return true;
}

void parse_unicode_data(const char* unicode_data_contents, vector<char>* buffer, vector<data_entry>* out) {
    assert(unicode_data_contents);

    string_segment line = {};
    int32_t last_cp = -1;
    while (get_line(&unicode_data_contents, &line)) {
        // Code value.
        uint32_t cp = invalid_codepoint;
        if (!to_cp(extract_next(&line), buffer, &cp)) continue;

        if ((int32_t)cp <= last_cp) fatal_error("Unicode data is not sorted.");
        last_cp = (int32_t)cp;

        // Character name.
        auto name = extract_next(&line);
        if (name.empty()) fatal_error("Invalid codepoint name.");

        // General Category.
        auto category = to_category_enum(extract_next(&line));
        if (category < 0) fatal_error("Invalid general category.");

        // Canonical Combining Classes.
        extract_next(&line);

        // Bidirectional Category.
        auto bidirectional_category = to_bidirectional_category_enum(extract_next(&line));
        if (bidirectional_category < 0) fatal_error("Invalid bidirectional category.");

        // Character Decomposition Mapping.
        tag_entry tag;
        bool has_tag = false;

        auto tags = extract_next(&line);
        if (!tags.empty()) {
            tag_enum tag_type = tag_canonical;
            if (*tags.first == '<') {
                tag_type = to_tag_enum(extract_until_delim(&tags, ' '));
                if (tag_type < 0) fatal_error("Invalid tag.");
            }

            // Count how many values the tag has.
            int32_t values_count = 0;
            {
                auto tags_cpy = tags;
                uint32_t tag_value = invalid_codepoint;
                while (to_cp(extract_until_delim(&tags_cpy, ' '), buffer, &tag_value)) {
                    ++values_count;
                }
            }
            if (!values_count) {
                fatal_error("Tag value expected.");
            }

            auto values = monotonic_new_array<uint32_t>(values_count);
            for (int32_t i = 0; i < values_count; ++i) {
                if (!to_cp(extract_until_delim(&tags, ' '), buffer, &values[i])) {
                    fatal_error("Invalid tag value.");
                }
            }
            if (!tags.empty()) {
                fatal_error("More tags expected.");
            }

            tag = {tag_type, values, values_count};
            has_tag = true;
        }

        // Decimal digit value.
        extract_next(&line);

        // Digit value.
        extract_next(&line);

        // Numeric value.
        extract_next(&line);

        // Mirrored.
        extract_next(&line);

        // Unicode 1.0 Name.
        extract_next(&line);

        // ISO comment (deprecated, should be empty).
        extract_next(&line);

        // Uppercase Mapping.
        uint32_t upper = cp;
        to_cp(extract_next(&line), buffer, &upper);

        // Lowercase Mapping.
        uint32_t lower = cp;
        to_cp(extract_next(&line), buffer, &lower);

        // Titlecase Mapping.
        uint32_t title = cp;
        to_cp(extract_next(&line), buffer, &title);

        assert(cp != invalid_codepoint);
        auto data = monotonic_new<unicode_data>();
        data->cp = cp;
        data->category = category;
        data->bidirectional_category = bidirectional_category;
        data->name = name;

        if (upper != cp || lower != cp || title != cp) {
            data->simple = monotonic_new<simple_case>();
            data->simple->upper = upper;
            data->simple->lower = lower;
            data->simple->title = title;
        }

        if (has_tag) data->tag = monotonic_new<tag_entry>(tag);

        out->push_back({cp, data});
    }
}

size_t extract_cps(string_segment segment, vector<char>* buffer, uint32_t* out, size_t out_len) {
    size_t count = 0;
    while (!segment.empty()) {
        uint32_t cur = invalid_codepoint;
        if (!to_cp(extract_until_delim(&segment, ' '), buffer, &cur)) {
            fatal_error("Codepoint expected.");
        }

        if (count >= out_len) fatal_error("Too many codepoints.");
        out[count++] = cur;
    }
    assert(segment.empty());
    return count;
}

data_entry* find_entry(vector<data_entry>* out, uint32_t cp) {
    data_entry* entry = nullptr;
    auto it =
        lower_bound(out->begin(), out->end(), cp, [](const data_entry& entry, uint32_t cp) { return entry.cp < cp; });
    if (it != out->end() && it->cp == cp) entry = &(*it);
    return entry;
}

size_t find_or_add(vector<data_entry>* out, uint32_t cp) {
    auto it =
        lower_bound(out->begin(), out->end(), cp, [](const data_entry& entry, uint32_t cp) { return entry.cp < cp; });
    if (it == out->end() || it->cp != cp) {
        auto data = monotonic_new<unicode_data>();
        data->cp = cp;
        it = out->insert(it, 1, {cp, data});
    }
    return distance(out->begin(), it);
}

size_t find_or_add_linear(vector<data_entry>* out, uint32_t cp, size_t start) {
    assert(start <= out->size());
    auto count = out->size();
    while (start < count && out->at(start).cp < cp) {
        ++start;
    }
    if (start >= count || out->at(start).cp != cp) {
        auto data = monotonic_new<unicode_data>();
        data->cp = cp;
        out->insert(out->begin() + start, 1, {cp, data});
    }
    return start;
}

void parse_special_casing(const char* special_casing_contents, vector<char>* buffer, vector<data_entry>* out) {
    assert(special_casing_contents);

    string_segment line = {};
    while (get_line(&special_casing_contents, &line)) {
        // Code value.
        uint32_t cp = invalid_codepoint;
        if (!to_cp(extract_next(&line), buffer, &cp)) continue;

        auto it = find_entry(out, cp);
        if (!it) fatal_error("Unknown codepoint.");
        auto data = it->data;
        assert(data);

        full_case full = {};

        auto lower_count = (int8_t)extract_cps(extract_next(&line), buffer, full.lower, size(full.lower));
        auto title_count = (int8_t)extract_cps(extract_next(&line), buffer, full.title, size(full.title));
        auto upper_count = (int8_t)extract_cps(extract_next(&line), buffer, full.upper, size(full.upper));

        /* Don't bother with simple cases. */
        if (lower_count > 1) full.lower_count = lower_count;
        if (title_count > 1) full.title_count = title_count;
        if (upper_count > 1) full.upper_count = upper_count;

        auto conditions = extract_next(&line);
        if (conditions.empty() || conditions.first[0] == '#') {
            if (auto dest = data->full) {
                if (dest->lower_count > 0 || dest->title_count > 0 || dest->upper_count > 0) {
                    fatal_error("Duplicate entry found.");
                }
                dest->lower_count = full.lower_count;
                dest->title_count = full.title_count;
                dest->upper_count = full.upper_count;
                if (full.lower_count) copy(full.lower, full.lower + full.lower_count, dest->lower);
                if (full.title_count) copy(full.title, full.title + full.title_count, dest->title);
                if (full.upper_count) copy(full.upper, full.upper + full.upper_count, dest->upper);
            } else {
                data->full = monotonic_new<full_case>(full);
            }
        } else {
            auto conditional = monotonic_new<conditional_case>();
            *static_cast<full_case*>(conditional) = full;
            conditional->conditions_count = 0;
            while (!conditions.empty()) {
                auto condition_segment = extract_until_delim(&conditions, ' ');
                if (conditional->language == language_general) {
                    auto language = to_language_enum(condition_segment);
                    if (language >= 0) {
                        conditional->language = language;
                        continue;
                    }
                }

                auto condition = to_condition_enum(condition_segment);
                if (condition < 0) fatal_error("Invalid condition.");

                if (conditional->conditions_count >= (int32_t)size(conditional->conditions)) {
                    fatal_error("Too many conditions.");
                }
                conditional->conditions[conditional->conditions_count++] = condition;
            }
            if (data->conditionals_count >= (int8_t)size(data->conditionals)) {
                fatal_error("Too many conditionals.");
            }
            data->conditionals[data->conditionals_count++] = conditional;
        }
    }
}

void parse_case_folding(const char* case_folding_contents, vector<char>* buffer, vector<data_entry>* out,
                        string_segment* out_version) {
    assert(case_folding_contents);
    assert(out);
    assert(out_version);

    // Try to extract version first.
        // First line should look like "# CaseFolding-13.0.0.txt"
    if (strncmp(case_folding_contents, "# CaseFolding-", 14) == 0) {
        auto p = case_folding_contents + 14;
        while (isdigit((uint8_t)*p) || *p == '.') ++p;
        if (*(p - 1) == '.') --p;
        *out_version = {case_folding_contents + 14, p};
    }

    string_segment line = {};
    while (get_line(&case_folding_contents, &line)) {
        // Code value.
        uint32_t cp = invalid_codepoint;
        if (!to_cp(extract_next(&line), buffer, &cp)) continue;

        auto it = find_entry(out, cp);
        if (!it) fatal_error("Unknown codepoint.");
        auto data = it->data;
        assert(data);

        auto status = extract_next(&line);
        if (status == "C" || status == "S") {
            // Common or simple case folding. One to one mapping.
            if (!data->simple) data->simple = monotonic_new<simple_case>();
            if (data->simple->case_fold != invalid_codepoint) fatal_error("Duplicate case fold.");
            if (!to_cp(extract_next(&line), buffer, &data->simple->case_fold)) {
                fatal_error("Invalid case folding codepoint.");
            }
        } else if (status == "F") {
            // Full case folding. One to many mapping.
            if (!data->full) data->full = monotonic_new<full_case>();
            auto dest = data->full;
            if (dest->case_fold_count > 0) fatal_error("Duplicate case fold.");
            dest->case_fold_count =
                (int8_t)extract_cps(extract_next(&line), buffer, dest->case_fold, size(dest->case_fold));
        } else if (status == "T") {
            // Special case for turkic languages.
            if (!data->simple) data->simple = monotonic_new<simple_case>();
            if (data->simple->turkic_case_fold != invalid_codepoint) fatal_error("Duplicate case fold.");
            if (!to_cp(extract_next(&line), buffer, &data->simple->turkic_case_fold)) {
                fatal_error("Invalid case folding codepoint.");
            }
        } else {
            fatal_error("Unknown case folding status.");
        }
    }
}

struct codepoint_range {
    uint32_t first;
    uint32_t last;  // Exclusive.
};

codepoint_range extract_codepoint_range(string_segment segment, vector<char>* buffer) {
    auto cp_segment_size = segment.size();
    auto first_cp_segment = extract_until_delim(&segment, '.');
    uint32_t first_cp = invalid_codepoint;
    if (!to_cp(first_cp_segment, buffer, &first_cp)) {
        fatal_error("Codepoint expected.");
    }

    uint32_t last_cp = first_cp + 1;
    if (first_cp_segment.size() != cp_segment_size) {
        // There is a range of codepoints.
        // Skip ..
        while (segment.size() && *segment.first == '.') ++segment.first;
        if (!to_cp(segment, buffer, &last_cp)) {
            fatal_error("Codepoint expected.");
        }
        ++last_cp;  // Make range not inclusive.
    }

    return {first_cp, last_cp};
}

void parse_grapheme_break(const char* grapheme_break_contents, vector<char>* buffer, vector<data_entry>* out) {
    assert(grapheme_break_contents);

    string_segment line = {};
    while (get_line(&grapheme_break_contents, &line)) {
        auto cp_segment = extract_next(&line);
        if (cp_segment.empty()) continue;

        auto range = extract_codepoint_range(cp_segment, buffer);

        auto grapheme_break = to_grapheme_break_enum(extract_next(&line));
        if (grapheme_break < 0) fatal_error("Invalid grapheme break.");

#if 0
        // Not ignoring reserved/invalid codepoints is more robust, but comes at the cost of a couple of extra
        // blocks.
        auto category_segment = extract_until_delim(&line, ' ');
        if (category_segment == "Cn" || category_segment == "Cs") {
            // Reserved or invalid codepoint. Ignore.
            continue;
        }
#endif

        for (uint32_t cp = range.first; cp < range.last; ++cp) {
            auto index = find_or_add(out, cp);
            auto data = out->at(index).data;
            assert(data->grapheme_break == grapheme_break_other);
            data->grapheme_break = grapheme_break;
        }
    }
}

void parse_emoji_data(const char* emoji_data_contents, vector<char>* buffer, vector<data_entry>* out) {
    assert(emoji_data_contents);

    string_segment line = {};
    while (get_line(&emoji_data_contents, &line)) {
        auto cp_segment = extract_next(&line);
        if (cp_segment.empty()) continue;

        auto range = extract_codepoint_range(cp_segment, buffer);

        auto type = extract_next(&line);
        if (type != "Extended_Pictographic") continue;

        for (uint32_t cp = range.first; cp < range.last; ++cp) {
            auto index = find_or_add(out, cp);
            auto data = out->at(index).data;
            assert(data->grapheme_break == grapheme_break_other);
            data->grapheme_break = grapheme_break_extended_pictographic;
        }
    }
}

static void append_range(vector<codepoint_range>* ranges, codepoint_range range) {
    if (!ranges->empty() && ranges->back().last == range.first) {
        ranges->back().last = range.last;
    } else {
        ranges->push_back(range);
    }
}

struct parsed_data {
    vector<data_entry> data_entries;
    vector<codepoint_range> width_ranges[width_count];
    string_segment version;
};

void parse_east_asian_width(const char* east_asian_width_contents, vector<char>* buffer, parsed_data* out) {
    assert(east_asian_width_contents);

    string_segment line = {};
    while (get_line(&east_asian_width_contents, &line)) {
        auto cp_segment = extract_next(&line);
        if (cp_segment.empty()) continue;

        auto range = extract_codepoint_range(cp_segment, buffer);

        auto width = to_width_enum(extract_next(&line));
        if (width < 0) continue;

#if 0
        // Not ignoring reserved/invalid codepoints is more robust, but comes at the cost of a couple of extra
        // blocks.
        auto category_segment = extract_until_delim(&line, ' ');
        if (category_segment == "Cn" || category_segment == "Cs" || category_segment == "Co") {
            // Reserved, invalid or private use codepoint. Ignore.
            continue;
        }
#endif

        assert(width >= 0 && width < width_count);
        append_range(&out->width_ranges[width], range);

        if (width == width_neutral) continue;

        auto index = find_or_add(&out->data_entries, range.first);
        auto data = out->data_entries[index].data;
        assert(data->width == width_neutral);
        data->width = width;

        for (uint32_t cp = range.first + 1; cp < range.last; ++cp) {
            index = find_or_add_linear(&out->data_entries, cp, index);
            data = out->data_entries[index].data;
            assert(data->width == width_neutral);
            data->width = width;
        }
    }
}

parsed_data parse_all(uint32_t flags, const char* unicode_data_contents, const char* case_folding_contents,
                      const char* special_casing_contents, const char* grapheme_break_contents,
                      const char* east_asian_width_contents, const char* emoji_data_contents) {
    assert(unicode_data_contents);
    assert(special_casing_contents);
    assert(case_folding_contents);

    parsed_data result;
    vector<char> buffer;
    parse_unicode_data(unicode_data_contents, &buffer, &result.data_entries);
    parse_case_folding(case_folding_contents, &buffer, &result.data_entries, &result.version);
    parse_special_casing(special_casing_contents, &buffer, &result.data_entries);
    parse_grapheme_break(grapheme_break_contents, &buffer, &result.data_entries);
    if (flags & generate_flags_width) parse_east_asian_width(east_asian_width_contents, &buffer, &result);
    parse_emoji_data(emoji_data_contents, &buffer, &result.data_entries);

#ifdef _DEBUG
    printf("%zu bytes of monotonic space used (%.2f megabytes).\n", global_allocator.size(),
           (double)global_allocator.size() / 1024 / 1024);
#endif

    return result;
}

enum ucd_flags {
    ucd_category_control = 0,
    ucd_category_letter = 1,
    ucd_category_mark = 2,
    ucd_category_number = 3,
    ucd_category_punctuation = 4,
    ucd_category_symbol = 5,
    ucd_category_separator = 6,

    ucd_category_count = 7,
    ucd_category_shift = 0,
    ucd_category_mask = 7,

    ucd_case_caseless = 0,
    ucd_case_upper = 1,
    ucd_case_lower = 2,
    ucd_case_title = 3,

    ucd_case_count = 4,
    ucd_case_shift = 3,
    ucd_case_mask = 3,

    ucd_whitespace_flag = 64,
};

uint8_t get_ucd_case(uint8_t flags) { return (flags >> ucd_case_shift) & ucd_case_mask; }
uint8_t get_ucd_category(uint8_t flags) { return (flags >> ucd_category_shift) & ucd_category_mask; }

uint8_t to_flags(uint32_t generate_flags, category_enum category, bidirectional_category_enum bidirectional_category) {
    uint8_t ucd_category = 0;
    uint8_t ucd_case = 0;
    uint8_t ucd_whitespace = 0;

    if (generate_flags & generate_flags_category) {
        if (bidirectional_category == bidirectional_category_whitespace) {
            ucd_whitespace = ucd_whitespace_flag;
        }
        switch (category) {
            case category_uppercase_letter:
            case category_lowercase_letter:
            case category_titlecase_letter:
            case category_cased_letter:
            case category_modifier_letter:
            case category_other_letter:
            case category_letter: {
                ucd_category = ucd_category_letter;
                break;
            }

            case category_nonspacing_mark:
            case category_spacing_mark:
            case category_enclosing_mark:
            case category_mark: {
                ucd_category = ucd_category_mark;
                break;
            }

            case category_decimal_number:
            case category_letter_number:
            case category_other_number:
            case category_number: {
                ucd_category = ucd_category_number;
                break;
            }

            case category_connector_punctuation:
            case category_dash_punctuation:
            case category_open_punctuation:
            case category_close_punctuation:
            case category_initial_punctuation:
            case category_final_punctuation:
            case category_other_punctuation:
            case category_punctuation: {
                ucd_category = ucd_category_punctuation;
                break;
            }

            case category_math_symbol:
            case category_currency_symbol:
            case category_modifier_symbol:
            case category_other_symbol:
            case category_symbol: {
                ucd_category = ucd_category_symbol;
                break;
            }

            case category_space_separator:
            case category_line_separator:
            case category_paragraph_separator:
            case category_separator: {
                ucd_category = ucd_category_separator;
                break;
            }

            case category_control:
            case category_control_format:
            case category_control_surrogate:
            case category_control_private_use:
            case category_control_unassigned:
            default: {
                ucd_category = ucd_category_control;
                break;
            }
        }
    }

    if (generate_flags & generate_flags_case_info) {
        switch (category) {
            case category_uppercase_letter: {
                ucd_case = ucd_case_upper;
                break;
            }
            case category_lowercase_letter: {
                ucd_case = ucd_case_lower;
                break;
            }
            case category_titlecase_letter: {
                ucd_case = ucd_case_title;
                break;
            }
            default: {
                ucd_case = ucd_case_caseless;
                break;
            }
        }
    }

    uint8_t ucd_shifted_category = ((ucd_category & ucd_category_mask) << ucd_category_shift);
    uint8_t ucd_shifted_case = (ucd_case & ucd_case_mask) << ucd_case_shift;
    uint8_t result = ucd_shifted_category | ucd_shifted_case | ucd_whitespace;
    assert(get_ucd_category(result) == ucd_category);
    assert(get_ucd_case(result) == ucd_case);
    assert((result & ucd_whitespace_flag) == ucd_whitespace);
    return result;
}

enum gbw_bits : uint8_t {
    grapheme_break_mask = 0xF,
    grapheme_break_shift = 0,

    width_mask = 7,
    width_shift = 4,
};

grapheme_break_enum get_grapheme_break_from_bits(uint8_t bits) {
    return (grapheme_break_enum)((bits >> grapheme_break_shift) & grapheme_break_mask);
}
int8_t get_width_from_bits(uint8_t bits) { return (int8_t)((bits >> width_shift) & width_mask); }

uint8_t to_grapheme_break_and_width_bits(uint32_t flags, grapheme_break_enum grapheme_break, int8_t width) {
    static_assert(grapheme_break_count <= 16, "");

    uint8_t result = 0;
    if (flags & generate_flags_grapheme_break) {
        result |= ((uint8_t)grapheme_break & grapheme_break_mask) << grapheme_break_shift;
    }

    if (flags & generate_flags_width) {
        result |= ((uint8_t)width & width_mask) << width_shift;
    }

    assert(get_grapheme_break_from_bits(result) == grapheme_break);
    assert(get_width_from_bits(result) == width);
    return result;
}

struct ucd_entry {
    struct data_t {
        uint8_t flags;
        grapheme_break_enum grapheme_break = grapheme_break_other;
        int8_t width;

        int32_t canonical_index;
        int32_t simple_canonical_offset;
        int32_t compatibility_index;
        int32_t simple_compatibility_offset;
        int32_t full_upper_index;
        int32_t full_title_index;
        int32_t full_lower_index;
        int32_t full_case_toggle_index;
        int32_t full_case_fold_index;
        int32_t simple_upper_offset;
        int32_t simple_title_offset;
        int32_t simple_lower_offset;
        int32_t simple_case_toggle_offset;
        int32_t simple_case_fold_offset;
    } data;

    uint8_t internal_case_flags;
    int32_t references;

    bool equals(const ucd_entry& other) const { return memcmp(&data, &other.data, sizeof(data_t)) == 0; }
};

struct codepoint_run {
    uint32_t* codepoints;
    int32_t count;

    int32_t references;

    bool equals(const uint32_t* other, int32_t other_count) const {
        return other_count == count && (count == 0 || memcmp(other, codepoints, count * sizeof(uint32_t)) == 0);
    }
};

struct pruned_block_range {
    int32_t first;
    int32_t last;
    int32_t value;

    int32_t length() const { return last - first; }
};

struct pruned_block_ranges {
    pruned_block_range entries[5];
    int32_t count = 0;
};

struct ucd_block {
    int32_t offset[1 << 8];

    int32_t references = 0;
    int32_t redirect_index = -1;

    pruned_block_ranges ranges;

    ucd_block() { fill(offset, offset + size(offset), -1); }
    bool equals(const ucd_block& other) const { return memcmp(offset, other.offset, sizeof(offset)) == 0; }
};

struct ucd_min_sizes {
    struct pair {
        bool is_signed;
        int32_t size;
    };

    pair canonical_index;
    pair simple_canonical_offset;
    pair compatibility_index;
    pair simple_compatibility_offset;
    pair full_upper_index;
    pair full_title_index;
    pair full_lower_index;
    pair full_case_toggle_index;
    pair full_case_fold_index;
    pair simple_upper_offset;
    pair simple_title_offset;
    pair simple_lower_offset;
    pair simple_case_toggle_offset;
    pair simple_case_fold_offset;

    pair stage_one_table;
    pair stage_one_function;
    pair stage_two;

    pair full_upper_array;
    pair full_title_array;
    pair full_lower_array;
    pair full_case_fold_array;
    pair canonical_array;
    pair compatibility_array;

    pair all_arrays;

    int32_t ucd_entry_size(uint32_t flags) const {
        struct tracking {
            int32_t size;
            int32_t prev_alignment;
        };
        auto add_size = [](tracking* t, int32_t next_size) {
            if (next_size == 0) return;
            if (t->size && t->prev_alignment != next_size) {
                t->size += next_size - (t->size % next_size);
            }
            t->size += next_size;
            t->prev_alignment = next_size;
        };
        tracking t = {0, 1};
        if ((flags & generate_flags_category) || (flags & generate_flags_case_info)) {
            add_size(&t, 1);
        }
        if ((flags & generate_flags_grapheme_break) || (flags & generate_flags_width)) {
            add_size(&t, 1);
        }
        if (flags & generate_flags_grapheme_break) {
            add_size(&t, 1);
        }
        if (flags & generate_flags_full_case) {
            add_size(&t, full_upper_index.size / 8);
            add_size(&t, full_title_index.size / 8);
            add_size(&t, full_lower_index.size / 8);
        }
        if (flags & generate_flags_full_case_toggle) {
            add_size(&t, full_case_toggle_index.size / 8);
        }
        if (flags & generate_flags_full_case_fold) {
            add_size(&t, full_case_fold_index.size / 8);
        }
        if (flags & generate_flags_canonical) {
            add_size(&t, canonical_index.size / 8);
        }
        if (flags & generate_flags_compatibility) {
            add_size(&t, compatibility_index.size / 8);
        }

        if (flags & generate_flags_simple_case) {
            add_size(&t, simple_upper_offset.size / 8);
            add_size(&t, simple_title_offset.size / 8);
            add_size(&t, simple_lower_offset.size / 8);
        }
        if (flags & generate_flags_simple_case_toggle) {
            add_size(&t, simple_case_toggle_offset.size / 8);
        }
        if (flags & generate_flags_simple_case_fold) {
            add_size(&t, simple_case_fold_offset.size / 8);
        }
        if (flags & generate_flags_canonical) {
            add_size(&t, simple_canonical_offset.size / 8);
        }
        if (flags & generate_flags_compatibility) {
            add_size(&t, simple_compatibility_offset.size / 8);
        }
        return t.size;
    }
};

struct unique_ucd {
    vector<codepoint_run> full_upper = vector<codepoint_run>(1);
    vector<codepoint_run> full_title = vector<codepoint_run>(1);
    vector<codepoint_run> full_lower = vector<codepoint_run>(1);
    vector<codepoint_run> full_case_fold = vector<codepoint_run>(1);
    vector<codepoint_run> canonical = vector<codepoint_run>(1);
    vector<codepoint_run> compatibility = vector<codepoint_run>(1);

    vector<ucd_entry> entries = vector<ucd_entry>(1);

    ucd_min_sizes min_sizes;
    uint32_t flags = generate_flags_default;

    vector<int32_t> stage_one;
    vector<ucd_block> stage_two = vector<ucd_block>(1);
    size_t pruned_stage_one_size = 0;
    size_t pruned_stage_two_start = 0;

    unique_ucd() {
        // Create default empty block.
        fill(stage_two.back().offset, stage_two.back().offset + size(stage_two.back().offset), 0);
    }

    static int32_t add_unique_run(vector<codepoint_run>* runs, const uint32_t* codepoints, int32_t count) {
        if (!count) return 0;
        int32_t prev_codepoints = 0;
        for (int32_t i = 0, runs_count = (int32_t)runs->size(); i < runs_count; ++i) {
            auto current = &runs->at(i);
            if (current->equals(codepoints, count)) {
                ++current->references;
                return prev_codepoints;
            }
            prev_codepoints += current->count + 1;
        }
        uint32_t* added = monotonic_new_array<uint32_t>(count);
        copy(codepoints, codepoints + count, added);
        runs->push_back({added, count, 1});
        return prev_codepoints;
    }
    static int32_t add_unique_ucd_entry(vector<ucd_entry>* entries, const ucd_entry& entry) {
        for (int32_t i = 0, entries_count = (int32_t)entries->size(); i < entries_count; ++i) {
            auto current = &entries->at(i);
            if (current->equals(entry)) {
                ++current->references;
                return i;
            }
        }
        entries->push_back(entry);
        return (int32_t)(entries->size() - 1);
    }

    int32_t add_full_upper_run(const uint32_t* codepoints, int32_t count) {
        return add_unique_run(&full_upper, codepoints, count);
    }
    int32_t add_full_title_run(const uint32_t* codepoints, int32_t count) {
        return add_unique_run(&full_title, codepoints, count);
    }
    int32_t add_full_lower_run(const uint32_t* codepoints, int32_t count) {
        return add_unique_run(&full_lower, codepoints, count);
    }
    int32_t add_full_case_fold_run(const uint32_t* codepoints, int32_t count) {
        return add_unique_run(&full_case_fold, codepoints, count);
    }
    int32_t add_canonical_run(const uint32_t* codepoints, int32_t count) {
        return add_unique_run(&canonical, codepoints, count);
    }
    int32_t add_compatibility_run(const uint32_t* codepoints, int32_t count) {
        return add_unique_run(&compatibility, codepoints, count);
    }
    int32_t add_ucd_entry(const ucd_entry& entry) { return add_unique_ucd_entry(&entries, entry); }

    static void add_to_multistage_table_impl(vector<int32_t>* stage_one, vector<ucd_block>* stage_two,
                                             uint32_t codepoint, int32_t entry_index, int32_t stage_two_block_size) {
        auto stage_one_index = codepoint / stage_two_block_size;
        auto stage_two_index = codepoint % stage_two_block_size;
        if (stage_one_index >= stage_one->size()) {
            stage_one->resize(stage_one_index + 1, -1);
        }
        if (stage_one->at(stage_one_index) < 0) {
            stage_two->push_back({});
            stage_one->at(stage_one_index) = (int32_t)(stage_two->size() - 1);
        }
        auto index = stage_one->at(stage_one_index);
        assert(stage_two->at(index).offset[stage_two_index] < 0);
        stage_two->at(index).offset[stage_two_index] = entry_index;
    }

    void add_to_multistage_table(uint32_t codepoint, int32_t entry_index) {
        add_to_multistage_table_impl(&stage_one, &stage_two, codepoint, entry_index, block_size);
    }

    void add_ucd_entry_to_multistage_tables(uint32_t cp, const ucd_entry& ucd) {
        add_to_multistage_table(cp, add_ucd_entry(ucd));
    }

    static void prune_duplicate_blocks_impl(vector<int32_t>& stage_one, vector<ucd_block>& stage_two) {
        const int32_t empty_block_index = 0;

        vector<ucd_block> pruned_stage_two;

        // Normalize all blocks, bind unallocated entries (<0) to default entry.
        for (auto& block : stage_two) {
            block.references = 0;
            for (auto& entry : block.offset) {
                if (entry < 0) entry = 0;
            }
        }

        // Find duplicate blocks and mark them obsolete.
        for (int32_t i = 0, count = (int32_t)stage_two.size(); i < count; i++) {
            auto& current = stage_two[i];
            for (int32_t j = i + 1; j < count; j++) {
                auto& other = stage_two[j];
                if (&other == &current) continue;
                if (other.redirect_index >= 0) continue;
                if (other.equals(current)) {
                    other.redirect_index = i;
                }
            }
        }

        // Rebind stage_one table to redirected blocks and count references.
        for (int32_t i = 0, count = (int32_t)stage_one.size(); i < count; i++) {
            auto& stage_one_index = stage_one[i];
            if (stage_one_index < 0) {
                stage_one_index = empty_block_index;
                stage_two[empty_block_index].references++;
                continue;
            }
            auto& block = stage_two[stage_one_index];
            if (block.redirect_index >= 0) {
                stage_one_index = block.redirect_index;
                assert(stage_two[stage_one_index].redirect_index < 0);
            }
            stage_two[stage_one_index].references++;
        }

        // Add referenced blocks to new pruned stage_two, updating their references.
        for (int32_t i = 0, count = (int32_t)stage_two.size(); i < count; i++) {
            auto& current = stage_two[i];
            if (current.references > 0) {
                assert(current.redirect_index < 0);
                pruned_stage_two.push_back(current);
                current.redirect_index = (int32_t)(pruned_stage_two.size() - 1);
            }
        }

        // Rebind stage_one again, this time to new pruned stage_two.
        for (int32_t i = 0, count = (int32_t)stage_one.size(); i < count; i++) {
            auto& stage_one_index = stage_one[i];
            auto& block = stage_two[stage_one_index];
            assert(block.redirect_index >= 0);
            stage_one_index = block.redirect_index;
        }

        // Move pruned vector into original vector.
        stage_two = move(pruned_stage_two);
    }
    void prune_duplicate_blocks() {
        prune_duplicate_blocks_impl(stage_one, stage_two);

        int32_t default_block_pos = 0;
        if (flags & generate_flags_prune_stage_two) {
            pruned_stage_two_start = prune_trivial_blocks(stage_one, stage_two);
            default_block_pos = (int32_t)pruned_stage_two_start;
        }

        if (flags & generate_flags_prune_stage_one) {
            pruned_stage_one_size = pruned_size;
            for (size_t i = pruned_stage_one_size - 1; i > 0; i--) {
                if (stage_one[i] == default_block_pos) {
                    --pruned_stage_one_size;
                } else {
                    break;
                }
            }
        }
    }

    static size_t prune_trivial_blocks(vector<int32_t>& stage_one, vector<ucd_block>& blocks) {
        int32_t current = 0;

        struct extractor {
            int32_t* p;
            int32_t* first;
            int32_t* last;

            extractor(int32_t* first, int32_t* last) : p(first), first(first), last(last) {}

            bool next_range(pruned_block_range* out) {
                if (p == last) return false;

                *out = {(int32_t)(p - first)};
                auto cmp = *p++;
                while (p < last && *p == cmp) ++p;
                out->last = (int32_t)(p - first);
                out->value = cmp;
                return true;
            }
            bool completed() const { return p == last; }
        };

        for (auto& block : blocks) {
            block.redirect_index = current;

            auto ex = extractor{block.offset, block.offset + block_size};
            pruned_block_ranges ranges;

            while (ranges.count < (int32_t)size(ranges.entries) && ex.next_range(&ranges.entries[ranges.count])) {
                ranges.count++;
            }
            if (!ex.completed()) {
                ++current;
                continue;
            }

            switch (ranges.count) {
                case 1:
                case 2: {
                    block.ranges = ranges;
                    break;
                }
                case 3: {
                    if (ranges.entries[0].value == ranges.entries[2].value) {
                        block.ranges = ranges;
                    } else {
                        ++current;
                    }
                    break;
                }
                case 4: {
                    bool same_alternating_cmp0 = ranges.entries[0].value == ranges.entries[2].value;
                    bool same_alternating_cmp1 = ranges.entries[1].value == ranges.entries[3].value;
                    bool is_trivial_length0 = ranges.entries[0].length() == 1 && ranges.entries[2].length() == 1;
                    bool is_trivial_length1 = ranges.entries[1].length() == 1 && ranges.entries[3].length() == 1;
                    if (same_alternating_cmp0 && same_alternating_cmp1 && (is_trivial_length0 || is_trivial_length1)) {
                        block.ranges = ranges;
                    } else {
                        ++current;
                    }
                    break;
                }
                case 5: {
                    bool same_alternating_cmp0 = ranges.entries[0].value == ranges.entries[2].value &&
                                                 ranges.entries[0].value == ranges.entries[4].value;
                    bool same_alternating_cmp1 = ranges.entries[1].value == ranges.entries[3].value;
                    bool is_trivial_length = ranges.entries[1].length() == 1 && ranges.entries[3].length() == 1;
                    if (same_alternating_cmp0 && same_alternating_cmp1 && is_trivial_length) {
                        block.ranges = ranges;
                    } else {
                        ++current;
                    }
                    break;
                }
                default: {
                    ++current;
                    break;
                }
            }
        }

        size_t nontrivial_start = (size_t)current;

        // Trivial blocks are indexed after non trivial blocks.
        for (auto& block : blocks) {
            if (block.ranges.count) block.redirect_index = current++;
        }

        // Rebind stage_one.
        for (int32_t i = 0, count = (int32_t)stage_one.size(); i < count; i++) {
            auto& stage_one_index = stage_one[i];
            auto& block = blocks[stage_one_index];
            assert(block.redirect_index >= 0);
            stage_one_index = block.redirect_index;
        }

        auto rearranged = vector<ucd_block>(blocks.size());
        for (auto& block : blocks) {
            rearranged[block.redirect_index] = block;
        }
        blocks = move(rearranged);

        return nontrivial_start;
    }

    void determine_min_sizes() {
        struct pair {
            int32_t min;
            int32_t max;
        };
        pair canonical_index = {0, 0};
        pair simple_canonical_offset = {0, 0};
        pair compatibility_index = {0, 0};
        pair simple_compatibility_offset = {0, 0};
        pair full_upper_index = {0, 0};
        pair full_title_index = {0, 0};
        pair full_lower_index = {0, 0};
        pair full_case_toggle_index = {0, 0};
        pair full_case_fold_index = {0, 0};
        pair simple_upper_offset = {0, 0};
        pair simple_title_offset = {0, 0};
        pair simple_lower_offset = {0, 0};
        pair simple_case_toggle_offset = {0, 0};
        pair simple_case_fold_offset = {0, 0};

        // Handle case toggling seperately, since it encompasses two lists at once.
        for (auto& run : full_upper) {
            full_case_toggle_index.max += run.count + 1; // Plus nullterminator.
        }
        for (auto& run : full_lower) {
            full_case_toggle_index.max += run.count + 1; // Plus nullterminator.
        }

        // Unicoda data sizes.
        for (auto& entry : entries) {
            canonical_index.min = min(entry.data.canonical_index, canonical_index.min);
            simple_canonical_offset.min = min(entry.data.simple_canonical_offset, simple_canonical_offset.min);
            compatibility_index.min = min(entry.data.compatibility_index, compatibility_index.min);
            simple_compatibility_offset.min =
                min(entry.data.simple_compatibility_offset, simple_compatibility_offset.min);
            full_upper_index.min = min(entry.data.full_upper_index, full_upper_index.min);
            full_title_index.min = min(entry.data.full_title_index, full_title_index.min);
            full_lower_index.min = min(entry.data.full_lower_index, full_lower_index.min);
            full_case_fold_index.min = min(entry.data.full_case_fold_index, full_case_fold_index.min);
            simple_upper_offset.min = min(entry.data.simple_upper_offset, simple_upper_offset.min);
            simple_title_offset.min = min(entry.data.simple_title_offset, simple_title_offset.min);
            simple_lower_offset.min = min(entry.data.simple_lower_offset, simple_lower_offset.min);
            simple_case_toggle_offset.min = min(entry.data.simple_case_toggle_offset, simple_case_toggle_offset.min);
            simple_case_fold_offset.min = min(entry.data.simple_case_fold_offset, simple_case_fold_offset.min);

            canonical_index.max = max(entry.data.canonical_index, canonical_index.max);
            simple_canonical_offset.max = max(entry.data.simple_canonical_offset, simple_canonical_offset.max);
            compatibility_index.max = max(entry.data.compatibility_index, compatibility_index.max);
            simple_compatibility_offset.max =
                max(entry.data.simple_compatibility_offset, simple_compatibility_offset.max);
            full_upper_index.max = max(entry.data.full_upper_index, full_upper_index.max);
            full_title_index.max = max(entry.data.full_title_index, full_title_index.max);
            full_lower_index.max = max(entry.data.full_lower_index, full_lower_index.max);
            full_case_fold_index.max = max(entry.data.full_case_fold_index, full_case_fold_index.max);
            simple_upper_offset.max = max(entry.data.simple_upper_offset, simple_upper_offset.max);
            simple_title_offset.max = max(entry.data.simple_title_offset, simple_title_offset.max);
            simple_lower_offset.max = max(entry.data.simple_lower_offset, simple_lower_offset.max);
            simple_case_toggle_offset.max = max(entry.data.simple_case_toggle_offset, simple_case_toggle_offset.max);
            simple_case_fold_offset.max = max(entry.data.simple_case_fold_offset, simple_case_fold_offset.max);
        }

        // Staged tables sizes.
        auto get_staged_tables_sizes = [](const vector<int32_t>& stage_one, size_t stage_one_count,
                                          const vector<ucd_block>& stage_two, pair* stage_one_table_size,
                                          pair* stage_one_function_size, pair* stage_two_size) {
            *stage_one_table_size = {0, 0};
            *stage_one_function_size = {0, 0};
            *stage_two_size = {0, 0};
            for (size_t i = 0; i < stage_one_count; ++i) {
                auto entry = stage_one[i];
                stage_one_table_size->min = min(stage_one_table_size->min, entry);
                stage_one_table_size->max = max(stage_one_table_size->max, entry);
            }

            for (auto& entry : stage_one) {
                stage_one_function_size->min = min(stage_one_function_size->min, entry);
                stage_one_function_size->max = max(stage_one_function_size->max, entry);
            }

            for (auto& block : stage_two) {
                for (auto& entry : block.offset) {
                    stage_two_size->min = min(stage_two_size->min, entry);
                    stage_two_size->max = max(stage_two_size->max, entry);
                }
            }
        };
        pair stage_one_table_size = {0, 0};
        pair stage_one_function_size = {0, 0};
        pair stage_two_size = {0, 0};
        get_staged_tables_sizes(stage_one,
                                (flags & generate_flags_prune_stage_one) ? pruned_stage_one_size : stage_one.size(),
                                stage_two, &stage_one_table_size, &stage_one_function_size, &stage_two_size);

        // Arrays.
        auto get_codepoint_run_sizes = [](const vector<codepoint_run>& run) -> pair {
            pair result = {0, 0};
            for (auto& entry : run) {
                for (int32_t i = 0, count = entry.count; i < count; i++) {
                    result.min = min(result.min, (int32_t)entry.codepoints[i]);
                    result.max = max(result.max, (int32_t)entry.codepoints[i]);
                }
            }
            return result;
        };

        auto full_upper_pair = get_codepoint_run_sizes(full_upper);
        auto full_title_pair = get_codepoint_run_sizes(full_title);
        auto full_lower_pair = get_codepoint_run_sizes(full_lower);
        auto full_case_fold_pair = get_codepoint_run_sizes(full_case_fold);
        auto canonical_pair = get_codepoint_run_sizes(canonical);
        auto compatibility_pair = get_codepoint_run_sizes(compatibility);

        auto value_to_size = [](pair value) -> ucd_min_sizes::pair {
            struct range {
                int64_t min;
                int64_t max;
                int32_t size;
            };
            static const range ranges[] = {
                // Unsigned ranges.
                {0, UINT8_MAX, 8},
                {0, UINT16_MAX, 16},
                {0, UINT32_MAX, 32},

                // Signed ranges.
                {INT8_MIN, INT8_MAX, 8},
                {INT16_MIN, INT16_MAX, 16},
                {INT32_MIN, INT32_MAX, 32},
            };

            if (value.min == value.max) return {false, 0};

            for (auto& entry : ranges) {
                if ((int64_t)value.min >= entry.min && (int64_t)value.max <= entry.max) {
                    return {value.min < 0, entry.size};
                }
            }
            fatal_error("Couldn't determine min sizes for Unicode data entries.");
            return {false, -1};
        };

        min_sizes.canonical_index = value_to_size(canonical_index);
        min_sizes.simple_canonical_offset = value_to_size(simple_canonical_offset);
        min_sizes.compatibility_index = value_to_size(compatibility_index);
        min_sizes.simple_compatibility_offset = value_to_size(simple_compatibility_offset);
        min_sizes.full_upper_index = value_to_size(full_upper_index);
        min_sizes.full_title_index = value_to_size(full_title_index);
        min_sizes.full_lower_index = value_to_size(full_lower_index);
        min_sizes.full_case_toggle_index = value_to_size(full_case_toggle_index);
        min_sizes.full_case_fold_index = value_to_size(full_case_fold_index);
        min_sizes.simple_upper_offset = value_to_size(simple_upper_offset);
        min_sizes.simple_title_offset = value_to_size(simple_title_offset);
        min_sizes.simple_lower_offset = value_to_size(simple_lower_offset);
        min_sizes.simple_case_toggle_offset = value_to_size(simple_case_toggle_offset);
        min_sizes.simple_case_fold_offset = value_to_size(simple_case_fold_offset);

        min_sizes.stage_one_table = value_to_size(stage_one_table_size);
        min_sizes.stage_one_function = value_to_size(stage_one_function_size);
        min_sizes.stage_two = value_to_size(stage_two_size);

        min_sizes.full_upper_array = value_to_size(full_upper_pair);
        min_sizes.full_title_array = value_to_size(full_title_pair);
        min_sizes.full_lower_array = value_to_size(full_lower_pair);
        min_sizes.full_case_fold_array = value_to_size(full_case_fold_pair);
        min_sizes.canonical_array = value_to_size(canonical_pair);
        min_sizes.compatibility_array = value_to_size(compatibility_pair);

        min_sizes.all_arrays.size = max({min_sizes.full_upper_array.size, min_sizes.full_title_array.size,
                                         min_sizes.full_lower_array.size, min_sizes.full_case_fold_array.size,
                                         min_sizes.canonical_array.size, min_sizes.compatibility_array.size});
        min_sizes.all_arrays.is_signed = false;

#ifdef _DEBUG
        FPRINTF(stdout, TEXT("Determined min sizes:\n"));
        FPRINTF(stdout, TEXT("canonical_index needs %d bits.\n"), min_sizes.canonical_index.size);
        FPRINTF(stdout, TEXT("simple_canonical_offset needs %d bits.\n"), min_sizes.simple_canonical_offset.size);
        FPRINTF(stdout, TEXT("compatibility_index needs %d bits.\n"), min_sizes.compatibility_index.size);
        FPRINTF(stdout, TEXT("simple_compatibility_offset needs %d bits.\n"),
                min_sizes.simple_compatibility_offset.size);
        FPRINTF(stdout, TEXT("full_upper_index needs %d bits.\n"), min_sizes.full_upper_index.size);
        FPRINTF(stdout, TEXT("full_title_index needs %d bits.\n"), min_sizes.full_title_index.size);
        FPRINTF(stdout, TEXT("full_lower_index needs %d bits.\n"), min_sizes.full_lower_index.size);
        FPRINTF(stdout, TEXT("full_case_toggle_index needs %d bits.\n"), min_sizes.full_case_toggle_index.size);
        FPRINTF(stdout, TEXT("full_case_fold_index needs %d bits.\n"), min_sizes.full_case_fold_index.size);
        FPRINTF(stdout, TEXT("simple_upper_offset needs %d bits.\n"), min_sizes.simple_upper_offset.size);
        FPRINTF(stdout, TEXT("simple_title_offset needs %d bits.\n"), min_sizes.simple_title_offset.size);
        FPRINTF(stdout, TEXT("simple_lower_offset needs %d bits.\n"), min_sizes.simple_lower_offset.size);
        FPRINTF(stdout, TEXT("simple_case_toggle_offset needs %d bits.\n"), min_sizes.simple_case_toggle_offset.size);
        FPRINTF(stdout, TEXT("simple_case_fold_offset needs %d bits.\n\n"), min_sizes.simple_case_fold_offset.size);

        FPRINTF(stdout, TEXT("stage_one needs %d bits.\n"), min_sizes.stage_one.size);
        FPRINTF(stdout, TEXT("stage_two needs %d bits.\n\n"), min_sizes.stage_two.size);
#endif
    }
};

const ucd_entry& get_ucd_entry(const unique_ucd& ucd, uint32_t cp) {
    auto stage_one_index = cp / block_size;
    auto stage_two_index = cp % block_size;

    if (stage_one_index >= ucd.stage_one.size()) return ucd.entries[0];
    auto entry_index = ucd.stage_two[ucd.stage_one[stage_one_index]].offset[stage_two_index];
    if (entry_index < 0) return ucd.entries[0];
    return ucd.entries[entry_index];
}

void generate_tables(const vector<data_entry> data_entries, unique_ucd* out) {
    struct cp_entry_pair {
        uint32_t cp;
        ucd_entry entry;
    };

    auto flags = out->flags;
    vector<cp_entry_pair> pairs;

    for (auto& entry : data_entries) {
        auto data = entry.data;
        auto cp = (int32_t)data->cp;
        assert(cp >= 0 && cp <= 0x10ffff);

        ucd_entry ucd = {};

        auto case_flags = to_flags(0xFFFFFFFFu, data->category, data->bidirectional_category);

        ucd.internal_case_flags = case_flags;
        ucd.data.flags = to_flags(flags, data->category, data->bidirectional_category);
        ucd.data.grapheme_break = (flags & generate_flags_grapheme_break) ? data->grapheme_break : grapheme_break_other;
        ucd.data.width = 0;
        if (flags & generate_flags_width) {
            ucd.data.width = 1;
            if (data->grapheme_break == grapheme_break_extend || data->grapheme_break == grapheme_break_prepend ||
                data->grapheme_break == grapheme_break_zwj || data->grapheme_break == grapheme_break_control ||
                data->grapheme_break == grapheme_break_cr || data->grapheme_break == grapheme_break_lf) {
                ucd.data.width = 0;
            } else if (data->width == width_full || data->width == width_wide) {
                ucd.data.width = 2;
            }
        }

        if ((flags & generate_flags_canonical) || (flags & generate_flags_compatibility)) {
            if (auto tag = data->tag) {
                if (tag->type == tag_canonical) {
                    if (flags & generate_flags_canonical) {
                        if (tag->values_count) {
                            ucd.data.simple_canonical_offset = (int32_t)tag->values[0] - cp;
                        } else {
                            ucd.data.canonical_index = out->add_canonical_run(tag->values, tag->values_count);
                        }
                    }
                } else {
                    if (flags & generate_flags_compatibility) {
                        if (tag->values_count) {
                            ucd.data.simple_compatibility_offset = (int32_t)tag->values[0] - cp;
                        } else {
                            ucd.data.compatibility_index = out->add_canonical_run(tag->values, tag->values_count);
                        }
                    }
                }
            }
        }

        if (auto f = data->full) {
            if (flags & generate_flags_full_case) {
                ucd.data.full_upper_index = out->add_full_upper_run(f->upper, f->upper_count);
                ucd.data.full_title_index = out->add_full_title_run(f->title, f->title_count);
                ucd.data.full_lower_index = out->add_full_lower_run(f->lower, f->lower_count);
            }

            if (flags & generate_flags_full_case_toggle) {
                if (get_ucd_case(case_flags) == ucd_case_lower) {
                    ucd.data.full_case_toggle_index = out->add_full_upper_run(f->upper, f->upper_count);
                } else if (get_ucd_case(case_flags) == ucd_case_upper) {
                    // Make index negative, so that it maps to a different entry in the multistage tables than the upper
                    // case toggle.
                    ucd.data.full_case_toggle_index = -out->add_full_lower_run(f->lower, f->lower_count);
                }
            }

            if (flags & generate_flags_full_case_fold) {
                ucd.data.full_case_fold_index = out->add_full_case_fold_run(f->case_fold, f->case_fold_count);
            }
        }

        if (auto simple = data->simple) {
            if (flags & generate_flags_simple_case) {
                if (simple->upper != invalid_codepoint) ucd.data.simple_upper_offset = (int32_t)simple->upper - cp;
                if (simple->title != invalid_codepoint) ucd.data.simple_title_offset = (int32_t)simple->title - cp;
                if (simple->lower != invalid_codepoint) ucd.data.simple_lower_offset = (int32_t)simple->lower - cp;
            }

            if (flags & generate_flags_simple_case_toggle) {
                if (get_ucd_case(case_flags) == ucd_case_lower) {
                    if (simple->upper != invalid_codepoint) {
                        ucd.data.simple_case_toggle_offset = (int32_t)simple->upper - cp;
                    }
                } else if (get_ucd_case(case_flags) == ucd_case_upper) {
                    if (simple->lower != invalid_codepoint) {
                        ucd.data.simple_case_toggle_offset = (int32_t)simple->lower - cp;
                    }
                }
            }

            if (flags & generate_flags_simple_case_fold) {
                if (simple->case_fold != invalid_codepoint) {
                    ucd.data.simple_case_fold_offset = (int32_t)simple->case_fold - cp;
                }
            }
        }

        out->add_ucd_entry_to_multistage_tables(cp, ucd);
        pairs.push_back({data->cp, ucd});
    }

    out->prune_duplicate_blocks();
    out->determine_min_sizes();

#ifdef _DEBUG
    auto ucd_entry_size = out->min_sizes.ucd_entry_size(flags);
    {
        size_t stage_one_size = out->stage_one.size();
        if (flags & generate_flags_prune_stage_one) stage_one_size = out->pruned_stage_one_size;
        size_t stage_two_size =
            (flags & generate_flags_prune_stage_two) ? out->pruned_stage_two_start : out->stage_two.size();
        size_t overall_size = out->entries.size() * ucd_entry_size +
                              stage_one_size * (out->min_sizes.stage_one.size / 8) +
                              stage_two_size * block_size * (out->min_sizes.stage_two.size / 8);
        double overall_size_kilobytes = overall_size / 1024.0;
        FPRINTF(stdout,
                TEXT("There are %zu ucd_entries.\nStage one table has %zu entries.\nStage two table has %zu blocks.\n"),
                out->entries.size(), stage_one_size, out->stage_two.size());
        FPRINTF(stdout, TEXT("Overall size usage is %zu bytes (%.2f kilobytes).\n"), overall_size,
                overall_size_kilobytes);
        if (flags & generate_flags_prune_stage_one) {
            size_t unpruned_size = out->entries.size() * ucd_entry_size +
                                   out->stage_one.size() * (out->min_sizes.stage_one.size / 8) +
                                   out->stage_two.size() * block_size * (out->min_sizes.stage_two.size / 8);
            double unpruned_overall_size_kilobytes = unpruned_size / 1024.0;
            FPRINTF(stdout, TEXT("(Unpruned it would be %zu bytes (%.2f kilobytes).\n"), unpruned_size,
                    unpruned_overall_size_kilobytes);
        }
        FPRINTF(stdout, TEXT("\n"));
    }

    FPRINTF(stdout, TEXT("Testing validity of ucd entries.\n"));
    for (auto& pair : pairs) {
        if (!get_ucd_entry(*out, pair.cp).equals(pair.entry)) {
            fatal_error("Ucd entry generator is wrong.");
        }
    }
    FPRINTF(stdout, TEXT("Ucd entries are all valid for all codepoints.\n"));
#endif
}

void write(vector<char>* out, const char* fmt, ...) {
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    auto needed = vsnprintf(nullptr, 0, fmt, args1);
    va_end(args1);

    if (needed <= 0) {
        FPRINTF(stderr, TEXT("Unable to write to output.\n"));
        exit(-1);
    }

    size_t start = out->size();
    out->resize(out->size() + needed + 1);

    auto written = vsnprintf(out->data() + start, out->size() - start, fmt, args2);

    if (written <= 0 || written > needed + 1) {
        FPRINTF(stderr, TEXT("Unable to write to output.\n"));
        exit(-1);
    }
    va_end(args2);

    out->pop_back();  // Remove null terminator.
}

#define grapheme_no_break 128u
#define grapheme_mask 127u
#define grapheme_entries 16
uint8_t grapheme_break_transitions[grapheme_entries][grapheme_entries];
void init_grapheme_break_transitions() {
    // Initialize transitions from X to X.
    for (auto from = 0; from < grapheme_entries; ++from) {
        for (auto to = 0; to < grapheme_entries; ++to) {
            grapheme_break_transitions[from][to] = (uint8_t)to;
        }
    }

    // Add transitions, where breaking isn't allowed.

    // Transitions to extend or spacing mark aren't allowed except for cr, lf and control.
    grapheme_break_transitions[grapheme_break_other][grapheme_break_extend] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_other][grapheme_break_spacing_mark] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_other][grapheme_break_zwj] |= grapheme_no_break;
    for (int from = grapheme_break_prepend; from <= grapheme_break_extended_pictographic; ++from) {
        grapheme_break_transitions[from][grapheme_break_extend] |= grapheme_no_break;
        grapheme_break_transitions[from][grapheme_break_spacing_mark] |= grapheme_no_break;
        grapheme_break_transitions[from][grapheme_break_zwj] |= grapheme_no_break;
    }

    // No break between cr and lf.
    grapheme_break_transitions[grapheme_break_cr][grapheme_break_lf] |= grapheme_no_break;

    // We can't break between pairs of regional indicators. We transition after each pair to grapheme_break_other.
    grapheme_break_transitions[grapheme_break_regional_indicator][grapheme_break_regional_indicator] =
        grapheme_no_break | grapheme_break_other;

    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_other] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_regional_indicator] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_prepend] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_l] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_v] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_t] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_lv] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_lvt] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_prepend][grapheme_break_extended_pictographic] |= grapheme_no_break;

    grapheme_break_transitions[grapheme_break_l][grapheme_break_l] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_l][grapheme_break_v] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_l][grapheme_break_lv] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_l][grapheme_break_lvt] |= grapheme_no_break;

    grapheme_break_transitions[grapheme_break_v][grapheme_break_v] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_v][grapheme_break_t] |= grapheme_no_break;

    grapheme_break_transitions[grapheme_break_t][grapheme_break_t] |= grapheme_no_break;

    grapheme_break_transitions[grapheme_break_lv][grapheme_break_v] |= grapheme_no_break;
    grapheme_break_transitions[grapheme_break_lv][grapheme_break_t] |= grapheme_no_break;

    grapheme_break_transitions[grapheme_break_lvt][grapheme_break_t] |= grapheme_no_break;

    // These transitions handle the following rule (11.0):
    // extended_pictographic extend* zwj  ×   extended_pictographic

    // We treat extended emojis as if they weren't extended.
    grapheme_break_transitions[grapheme_break_extended_pictographic][grapheme_break_extend] =
        grapheme_no_break | grapheme_break_extended_pictographic;

    // When we transition from an emoji to a zero width join, we remember that we come from an emoji.
    grapheme_break_transitions[grapheme_break_extended_pictographic][grapheme_break_zwj] =
        grapheme_no_break | grapheme_break_extended_pictographic_zwj;

    // Here we implement rule (11.0). We don't break between emojis with an zero width join between them.
    grapheme_break_transitions[grapheme_break_extended_pictographic_zwj][grapheme_break_extended_pictographic] =
        grapheme_no_break | grapheme_break_extended_pictographic;
}

grapheme_break_enum get_grapheme_break_property(const unique_ucd& ucd, uint32_t cp) {
    auto stage_one_index = cp / block_size;
    auto stage_two_index = cp % block_size;

    if (stage_one_index >= ucd.stage_one.size()) return grapheme_break_other;
    auto entry_index = ucd.stage_two[ucd.stage_one[stage_one_index]].offset[stage_two_index];
    if (entry_index < 0) return grapheme_break_other;
    return ucd.entries[entry_index].data.grapheme_break;
}

size_t to_next_grapheme_break(const unique_ucd& ucd, size_t current, uint32_t* cps, size_t count) {
    assert(cps);
    assert(current <= count);

    if (current == count) return count;
    if (current == count - 1) return count;

    uint8_t current_prop = get_grapheme_break_property(ucd, cps[current]);
    for (;;) {
        ++current;
        if (current == count) return current;
        uint8_t next_prop = get_grapheme_break_property(ucd, cps[current]);

        uint8_t transition = grapheme_break_transitions[current_prop][next_prop];
        if (transition & grapheme_no_break) {
            current_prop = transition & grapheme_mask;
            continue;
        }
        break;
    }
    return current;
}

void test_grapheme_break(const unique_ucd& ucd, const TSTRING& grapheme_break_test_filename) {
    if (!(ucd.flags & generate_flags_grapheme_break)) return;
    if (grapheme_break_test_filename.empty()) return;

    FILE* grapheme_break_test_file = FOPEN(grapheme_break_test_filename.c_str(), TEXT("rb"));
    if (!grapheme_break_test_file) return;

#ifdef _DEBUG
    FPRINTF(stdout, TEXT("Testing grapheme cluster break detection using \"") TSTRF TEXT(".\n"),
            grapheme_break_test_filename.c_str());
#endif

    auto grapheme_break_test = read_file(grapheme_break_test_file);
    close_stream(app_name, grapheme_break_test_filename.c_str(), TEXT("Error reading from"), grapheme_break_test_file);

    const char* content = grapheme_break_test.data();

    struct test {
        vector<uint32_t> cps;
        vector<bool> can_break;
    };

    vector<test> tests;
    vector<char> buffer;
    string_segment line = {};
    while (get_line(&content, &line)) {
        auto test_segment = extract_next(&line);
        if (test_segment.empty()) continue;

        auto& entry = tests.emplace_back();

        for (;;) {
            auto break_segment = extract_until_delim(&test_segment, ' ');
            if (break_segment.empty()) fatal_error("Invalid grapheme break test.");
            // Check whether codepoint is ÷ or × in utf-8.
            if (break_segment != "\xC3\xB7" && break_segment != "\xC3\x97") {
                fatal_error("Invalid grapheme break test.");
            }
            entry.can_break.push_back(break_segment == "\xC3\xB7");

            if (test_segment.empty()) break;
            uint32_t cp = invalid_codepoint;
            if (!to_cp(extract_until_delim(&test_segment, ' '), &buffer, &cp)) {
                fatal_error("Invalid grapheme break test.");
            }

            entry.cps.push_back(cp);
        }
    }

    // Perform tests.
    init_grapheme_break_transitions();
    for (size_t entry_index = 0; entry_index < tests.size(); ++entry_index) {
        auto& entry = tests[entry_index];

        size_t current = 0;
        auto cps = entry.cps.data();
        auto cps_count = entry.cps.size();
        while (current != cps_count) {
            auto prev = current;
            current = to_next_grapheme_break(ucd, current, cps, cps_count);
            for (auto i = prev + 1; i < current; ++i) {
                if (entry.can_break[i]) {
                    fatal_error("Grapheme break transitions are wrong.");
                }
            }
            if (!entry.can_break[current]) {
                fatal_error("Grapheme break transitions are wrong.");
            }
        }
    }

#ifdef _DEBUG
    FPRINTF(stdout, TEXT("Grapheme cluster break detection passed all tests.\n"));
#endif
}

struct stream_close_guard {
    FILE* stream = nullptr;
    const tchar* filename = nullptr;
    stream_close_guard() = default;
    stream_close_guard(FILE* stream, const tchar* filename) : stream(stream), filename(filename) {}
    ~stream_close_guard() {
        if (stream) {
            if (!close_stream(app_name, filename, TEXT("Error writing to"), stream)) {
                exit(-1);
            }
        }
    }
    void change_guard(FILE* new_stream, const tchar* new_filename) {
        stream = new_stream;
        filename = new_filename;
    }
    void dismiss() { stream = nullptr; }
};

#include "generate.cpp"

extern "C" int MAIN(int argc, tchar const* argv[]) {
    app_name = argv[0];
    stream_close_guard output_guard{stdout, TEXT("stdout")};

    if (argc <= 1) {
        print_usage(app_name);
        return -1;
    }

    auto cli = parse_cli(argc, argv);

    auto unicode_data = read_file(cli.unicode_data_filename.c_str());
    auto case_folding = read_file(cli.case_folding_filename.c_str());
    auto special_casing = read_file(cli.special_casing_filename.c_str());
    auto grapheme_break = read_file(cli.grapheme_break_filename.c_str());
    auto emoji_data = read_file(cli.emoji_data_filename.c_str());
    auto east_asian_width = read_file(cli.east_asian_width_filename.c_str());

    auto parsed_data = parse_all(cli.flags, unicode_data.data(), case_folding.data(), special_casing.data(),
                                 grapheme_break.data(), east_asian_width.data(), emoji_data.data());

    unique_ucd db;
    db.flags = cli.flags;
    generate_tables(parsed_data.data_entries, &db);

    test_grapheme_break(db, cli.grapheme_break_test_filename);

    FILE* out_file = stdout;
    if (cli.out_filename) {
        errno = 0;
        out_file = FOPEN(cli.out_filename, TEXT("wb"));
        if (!out_file) {
            print_error(app_name, cli.out_filename, TEXT("Failed to open file"));
            return -1;
        }

        // There is an output file, we don't care about reporting errors due to closing stdout anymore.
        // Instead switch guard focus to the output file.
        // If writing to it failes, we want to report it.
        output_guard.change_guard(out_file, cli.out_filename);
    }

    FILE* header_out_file = out_file;
    stream_close_guard header_file_guard{};
    if (cli.header_filename) {
        errno = 0;
        header_out_file = FOPEN(cli.header_filename, TEXT("wb"));
        if (!header_out_file) {
            print_error(app_name, cli.header_filename, TEXT("Failed to open file"));
            return -1;
        }

        // There is an output file, we don't care about reporting errors due to closing stdout anymore.
        // Instead switch guard focus to the output file.
        // If writing to it failes, we want to report it.
        header_file_guard.change_guard(header_out_file, cli.header_filename);
    }

    generate_header_file(db.min_sizes, cli.flags, cli.strings, header_out_file);
    generate_source_file(parsed_data, db, cli.strings, out_file);
#ifdef _DEBUG
    FPRINTF(stdout, TEXT("Successfully parsed unicode data and generated source file.\n"));
#endif

    // We don't need to close out_file/stdout, since output_guard handles it.
    return 0;
}
