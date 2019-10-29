#if defined(__GNUC__)
    #error Not implemented.
    // See https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html#g_t_005f_005fatomic-Builtins
    // See https://llvm.org/docs/Atomics.html#libcalls-atomic
    typedef unsigned int tma_uint;
    typedef unsigned int tma_atomic_uint;
    #define TMA_ATOMIC_LOAD(x) __atomic_load_4((x), __ATOMIC_ACQUIRE)
    #define TMA_ATOMIC_STORE(x, rhs) __atomic_store_4((x), (rhs), __ATOMIC_RELEASE)
    #define TMA_ATOMIC_FETCH_ADD(x) __atomic_fetch_add_4((x), 1, __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_FETCH_SUB(x) __atomic_fetch_sub_4((x), 1, __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_EXCHANGE(x, new_value) __atomic_exchange_4((x), (new_value), __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_EXCHANGE_SLOT(x, new_value) __atomic_exchange_4((x), (new_value), __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_COMPARE_AND_SWAP(x, comperand, new_value) \
        __atomic_compare_exchange_4((x), (comperand), (new_value), /*weak=*/TM_FALSE, __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_COMPARE_AND_SWAP_SLOT(x, comperand, new_value) \
        __atomic_compare_exchange_4((x), (comperand), (new_value), /*weak=*/TM_FALSE, __ATOMIC_SEQ_CST)
#elif defined(_MSC_VER) && !defined(_M_ARM) && !defined(_M_ARM64)
    #include "atomic_int_msvc.h"
#elif defined(__cplusplus)
    #include <atomic>
    #error Not implemented.
#else
    #error Not implemented.
    // C11 version.
    #include <stdatomic.h>
    typedef unsigned int tma_uint;
    typedef _Atomic unsigned int tma_atomic_uint;
    #define TMA_ATOMIC_LOAD(x) atomic_load((x))
    #define TMA_ATOMIC_STORE(x, rhs) atomic_store((x), (rhs))
    #define TMA_ATOMIC_FETCH_ADD(x) atomic_fetch_add((x), 1)
    #define TMA_ATOMIC_FETCH_SUB(x) atomic_fetch_sub((x), 1)
    #define TMA_ATOMIC_EXCHANGE(x, new_value) atomic_exchange((x), (new_value))
    #define TMA_ATOMIC_EXCHANGE_SLOT(x, new_value) atomic_exchange((x), new_value)
    // atomic_compare_exchange_strong takes the comperand first as a pointer.
    static tm_bool tma_impl_atomic_compare_and_swap(volatile tma_atomic_uint* x, unsigned int comperand,
                                                    unsigned int new_value) {
        return atomic_compare_exchange_strong(x, &comperand, new_value);
    }
    static tm_bool tma_impl_atomic_compare_and_swap_slot(struct tma_work_slot* volatile* x, void* comperand,
                                                         void* new_value) {
        return atomic_compare_exchange_strong(x, &comperand, new_value);
    }
    #define TMA_ATOMIC_COMPARE_AND_SWAP(x, comperand, new_value) \
        tma_impl_atomic_compare_and_swap((x), comperand, new_value)
    #define TMA_ATOMIC_COMPARE_AND_SWAP_SLOT(x, comperand, new_value) \
        tma_impl_atomic_compare_and_swap_slot((x), comperand, new_value)
#endif