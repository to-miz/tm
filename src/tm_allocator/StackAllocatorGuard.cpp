tml::StackAllocatorGuard::StackAllocatorGuard() : allocator(nullptr), state{} {}
tml::StackAllocatorGuard::StackAllocatorGuard(StackAllocator* allocator)
    : allocator(allocator), state(allocator->get_state()) {}
tml::StackAllocatorGuard::StackAllocatorGuard(StackAllocatorGuard&& other)
    : allocator(other.allocator), state(other.state) {
    other.dismiss();
}
tml::StackAllocatorGuard& tml::StackAllocatorGuard::operator=(StackAllocatorGuard&& other) {
    if (&other != this) {
        if (allocator) allocator->set_state(state);
        allocator = other.allocator;
        state = other.state;
        other.dismiss();
    }
    return *this;
}
tml::StackAllocatorGuard::~StackAllocatorGuard() {
    if (allocator) {
        allocator->set_state(state);
        allocator = nullptr;
    }
}

void tml::StackAllocatorGuard::dismiss() { allocator = nullptr; }