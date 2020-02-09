#define TMI_NO_MUL128

TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
    TM_ASSERT(v);
    return 31 - __builtin_clz(v);
}
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
    TM_ASSERT(v);
    return __builtin_ctz(v);
}
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) { return __builtin_popcount(v); }
TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
    TM_ASSERT(v);
    return 63 - __builtin_clzll(v);
}
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
    TM_ASSERT(v);
    return __builtin_ctzll(v);
}
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) { return __builtin_popcountll(v); }