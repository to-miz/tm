tml::StackAllocator::StackAllocator(void* ptr, size_t capacity) : p((char*)ptr), cap(capacity) {
    TM_ASSERT(ptr || capacity == 0);
}

tml::MemoryBlock tml::StackAllocator::allocate_bytes(size_t size, size_t alignment /* = DEF_ALIGN*/) {
    if (!size) return {};

    auto offset = alignment_offset(end(), alignment);
    if (sz + offset + size > cap) return {};

    auto result = end() + offset;
    sz += offset + size;
    TM_ASSERT(is_pointer_aligned(result, alignment));
    last_popped_alignment = 1;
    return {result, size};
}
tml::MemoryBlock tml::StackAllocator::reallocate_bytes(void* ptr, size_t old_size, size_t new_size,
                                                       size_t alignment /* = DEF_ALIGN*/) {
    TM_ASSERT(is_valid(this));
    if (is_most_recent_allocation(ptr, old_size)) {
        if (sz + (new_size - old_size) > cap) return {};
        sz += new_size - old_size;
        return {ptr, new_size};
    } else if (new_size < old_size) {
        // no reallocation needed, but returning ptr here means we leak (old_size - new_size) bytes
        return {ptr, old_size};
    }

    auto result = allocate_bytes(new_size, alignment);
    if (result) {
        TM_MEMCPY(result.ptr, ptr, (old_size < new_size) ? (old_size) : (new_size));
        // free_bytes(ptr, old_size, alignment); // this free will fail, basically we are leaking old_size bytes
    }
    return result;
}
bool tml::StackAllocator::reallocate_bytes_in_place(void* ptr, size_t old_size, size_t new_size,
                                                    size_t /*alignment  = DEF_ALIGN*/) {
    TM_ASSERT(p);
    if (is_most_recent_allocation(ptr, old_size)) {
        if (sz + (new_size - old_size) <= cap) {
            sz += new_size - old_size;
            return true;
        }
    }
    return false;
}
void tml::StackAllocator::free_bytes(void* ptr, size_t size, size_t alignment /* = DEF_ALIGN*/) {
    TM_ASSERT(is_valid(this));

    // if this assertion triggers, ptr is being freed using a wrong allocator
    TM_ASSERT(!ptr || owns(ptr));

    // this assertion triggers if free isn't called in reverse order of allocation or there were interlocking
    // allocations/free's with differing alignments
    TM_ASSERT(!ptr || size == 0 || is_most_recent_allocation(ptr, size));
    if (ptr && is_most_recent_allocation(ptr, size)) {
        sz -= size + alignment_offset((const char*)ptr + size, last_popped_alignment);
        last_popped_alignment = alignment;
    }
}
bool tml::StackAllocator::is_most_recent_allocation(const void* ptr, size_t size) const {
    const char* end_ptr = (const char*)ptr + size;
    // equality on arbitrary pointers is specified behavior
    return end_ptr + alignment_offset(end_ptr, last_popped_alignment) == end();
}
bool tml::StackAllocator::owns(const void* ptr) const {
    if (!ptr) return true;
    const char* storage = (const char*)ptr;
    // On platforms/compilers where this library is supported this comparison is fine even though technically its
    // unspecified behaviour, so we might want to use std::less<> or cast to uintptr_t later.
    auto result = storage >= p && storage < p + sz;
    TM_ASSERT(result || !(storage >= p && storage < p + cap));
    return result;
}
size_t tml::StackAllocator::remaining(size_t alignment) const {
    TM_ASSERT(is_valid(this));
    auto result = cap - sz;
    auto offset = alignment_offset(end(), alignment);
    if (result >= offset) {
        result -= offset;
    } else {
        // no room to align
        result = 0;
    }
    return result;
}
void tml::StackAllocator::clear() {
    sz = 0;
    last_popped_alignment = 1;
}
void tml::StackAllocator::set_state(StackAllocator::StateSnapshot snapshot) {
    sz = snapshot.sz;
    last_popped_alignment = snapshot.last_popped_alignment;
}

tml::DynamicStackAllocator::DynamicStackAllocator(size_t capacity) {
    if (capacity) {
        p = (char*)TM_MALLOC(capacity, TM_DEFAULT_ALIGNMENT);
        if (p) cap = capacity;
    }
}
tml::DynamicStackAllocator::DynamicStackAllocator(DynamicStackAllocator&& other) {
    p = other.p;
    sz = other.sz;
    cap = other.cap;
    last_popped_alignment = other.last_popped_alignment;
    other.p = nullptr;
    other.sz = 0;
    other.cap = 0;
    other.last_popped_alignment = 1;
}
tml::DynamicStackAllocator& tml::DynamicStackAllocator::operator=(DynamicStackAllocator&& other) {
    if (this != &other) {
        if (p) {
            TM_FREE(p);
            p = nullptr;
        }
        p = other.p;
        sz = other.sz;
        cap = other.cap;
        last_popped_alignment = other.last_popped_alignment;
        other.p = nullptr;
        other.sz = 0;
        other.cap = 0;
        other.last_popped_alignment = 1;
    }
    return *this;
}
tml::DynamicStackAllocator::~DynamicStackAllocator() {
    if (p) {
        TM_FREE(p);
        p = nullptr;
    }
}