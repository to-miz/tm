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

    MemoryBlock allocate_bytes(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    MemoryBlock reallocate_bytes(void* ptr, size_t old_size, size_t new_size, size_t alignment = TM_DEFAULT_ALIGNMENT);
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
 * example:
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