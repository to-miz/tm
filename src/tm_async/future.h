#ifndef TM_REMOVE_REFERENCE
#include <type_traits>
#define TM_REMOVE_REFERENCE(typeArg) typename std::remove_reference<typeArg>::type
#endif

namespace tml {
template <class T>
class future {
    static_assert(sizeof(T) <= TMA_ASYNC_MAX_STORAGE_SIZE, "T is too large.");
    static_assert(alignof(T) <= TM_MAX_ALIGN, "Overaligned T is not supported.");

    tma_async_handle handle = {};

   public:
    future() noexcept = default;
    explicit future(tma_async_handle handleArg) noexcept : handle(handleArg) {}
    future(future&& other) noexcept : handle(other.handle) { other.handle = {}; }
    future& operator=(future&& other) noexcept {
        if (this != &other) swap(*this, other);
        return *this;
    }
    ~future() {
        if (handle) tma_free_async_work(&handle);
    }

    tm_errc wait() {
        TM_ASSERT(valid());
        return tma_wait_single(handle);
    }
    tm_errc wait_for(int32_t milliseconds) {
        TM_ASSERT(valid());
        return tma_wait_single_for(handle, milliseconds);
    }

    T get() {
        TM_ASSERT(valid());
        wait();
        return std::move(*static_cast<T*>(tma_get_storage(handle)));
    }

    int32_t progress() { return tma_get_progress(handle); }
    bool dispatch() { return tma_dispatch_async_work(handle); }
    void cancel() { tma_cancel(handle); }
    bool valid() const { return static_cast<bool>(handle); }
    tm_errc error_code() const { return handle.error_code(); }
    void clear() {
        if (handle) {
            tma_free_async_work(&handle);
            handle = {};
        }
    }

    friend void swap(future& a, future& b) noexcept {
        tma_async_handle temp = a.handle;
        a.handle = b.handle;
        b.handle = temp;
    }
};
template <>
class future<void> {
    tma_async_handle handle = {};

   public:
    inline future() noexcept = default;
    inline explicit future(tma_async_handle handleArg) noexcept : handle(handleArg) {}
    inline future(future&& other) noexcept : handle(other.handle) { other.handle = {}; }
    inline future& operator=(future&& other) noexcept {
        if (this != &other) swap(*this, other);
        return *this;
    }
    inline ~future() { tma_free_async_work(&handle); }

    inline tm_errc wait() {
        TM_ASSERT(valid());
        return tma_wait_single(handle);
    }
    inline tm_errc wait_for(int32_t milliseconds) {
        TM_ASSERT(valid());
        return tma_wait_single_for(handle, milliseconds);
    }

    inline void get() {
        TM_ASSERT(valid());
        wait();
    }

    inline int32_t progress() { return tma_get_progress(handle); }
    inline bool dispatch() { return tma_dispatch_async_work(handle); }
    inline bool valid() const { return static_cast<bool>(handle); }
    inline tm_errc error_code() const { return handle.error_code(); }
    void clear() {
        if (handle) {
            tma_free_async_work(&handle);
            handle = {};
        }
    }

    friend void swap(future& a, future& b) noexcept {
        tma_async_handle temp = a.handle;
        a.handle = b.handle;
        b.handle = temp;
    }
};

/* These reinterpret casts are very iffy, but we at least assert that on the platforms this library is supported, it
   should work as expected, even if technically UB. We could instead copy into a temporary buffer, since there is a hard
   cap on waitable handles of 64 currently. */
template <class T>
tm_errc wait_all(future<T>* futures, tm_size_t futures_count) {
    static_assert(sizeof(future<T>) == sizeof(tma_async_handle), "");
    static_assert(alignof(future<T>) == alignof(tma_async_handle), "");
    return tma_wait_all((const tma_async_handle*)futures, futures_count);
}
template <class T>
tm_errc wait_all_for(future<T>* futures, tm_size_t futures_count, int32_t milliseconds) {
    static_assert(sizeof(future<T>) == sizeof(tma_async_handle), "");
    static_assert(alignof(future<T>) == alignof(tma_async_handle), "");
    return tma_wait_all_for((const tma_async_handle*)futures, futures_count, milliseconds);
}
template <class T>
tma_wait_any_result wait_any(future<T>* futures, tm_size_t futures_count) {
    static_assert(sizeof(future<T>) == sizeof(tma_async_handle), "");
    static_assert(alignof(future<T>) == alignof(tma_async_handle), "");
    return tma_wait_any((const tma_async_handle*)futures, futures_count);
}
template <class T>
tma_wait_any_result wait_any_for(future<T>* futures, tm_size_t futures_count, int32_t milliseconds) {
    static_assert(sizeof(future<T>) == sizeof(tma_async_handle), "");
    static_assert(alignof(future<T>) == alignof(tma_async_handle), "");
    return tma_wait_any_for((const tma_async_handle*)futures, futures_count, milliseconds);
}

template <class Func>
auto async(Func&& func, bool deferred = false) -> future<decltype(func(tma_worker_context{}))> {
    typedef decltype(func(tma_worker_context{})) result_type;
    typedef TM_REMOVE_REFERENCE(Func) functor_type;

    tma_async_handle handle = tma_push_async_work(
        [](tma_worker_context worker, void* execution_context) {
            functor_type& functor = *static_cast<functor_type*>(execution_context);
            if constexpr (std::is_same<result_type, void>::value) {
                // Execute function directly.
                functor(worker);
            } else {
                // Execute function and store result in storage.
                TM_ASSERT(worker.storage);
                ::new (worker.storage) result_type(functor(worker));
            }
        },
        &func, deferred);

    return future<result_type>{handle};
}
}  // namespace tml