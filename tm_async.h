/*
tm_async.h v0.0.1 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2019

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_ASYNC_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

DESCRIPTION
    An async library for single producer, multiple consumer asynchronous execution that uses an internal thread pool.

SWITCHES
    TMA_NO_TMA_PREFIX
        Controls whether type and function names are prefixed with tma_.
        Defining it before including this file will strip the prefix.

    TM_BITSCAN_FORWARD
        Represents a function with this signature:
            int32_t TM_BITSCAN_FORWARD(uint32_t x);
        Returns the index of the first set bit of x.
        The implementation uses a custom procedure if this is not defined.
        Possible redefinitions are __builtin_ctz on gcc/clang or a wrapper around _BitScanForward on MSVC.

ISSUES
    - Not yet first release.
    - Currently windows only.
    - Only the main thread can issue or free async work. This is unlikely to change, because allowing all threads to
      create and free async work can seemingly lead to deadlock situations, if all threads created work and waited on
      completion.

TODO
    - Write documentation.
        - Write about in which order async jobs are executed in generally and when it doesn't hold.

HISTORY     (DD.MM.YY)
    v0.0.1   31.05.19 Initial Commit.
*/

/* Dependencies */
#ifdef TM_ASYNC_IMPLEMENTATION

/* Global allocation functions to use. */
#if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
    // Either all or none have to be defined.
    #include <stdlib.h>
    #define TM_MALLOC(size, alignment) malloc((size))
    #define TM_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) realloc((ptr), (new_size))
    #define TM_FREE(ptr, size, alignment) free((ptr))
#endif

#if !defined(TM_MEMSET) || !defined(TM_MEMMOVE)
    #include <string.h>
    #ifndef TM_MEMSET
        #define TM_MEMSET memset
    #endif
    #ifndef TM_MEMMOVE
        #define TM_MEMMOVE memmove
    #endif
#endif

#endif /* defined(TM_ASYNC_IMPLEMENTATION) */

#ifndef _TM_ASYNC_H_INCLUDED_F1955725_9F8A_40CC_92E3_F097818D4383_
#define _TM_ASYNC_H_INCLUDED_F1955725_9F8A_40CC_92E3_F097818D4383_

/* Native bools, override by defining TM_BOOL_DEFINED yourself before including this file. */
#ifndef TM_BOOL_DEFINED
    #define TM_BOOL_DEFINED
    #ifdef __cplusplus
        typedef bool tm_bool;
        #define TM_TRUE true
        #define TM_FALSE false
    #else
        typedef _Bool tm_bool;
        #define TM_TRUE 1
        #define TM_FALSE 0
    #endif
#endif /* !defined(TM_BOOL_DEFINED) */

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* Define to 1 if tm_size_t is signed. */
    #include <stddef.h> /* Include C version so identifiers are in global namespace. */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
    #define TM_ERRC_DEFINED
    #include <errno.h>
    enum TM_ERRC_CODES {
        TM_OK           = 0,            /* Alternatively std::errc() */
        TM_EPERM        = EPERM,        /* Alternatively std::errc::operation_not_permitted */
        TM_ENOENT       = ENOENT,       /* Alternatively std::errc::no_such_file_or_directory */
        TM_EIO          = EIO,          /* Alternatively std::errc::io_error */
        TM_EAGAIN       = EAGAIN,       /* Alternatively std::errc::resource_unavailable_try_again */
        TM_ENOMEM       = ENOMEM,       /* Alternatively std::errc::not_enough_memory */
        TM_EACCES       = EACCES,       /* Alternatively std::errc::permission_denied */
        TM_EBUSY        = EBUSY,        /* Alternatively std::errc::device_or_resource_busy */
        TM_EEXIST       = EEXIST,       /* Alternatively std::errc::file_exists */
        TM_EXDEV        = EXDEV,        /* Alternatively std::errc::cross_device_link */
        TM_ENODEV       = ENODEV,       /* Alternatively std::errc::no_such_device */
        TM_EINVAL       = EINVAL,       /* Alternatively std::errc::invalid_argument */
        TM_EMFILE       = EMFILE,       /* Alternatively std::errc::too_many_files_open */
        TM_EFBIG        = EFBIG,        /* Alternatively std::errc::file_too_large */
        TM_ENOSPC       = ENOSPC,       /* Alternatively std::errc::no_space_on_device */
        TM_ERANGE       = ERANGE,       /* Alternatively std::errc::result_out_of_range */
        TM_ENAMETOOLONG = ENAMETOOLONG, /* Alternatively std::errc::filename_too_long */
        TM_ENOLCK       = ENOLCK,       /* Alternatively std::errc::no_lock_available */
        TM_ECANCELED    = ECANCELED,    /* Alternatively std::errc::operation_canceled */
        TM_ENOSYS       = ENOSYS,       /* Alternatively std::errc::function_not_supported */
        TM_ENOTEMPTY    = ENOTEMPTY,    /* Alternatively std::errc::directory_not_empty */
        TM_EOVERFLOW    = EOVERFLOW,    /* Alternatively std::errc::value_too_large */
        TM_ETIMEDOUT    = ETIMEDOUT,    /* Alternatively std::errc::timed_out */
    };
    typedef int tm_errc;
#endif

/* assert */
#ifndef TM_ASSERT
    #include <assert.h>
    #define TM_ASSERT assert
#endif /* !defined(TM_ASSERT) */

/* We need the max align value, but it is a C11 feature. We go with a sensible default for C. */
#ifndef TM_MAX_ALIGN
    #ifdef __cplusplus
        #include <cstddef>
        #define TM_MAX_ALIGN sizeof(std::max_align_t)
    #else
        #define TM_MAX_ALIGN (sizeof(void*) * 2)
    #endif
#endif

/* Linkage defaults to extern, to override define TMA_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMA_DEF
    #define TMA_DEF extern
#endif

/* Namespacing for C while allowing renaming. */
#ifdef TMA_NO_TMA_PREFIX
    /* Types */
    #define tma_worker_context               worker_context
    #define tma_async_handle                 async_handle
    #define tma_async_work_procedure         async_work_procedure
    #define tma_wait_any_result              wait_any_result

    /* Functions */
    #define tma_is_async_handle_valid        is_async_handle_valid
    #define tma_get_error_code               get_error_code
    #define tma_initialize_async_thread_pool initialize_async_thread_pool
    #define tma_destroy_async_thread_pool    destroy_async_thread_pool
    #define tma_push_async_work              push_async_work
    #define tma_dispatch_async_work          dispatch_async_work
    #define tma_free_async_work              free_async_work
    #define tma_get_storage                  get_storage
    #define tma_get_progress                 get_progress
    #define tma_report_progress              report_progress
    #define tma_cancel                       async_cancel
    #define tma_is_cancelled                 async_is_cancelled
    #define tma_wait_single                  wait_single
    #define tma_wait_single_for              wait_single_for
    #define tma_wait_all                     wait_all
    #define tma_wait_all_for                 wait_all_for
    #define tma_wait_any                     wait_any
    #define tma_wait_any_for                 wait_any_for
    #define tma_sleep                        sleep
#endif /* !defined(TMA_NO_TMA_PREFIX) */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t thread_id;
    void* storage;

    void* tma_internal; /* Used by tma_get_progress, do not modify. */
} tma_worker_context;

typedef struct {
    int32_t id;

#ifdef __cplusplus
    inline explicit operator bool() const { return id > 0; }
    inline tm_errc error_code() const { return (id <= 0) ? tm_errc(-(id - 1)) : tm_errc(); }
#endif
} tma_async_handle;

static inline tm_bool tma_is_async_handle_valid(tma_async_handle handle) { return handle.id > 0; }
static inline tm_errc tma_get_error_code(tma_async_handle handle) {
    return (handle.id <= 0) ? (tm_errc)(-(handle.id - 1)) : (tm_errc)0;
}

/* Calling convention of the async procedure. */
#if defined(_MSC_VER)
    #define TMA_ASYNC_CALL __cdecl
#elif defined(__GNUC__)
    #define TMA_ASYNC_CALL __attribute__((cdecl))
#endif

typedef void TMA_ASYNC_CALL tma_async_work_procedure(tma_worker_context worker, void* execution_context);
typedef void TMA_ASYNC_CALL tma_async_setup_procedure(int32_t thread_id, void* execution_context, tm_bool startup);

/* Initializes the thread pool used by the async functions. */
TMA_DEF tm_errc tma_initialize_async_thread_pool(tm_size_t threads_count);
/* Same as tma_initialize_async_thread_pool, but each worker thread calls the setup function on startup and termination.
   Useful to setup threadlocal storage and per thread initialization. The context should be valid for the whole lifetime
   of the threadpool. */
TMA_DEF tm_errc tma_initialize_async_thread_pool_ex(tm_size_t threads_count, tma_async_setup_procedure* setup_procedure,
                                                    void* context);
/*
Destroys a threadpool. The param completely controls whether the function is allowed to "leak" allocations,
it can be safely set to true when cleanup is being done on process exit. That will leak the allocations to the operating
system, which will free them automatically. This can speed up process exit times.
*/
TMA_DEF void tma_destroy_async_thread_pool(tm_bool completely);

TMA_DEF tma_async_handle tma_push_async_work(tma_async_work_procedure* procedure, void* context, tm_bool deferred);
TMA_DEF tm_bool tma_dispatch_async_work(tma_async_handle handle);
TMA_DEF void tma_free_async_work(tma_async_handle* handle);

typedef struct {
    tm_size_t index;
    tm_errc ec;
} tma_wait_any_result;

TMA_DEF void* tma_get_storage(tma_async_handle handle);
/* Returns current progress value. */
TMA_DEF int32_t tma_get_progress(tma_async_handle handle);
/* Sets progress value. To be called from inside an async procedure. */
TMA_DEF void tma_report_progress(tma_worker_context worker, int32_t progress);
/* Returns current progress value. */
TMA_DEF void tma_cancel(tma_async_handle handle);
/* Gets whether async work was requested to be cancelled. To be called from inside an async procedure. */
TMA_DEF tm_bool tma_is_cancelled(tma_worker_context worker);
TMA_DEF tm_errc tma_wait_single(tma_async_handle handle);
TMA_DEF tm_errc tma_wait_single_for(tma_async_handle handle, int32_t milliseconds);
// Wait on multiple handles at once. The parameters handles and handles_count must refer to a valid, non empty range of
// handles. Some of the handles may be zero ({0}), but not all of them.
TMA_DEF tm_errc tma_wait_all(const tma_async_handle* handles, tm_size_t handles_count);
TMA_DEF tm_errc tma_wait_all_for(const tma_async_handle* handles, tm_size_t handles_count, int32_t milliseconds);
TMA_DEF tma_wait_any_result tma_wait_any(const tma_async_handle* handles, tm_size_t handles_count);
TMA_DEF tma_wait_any_result tma_wait_any_for(const tma_async_handle* handles, tm_size_t handles_count,
                                             int32_t milliseconds);

TMA_DEF void tma_sleep(int32_t milliseconds);

#ifdef __cplusplus
}
#endif

enum { TMA_ASYNC_MAX_STORAGE_SIZE = 32 };

/* Templated future<T> api for C++. */
#ifdef __cplusplus

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

#endif /* defined(__cplusplus) */

#endif /* defined(_TM_ASYNC_H_INCLUDED_F1955725_9F8A_40CC_92E3_F097818D4383_) */

