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