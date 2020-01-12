void* tml::MallocAllocator::allocate_bytes(size_t size, size_t alignment) {
    TM_MAYBE_UNUSED(alignment);
    return TM_MALLOC(size, alignment);
}
void* tml::MallocAllocator::reallocate_bytes(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    TM_MAYBE_UNUSED(old_size);
    TM_MAYBE_UNUSED(alignment);
    TM_MAYBE_UNUSED(new_size);
    TM_MAYBE_UNUSED(alignment);
    return TM_REALLOC(ptr, old_size, alignment, new_size, alignment);
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