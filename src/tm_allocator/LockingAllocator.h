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

    MemoryBlock allocate_bytes(size_t size, size_t alignment = TM_DEFAULT_ALIGNMENT) {
        LockGuard guard{mut};
        return allocator.allocate_bytes(size, alignment);
    }
    MemoryBlock reallocate_bytes(void* ptr, size_t oldSize, size_t newSize, size_t alignment = TM_DEFAULT_ALIGNMENT) {
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