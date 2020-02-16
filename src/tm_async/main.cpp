/*
tm_async.h v0.0.2 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak MERGE_YEAR

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
    v0.0.2   01.07.19 Removed duplicate typedef of tma_atomic_int.
    v0.0.1   31.05.19 Initial Commit.
*/

/* Dependencies */
#ifdef TM_ASYNC_IMPLEMENTATION

#include "../common/tm_malloc.inc"

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

#include "../common/tm_bool.inc"

#include "../common/stdint.inc"

#include "../common/tm_size_t.inc"

#include "../common/tm_errc.inc"

#include "../common/tm_assert.inc"

#include "../common/tm_max_align.inc"

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

#include "future.h"

#endif /* defined(__cplusplus) */

#endif /* defined(_TM_ASYNC_H_INCLUDED_F1955725_9F8A_40CC_92E3_F097818D4383_) */

#ifdef TM_ASYNC_IMPLEMENTATION

#ifndef TM_MAYBE_UNUSED
    #define TM_MAYBE_UNUSED(x) ((void)(x))
#endif

#include "../common/tm_static_assert.inc"

#include "../common/tm_null.inc"

#include "../common/tm_assert_valid_size.inc"

#include "../common/tm_array_count.inc"

#ifdef __cplusplus
extern "C" {
#endif

#include "atomic_int.h"

#include "implementation.c"

#ifdef _WIN32

#define MERGE_WINERROR_TO_ERRC tma_winerror_to_errc
#include "../common/tm_winerror_to_errc.inc"

#include "implementation_windows.h"
#include "implementation_windows.c"
#endif /* defined(_WIN32) */

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_ASYNC_IMPLEMENTATION) */

#include "../common/LICENSE.inc"
