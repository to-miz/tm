/* Default alignment for TM_MALLOC, the default for malloc is alignof(max_align_t). */
#ifndef TM_MAX_ALIGN
    #ifdef __cplusplus
        #include <cstddef>
        #define TM_MAX_ALIGN alignof(std::max_align_t)
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #include <stddef.h>
        #define TM_MAX_ALIGN _Alignof(max_align_t)
    #else
        #define TM_MAX_ALIGN (sizeof(void*))
    #endif
#endif