/* Force inline when optimizing. */
#ifndef TM_INLINE
    #if defined(__GNUC__) || defined(__clang__)
        #ifdef __OPTIMIZE__
            #define TM_INLINE static inline __attribute__((always_inline))
        #else
            #define TM_INLINE static inline
        #endif
    #elif defined(_MSC_VER)
        #ifdef NDEBUG
            #define TM_INLINE static __forceinline
        #else
            #define TM_INLINE static inline
        #endif
    #else
        #error Unsupported compiler.
    #endif
#endif