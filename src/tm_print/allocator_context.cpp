struct tmp_reallocate_result {
    char* ptr;
    tm_size_t size;
};

struct tmp_allocator_context {
    void* context;
    tmp_reallocate_result (*reallocate)(void* context, char* old_ptr, tm_size_t old_size, tm_size_t new_size);
    void (*destroy)(void* context, char* ptr, tm_size_t size);
};

static tmp_reallocate_result tmp_allocator_default_reallocate(void* /*context*/, char* old_ptr, tm_size_t old_size,
                                                              tm_size_t new_size) {
    TM_ASSERT_VALID_SIZE(new_size);

    tmp_reallocate_result result = {nullptr, 0};
    if (old_ptr && old_size > 0) {
        result.ptr =
            (char*)TM_REALLOC(old_ptr, old_size * sizeof(char), sizeof(char), new_size * sizeof(char), sizeof(char));
    } else {
        result.ptr = (char*)TM_MALLOC(new_size * sizeof(char), sizeof(char));
    }
    result.size = (result.ptr) ? new_size : 0;
    return result;
}
static void tmp_allocator_default_destroy(void* /*context*/, char* ptr, tm_size_t size) {
    (void)size;
    TM_FREE(ptr, size * sizeof(char), sizeof(char));
}

static tmp_allocator_context tmp_default_allocator() {
    return {nullptr, tmp_allocator_default_reallocate, tmp_allocator_default_destroy};
}

#ifdef TMP_USE_STL
static tmp_reallocate_result tmp_allocator_std_string_reallocate(void* context, char* /*old_ptr*/,
                                                                 tm_size_t /*old_size*/, tm_size_t new_size) {
    TM_ASSERT(context);
    TM_ASSERT_VALID_SIZE(new_size);

    auto str = static_cast<::std::string*>(context);
#if 1
    // We use two resizes, first to allocate memory, second to make use of all memory allocated.
    str->resize((size_t)new_size);
    str->resize(str->capacity());
    // TODO: This requires C++17, maybe use &((*str)[0]) instead, which isn't guaranteed to be contiguous?
    return {str->data(), (tm_size_t)str->size()};
#else
    str->resize((size_t)new_size);
    return {str->data(), new_size};
#endif
}
static void tmp_allocator_std_string_destroy(void* /*context*/, char* /*ptr*/, tm_size_t /*size*/) {}

static tmp_allocator_context tmp_std_string_allocator(::std::string* out) {
    return {static_cast<void*>(out), tmp_allocator_std_string_reallocate, tmp_allocator_std_string_destroy};
}
#endif /* defined(TMP_USE_STL) */