static size_t MERGE_ALIGNMENT_OFFSET(const void* ptr, size_t alignment) {
#ifdef _DEBUG
    tm_bool is_power_of_two = (alignment && !(alignment & (alignment - 1)));
    TM_ASSERT(is_power_of_two);
#endif
    size_t offset = (alignment - ((uintptr_t)ptr)) & (alignment - 1);
#ifdef _DEBUG
    tm_bool is_aligned = ((uintptr_t)((const char*)ptr + offset) % alignment) == 0;
    TM_ASSERT(is_aligned);
#endif
    return offset;
}
#undef MERGE_ALIGNMENT_OFFSET