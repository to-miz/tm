/*!
 * @brief The result of an allocation, ptr points to uninitialized memory.
 */
template <class T>
struct AllocationResult {
    T* ptr;
    size_t size;

    inline explicit operator bool() const { return ptr; }
};

/*!
 * @brief Different from AllocationResult, ptr points to constructed objects ready to use.
 */
template <class T>
struct CreateResult {
    T* ptr;
    size_t size;

    inline explicit operator bool() const { return ptr; }
};

struct MemoryBlock {
    void* ptr;
    size_t size;

    inline explicit operator bool() const { return ptr; }

    template <class T>
    AllocationResult<T> as() {
        return {static_cast<T*>(ptr), size / sizeof(T)};
    }
};