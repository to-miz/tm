// Redirect allocations.
#include <functional>
std::function<void*(size_t)> redirect_malloc;
std::function<void*(void*, size_t, size_t)> redirect_realloc;
std::function<void*(void*, size_t, size_t)> redirect_realloc_in_place;
std::function<void(void*, size_t)> redirect_free;

size_t allocated_size = 0;
void* test_malloc(size_t size) {
    void* result = nullptr;
    if (redirect_malloc) {
        result = redirect_malloc(size);
    } else {
        result = malloc(size);
    }
    if (result) allocated_size += size;
    return result;
}
void* test_realloc(void* ptr, size_t old_size, size_t new_size) {
    void* result = nullptr;
    if (redirect_realloc) {
        result = redirect_realloc(ptr, old_size, new_size);
    } else {
        result = realloc(ptr, new_size);
    }
    if (result) allocated_size += new_size - old_size;
    return result;
}
void* test_realloc_in_place(void* ptr, size_t old_size, size_t new_size) {
    if (redirect_realloc_in_place) {
        void* result = redirect_realloc_in_place(ptr, old_size, new_size);
        if (result) allocated_size += new_size - old_size;
        return result;
    } else {
        if (new_size <= old_size) {
            allocated_size += new_size - old_size;
            return ptr;
        }
        return nullptr;
    }
}
void test_free(void* ptr, size_t size) {
    if (redirect_free) {
        redirect_free(ptr, size);
    } else {
        free(ptr);
    }
    allocated_size -= size;
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

struct redirect_guard {
    std::function<void*(size_t)> prev_malloc;
    std::function<void*(void*, size_t, size_t)> prev_realloc;
    std::function<void*(void*, size_t, size_t)> prev_realloc_in_place;
    std::function<void(void*, size_t)> prev_free;
    redirect_guard(std::function<void*(size_t)> new_malloc, std::function<void*(void*, size_t, size_t)> new_realloc,
                   std::function<void*(void*, size_t, size_t)> new_realloc_in_place,
                   std::function<void(void*, size_t)> new_free)
        : prev_malloc(redirect_malloc),
          prev_realloc(redirect_realloc),
          prev_realloc_in_place(redirect_realloc_in_place),
          prev_free(redirect_free) {
        redirect_malloc = new_malloc;
        redirect_realloc = new_realloc;
        redirect_realloc_in_place = new_realloc_in_place;
        redirect_free = new_free;
    }
    ~redirect_guard() {
        redirect_malloc = prev_malloc;
        redirect_realloc = prev_realloc;
        redirect_realloc_in_place = prev_realloc_in_place;
        redirect_free = prev_free;
    }
};