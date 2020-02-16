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

/*!
 * @brief Find last set, returns the last set bit (most significant bit set to 1) of v. Undefined behavior if v is 0.
 * See tmi_fls_s for safe variants.
 * Also called bitscan reverse or (127 - clz) (count leading zeroes).
 * @param v[IN] The value to search the last set bit in. Undefined behavior if v is 0.
 * @return Returns the bit position of the last set bit. All bits to the left of this position will be 0.
 * Return value will satisfy (pseudo-code): (v & (1 << return_value)) != 0.
 */
TMI_DEF uint64_t tmi_fls(tmi_uint128_t_arg v);
/*! @copydoc tmi_fls() */
TMI_DEF uint32_t tmi_fls_u32(uint32_t v);
/*! @copydoc tmi_fls() */
TMI_DEF uint64_t tmi_fls_u64(uint64_t v);

/*!
 * @brief Find first set, returns the first set bit (least significant bit) of v. Undefined behavior if v is 0.
 * See tmi_ffs_s for safe variants.
 * Also called bitscan forward or ctz (count trailing zeroes).
 * @param v[IN] The value to search the first set bit in. Undefined behavior if v is 0.
 * @return Returns the bit position of the first set bit. All bits to the right of this position will be 0.
 * Return value will satisfy (pseudo-code): (v & (1 << return_value)) != 0.
 */
TMI_DEF uint64_t tmi_ffs(tmi_uint128_t_arg v);
/*! @copydoc tmi_ffs() */
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v);
/*! @copydoc tmi_ffs() */
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v);

/*!
 * @brief Find last set, returns the last set bit (most significant bit set to 1) of v.
 * Also called bitscan reverse, (127 - clz) (count leading zeroes) or floor(log2(v)).
 * @param v[IN] The value to search the last set bit in.
 * @return Returns the bit position of the last set bit if v != 0, return -1 otherwise. All bits to the left of this
 * position will be 0. Return value will satisfy (pseudo-code): (v & (1 << return_value)) != 0.
 */
TMI_DEF int tmi_fls_s(tmi_uint128_t_arg v);
/*! @copydoc tmi_fls_s() */
TMI_DEF int tmi_fls_u32_s(uint32_t v);
/*! @copydoc tmi_fls_s() */
TMI_DEF int tmi_fls_u64_s(uint64_t v);

/*!
 * @brief Find first set, returns the first set bit (least significant bit) of v.
 * Also called bitscan forward or ctz (count trailing zeroes).
 * @param v[IN] The value to search the first set bit in.
 * @return Returns the bit position of the first set bit if v != 0, return -1 otherwise. All bits to the right of this
 * position will be 0. Return value will satisfy (pseudo-code): (v & (1 << return_value)) != 0.
 */
TMI_DEF int tmi_ffs_s(tmi_uint128_t_arg v);
/*! @copydoc tmi_ffs_s() */
TMI_DEF int tmi_ffs_u32_s(uint32_t v);
/*! @copydoc tmi_ffs_s() */
TMI_DEF int tmi_ffs_u64_s(uint64_t v);

/*!
 * @brief Returns the count of set bits (bits set to 1) in v. Also called hamming weight or sideways sum.
 * @param v[IN] The value to count the bits in.
 * @return Count of bits set to 1 in v.
 */
TMI_DEF uint64_t tmi_popcount(tmi_uint128_t_arg v);
/*! @copydoc tmi_popcount() */
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v);
/*! @copydoc tmi_popcount() */
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v);

typedef struct {
    uint8_t entries[16];  //!< Little endian ordering: Least significant byte is entries[0].
} tmi_bytes;
/*!
 * @brief Returns byte representation of v.
 * @param v[IN] Value to turn into bytes.
 * @return Returns an object containing an array of 16 uint8_ts. The first entry is the least significant byte.
 */
TMI_DEF tmi_bytes tmi_to_bytes(tmi_uint128_t_arg v);

#ifndef TMI_NO_IEEE_754

/*!
 * @brief Converts a tmi_uint128_t to float using the rounding mode returned by TM_FEGETROUND().
 * @param v[IN] The value to be converted.
 * @return The converted float, or TM_FLT_MAX on overflow.
 */
