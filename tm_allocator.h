/*
tm_allocator.h v0.0.2 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2020

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

/* This is a generated file, do not modify directly. You can find the generator files in the src directory. */

/*
General Dependencies

You can redefine these macros to either get rid of the dependencies with native versions.
*/
/* assert */
#ifndef TM_ASSERT
    #include <assert.h>
    #define TM_ASSERT assert
#endif /* !defined(TM_ASSERT) */

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

#ifndef TM_PLACEMENT_NEW
    #include <new>
    #define TM_PLACEMENT_NEW(dest) ::new (static_cast<void*>(dest))
#endif

#ifndef TMAL_NO_STL
    #include <iterator>  // std::forward_iterator_tag
#endif

#ifdef TM_ALLOCATOR_IMPLEMENTATION
    /* Global allocation functions to use. */
    #if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
        // Either all or none have to be defined.
        #include <cstdlib>
        #define TM_MALLOC(size, alignment) std::malloc((size))
        #define TM_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) std::realloc((ptr), (new_size))
        // #define TM_REALLOC_IN_PLACE(ptr, old_size, old_alignment, new_size, new_alignment) // Optional
        #define TM_FREE(ptr, size, alignment) std::free((ptr))
    #endif

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
#endif /* defined(TM_ALLOCATOR_IMPLEMENTATION) */

#ifndef _TM_ALLOCATOR_H_INCLUDED_F4741AD2_9668_4A0F_845F_6204A55B5B14
#define _TM_ALLOCATOR_H_INCLUDED_F4741AD2_9668_4A0F_845F_6204A55B5B14

#ifndef TM_DEFAULT_ALIGNMENT
    #define TM_DEFAULT_ALIGNMENT 8
#endif

namespace tml {

bool is_power_of_two(size_t value);
bool is_pointer_aligned(const void* ptr, size_t alignment);
bool is_pointer_aligned(uintptr_t ptr, size_t alignment);
/*!
 * @brief Returns the alignment offset to the given pointer.
 * @param ptr[in] Pointer to align.
 * @param alignment[in] Alignment to use.
 * @return Returns how much to adjust ptr to align it to alignment.
 * @example char* aligned_ptr = (char*)ptr + alignment_offset(ptr, alignof(int));
 */
size_t alignment_offset(const void* ptr, size_t alignment);
/*!
 * @brief Returns the alignment offset to the given pointer value.
 * @param ptr[in] Pointer to align.
 * @param alignment[in] Alignment to use.
 * @return Returns how much to adjust ptr to align it to alignment.
 */
size_t alignment_offset(uintptr_t ptr, size_t alignment);
/*!
 * @brief Helper to calculate the next capacity of an array in case of growth.
 * @return Basically returns current_capacity * 1.5 more or less.
 */
inline size_t next_capacity(size_t current_capacity) { return 3 * ((current_capacity + 2) / 2); }

/*!
 * @brief Helper wrapper around mmap/VirtualAlloc depending on platform.
 */
void* tmal_mmap(size_t size);
/*!
 * @brief Helper wrapper around munmmap/VirtualFree depending on platform.
 */
bool tmal_munmap(void* ptr, size_t size);
/*!
 * @brief Helper wrapper around sysconf(_SC_PAGESIZE)/GetSystemInfo(&info) depending on platform.
 */
size_t tmal_get_mmap_granularity();

struct RecursiveMutex {
    RecursiveMutex();
    RecursiveMutex(RecursiveMutex&& other);
    RecursiveMutex& operator=(RecursiveMutex&& other);
    ~RecursiveMutex();

    bool lock() const;
    /*!
     * @brief Locks mutex, but throws on failure.
     * @throw
     *  If TMAL_NO_STL is defined:
     *      Throws nullptr_t on failure.
     *  Otherwise:
     *      Throws either std::bad_alloc if mutex wasn't initialized, or std::system_error if locking failed.
     */
    void lock_throws() const;
    bool try_lock() const;
    bool unlock() const;

    /*!
     * @brief Destroys the mutex. Destroying the mutex while it is locked is undefined behavior.
     */
    void destroy();

   private:
    void* internal;
};

struct LockGuard {
    LockGuard() = default;
    /*!
     * @brief Unlocks the mutex if locked.
     */
    ~LockGuard();

    /*!
     * @brief Tries to lock the mutex. Throws std::system_error or nullptr_t if TMAL_NO_STL is defined on failure.
     * @throw
     *  If TMAL_NO_STL is defined:
     *      Throws nullptr_t on failure.
     *  Otherwise:
     *      Throws either std::bad_alloc if mutex wasn't initialized, or std::system_error if locking failed.
     */
    explicit LockGuard(const RecursiveMutex& mutex);

    /*!
     * @brief Tries to lock the mutex.
     * @param mutex[IN] The mutex that will be locked.
     * @return Returns true if the lock operation succeeds, false otherwise.
     */
    bool lock(const RecursiveMutex& mutex);

    bool lock(const RecursiveMutex&& mutex) = delete;

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

   private:
    const RecursiveMutex* mutex = nullptr;
};

/*!
 * @brief A stack allocator that grows and shrinks in a linear fashion on a contiguous block of memory.
 * Note that the StackAllocator does not allocate or free its internal memory, it is designed to use any buffer given to
 * it as its memory pool.
 */
struct StackAllocator {
   protected:
    char* p = nullptr;
    size_t sz = 0;
    size_t cap = 0;
    size_t last_popped_alignment = 0;

   public:
    StackAllocator() = default;
    StackAllocator(void* ptr, size_t capacity);
    StackAllocator(StackAllocator&& other) = default;
    StackAllocator& operator=(StackAllocator&& other) = default;

