#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <assert_throws.h>
#include <assert_throws.cpp>

// clang-format off
#ifdef USE_SIGNED_SIZE_T
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 1
    typedef int tm_size_t;
#endif
// clang-format on

#include <memory>
#include <redirected_malloc.cpp>
