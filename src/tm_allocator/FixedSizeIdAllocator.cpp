namespace tml {

FixedSizeIdAllocator::FixedSizeIdAllocator(int32_t initial_element_capacity, int32_t element_size_in_bytes,
                                           int32_t element_alignment)
    : allocation_size(element_size_in_bytes), allocation_alignment(element_alignment) {
    TM_ASSERT(allocation_size >= (int32_t)sizeof(int32_t));
    if (allocation_alignment < (int32_t)sizeof(int32_t)) allocation_alignment = (int32_t)sizeof(int32_t);
    if (initial_element_capacity <= 0) return;

    data = TM_MALLOC(allocation_size * initial_element_capacity, allocation_alignment);
    if (data) capacity = initial_element_capacity;
}
FixedSizeIdAllocator::~FixedSizeIdAllocator() {
    if (data) {
        TM_FREE(data);
        data = nullptr;
    }
}
FixedSizeIdAllocator::FixedSizeIdAllocator(FixedSizeIdAllocator&& other)
    : data(other.data),
      size(other.size),
      capacity(other.capacity),
      free_index(other.free_index),
      allocation_size(other.allocation_size),
      allocation_alignment(other.allocation_alignment) {
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
    other.free_index = -1;
}
FixedSizeIdAllocator& FixedSizeIdAllocator::operator=(FixedSizeIdAllocator&& other) {
    if (this != &other) {
        if (data) {
            TM_FREE(data);
        }
        data = other.data;
        size = other.size;
        capacity = other.capacity;
        free_index = other.free_index;
        allocation_size = other.allocation_size;
        allocation_alignment = other.allocation_alignment;

        other.data = nullptr;
    }
    return *this;
}

uint32_t FixedSizeIdAllocator::create() {
    TM_ASSERT(data);
    if (auto id = pop()) return id;

    if (size >= capacity) {
        auto new_capacity = next_capacity(capacity);
        auto new_data = TM_REALLOC(data, new_capacity * allocation_size, allocation_alignment);
        if (new_data) {
            data = new_data;
            capacity = (int32_t)new_capacity;
        }
    }

    if (size < capacity) {
        size++;
        TM_ASSERT(size > 0);
        return (uint32_t)size;
    }
    return 0;
}
void FixedSizeIdAllocator::destroy(uint32_t id) {
    if (data_from_id(id)) push(id);
}
void* FixedSizeIdAllocator::data_from_id(uint32_t id) {
    if (id > 0 && (int32_t)(id - 1) < size) return (char*)data + ((id - 1) * allocation_size);
    return nullptr;
}
uint32_t FixedSizeIdAllocator::pop() {
    if (free_index >= 0) {
        TM_ASSERT(free_index < size);
        auto result = (uint32_t)(free_index + 1);
        free_index = *((int32_t*)((char*)data + free_index * allocation_size));
        return result;
    }
    return 0;
}
void FixedSizeIdAllocator::push(uint32_t id) {
    TM_ASSERT(id > 0);
    auto index = id - 1;
    auto new_free = (int32_t*)((char*)data + index * allocation_size);
    *new_free = free_index;
    free_index = index;
}

#include "FixedSizeGenerationalIdAllocator.cpp"

}  // namespace tml