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
     * @return MemoryBlock to the allocated memory region, result.ptr == nullptr if out of memory.
     * When freeing, either size or result.size can be used.
     */
    MemoryBlock allocate_bytes(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    /*!
     * @brief Allocates memory. Same as allocate_bytes, but throws if out of memory.
     * @param size[IN] Size of the memory region to be allocated in bytes.
     * @param alignment[IN] Alignment of the memory region. Default is at least TM_DEFAULT_ALIGNMENT(8).
     * @return Pointer to the allocated memory region.
     * @throw Throws std::bad_alloc (or nullptr_t if TMAL_NO_STL is defined) if out of memory.
     * When freeing, either size or result.size can be used.
     */
    MemoryBlock allocate_bytes_throws(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
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