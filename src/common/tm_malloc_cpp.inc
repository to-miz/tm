/* Global allocation functions to use. */
#if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
    // Either all or none have to be defined.
    #include <cstdlib>
    #define TM_MALLOC(size, alignment) std::malloc((size))
    #define TM_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) std::realloc((ptr), (new_size))
    // #define TM_REALLOC_IN_PLACE(ptr, old_size, old_alignment, new_size, new_alignment) // Optional
    #define TM_FREE(ptr, size, alignment) std::free((ptr))
#endif