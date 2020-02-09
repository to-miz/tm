#ifdef TMI_NO_FLS32
TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
    TM_ASSERT(v != 0);
    // from http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn (Public Domain)
    static const int32_t MultiplyDeBruijnBitPosition[32] = {0,  9,  1,  10, 13, 21, 2,  29, 11, 14, 16,
                                                            18, 22, 25, 3,  30, 8,  12, 20, 28, 15, 17,
                                                            24, 7,  19, 27, 23, 6,  26, 5,  4,  31};

    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}
#endif

#ifdef TMI_NO_FFS32
TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
    TM_ASSERT(v != 0);
    // from http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup (Public Domain)
    static const int32_t MultiplyDeBruijnBitPosition[32] = {0,  1,  28, 2,  29, 14, 24, 3,  30, 22, 20,
                                                            15, 25, 17, 4,  8,  31, 27, 13, 23, 21, 19,
                                                            16, 7,  26, 12, 18, 6,  11, 5,  10, 9};
    return MultiplyDeBruijnBitPosition[((uint32_t)((v & (~(v - 1))) * 0x077CB531u)) >> 27];
}
#endif

#ifdef TMI_NO_POPCOUNT32
TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) {
    // from https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel (Public Domain)
    v = v - ((v >> 1) & 0x55555555u);
    v = (v & 0x33333333u) + ((v >> 2) & 0x33333333u);
    v = (v + (v >> 4)) & 0x0F0F0F0Fu;
    return (v * 0x01010101u) >> 24u;
}
#endif

#ifdef TMI_NO_POPCOUNT64
TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) {
    // from https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel (Public Domain)
    v = v - ((v >> 1) & 0x5555555555555555ull);
    v = (v & 0x3333333333333333ull) + ((v >> 2) & 0x3333333333333333ull);
    v = (v + (v >> 4)) & 0x0F0F0F0F0F0F0F0Full;
    return (v * 0x0101010101010101ull) >> 56ull;
}
#endif

#ifdef TMI_NO_FLS64
TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
    uint32_t high = (uint32_t)(v >> 32u);
    if (high) return tmi_fls_u32(high) + 32;
    return tmi_fls_u32((uint32_t)v);
}
#endif

#ifdef TMI_NO_FFS64
TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
    uint32_t low = (uint32_t)(v & 0xFFFFFFFFull);
    if (low) return tmi_ffs_u32(low);
    return tmi_ffs_u32((uint32_t)(v >> 32)) + 32;
}
#endif