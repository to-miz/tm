#include "../common/tm_assert.inc"

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* Optional STL extensions, like printing directly to std::string. */
#ifdef TMP_USE_STL
    #include <string>
#endif

#include <type_traits>

/* Linkage defaults to extern, to override define TMP_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMP_DEF
    #define TMP_DEF extern
#endif

#include "../common/tm_size_t.inc"

#include "../common/tm_errc.inc"