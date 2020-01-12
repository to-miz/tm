bool tml::is_power_of_two(size_t value) { return (value && !(value & (value - 1))); }
bool tml::is_pointer_aligned(const void* ptr, size_t alignment) { return ((uintptr_t)ptr % alignment) == 0; }
bool tml::is_pointer_aligned(uintptr_t ptr, size_t alignment) { return ((uintptr_t)ptr % alignment) == 0; }
size_t tml::alignment_offset(const void* ptr, size_t alignment) {
    TM_ASSERT(alignment != 0 && is_power_of_two(alignment));
    size_t offset = (alignment - ((uintptr_t)ptr)) & (alignment - 1);
    TM_ASSERT(is_pointer_aligned((char*)ptr + offset, alignment));
    return offset;
}
size_t tml::alignment_offset(uintptr_t ptr, size_t alignment) {
    TM_ASSERT(alignment != 0 && is_power_of_two(alignment));
    size_t offset = (alignment - ptr) & (alignment - 1);
    TM_ASSERT(is_pointer_aligned(ptr + offset, alignment));
    return offset;
}

#if defined(_WIN32) && defined(TMAL_HAS_WINDOWS_H_INCLUDED)
    // We can use VirtualAlloc.
    void* tml::tmal_mmap(size_t size) {
        return VirtualAlloc(/*start_address=*/nullptr, (SIZE_T)size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }
    bool tml::tmal_munmap(void* ptr, size_t size) {
        // Second parameter to VirtualFree must be 0 for MEM_RELEASE.
        if (ptr) return VirtualFree(ptr, 0, MEM_RELEASE) != 0;
        return size == 0;
    }
    size_t tml::tmal_get_mmap_granularity() {
        // TODO: Cache the allocation granularity?
        SYSTEM_INFO info = {};
        GetSystemInfo(&info);
        return (size_t)info.dwAllocationGranularity;
    }

#elif defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
    void* tml::tmal_mmap(size_t size) {
        void* mmap_result =
            mmap(/*start_address=*/nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, /*fd=*/0, /*offset=*/0);
        if (mmap_result == MAP_FAILED) return nullptr;
        return mmap_result;
    }
    bool tml::tmal_munmap(void* ptr, size_t size) { return (ptr) ? (munmap(ptr, size) == 0) : (size == 0); }
    size_t tml::tmal_get_mmap_granularity() {
        long page_size = sysconf(_SC_PAGESIZE);
        if (page_size < 0) return 4096;
        return (size_t)page_size;
    }

#else
    // Fall back on using malloc/free.
    void* tml::tmal_mmap(size_t size) { return TM_MALLOC(size, 4096); }
    bool tml::tmal_munmap(void* ptr, size_t size) {
        TM_MAYBE_UNUSED(size);
        TM_FREE(ptr, size, 4096);
        return true;
    }
    size_t tml::tmal_get_mmap_granularity() { return 4096; }

#endif