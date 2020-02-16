#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#define TMI_NO_MUL128
#define TMI_NO_SHIFT128

#if __has_builtin(__builtin_clz)
    TMI_DEF uint32_t tmi_fls_u32(uint32_t v) {
        TM_ASSERT(v);
        return 31 - __builtin_clz(v);
    }
#else
    #define TMI_NO_FLS32
#endif

#if __has_builtin(__builtin_ctz)
    TMI_DEF uint32_t tmi_ffs_u32(uint32_t v) {
        TM_ASSERT(v);
        return __builtin_ctz(v);
    }
#else
    #define TMI_NO_FFS32
#endif

#if __has_builtin(__builtin_popcount)
    TMI_DEF uint32_t tmi_popcount_u32(uint32_t v) { return __builtin_popcount(v); }
#else
    #define TMI_NO_POPCOUNT32
#endif

#if __has_builtin(__builtin_clzll)
    TMI_DEF uint64_t tmi_fls_u64(uint64_t v) {
        TM_ASSERT(v);
        return 63 - __builtin_clzll(v);
    }
#else
    #define TMI_NO_FLS64
#endif


#if __has_builtin(__builtin_ctzll)
    TMI_DEF uint64_t tmi_ffs_u64(uint64_t v) {
        TM_ASSERT(v);
        return __builtin_ctzll(v);
    }
#else
    #define TMI_NO_FFS64
#endif

#if __has_builtin(__builtin_popcountll)
    TMI_DEF uint64_t tmi_popcount_u64(uint64_t v) { return __builtin_popcountll(v); }
#else
    #define TMI_NO_POPCOUNT64
#endif