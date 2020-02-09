tml::uint128_t::uint128_t(tmi_uint128_t x) : value(x) {}
tml::uint128_t::uint128_t(uint64_t x) : value(tmi_make_low(x)) {}
tml::uint128_t::uint128_t(uint32_t x) : value(tmi_make_low(x)) {}
tml::uint128_t::uint128_t(uint16_t x) : value(tmi_make_low(x)) {}
tml::uint128_t::uint128_t(uint8_t x) : value(tmi_make_low(x)) {}
tml::uint128_t::uint128_t(uint64_t low, uint64_t high) : value(tmi_make(low, high)) {}

tml::uint128_t& tml::uint128_t::operator+=(uint128_t_arg rhs) {
    value = tmi_add(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator-=(uint128_t_arg rhs) {
    value = tmi_sub(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator*=(uint128_t_arg rhs) {
    value = tmi_mul(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator/=(uint128_t_arg rhs) {
    value = tmi_div(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator%=(uint128_t_arg rhs) {
    value = tmi_mod(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator&=(uint128_t_arg rhs) {
    value = tmi_and(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator|=(uint128_t_arg rhs) {
    value = tmi_or(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator^=(uint128_t_arg rhs) {
    value = tmi_xor(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator<<=(uint128_t_arg rhs) {
    value = tmi_shl(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}
tml::uint128_t& tml::uint128_t::operator>>=(uint128_t_arg rhs) {
    value = tmi_shr(TMI_PASS(value), TMI_PASS(rhs));
    return *this;
}

tml::uint128_t& tml::uint128_t::operator++() {
    value = tmi_inc(value);
    return *this;
}
tml::uint128_t tml::uint128_t::operator++(int) {
    auto old = *this;
    value = tmi_inc(value);
    return old;
}

tml::uint128_t tml::uint128_t::operator~() const { return {tmi_not(TMI_PASS(value))}; }

tml::uint128_t tml::operator+(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_add(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator-(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_sub(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator*(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_mul(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator/(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_div(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator%(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_mod(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator&(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_and(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator|(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_or(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator^(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_xor(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator<<(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_shl(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
tml::uint128_t tml::operator>>(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return {tmi_shr(TMI_PASS(lhs.value), TMI_PASS(rhs.value))};
}
bool tml::operator==(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_eq(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator!=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return !tmi_eq(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator<(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_lt(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator<=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_lte(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator>(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_gt(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}
bool tml::operator>=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs) {
    return tmi_gte(TMI_PASS(lhs.value), TMI_PASS(rhs.value));
}

tml::uint128_t::operator tmi_uint128_t() { return value; }
tml::uint128_t::operator const tmi_uint128_t&() const { return value; }

TMI_DEF std::string tml::to_string(tml::uint128_t v, int32_t base /*= 10*/) {
    return to_string(static_cast<const tmi_uint128_t&>(v), base);
}
TMI_DEF std::string tml::to_string(tmi_uint128_t v, int32_t base /*= 10*/) {
    auto width = tmi_get_digits_count_u128(v, base);
    std::string result(width, 0);
    tmi_print_u128_w(result.data(), (tm_size_t)result.size(), width, v, base, /*lowercase=*/false);
    return result;
}
TMI_DEF tml::uint128_t tml::from_string(const char* nullterminated, int32_t base /*= 10*/) {
    return {tmi_from_string(nullterminated, base)};
}
TMI_DEF tml::uint128_t tml::from_string(const char* str, tm_size_t maxlen, int32_t base /*= 10*/) {
    return {tmi_from_string_n(str, maxlen, base)};
}