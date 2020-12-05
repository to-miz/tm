FixedSizeGenerationalIdAllocator::FixedSizeGenerationalIdAllocator(int32_t initial_element_capacity,
                                                                   int32_t element_size_in_bytes,
                                                                   int32_t element_alignment)
    : allocation_size(element_size_in_bytes), allocation_alignment(element_alignment) {
    if (allocation_alignment < (int32_t)sizeof(int32_t)) allocation_alignment = (int32_t)sizeof(int32_t);
    // Increase the allocation size by the size of the header.
    allocation_size += (int32_t)(sizeof(uint32_t) + alignment_offset(sizeof(uint32_t), element_alignment));

    if (initial_element_capacity <= 0) return;

    data = TM_MALLOC(allocation_size * initial_element_capacity, allocation_alignment);
    if (data) capacity = initial_element_capacity;
}
FixedSizeGenerationalIdAllocator::~FixedSizeGenerationalIdAllocator() {
    if (data) {
        TM_FREE(data);
        data = nullptr;
    }
}
FixedSizeGenerationalIdAllocator::FixedSizeGenerationalIdAllocator(FixedSizeGenerationalIdAllocator&& other)
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
FixedSizeGenerationalIdAllocator& FixedSizeGenerationalIdAllocator::operator=(
    FixedSizeGenerationalIdAllocator&& other) {
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
uint32_t FixedSizeGenerationalIdAllocator::create() {
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
        TM_ASSERT(size != INT32_MAX);
        auto header = (Header*)((char*)data + size * allocation_size);
        *header = Header::make(/*generation=*/0, size, /*occupied=*/true);
        size++;
        return header->bits;
    }
    return 0;
}
void FixedSizeGenerationalIdAllocator::destroy(uint32_t id) {
    if (base_from_id(id)) push(id);
}
void* FixedSizeGenerationalIdAllocator::data_from_id(uint32_t id) {
    if (auto base = base_from_id(id)) return body_from_base(base);
    return nullptr;
}
uint32_t FixedSizeGenerationalIdAllocator::pop() {
    if (free_index > 0) {
        TM_ASSERT(free_index <= size);
        auto header = (Header*)((char*)data + free_index * allocation_size);
        auto result = Header::make(header->generation(), (uint32_t)free_index, /*occupied=*/true);
        free_index = header->id();
        return result.bits;
    }
    return 0;
}
void FixedSizeGenerationalIdAllocator::push(uint32_t id) {
    auto index = Header::id(id);
    auto new_free = (Header*)((char*)data + index * allocation_size);
    TM_ASSERT(new_free->bits == id);
    *new_free = Header::make(new_free->generation() + 1, free_index, /*occupied=*/false);
    free_index = index;
}
void* FixedSizeGenerationalIdAllocator::base_from_id(uint32_t id) {
    if (!Header::occupied(id)) return nullptr;
    auto index = Header::id(id);
    if ((int32_t)index >= size) return nullptr;
    auto result = (char*)data + (Header::id(id) * allocation_size);
    auto header = (Header*)result;
    if (header->bits != id) return nullptr;
    return result;
}
void* FixedSizeGenerationalIdAllocator::body_from_base(void* header) {
    auto end_of_header = (char*)header + sizeof(uint32_t);
    return end_of_header + alignment_offset(end_of_header, allocation_alignment);
}

FixedSizeGenerationalIdAllocator::ForwardIterator::ForwardIterator(void* data, int32_t size, int32_t allocation_size,
                                                                   int32_t allocation_alignment)
    : data(data), size(size), allocation_size(allocation_size), allocation_alignment(allocation_alignment) {
    if (size) {
        if (!((Header*)data)->occupied()) ++(*this);
    }
}

bool FixedSizeGenerationalIdAllocator::ForwardIterator::operator==(const ForwardIterator& other) const {
    return data == other.data && size == other.size && allocation_size == other.allocation_size;
}
bool FixedSizeGenerationalIdAllocator::ForwardIterator::operator!=(const ForwardIterator& other) const {
    return data != other.data || size != other.size || allocation_size != other.allocation_size;
}
uint32_t FixedSizeGenerationalIdAllocator::ForwardIterator::id() const { return ((Header*)data)->bits; }
void* FixedSizeGenerationalIdAllocator::ForwardIterator::operator*() const {
    auto end_of_header = (char*)data + sizeof(uint32_t);
    return end_of_header + alignment_offset(end_of_header, allocation_alignment);
}
FixedSizeGenerationalIdAllocator::ForwardIterator FixedSizeGenerationalIdAllocator::ForwardIterator::operator++() {
    // Prefix increment.
    TM_ASSERT(size);
    char* p = (char*)data + allocation_size;
    --size;
    do {
        auto header = (Header*)p;
        if (header->occupied()) break;
        p += allocation_size;
        --size;
    } while (size);
    data = p;
    return *this;
}
FixedSizeGenerationalIdAllocator::ForwardIterator FixedSizeGenerationalIdAllocator::ForwardIterator::operator++(int) {
    // Postfix increment.
    auto previous = *this;
    this->operator++();
    return previous;
}

FixedSizeGenerationalIdAllocator::ConstForwardIterator::ConstForwardIterator(void* data, int32_t size,
                                                                             int32_t allocation_size,
                                                                             int32_t allocation_alignment)
    : data(data), size(size), allocation_size(allocation_size), allocation_alignment(allocation_alignment) {
    if (size) {
        if (!((const Header*)data)->occupied()) ++(*this);
    }
}
bool FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator==(const ConstForwardIterator& other) const {
    return data == other.data && size == other.size && allocation_size == other.allocation_size;
}
bool FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator!=(const ConstForwardIterator& other) const {
    return data != other.data || size != other.size || allocation_size != other.allocation_size;
}
const void* FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator*() const {
    auto end_of_header = (const char*)data + sizeof(uint32_t);
    return end_of_header + alignment_offset(end_of_header, allocation_alignment);
}
FixedSizeGenerationalIdAllocator::ConstForwardIterator
FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator++() {
    // Prefix increment.
    TM_ASSERT(size);
    const char* p = (const char*)data + allocation_size;
    --size;
    do {
        auto header = (const Header*)p;
        if (header->occupied()) break;
        p += allocation_size;
        --size;
    } while (size);
    data = p;
    return *this;
}
FixedSizeGenerationalIdAllocator::ConstForwardIterator
FixedSizeGenerationalIdAllocator::ConstForwardIterator::operator++(int) {
    // Postfix increment.
    auto previous = *this;
    this->operator++();
    return previous;
}

uint32_t FixedSizeGenerationalIdAllocator::ConstForwardIterator::id() const { return ((const Header*)data)->bits; }