// Example Usage Code

struct async_file_read {
    // Note how the header member is always the first member in the struct. This is important for how the strided async
    // handling works.
    tma_async_header async_header;

    const char *filename;
    long filesize;
    char *contents;
};

// We read the file in chunks of 2048 bytes for this example.
#define CHUNK_SIZE 2048
void TMA_ASYNC_CALL async_read_file_worker(tma_worker_context worker, void *context) {
    // No error checking for the sake of brevity.
    struct async_file_read *data = (struct async_file_read *)context;

    FILE *f = fopen(data->filename, "rb");

    // Lets assume that the standard library and the file we want to read from support SEEK_END for the sake of this
    // example.
    fseek(f, 0, SEEK_END);
    data->filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    data->contents = malloc(data->filesize);

    char *current = data->contents;
    long chunks_count = data->filesize / CHUNK_SIZE;
    long last_chunk_size = data->filesize % CHUNK_SIZE;
    for (int i = 0; i < chunks_count; ++i) {
        fread(current, sizeof(char), CHUNK_SIZE, f);
        tma_report_progress(worker, i + 1);  // Report file read progress.
        // The progress counter is always initialized to 0 at the beginning, we add 1 to denote that work has been done.
        // Note how by the time we report progress the first time, data->filesize and data->contents have meaningful
        // data.

        current += CHUNK_SIZE;
    }
    // Read last chunk.
    fread(current, sizeof(char), last_chunk_size, f);
    tma_report_progress(worker, chunks_count + 1);

    fclose(f);
}

void async_read_file(const char *filename, struct async_file_read *out) {
    out->filename = filename;
    // Note how we pass the size of the whole structure to tma_push_async_work_strided, not just the size of the header.
    tma_push_async_work_strided(async_read_file_worker, /*async_context=*/&out->header, /*stride=*/sizeof(*out),
                                /*deferred=*/0);
}

struct async_big_job {
    tma_async_header async_header;

    int some_data[256];
    int result;
};

void TMA_ASYNC_CALL async_big_job_worker(tma_worker_context worker, void *context) {
    struct async_big_job *data = (struct async_big_job *)context;

    // Contrieved example.
    int result = 0;
    for (int i = 0; i < 256; ++i) {
        result += data->some_data[i];
    }
    data->result = result;
}

void async_do_big_job(struct async_big_job *out) {
    // Initialize out in some meaningful way.
    for (int i = 0; i < 256; ++i) {
        // Alternating between -1 and 1, why not.
        out->some_data[i] = (i % 2) * 2 - 1;
    }
    tma_push_async_work_strided(async_big_job_worker, /*async_context=*/&out->header,
                                /*stride=*/sizeof(*out), /*deferred=*/0);
}

struct async_ultimate_answer {
    tma_async_header async_header;

    int ultimate_answer;
};

void TMA_ASYNC_CALL async_ultimate_answer_worker(tma_worker_context worker, void *context) {
    struct async_ultimate_answer *data = (struct async_ultimate_answer *)context;
    data->ultimate_answer = 42;
}

void async_get_ultimate_answer(struct async_ultimate_answer *out) {
    tma_push_async_work_strided(async_ultimate_answer_worker,
                                /*async_context=*/out,
                                /*stride=*/sizeof(*out),
                                /*deferred*/ 0);
}

// Here is how you now would use these async calls:
void do_some_work() {
    // A structure containing all our async work loads.
    // Note how all async structures come one after another.
    struct {
        struct async_file_read file;
        struct async_big_job big;
        struct async_ultimate_answer ultimate;
    } async_jobs;
    const int async_jobs_count = 3;  // async_jobs has 3 jobs.

    // Launch async jobs.
    async_read_file("some_file.txt", &async_jobs.file);
    async_do_big_job(&async_jobs.big);
    async_get_ultimate_answer(&async_jobs.ultimate);

    // Poll completion.
    int finished_jobs = 0;
    tma_wait_any_result wait_result;
    while (finished_jobs < async_jobs_count) {
        // We wait on all jobs at once by passing in the jobs structure itself.
        // We wait for 0 milliseconds, basically polling the completion status.
        wait_result = tma_wait_any_for_strided(&async_jobs.file.header, async_jobs_count,
                                               /*milliseconds=*/10);
        switch (wait_result.index) {
            case 0: {
                // Read file completed.
                printf("File %s was read fully.\n", async_jobs.file.filename);
                ++finished_jobs;
                break;
            }
            case 1: {
                // Big job completed.
                printf("Big job completed, result is %d.\n", async_jobs.big.result);
                ++finished_jobs;
                break;
            }
            case 2: {
                // We got the ultimate answer.
                printf("The ultimate answer is: %d.\n", async_jobs.ultimate.ultimate_answer);
                ++finished_jobs;
                break;
            }
            case TMA_WAIT_TIMEOUT_INDEX: {
                // The wait timed out, process progress of the file read.
                int file_read_progress = tma_get_progress(&async_jobs.file);
                if (file_read_progress > 0) {
                    // There is meaningful data already.
                    long bytes_read = file_read_progress * CHUNK_SIZE;
                    if (bytes_read > data->filesize) bytes_read = data->filesize;
                    printf("%s: %d of %d bytes read.\n", data->filename, bytes_read, data->filesize);
                }
                break;
            }
            case TMA_WAIT_ERROR_INDEX:
            default: {
                // An error occured when waiting. wait_result.ec will contain a POSIX error code.
                printf("Error occured with error code: %d.", wait_result.ec);
                finished_jobs = async_jobs_count;  // Break out of outer loop.
                break;
            }
        }
    }

    // Free all async data at once for all jobs.
    tma_free_async_work_strided(&async_jobs, async_jobs_count);
    free(async_jobs.file.contents);
}