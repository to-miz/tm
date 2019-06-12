/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* Define to 1 if tm_size_t is signed. */
    #include <stddef.h> /* Include C version so identifiers are in global namespace. */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */