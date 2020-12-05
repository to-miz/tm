/*!
 * @brief A generational id. An invalid id is one where all bits are 0, which means failure to allocate.
 */
struct GenerationalId {
    uint32_t bits;  //!< The first byte is the generation, while the remaining are the id.

    constexpr uint32_t generation() const { return bits >> 24; }
    constexpr uint32_t id() const { return bits & 0x007FFFFFu; }
    constexpr bool occupied() const { return (bits & (1 << 23)) != 0; }
    constexpr explicit operator bool() const { return (bits & (1 << 23)) != 0; }

    constexpr static uint32_t generation(uint32_t bits) { return bits >> 24; }
    constexpr static uint32_t id(uint32_t bits) { return bits & 0x007FFFFFu; }
    constexpr static bool occupied(uint32_t bits) { return (bits & (1 << 23)) != 0; }
    constexpr static bool valid(uint32_t bits) { return (bits & (1 << 23)) != 0; }
    constexpr static GenerationalId make(uint32_t generation, uint32_t id, bool occupied) {
        return {((generation & 0xFFu) << 24) | (id & 0x007FFFFFu) | ((uint32_t)occupied << 23)};
    }
};