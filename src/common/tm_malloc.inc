/* Global allocation functions to use. */
#if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
    // Either all or none have to be defined.
    #include <stdlib.h>
    #define TM_MALLOC(size, alignment) malloc((size))
    #define TM_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) realloc((ptr), (new_size))
    // #define TM_REALLOC_IN_PLACE(ptr, old_size, old_alignment, new_size, new_alignment) // Optional
    #define TM_FREE(ptr, size, alignment) free((ptr))
#endif