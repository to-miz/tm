/* Global allocation functions to use. */
#if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
    // Either all or none have to be defined.
    #include <cstdlib>
    #define TM_MALLOC(size, alignment) std::malloc((size))
    #define TM_REALLOC(ptr, new_size, new_alignment) std::realloc((ptr), (new_size))
    // #define TM_REALLOC_IN_PLACE(ptr, new_size, new_alignment) // Optional
    #define TM_FREE(ptr) std::free((ptr))
    // Define as 1 if alignment parameter is actually respected.
    #define TM_MALLOC_ALIGNMENT_AWARE 0
#endif