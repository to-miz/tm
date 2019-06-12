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
