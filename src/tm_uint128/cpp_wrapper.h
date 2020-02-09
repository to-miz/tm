namespace tml {

class uint128_t {
   private:
    tmi_uint128_t value;

   public:
#ifdef TMI_ARGS_BY_POINTER
    typedef const uint128_t& uint128_t_arg;
#else
    typedef uint128_t uint128_t_arg;
#endif

    uint128_t(tmi_uint128_t x);
    uint128_t(uint64_t x);
    uint128_t(uint32_t x);
    uint128_t(uint16_t x);
    uint128_t(uint8_t x);
    uint128_t(uint64_t low, uint64_t high);

    uint128_t() = default;
    uint128_t(const uint128_t_arg&) = default;

    uint128_t& operator+=(uint128_t_arg rhs);
    uint128_t& operator-=(uint128_t_arg rhs);
    uint128_t& operator*=(uint128_t_arg rhs);
    uint128_t& operator/=(uint128_t_arg rhs);
    uint128_t& operator%=(uint128_t_arg rhs);
    uint128_t& operator&=(uint128_t_arg rhs);
    uint128_t& operator|=(uint128_t_arg rhs);
    uint128_t& operator^=(uint128_t_arg rhs);
    uint128_t& operator<<=(uint128_t_arg rhs);
    uint128_t& operator>>=(uint128_t_arg rhs);

    uint128_t& operator++();
    uint128_t operator++(int);

    uint128_t operator~() const;

    friend uint128_t operator+(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator-(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator*(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator/(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator%(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator&(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator|(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator^(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator<<(uint128_t_arg lhs, uint128_t_arg rhs);
    friend uint128_t operator>>(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator==(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator!=(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator<(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator<=(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator>(uint128_t_arg lhs, uint128_t_arg rhs);
    friend bool operator>=(uint128_t_arg lhs, uint128_t_arg rhs);
    inline explicit operator bool() const { return tmi_is_not_zero(TMI_PASS(value)); }

    operator tmi_uint128_t();
    operator const tmi_uint128_t&() const;
};

uint128_t operator+(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator-(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator*(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator/(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator%(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator&(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator|(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator^(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator<<(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
uint128_t operator>>(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator==(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator!=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator<(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator<=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator>(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);
bool operator>=(tml::uint128_t::uint128_t_arg lhs, tml::uint128_t::uint128_t_arg rhs);

#ifndef TMI_NO_STL
TMI_DEF std::string to_string(uint128_t v, int32_t base = 10);
TMI_DEF std::string to_string(tmi_uint128_t v, int32_t base = 10);
TMI_DEF uint128_t from_string(const char* nullterminated, int32_t base = 10);
TMI_DEF uint128_t from_string(const char* str, tm_size_t maxlen, int32_t base = 10);
#endif

}  // namespace tml