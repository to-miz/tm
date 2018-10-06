#include "../common/tm_assert.inc"

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

#include <type_traits>

/* Linkage defaults to extern, to override define TMC_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMC_DEF
    #define TMC_DEF extern
#endif

#include "../common/tm_size_t.inc"

#include "../common/tm_errc.inc"