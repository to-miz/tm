#include "../common/tm_malloc_cpp.inc"

#if !defined(TM_MEMCPY)
    #include <cstring>
    #ifndef TM_MEMCPY
        #define TM_MEMCPY std::memcpy
    #endif
#endif

#ifndef TMAL_NO_STL
    #include <exception>  // std::bad_alloc, std::system_error
    #include <memory>     // std::unique_ptr
    #include <utility>    // std::forward, std::move
    #include <mutex>      // std::recursive_mutex

#else
    #if defined(_WIN32) && !defined(TMAL_HAS_WINDOWS_H_INCLUDED)
        // If TMAL_NO_STL is defined, we will need <windows.h> for RecursiveMutex, in which case we can allow Win32
        // specific implementations.
        #define TMAL_HAS_WINDOWS_H_INCLUDED

        #ifndef NOMINMAX
            #define NOMINMAX
        #endif
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif
        #include <windows.h>
    #endif
#endif

#if defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
    #include <sys/mman.h>  // mmap
    #include <unistd.h>    // sysconf(_SC_PAGESIZE)
#endif