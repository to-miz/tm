/*
tm_allocator.h v0.0.2 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2019

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_ALLOCATOR_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

DESCRIPTION
    An allocator collection for simple and efficient allocators.

    An overview of some of the allocators:
    StackAllocator:
        An allocator that uses a contiguous memory pool to dish out allocations.
        Perfect when you want to allocate a bunch of different objects that are in a single contiguous block.
    MonotonicAllocator:
        Similar to StackAllocator, dishes out memory using whole memory pages.
        Perfect for tools like parsers, that allocate a lot of memory for things like ASTs and then discard all at once
        at termination.
    FixedSizeGenerationalIdAllocator:
        An allocator that hands out opaque handles to memory. These handles protect against double-free and
        use-after-free errors. Perfect for systems that hand out handles to resources, for instance an EntityManager
        that hands out Entity Ids.

    There are two main API's to allocate memory using these allocators.

    The first set are allocate_storage, reallocate_storage, reallocate_storage_in_place, free_storage.
    These are basically malloc, realloc and free. They only return raw bytes to be used as storage, no initialization
    takes place. They can be used for allocation of POD types.

    The second set are create<T>, create<T[]> and destroy.
    These are basically new, new[], delete and delete[]. They allocate and call constructors/destructors, so that they
    can be used for complex C++ objects. Destroy is used for both single allocations and arrays.
    The simpler API is create_default_init, which can allocate both single objects and arrays.

    There are also tml::make_unique<T> and tml::make_unique<T[]> that work with allocators and return std::unique_ptrs
    that automatically call destructors and free the memory.

SWITCHES
    TMAL_NO_STL
        Define this if you want to avoid including optional STL headers. This will improve compile times, but some APIs
        become unavailable:
            - create<T>, create<T[]> (use create_default_init instead).
            - tml::make_unique<T>
            - std::recursive_mutex backend for RecursiveMutex.
              This means that either <pthreads> or <windows.h> are needed for locking, unless TMAL_NO_LOCKS is defined.
            - Some throwing functions thow nullptr_t instead of standard exceptions.
        These headers will not be included:
            <exception>
            <memory>
            <utility>
            <iterator>
            <mutex>

    TMAL_NO_LOCKS
        Disables locking features for single threaded apps, RecursiveMutex and LockingAllocator<T> are unavailable.

    TMAL_ALLOCATION_HELPERS_LEVEL = 1
        Which debug level to use. Default is 0, which disables debug features.
        Level 1 will enable indepth logging of allocations, which will catch double-free and use-after-free errors, but
        slows down execution by a lot.
        Level 2 will additionaly redirect every single allocation to a mmap/VirtualAlloc with protected page boundaries,
        to detect out of bounds reads/writes.

    TMAL_HAS_WINDOWS_H_INCLUDED
        Define this if you already have <windows.h> included and want to enable Win32 specific features. This will
        enable tmal_mmap to use VirtualAlloc on Windows instead of malloc and RecursiveMutex to use CRITICAL_SECTIONs.
        Note that this will be automatically defined, if TMAL_NO_STL is defined, since for some implementations
        there is no STL to fall back on.

    For other switches/macros that control dependencies, search for "General Dependencies".

ISSUES
    - Not yet first release.

TODO
    - Write documentation.

HISTORY     (DD.MM.YY)
    v0.0.2   04.01.20 Added tml::MonotonicAllocator::current_stack_allocator.
    v0.0.1   21.12.19 Initial Commit.
*/

#include "../common/GENERATED_WARNING.inc"

/*
General Dependencies

You can redefine these macros to either get rid of the dependencies with native versions.
*/
#include "dependencies_header.h"

#ifdef TM_ALLOCATOR_IMPLEMENTATION
    #include "dependencies.h"
#endif /* defined(TM_ALLOCATOR_IMPLEMENTATION) */

#ifndef _TM_ALLOCATOR_H_INCLUDED_F4741AD2_9668_4A0F_845F_6204A55B5B14
#define _TM_ALLOCATOR_H_INCLUDED_F4741AD2_9668_4A0F_845F_6204A55B5B14

#include "definitions.h"

namespace tml {

#include "functions.h"

#include "Mutex.h"

#include "StackAllocator.h"

#include "MonotonicAllocator.h"

#include "FixedSizeIdAllocator.h"

#include "LockingAllocator.h"

#include "MallocAllocator.h"

#include "allocation_helpers.h"

}  // namespace tml

#endif /* !defined(_TM_ALLOCATOR_H_INCLUDED_F4741AD2_9668_4A0F_845F_6204A55B5B14) */

// Implementation

#ifdef TM_ALLOCATOR_IMPLEMENTATION

#include "../common/tm_unreferenced_param.inc"

#include "functions.cpp"

#include "Mutex.cpp"

#include "StackAllocator.cpp"

#include "MonotonicAllocator.cpp"

#include "StackAllocatorGuard.cpp"

#include "FixedSizeIdAllocator.cpp"

#include "MallocAllocator.cpp"

#endif /* defined(TM_ALLOCATOR_IMPLEMENTATION) */

#define MERGE_YEAR 2019
#include "../common/LICENSE.inc"