    void* allocate_bytes(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    void* reallocate_bytes(void* ptr, size_t old_size, size_t new_size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    bool reallocate_bytes_in_place(void* ptr, size_t old_size, size_t new_size,
                                   size_t alignment = TM_DEFAULT_ALIGNMENT);
    void free_bytes(void* ptr, size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    bool owns(const void* ptr) const;
    bool is_most_recent_allocation(const void* ptr, size_t size) const;
    void* base() const;

    inline size_t remaining() const { return cap - sz; }
    size_t remaining(size_t alignment) const;

    inline char* end() const {
        TM_ASSERT(is_valid(this));
        return p + sz;
    }
    inline char* back() const {
        TM_ASSERT(is_valid(this));
        return p + sz;
    }
    void clear();

    inline static bool is_valid(const StackAllocator* allocator) {
        return allocator && (!allocator->p || (allocator->cap && allocator->sz <= allocator->cap));
    }

    struct StateSnapshot {
        size_t sz;
        size_t last_popped_alignment;
    };

    inline StateSnapshot get_state() const { return {sz, last_popped_alignment}; }
    void set_state(StateSnapshot snapshot);
};

/*!
 * @brief Same as StackAllocator, but allocates its underlying memory pool using TM_MALLOC and frees it upon
 * destruction.
 */
struct DynamicStackAllocator : StackAllocator {
   public:
    DynamicStackAllocator() = default;
    DynamicStackAllocator(size_t capacity);
    DynamicStackAllocator(DynamicStackAllocator&& other);
    DynamicStackAllocator& operator=(DynamicStackAllocator&& other);
    ~DynamicStackAllocator();
};

/*!
 * @brief Returns how much capacity the given allocator has for a given type T.
 * @param allocator[IN] The allocator to be used.
 * @return Returns how many elements of T can be allocated using allocator.
 * @example
 *      StackAllocator allocator = ...
 *      size_t count = get_capacity_for<int>(&allocator);
 *      int* ints = allocate_storage(&allocator, int, count); // We have now exhausted the allocator.
 */
template <class T>
size_t get_capacity_for(StackAllocator* allocator) {
    TM_ASSERT(allocator);
    return allocator->remaining(alignof(T)) / sizeof(T);
}

/*!
 * @brief A guard for StackAllocator that frees all memory that was allocated after its constructor.
 *
 * Common use cases are:
 *  - There are multiple error paths, but only one success path. The guard allows that memory is freed on error by
 *    returning, but on success you can keep allocated memory by dismissing the guard.
 *  - Use the allocator as temporary memory and free allocated memory at the end of the scope.
 *
 * @example
 *      StackAllocator allocator = ...
 *      int* single_int = allocate_storage(&allocator, int, 1); // Allocate a single int.
 *      {
 *          auto guard = StackAllocatorGuard{&allocator};
 *          int* four_ints = allocate_storage(&allocator, int, 4);
 *          int* five_ints = allocate_storage(&allocator, int, 5);
 *          // four_ints and five_ints get freed once this scope ends.
 *      }
 *      // single_int is still valid, only four_ints and five_ints were freed.
 */
struct StackAllocatorGuard {
    StackAllocator* allocator;
    StackAllocator::StateSnapshot state;

    StackAllocatorGuard();
    explicit StackAllocatorGuard(StackAllocator* allocator);
    StackAllocatorGuard(StackAllocatorGuard&& other);
    StackAllocatorGuard& operator=(StackAllocatorGuard&& other);
    ~StackAllocatorGuard();

    /*!
     * @brief Dismisses the guard. The destructor doesn't free the allocated memory.
     */
    void dismiss();
};

/*!
 * @brief A monotonic growing allocator that dishes out memory as requested.
 *
 * Freeing of memory is not supported. Perfect for applications and tools that use an indeterminate amount of memory for
 * processing, and then free it all at once. Internally it is just a collection of StackAllocators, with calls to
 * VirtualAlloc/mmap.
 */
class MonotonicAllocator {
    StackAllocator* allocators = nullptr;  //!< Dynamic array of StackAllocators.
    int32_t current = 0;                   //!< Index into allocators that marks the currently active allocator.
    int32_t capacity = 0;                  //!< Capacity of allocators.
    size_t block_size = 0;                 //!< The minimum size of each StackAllocator in allocators.
    bool leak_memory = false;              //!< Whether to leak memory on destruction.

   public:
    MonotonicAllocator();
    MonotonicAllocator(MonotonicAllocator&& other);
    MonotonicAllocator& operator=(MonotonicAllocator&& other);
    ~MonotonicAllocator();

    /*!
     * @brief Allocates memory.
     * @param size[IN] Size of the memory region to be allocated in bytes.
     * @param alignment[IN] Alignment of the memory region. Default is at least TM_DEFAULT_ALIGNMENT(8).
     * @return Pointer to the allocated memory region, nullptr if out of memory.
     */
    void* allocate_bytes(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    /*!
     * @brief Allocates memory. Same as allocate_bytes, but throws if out of memory.
     * @param size[IN] Size of the memory region to be allocated in bytes.
     * @param alignment[IN] Alignment of the memory region. Default is at least TM_DEFAULT_ALIGNMENT(8).
     * @return Pointer to the allocated memory region.
     * @throw Throws std::bad_alloc (or nullptr_t if TMAL_NO_STL is defined) if out of memory.
     */
    void* allocate_bytes_throws(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    /*!
     * @brief Returns whether the given pointer was allocated using this allocator.
     */
    bool owns(const void* ptr) const;

    /*!
     * @brief Whether to leak the memory that was allocated on destruction.
     *
     * This allocator makes the most sense for tools that allocate a lot of memory, process it, and then exit.
     * In this scenario, cleanup of the allocated resources can be seen as waste, since the application is exiting
     * anyway, in which case the OS will do the cleanup. The additional bookkeeping of freeing the memory is just
     * wasted. In these cases, leak_memory_on_destruction allows to leak the memory to the OS.
     *
     * @param enabled[IN] Set to true to leak memory on destruction.
     */
    inline void leak_memory_on_destruction(bool enabled) { leak_memory = enabled; }

    /*!
     * @brief Returns the currently active StackAllocator.
     */
    StackAllocator* current_stack_allocator();

    /*!
     * @brief Returns whether allocator is in a valid state.
     */
    inline static bool is_valid(const MonotonicAllocator* allocator) {
        return allocator && allocator->allocators && allocator->current >= 0 &&
               allocator->current < allocator->capacity;
    }
};

/*!
 * @brief A generational id. An invalid id is one where all bits are 0, which means failure to allocate.
 */
struct GenerationalId {
    uint32_t bits;  //!< The first byte is the generation, while the remaining are the id.

    inline uint32_t generation() const { return bits >> 24; }
    inline uint32_t id() const { return bits & 0x007FFFFFu; }
    inline bool occupied() const { return (bits & (1 << 23)) != 0; }
    inline explicit operator bool() const { return (bits & (1 << 23)) != 0; }

    inline static uint32_t generation(uint32_t bits) { return bits >> 24; }
    inline static uint32_t id(uint32_t bits) { return bits & 0x007FFFFFu; }
    inline static bool occupied(uint32_t bits) { return (bits & (1 << 23)) != 0; }
    inline static bool valid(uint32_t bits) { return (bits & (1 << 23)) != 0; }
    inline static GenerationalId make(uint32_t generation, uint32_t id, bool occupied) {
        return {((generation & 0xFFu) << 24) | (id & 0x007FFFFFu) | ((uint32_t)occupied << 23)};
    }
};

/*!
 * @brief A fixed size allocator that returns simple ids instead of pointers.
 *
 * The ids are simple indexes into the underlying memory. As such, care has to be taken to not use ids after they are
 * destroyed. For a fixed size allocator that protects against use-after-free errors, see
 * FixedSizeGenerationalIdAllocator.
 */
struct FixedSizeIdAllocator {
    FixedSizeIdAllocator() = default;
    /*!
     * @brief Creates a FixedSizeIdAllocator.
     * @param initial_element_capacity[IN] Initial capacity in elements.
     * @param element_size_in_bytes[IN] Size of each element in bytes. Must be at least sizeof(int32_t)
     * @param element_alignment[IN] Alignment of each element in bytes. Must be power of two.
     */
    FixedSizeIdAllocator(int32_t initial_element_capacity, int32_t element_size_in_bytes,
                         int32_t element_alignment = TM_DEFAULT_ALIGNMENT);
    ~FixedSizeIdAllocator();
    FixedSizeIdAllocator(FixedSizeIdAllocator&&);
    FixedSizeIdAllocator& operator=(FixedSizeIdAllocator&&);

    /*!
     * @brief Allocates a fixed size memory block and returns its generational id.
     * @return Returns the id of the allocated block. A value of 0 means failure to allocate. The returned uint32_t
     * should be treated as an opaque resource that should be packed into a type safe wrapper, so that ids from
     * different allocators don't get mixed.
     */
    uint32_t create();
    /*!
     * @brief Destroys the element with the given id. The pointer gotten from data_from_id is invalidated for the given
     * id.
     * @param id[IN] The id of the element to be destroyed.
     */
    void destroy(uint32_t id);
    /*!
     * @brief Returns the allocated pointer that corresponds to the given id.
     * Note that the returned pointer should not be stored as the pointer gets invalidated whenever create is called.
     * Use the pointer only for immediate lookups and calculations, only the id is meant to be stored.
     * @return Returns the allocated pointer that corresponds to the given id, or nullptr if the id is invalid.
     */
    void* data_from_id(uint32_t id);

    inline explicit operator bool() const { return data != nullptr && capacity; };

   private:
    void* data = nullptr;
    int32_t size = 0;
    int32_t capacity = 0;
    int32_t free_index = -1;
    int32_t allocation_size = 0;
    int32_t allocation_alignment = 0;

    uint32_t pop();
    void push(uint32_t id);
};

/*!
 * @brief A fixed size allocator that returns generational ids instead of pointers.
 *
 * The generational ids allow for the reallocation of the underlying memory without invalidating the ids.
 * Best suited for dishing out opaque identifiers for resources that are managed by an encapsulated system.
 * Think of textures, entity ids and such.
 *
 * The implementation uses headers for each field, which makes it possible to catch use-after-free errors and enables
 * iteration.
 */
struct FixedSizeGenerationalIdAllocator {
    FixedSizeGenerationalIdAllocator() = default;
    /*!
     * @brief Creates a FixedSizeGenerationalIdAllocator.
     * @param initial_element_capacity[IN] Initial capacity in elements.
     * @param element_size_in_bytes[IN] Size of each element in bytes.
     * @param element_alignment[IN] Alignment of each element in bytes. Must be power of two.
     */
    FixedSizeGenerationalIdAllocator(int32_t initial_element_capacity, int32_t element_size_in_bytes,
                         int32_t element_alignment = TM_DEFAULT_ALIGNMENT);
    ~FixedSizeGenerationalIdAllocator();
    FixedSizeGenerationalIdAllocator(FixedSizeGenerationalIdAllocator&&);
    FixedSizeGenerationalIdAllocator& operator=(FixedSizeGenerationalIdAllocator&&);

    /*!
     * @brief Allocates a fixed size memory block and returns its generational id.
     * @return Returns the id of the allocated block. A value of 0 means failure to allocate. The returned uint32_t
     * should be treated as an opaque resource that should be packed into a type safe wrapper, so that ids from
     * different allocators don't get mixed.
     */
    uint32_t create();
    /*!
     * @brief Destroys the element with the given id. The pointer gotten from data_from_id is invalidated for the given
     * id. Iterators are not invalidated on destroy, because destroy doesn't change the size of the container and
     * doesn't move elements around.
     * @param id[IN] The id of the element to be destroyed.
     */
    void destroy(uint32_t id);
    /*!
     * @brief Returns the allocated pointer that corresponds to the given id.
     * Note that the returned pointer should not be stored as the pointer gets invalidated whenever create is called.
     * Use the pointer only for immediate lookups and calculations, only the id is meant to be stored.
     * @return Returns the allocated pointer that corresponds to the given id, or nullptr if the id is invalid.
     */
    void* data_from_id(uint32_t id);

    inline explicit operator bool() const { return data != nullptr && capacity; };

    struct ForwardIterator {
        friend struct FixedSizeGenerationalIdAllocator;

       public:
#ifndef TMAL_NO_STL
        typedef std::forward_iterator_tag iterator_category;
#endif
        typedef ptrdiff_t difference_type;
        typedef void* value_type;
        typedef void** pointer;
        typedef void*& reference;

       private:
        void* data = nullptr;
        int32_t size = 0;
        int32_t allocation_size = 0;

        inline ForwardIterator(void* data, int32_t size, int32_t allocation_size)
            : data(data), size(size), allocation_size(allocation_size) {}

       public:
        bool operator==(const ForwardIterator& other) const;
        bool operator!=(const ForwardIterator& other) const;
        void* operator*() const;
        ForwardIterator operator++();     // Prefix
        ForwardIterator operator++(int);  // Postfix
    };
    struct ConstForwardIterator {
        friend struct FixedSizeGenerationalIdAllocator;

       public:
#ifndef TMAL_NO_STL
        typedef std::forward_iterator_tag iterator_category;
#endif
        typedef ptrdiff_t difference_type;
        typedef const void* value_type;
        typedef const void** pointer;
        typedef const void*& reference;

       protected:
        void* data = nullptr;
        int32_t size = 0;
        int32_t allocation_size = 0;

        inline ConstForwardIterator(void* data, int32_t size, int32_t allocation_size)
            : data(data), size(size), allocation_size(allocation_size) {}

       public:
        bool operator==(const ConstForwardIterator& other) const;
        bool operator!=(const ConstForwardIterator& other) const;
        const void* operator*() const;
        ConstForwardIterator operator++();     // Prefix
        ConstForwardIterator operator++(int);  // Postfix
    };

    inline ForwardIterator begin() { return {data, size, allocation_size}; }
    inline ForwardIterator end() { return {(char*)data + size * allocation_size, 0, allocation_size}; }
    inline ConstForwardIterator begin() const { return {data, size, allocation_size}; }
    inline ConstForwardIterator end() const { return {(char*)data + size * allocation_size, 0, allocation_size}; }
    inline ConstForwardIterator cbegin() const { return {data, size, allocation_size}; }
    inline ConstForwardIterator cend() const { return {(char*)data + size * allocation_size, 0, allocation_size}; }

   private:
    typedef GenerationalId Header;

    void* base_from_id(uint32_t id);
    inline static Header* header_from_base(void* base) { return (Header*)base; }
    void* body_from_base(void* base);

    void* data = nullptr;
    int32_t size = 0;
    int32_t capacity = 0;
    int32_t free_index = -1;
    int32_t allocation_size = 0;
    int32_t allocation_alignment = 0;

    uint32_t pop();
    void push(uint32_t id);
};

/*!
 * @brief A fixed size allocator that returns generational ids instead of pointers.
 *
 * The generational ids allow for the reallocation of the underlying memory without invalidating the ids.
 * Best suited for dishing out opaque identifiers for resources that are managed by an encapsulated system.
 * Think of textures, entity ids and such.
 *
 * This is a type-safe version of FixedSizeGenerationalIdAllocator.
 */
template <class T>
struct FixedSizeTypedIdAllocator : private FixedSizeGenerationalIdAllocator {
   private:
    typedef FixedSizeGenerationalIdAllocator Base;

   public:
    FixedSizeTypedIdAllocator() = default;
    explicit FixedSizeTypedIdAllocator(int32_t initial_capacity) : Base(initial_capacity, sizeof(T), alignof(T)) {}
    FixedSizeTypedIdAllocator(FixedSizeTypedIdAllocator&&) = default;
    FixedSizeTypedIdAllocator& operator=(FixedSizeTypedIdAllocator&&) = default;

    using Base::create;
    using Base::destroy;

    /*!
     * @brief Returns the allocated pointer that corresponds to the given id.
     * Note that the returned pointer should not be stored as the pointer gets invalidated whenever create is called.
     * Use the pointer only for immediate lookups and calculations, only the id is meant to be stored.
     * @return Returns the allocated pointer that corresponds to the given id, or nullptr if the id is invalid.
     */
    T* data_from_id(uint32_t id) { return static_cast<T*>(Base::data_from_id(id)); }

    using Base::operator bool;

    struct ForwardIterator : private FixedSizeGenerationalIdAllocator::ForwardIterator {
        friend struct FixedSizeTypedIdAllocator;

       private:
        typedef FixedSizeGenerationalIdAllocator::ForwardIterator Base;

       public:
#ifndef TMAL_NO_STL
        using Base::iterator_category;
#endif
        using Base::difference_type;
        using Base::pointer;
        using Base::reference;
        using Base::value_type;

       private:
        inline ForwardIterator(void* data, int32_t size, int32_t allocation_size) : Base(data, size, allocation_size) {}

       public:
        using Base::operator==;
        using Base::operator!=;
        using Base::operator++;
        T* operator*() const { return static_cast<T*>(Base::operator*()); }
    };
    struct ConstForwardIterator : private FixedSizeGenerationalIdAllocator::ConstForwardIterator {
        friend struct FixedSizeTypedIdAllocator;

       private:
        typedef FixedSizeGenerationalIdAllocator::ConstForwardIterator Base;

       public:
#ifndef TMAL_NO_STL
        using Base::iterator_category;
#endif
        using Base::difference_type;
        using Base::pointer;
        using Base::reference;
        using Base::value_type;

       private:
        inline ConstForwardIterator(void* data, int32_t size, int32_t allocation_size)
            : Base(data, size, allocation_size) {}

       public:
        using Base::operator==;
        using Base::operator!=;
        using Base::operator++;
        const T* operator*() const { return static_cast<const T*>(Base::operator*()); }
    };

    ForwardIterator begin() { return {this->data, this->size, this->allocation_size}; }
    ForwardIterator end() { return {(char*)this->data + this->size * this->allocation_size, 0, this->allocation_size}; }
    ConstForwardIterator begin() const { return {this->data, this->size, this->allocation_size}; }
    ConstForwardIterator end() const {
        return {(char*)this->data + this->size * this->allocation_size, 0, this->allocation_size};
    }
    ConstForwardIterator cbegin() const { return {this->data, this->size, this->allocation_size}; }
    ConstForwardIterator cend() const {
        return {(char*)this->data + this->size * this->allocation_size, 0, this->allocation_size};
    }
};

/*!
 * @brief A wrapper around an allocator that allows thread-safe allocations using a mutex.
 */
template <class Allocator>
class LockingAllocator {
    RecursiveMutex mut;
    Allocator allocator;

   public:
    explicit LockingAllocator(Allocator&& underlying_allocator) : allocator(std::move(underlying_allocator)) {}
    LockingAllocator() = default;
    LockingAllocator(LockingAllocator&& other) = default;
    LockingAllocator& operator=(LockingAllocator&& other) = default;

    void* allocate_bytes(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT) {
        LockGuard guard{mut};
        return allocator.allocate_bytes(size, alignment);
    }
    void* reallocate_bytes(void* ptr, size_t oldSize, size_t newSize, size_t alignment = TM_DEFAULT_ALIGNMENT) {
        LockGuard guard{mut};
        return allocator.reallocate_bytes(ptr, oldSize, newSize, alignment);
    }
    bool reallocate_bytes_in_place(void* ptr, size_t oldSize, size_t newSize, size_t alignment = TM_DEFAULT_ALIGNMENT) {
        LockGuard guard{mut};
        return allocator.reallocate_bytes_in_place(ptr, oldSize, newSize, alignment);
    }
    void free_bytes(void* ptr, size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT) {
        LockGuard guard{mut};
        allocator.free_bytes(ptr, size, alignment);
    }
    bool owns(const void* ptr) const {
        LockGuard guard{mut};
        return allocator.owns(ptr);
    }
    bool is_most_recent_allocation(const void* ptr, size_t size) const {
        LockGuard guard{mut};
        return allocator.is_most_recent_allocation(ptr, size);
    }
    size_t remaining() const {
        LockGuard guard{mut};
        return allocator.remaining();
    }
    char* end() const {
        LockGuard guard{mut};
        return allocator.end();
    }
    void clear() {
        LockGuard guard{mut};
        allocator.clear();
    }

    /*!
     * @brief Returns the mutex used by this allocator.
     * Together with underlying() can be used for more efficient locking, when multiple allocations have to be done at once.
     * @return The mutex used for protecting the allocator.
     */
    const RecursiveMutex& mutex() const { return mutex; }
    /*!
     * @brief Returns the underlying allocator.
     * Together with mutex() can be used for more efficient locking, when multiple allocations have to be done at once.
     * @return The underlying allocator.
     */
    Allocator* underlying() { return &allocator; }

    static bool is_valid(const LockingAllocator* allocator) { return Allocator::is_valid(allocator->allocator); }
};

/*!
 * @brief An allocator that wraps TM_MALLOC.
 * This way all the allocation APIs can be used with the global heap.
 */
struct MallocAllocator {
    void* allocate_bytes(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    void* reallocate_bytes(void* ptr, size_t old_size, size_t new_size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    bool reallocate_bytes_in_place(void* ptr, size_t old_size, size_t new_size,
                                   size_t alignment = TM_DEFAULT_ALIGNMENT);
    void free_bytes(void* ptr, size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);

    inline static bool isValid(const MallocAllocator* /*allocator*/) { return true; }
};

/*!
 * @brief Helper to get sizeof(T) and alignof(T) when T can also be void.
 */
template <class T>
struct TypeSizes {
    static constexpr const size_t size = sizeof(T);
    static constexpr const size_t alignment = alignof(T);
};
template <>
struct TypeSizes<void> {
    static constexpr const size_t size = sizeof(char);
    static constexpr const size_t alignment = TM_DEFAULT_ALIGNMENT;
};

#ifndef TMAL_ALLOCATION_HELPERS_LEVEL
#define TMAL_ALLOCATION_HELPERS_LEVEL 0
#endif

#if TMAL_ALLOCATION_HELPERS_LEVEL == 1
// Debug Level
// Allocation helpers will log allocations.
namespace tmal_debug {
struct AllocationBookkeeping {
    struct Entry {
        const void* base;
        size_t size;
        size_t alignment;
        const void* allocator;
        const char* filename;
        size_t line;
    };

    Entry* entries;
    size_t sz;
    size_t cap;
    Mutex mutex;

    AllocationBookkeeping();
    ~AllocationBookkeeping();
    void addAllocation(const void* allocator, const void* base, size_t size, size_t alignment, const char* filename,
                       size_t line);
    void removeAllocation(const void* allocator, const void* base, size_t size, size_t alignment);
    void removeAllocationRange(const void* allocator, const void* start, const void* end);
};

}  // namespace tmal_debug
tmal_debug::AllocationBookkeeping* get_global_allocation_bookkeeping();

template <class T, class Allocator>
T* allocate_storage_impl(Allocator* allocator, const char* file, size_t line, size_t count = 1,
                         size_t alignment = TypeSizes<T>::alignment) {
        TM_ASSERT(isValidallocator));
        auto result = (T*)allocator->allocate_bytes(count * TypeSizes<T>::size, alignment);
        if (result) {
            const void* concrete_allocator = allocator;
            if constexpr (std::is_same<Allocator, PolymorphicAllocator>::value) {
                concrete_allocator = allocator->underlyingTypeErasedAllocator();
            }
            get_global_allocation_bookkeeping()->addAllocation(concrete_allocator, result, count * TypeSizes<T>::size,
                                                               alignment, file, line);
        }
        return result;
}
template <class T, class Allocator>
T* reallocate_storage_impl(Allocator* allocator, const char* file, size_t line, T* ptr, size_t old_count,
                           size_t new_count, size_t alignment = TypeSizes<T>::alignment) {
    tmal_ASSERT(is_valid(allocator));
    auto result =
        (T*)allocator->reallocate_bytes(ptr, old_count * TypeSizes<T>::size, new_count * TypeSizes<T>::size, alignment);
    if (result || new_count == 0) {
        const void* concrete_allocator = allocator;
        if constexpr (std::is_same<Allocator, PolymorphicAllocator>::value) {
            concrete_allocator = allocator->underlyingTypeErasedAllocator();
        }
        get_global_allocation_bookkeeping()->removeAllocation(concrete_allocator, ptr, old_count * TypeSizes<T>::size,
                                                              alignment);
        get_global_allocation_bookkeeping()->addAllocation(concrete_allocator, result, new_count * TypeSizes<T>::size,
                                                           alignment, file, line);
    }
    return result;
}
template <class T, class Allocator>
bool reallocate_storage_in_place_impl(Allocator* allocator, const char* file, size_t line, T* ptr, size_t old_count,
                                      size_t new_count, size_t alignment = TypeSizes<T>::alignment) {
    tmal_ASSERT(is_valid(allocator));
    auto result = allocator->reallocate_bytes_in_place(ptr, old_count * TypeSizes<T>::size,
                                                       new_count * TypeSizes<T>::size, alignment);
    if (result || new_count == 0) {
        const void* concrete_allocator = allocator;
        if constexpr (std::is_same<Allocator, PolymorphicAllocator>::value) {
            concrete_allocator = allocator->underlyingTypeErasedAllocator();
        }
        get_global_allocation_bookkeeping()->removeAllocation(concrete_allocator, ptr, old_count * TypeSizes<T>::size,
                                                              alignment);
        get_global_allocation_bookkeeping()->addAllocation(concrete_allocator, ptr, new_count * TypeSizes<T>::size,
                                                           alignment, file, line);
    }
    return result;
}

template <class T, class Allocator>
void free_storage_impl(Allocator* allocator, const char* file, size_t line, T* ptr, size_t count = 1,
                     size_t alignment = TypeSizes<T>::alignment) {
    tmal_ASSERT(is_valid(allocator));
    const void* concrete_allocator = allocator;
    if constexpr (std::is_same<Allocator, PolymorphicAllocator>::value) {
        concrete_allocator = allocator->underlyingTypeErasedAllocator();
    }
    get_global_allocation_bookkeeping()->removeAllocation(concrete_allocator, ptr, count * TypeSizes<T>::size,
                                                          alignment);
    allocator->free_bytes(ptr, count * TypeSizes<T>::size, alignment);
}

#define allocate_storage(allocator, type, ...) \
    ::tml::allocate_storage_impl<type>((allocator), __FILE__, __LINE__, ##__VA_ARGS__)
#define reallocate_storage(allocator, ptr, old_count, new_count, ...) \
    ::tml::reallocate_storage_impl((allocator), __FILE__, __LINE__, (ptr), (old_count), (new_count), ##__VA_ARGS__)
#define reallocate_storage_in_place(allocator, ptr, old_count, new_count, ...)                                 \
    ::tml::reallocate_storage_in_place_impl((allocator), __FILE__, __LINE__, (ptr), (old_count), (new_count), \
                                             ##__VA_ARGS__)
#define free_storage(allocator, ptr, ...) \
    ::tml::free_storage_impl((allocator), __FILE__, __LINE__, (ptr), ##__VA_ARGS__)

#elif TMAL_ALLOCATION_HELPERS_LEVEL == 0
// Normal Level
// Allocation helpers will just delegate.

template <class T, class Allocator>
T* allocate_storage_impl(Allocator* allocator, size_t count = 1, size_t alignment = TypeSizes<T>::alignment) {
    TM_ASSERT(Allocator::is_valid(allocator));
    return (T*)allocator->allocate_bytes(count * TypeSizes<T>::size, alignment);
}
template <class T, class Allocator>
T* reallocate_storage_impl(Allocator* allocator, T* ptr, size_t old_count, size_t new_count,
                           size_t alignment = TypeSizes<T>::alignment) {
    TM_ASSERT(Allocator::is_valid(allocator));
    return (T*)allocator->reallocate_bytes(ptr, old_count * TypeSizes<T>::size, new_count * TypeSizes<T>::size,
                                           alignment);
}
template <class T, class Allocator>
bool reallocate_storage_in_place_impl(Allocator* allocator, T* ptr, size_t old_count, size_t new_count,
                                      size_t alignment = TypeSizes<T>::alignment) {
    TM_ASSERT(Allocator::is_valid(allocator));
    return allocator->reallocate_bytes_in_place(ptr, old_count * TypeSizes<T>::size, new_count * TypeSizes<T>::size,
                                                alignment);
}

template <class T, class Allocator>
void free_storage_impl(Allocator* allocator, T* ptr, size_t count = 1, size_t alignment = TypeSizes<T>::alignment) {
    TM_ASSERT(Allocator::is_valid(allocator));
    allocator->free_bytes(ptr, count * TypeSizes<T>::size, alignment);
}

#define allocate_storage(allocator, type, ...) ::tml::allocate_storage_impl<type>((allocator), ##__VA_ARGS__)
#define reallocate_storage(allocator, ptr, old_count, new_count, ...) \
    ::tml::reallocate_storage_impl((allocator), (ptr), (old_count), (new_count), ##__VA_ARGS__)
#define reallocate_storage_in_place(allocator, ptr, old_count, new_count, ...) \
    ::tml::reallocate_storage_in_place_impl((allocator), (ptr), (old_count), (new_count), ##__VA_ARGS__)
#define free_storage(allocator, ptr, ...) ::tml::free_storage_impl((allocator), (ptr), ##__VA_ARGS__)

#endif

#undef TMAL_ALLOCATION_HELPERS_LEVEL

template <class T, class Allocator>
T* create_default_init(Allocator* allocator, size_t count = 1, size_t alignment = alignof(T)) {
    T* storage = allocate_storage(allocator, T, count, alignment);
    if (storage) {
        for (size_t i = 0; i < count; ++i) {
            TM_PLACEMENT_NEW(&storage[i]) T();
        }
    }
    return storage;
}

#ifndef TMAL_NO_STL
template <class T, class Allocator, class... Args, ::std::enable_if_t<!::std::is_array_v<T>, int> = 0>
T* create(Allocator* allocator, Args&&... args) {
    T* storage = allocate_storage(allocator, T, 1, alignof(T));
    if (storage) {
        TM_PLACEMENT_NEW(storage) T(::std::forward<Args>(args)...);
    }
    return storage;
}

template <class T, class Allocator, ::std::enable_if_t<::std::is_array_v<T>&& ::std::extent_v<T> == 0, int> = 0>
::std::remove_extent_t<T>* create(Allocator* allocator, size_t count, size_t alignment = alignof(T)) {
    auto storage = allocate_storage(allocator, ::std::remove_extent_t<T>, count, alignment);
    if (storage) {
        for (size_t i = 0; i < count; ++i) {
            TM_PLACEMENT_NEW(&storage[i]) ::std::remove_pointer_t<::std::remove_extent_t<T>>();
        }
    }
    return storage;
}

template <class T, class... Args, ::std::enable_if_t<::std::extent_v<T> != 0, int> = 0>
void create(Args&&...) = delete;
#endif

template <class T, class Allocator>
void destroy(Allocator* allocator, T* ptr, size_t count = 1, size_t alignment = alignof(T)) {
    if (ptr) {
        for (size_t i = 0; i < count; ++i) {
            ptr[i].~T();
        }
    }
    free_storage(allocator, ptr, count, alignment);
}

#if 0
/*!
 * @brief Classes similar to
 */
template <class T, class Allocator>
class UniquePtr {
    T* ptr = nullptr;
    Allocator* allocator = nullptr;

   public:
    UniquePtr() = default;
    UniquePtr(UniquePtr&& other) : ptr(other.ptr), allocator(other.allocator) {
        other.ptr = nullptr;
        other.allocator = nullptr;
    }
    UniquePtr& operator=(UniquePtr&& other) {
        if (this != &other) swap(static_cast<UniquePtr&&>(other));
        return *this;
    }
    ~UniquePtr() { reset(); }

    T* get() { return ptr; }
    T* release() {
        T* result = ptr;
        ptr = nullptr;
        allocator = nullptr;
        return result;
    }
    void reset() {
        destroy(allocator, ptr);
        ptr = nullptr;
        allocator = nullptr;
    }
    T* operator*() { return ptr; }
    const T* operator*() const { return ptr; }
    T* operator->() { return ptr; }
    const T* operator->() const { return ptr; }

    explicit operator bool() const { return ptr != nullptr; }

    void swap(UniquePtr& other) {
        auto temp_ptr = ptr;
        ptr = other.ptr;
        other.ptr = temp_ptr;

        auto temp_allocator = allocator;
        allocator = other.allocator;
        other.allocator = temp_allocator;
    }
    friend void swap(UniquePtr& a, UniquePtr& b) { a.swap(b); }
};

template <class T, class Allocator>
class UniquePtr<T[], Allocator> {
    T* ptr = nullptr;
    size_t count = 0;
    Allocator* allocator = nullptr;

   public:
    UniquePtr() = default;
    UniquePtr(T* ptr, size_t count, Allocator* allocator) : ptr(ptr), count(count), allocator(allocator) {}
    UniquePtr(UniquePtr&& other) : ptr(other.ptr), count(other.count), allocator(other.allocator) {
        other.ptr = nullptr;
        other.count = 0;
        other.allocator = nullptr;
    }
    UniquePtr& operator=(UniquePtr&& other) {
        if (this != &other) swap(other);
        return *this;
    }
    ~UniquePtr() { reset(); }

    T* get() { return ptr; }
    T* release() {
        T* result = ptr;
        ptr = nullptr;
        allocator = nullptr;
        return result;
    }
    void reset() {
        destroy(allocator, ptr);
        ptr = nullptr;
        allocator = nullptr;
    }

    size_t size() const { return count; }

    T& operator[](int index) {
        TM_ASSERT(ptr);
        TM_ASSERT(index >= 0 && (size_t)index < count);
        return ptr[index];
    }
    T& operator[](size_t index) {
        TM_ASSERT(ptr);
        TM_ASSERT(index < count);
        return ptr[index];
    }
    const T& operator[](int index) const {
        TM_ASSERT(ptr);
        TM_ASSERT(index >= 0 && (size_t)index < count);
        return ptr[index];
    }
    const T& operator[](size_t index) const {
        TM_ASSERT(ptr);
        TM_ASSERT(index < count);
        return ptr[index];
    }

    explicit operator bool() const { return ptr != nullptr; }

    void swap(UniquePtr& other) {
        auto temp_ptr = ptr;
        ptr = other.ptr;
        other.ptr = temp_ptr;

        auto temp_count = count;
        count = other.count;
        other.count = temp_count;

        auto temp_allocator = allocator;
        allocator = other.allocator;
        other.allocator = temp_allocator;
    }

    friend void swap(UniquePtr& a, UniquePtr& b) { a.swap(b); }
};

#endif

/* std::unique_ptr support */
#ifndef TMAL_NO_STL

/*!
 * @brief Custom deleter for std::unique_ptr for single objects.
 */
template <class T, class Allocator>
struct UniquePtrDeleter {
    Allocator* allocator;

    void operator()(T* ptr) { destroy(allocator, ptr, 1, alignof(T)); }
};
/*!
 * @brief Custom deleter for std::unique_tr for arrays.
 */
template <class Allocator>
struct UniquePtrArrayDeleter {
    size_t count;
    Allocator* allocator;

    template<class T>
    void operator()(T* ptr) { destroy(allocator, ptr, count, alignof(T)); }
};

/*!
 * @brief New typenames for std::unique_ptr that use the custom deleters.
 */
template <class T, class Allocator>
using unique_ptr = ::std::unique_ptr<T, UniquePtrDeleter<T, Allocator>>;
template <class T, class Allocator>
using unique_ptr_array = ::std::unique_ptr<T, UniquePtrArrayDeleter<Allocator>>;

/*!
 * @brief Allocates and constructs a single object and returns it as a unique_ptr.
 * @param allocator[IN] The allocator to use.
 * @param args[IN] Variadic arguments to be passed to the constructor.
 * @return Returns a unique_ptr to the allocated object.
 */
template <class T, class Allocator, class... Args, ::std::enable_if_t<!::std::is_array_v<T>, int> = 0>
unique_ptr<T, Allocator> make_unique(Allocator* allocator, Args&&... args) {
    auto ptr = create<T>(allocator, ::std::forward<Args>(args)...);
    return {ptr, {allocator}};
}

/*!
 * @brief Allocates and constructs an array and returns it as a unique_ptr.
 * @param allocator[IN] The allocator to use.
 * @param count[IN] How many elements to allocate.
 * @return Returns a unique_ptr to the allocated array.
 * @example auto int_array = tml::make_unique<int[]>(allocator, 2);
 */
template <class T, class Allocator, ::std::enable_if_t<::std::is_array_v<T>&& ::std::extent_v<T> == 0, int> = 0>
unique_ptr_array<T, Allocator> make_unique(Allocator* allocator, size_t count) {
    auto ptr = create<T>(allocator, count);
    return unique_ptr_array<T, Allocator>(ptr, {count, allocator});
}

template <class T, class Allocator, class... Args, ::std::enable_if_t<::std::extent_v<T> != 0, int> = 0>
void make_unique(Allocator* allocator, Args&&...) = delete;
#endif

}  // namespace tml

#endif /* !defined(_TM_ALLOCATOR_H_INCLUDED_F4741AD2_9668_4A0F_845F_6204A55B5B14) */

// Implementation

#ifdef TM_ALLOCATOR_IMPLEMENTATION

#ifndef TM_UNREFERENCED_PARAM
	#define TM_UNREFERENCED_PARAM(x) ((void)(x))
	#define TM_UNREFERENCED(x) ((void)(x))
    #define TM_MAYBE_UNUSED(x) ((void)(x))
#endif

bool tml::is_power_of_two(size_t value) { return (value && !(value & (value - 1))); }
bool tml::is_pointer_aligned(const void* ptr, size_t alignment) { return ((uintptr_t)ptr % alignment) == 0; }
bool tml::is_pointer_aligned(uintptr_t ptr, size_t alignment) { return ((uintptr_t)ptr % alignment) == 0; }
size_t tml::alignment_offset(const void* ptr, size_t alignment) {
    TM_ASSERT(alignment != 0 && is_power_of_two(alignment));
    size_t offset = (alignment - ((uintptr_t)ptr)) & (alignment - 1);
    TM_ASSERT(is_pointer_aligned((char*)ptr + offset, alignment));
    return offset;
}
size_t tml::alignment_offset(uintptr_t ptr, size_t alignment) {
    TM_ASSERT(alignment != 0 && is_power_of_two(alignment));
    size_t offset = (alignment - ptr) & (alignment - 1);
    TM_ASSERT(is_pointer_aligned(ptr + offset, alignment));
    return offset;
}

#if defined(_WIN32) && defined(TMAL_HAS_WINDOWS_H_INCLUDED)
    // We can use VirtualAlloc.
    void* tml::tmal_mmap(size_t size) {
        return VirtualAlloc(/*start_address=*/nullptr, (SIZE_T)size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }
    bool tml::tmal_munmap(void* ptr, size_t size) {
        // Second parameter to VirtualFree must be 0 for MEM_RELEASE.
        if (ptr) return VirtualFree(ptr, 0, MEM_RELEASE) != 0;
        return size == 0;
    }
    size_t tml::tmal_get_mmap_granularity() {
        // TODO: Cache the allocation granularity?
        SYSTEM_INFO info = {};
        GetSystemInfo(&info);
        return (size_t)info.dwAllocationGranularity;
    }

#elif defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
    void* tml::tmal_mmap(size_t size) {
        void* mmap_result =
            mmap(/*start_address=*/nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, /*fd=*/0, /*offset=*/0);
        if (mmap_result == MAP_FAILED) return nullptr;
        return mmap_result;
    }
    bool tml::tmal_munmap(void* ptr, size_t size) { return (ptr) ? (munmap(ptr, size) == 0) : (size == 0); }
    size_t tml::tmal_get_mmap_granularity() {
        long page_size = sysconf(_SC_PAGESIZE);
        if (page_size < 0) return 4096;
        return (size_t)page_size;
    }

#else
    // Fall back on using malloc/free.
    void* tml::tmal_mmap(size_t size) { return TM_MALLOC(size, 4096); }
    bool tml::tmal_munmap(void* ptr, size_t size) {
        TM_MAYBE_UNUSED(size);
        TM_FREE(ptr, size, 4096);
        return true;
    }
    size_t tml::tmal_get_mmap_granularity() { return 4096; }

#endif

tml::LockGuard::LockGuard(const tml::RecursiveMutex& mut) : mutex(&mut) { mutex->lock_throws(); }
bool tml::LockGuard::lock(const tml::RecursiveMutex& mut) {
    TM_ASSERT(!this->mutex);
    this->mutex = &mut;
    return mut.lock();
}
tml::LockGuard::~LockGuard() {
    if (mutex) mutex->unlock();
}

tml::RecursiveMutex::RecursiveMutex(RecursiveMutex&& other) : internal(other.internal) { other.internal = nullptr; }
tml::RecursiveMutex& tml::RecursiveMutex::operator=(RecursiveMutex&& other) {
    if (this != &other) {
        auto temp = internal;
        internal = other.internal;
        other.internal = temp;
    }
    return *this;
}

#ifndef TMAL_NO_STL
    #include <mutex>
    tml::RecursiveMutex::RecursiveMutex() {
        internal = TM_MALLOC(sizeof(std::recursive_mutex), sizeof(void*));
        if (internal) {
            TM_PLACEMENT_NEW(internal) std::recursive_mutex();
        }
    }
    tml::RecursiveMutex::~RecursiveMutex() { destroy(); }
    bool tml::RecursiveMutex::lock() const {
        if (!internal) return false;
        try {
            static_cast<std::recursive_mutex*>(internal)->lock();
            return true;
        } catch (std::exception&) {
            return false;
        }
    }
    void tml::RecursiveMutex::lock_throws() const {
        if (!internal) throw std::bad_alloc();
        static_cast<std::recursive_mutex*>(internal)->lock();
    }
    bool tml::RecursiveMutex::try_lock() const {
        if (!internal) return false;
        try {
            return static_cast<std::recursive_mutex*>(internal)->try_lock();
        } catch (std::exception&) {
            return false;
        }
    }
    bool tml::RecursiveMutex::unlock() const {
        if (!internal) return false;
        try {
            static_cast<std::recursive_mutex*>(internal)->unlock();
            return true;
        } catch (std::exception&) {
            return false;
        }
    }
    void tml::RecursiveMutex::destroy() {
        if (internal) {
            using std::recursive_mutex;
            static_cast<recursive_mutex*>(internal)->~recursive_mutex();
            TM_FREE(internal, sizeof(recursive_mutex), sizeof(void*));
            internal = nullptr;
        }
    }

#elif defined(_WIN32)
    tml::RecursiveMutex::RecursiveMutex() {
        CRITICAL_SECTION* cs = (CRITICAL_SECTION*)TM_MALLOC(sizeof(CRITICAL_SECTION), sizeof(void*));
        if (cs) InitializeCriticalSection(cs);
        internal = cs;
    }
    tml::RecursiveMutex::~RecursiveMutex() { destroy(); }
    bool tml::RecursiveMutex::lock() const {
        if (!internal) return false;
        EnterCriticalSection((CRITICAL_SECTION*)internal);
        return true;
    }
    void tml::RecursiveMutex::lock_throws() const {
        if (!internal) {
#ifndef TMAL_NO_STL
            throw std::bad_alloc();
#else
            throw nullptr;
#endif
        }
        EnterCriticalSection((CRITICAL_SECTION*)internal);
    }
    bool tml::RecursiveMutex::try_lock() const {
        if (!internal) return false;
        return TryEnterCriticalSection((CRITICAL_SECTION*)internal);
    }
    bool tml::RecursiveMutex::unlock() const {
        if (!internal) return false;
        LeaveCriticalSection((CRITICAL_SECTION*)internal);
        return true;
    }
    void tml::RecursiveMutex::destroy() {
        if (internal) {
            DeleteCriticalSection((CRITICAL_SECTION*)internal);
            TM_FREE(internal, sizeof(CRITICAL_SECTION), sizeof(void*));
            internal = nullptr;
        }
    }

#else
    // Assume a platform with pthread
    #include <pthread.h>
    tml::RecursiveMutex::RecursiveMutex() {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_t* mutex = (pthread_mutex_t*)TM_MALLOC(sizeof(pthread_mutex_t), sizeof(void*));
        int result = pthread_mutex_init(mutex, &attr);
        if (result != 0) {
            TM_FREE(mutex, sizeof(pthread_mutex_t), sizeof(void*));
        }
        pthread_mutexattr_destroy(&attr);
        internal = mutex;
    }
    tml::RecursiveMutex::~RecursiveMutex() { destroy(); }
    bool tml::RecursiveMutex::lock() const {
        if (!internal) return false;
        return pthread_mutex_lock((pthread_mutex_t*)internal) == 0;
    }
    void tml::RecursiveMutex::lock_throws() const {
        if (!internal) {
#ifndef TMAL_NO_STL
            throw std::bad_alloc();
#else
            throw nullptr;
#endif
        }
        int result = pthread_mutex_lock((pthread_mutex_t*)internal);
        if (result != 0) {
#ifndef TMAL_NO_STL
            throw std::system_error(std::make_error_code((std::errc)result));
#else
            throw nullptr;
#endif
        }
    }
    bool tml::RecursiveMutex::try_lock() const {
        if (!internal) return false;
        return pthread_mutex_trylock((pthread_mutex_t*)internal) == 0;
    }
    bool tml::RecursiveMutex::unlock() const {
        if (!internal) return false;
        return pthread_mutex_unlock((pthread_mutex_t*)internal) == 0;
    }
    void tml::RecursiveMutex::destroy() {
        if (!internal) return;
        if (pthread_mutex_destroy((pthread_mutex_t*)internal) == 0) {
            TM_FREE(internal, sizeof(pthread_mutex_t), sizeof(void*));
            internal = nullptr;
            return true;
        }
        TM_ASSERT(0 && "Undefined behavior, mutex is still locked.");
    }
#endif

tml::StackAllocator::StackAllocator(void* ptr, size_t capacity) : p((char*)ptr), cap(capacity) {
    TM_ASSERT(ptr || capacity == 0);
}

void* tml::StackAllocator::allocate_bytes(size_t size, size_t alignment /* = DEF_ALIGN*/) {
    if (!size) return nullptr;

    auto offset = alignment_offset(end(), alignment);
    if (sz + offset + size > cap) return nullptr;

    auto result = end() + offset;
    sz += offset + size;
    TM_ASSERT(is_pointer_aligned(result, alignment));
    last_popped_alignment = 1;
    return result;
}
void* tml::StackAllocator::reallocate_bytes(void* ptr, size_t old_size, size_t new_size,
                                            size_t alignment /* = DEF_ALIGN*/) {
    TM_ASSERT(is_valid(this));
    if (is_most_recent_allocation(ptr, old_size)) {
        if (sz + (new_size - old_size) > cap) return nullptr;
        sz += new_size - old_size;
        return ptr;
    } else if (new_size < old_size) {
        // no reallocation needed, but returning ptr here means we leak (old_size - new_size) bytes
        return ptr;
    }

    auto result = allocate_bytes(new_size, alignment);
    if (result) {
        TM_MEMCPY(result, ptr, (old_size < new_size) ? (old_size) : (new_size));
        // free_bytes(ptr, old_size, alignment); // this free will fail, basically we are leaking old_size bytes
    }
    return result;
}
bool tml::StackAllocator::reallocate_bytes_in_place(void* ptr, size_t old_size, size_t new_size,
                                                    size_t /*alignment  = DEF_ALIGN*/) {
    TM_ASSERT(p);
    if (is_most_recent_allocation(ptr, old_size)) {
        if (sz + (new_size - old_size) <= cap) {
            sz += new_size - old_size;
            return true;
        }
    }
    return false;
}
void tml::StackAllocator::free_bytes(void* ptr, size_t size, size_t alignment /* = DEF_ALIGN*/) {
    TM_ASSERT(is_valid(this));

    // if this assertion triggers, ptr is being freed using a wrong allocator
    TM_ASSERT(!ptr || owns(ptr));

    // this assertion triggers if free isn't called in reverse order of allocation or there were interlocking
    // allocations/free's with differing alignments
    TM_ASSERT(!ptr || size == 0 || is_most_recent_allocation(ptr, size));
    if (ptr && is_most_recent_allocation(ptr, size)) {
        sz -= size + alignment_offset((const char*)ptr + size, last_popped_alignment);
        last_popped_alignment = alignment;
    }
}
bool tml::StackAllocator::is_most_recent_allocation(const void* ptr, size_t size) const {
    const char* end_ptr = (const char*)ptr + size;
    // equality on arbitrary pointers is specified behavior
    return end_ptr + alignment_offset(end_ptr, last_popped_alignment) == end();
}
bool tml::StackAllocator::owns(const void* ptr) const {
    if (!ptr) return true;
    const char* storage = (const char*)ptr;
    // On platforms/compilers where this library is supported this comparison is fine even though technically its
    // unspecified behaviour, so we might want to use std::less<> or cast to uintptr_t later.
    auto result = storage >= p && storage < p + sz;
    TM_ASSERT(result || !(storage >= p && storage < p + cap));
    return result;
}
size_t tml::StackAllocator::remaining(size_t alignment) const {
    TM_ASSERT(is_valid(this));
    auto result = cap - sz;
    auto offset = alignment_offset(end(), alignment);
    if (result >= offset) {
        result -= offset;
    } else {
        // no room to align
        result = 0;
    }
    return result;
}
void tml::StackAllocator::clear() {
    sz = 0;
    last_popped_alignment = 1;
}
void tml::StackAllocator::set_state(StackAllocator::StateSnapshot snapshot) {
    sz = snapshot.sz;
    last_popped_alignment = snapshot.last_popped_alignment;
}

tml::DynamicStackAllocator::DynamicStackAllocator(size_t capacity) {
    if (capacity) {
        p = (char*)TM_MALLOC(capacity, TM_DEFAULT_ALIGNMENT);
        if (p) cap = capacity;
    }
}
tml::DynamicStackAllocator::DynamicStackAllocator(DynamicStackAllocator&& other) {
    p = other.p;
    sz = other.sz;
    cap = other.cap;
    last_popped_alignment = other.last_popped_alignment;
    other.p = nullptr;
    other.sz = 0;
    other.cap = 0;
    other.last_popped_alignment = 1;
}
tml::DynamicStackAllocator& tml::DynamicStackAllocator::operator=(DynamicStackAllocator&& other) {
    if (this != &other) {
        if (p) {
            TM_FREE(p, capacity, TM_DEFAULT_ALIGNMENT);
            p = nullptr;
        }
        p = other.p;
        sz = other.sz;
        cap = other.cap;
        last_popped_alignment = other.last_popped_alignment;
        other.p = nullptr;
        other.sz = 0;
        other.cap = 0;
        other.last_popped_alignment = 1;
    }
    return *this;
}
tml::DynamicStackAllocator::~DynamicStackAllocator() {
    if (p) {
        TM_FREE(p, capacity, TM_DEFAULT_ALIGNMENT);
        p = nullptr;
    }
}

tml::MonotonicAllocator::MonotonicAllocator() : block_size(tmal_get_mmap_granularity()) {
    allocators = (StackAllocator*)TM_MALLOC(sizeof(StackAllocator), TM_DEFAULT_ALIGNMENT);
    if (allocators) {
        current = 0;
        capacity = 1;
    }
}
tml::MonotonicAllocator::MonotonicAllocator(MonotonicAllocator&& other)
    : allocators(other.allocators),
      current(other.current),
      capacity(other.capacity),
      block_size(other.block_size),
      leak_memory(other.leak_memory) {
    other.allocators = nullptr;
    other.current = 0;
    other.capacity = 0;
    other.block_size = 0;
    other.leak_memory = false;
}
tml::MonotonicAllocator& tml::MonotonicAllocator::operator=(MonotonicAllocator&& other) {
    if (this != &other) {
        if (!leak_memory && allocators) {
            TM_FREE(allocators, capacity * sizeof(StackAllocator), TM_DEFAULT_ALIGNMENT);
        }
        allocators = other.allocators;
        current = other.current;
        capacity = other.capacity;
        block_size = other.block_size;
        leak_memory = other.leak_memory;
        other.allocators = nullptr;
        other.current = 0;
        other.capacity = 0;
        other.block_size = 0;
        other.leak_memory = false;
    }
    return *this;
}
tml::MonotonicAllocator::~MonotonicAllocator() {
    if (!leak_memory && allocators) {
        TM_FREE(allocators, capacity * sizeof(StackAllocator), TM_DEFAULT_ALIGNMENT);
        allocators = nullptr;
    }
}

void* tml::MonotonicAllocator::allocate_bytes_throws(size_t size, size_t alignment /*= TM_DEFAULT_ALIGNMENT*/) {
    auto result = allocate_bytes(size, alignment);
    if (!result) {
#ifndef TMAL_NO_STL
        throw std::bad_alloc();
#else
        throw nullptr;
#endif
    }
    return result;
}
void* tml::MonotonicAllocator::allocate_bytes(size_t size, size_t alignment /*= TM_DEFAULT_ALIGNMENT*/) {
    TM_ASSERT(allocators);
    if (void* result = allocators[current].allocate_bytes(size, alignment)) return result;

    auto new_capacity = capacity + 1;
    auto new_allocators = TM_REALLOC(allocators, capacity * sizeof(StackAllocator), TM_DEFAULT_ALIGNMENT,
                                     new_capacity * sizeof(StackAllocator), TM_DEFAULT_ALIGNMENT);
    if (!new_allocators) return nullptr;
    allocators = (StackAllocator*)new_allocators;
    capacity = new_capacity;

    auto allocator_index = new_capacity - 1;
    allocators[allocator_index] = {};
    size_t allocation_size = block_size;

    if (size <= block_size) {
        // Allocation size fits in a single block.
        current = allocator_index;
    } else {
        // Allocation size is more than the block size of a single monotonic allocator.
        // So we create memory just for this single allocation without making the resulting allocator the current
        // one, since it is immediately empty.
        allocation_size = size;
    }

    void* block = tmal_mmap(allocation_size);
    if (!block) return nullptr;
    allocators[allocator_index] = {block, allocation_size};
    return allocators[allocator_index].allocate_bytes(size, alignment);
}
bool tml::MonotonicAllocator::owns(const void* ptr) const {
    if (!ptr) return true;
    for (int i = 0, count = capacity; i < count; ++i) {
        if (allocators[i].owns(ptr)) return true;
    }
    return false;
}

tml::StackAllocator* tml::MonotonicAllocator::current_stack_allocator() {
    TM_ASSERT(current >= 0 && current < capacity);
    return &allocators[current];
}

tml::StackAllocatorGuard::StackAllocatorGuard() : allocator(nullptr), state{} {}
tml::StackAllocatorGuard::StackAllocatorGuard(StackAllocator* allocator)
    : allocator(allocator), state(allocator->get_state()) {}
tml::StackAllocatorGuard::StackAllocatorGuard(StackAllocatorGuard&& other)
    : allocator(other.allocator), state(other.state) {
    other.dismiss();
}
tml::StackAllocatorGuard& tml::StackAllocatorGuard::operator=(StackAllocatorGuard&& other) {
    if (&other != this) {
        if (allocator) allocator->set_state(state);
        allocator = other.allocator;
        state = other.state;
        other.dismiss();
    }
    return *this;
}
tml::StackAllocatorGuard::~StackAllocatorGuard() {
    if (allocator) {
        allocator->set_state(state);
        allocator = nullptr;
    }
}

void tml::StackAllocatorGuard::dismiss() { allocator = nullptr; }

tml::FixedSizeIdAllocator::FixedSizeIdAllocator(int32_t initial_element_capacity, int32_t element_size_in_bytes,
                                                int32_t element_alignment)
    : allocation_size(element_size_in_bytes), allocation_alignment(element_alignment) {
    TM_ASSERT(allocation_size >= (int32_t)sizeof(int32_t));
    if (allocation_alignment < (int32_t)sizeof(int32_t)) allocation_alignment = (int32_t)sizeof(int32_t);
    if (initial_element_capacity <= 0) return;

    data = TM_MALLOC(allocation_size * initial_element_capacity, allocation_alignment);
    if (data) capacity = initial_element_capacity;
}
tml::FixedSizeIdAllocator::~FixedSizeIdAllocator() {
    if (data) {
        TM_FREE(data, allocation_size * capacity, allocation_alignment);
        data = nullptr;
    }
}
tml::FixedSizeIdAllocator::FixedSizeIdAllocator(FixedSizeIdAllocator&& other)
    : data(other.data),
      size(other.size),
      capacity(other.capacity),
      free_index(other.free_index),
      allocation_size(other.allocation_size),
      allocation_alignment(other.allocation_alignment) {
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
    other.free_index = -1;
}
tml::FixedSizeIdAllocator& tml::FixedSizeIdAllocator::operator=(FixedSizeIdAllocator&& other) {
    if (this != &other) {
        if (data) {
            TM_FREE(data, allocation_size * capacity, allocation_alignment);
        }
        data = other.data;
        size = other.size;
        capacity = other.capacity;
        free_index = other.free_index;
        allocation_size = other.allocation_size;
        allocation_alignment = other.allocation_alignment;

        other.data = nullptr;
    }
    return *this;
}

uint32_t tml::FixedSizeIdAllocator::create() {
    TM_ASSERT(data);
    if (auto id = pop()) return id;

    if (size >= capacity) {
        auto new_capacity = next_capacity(capacity);
        auto new_data = TM_REALLOC(data, capacity * allocation_size, allocation_size, new_capacity * allocation_size,
                                   allocation_alignment);
        if (new_data) {
            data = new_data;
            capacity = (int32_t)new_capacity;
        }
    }

    if (size < capacity) {
        size++;
        TM_ASSERT(size > 0);
        return (uint32_t)size;
    }
    return 0;
}
void tml::FixedSizeIdAllocator::destroy(uint32_t id) {
    if (data_from_id(id)) push(id);
}
void* tml::FixedSizeIdAllocator::data_from_id(uint32_t id) {
    if (id > 0 && (int32_t)(id - 1) < size) return (char*)data + ((id - 1) * allocation_size);
    return nullptr;
}
uint32_t tml::FixedSizeIdAllocator::pop() {
    if (free_index >= 0) {
        TM_ASSERT(free_index < size);
        auto result = (uint32_t)(free_index + 1);
        free_index = *((int32_t*)((char*)data + free_index * allocation_size));
        return result;
    }
    return 0;
}
void tml::FixedSizeIdAllocator::push(uint32_t id) {
    TM_ASSERT(id > 0);
    auto index = id - 1;
    auto new_free = (int32_t*)((char*)data + index * allocation_size);
    *new_free = free_index;
    free_index = index;
}

tml::FixedSizeGenerationalIdAllocator::FixedSizeGenerationalIdAllocator(int32_t initial_element_capacity,
                                                                        int32_t element_size_in_bytes,
                                                                        int32_t element_alignment)
    : allocation_size(element_size_in_bytes), allocation_alignment(element_alignment) {
    if (allocation_alignment < (int32_t)sizeof(int32_t)) allocation_alignment = (int32_t)sizeof(int32_t);
    // Increase the allocation size by the size of the header.
    allocation_size += (int32_t)(sizeof(uint32_t) + alignment_offset(sizeof(uint32_t), element_alignment));

    if (initial_element_capacity <= 0) return;

    data = TM_MALLOC(allocation_size * initial_element_capacity, allocation_alignment);
    if (data) capacity = initial_element_capacity;
}
tml::FixedSizeGenerationalIdAllocator::~FixedSizeGenerationalIdAllocator() {
    if (data) {
        TM_FREE(data, allocation_size * capacity, allocation_alignment);
        data = nullptr;
    }
}
tml::FixedSizeGenerationalIdAllocator::FixedSizeGenerationalIdAllocator(FixedSizeGenerationalIdAllocator&& other)
    : data(other.data),
      size(other.size),
      capacity(other.capacity),
      free_index(other.free_index),
      allocation_size(other.allocation_size),
      allocation_alignment(other.allocation_alignment) {
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
    other.free_index = -1;
}
tml::FixedSizeGenerationalIdAllocator& tml::FixedSizeGenerationalIdAllocator::operator=(
    FixedSizeGenerationalIdAllocator&& other) {
    if (this != &other) {
        if (data) {
            TM_FREE(data, allocation_size * capacity, allocation_alignment);
        }
        data = other.data;
        size = other.size;
        capacity = other.capacity;
        free_index = other.free_index;
        allocation_size = other.allocation_size;
        allocation_alignment = other.allocation_alignment;

        other.data = nullptr;
    }
    return *this;
}
uint32_t tml::FixedSizeGenerationalIdAllocator::create() {
    TM_ASSERT(data);
    if (auto id = pop()) return id;

    if (size >= capacity) {
        auto new_capacity = next_capacity(capacity);
        auto new_data = TM_REALLOC(data, capacity * allocation_size, allocation_size, new_capacity * allocation_size,
                                   allocation_alignment);
        if (new_data) {
            data = new_data;
            capacity = (int32_t)new_capacity;
        }
    }

    if (size < capacity) {
        TM_ASSERT(size != INT32_MAX);
        TM_ASSERT(size > 0);
        auto header = (Header*)((char*)data + size * allocation_size);
        *header = Header::make(/*generation=*/0, size, /*occupied=*/true);
        size++;
        return header->bits;
    }
    return 0;
}
void tml::FixedSizeGenerationalIdAllocator::destroy(uint32_t id) {
    if (base_from_id(id)) push(id);
}
void* tml::FixedSizeGenerationalIdAllocator::data_from_id(uint32_t id) {
    if (auto base = base_from_id(id)) return body_from_base(base);
    return nullptr;
}
uint32_t tml::FixedSizeGenerationalIdAllocator::pop() {
    if (free_index > 0) {
        TM_ASSERT(free_index <= size);
        auto header = (Header*)((char*)data + free_index * allocation_size);
        auto result = Header::make(header->generation(), (uint32_t)free_index, /*occupied=*/true);
        free_index = header->id();
        return result.bits;
    }
    return 0;
}
void tml::FixedSizeGenerationalIdAllocator::push(uint32_t id) {
    auto index = Header::id(id);
    auto new_free = (Header*)((char*)data + index * allocation_size);
    TM_ASSERT(new_free->bits == id);
    *new_free = Header::make(new_free->generation() + 1, free_index, /*occupied=*/false);
    free_index = index;
}
void* tml::FixedSizeGenerationalIdAllocator::base_from_id(uint32_t id) {
    if (!Header::occupied(id)) return nullptr;
    auto index = Header::id(id);
    if ((int32_t)index >= size) return nullptr;
    auto result = (char*)data + (Header::id(id) * allocation_size);
    auto header = (Header*)result;
    if (header->bits != id) return nullptr;
    return result;
}
void* tml::FixedSizeGenerationalIdAllocator::body_from_base(void* header) {
    auto end_of_header = (char*)header + sizeof(uint32_t);
    return end_of_header + alignment_offset(end_of_header, allocation_alignment);
}

bool tml::FixedSizeGenerationalIdAllocator::ForwardIterator::operator==(const ForwardIterator& other) const {
    return data == other.data && size == other.size && allocation_size == other.allocation_size;
}
bool tml::FixedSizeGenerationalIdAllocator::ForwardIterator::operator!=(const ForwardIterator& other) const {
    return data != other.data || size != other.size || allocation_size != other.allocation_size;
}
void* tml::FixedSizeGenerationalIdAllocator::ForwardIterator::operator*() const { return data; }
tml::FixedSizeGenerationalIdAllocator::ForwardIterator tml::FixedSizeGenerationalIdAllocator::ForwardIterator::
operator++() {
    // Prefix increment.
    TM_ASSERT(size);
    char* p = (char*)data + allocation_size;
    --size;
    do {
        auto header = (Header*)p;
        if (header->occupied()) break;
        p += allocation_size;
        --size;
    } while (size);
    return *this;
}
tml::FixedSizeGenerationalIdAllocator::ForwardIterator tml::FixedSizeGenerationalIdAllocator::ForwardIterator::
operator++(int) {
    // Postfix increment.
    auto previous = *this;
    this->operator++();
    return previous;
}

bool tml::FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator==(const ConstForwardIterator& other) const {
    return data == other.data && size == other.size && allocation_size == other.allocation_size;
}
bool tml::FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator!=(const ConstForwardIterator& other) const {
    return data != other.data || size != other.size || allocation_size != other.allocation_size;
}
const void* tml::FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator*() const { return data; }
tml::FixedSizeGenerationalIdAllocator::ConstForwardIterator
tml::FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator++() {
    // Prefix increment.
    TM_ASSERT(size);
    char* p = (char*)data + allocation_size;
    --size;
    do {
        auto header = (Header*)p;
        if (header->occupied()) break;
        p += allocation_size;
        --size;
    } while (size);
    return *this;
}
tml::FixedSizeGenerationalIdAllocator::ConstForwardIterator
tml::FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator++(int) {
    // Postfix increment.
    auto previous = *this;
    this->operator++();
    return previous;
}

void* tml::MallocAllocator::allocate_bytes(size_t size, size_t alignment) {
    TM_MAYBE_UNUSED(alignment);
    return TM_MALLOC(size, alignment);
}
void* tml::MallocAllocator::reallocate_bytes(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    TM_MAYBE_UNUSED(old_size);
    TM_MAYBE_UNUSED(alignment);
    TM_MAYBE_UNUSED(new_size);
    TM_MAYBE_UNUSED(alignment);
    return TM_REALLOC(ptr, old_size, alignment, new_size, alignment);
}
bool tml::MallocAllocator::reallocate_bytes_in_place(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    TM_MAYBE_UNUSED(ptr);
    TM_MAYBE_UNUSED(old_size);
    TM_MAYBE_UNUSED(new_size);
    TM_MAYBE_UNUSED(alignment);
#ifdef TM_REALLOC_IN_PLACE
    return TM_REALLOC_IN_PLACE(ptr, old_size, alignment, new_size, alignment);
#else
    return false;
#endif
}
void tml::MallocAllocator::free_bytes(void* ptr, size_t size, size_t alignment) {
    TM_MAYBE_UNUSED(size);
    TM_MAYBE_UNUSED(alignment);
    TM_FREE(ptr, size, alignment);
}

#endif /* defined(TM_ALLOCATOR_IMPLEMENTATION) */

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2020 Tolga Mizrak

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------

Public Domain (www.unlicense.org):
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

---------------------------------------------------------------------------
*/