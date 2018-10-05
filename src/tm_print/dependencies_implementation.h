#ifdef TM_PRINT_IMPLEMENTATION
    /* string.h dependency */
    #if !defined(TM_MEMCPY) || !defined(TM_MEMSET) || !defined(TM_MEMCHR) || !defined(TM_STRLEN)
        #include <cstring>
    #endif
    #ifndef TM_MEMCPY
        #define TM_MEMCPY std::memcpy
    #endif
    #ifndef TM_MEMMOVE
        #define TM_MEMMOVE std::memmove
    #endif
    #ifndef TM_MEMSET
        #define TM_MEMSET std::memset
    #endif
    #ifndef TM_MEMCHR
        #define TM_MEMCHR std::memchr
    #endif
    #ifndef TM_STRLEN
        #define TM_STRLEN std::strlen
    #endif

    /* cstdlib dependency */
    #if !defined(TMP_MALLOC) || !defined(TMP_REALLOC) || !defined(TMP_FREE)
        // either all or none have to be defined
        #include <cstdlib>
        #define TMP_MALLOC(type, size) (type*)malloc(size)
        #define TMP_FREE(ptr) free(ptr)
        #define TMP_REALLOC(type, ptr, size) (type*)realloc(ptr, size)
    #endif

    /*
    What dependency/backend to use to convert values.
    Available backends are:
        Integer values (int32_t, int64_t, uint32_t, uint64_t):
            TMP_INT_BACKEND_CRT                inefficient, only base 10, 16 and 8 are possible, locale dependent
            TMP_INT_BACKEND_TM_CONVERSION      efficient, because size calculations are possible without printing
            TMP_INT_BACKEND_CHARCONV           needs C++17/20 standard library, but postprocessing needed after printing
        Floating point values (float, double):
            TMP_FLOAT_BACKEND_CRT              might not do correct rounding, locale dependent
            TMP_FLOAT_BACKEND_TM_CONVERSION    very inaccurate, hex and scientific printing not implemented yet
            TMP_FLOAT_BACKEND_CHARCONV         needs C++17/20 standard library, probably fast and accurate
    */
    #if !defined(TMP_INT_BACKEND_CRT) && !defined(TMP_INT_BACKEND_TM_CONVERSION) && !defined(TMP_INT_BACKEND_CHARCONV)
        #define TMP_INT_BACKEND_CRT
    #endif
    #if !defined(TMP_FLOAT_BACKEND_CRT) && !defined(TMP_FLOAT_BACKEND_TM_CONVERSION) && !defined(TMP_FLOAT_BACKEND_CHARCONV)
        #define TMP_FLOAT_BACKEND_CRT
    #endif

    #if defined(TMP_INT_BACKEND_CRT) + defined(TMP_INT_BACKEND_TM_CONVERSION) + defined(TMP_INT_BACKEND_CHARCONV) != 1
        #error "Only one backend for int is allowed."
    #endif

    #if defined(TMP_FLOAT_BACKEND_CRT) + defined(TMP_FLOAT_BACKEND_TM_CONVERSION) + defined(TMP_FLOAT_BACKEND_CHARCONV) != 1
        #error "Only one backend for float is allowed."
    #endif

    #if defined(TMP_INT_BACKEND_TM_CONVERSION) || defined(TMP_FLOAT_BACKEND_TM_CONVERSION)
        #define TMP_TM_CONVERSION_INCLUDED
        // in case you want to use tm_conversion but just don't want this header to include it
        #ifndef TMP_NO_INCLUDE_TM_CONVERSION
            #include <tm_conversion.h>
        #endif
    #endif

    #if defined(TMP_INT_BACKEND_CRT) || defined(TMP_FLOAT_BACKEND_CRT) || !defined(TMP_NO_CRT_FILE_PRINTING)
        #define TMP_CRT_INCLUDED
        #include <cstdio>
        #include <cinttypes>
    #endif

    #if defined(TMP_INT_BACKEND_CHARCONV) || defined(TMP_FLOAT_BACKEND_CHARCONV)
        #define TMP_CHARCONV_INCLUDED
        #include <charconv>
    #endif

    #if !defined(TMP_STRTOUL) && (!defined(TMP_TM_CONVERSION_INCLUDED) && !defined(TMP_CHARCONV_INCLUDED))
        #include <cstdlib>
        #define TMP_STRTOUL strtoul
    #endif

    #if !defined(TMP_SNPRINTF) && (defined(TMP_INT_BACKEND_CRT) || defined(TMP_FLOAT_BACKEND_CRT))
        #include <cstdio>
        #if defined(_MSC_VER) && _MSC_VER <= 1800
            // in case _MSC_VER is <= Visual Studio 2013
            // snprintf wasn't supported until after Visual Studio 2013
            #define TMP_SNPRINTF _snprintf
        #else
            #define TMP_SNPRINTF snprintf
        #endif
    #endif

    #if !defined(TMP_TOUPPER) && defined(TMP_INT_BACKEND_CHARCONV)
        #include <cctype>
        #define TMP_TOUPPER toupper
    #endif

    // the buffer size used for small buffer optimization, change this by profiling if malloc is a bottleneck
    #ifndef TMP_SBO_SIZE
        #define TMP_SBO_SIZE 200
    #endif  // !defined( TMP_SBO_SIZE )
#endif
