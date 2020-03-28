tml::MemoryBlock tml::MallocAllocator::allocate_bytes(size_t size, size_t alignment) {
    TM_MAYBE_UNUSED(alignment);
    void* ptr = TM_MALLOC(size, alignment);
    return {ptr, (ptr) ? size : 0};
}
tml::MemoryBlock tml::MallocAllocator::reallocate_bytes(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    TM_MAYBE_UNUSED(old_size);
    TM_MAYBE_UNUSED(alignment);
    TM_MAYBE_UNUSED(new_size);
    TM_MAYBE_UNUSED(alignment);
    void* new_ptr = TM_REALLOC(ptr, old_size, alignment, new_size, alignment);
    return {new_ptr, (new_ptr) ? new_size : 0};
}
bool tml::MallocAllocator::reallocate_bytes_in_place(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    TM_MAYBE_UNUSED(ptr);
    TM_MAYBE_UNUSED(old_size);
    TM_MAYBE_UNUSED(new_size);
    TM_MAYBE_UNUSED(alignment);
#ifdef TM_REALLOC_IN_PLACE
    return TM_REALLOC_IN_PLACE(ptr, old_size, alignment, new_size, alignment);
#else
    return false;
#endif
}
void tml::MallocAllocator::free_bytes(void* ptr, size_t size, size_t alignment) {
    TM_MAYBE_UNUSED(size);
    TM_MAYBE_UNUSED(alignment);
    TM_FREE(ptr, size, alignment);
}