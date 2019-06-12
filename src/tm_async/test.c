#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define TMA_USE_TMA_PREFIX
#include "main.cpp"
#define TM_ASYNC_IMPLEMENTATION
#include "main.cpp"

struct async_data {
    int32_t thread;
    int32_t value;
};

void async_func(tma_worker_context worker, void* context) {
    struct async_data* data = (struct async_data*)context;

    tma_sleep(100);
    data->thread = worker.thread_id;
}

void async_any_test() {
    printf("\nasync_any_test:\n");

    struct async_data data[10];
    tma_async_handle handles[10];
    size_t handles_count = 10;
    for (size_t i = 0; i < handles_count; ++i) {
        data[i].value = (int32_t)i;
        handles[i] = tma_push_async_work(async_func, &data[i], 0);
    }

    for (size_t i = 0; i < handles_count; ++i) {
        tma_wait_any_result wait_result = tma_wait_any(handles, handles_count);
        if (wait_result.ec != TM_OK) {
            printf("Wait failed.\n");
            break;
        }
        printf("Thread %d returned %d.\n", data[wait_result.index].thread, data[wait_result.index].value);
        tma_free_async_work(&handles[wait_result.index]);
    }

    for (size_t i = 0; i < handles_count; ++i) {
        tma_free_async_work(&handles[i]);
    }
}

void async_all_test() {
    printf("\nasync_all_test:\n");

    struct async_data data[10];
    tma_async_handle handles[10];
    size_t handles_count = 10;
    for (size_t i = 0; i < handles_count; ++i) {
        data[i].value = (int32_t)i;
        handles[i] = tma_push_async_work(async_func, &data[i], 0);
    }

    if (tma_wait_all(handles, handles_count) == TM_OK) {
        for (size_t i = 0; i < handles_count; ++i) {
            printf("Thread %d returned %d.\n", data[i].thread, data[i].value);
        }
    } else {
        printf("Wait failed.\n");
    }

    for (size_t i = 0; i < handles_count; ++i) {
        tma_free_async_work(&handles[i]);
    }
}

int main() {
    tma_initialize_async_thread_pool(4);

    for (int i = 0; i < 20; ++i) {
        async_any_test();
        async_all_test();
    }

    tma_destroy_async_thread_pool(/*completely=*/1);
    return 0;
}