TMI_DEF float tmi_to_float(tmi_uint128_t_arg v);
/*!
 * @brief Converts a tmi_uint128_t to float.
 * @param v[IN] The value to be converted.
 * @param rounding_mode[IN] The rounding mode to use. One of TM_FE_DOWNWARD, TM_FE_TONEAREST,
 * TM_FE_TOWARDZERO, TM_FE_UPWARD.
 * @return The converted float, or TM_FLT_MAX on overflow.
 */
TMI_DEF float tmi_to_float_rm(tmi_uint128_t_arg v, int rounding_mode);

/*!
 * @brief Converts a tmi_uint128_t to double.
 * @param v[IN] The value to be converted.
 * @param rounding_mode[IN] The rounding mode to use. One of TM_FE_DOWNWARD, TM_FE_TONEAREST,
 * TM_FE_TOWARDZERO, TM_FE_UPWARD.
 * @return The converted double, or TM_DBL_MAX on overflow.
 */
TMI_DEF double tmi_to_double(tmi_uint128_t_arg v);
/*!
 * @brief Converts a tmi_uint128_t to double using the rounding mode returned by TM_FEGETROUND().
 * @param v[IN] The value to be converted.
 * @param rounding_mode[IN] The rounding mode to use. One of TM_FE_DOWNWARD, TM_FE_TONEAREST,
 * TM_FE_TOWARDZERO, TM_FE_UPWARD.
 * @return The converted double, or TM_DBL_MAX on overflow.
 */
TMI_DEF double tmi_to_double_rm(tmi_uint128_t_arg v, int rounding_mode);

/*!
 * @brief Converts a float to tmi_uint128_t. Undefined behavior if v is NaN, +-Infinite or
 * would overflow on conversion.
 * Use the tmi_from_float_s for a safe variant.
 * @param v[IN] Must not be NaN, +-Infinite or bigger than tmi_max.
 * @return The converted value. If v is negative, twos complement of v is returned.
 */
TMI_DEF tmi_uint128_t tmi_from_float(float v);
/*!
 * @brief Converts a double to tmi_uint128_t. Undefined behavior if v is NaN, +-Infinite or
 * would overflow on conversion.
 * Use the tmi_from_double_s for a safe variant.
 * @param v[IN] Must not be NaN, +-Infinite or bigger than tmi_max.
 * @return The converted value. If v is negative, twos complement of v is returned.
 */
TMI_DEF tmi_uint128_t tmi_from_double(double v);

/*!
 * @brief Converts a float to tmi_uint128_t.
 * @param v[IN] The value to be converted. The function fails (returns false) if v is NaN,
 * +-Infinite or would overflow on conversion.
 * @param out[OUT] The output parameter. It is NOT written to if the return value is false. Must not be NULL.
 * @return True on success, false otherwise.
 */
TMI_DEF tm_bool tmi_from_float_s(float v, tmi_uint128_t* out);
/*!
 * @brief Converts a double to tmi_uint128_t.
 * @param v[IN] The value to be converted. The function fails (returns false) if v is NaN,
 * +-Infinite or would overflow on conversion.
 * @param out[OUT] The output parameter. It is NOT written to if the return value is false. Must not be NULL.
 * @return True on success, false otherwise.
 */
TMI_DEF tm_bool tmi_from_double_s(double v, tmi_uint128_t* out);

#endif /* !defined(TMI_NO_IEEE_754) */

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

/*!
 * @brief Converts a nullterminated string to a tmi_uint128_t using a given base.
 * @param nullterminated[IN] A nullterminated string.
 * @param out[OUT] The output parameter. Cannot be nullptr. On success *out will contain the converted value.
 * No modifications on error.
 * @param base[IN] The base to use for the conversion.
 * @return Returns a size and an error code.
 * On success ec equals TM_OK and size contains how many bytes of the input string were consumed.
 * Possible error codes are TM_EINVAL and TM_ERANGE.
 */
TMI_DEF tmi_conv_result tmi_scan_u128(const char* nullterminated, tmi_uint128_t* out, int32_t base);

