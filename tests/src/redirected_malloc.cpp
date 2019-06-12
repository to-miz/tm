// Redirect allocations.
size_t allocated_size = 0;
void* test_malloc(size_t size) {
    void* result = malloc(size);
    if (result) allocated_size += size;
    return result;
}
void* test_realloc(void* ptr, size_t old_size, size_t new_size) {
    void* result = realloc(ptr, new_size);
    if (result) allocated_size += new_size - old_size;
    return result;
}
void* test_realloc_in_place(void* ptr, size_t old_size, size_t new_size) {
    if (new_size <= old_size) {
        allocated_size += new_size - old_size;
        return ptr;
    }
    return nullptr;
}
void test_free(void* ptr, size_t size) {
    allocated_size -= size;
    free(ptr);
}
#define TM_MALLOC(size, alignment) test_malloc((size))
#define TM_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) test_realloc((ptr), (old_size), (new_size))
#define TM_REALLOC_IN_PLACE(ptr, old_size, old_alignment, new_size, new_alignment) \
    test_realloc_in_place((ptr), (old_size), (new_size))
#define TM_FREE(ptr, size, alignment) test_free((ptr), (size))

struct allocation_guard {
    size_t prev_allocated_size;
    allocation_guard() : prev_allocated_size(allocated_size) {}
    ~allocation_guard() { CHECK(prev_allocated_size == allocated_size); }
};