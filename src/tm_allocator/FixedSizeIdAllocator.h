#include "GenerationalId.h"

/*!
 * @brief A fixed size allocator that returns simple ids instead of pointers.
 *
 * The ids are simple indexes into the underlying memory. As such, care has to be taken to not use ids after they are
 * destroyed. For a fixed size allocator that protects against use-after-free errors, see
 * FixedSizeGenerationalIdAllocator.
 */
struct FixedSizeIdAllocator {
    FixedSizeIdAllocator() = default;
    /*!
     * @brief Creates a FixedSizeIdAllocator.
     * @param initial_element_capacity[IN] Initial capacity in elements.
     * @param element_size_in_bytes[IN] Size of each element in bytes. Must be at least sizeof(int32_t)
     * @param element_alignment[IN] Alignment of each element in bytes. Must be power of two.
     */
    FixedSizeIdAllocator(int32_t initial_element_capacity, int32_t element_size_in_bytes,
                         int32_t element_alignment = TM_DEFAULT_ALIGNMENT);
    ~FixedSizeIdAllocator();
    FixedSizeIdAllocator(FixedSizeIdAllocator&&);
    FixedSizeIdAllocator& operator=(FixedSizeIdAllocator&&);

    /*!
     * @brief Allocates a fixed size memory block and returns its generational id.
     * @return Returns the id of the allocated block. A value of 0 means failure to allocate. The returned uint32_t
     * should be treated as an opaque resource that should be packed into a type safe wrapper, so that ids from
     * different allocators don't get mixed.
     */
    uint32_t create();
    /*!
     * @brief Destroys the element with the given id. The pointer gotten from data_from_id is invalidated for the given
     * id.
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

   private:
    void* data = nullptr;
    int32_t size = 0;
    int32_t capacity = 0;
    int32_t free_index = -1;
    int32_t allocation_size = 0;
    int32_t allocation_alignment = 0;

    uint32_t pop();
    void push(uint32_t id);
};

#include "FixedSizeGenerationalIdAllocator.h"

/*!
 * @brief A fixed size allocator that returns generational ids instead of pointers.
 *
 * The generational ids allow for the reallocation of the underlying memory without invalidating the ids.
 * Best suited for dishing out opaque identifiers for resources that are managed by an encapsulated system.
 * Think of textures, entity ids and such.
 *
 * This is a type-safe version of FixedSizeGenerationalIdAllocator.
 */
template <class T>
struct FixedSizeTypedIdAllocator : private FixedSizeGenerationalIdAllocator {
   private:
    typedef FixedSizeGenerationalIdAllocator Base;

   public:
    FixedSizeTypedIdAllocator() = default;
    explicit FixedSizeTypedIdAllocator(int32_t initial_capacity) : Base(initial_capacity, sizeof(T), alignof(T)) {}
    FixedSizeTypedIdAllocator(FixedSizeTypedIdAllocator&&) = default;
    FixedSizeTypedIdAllocator& operator=(FixedSizeTypedIdAllocator&&) = default;

    using Base::create;
    using Base::destroy;

    /*!
     * @brief Returns the allocated pointer that corresponds to the given id.
     * Note that the returned pointer should not be stored as the pointer gets invalidated whenever create is called.
     * Use the pointer only for immediate lookups and calculations, only the id is meant to be stored.
     * @return Returns the allocated pointer that corresponds to the given id, or nullptr if the id is invalid.
     */
    T* data_from_id(uint32_t id) { return static_cast<T*>(Base::data_from_id(id)); }

    using Base::operator bool;

    struct ForwardIterator : private FixedSizeGenerationalIdAllocator::ForwardIterator {
        friend struct FixedSizeTypedIdAllocator;

       private:
        typedef FixedSizeGenerationalIdAllocator::ForwardIterator Base;

       public:
#ifndef TMAL_NO_STL
        using Base::iterator_category;
#endif
        using Base::difference_type;
        using Base::pointer;
        using Base::reference;
        using Base::value_type;

       private:
        inline ForwardIterator(void* data, int32_t size, int32_t allocation_size, int32_t allocation_alignment)
            : Base(data, size, allocation_size, allocation_alignment) {}

       public:
        using Base::operator==;
        using Base::operator!=;
        using Base::operator++;
        using Base::id;
        T* operator*() const { return static_cast<T*>(Base::operator*()); }
    };
    struct ConstForwardIterator : private FixedSizeGenerationalIdAllocator::ConstForwardIterator {
        friend struct FixedSizeTypedIdAllocator;

       private:
        typedef FixedSizeGenerationalIdAllocator::ConstForwardIterator Base;

       public:
#ifndef TMAL_NO_STL
        using Base::iterator_category;
#endif
        using Base::difference_type;
        using Base::pointer;
        using Base::reference;
        using Base::value_type;

       private:
        inline ConstForwardIterator(void* data, int32_t size, int32_t allocation_size, int32_t allocation_alignment)
            : Base(data, size, allocation_size, allocation_alignment) {}

       public:
        using Base::operator==;
        using Base::operator!=;
        using Base::operator++;
        using Base::id;
        const T* operator*() const { return static_cast<const T*>(Base::operator*()); }
    };

    ForwardIterator begin() { return {this->data, this->size, this->allocation_size, this->allocation_alignment}; }
    ForwardIterator end() {
        return {(char*)this->data + this->size * this->allocation_size, 0,
                this->allocation_size, this->allocation_alignment};
    }
    ConstForwardIterator begin() const {
        return {this->data, this->size, this->allocation_size, this->allocation_alignment};
    }
    ConstForwardIterator end() const {
        return {(char*)this->data + this->size * this->allocation_size, 0,
                this->allocation_size, this->allocation_alignment};
    }
    ConstForwardIterator cbegin() const {
        return {this->data, this->size, this->allocation_size, this->allocation_alignment};
    }
    ConstForwardIterator cend() const {
        return {(char*)this->data + this->size * this->allocation_size,
                0, this->allocation_size, this->allocation_alignment};
    }
};