#ifdef TM_ASYNC_IMPLEMENTATION

#ifndef TM_MAYBE_UNUSED
    #define TM_MAYBE_UNUSED(x) ((void)(x))
#endif

/* Static assert macro for C11/C++. Second parameter must be an identifier, not a string literal. */
#ifndef TM_STATIC_ASSERT
    #if defined(__cplusplus)
        #ifdef __cpp_static_assert
            #define TM_STATIC_ASSERT(cond, msg) static_assert(cond, #msg)
        #endif
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #define TM_STATIC_ASSERT(cond, msg) _Static_assert(cond, #msg)
    #else
        #define TM_STATIC_ASSERT(cond, msg) typedef char static_assertion_##msg[(cond) ? 1 : -1]
    #endif
#endif /* !defined(TM_STATIC_ASSERT) */

/* Use null of the underlying language. */
#ifndef TM_NULL
    #ifdef __cplusplus
        #define TM_NULL nullptr
    #else
        #define TM_NULL NULL
    #endif
#endif

#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

#ifndef TM_ARRAY_COUNT
    #define TM_ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))
#endif /* !defined(TM_ARRAY_COUNT) */

#ifdef __cplusplus
extern "C" {
#endif

typedef long tma_int;
typedef volatile long tma_atomic_int;

#if defined(__GNUC__)
    #error Not implemented.
    // See https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html#g_t_005f_005fatomic-Builtins
    // See https://llvm.org/docs/Atomics.html#libcalls-atomic
    typedef unsigned int tma_uint;
    typedef unsigned int tma_atomic_uint;
    #define TMA_ATOMIC_LOAD(x) __atomic_load_4((x), __ATOMIC_ACQUIRE)
    #define TMA_ATOMIC_STORE(x, rhs) __atomic_store_4((x), (rhs), __ATOMIC_RELEASE)
    #define TMA_ATOMIC_FETCH_ADD(x) __atomic_fetch_add_4((x), 1, __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_FETCH_SUB(x) __atomic_fetch_sub_4((x), 1, __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_EXCHANGE(x, new_value) __atomic_exchange_4((x), (new_value), __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_EXCHANGE_SLOT(x, new_value) __atomic_exchange_4((x), (new_value), __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_COMPARE_AND_SWAP(x, comperand, new_value) \
        __atomic_compare_exchange_4((x), (comperand), (new_value), /*weak=*/TM_FALSE, __ATOMIC_SEQ_CST)
    #define TMA_ATOMIC_COMPARE_AND_SWAP_SLOT(x, comperand, new_value) \
        __atomic_compare_exchange_4((x), (comperand), (new_value), /*weak=*/TM_FALSE, __ATOMIC_SEQ_CST)
#elif defined(_MSC_VER) && !defined(_M_ARM) && !defined(_M_ARM64)
    // Need atomic intrinsics for MSVC.
    #include <intrin.h>
    typedef long tma_int;
    typedef volatile long tma_atomic_int;

    #if !defined(TM_BITSCAN_FORWARD)
        static int32_t tma_bitscan_forward(uint32_t x) {
            // Implement bitscan forward using an intrinsic.
            if (x == 0) return -1;
            unsigned long index = 0;
            _BitScanForward(&index, (unsigned long)x);
            return (int32_t)index;
        }
        #define TM_BITSCAN_FORWARD tma_bitscan_forward
    #endif

    #if defined(_M_ARM) || defined(_M_ARM64)
        #error Not implemented.
        // On ARM volatile access doesn't have acquire/load semantics, we need these intrinsics to have the compiler not
        // optimize volatile access away. Hardware barriers are also needed for memory order.
        // See https://docs.microsoft.com/en-us/cpp/intrinsics/arm-intrinsics?view=vs-2019#IsoVolatileLoadStore
        #define TMA_LOAD_VOLATILE(x) __iso_volatile_load32(x)
        #define TMA_STORE_VOLATILE(x, value) __iso_volatile_store32((x), (value))
    #elif defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86)
        // x86/64 has implicit acquire/release semantics for volatile access even with -volatile:iso.
        #define TMA_LOAD_VOLATILE(x) (*(x))
        #define TMA_STORE_VOLATILE(x, value) (*(x) = (value))
    #else
        #error Not implemented for this architecture.
    #endif

    #define TMA_COMPILER_BARRIER() _ReadWriteBarrier()

    static tma_int tma_atomic_load_acquire(tma_atomic_int* value) {
        tma_int result = TMA_LOAD_VOLATILE(value);
        TMA_COMPILER_BARRIER();
        return result;
    }
    static tma_int tma_atomic_load_seq_cst(tma_atomic_int* value) {
        tma_int result = TMA_LOAD_VOLATILE(value);
        TMA_COMPILER_BARRIER();
        return result;
    }
    static tma_int tma_atomic_load_relaxed(tma_atomic_int* value) {
        tma_int result = TMA_LOAD_VOLATILE(value);
        return result;
    }
    static void tma_atomic_store_release(tma_atomic_int* dest, tma_int value) {
        TMA_COMPILER_BARRIER();
        TMA_STORE_VOLATILE(dest, value);
    }
    static void tma_atomic_store_relaxed(tma_atomic_int* dest, tma_int value) { *dest = value; }
    // Sequentially consistent store.
    static void tma_atomic_store_seq_cst(tma_atomic_int* dest, tma_int value) { _InterlockedExchange(dest, value); }

    static tm_bool tma_atomic_compare_and_swap_strong(tma_atomic_int* dest, tma_int* comperand, tma_int new_value) {
        tma_int prev = _InterlockedCompareExchange(dest, new_value, *comperand);
        if (prev == *comperand) return TM_TRUE;
        *comperand = prev;
        return TM_FALSE;
    }
    // There is no weak version on MSVC.
    #define tma_atomic_compare_and_swap_weak tma_atomic_compare_and_swap_strong

    static tm_bool tma_atomic_compare_and_swap_strong_pointer(void* volatile* dest, void** comperand, void* new_value) {
        void* prev = _InterlockedCompareExchangePointer(dest, new_value, *comperand);
        if (prev == *comperand) return TM_TRUE;
        *comperand = prev;
        return TM_FALSE;
    }

    static void* tma_atomic_exchange_seq_cst_pointer(void* volatile* dest, void* new_value) {
        return _InterlockedExchangePointer(dest, new_value);
    }
#elif defined(__cplusplus)
    #include <atomic>
    #error Not implemented.
#else
    #error Not implemented.
    // C11 version.
    #include <stdatomic.h>
    typedef unsigned int tma_uint;
    typedef _Atomic unsigned int tma_atomic_uint;
    #define TMA_ATOMIC_LOAD(x) atomic_load((x))
    #define TMA_ATOMIC_STORE(x, rhs) atomic_store((x), (rhs))
    #define TMA_ATOMIC_FETCH_ADD(x) atomic_fetch_add((x), 1)
    #define TMA_ATOMIC_FETCH_SUB(x) atomic_fetch_sub((x), 1)
    #define TMA_ATOMIC_EXCHANGE(x, new_value) atomic_exchange((x), (new_value))
    #define TMA_ATOMIC_EXCHANGE_SLOT(x, new_value) atomic_exchange((x), new_value)
    // atomic_compare_exchange_strong takes the comperand first as a pointer.
    static tm_bool tma_impl_atomic_compare_and_swap(volatile tma_atomic_uint* x, unsigned int comperand,
                                                    unsigned int new_value) {
        return atomic_compare_exchange_strong(x, &comperand, new_value);
    }
    static tm_bool tma_impl_atomic_compare_and_swap_slot(struct tma_work_slot* volatile* x, void* comperand,
                                                         void* new_value) {
        return atomic_compare_exchange_strong(x, &comperand, new_value);
    }
    #define TMA_ATOMIC_COMPARE_AND_SWAP(x, comperand, new_value) \
        tma_impl_atomic_compare_and_swap((x), comperand, new_value)
    #define TMA_ATOMIC_COMPARE_AND_SWAP_SLOT(x, comperand, new_value) \
        tma_impl_atomic_compare_and_swap_slot((x), comperand, new_value)
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define tma_read_write_barrier() __asm__ __volatile__("" ::: "memory")
#elif _MSC_VER
    #define tma_read_write_barrier() _ReadWriteBarrier()
#endif

#if !defined(TM_BITSCAN_FORWARD)
    static int32_t tma_bitscan_forward(uint32_t x) {
        if (x == 0) return -1;
        /* From http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup (Public Domain). */
        static const int32_t MultiplyDeBruijnBitPosition[32] = {0,  1,  28, 2,  29, 14, 24, 3,  30, 22, 20,
                                                                15, 25, 17, 4,  8,  31, 27, 13, 23, 21, 19,
                                                                16, 7,  26, 12, 18, 6,  11, 5,  10, 9};
        return MultiplyDeBruijnBitPosition[((uint32_t)((x & (~(x - 1))) * 0x077CB531u)) >> 27];
    }
    #define TM_BITSCAN_FORWARD tma_bitscan_forward
#endif /* !defined(TM_BITSCAN_FORWARD) */

#ifdef _WIN32

static tm_errc tma_winerror_to_errc(DWORD error, tm_errc def) {
    switch (error) {
        case ERROR_ACCESS_DENIED:
        case ERROR_CANNOT_MAKE:
        case ERROR_CURRENT_DIRECTORY:
        case ERROR_INVALID_ACCESS:
        case ERROR_NOACCESS:
        case ERROR_SHARING_VIOLATION:
        case ERROR_WRITE_PROTECT: {
            return TM_EACCES;
        }
        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS: {
            return TM_EEXIST;
        }
        case ERROR_CANTOPEN:
        case ERROR_CANTREAD:
        case ERROR_CANTWRITE:
        case ERROR_OPEN_FAILED:
        case ERROR_READ_FAULT:
        case ERROR_SEEK:
        case ERROR_WRITE_FAULT: {
            return TM_EIO;
        }
        case ERROR_DIRECTORY:
        case ERROR_INVALID_HANDLE:
        case ERROR_INVALID_NAME:
        case ERROR_NEGATIVE_SEEK:
        case ERROR_NO_UNICODE_TRANSLATION:
        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_FLAGS: {
            return TM_EINVAL;
        }
        case ERROR_INSUFFICIENT_BUFFER: {
            return TM_ERANGE;
        }
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND: {
            return TM_ENOENT;
        }
        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY: {
            return TM_ENOMEM;
        }
        case ERROR_BAD_UNIT:
        case ERROR_DEV_NOT_EXIST:
        case ERROR_INVALID_DRIVE: {
            return TM_ENODEV;
        }
        case ERROR_BUSY:
        case ERROR_BUSY_DRIVE:
        case ERROR_DEVICE_IN_USE:
        case ERROR_OPEN_FILES: {
            return TM_EBUSY;
        }
        case ERROR_DISK_FULL:
        case ERROR_HANDLE_DISK_FULL: {
            return TM_ENOSPC;
        }
        case ERROR_BUFFER_OVERFLOW: {
            return TM_ENAMETOOLONG;
        }
        case ERROR_DIR_NOT_EMPTY: {
            return TM_ENOTEMPTY;
        }
        case ERROR_NOT_SAME_DEVICE: {
            return TM_EXDEV;
        }
        case ERROR_TOO_MANY_OPEN_FILES: {
            return TM_EMFILE;
        }
        case ERROR_NOT_READY:
        case ERROR_RETRY: {
            return TM_EAGAIN;
        }
        case ERROR_INVALID_FUNCTION: {
            return TM_ENOSYS;
        }
        case ERROR_LOCK_VIOLATION:
        case ERROR_LOCKED: {
            return TM_ENOLCK;
        }
        case ERROR_OPERATION_ABORTED: {
            return TM_ECANCELED;
        }
        default: {
            return def;
        }
    }
}

enum { TMA_MAX_WORK_RING = 256, TMA_SLOTS_COUNT = 256 };

typedef struct {
    // These fields are const/threadsafe for the lifetime of an allocated slot.
    HANDLE event;
    char storage[TMA_ASYNC_MAX_STORAGE_SIZE + TM_MAX_ALIGN];
    void* context;
    tma_async_work_procedure* procedure;

    // Atomics
    tma_atomic_int progress_report;
    tma_atomic_int cancelled;
    tma_atomic_int event_signaled;
    tm_bool pending;  // TODO: Maybe make atomic so all threads benefit of it being set?
    tma_atomic_int work_ring_pos;
} tma_work_slot;

struct tma_thread_pool_struct;

struct tma_worker_init {
    struct tma_thread_pool_struct* pool;
    tma_async_setup_procedure* setup_procedure;
    void* setup_context;
    tma_worker_context context;
};

struct tma_thread {
    HANDLE handle;
    DWORD os_thread_id;
};

struct tma_worker_state {
    struct tma_thread thread;
    struct tma_worker_init init;
};

// TODO: Rework slots.
struct tma_work_slots {
    tma_work_slot entries[TMA_SLOTS_COUNT];
    unsigned int available_mask[(TMA_SLOTS_COUNT + 31) / 32];
    int available_count;
    struct tma_work_slots* next;
};

struct tma_work_queue {
    tma_work_slot* ring[TMA_MAX_WORK_RING];
    tma_atomic_int read_pos;
    tma_atomic_int write_pos;
    HANDLE read_semaphore;
    HANDLE write_semaphore;
};

struct tma_dispatch_array {
    tma_work_slot* sbo[16];  // Small buffer optimization.
    tma_work_slot** data;
    tm_size_t size;
    tm_size_t capacity;
};

enum { tma_dispatch_message_resume, tma_dispatch_message_ready_to_shutdown, tma_dispatch_message_shutdown_now };
struct tma_dispatch_message {
    int request;
    tm_size_t answer;
};

typedef struct tma_thread_pool_struct {
    struct tma_worker_state* workers;
    tm_size_t workers_count;
    HANDLE workers_shutdown_event;

    struct tma_work_queue work_queue;
    struct tma_work_queue dispatch_queue;
    struct tma_dispatch_array dispatch_array;

    struct tma_work_slots work_slots;

    struct tma_thread dispatch_thread;

    struct tma_dispatch_message dispatch_message;
    HANDLE dipatch_request;
    HANDLE dipatch_answer;
    DWORD main_os_thread_id;
} tma_thread_pool;

extern tma_thread_pool* tma_global_thread_pool;

TM_STATIC_ASSERT(sizeof(tma_int) == 4, TMA_INT_MUST_BE_32BIT);
TM_STATIC_ASSERT(WAIT_OBJECT_0 == 0, TMA_WINDOWS_CONSTANT_CHANGED);

tma_thread_pool* tma_global_thread_pool = TM_NULL;

static tma_async_handle tma_make_async_handle_errc(tm_errc ec) {
    tma_async_handle result;
    result.id = -((int32_t)ec - 1);
    return result;
}
static tma_async_handle tma_make_async_handle_index(int32_t index) {
    tma_async_handle result;
    result.id = index + 1;
    return result;
}
static int32_t tma_async_handle_get_index(tma_async_handle handle) { return handle.id - 1; }

static size_t tma_alignment_offset(const void* ptr, size_t alignment) {
#ifdef _DEBUG
    tm_bool is_power_of_two = (alignment && !(alignment & (alignment - 1)));
    TM_ASSERT(is_power_of_two);
#endif
    size_t offset = (alignment - ((uintptr_t)ptr)) & (alignment - 1);
#ifdef _DEBUG
    tm_bool is_aligned = ((uintptr_t)((const char*)ptr + offset) % alignment) == 0;
    TM_ASSERT(is_aligned);
#endif
    return offset;
}

static tm_size_t tma_next_capacity(tm_size_t cap) { return 3 * ((cap + 2) / 2); }

static tm_bool tma_impl_try_push(struct tma_work_queue* queue, tma_work_slot* slot) {
    // Try to get a write pos.
    // We load it relaxed, because the weak compare below will get us an updated value everytime.
    tma_int write_pos = tma_atomic_load_relaxed(&queue->write_pos);
    for (;;) {
        tma_int next_write_pos = (write_pos + 1) % TMA_MAX_WORK_RING;
        if (tma_atomic_compare_and_swap_weak(&queue->write_pos, &write_pos, next_write_pos)) {
            // We got a write position.
            break;
        }
    }

    void* comperand = TM_NULL;
    if (tma_atomic_compare_and_swap_strong_pointer((void* volatile*)&queue->ring[write_pos], &comperand, slot)) {
        tma_atomic_store_release(&slot->work_ring_pos, write_pos);
        ReleaseSemaphore(queue->read_semaphore, /*release_count=*/1, /*prev_value=*/TM_NULL);
        return TM_TRUE;
    }
    return TM_FALSE;
}

static tm_bool tma_impl_push(struct tma_work_queue* queue, tma_work_slot* slot, DWORD milliseconds) {
    DWORD wait_result = WaitForSingleObject(queue->write_semaphore, milliseconds);
    if (wait_result != WAIT_OBJECT_0) return TM_FALSE;

    return tma_impl_try_push(queue, slot);
}
static tma_work_slot* tma_impl_pop_force(struct tma_work_queue* queue) {
    // Try to get a read pos.
    // We load it relaxed, because the weak compare below will get us an updated value everytime.
    tma_int read_pos = tma_atomic_load_relaxed(&queue->read_pos);
    for (;;) {
        tma_int next_read_pos = (read_pos + 1) % TMA_MAX_WORK_RING;
        if (tma_atomic_compare_and_swap_weak(&queue->read_pos, &read_pos, next_read_pos)) {
            // We got a read position.
            break;
        }
    }

    // TODO: Is relaxed ordering ok here?
    void* result = tma_atomic_exchange_seq_cst_pointer((void* volatile*)&queue->ring[read_pos], TM_NULL);
    ReleaseSemaphore(queue->write_semaphore, /*release_count=*/1, /*prev_value_out=*/TM_NULL);
    return (tma_work_slot*)result;
}
#if 0
static tma_work_slot* tma_impl_pop(struct tma_work_queue* queue, DWORD milliseconds) {
    // Wait for spot to be freed.
    DWORD wait_result = WaitForSingleObject(queue->read_semaphore, milliseconds);
    if (wait_result != WAIT_OBJECT_0) return TM_NULL;

    return tma_impl_pop_force(queue);
}
#endif
static tm_bool tma_impl_pop_slot(struct tma_work_queue* queue, tma_work_slot* slot) {
    tma_int pos = tma_atomic_load_seq_cst(&slot->work_ring_pos);
    void* comperand = slot;
    return tma_atomic_compare_and_swap_strong_pointer((void* volatile*)&queue->ring[pos], &comperand, TM_NULL);
}

static tm_bool tma_impl_make_work_queue(struct tma_work_queue* out) {
    out->read_pos = 0;
    out->write_pos = 0;
    out->read_semaphore = CreateSemaphoreW(/*lpSemaphoreAttributes=*/TM_NULL, /*lInitialCount=*/0,
                                           /*lMaximumCount=*/(LONG)TMA_MAX_WORK_RING,
                                           /*lpName=*/TM_NULL);
    out->write_semaphore =
        CreateSemaphoreW(/*lpSemaphoreAttributes=*/TM_NULL, /*lInitialCount=*/(LONG)TMA_MAX_WORK_RING,
                         /*lMaximumCount=*/(LONG)TMA_MAX_WORK_RING,
                         /*lpName=*/TM_NULL);
    return (out->read_semaphore != TM_NULL) && (out->write_semaphore != TM_NULL);
}
static void tma_impl_destroy_work_queue(struct tma_work_queue* queue) {
    if (queue->read_semaphore) {
        CloseHandle(queue->read_semaphore);
        queue->read_semaphore = TM_NULL;
    }
    if (queue->write_semaphore) {
        CloseHandle(queue->write_semaphore);
        queue->write_semaphore = TM_NULL;
    }
}

static void* tma_impl_get_storage(tma_work_slot* slot) {
    char* p = slot->storage;
    char* result = p + tma_alignment_offset(p, TM_MAX_ALIGN);
    TM_ASSERT(p + sizeof(slot->storage) - result >= TMA_ASYNC_MAX_STORAGE_SIZE);
    return result;
}

static tma_work_slot* tma_get_work_slot(tma_thread_pool* pool, int32_t index) {
    TM_ASSERT(pool);
    TM_ASSERT(index >= 0);

    struct tma_work_slots* slots = &pool->work_slots;
    int32_t slots_index = index / TMA_SLOTS_COUNT;
    int32_t local_index = index % TMA_SLOTS_COUNT;
    for (int32_t i = 0; i < slots_index; ++i) {
        struct tma_work_slots* next = slots->next;
        TM_ASSERT(next);
        slots = next;
    }
    TM_ASSERT(local_index >= 0 && local_index < (int32_t)TM_ARRAY_COUNT(slots->entries));
    return &slots->entries[local_index];
}
static tma_work_slot* tma_get_work_slot_from_handle(tma_thread_pool* pool, tma_async_handle handle) {
    TM_ASSERT(tma_is_async_handle_valid(handle));
    return tma_get_work_slot(pool, tma_async_handle_get_index(handle));
}

static void tma_impl_execute(tma_work_slot* slot, int32_t thread_id) {
    // Pass storage to context so procedure can use it.
    tma_worker_context worker;
    worker.thread_id = thread_id;
    worker.storage = tma_impl_get_storage(slot);
    worker.tma_internal = slot;
    TM_ASSERT(slot->procedure);
    slot->procedure(worker, slot->context);
    tma_atomic_store_release(&slot->event_signaled, 1);
    TM_ASSERT(slot->event);
    BOOL set_event_result = SetEvent(slot->event);
    TM_ASSERT(set_event_result);
    TM_MAYBE_UNUSED(set_event_result);
}

static DWORD WINAPI tma_worker_procedure(LPVOID param) {
    struct tma_worker_init* init = (struct tma_worker_init*)param;
    int32_t thread_id = init->context.thread_id;
    tma_thread_pool* pool = init->pool;
    struct tma_work_queue* queue = &pool->work_queue;

    if (init->setup_procedure) {
        init->setup_procedure(init->context.thread_id, init->setup_context, /*startup=*/TM_TRUE);
    }

    HANDLE wait_handles[2];
    wait_handles[0] = queue->read_semaphore;
    wait_handles[1] = pool->workers_shutdown_event;

    DWORD result = 0;
    for (;;) {
        DWORD wait_result = WaitForMultipleObjects(2, wait_handles, /*wait_all=*/0, INFINITE);
        switch (wait_result) {
            case WAIT_OBJECT_0: {
                tma_work_slot* slot = tma_impl_pop_force(queue);
                if (slot) tma_impl_execute(slot, thread_id);
                break;
            }
            case WAIT_OBJECT_0 + 1: {
                // Shutdown Event
                goto exit;
            }
            default: {
                // Error
                result = 1;
                goto exit;
            }
        }
    }

exit:
    if (init->setup_procedure) {
        init->setup_procedure(init->context.thread_id, init->setup_context, /*startup=*/TM_FALSE);
    }
    return result;
}

static tm_bool tma_impl_push_dispatch_array(struct tma_dispatch_array* array, tma_work_slot* slot) {
    if (array->size + 1 >= array->capacity) {
        tm_size_t new_capacity = tma_next_capacity(array->capacity);
        void* new_data = TM_NULL;
        if (array->data == array->sbo) {
            new_data = TM_MALLOC(sizeof(tma_work_slot*) * new_capacity, sizeof(void*));
        } else {
            new_data = TM_REALLOC(array->data, /*old_size=*/sizeof(tma_work_slot*) * array->capacity,
                                  /*old_alignment=*/sizeof(void*), sizeof(tma_work_slot*) * new_capacity,
                                  /*new_alignment=*/sizeof(void*));
        }
        if (new_data) return TM_FALSE;
        array->data = (tma_work_slot**)new_data;
        array->capacity = new_capacity;
    }
    array->data[array->size++] = slot;
    return TM_TRUE;
}
static void tma_impl_pop_front(struct tma_dispatch_array* array) {
    --array->size;
    if (array->size > 0) {
        TM_MEMMOVE(array->data, array->data + 1, array->size * sizeof(tma_work_slot*));
    }
}

static DWORD WINAPI tma_dispatch_thread_procedure(LPVOID param) {
    tma_thread_pool* pool = (tma_thread_pool*)param;
    // struct tma_thread* dispatch = &pool->dispatch_thread;
    struct tma_work_queue* dispatch_queue = &pool->dispatch_queue;
    struct tma_work_queue* work_queue = &pool->work_queue;
    struct tma_dispatch_array* dispatch_array = &pool->dispatch_array;

    HANDLE wait_handles[3];
    wait_handles[0] = dispatch_queue->read_semaphore;
    wait_handles[1] = pool->dipatch_request;
    wait_handles[2] = work_queue->write_semaphore;
    HANDLE dipatch_request = pool->dipatch_request;
    HANDLE dipatch_answer = pool->dipatch_answer;

    for (;;) {
        tm_bool undispatched_work = (dispatch_array->size > 0);
        DWORD wait_result = WaitForMultipleObjects(2 + undispatched_work, wait_handles, /*wait_all=*/0, INFINITE);
        switch (wait_result) {
            case WAIT_OBJECT_0: {
                // Dispatch queue has new item.
                tma_work_slot* slot = tma_impl_pop_force(dispatch_queue);
                if (!slot) break;
                if (!tma_impl_push_dispatch_array(dispatch_array, slot)) {
                    // We are out of memory, this can very well turn into a deadlock situation if any thread waits for
                    // the completion of a slot that we can't dispatch.
                    // TODO: Is there a meaningful way to recover?
                    exit(-1);
                }
                break;
            }
            case WAIT_OBJECT_0 + 1: {
                // Shutdown request event.
                if (pool->dispatch_message.request == tma_dispatch_message_ready_to_shutdown) {
                    pool->dispatch_message.answer = (dispatch_array->size == 0);
                    SetEvent(dipatch_answer);
                    if (dispatch_array->size == 0) {
                        WaitForSingleObject(dipatch_request, INFINITE);
                        if (pool->dispatch_message.request == tma_dispatch_message_shutdown_now) {
                            return 0;
                        }
                    }
                }
                break;
            }
            case WAIT_OBJECT_0 + 3: {
                // Work queue has a free slot.
                TM_ASSERT(dispatch_array->size > 0);
                tma_work_slot* slot = dispatch_array->data[0];
                TM_ASSERT(slot);

                if (tma_impl_try_push(work_queue, slot)) tma_impl_pop_front(dispatch_array);
                break;
            }
            default: {
                // Error
                return 1;
            }
        }
    }
}

static tm_bool tma_impl_dispatch_async_work(tma_thread_pool* pool, tma_work_slot* slot) {
    TM_ASSERT(pool);
    TM_ASSERT(slot);
    TM_ASSERT(tma_atomic_load_seq_cst(&slot->work_ring_pos) < 0);
    TM_ASSERT(slot->procedure);

    // Try pushing into work queue.
    if (tma_impl_push(&pool->work_queue, slot, /*milliseconds=*/0)) return TM_TRUE;
    // If work queue is full, push into dispatch queue and block until it succeeds.
    return tma_impl_push(&pool->dispatch_queue, slot, INFINITE);
}

static tm_errc tma_wait_result_to_errc(DWORD wait_result, DWORD count) {
    switch (wait_result) {
        default: {
            if (wait_result < count) {
                return TM_OK;
            }
            return TM_ECANCELED;
        }
        case WAIT_TIMEOUT: {
            return TM_ETIMEDOUT;
        }
        case WAIT_FAILED: {
            return tma_winerror_to_errc(GetLastError(), TM_ECANCELED);
        }
    }
}

static tm_errc tma_impl_wait_single(tma_thread_pool* pool, tma_async_handle handle, DWORD milliseconds) {
    if (!tma_is_async_handle_valid(handle)) return TM_EPERM;
    tma_work_slot* slot = tma_get_work_slot_from_handle(pool, handle);
    if (tma_atomic_load_relaxed(&slot->event_signaled)) return TM_OK;
    if (milliseconds == INFINITE && tma_impl_pop_slot(&pool->work_queue, slot)) {
        // Do the work here.
        tma_impl_execute(slot, /*thread_id=*/0);
        return TM_OK;
    }
    // TODO: Handle deferred slots.
    TM_ASSERT(slot->event);
    DWORD wait_result = WaitForSingleObject(slot->event, milliseconds);
    tm_errc result = tma_wait_result_to_errc(wait_result, 1);
    tma_atomic_store_release(&slot->event_signaled, 1);
    return result;
}

enum { tma_impl_processed, tma_impl_no_work, tma_impl_overflow };
static int tma_impl_wait_multiple_chunk(tma_thread_pool* pool, const tma_async_handle* handles, tm_size_t handles_count,
                                        tm_bool wait_all, DWORD milliseconds, tma_wait_any_result* out) {
    TM_ASSERT_VALID_SIZE(handles_count);
    TM_ASSERT(handles || handles_count == 0);

    out->index = 0;
    out->ec = TM_OK;

    HANDLE events_buffer[MAXIMUM_WAIT_OBJECTS];
    tm_size_t handles_index[MAXIMUM_WAIT_OBJECTS];
    DWORD buffer_size = 0;

    tma_work_slot* undispatched = TM_NULL;

    if (!wait_all) {
        for (tm_size_t i = 0; i < handles_count; ++i) {
            if (!tma_is_async_handle_valid(handles[i])) continue;
            int32_t slot_index = tma_async_handle_get_index(handles[i]);
            tma_work_slot* slot = tma_get_work_slot(pool, slot_index);
            if (tma_atomic_load_relaxed(&slot->event_signaled)) {
                out->index = i;
                return tma_impl_processed;
            }
            if (tma_atomic_load_seq_cst(&slot->work_ring_pos) < 0) {
                if (!tma_impl_dispatch_async_work(pool, slot)) {
                    undispatched = slot;
                    continue;
                }
            }
            HANDLE event = slot->event;
            TM_ASSERT(event);
            if (buffer_size >= MAXIMUM_WAIT_OBJECTS) return tma_impl_overflow;
            events_buffer[buffer_size] = event;
            handles_index[buffer_size] = i;
            ++buffer_size;
        }
        if (buffer_size == 0) {
            // There was no finished work (we would have returned already), meaning all handles are invalid.
            out->ec = TM_EPERM;
            return tma_impl_no_work;
        }
    } else {
        tm_bool has_finished_work = TM_FALSE;
        for (tm_size_t i = 0; i < handles_count; ++i) {
            if (!tma_is_async_handle_valid(handles[i])) continue;
            int32_t slot_index = tma_async_handle_get_index(handles[i]);
            tma_work_slot* slot = tma_get_work_slot(pool, slot_index);
            if (tma_atomic_load_relaxed(&slot->event_signaled)) {
                has_finished_work = TM_TRUE;
                continue;
            }
            if (tma_atomic_load_seq_cst(&slot->work_ring_pos) < 0) {
                if (!tma_impl_dispatch_async_work(pool, slot)) {
                    if (milliseconds == INFINITE) {
                        // We are waiting on all work, if the queue is full, we can do some work here instead of
                        // waiting.
                        tma_impl_execute(slot, /*thread_id=*/0);
                        has_finished_work = TM_TRUE;
                        continue;
                    } else if (milliseconds == 0) {
                        // There is undispatched work, so there is no way that work could be finished.
                        out->ec = TM_ETIMEDOUT;
                        return tma_impl_processed;
                    }
                    // There is undispatched work with a full queue and we are on a timer.
                    // Do the slow route instead.
                    return tma_impl_overflow;
                }
            }
            HANDLE event = slot->event;
            TM_ASSERT(event);
            if (buffer_size >= MAXIMUM_WAIT_OBJECTS) return tma_impl_overflow;
            events_buffer[buffer_size] = event;
            handles_index[buffer_size] = i;
            ++buffer_size;
        }
        // No work to be done, everything is already finished.
        if (buffer_size == 0) {
            if (has_finished_work) {
                out->ec = TM_OK;
                return tma_impl_processed;
            }
            out->ec = TM_EPERM;
            return tma_impl_no_work;
        }
    }

    // TODO: In case we are to wait for all objects, try to do some of the available work here instead of waiting.

    TM_ASSERT(buffer_size > 0);
    DWORD wait_result = 0;
    wait_result = WaitForMultipleObjects(buffer_size, events_buffer, wait_all, milliseconds);
    out->ec = tma_wait_result_to_errc(wait_result, buffer_size);
    if (out->ec == TM_OK) {
        TM_ASSERT(wait_result < buffer_size);
        out->index = handles_index[wait_result];

        // Mark events as signalled.
        if (!wait_all) {
            tma_work_slot* slot = tma_get_work_slot_from_handle(pool, handles[out->index]);
            tma_atomic_store_release(&slot->event_signaled, 1);
            TM_ASSERT(slot->event == events_buffer[wait_result]);
        } else {
            for (tm_size_t i = 0; i < (tm_size_t)buffer_size; ++i) {
                tm_size_t handle_index = handles_index[i];
                if (!tma_is_async_handle_valid(handles[handle_index])) continue;
                int32_t slot_index = tma_async_handle_get_index(handles[handle_index]);
                tma_work_slot* slot = tma_get_work_slot(pool, slot_index);
                tma_atomic_store_release(&slot->event_signaled, 1);
                TM_ASSERT(slot->event == events_buffer[handle_index]);
            }
        }

        // We successfully waited for the completion of a work slot, freeing up a spot in the queue. Try dispatching an
        // undispatched slot.
        if (undispatched) tma_impl_dispatch_async_work(pool, undispatched);
    }
    return tma_impl_processed;
}

struct tma_impl_wait_context {
    tma_int handle_index;
    tma_atomic_int* receiver;
    HANDLE semaphore;
};

static VOID CALLBACK tma_impl_wait_any_callback(PVOID param, BOOLEAN timer_or_wait_fired) {
    TM_MAYBE_UNUSED(timer_or_wait_fired);
    struct tma_impl_wait_context* context = (struct tma_impl_wait_context*)param;
    tma_atomic_store_seq_cst(context->receiver, context->handle_index);
    ReleaseSemaphore(context->semaphore, /*lReleaseCount=*/1, /*lpPreviousCount=*/TM_NULL);
}
static VOID CALLBACK tma_impl_wait_all_callback(PVOID param, BOOLEAN timer_or_wait_fired) {
    TM_MAYBE_UNUSED(timer_or_wait_fired);
    HANDLE semaphore = (HANDLE)param;
    ReleaseSemaphore(semaphore, /*lReleaseCount=*/1, /*lpPreviousCount=*/TM_NULL);
}

static tma_wait_any_result tma_impl_wait_all_infinite(tma_thread_pool* pool, const tma_async_handle* handles,
                                                      tm_size_t handles_count) {
    tma_wait_any_result result = {0, TM_OK};

    // At least 1280 bytes on the stack.
    tma_work_slot* slots[MAXIMUM_WAIT_OBJECTS];
    HANDLE events_buffer[MAXIMUM_WAIT_OBJECTS];
    int slot_index[MAXIMUM_WAIT_OBJECTS];

    // tm_size_t dispatch_threshold = pool->workers_count * 4;
    tm_bool work_to_do = TM_FALSE;
    tm_bool first_overall_pass = TM_TRUE;
    do {
        work_to_do = TM_FALSE;
        tm_bool did_work = TM_FALSE;

        // Working in chunks.
        for (tm_size_t chunk_start = 0; chunk_start < handles_count; chunk_start += (tm_size_t)MAXIMUM_WAIT_OBJECTS) {
            int slots_count = 0;
            DWORD events_size = 0;

            // Get remaining chunk size.
            int remaining = MAXIMUM_WAIT_OBJECTS;
            if ((tm_size_t)remaining > handles_count - chunk_start) remaining = (int)(handles_count - chunk_start);

            const tma_async_handle* handles_start = handles + chunk_start;

            // Get slots. Dispatch any slots that were deferred.
            for (int i = 0; i < remaining; ++i) {
                if (!tma_is_async_handle_valid(handles_start[i])) continue;
                tma_work_slot* slot = tma_get_work_slot(pool, tma_async_handle_get_index(handles_start[i]));
                if (tma_atomic_load_acquire(&slot->event_signaled)) continue;
                if (tma_atomic_load_seq_cst(&slot->work_ring_pos) < 0) tma_impl_dispatch_async_work(pool, slot);

                // We have a slot that actually needs work done.
                slots[slots_count] = slot;
                ++slots_count;
            }

            // All slots are either finished or empty.
            if (slots_count == 0) continue;

            // Second pass, try to do work.
            for (int i = 0; i < slots_count; ++i) {
                tma_work_slot* slot = slots[i];
                if (tma_atomic_load_seq_cst(&slot->work_ring_pos) < 0) {
                    if (!tma_impl_dispatch_async_work(pool, slot)) {
                        // Queue is full, do work here.
                        tma_impl_execute(slot, /*thread_id=*/0);
                        did_work = TM_TRUE;
                        continue;
                    }
                }
                if (!slot->pending) {
                    if (tma_impl_pop_slot(&pool->work_queue, slot)) {
                        // We were able to get a slot from the queue.
                        tma_impl_execute(slot, /*thread_id=*/0);
                        did_work = TM_TRUE;
                        continue;
                    }

                    // The slot is not in the queue anymore, which means it was picked up by a worker thread.
                    slot->pending = TM_TRUE;
                }

                // We have a pending slot, try waiting on them.
                TM_ASSERT(slot->event);
                events_buffer[events_size] = slot->event;
                slot_index[events_size] = i;
                ++events_size;
            }

            if (events_size) {
                work_to_do = TM_TRUE;
                // If we did work even once or this is the first pass, we just want to poll, instead of waiting.
                DWORD milliseconds = (did_work || first_overall_pass) ? 0 : INFINITE;
                // Poll if events have finished yet.
                DWORD wait_result =
                    WaitForMultipleObjects(events_size, events_buffer, /*wait_all=*/TM_TRUE, milliseconds);
                tm_errc ec = tma_wait_result_to_errc(wait_result, events_size);
                if (ec == TM_OK) {
                    // All events have been signaled. Mark slots as finished.
                    for (DWORD i = 0; i < events_size; ++i) {
                        TM_ASSERT(slot_index[i] < slots_count);
                        slots[slot_index[i]]->event_signaled = TM_TRUE;
                    }
                    continue;
                } else if (ec != TM_ETIMEDOUT) {
                    // An error occured, abort.
                    result.ec = ec;
                    return result;
                }
                TM_ASSERT(ec == TM_ETIMEDOUT);
            }
        }
        first_overall_pass = TM_FALSE;
    } while (work_to_do);

    return result;
}

/*
Waits on multiple handles at once. On a basic level this calls the winapi function WaitForMultipleObjects.
Unfortunately there is a limitation on WaitForMultipleObjects that it can only wait on MAXIMUM_WAIT_OBJECTS
(seems to be 64 in practice) amount of handles at once. So we need a fallback algorithm in case we need to
wait on more handles.

There are three notable cases:
1. The handles are less than MAXIMUM_WAIT_OBJECTS:
    We can just call WaitForMultipleObjects once.

2. The timeout value is 0:
    In essence, this is a polling call. We can repeatedly call WaitForMultipleObjects with chunks of
    MAXIMUM_WAIT_OBJECTS. Either we need to wait for all and all handles return ready, or we wait for a single
    completion and any chunk returns ready. If all chunks timeout we return timeout too.

3. The timout value is infinite and we need to wait for all handles.
    In this case we can skip waiting and use the waiting thread itself in executing some of the workload.

4. The timout value is greater than 0 and less than infinite, and there are more than MAXIMUM_WAIT_OBJECTS handles:
    This is the slowest and most complex case. We need a lot of preparation until we can even begin to wait.
    The main idea is to call RegisterWaitForSingleObject on every handle. This will use the wait thread pool of the
    operating system to wait on every handle. Once waiting is complete, it calls a user supplied callback.
    For all of this, we need a couple of memory allocations for each of the wait handles, the callback contexts, a timer
    and a semaphore. We wait until the semaphore is released enough times or we time out.
*/
static tma_wait_any_result tma_impl_wait_multiple(tma_thread_pool* pool, const tma_async_handle* handles,
                                                  tm_size_t handles_count, tm_bool wait_all, DWORD milliseconds) {
    tma_wait_any_result result = {0, TM_OK};

    // Don't call this wait function with zero handles.
    TM_ASSERT(handles && handles_count > 0);
    // Don't call this wait function with zero handles.
#ifndef NDEBUG
    tm_bool has_some_valid_handles = TM_FALSE;
    for (tm_size_t i = 0; i < handles_count; ++i) {
        if (tma_is_async_handle_valid(handles[i])) {
            has_some_valid_handles = TM_TRUE;
            break;
        }
    }
    // This assertion hits if the handles array has only invalid handles. At least one handle must be valid.
    TM_ASSERT(has_some_valid_handles);
#endif

    if (milliseconds == 0) {
        // Handle polling case.
        if (wait_all) {
            // Iterate over all chunks.
            for (tm_size_t i = 0; i < handles_count; i += (tm_size_t)MAXIMUM_WAIT_OBJECTS) {
                // Get remaining chunk size.
                tm_size_t remaining = MAXIMUM_WAIT_OBJECTS;
                if (remaining < handles_count - i) remaining = handles_count - i;

                // Process chunk.
                tma_impl_wait_multiple_chunk(pool, handles + i, remaining, wait_all, 0, &result);
                if (result.ec != TM_OK) return result;
                result.index += i;  // Turn chunk index to index into handles array.
            }
            TM_ASSERT(result.ec == TM_OK);
            return result;
        } else {
            int chunk_result = 0;
            // Iterate over all chunks.
            tm_bool timed_out = TM_FALSE;
            for (tm_size_t i = 0; i < handles_count; i += (tm_size_t)MAXIMUM_WAIT_OBJECTS) {
                // Get remaining chunk size.
                tm_size_t remaining = MAXIMUM_WAIT_OBJECTS;
                if (remaining > handles_count - i) remaining = handles_count - i;

                // Process chunk.
                chunk_result = tma_impl_wait_multiple_chunk(pool, handles + i, remaining, wait_all, 0, &result);
                if (chunk_result == tma_impl_no_work) continue;  // No work means handles in this chunk are empty.
                if (result.ec == TM_OK) result.index += i;       // Turn chunk index to index into handles array.
                if (result.ec != TM_ETIMEDOUT) return result;    // Only if we don't timeout do we return.
                timed_out = TM_TRUE;
            }
            if (chunk_result == tma_impl_no_work) {
                if (timed_out) {
                    // We timed out at least once, which means there are still handles that are executing.
                    result.index = 0;
                    result.ec = TM_ETIMEDOUT;
                } else {
                    // All handles were empty. Return error in this case.
                    result.index = 0;
                    result.ec = TM_EPERM;
                }
                return result;
            }
            TM_ASSERT(result.ec == TM_ETIMEDOUT);
            return result;
        }
    }

    // 3. Case: Use main thread to also do work.
    if (milliseconds == INFINITE && wait_all) {
        return tma_impl_wait_all_infinite(pool, handles, handles_count);
    }

    // Try handling all handles at once. It could be that the amount of waitable handles is less than
    // MAXIMUM_WAIT_OBJECTS.
    if (tma_impl_wait_multiple_chunk(pool, handles, handles_count, wait_all, milliseconds, &result) ==
        tma_impl_processed) {
        return result;
    }

// TODO: Do we want an upper count?
#if 0
    if (handles_count > 512) {
        result.ec = TM_EPERM;
        return result;
    }
#endif

    result.index = 0;

    /*
    There are too many handles, so we need to wait on them differently.
    The idea is this:
    We use the winapi function RegisterWaitForSingleObject that uses wait threads to wait on each handle.
    We register a callback that just releases a semaphore.
    We wait until the semaphore is released enough times or we time out.
    */

    HANDLE waitable_timer = TM_NULL;
    HANDLE semaphore = TM_NULL;
    HANDLE* wait_handles = TM_NULL;
    tm_size_t wait_handles_count = 0;
    LARGE_INTEGER due_time = {0};
    tma_work_slot* single_undispatched = TM_NULL;

    HANDLE semaphore_and_timer_buffer[2] = {TM_NULL, TM_NULL};
    DWORD semaphore_and_timer_buffer_size = 0;

    // Array to hold all undispatched slots. If we need to wait for all handles, we need to dispatch and handle all of
    // them.
    tma_work_slot** undispatched = TM_NULL;
    tm_size_t undispatched_count = 0;
    tm_size_t dispatched_count = 0;

    // Array to hold all contexts for when we only need to wait for the first slot to complete.
    struct tma_impl_wait_context* contexts = TM_NULL;
    volatile LONG first_completed_index = 0;  // Volatile is probably overkill, but we need it for interlocked access.

    // Flags for RegisterWaitForSingleObject.
    // We want the callback to be called from the wait thread itself, since all it is doing is releasing a semaphore.
    // Otherwise it would be issued to another thread, that then would call the callback, increasing latency.
    // We need the wait operation executed only once.
    ULONG register_wait_flags = WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE;

    DWORD wait_result = 0;

    if (milliseconds != INFINITE) {
        waitable_timer =
            CreateWaitableTimerW(/*lpTimerAttributes=*/TM_NULL, /*bManualReset=*/TM_TRUE, /*lpTimerName=*/TM_NULL);
        if (!waitable_timer) {
            result.ec = tma_winerror_to_errc(GetLastError(), TM_ENOMEM);
            goto exit;
        }
        // Due time for SetWaitableTimer.
        // Negative values denote relative time. Time is in 100 nanosecond intervals.
        due_time.QuadPart = -(long long)milliseconds * 10000ll;
    }

    semaphore = CreateSemaphoreW(/*lpSemaphoreAttributes=*/TM_NULL, /*lInitialCount=*/0,
                                 /*lMaximumCount=*/(LONG)handles_count,
                                 /*lpName=*/TM_NULL);
    if (!semaphore) {
        result.ec = tma_winerror_to_errc(GetLastError(), TM_ENOMEM);
        goto exit;
    }

    wait_handles = (HANDLE*)TM_MALLOC(sizeof(HANDLE) * handles_count, sizeof(void*));
    if (!wait_handles) {
        result.ec = TM_ENOMEM;
        goto exit;
    }

    if (wait_all) {
        undispatched = (tma_work_slot**)TM_MALLOC(sizeof(tma_work_slot*) * handles_count, sizeof(void*));
        if (!undispatched) {
            result.ec = TM_ENOMEM;
            goto exit;
        }
    } else {
        contexts = (struct tma_impl_wait_context*)TM_MALLOC(sizeof(struct tma_impl_wait_context) * handles_count,
                                                            sizeof(void*));
        if (!contexts) {
            result.ec = TM_ENOMEM;
            goto exit;
        }
    }

    if (waitable_timer) {
        BOOL set_timer_result = !SetWaitableTimer(
            waitable_timer, &due_time,
            /*lPeriod=*/0,                         // We need the timer to only fire once, no period needed.
            /*pfnCompletionRoutine=*/TM_NULL,      // No callbacks needed, we will wait on the handle directly.
            /*lpArgToCompletionRoutine=*/TM_NULL,  // See above.
            /*fResume=*/TM_FALSE                   // Whether timer wakes a system from suspended power.
        );
        if (!set_timer_result) {
            result.ec = tma_winerror_to_errc(GetLastError(), TM_EPERM);
            goto exit;
        }
    }

    if (!wait_all) {
        for (tm_size_t i = 0; i < handles_count; ++i) {
            if (!tma_is_async_handle_valid(handles[i])) continue;
            int32_t slot_index = tma_async_handle_get_index(handles[i]);
            tma_work_slot* slot = tma_get_work_slot(pool, slot_index);
            if (tma_atomic_load_relaxed(&slot->event_signaled)) {
                // We found a handle that was already finished and we don't need to wait on all, we can exit here.
                result.index = i;
                goto exit;
            }
            if (slot->work_ring_pos < 0) {
                if (!tma_impl_dispatch_async_work(pool, slot)) {
                    single_undispatched = slot;
                    continue;
                }
            }
            {
                HANDLE event = slot->event;
                struct tma_impl_wait_context* context = &contexts[wait_handles_count];
                context->handle_index = (LONG)i;
                context->receiver = &first_completed_index;
                context->semaphore = semaphore;

                TM_ASSERT(event);
                BOOL register_result =
                    RegisterWaitForSingleObject(&wait_handles[wait_handles_count], event, tma_impl_wait_any_callback,
                                                /*Context=*/context,
                                                /*dwMilliseconds=*/INFINITE,  // We have our own timer.
                                                register_wait_flags);
                if (!register_result) {
                    result.ec = tma_winerror_to_errc(GetLastError(), TM_EPERM);
                    goto exit;
                }
                ++wait_handles_count;
            }
        }
        if (wait_handles_count == 0) {
            // There was no finished work (we would have returned already), meaning all handles are invalid.
            result.ec = TM_EPERM;
            goto exit;
        }
    } else {
        tm_bool has_finished_work = TM_FALSE;
        for (tm_size_t i = 0; i < handles_count; ++i) {
            if (!tma_is_async_handle_valid(handles[i])) continue;
            int32_t slot_index = tma_async_handle_get_index(handles[i]);
            tma_work_slot* slot = tma_get_work_slot(pool, slot_index);
            if (tma_atomic_load_relaxed(&slot->event_signaled)) {
                has_finished_work = TM_TRUE;
                continue;
            }
            if (slot->work_ring_pos < 0) {
                if (!tma_impl_dispatch_async_work(pool, slot)) {
                    if (milliseconds == INFINITE) {
                        // We are waiting on all work, if the queue is full, we can do some work here instead of
                        // waiting.
                        tma_impl_execute(slot, /*thread_id=*/0);
                        has_finished_work = TM_TRUE;
                        continue;
                    } else {
                        // Keep track of undispatched slots.
                        undispatched[undispatched_count] = slot;
                        ++undispatched_count;
                        // We still need the event of undispatched slots to be registered, so we don't break or continue
                        // here.
                    }
                }
            }
            {
                HANDLE event = slot->event;
                TM_ASSERT(event);
                BOOL register_result =
                    RegisterWaitForSingleObject(&wait_handles[wait_handles_count], event, tma_impl_wait_all_callback,
                                                /*Context=*/semaphore,
                                                /*dwMilliseconds=*/INFINITE,  // We have our own timer.
                                                register_wait_flags);
                if (!register_result) {
                    result.ec = tma_winerror_to_errc(GetLastError(), TM_EPERM);
                    goto exit;
                }
                ++wait_handles_count;
            }
        }
        // No work to be done, everything is already finished.
        if (wait_handles_count == 0) {
            result.ec = (has_finished_work) ? TM_OK : TM_EPERM;
            goto exit;
        }
    }

    // Now all handles are being waited on. All we need to do is wait on the semaphore and optionally the timer.
    // The semaphore is always on index 0 even if there is a timer.
    semaphore_and_timer_buffer[0] = semaphore;
    semaphore_and_timer_buffer_size = 1;
    if (waitable_timer) {
        semaphore_and_timer_buffer[1] = waitable_timer;
        semaphore_and_timer_buffer_size = 2;
    }

    if (wait_all) {
        // We need to wait wait_handles_count times, since that is the amount of times the semaphore will be released.
        for (tm_size_t i = 0; i < wait_handles_count; ++i) {
            wait_result = WaitForMultipleObjects(semaphore_and_timer_buffer_size, semaphore_and_timer_buffer,
                                                 /*bWaitAll=*/TM_TRUE, INFINITE);
            switch (wait_result) {
                case WAIT_OBJECT_0: {
                    // Semaphore was released.
                    // Try to dispatch as many undispatched slots as possible, otherwise we might timeout waiting on
                    // undispatched slots.
                    while (dispatched_count < undispatched_count) {
                        if (!tma_impl_dispatch_async_work(pool, undispatched[dispatched_count])) break;
                        ++dispatched_count;
                    }
                    break;
                }
                case WAIT_OBJECT_0 + 1: {
                    // Timer expired.
                    result.ec = TM_ETIMEDOUT;
                    goto exit;
                }
                case WAIT_FAILED: {
                    result.ec = tma_winerror_to_errc(GetLastError(), TM_EPERM);
                    goto exit;
                }
                default: {
                    // Unspecified error.
                    result.ec = TM_EPERM;
                    goto exit;
                }
            }
        }
    } else {
        // We only need one event to fire.
        wait_result = WaitForMultipleObjects(semaphore_and_timer_buffer_size, semaphore_and_timer_buffer,
                                             /*bWaitAll=*/TM_TRUE, INFINITE);
        switch (wait_result) {
            case WAIT_OBJECT_0: {
                // Semaphore was released.
                result.index = tma_atomic_load_seq_cst(&first_completed_index);
                result.ec = TM_OK;
                tma_work_slot* slot = tma_get_work_slot(pool, (int32_t)result.index);
                tma_atomic_store_release(&slot->event_signaled, 1);

                // Try to dispatch as many undispatched slots as possible while we are at it.
                while (dispatched_count < undispatched_count) {
                    if (!tma_impl_dispatch_async_work(pool, undispatched[dispatched_count])) break;
                    ++dispatched_count;
                }
                break;
            }
            case WAIT_OBJECT_0 + 1: {
                // Timer expired.
                result.ec = TM_ETIMEDOUT;
                goto exit;
            }
            case WAIT_FAILED: {
                result.ec = tma_winerror_to_errc(GetLastError(), TM_EPERM);
                goto exit;
            }
            default: {
                // Unspecified error.
                result.ec = TM_EPERM;
                goto exit;
            }
        }
    }

exit:
    // Mark all slots as visited if we needed to wait for all.
    if (result.ec == TM_OK && wait_all) {
        for (tm_size_t i = 0; i < handles_count; ++i) {
            if (!tma_is_async_handle_valid(handles[i])) continue;
            int32_t slot_index = tma_async_handle_get_index(handles[i]);
            tma_work_slot* slot = tma_get_work_slot(pool, slot_index);
            tma_atomic_store_release(&slot->event_signaled, 1);
        }
    }

    // Cleanup

    // Wait handles need to be cleaned up first, since they have references to some of the other allocations.
    if (wait_handles) {
        for (tm_size_t i = 0; i < wait_handles_count; ++i) {
            UnregisterWait(wait_handles[i]);
        }
        TM_FREE(wait_handles, sizeof(HANDLE) * handles_count, sizeof(void*));
    }
    if (contexts) {
        TM_FREE(contexts, sizeof(struct tma_impl_wait_context) * handles_count, sizeof(void*));
    }
    if (undispatched) {
        TM_FREE(undispatched, sizeof(tma_work_slot*) * handles_count, sizeof(void*));
    }
    if (semaphore) CloseHandle(semaphore);
    if (waitable_timer) CloseHandle(waitable_timer);

    return result;
}

static void tma_impl_init_slot(tma_work_slot* slot, tma_async_work_procedure* procedure, void* context) {
    slot->context = context;
    slot->procedure = procedure;
    TM_MEMSET(slot->storage, 0, sizeof(slot->storage));
    tma_atomic_store_release(&slot->progress_report, 0);
    tma_atomic_store_release(&slot->cancelled, 0);
    tma_atomic_store_release(&slot->event_signaled, 0);
    tma_atomic_store_release(&slot->work_ring_pos, -1);
}
static void tma_impl_init_work_slots(struct tma_work_slots* slots) {
    for (int32_t i = 0, count = (int32_t)TM_ARRAY_COUNT(slots->available_mask); i < count; ++i) {
        slots->available_mask[i] = 0xFFFFFFFFu;
    }
    slots->available_count = TMA_SLOTS_COUNT;
}

static tma_async_handle tma_impl_push_async_work(tma_thread_pool* pool, tma_async_work_procedure* procedure,
                                                 void* context, tm_bool deferred) {
    TM_ASSERT(pool);
    TM_ASSERT(procedure);
    // If this assertion hits, a thread other than the main thread (the thread that called
    // tma_initialize_async_thread_pool) issued async work. Only the main thread can issue and free async work.
    TM_ASSERT(GetCurrentThreadId() == pool->main_os_thread_id);

    struct tma_work_slots* slots = &pool->work_slots;
    int32_t slot_index = 0;
    while (slots->available_count == 0) {
        // Check overflow.
        if (INT32_MAX - slot_index < TMA_SLOTS_COUNT) {
            return tma_make_async_handle_errc(TM_EOVERFLOW);
        }
        slot_index += TMA_SLOTS_COUNT;
        if (!slots->next) {
            void* next_storage = TM_MALLOC(sizeof(struct tma_work_slots), sizeof(void*));
            if (!next_storage) return tma_make_async_handle_errc(TM_ENOMEM);
            TM_MEMSET(next_storage, 0, sizeof(struct tma_work_slots));
            struct tma_work_slots* next = (struct tma_work_slots*)next_storage;
            tma_impl_init_work_slots(next);
            slots->next = next;
        }
        slots = slots->next;
        TM_ASSERT(slots);
    }

    // Get local slot index within current slots structure.
    int32_t local_index = -1;
    int32_t mask_index = -1;
    int32_t bit_index = -1;
    for (int32_t i = 0, count = (int32_t)TM_ARRAY_COUNT(slots->available_mask); i < count; ++i) {
        if (slots->available_mask[i] == 0) continue;
        mask_index = i;
        bit_index = TM_BITSCAN_FORWARD(slots->available_mask[i]);
        local_index = bit_index + (i * 32);
        break;
    }
    TM_ASSERT(mask_index >= 0 && mask_index < (int32_t)TM_ARRAY_COUNT(slots->available_mask));
    TM_ASSERT(local_index >= 0 && local_index < (int32_t)TM_ARRAY_COUNT(slots->entries));
    tma_work_slot* slot = &slots->entries[local_index];
    slot_index += local_index;

    // Initialize event signal if it doesn't exist.
    if (!slot->event) {
        slot->event = CreateEventW(
            /*lpEventAttributes=*/TM_NULL,  // Event cannot be inherited by child processes.
            /*bManualReset=*/TM_TRUE,    // Create a manual reset event, so that we can wait on the event repeatedly and
                                         // only reset once free_async_work is called.
            /*bInitialState=*/TM_FALSE,  // Initially not signaled.
            /*lpName=*/TM_NULL           // Unnamed event.
        );
        if (!slot->event) {
            tm_errc ec = tma_winerror_to_errc(GetLastError(), TM_EPERM);
            return tma_make_async_handle_errc(ec);
        }
    }

    tma_impl_init_slot(slot, procedure, context);

    // Make slot unavailable.
    TM_ASSERT(bit_index >= 0 && bit_index < 32);
    slots->available_mask[mask_index] &= ~(1u << bit_index);
    --slots->available_count;
    TM_ASSERT(slots->available_count >= 0);

    TM_ASSERT(tma_get_work_slot(pool, slot_index) == slot);

    // Signal to threads to pick up new work.
    if (!deferred) tma_impl_dispatch_async_work(pool, slot);

    return tma_make_async_handle_index(slot_index);
}
static tm_bool tma_dispatch_async_work_by_handle(tma_thread_pool* pool, tma_async_handle handle) {
    TM_ASSERT(pool);
    TM_ASSERT(tma_is_async_handle_valid(handle));

    int32_t index = tma_async_handle_get_index(handle);
    tma_work_slot* slot = tma_get_work_slot(pool, index);
    if (slot->work_ring_pos >= 0) return TM_TRUE;
    return tma_impl_dispatch_async_work(pool, slot);
}
static void tma_impl_free_async_work(tma_thread_pool* pool, tma_async_handle handle) {
    TM_ASSERT(pool);
    if (!tma_is_async_handle_valid(handle)) return;

    int32_t index = tma_async_handle_get_index(handle);
    int32_t slots_index = index / TMA_SLOTS_COUNT;
    int32_t local_index = index % TMA_SLOTS_COUNT;
    int32_t mask_index = local_index / 32;
    int32_t bit_index = local_index % 32;
    struct tma_work_slots* slots = &pool->work_slots;
    for (int32_t i = 0; i < slots_index; ++i) {
        slots = slots->next;
        TM_ASSERT(slots);
    }
    TM_ASSERT(local_index >= 0 && local_index < (int32_t)TM_ARRAY_COUNT(slots->entries));
    TM_ASSERT(mask_index >= 0 && mask_index < (int32_t)TM_ARRAY_COUNT(slots->available_mask));
    TM_ASSERT(bit_index >= 0 && bit_index < 32);
    tma_work_slot* slot = &slots->entries[local_index];
    TM_ASSERT((slots->available_mask[mask_index] & (1u << bit_index)) == 0);

    // Check if work was put into work ring to be picked up by worker threads and is still being processed.
    if (tma_atomic_load_acquire(&slot->work_ring_pos) >= 0 && !tma_atomic_load_acquire(&slot->event_signaled)) {
        if (!tma_impl_pop_slot(&pool->work_queue, slot)) {
            // Async function was dispatched already, we have to wait for it to finish.
            TM_ASSERT(slot->event);
            WaitForSingleObject(slot->event, INFINITE);
            tma_atomic_store_release(&slot->event_signaled, 1);
        }
    }

    TMA_COMPILER_BARRIER();

    // Reset event so it can be reused.
    if (slot->event) {
        TM_ASSERT(slot->event_signaled || slot->work_ring_pos < 0);
        ResetEvent(slot->event);
    }

    tma_impl_init_slot(slot, /*procedure=*/TM_NULL, /*context=*/TM_NULL);

    // Make slot available again.
    slots->available_mask[mask_index] |= (1u << bit_index);
}

static tm_bool tma_impl_try_join(HANDLE handle, DWORD milliseconds) {
    DWORD wait_result = WaitForSingleObject(handle, milliseconds);
    switch (wait_result) {
        case WAIT_ABANDONED:
        case WAIT_OBJECT_0: {
            // Thread exited properly.
            return TM_TRUE;
        }
        case WAIT_TIMEOUT: {
            return TM_FALSE;
        }
        case WAIT_FAILED:
        default: {
            // TODO: Log error.
            return TM_TRUE;
        }
    }
}

static tm_bool tma_impl_are_workers_terminated(tma_thread_pool* pool) {
    for (tm_size_t i = 0, count = pool->workers_count; i < count; ++i) {
        if (!tma_impl_try_join(pool->workers[i].thread.handle, 0)) {
            return TM_FALSE;
        }
    }
    return TM_TRUE;
}

static void tma_impl_destroy_async_thread_pool(tma_thread_pool* pool, tm_bool completely) {
    if (!pool) return;

    if (pool->workers_shutdown_event) SetEvent(pool->workers_shutdown_event);

    // Check if any threads were created.
    if (pool->dispatch_thread.handle) {
        tm_bool workers_terminated = tma_impl_are_workers_terminated(pool);
        // We work through the work queue in a synchronized fashion. This is slow but deterministic.
        // This is just so we can properly exit without missing work even if user code failed to wait on async handles
        // (which is a bug). Nevertheless we don't want to cause lost work, so we do the remaining work single threaded.

        struct tma_work_queue* dispatch_queue = &pool->dispatch_queue;
        struct tma_work_queue* work_queue = &pool->work_queue;

        HANDLE wait_handles[2];
        wait_handles[0] = dispatch_queue->read_semaphore;
        wait_handles[1] = work_queue->read_semaphore;
        DWORD milliseconds = 0;
        for (;;) {
            // Work through queues.
            for (tm_bool has_work = TM_TRUE; has_work;) {
                DWORD wait_result = WaitForMultipleObjects(2, wait_handles, /*wait_all=*/0, milliseconds);
                milliseconds = 0;
                switch (wait_result) {
                    case WAIT_OBJECT_0: {
                        // Dispatch queue has new item.
                        tma_work_slot* slot = tma_impl_pop_force(dispatch_queue);
                        if (slot) tma_impl_execute(slot, /*thread_id=*/0);
                        break;
                    }
                    case WAIT_OBJECT_0 + 1: {
                        tma_work_slot* slot = tma_impl_pop_force(work_queue);
                        if (slot) tma_impl_execute(slot, /*thread_id=*/0);
                        break;
                    }
                    case WAIT_TIMEOUT: {
                        has_work = TM_FALSE;
                        break;
                    }
                    default: {
                        // Error
                        return;
                    }
                }
            }

            if (!workers_terminated) workers_terminated = tma_impl_are_workers_terminated(pool);
            if (workers_terminated) {
                // Workers are terminated, there was no more work in the queues. Check if dispatch thread is ready to
                // shutdown, otherwise wait for more work.
                pool->dispatch_message.request = tma_dispatch_message_ready_to_shutdown;
                SetEvent(pool->dipatch_request);
                WaitForSingleObject(pool->dipatch_answer, INFINITE);
                if (pool->dispatch_message.answer == 1) {
                    // Dispatch thread is now waiting for another message. We know that there won't be any new slots
                    // added by the dispatch thread currently. Make sure that the queues are really empty before we
                    // proceed.

                    DWORD wait_result = WaitForMultipleObjects(2, wait_handles, /*wait_all=*/0, /*milliseconds=*/0);
                    switch (wait_result) {
                        case WAIT_OBJECT_0: {
                            // Dispatch queue has new item.
                            tma_work_slot* slot = tma_impl_pop_force(dispatch_queue);
                            if (slot) tma_impl_execute(slot, /*thread_id=*/0);
                            // There was still work, resume.
                            pool->dispatch_message.request = tma_dispatch_message_resume;
                            break;
                        }
                        case WAIT_OBJECT_0 + 1: {
                            tma_work_slot* slot = tma_impl_pop_force(work_queue);
                            if (slot) tma_impl_execute(slot, /*thread_id=*/0);
                            // There was still work, resume.
                            pool->dispatch_message.request = tma_dispatch_message_resume;
                            break;
                        }
                        case WAIT_TIMEOUT:
                        default: {
                            pool->dispatch_message.request = tma_dispatch_message_shutdown_now;
                            break;
                        }
                    }
                    if (pool->dispatch_message.request == tma_dispatch_message_shutdown_now) {
                        // We are now finally able to terminate all threads.
                        SetEvent(pool->dipatch_request);
                        WaitForSingleObject(pool->dispatch_thread.handle, INFINITE);
                        break;
                    } else {
                        SetEvent(pool->dipatch_request);
                    }
                } else {
                    // Dispatch thread told us that there is more work, so we will block until we receive it.
                    // This is guaranteed to not deadlock because we made sure that all workers are terminated.
                    // This thread is the only one that can receive work now.
                    milliseconds = INFINITE;
                }
            }
        }
    }

    if (completely) {
        // Destroy queues.
        tma_impl_destroy_work_queue(&pool->work_queue);
        tma_impl_destroy_work_queue(&pool->dispatch_queue);

        // Destroy events.
        if (pool->workers_shutdown_event) {
            CloseHandle(pool->workers_shutdown_event);
            pool->workers_shutdown_event = TM_NULL;
        }
        if (pool->dipatch_request) {
            CloseHandle(pool->dipatch_request);
            pool->dipatch_request = TM_NULL;
        }
        if (pool->dipatch_answer) {
            CloseHandle(pool->dipatch_answer);
            pool->dipatch_answer = TM_NULL;
        }

        TM_FREE(pool->workers, sizeof(struct tma_worker_state) * pool->workers_count, sizeof(void*));
        pool->workers = TM_NULL;
        pool->workers_count = 0;

        struct tma_work_slots* slots = &pool->work_slots;
        while (slots) {
            for (int i = 0; i < (int)TM_ARRAY_COUNT(slots->entries); ++i) {
                tma_work_slot* slot = &slots->entries[i];
                if (slot->event) {
                    CloseHandle(slot->event);
                    slot->event = TM_NULL;
                }
            }
            struct tma_work_slots* next = slots->next;
            if (slots != &pool->work_slots) {
                TM_FREE(slots, sizeof(struct tma_work_slots), sizeof(void*));
            }
            slots = next;
        }
        TM_MEMSET(&pool->work_slots, 0, sizeof(pool->work_slots));

        // Destroy dispatch array.
        if (pool->dispatch_array.data && pool->dispatch_array.data != pool->dispatch_array.sbo) {
            TM_FREE(pool->dispatch_array.data, sizeof(tma_work_slot*) * pool->dispatch_array.capacity,
                    sizeof(tma_work_slot*));
            pool->dispatch_array.data = pool->dispatch_array.sbo;
            pool->dispatch_array.size = 0;
            pool->dispatch_array.capacity = 16;
        }
    }
}

static tm_errc tma_impl_initialize_async_thread_pool(tm_size_t threads_count,
                                                     tma_async_setup_procedure* setup_procedure, void* context,
                                                     tma_thread_pool* out) {
    TM_ASSERT(out);
    TM_ASSERT_VALID_SIZE(threads_count);
    TM_ASSERT(threads_count <= (tm_size_t)INT32_MAX);

    TM_MEMSET(out, 0, sizeof(tma_thread_pool));
    tma_impl_init_work_slots(&out->work_slots);

    out->main_os_thread_id = GetCurrentThreadId();

    if (threads_count == 0) return TM_OK;
    tm_errc result = TM_EPERM;

    // Initialize queues.
    if (!tma_impl_make_work_queue(&out->work_queue) || !tma_impl_make_work_queue(&out->dispatch_queue)) {
        result = TM_EPERM;
        goto exit;
    }
    out->dispatch_array.data = out->dispatch_array.sbo;
    out->dispatch_array.size = 0;
    out->dispatch_array.capacity = (tm_size_t)TM_ARRAY_COUNT(out->dispatch_array.sbo);

    out->workers_shutdown_event = CreateEventW(
        /*lpEventAttributes=*/TM_NULL,  // Event cannot be inherited by child processes.
        /*bManualReset=*/TM_TRUE,       // Create a manual reset event, so that we can wait on the event repeatedly and
                                        // only reset once free_async_work is called.
        /*bInitialState=*/TM_FALSE,     // Initially not signaled.
        /*lpName=*/TM_NULL              // Unnamed event.
    );
    if (!out->workers_shutdown_event) {
        result = tma_winerror_to_errc(GetLastError(), TM_EPERM);
        goto exit;
    }
    // These two events are automatic reset events, since they are used for communication between two threads.
    out->dipatch_request = CreateEventW(
        /*lpEventAttributes=*/TM_NULL,
        /*bManualReset=*/TM_FALSE,
        /*bInitialState=*/TM_FALSE,
        /*lpName=*/TM_NULL);
    if (!out->dipatch_request) {
        result = tma_winerror_to_errc(GetLastError(), TM_EPERM);
        goto exit;
    }
    out->dipatch_answer = CreateEventW(
        /*lpEventAttributes=*/TM_NULL,
        /*bManualReset=*/TM_FALSE,
        /*bInitialState=*/TM_FALSE,
        /*lpName=*/TM_NULL);
    if (!out->dipatch_answer) {
        result = tma_winerror_to_errc(GetLastError(), TM_EPERM);
        goto exit;
    }

    // Create threads array.
    out->workers = (struct tma_worker_state*)TM_MALLOC(sizeof(struct tma_worker_state) * threads_count, sizeof(void*));
    if (!out->workers) {
        result = TM_ENOMEM;
        goto exit;
    }
    out->workers_count = threads_count;

    // Create threads.
    for (int32_t i = 0, count = (int32_t)threads_count; i < count; ++i) {
        struct tma_worker_state* worker = &out->workers[i];
        worker->init.pool = out;
        worker->init.setup_procedure = setup_procedure;
        worker->init.setup_context = context;
        worker->init.context.thread_id = i + 1;

        worker->thread.handle = CreateThread(
            /*lpThreadAttributes=*/TM_NULL,              // Thread cannot be inherited from child processes.
            /*dwStackSize=*/0,                           // Default stack size.
            /*lpStartAddress=*/tma_worker_procedure,     // Pointer to the function to be executed by the
                                                         // thread.
            /*lpParameter=*/&worker->init,               // Parameter to be passed to tma_worker_procedure.
            /*dwCreationFlags=*/0,                       // Thread runs immediately after creation.
            /*lpThreadId=*/&worker->thread.os_thread_id  // Thread id returned by windows.
        );
        if (!worker->thread.handle) {
            result = tma_winerror_to_errc(GetLastError(), TM_EPERM);
            goto exit;
        }
    }

    // Create dispatch thread.
    out->dispatch_thread.handle = CreateThread(
        /*lpThreadAttributes=*/TM_NULL,                    // Thread cannot be inherited from child processes.
        /*dwStackSize=*/0,                                 // Default stack size.
        /*lpStartAddress=*/tma_dispatch_thread_procedure,  // Pointer to the function to be executed by the
                                                           // thread.
        /*lpParameter=*/out,                               // Parameter to be passed to the thread procedure.
        /*dwCreationFlags=*/0,                             // Thread runs immediately after creation.
        /*lpThreadId=*/&out->dispatch_thread.os_thread_id  // Thread id returned by windows.
    );
    if (!out->dispatch_thread.handle) {
        result = tma_winerror_to_errc(GetLastError(), TM_EPERM);
        goto exit;
    }

    result = TM_OK;
exit:
    if (result != TM_OK) {
        tma_impl_destroy_async_thread_pool(out, /*completely=*/TM_TRUE);
    }

    return result;
}

TMA_DEF tm_errc tma_initialize_async_thread_pool_ex(tm_size_t threads_count, tma_async_setup_procedure* setup_procedure,
                                                    void* context) {
    TM_ASSERT(!tma_global_thread_pool);

    tma_thread_pool* pool = (tma_thread_pool*)TM_MALLOC(sizeof(tma_thread_pool), sizeof(void*));
    if (!pool) return TM_ENOMEM;

    tma_global_thread_pool = pool;
    tm_errc result = tma_impl_initialize_async_thread_pool(threads_count, setup_procedure, context, pool);
    if (result != TM_OK) {
        TM_FREE(tma_global_thread_pool, sizeof(tma_thread_pool), sizeof(void*));
        tma_global_thread_pool = TM_NULL;
    }
    return result;
}

TMA_DEF tm_errc tma_initialize_async_thread_pool(tm_size_t threads_count) {
    return tma_initialize_async_thread_pool_ex(threads_count, /*setup_procedure=*/TM_NULL, /*context=*/TM_NULL);
}
TMA_DEF void tma_destroy_async_thread_pool(tm_bool completely) {
    tma_impl_destroy_async_thread_pool(tma_global_thread_pool, completely);
    if (completely) {
        if (tma_global_thread_pool) {
            TM_FREE(tma_global_thread_pool, sizeof(tma_thread_pool), sizeof(void*));
            tma_global_thread_pool = TM_NULL;
        }
    }
}

TMA_DEF tma_async_handle tma_push_async_work(tma_async_work_procedure* procedure, void* context, tm_bool deferred) {
    return tma_impl_push_async_work(tma_global_thread_pool, procedure, context, deferred);
}
TMA_DEF tm_bool tma_dispatch_async_work(tma_async_handle handle) {
    return tma_dispatch_async_work_by_handle(tma_global_thread_pool, handle);
}
TMA_DEF void tma_free_async_work(tma_async_handle* handle) {
    tma_impl_free_async_work(tma_global_thread_pool, *handle);
    handle->id = 0;
}

TMA_DEF void* tma_get_storage(tma_async_handle handle) {
    return tma_impl_get_storage(tma_get_work_slot_from_handle(tma_global_thread_pool, handle));
}

TMA_DEF int32_t tma_get_progress(tma_async_handle handle) {
    tma_work_slot* slot = tma_get_work_slot_from_handle(tma_global_thread_pool, handle);
    // Read progress value atomically.
    LONG progress = tma_atomic_load_acquire(&slot->progress_report);
    return (int32_t)progress;
}
TMA_DEF void tma_report_progress(tma_worker_context worker, int32_t progress) {
    TM_ASSERT(worker.tma_internal);
    tma_work_slot* slot = (tma_work_slot*)worker.tma_internal;
    tma_atomic_store_release(&slot->progress_report, (LONG)progress);
}

TMA_DEF void tma_cancel(tma_async_handle handle) {
    tma_work_slot* slot = tma_get_work_slot_from_handle(tma_global_thread_pool, handle);
    TM_ASSERT(slot);
    tma_atomic_store_relaxed(&slot->cancelled, 1);
}
/* Gets whether async work was requested to be cancelled. To be called from inside an async procedure. */
TMA_DEF tm_bool tma_is_cancelled(tma_worker_context worker) {
    TM_ASSERT(worker.tma_internal);
    tma_work_slot* slot = (tma_work_slot*)worker.tma_internal;

    // Relaxed is ok, because cancellation doesn't protect
    return tma_atomic_load_relaxed(&slot->cancelled) != 0;
}

TMA_DEF tm_errc tma_wait_single(tma_async_handle handle) {
    return tma_impl_wait_single(tma_global_thread_pool, handle, INFINITE);
}
TMA_DEF tm_errc tma_wait_single_for(tma_async_handle handle, int32_t milliseconds) {
    TM_ASSERT(milliseconds >= 0);
    return tma_impl_wait_single(tma_global_thread_pool, handle, (DWORD)milliseconds);
}
TMA_DEF tm_errc tma_wait_all(const tma_async_handle* handles, tm_size_t handles_count) {
    return tma_impl_wait_multiple(tma_global_thread_pool, handles, handles_count, /*wait_all=*/TM_TRUE, INFINITE).ec;
}
TMA_DEF tm_errc tma_wait_all_for(const tma_async_handle* handles, tm_size_t handles_count, int32_t milliseconds) {
    TM_ASSERT(milliseconds >= 0);
    return tma_impl_wait_multiple(tma_global_thread_pool, handles, handles_count, /*wait_all=*/TM_TRUE,
                                  (DWORD)milliseconds)
        .ec;
}
TMA_DEF tma_wait_any_result tma_wait_any(const tma_async_handle* handles, tm_size_t handles_count) {
    return tma_impl_wait_multiple(tma_global_thread_pool, handles, handles_count, /*wait_all=*/TM_FALSE, INFINITE);
}
TMA_DEF tma_wait_any_result tma_wait_any_for(const tma_async_handle* handles, tm_size_t handles_count,
                                             int32_t milliseconds) {
    TM_ASSERT(milliseconds >= 0);
    return tma_impl_wait_multiple(tma_global_thread_pool, handles, handles_count, /*wait_all=*/TM_FALSE,
                                  (DWORD)milliseconds);
}

TMA_DEF void tma_sleep(int32_t milliseconds) {
    TM_ASSERT(milliseconds >= 0);
    Sleep((DWORD)milliseconds);
}
#endif /* defined(_WIN32) */

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_ASYNC_IMPLEMENTATION) */

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2019 Tolga Mizrak

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
