#if defined(__GNUC__) || defined(__clang__)
    #define tma_read_write_barrier() __asm__ __volatile__("" ::: "memory")
#elif _MSC_VER
    #define tma_read_write_barrier() _ReadWriteBarrier()
#endif

#if !defined(TM_BITSCAN_FORWARD)
    static int32_t tma_bitscan_forward(uint32_t x) {
        if (x == 0) return -1;
        /* From http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup (Public Domain). */
        static const int32_t MultiplyDeBruijnBitPosition[32] = {0,  1,  28, 2,  29, 14, 24, 3,  30, 22, 20,
                                                                15, 25, 17, 4,  8,  31, 27, 13, 23, 21, 19,
                                                                16, 7,  26, 12, 18, 6,  11, 5,  10, 9};
        return MultiplyDeBruijnBitPosition[((uint32_t)((x & (~(x - 1))) * 0x077CB531u)) >> 27];
    }
    #define TM_BITSCAN_FORWARD tma_bitscan_forward
#endif /* !defined(TM_BITSCAN_FORWARD) */