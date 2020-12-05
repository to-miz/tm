// Redirect allocations.
#include <functional>
std::function<void*(size_t)> redirect_malloc;
std::function<void*(void*, size_t)> redirect_realloc;
std::function<void*(void*, size_t)> redirect_realloc_in_place;
std::function<void(void*)> redirect_free;

std::map<void*, size_t> allocations_map;

bool malloc_fail = false;
bool realloc_fail = false;

void* test_malloc(size_t size) {
    if (malloc_fail) {
        malloc_fail = false;
        return nullptr;
    }

    void* result = nullptr;
    if (redirect_malloc) {
        result = redirect_malloc(size);
    } else {
        result = malloc(size);
    }
    if (result) allocations_map[result] = size;
    return result;
}
void* test_realloc(void* ptr, size_t new_size) {
    if (realloc_fail) {
        realloc_fail = false;
        return nullptr;
    }

    void* result = nullptr;
    if (redirect_realloc) {
        result = redirect_realloc(ptr, new_size);
    } else {
        result = realloc(ptr, new_size);
    }
    if (result) {
        auto it = allocations_map.find(ptr);
        REQUIRE(it != allocations_map.end());
        allocations_map.erase(it);
        allocations_map[result] = new_size;
    }
    return result;
}
void* test_realloc_in_place(void* ptr, size_t new_size) {
    if (redirect_realloc_in_place) {
        void* result = redirect_realloc_in_place(ptr, new_size);
        if (result) {
            auto it = allocations_map.find(ptr);
            REQUIRE(it != allocations_map.end());
            it->second = new_size;
        };
        return result;
    } else {
        return nullptr;
    }
}
void test_free(void* ptr) {
    if (redirect_free) {
        redirect_free(ptr);
    } else {
        free(ptr);
    }
    if (ptr) {
        auto it = allocations_map.find(ptr);
        REQUIRE(it != allocations_map.end());
        allocations_map.erase(it);
    }
}
#define TM_MALLOC(size, alignment) test_malloc((size))
#define TM_REALLOC(ptr, new_size, new_alignment) test_realloc((ptr), (new_size))
#define TM_REALLOC_IN_PLACE(ptr, new_size, new_alignment) test_realloc_in_place((ptr), (new_size))
#define TM_FREE(ptr) test_free((ptr))

struct allocation_guard {
    std::map<void*, size_t> prev_allocations;
    allocation_guard() : prev_allocations(allocations_map) {}
    ~allocation_guard() { CHECK(prev_allocations == allocations_map); }
};

struct redirect_guard {
    std::function<void*(size_t)> prev_malloc;
    std::function<void*(void*, size_t)> prev_realloc;
    std::function<void*(void*, size_t)> prev_realloc_in_place;
    std::function<void(void*)> prev_free;
    redirect_guard(std::function<void*(size_t)> new_malloc, std::function<void*(void*, size_t)> new_realloc,
                   std::function<void*(void*, size_t)> new_realloc_in_place,
                   std::function<void(void*)> new_free)
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