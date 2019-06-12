#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if 1
#include <system_error>
#define TM_ERRC_DEFINED
#define TM_OK std::errc()
#define TM_EPERM std::errc::operation_not_permitted
#define TM_ENOENT std::errc::no_such_file_or_directory
#define TM_EIO std::errc::io_error
#define TM_EAGAIN std::errc::resource_unavailable_try_again
#define TM_ENOMEM std::errc::not_enough_memory
#define TM_EACCES std::errc::permission_denied
#define TM_EBUSY std::errc::device_or_resource_busy
#define TM_EEXIST std::errc::file_exists
#define TM_EXDEV std::errc::cross_device_link
#define TM_ENODEV std::errc::no_such_device
#define TM_EINVAL std::errc::invalid_argument
#define TM_EMFILE std::errc::too_many_files_open
#define TM_EFBIG std::errc::file_too_large
#define TM_ENOSPC std::errc::no_space_on_device
#define TM_ERANGE std::errc::result_out_of_range
#define TM_ENAMETOOLONG std::errc::filename_too_long
#define TM_ENOLCK std::errc::no_lock_available
#define TM_ECANCELED std::errc::operation_canceled
#define TM_ENOSYS std::errc::function_not_supported
#define TM_ENOTEMPTY std::errc::directory_not_empty
#define TM_EOVERFLOW std::errc::value_too_large
#define TM_ETIMEDOUT std::errc::timed_out
typedef std::errc tm_errc;
#endif

#define TMA_NO_TMA_PREFIX
#include "main.cpp"
#define TM_ASYNC_IMPLEMENTATION
#include "main.cpp"

using namespace tml;

void async_test() {
    struct AsyncResult {
        int32_t thread;
        int32_t value;
    };
    std::vector<future<AsyncResult>> futures;
    int test = 0;
    auto first = [&](worker_context context) {
        sleep(10);
        test = 12;
        return AsyncResult{context.thread_id, 0};
    };
    futures.push_back(async(first));
    futures.push_back(async([](worker_context context) {
        sleep(10);
        return AsyncResult{context.thread_id, 1};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(100);
        return AsyncResult{context.thread_id, 2};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(20);
        return AsyncResult{context.thread_id, 3};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(5);
        return AsyncResult{context.thread_id, 1};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(6);
        return AsyncResult{context.thread_id, 2};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(7);
        return AsyncResult{context.thread_id, 3};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(10);
        return AsyncResult{context.thread_id, 1};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(1);
        return AsyncResult{context.thread_id, 2};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(2);
        return AsyncResult{context.thread_id, 3};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(3);
        return AsyncResult{context.thread_id, 1};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(6);
        return AsyncResult{context.thread_id, 2};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(7);
        return AsyncResult{context.thread_id, 3};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(10);
        return AsyncResult{context.thread_id, 1};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(4);
        return AsyncResult{context.thread_id, 2};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(20);
        return AsyncResult{context.thread_id, 3};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(19);
        return AsyncResult{context.thread_id, 1};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(6);
        return AsyncResult{context.thread_id, 2};
    }));
    futures.push_back(async([](worker_context context) {
        sleep(1);
        return AsyncResult{context.thread_id, 3};
    }));

    while (futures.size()) {
        auto wait_result = wait_any(futures.data(), futures.size());
        if (wait_result.ec != TM_OK) {
            printf("Wait failed.\n");
            break;
        }
        auto result = futures[wait_result.index].get();
        printf("Thread %d returned %d.\n", result.thread, result.value);
        futures.erase(futures.begin() + wait_result.index);
    }

    printf("Test is %d.\n", test);
}

void async_report_test() {
    auto f = async([](worker_context worker) {
        for (auto i = 0; i < 100; ++i) {
            report_progress(worker, i);
            sleep(10);
        }
    });

    tm_errc wait_result = TM_OK;
    while ((wait_result = f.wait_for(10)) == TM_ETIMEDOUT) {
        printf("Progress: %d\n", f.progress());
    }
    if (wait_result == TM_OK) {
        printf("Finished!\n");
    } else {
        printf("Error!\n");
    }
}

int64_t init_ticks_per_second() {
    LARGE_INTEGER i;
    if (!QueryPerformanceFrequency(&i)) {
        return 1;
    }
    return i.QuadPart;
}
static const int64_t ticks_per_second = init_ticks_per_second();

int64_t get_ticks() {
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    return i.QuadPart;
}
double ticks_to_ms(int64_t t) { return t * (1000.0 / ticks_per_second); }

void async_wait_on_many_test() {
    const int size = 512;
    struct Functor {
        int i;
        bool finished = false;
        void operator()(worker_context) {
            sleep(50);
            finished = true;
        }
    };
    struct FutureData {
        Functor functors[size];
        future<void> futures[size];

        FutureData() {
            for (int i = 0; i < size; ++i) {
                functors[i] = {(i > 50) ? 50 : i};
                futures[i] = async(functors[i], /*deferred=*/true);
            }
        }
        ~FutureData() {
            for (auto& entry : futures) {
                entry.clear();
            }
        }
    };

    auto start = get_ticks();
    {
        bool succeeded = false;
        FutureData data;
        printf("Wait all:\n");
        succeeded = (wait_all(data.futures, size) == TM_OK);
        if (succeeded) {
            for (auto& entry : data.functors) {
                assert(entry.finished);
                if (!entry.finished) {
                    succeeded = false;
                    break;
                }
            }
        }
        if (succeeded) {
            printf("Wait all succeeded.\n");
        } else {
            printf("Wait all failed.\n");
        }
    }
    auto end = get_ticks();
    printf("Wait all took %.2fs.\n", ticks_to_ms(end - start) / 1000);
#if 0
    {
        FutureData data;
        printf("Wait all poll:\n");
        tm_errc wait_result = {};
        while ((wait_result = wait_all_for(data.futures, size, 0)) == TM_ETIMEDOUT);
        if (wait_result == TM_OK) {
            printf("Wait all succeeded.\n");
        } else {
            printf("Wait all failed.\n");
        }
    }
    {
        FutureData data;
        printf("Wait any:\n");
        wait_any_result wait_result = {};
        for (int i = 0; i < size; ++i) {
            wait_result = wait_any(data.futures, size);
            if (wait_result.ec != TM_OK) break;
            printf("Work index %d finished.\n", (int)wait_result.index);
            data.futures[wait_result.index].clear();
        }
        if (wait_result.ec == TM_OK) {
            printf("Work any succeeded.\n");
        } else {
            printf("Work any failed.\n");
        }
    }
    {
        FutureData data;
        printf("Wait any poll:\n");
        wait_any_result wait_result = {};
        for (int i = 0; i < size; ++i) {
            while ((wait_result = wait_any_for(data.futures, size, 0)).ec == TM_ETIMEDOUT);
            if (wait_result.ec != TM_OK) break;
            printf("Work index %d finished.\n", (int)wait_result.index);
            data.futures[wait_result.index].clear();
        }
        if (wait_result.ec == TM_OK) {
            printf("Work any succeeded.\n");
        } else {
            printf("Work any failed.\n");
        }
    }
#endif
}

int main() {
    initialize_async_thread_pool(4);

    async_wait_on_many_test();
#if 0
    async_report_test();
#endif
#if 0
    for (int i = 0; i < 1000; ++i) {
        async_test();
    }
#endif

    destroy_async_thread_pool(/*completely=*/true);
    return 0;
}