/*!
 * @brief Converts a sized string to a tmi_uint128_t using a given base.
 * @param nullterminated[IN] A non-nullterminated string.
 * @param maxlen[IN] The length of the string in bytes.
 * @param out[OUT] The output parameter. Cannot be nullptr. On success *out will contain the converted value.
 * No modifications on error.
 * @param base[IN] The base to use for the conversion.
 * @return Returns a size and an error code.
 * On success ec equals TM_OK and size contains how many bytes of the input string were consumed.
 * Possible error codes are TM_EINVAL and TM_ERANGE.
 */
TMI_DEF tmi_conv_result tmi_scan_u128_n(const char* str, tm_size_t len, tmi_uint128_t* out, int32_t base);

/*!
 * @brief Converts a tmi_uint128_t to a string.
 * @param dest[IN,OUT] A mutable buffer that will receive the string output.
 * @param maxlen[IN] The length of the buffer in bytes.
 * @param value[IN] The value to be converted.
 * @param base[IN] The base to use for the conversion.
 * @param lowercase[IN] Whether to use lowercase or uppercase letters for bases > 10. Set to true for lowercase.
 * @return Returns a size and an error code.
 * On success ec equals TM_OK and size contains how many bytes of the output buffer were used.
 * Possible error codes are TM_EOVERFLOW.
 */
TMI_DEF tmi_conv_result tmi_print_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value, int32_t base, tm_bool lowercase);

// TMI_DEF tmi_conv_result tmi_print_decimal_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value);
// TMI_DEF tmi_conv_result tmi_print_hex_u128(char* dest, tm_size_t maxlen, tmi_uint128_t value, tm_bool lowercase);

/*!
 * @brief Returns the number of digits (bytes) a tmi_uint128_t number would use if converted to string.
 * @param number[IN] The number to use.
 * @param base[IN] The base of the conversion to assume.
 * @return Returns the number of digits (bytes) the conversion would use. Can be used for allocations.
 * The return value should then be passed into tmi_print_u128_w instead of tmi_print_u128 so that the
 * digits count is not recalculated by the conversion.
 */
TMI_DEF tm_size_t tmi_get_digits_count_u128(tmi_uint128_t number, int32_t base);

/*!
 * @brief Converts a tmi_uint128_t to a string outputting at most {width} number of digits.
 * @param dest[IN,OUT] A mutable buffer that will receive the string output.
 * @param maxlen[IN] The length of the buffer in bytes.
 * @param width[IN] The number of digits to output. Must be calculated with tmi_get_digits_count_u128.
 * @param value[IN] The value to be converted.
 * @param base[IN] The base to use for the conversion.
 * @param lowercase[IN] Whether to use lowercase or uppercase letters for bases > 10. Set to true for lowercase.
 * @return Returns a size and an error code.
 * On success ec equals TM_OK and size contains how many bytes of the output buffer were used.
 * Possible error codes are TM_EOVERFLOW.
 */
TMI_DEF tmi_conv_result tmi_print_u128_w(char* dest, tm_size_t maxlen, tm_size_t width, tmi_uint128_t value,
                                         int32_t base, tm_bool lowercase);

/*!
 * @brief Convenience function to convert a string to a tmi_uint128_t ignoring errors.
 * @param nullterminated[IN] The nullterminated string to convert.
 * @return On conversion error 0 is returned. Otherwise the converted value is returned.
 */
TMI_DEF tmi_uint128_t tmi_from_string(const char* nullterminated, int32_t base);
/*!
 * @brief Convenience function to convert a string to a tmi_uint128_t ignoring errors.
 * @param str[IN] The sized string to convert.
 * @param maxlen[IN] The length of the string in bytes.
 * @return On conversion error 0 is returned. Otherwise the converted value is returned.
 */
TMI_DEF tmi_uint128_t tmi_from_string_n(const char* str, tm_size_t maxlen, int32_t base);

extern const tmi_uint128_t tmi_one;   //!< Same as tmi_make(1, 0) or tmi_make_low(1).
extern const tmi_uint128_t tmi_zero;  //!< Same as tmi_make(0, 0) or tmi_make_low(0).
extern const tmi_uint128_t tmi_max;   //!< Same as tmi_make(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull).