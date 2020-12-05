/* Global allocation functions to use. */
#if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
    // Either all or none have to be defined.
    #include <stdlib.h>
    #define TM_MALLOC(size, alignment) malloc((size))
    #define TM_REALLOC(ptr, new_size, new_alignment) realloc((ptr), (new_size))
    // #define TM_REALLOC_IN_PLACE(ptr, new_size, new_alignment) // Optional
    #define TM_FREE(ptr) free((ptr))
    // Define as 1 if alignment parameter is actually respected.
    #define TM_MALLOC_ALIGNMENT_AWARE 0
#endif