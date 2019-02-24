/* This file was generated using tools/unicode_gen. Do not modify by hand. */
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    tmu_grapheme_break_other,
    tmu_grapheme_break_cr,
    tmu_grapheme_break_lf,
    tmu_grapheme_break_control,
    tmu_grapheme_break_prepend,
    tmu_grapheme_break_extend,
    tmu_grapheme_break_regional_indicator,
    tmu_grapheme_break_spacing_mark,
    tmu_grapheme_break_l,
    tmu_grapheme_break_v,
    tmu_grapheme_break_t,
    tmu_grapheme_break_lv,
    tmu_grapheme_break_lvt,
    tmu_grapheme_break_zwj,
    tmu_grapheme_break_extended_pictographic,

    tmu_grapheme_break_count
} tmu_ucd_grapheme_break_enum;

typedef enum {
    tmu_ucd_case_caseless,
    tmu_ucd_case_upper,
    tmu_ucd_case_lower,
    tmu_ucd_case_title
} tmu_ucd_case_info_enum; 

typedef enum {
    tmu_ucd_category_control,
    tmu_ucd_category_letter,
    tmu_ucd_category_mark,
    tmu_ucd_category_number,
    tmu_ucd_category_punctuation,
    tmu_ucd_category_symbol,
    tmu_ucd_category_separator
} tmu_ucd_category_enum;

typedef struct {
    tmu_ucd_category_enum category;
    tmu_ucd_case_info_enum case_info;
    tmu_ucd_grapheme_break_enum grapheme_break;
    int8_t width;
    uint32_t simple_upper;
    uint32_t simple_title;
    uint32_t simple_lower;
    uint32_t simple_case_fold;
    uint32_t simple_case_toggle;
    const uint16_t* full_canonical;
    const uint16_t* full_compatibility;
    const uint16_t* full_upper;
    const uint16_t* full_title;
    const uint16_t* full_lower;
    const uint16_t* full_case_fold;
    const uint16_t* full_case_toggle;
} tmu_ucd_entry;

TMU_UCD_DEF tmu_ucd_entry tmu_ucd_get_entry(uint32_t codepoint);
TMU_UCD_DEF tmu_ucd_category_enum tmu_ucd_get_category(uint32_t codepoint);
TMU_UCD_DEF int tmu_ucd_is_whitespace(uint32_t codepoint);
TMU_UCD_DEF tmu_ucd_case_info_enum tmu_ucd_get_case_info(uint32_t codepoint);
TMU_UCD_DEF int tmu_ucd_get_width(uint32_t codepoint);

#define TMU_UCD_HAS_CASE_INFO 1
#define TMU_UCD_HAS_CATEGORY 1
#define TMU_UCD_HAS_GRAPHEME_BREAK 1
#define TMU_UCD_HAS_WIDTH 1
#define TMU_UCD_HAS_CANONICAL 1
#define TMU_UCD_HAS_COMPATIBILITY 1
#define TMU_UCD_HAS_FULL_CASE 1
#define TMU_UCD_HAS_FULL_CASE_FOLD 1
#define TMU_UCD_HAS_FULL_CASE_TOGGLE 1
#define TMU_UCD_HAS_SIMPLE_CASE 1
#define TMU_UCD_HAS_SIMPLE_CASE_FOLD 1
#define TMU_UCD_HAS_SIMPLE_CASE_TOGGLE 1

#ifdef __cplusplus
}
#endif
