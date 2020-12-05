/*!
 * @brief A fixed size allocator that returns generational ids instead of pointers.
 *
 * The generational ids allow for the reallocation of the underlying memory without invalidating the ids.
 * Best suited for dishing out opaque identifiers for resources that are managed by an encapsulated system.
 * Think of textures, entity ids and such.
 *
 * The implementation uses headers for each field, which makes it possible to catch use-after-free errors and enables
 * iteration.
 */
struct FixedSizeGenerationalIdAllocator {
    FixedSizeGenerationalIdAllocator() = default;
    /*!
     * @brief Creates a FixedSizeGenerationalIdAllocator.
     * @param initial_element_capacity[IN] Initial capacity in elements.
     * @param element_size_in_bytes[IN] Size of each element in bytes.
     * @param element_alignment[IN] Alignment of each element in bytes. Must be power of two.
     */
    FixedSizeGenerationalIdAllocator(int32_t initial_element_capacity, int32_t element_size_in_bytes,
                                     int32_t element_alignment = TM_DEFAULT_ALIGNMENT);
    ~FixedSizeGenerationalIdAllocator();
    FixedSizeGenerationalIdAllocator(FixedSizeGenerationalIdAllocator&&);
    FixedSizeGenerationalIdAllocator& operator=(FixedSizeGenerationalIdAllocator&&);

    /*!
     * @brief Allocates a fixed size memory block and returns its generational id.
     * @return Returns the id of the allocated block. A value of 0 means failure to allocate. The returned uint32_t
     * should be treated as an opaque resource that should be packed into a type safe wrapper, so that ids from
     * different allocators don't get mixed.
     */
    uint32_t create();
    /*!
     * @brief Destroys the element with the given id. The pointer gotten from data_from_id is invalidated for the given
     * id. Iterators are not invalidated on destroy, because destroy doesn't change the size of the container and
     * doesn't move elements around.
     * @param id[IN] The id of the element to be destroyed.
     */
    void destroy(uint32_t id);
    /*!
     * @brief Returns the allocated pointer that corresponds to the given id.
     * Note that the returned pointer should not be stored as the pointer gets invalidated whenever create is called.
     * Use the pointer only for immediate lookups and calculations, only the id is meant to be stored.
     * @return Returns the allocated pointer that corresponds to the given id, or nullptr if the id is invalid.
     */
    void* data_from_id(uint32_t id);

    inline explicit operator bool() const { return data != nullptr && capacity; };

    struct ForwardIterator {
        friend struct FixedSizeGenerationalIdAllocator;

       public:
#ifndef TMAL_NO_STL
        typedef std::forward_iterator_tag iterator_category;
#endif
        typedef ptrdiff_t difference_type;
        typedef void* value_type;
        typedef void** pointer;
        typedef void*& reference;

       private:
        void* data = nullptr;
        int32_t size = 0;
        int32_t allocation_size = 0;
        int32_t allocation_alignment = 0;

        ForwardIterator(void* data, int32_t size, int32_t allocation_size, int32_t allocation_alignment);


       public:
        bool operator==(const ForwardIterator& other) const;
        bool operator!=(const ForwardIterator& other) const;
        void* operator*() const;
        ForwardIterator operator++();     // Prefix
        ForwardIterator operator++(int);  // Postfix

        uint32_t id() const;
    };
    struct ConstForwardIterator {
        friend struct FixedSizeGenerationalIdAllocator;

       public:
#ifndef TMAL_NO_STL
        typedef std::forward_iterator_tag iterator_category;
#endif
        typedef ptrdiff_t difference_type;
        typedef const void* value_type;
        typedef const void** pointer;
        typedef const void*& reference;

       protected:
        const void* data = nullptr;
        int32_t size = 0;
        int32_t allocation_size = 0;
        int32_t allocation_alignment = 0;

        ConstForwardIterator(void* data, int32_t size, int32_t allocation_size, int32_t allocation_alignment);

       public:
        bool operator==(const ConstForwardIterator& other) const;
        bool operator!=(const ConstForwardIterator& other) const;
        const void* operator*() const;
        ConstForwardIterator operator++();     // Prefix
        ConstForwardIterator operator++(int);  // Postfix

        uint32_t id() const;
    };

    inline ForwardIterator begin() { return {data, size, allocation_size, allocation_alignment}; }
    inline ForwardIterator end() { return {(char*)data + size * allocation_size, 0, allocation_size, allocation_alignment}; }
    inline ConstForwardIterator begin() const { return {data, size, allocation_size, allocation_alignment}; }
    inline ConstForwardIterator end() const { return {(char*)data + size * allocation_size, 0, allocation_size, allocation_alignment}; }
    inline ConstForwardIterator cbegin() const { return {data, size, allocation_size, allocation_alignment}; }
    inline ConstForwardIterator cend() const { return {(char*)data + size * allocation_size, 0, allocation_size, allocation_alignment}; }

   private:
    typedef GenerationalId Header;

    void* base_from_id(uint32_t id);
    inline static Header* header_from_base(void* base) { return (Header*)base; }
    void* body_from_base(void* base);

    void* data = nullptr;
    int32_t size = 0;
    int32_t capacity = 0;
    int32_t free_index = -1;
    int32_t allocation_size = 0;
    int32_t allocation_alignment = 0;

    uint32_t pop();
    void push(uint32_t id);
};