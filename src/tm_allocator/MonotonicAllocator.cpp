tml::MonotonicAllocator::MonotonicAllocator() : block_size(tmal_get_mmap_granularity()) {
    allocators = (StackAllocator*)TM_MALLOC(sizeof(StackAllocator), TM_DEFAULT_ALIGNMENT);
    if (allocators) {
        current = 0;
        capacity = 1;
    }
}
tml::MonotonicAllocator::MonotonicAllocator(MonotonicAllocator&& other)
    : allocators(other.allocators),
      current(other.current),
      capacity(other.capacity),
      block_size(other.block_size),
      leak_memory(other.leak_memory) {
    other.allocators = nullptr;
    other.current = 0;
    other.capacity = 0;
    other.block_size = 0;
    other.leak_memory = false;
}
tml::MonotonicAllocator& tml::MonotonicAllocator::operator=(MonotonicAllocator&& other) {
    if (this != &other) {
        if (!leak_memory && allocators) {
            TM_FREE(allocators);
        }
        allocators = other.allocators;
        current = other.current;
        capacity = other.capacity;
        block_size = other.block_size;
        leak_memory = other.leak_memory;
        other.allocators = nullptr;
        other.current = 0;
        other.capacity = 0;
        other.block_size = 0;
        other.leak_memory = false;
    }
    return *this;
}
tml::MonotonicAllocator::~MonotonicAllocator() {
    if (!leak_memory && allocators) {
        TM_FREE(allocators);
        allocators = nullptr;
    }
}

tml::MemoryBlock tml::MonotonicAllocator::allocate_bytes_throws(size_t size,
                                                                size_t alignment /*= TM_DEFAULT_ALIGNMENT*/) {
    auto result = allocate_bytes(size, alignment);
    if (!result) {
#ifndef TMAL_NO_STL
        throw std::bad_alloc();
#else
        throw nullptr;
#endif
    }
    return result;
}
tml::MemoryBlock tml::MonotonicAllocator::allocate_bytes(size_t size, size_t alignment /*= TM_DEFAULT_ALIGNMENT*/) {
    TM_ASSERT(allocators);
    if (auto result = allocators[current].allocate_bytes(size, alignment)) return result;

    auto new_capacity = capacity + 1;
    auto new_allocators = TM_REALLOC(allocators, new_capacity * sizeof(StackAllocator), TM_DEFAULT_ALIGNMENT);
    if (!new_allocators) return {};
    allocators = (StackAllocator*)new_allocators;
    capacity = new_capacity;

    auto allocator_index = new_capacity - 1;
    allocators[allocator_index] = {};
    size_t allocation_size = block_size;

    if (size <= block_size) {
        // Allocation size fits in a single block.
        current = allocator_index;
    } else {
        // Allocation size is more than the block size of a single monotonic allocator.
        // So we create memory just for this single allocation without making the resulting allocator the current
        // one, since it is immediately empty.
        allocation_size = size;
    }

    void* block = tmal_mmap(allocation_size);
    if (!block) return {};
    allocators[allocator_index] = {block, allocation_size};
    return allocators[allocator_index].allocate_bytes(size, alignment);
}
bool tml::MonotonicAllocator::owns(const void* ptr) const {
    if (!ptr) return true;
    for (int i = 0, count = capacity; i < count; ++i) {
        if (allocators[i].owns(ptr)) return true;
    }
    return false;
}

tml::StackAllocator* tml::MonotonicAllocator::current_stack_allocator() {
    TM_ASSERT(current >= 0 && current < capacity);
    return &allocators[current];
}