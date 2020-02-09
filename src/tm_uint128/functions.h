TMI_DEF tmi_uint128_t tmi_make(uint64_t low, uint64_t high);
TMI_DEF tmi_uint128_t tmi_make_low(uint64_t low);
TMI_DEF tmi_uint128_t tmi_make_high(uint64_t high);
TMI_DEF tmi_uint128_t tmi_make_bitmask(uint64_t set_bit_position);

TMI_DEF tmi_uint128_t tmi_add(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_add_u64(tmi_uint128_t_arg lhs, uint64_t rhs);
TMI_DEF tmi_uint128_t tmi_sub(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_sub_u64(tmi_uint128_t_arg lhs, uint64_t rhs);
TMI_DEF tmi_uint128_t tmi_mul(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_mul_u64(tmi_uint128_t_arg lhs, uint64_t rhs);
TMI_DEF tmi_uint128_t tmi_div(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_mod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_and(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_or(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_xor(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_not(tmi_uint128_t_arg v);
TMI_DEF tmi_uint128_t tmi_shl(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_shr(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_uint128_t tmi_shl_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount);
TMI_DEF tmi_uint128_t tmi_shr_u64(tmi_uint128_t_arg lhs, uint64_t shift_amount);
typedef struct {
    tmi_uint128_t div;
    tmi_uint128_t mod;
} tmi_divmod_result;
TMI_DEF tmi_divmod_result tmi_divmod(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tmi_divmod_result tmi_divmod_u64(tmi_uint128_t_arg lhs, uint64_t rhs);

TMI_DEF tmi_uint128_t tmi_inc(tmi_uint128_t_arg v);
TMI_DEF tmi_uint128_t tmi_dec(tmi_uint128_t_arg v);

TMI_DEF tm_bool tmi_lt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tm_bool tmi_lte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tm_bool tmi_gt(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tm_bool tmi_gte(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF tm_bool tmi_eq(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);
TMI_DEF int tmi_cmp(tmi_uint128_t_arg lhs, tmi_uint128_t_arg rhs);

TMI_DEF tm_bool tmi_is_zero(tmi_uint128_t_arg v);
TMI_DEF tm_bool tmi_is_not_zero(tmi_uint128_t_arg v);
TMI_DEF tm_bool tmi_is_bit_set(tmi_uint128_t_arg v, uint64_t bit_position);
TMI_DEF tmi_uint128_t_arg tmi_set_bit(tmi_uint128_t_arg v, uint64_t bit_position);

TMI_DEF uint64_t tmi_low(tmi_uint128_t_arg v);
TMI_DEF uint64_t tmi_high(tmi_uint128_t_arg v);
TMI_DEF uint64_t tmi_fls(tmi_uint128_t_arg v);
TMI_DEF uint64_t tmi_ffs(tmi_uint128_t_arg v);
TMI_DEF uint64_t tmi_popcount(tmi_uint128_t_arg v);
TMI_DEF uint64_t tmi_fls_u64(uint64_t v);
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v);
TMI_DEF uint32_t tmi_fls_u32(uint32_t v);
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v);
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v);
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v);

typedef struct {
    uint8_t entries[16];  //!< Little endian ordering: Least significant byte is entries[0].
} tmi_bytes;
TMI_DEF tmi_bytes tmi_to_bytes(tmi_uint128_t_arg v);

// Include tm_conversion.h before this header to use tm_conversion types for string conversions.

// clang-format off
#if defined(_TM_CONVERSION_H_INCLUDED_)
    typedef tmc_conv_result tmi_conv_result;
    // Remove tmi prefix to keep in line with tm_conversion naming convention.
    #define tmi_scan_u128 scan_u128
    #define tmi_scan_u128_n scan_u128_n
    #define tmi_print_u128 print_u128
    #define tmi_print_decimal_u128 print_decimal_u128
    #define tmi_print_hex_u128 print_hex_u128
    #define tmi_get_digits_count_u128 get_digits_count_u128
#else
    typedef struct {
        tm_size_t size;
        tm_errc ec;
    } tmi_conv_result;
#endif
// clang-format on

TMI_DEF tmi_conv_result tmi_scan_u128(const char* nullterminated, tmi_uint128_t* out, int32_t base);
TMI_DEF tmi_conv_result tmi_scan_u128_n(const char* str, tm_size_t len, tmi_uint128_t* out, int32_t base);
TMI_DEF tmi_conv_result tmi_print_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value, int32_t base, tm_bool lowercase);
// TMI_DEF tmi_conv_result tmi_print_decimal_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value);
// TMI_DEF tmi_conv_result tmi_print_hex_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value, tm_bool lowercase);

TMI_DEF tm_size_t tmi_get_digits_count_u128(tmi_uint128_t number, int32_t base);
TMI_DEF tmi_conv_result tmi_print_u128_w(char* dest, tm_size_t maxlen, tm_size_t width, tmi_uint128_t value,
                                         int32_t base, tm_bool lowercase);

TMI_DEF tmi_uint128_t tmi_from_string(const char* nullterminated, int32_t base);
TMI_DEF tmi_uint128_t tmi_from_string_n(const char* str, tm_size_t maxlen, int32_t base);

extern const tmi_uint128_t tmi_one;
extern const tmi_uint128_t tmi_zero;
extern const tmi_uint128_t tmi_max;