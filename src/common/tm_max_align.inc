/* We need the max align value, but it is a C11 feature. We go with a sensible default for C. */
#ifndef TM_MAX_ALIGN
    #ifdef __cplusplus
        #include <cstddef>
        #define TM_MAX_ALIGN sizeof(std::max_align_t)
    #else
        #define TM_MAX_ALIGN (sizeof(void*) * 2)
    #endif
#endif