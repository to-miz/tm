/*!
 * @brief An allocator that wraps TM_MALLOC.
 * This way all the allocation APIs can be used with the global heap.
 */
struct MallocAllocator {
    MemoryBlock allocate_bytes(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    MemoryBlock reallocate_bytes(void* ptr, size_t old_size, size_t new_size, size_t alignment = TM_DEFAULT_ALIGNMENT);
    bool reallocate_bytes_in_place(void* ptr, size_t old_size, size_t new_size,
                                   size_t alignment = TM_DEFAULT_ALIGNMENT);
    void free_bytes(void* ptr, size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT);

    inline static bool is_valid(const MallocAllocator* /*allocator*/) { return true; }
};