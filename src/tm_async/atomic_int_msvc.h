// Need atomic intrinsics for MSVC.
#include <intrin.h>
typedef long tma_int;
typedef volatile long tma_atomic_int;

#if !defined(TM_BITSCAN_FORWARD)
    static int32_t tma_bitscan_forward(uint32_t x) {
        // Implement bitscan forward using an intrinsic.
        if (x == 0) return -1;
        unsigned long index = 0;
        _BitScanForward(&index, (unsigned long)x);
        return (int32_t)index;
    }
    #define TM_BITSCAN_FORWARD tma_bitscan_forward
#endif

#if defined(_M_ARM) || defined(_M_ARM64)
    #error Not implemented.
    // On ARM volatile access doesn't have acquire/load semantics, we need these intrinsics to have the compiler not
    // optimize volatile access away. Hardware barriers are also needed for memory order.
    // See https://docs.microsoft.com/en-us/cpp/intrinsics/arm-intrinsics?view=vs-2019#IsoVolatileLoadStore
    #define TMA_LOAD_VOLATILE(x) __iso_volatile_load32(x)
    #define TMA_STORE_VOLATILE(x, value) __iso_volatile_store32((x), (value))
#elif defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86)
    // x86/64 has implicit acquire/release semantics for volatile access even with -volatile:iso.
    #define TMA_LOAD_VOLATILE(x) (*(x))
    #define TMA_STORE_VOLATILE(x, value) (*(x) = (value))
#else
    #error Not implemented for this architecture.
#endif

#define TMA_COMPILER_BARRIER() _ReadWriteBarrier()

static tma_int tma_atomic_load_acquire(tma_atomic_int* value) {
    tma_int result = TMA_LOAD_VOLATILE(value);
    TMA_COMPILER_BARRIER();
    return result;
}
static tma_int tma_atomic_load_seq_cst(tma_atomic_int* value) {
    tma_int result = TMA_LOAD_VOLATILE(value);
    TMA_COMPILER_BARRIER();
    return result;
}
static tma_int tma_atomic_load_relaxed(tma_atomic_int* value) {
    tma_int result = TMA_LOAD_VOLATILE(value);
    return result;
}
static void tma_atomic_store_release(tma_atomic_int* dest, tma_int value) {
    TMA_COMPILER_BARRIER();
    TMA_STORE_VOLATILE(dest, value);
}
static void tma_atomic_store_relaxed(tma_atomic_int* dest, tma_int value) { *dest = value; }
// Sequentially consistent store.
static void tma_atomic_store_seq_cst(tma_atomic_int* dest, tma_int value) { _InterlockedExchange(dest, value); }

static tm_bool tma_atomic_compare_and_swap_strong(tma_atomic_int* dest, tma_int* comperand, tma_int new_value) {
    tma_int prev = _InterlockedCompareExchange(dest, new_value, *comperand);
    if (prev == *comperand) return TM_TRUE;
    *comperand = prev;
    return TM_FALSE;
}
// There is no weak version on MSVC.
#define tma_atomic_compare_and_swap_weak tma_atomic_compare_and_swap_strong

static tm_bool tma_atomic_compare_and_swap_strong_pointer(void* volatile* dest, void** comperand, void* new_value) {
    void* prev = _InterlockedCompareExchangePointer(dest, new_value, *comperand);
    if (prev == *comperand) return TM_TRUE;
    *comperand = prev;
    return TM_FALSE;
}

static void* tma_atomic_exchange_seq_cst_pointer(void* volatile* dest, void* new_value) {
    return _InterlockedExchangePointer(dest, new_value);
}