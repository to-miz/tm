/* Static assert macro for C11/C++. Second parameter must be an identifier, not a string literal. */
#ifndef TM_STATIC_ASSERT
    #if defined(__cplusplus)
        #ifdef __cpp_static_assert
            #define TM_STATIC_ASSERT(cond, msg) static_assert(cond, #msg)
        #endif
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #define TM_STATIC_ASSERT(cond, msg) _Static_assert(cond, #msg)
    #else
        #define TM_STATIC_ASSERT(cond, msg) typedef char static_assertion_##msg[(cond) ? 1 : -1]
    #endif
#endif /* !defined(TM_STATIC_ASSERT) */