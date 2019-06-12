/*
tm_small_vector.h v0.0.1 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2019

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This file is a single header library, just include it in a C++ source file.

DESCRIPTION
    A dynamic container type named small_vector that is very similar to STL <vector> but with a couple of differences:
        1. small_vector utilizes small buffer optimization to not allocate memory for the most common case.
        2. No custom allocator support. There are two allocation schemes, the malloc_allocator and the no_allocator.
           The malloc_allocator makes use of realloc for POD types and realloc_in_place if available.
           The no_allocator basically turns a static array a non resizable vector semantics.
        3. No exception support.
        4. Return values for some functions are different. This is because allocations can fail in this
           implementation instead of raising an exception. For instance emplace_back returns a pointer instead of a
           reference.
    If custom allocation or exception support is needed, STL <vector> is a better fit than this library.

SWITCHES
    TMSV_NO_ITERATOR:
    TMSV_NO_INITIALIZER_LIST:
        Control whether this library can include <iterator> and <initializer_list>. Otherwise no iterator or
        initializer_list support is included.

ISSUES
    - Not yet first release.

HISTORY     (DD.MM.YY)
    v0.0.1   08.06.19 Initial Commit.
*/

#pragma once

#ifndef _TM_SMALL_VECTOR_H_INCLUDED_02E65020_6126_478E_AECB_31129B77DED8_
#define _TM_SMALL_VECTOR_H_INCLUDED_02E65020_6126_478E_AECB_31129B77DED8_

#include "../common/stdint.inc"

#include "../common/tm_size_t.inc"

#include "../common/tm_assert.inc"
#include "../common/tm_assert_valid_size.inc"

#include "../common/tm_malloc.inc"

#include "../common/tm_unreferenced_param.inc"

#include "../common/tm_placement_new.inc"

#include <type_traits>

#if !defined(TM_MEMMOVE) || !defined(TM_MEMCPY) || !defined(TM_MEMSET)
    #include <cstring>
    #ifndef TM_MEMMOVE
        #define TM_MEMMOVE ::std::memmove
    #endif
    #ifndef TM_MEMCPY
        #define TM_MEMCPY ::std::memcpy
    #endif
    #ifndef TM_MEMSET
        #define TM_MEMSET ::std::memset
    #endif
#endif

#ifndef TMSV_NO_ITERATOR
    #include <iterator>
    namespace tml {
        using ::std::iterator_traits;
    }
#else
    namespace tml {
        // If <iterator> is not allowed, we use
        template <class T>
        struct iterator_traits {};
        template <class T>
        struct iterator_traits<T*> {
            struct iterator_category {};
        };
    }
#endif

#ifndef TMSV_NO_INITIALIZER_LIST
#include <initializer_list>
#endif

namespace tml {

struct malloc_allocator_tag {};
struct no_allocator_tag {};

namespace detail {

template <class T>
struct small_vector_guts {
    typedef typename ::std::make_unsigned<tm_size_t>::type usize_type;
    typedef tm_size_t size_type;

    static constexpr const usize_type is_sbo_mask = ((usize_type)1 << ((sizeof(usize_type) * 8) - 1u));
    static constexpr const size_type max_count = (size_type)(~(((usize_type)1 << ((sizeof(usize_type) * 8) - 1u))));

    T* ptr;
    size_type sz;
    usize_type cap_mask;

    void set_sbo_capacity(tm_size_t cap) {
        TM_ASSERT(cap <= max_count);
        cap_mask = (usize_type)cap;
        if (cap > 0) cap_mask |= is_sbo_mask;
    }
    void set_capacity(tm_size_t cap) {
        TM_ASSERT(cap <= max_count);
        cap_mask = (usize_type)cap;
    }
    tm_size_t capacity() const { return (tm_size_t)(cap_mask & (~is_sbo_mask)); }
    bool is_sbo() const { return (cap_mask & is_sbo_mask) != 0; }
};

template <class T>
class small_vector_base : protected small_vector_guts<T> {
   protected:
    typedef small_vector_guts<T> guts_t;
    using guts_t::cap_mask;
    using guts_t::ptr;
    using guts_t::sz;
    using guts_t::usize_type;

   public:
    // STL container type definitions.
    using typename guts_t::size_type;
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef size_type difference_type;

   public:
    // Iterators.

    iterator begin() const { return iterator(ptr); }
    iterator end() const { return iterator(ptr + sz); }
    const_iterator cbegin() const { return const_iterator(ptr); }
    const_iterator cend() const { return const_iterator(ptr + sz); }

#ifndef TMSV_NO_ITERATOR
    typedef ::std::reverse_iterator<iterator> reverse_iterator;
    typedef ::std::reverse_iterator<const_iterator> const_reverse_iterator;
    reverse_iterator rbegin() const { return reverse_iterator(ptr + sz); }
    reverse_iterator rend() const { return reverse_iterator(ptr); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(ptr + sz); }
    const_reverse_iterator crend() const { return const_reverse_iterator(ptr); }
#endif

    size_type max_size() const { return guts_t::max_count; }
    size_type capacity() const { return guts_t::capacity(); }
    size_type size() const { return sz; }

    pointer data() const { return ptr; }
    size_type length() const { return sz; }
    bool empty() const { return sz == 0; }
    size_type remaining() const {
        TM_ASSERT(capacity() >= sz);
        return capacity() - sz;
    }

#define TMSV_ASSERT_VALID_INDEX(index) \
    TM_ASSERT(ptr);                    \
    TM_ASSERT(sz > 0);                 \
    TM_ASSERT_VALID_SIZE(index);       \
    TM_ASSERT((index) < sz)

    reference operator[](size_type i) {
        TMSV_ASSERT_VALID_INDEX(i);
        return ptr[i];
    }

    const_reference operator[](size_type i) const {
        TMSV_ASSERT_VALID_INDEX(i);
        return ptr[i];
    }

    reference at(size_type i) {
        TMSV_ASSERT_VALID_INDEX(i);
        return ptr[i];
    }

    const_reference at(size_type i) const {
        TMSV_ASSERT_VALID_INDEX(i);
        return ptr[i];
    }

    reference back() {
        TMSV_ASSERT_VALID_INDEX(sz - 1);
        return ptr[sz - 1];
    }
    const_reference back() const {
        TMSV_ASSERT_VALID_INDEX(sz - 1);
        return ptr[sz - 1];
    }
    reference front() {
        TMSV_ASSERT_VALID_INDEX(0);
        return ptr[0];
    }
    const_reference front() const {
        TMSV_ASSERT_VALID_INDEX(0);
        return ptr[0];
    }

    void pop_back() {
        TMSV_ASSERT_VALID_INDEX(sz - 1);
        if constexpr (!::std::is_trivially_destructible<T>::value) {
            ptr[sz - 1].~T();
        }
        --sz;
    }

#undef TMSV_ASSERT_VALID_INDEX

   public:
    void clear() {
        TM_ASSERT(ptr || (sz == 0 && capacity() == 0));
        TM_ASSERT_VALID_SIZE(sz);
        if constexpr (!::std::is_trivially_destructible<T>::value) {
            for (size_type i = sz; i > 0; --i) {
                ptr[i - 1].~T();
            }
        }
        sz = 0;
    }

    iterator erase(const_iterator it) {
        TM_ASSERT(owns(it));
        TM_ASSERT(it != end());
        return erase(it, it + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        TM_ASSERT(owns(first));
        TM_ASSERT(owns(last));
        TM_ASSERT(first <= last);
        TM_ASSERT_VALID_SIZE(sz);

        ptrdiff_t prefix_count = first - cbegin();
        TM_ASSERT((size_type)prefix_count <= sz);
        size_t n = (size_t)(last - first);
        TM_ASSERT((size_t)sz >= n);
        if (first != last) {
            const_iterator end_iter = end();
            if constexpr (::std::is_trivially_copyable<T>::value) {
                if (last != end_iter) TM_MEMMOVE(ptr + prefix_count, last, (size_t)(end_iter - last) * sizeof(T));
            } else {
                T* tail = ptr + (last - cbegin());
                T* tail_end = end();
                if (last != end_iter) {
                    // Move assign into initialized range.
                    T* dest = ptr + prefix_count;
                    T* src = tail;
                    T* src_end = tail_end;
                    for (; src != src_end; ++src, (void)++dest) {
                        *dest = ::std::move(*src);
                    }
                }

                // Call destructors of culled range.
                if constexpr (!::std::is_trivially_destructible<T>::value) {
                    for (T* it = tail_end - n; it != tail_end; ++it) {
                        it->~T();
                    }
                }
            }
            sz -= (size_type)n;
        }
        return ptr + prefix_count;
    }

   protected:
    bool owns(const_iterator it) const {
        TM_ASSERT(ptr || (sz == 0 && capacity() == 0));
        return it >= cbegin() && it <= cend();
    }
    bool owns(iterator it) const {
        TM_ASSERT(ptr || (sz == 0 && capacity() == 0));
        return it >= begin() && it <= end();
    }
    template <class It>
    bool owns(It it) const {
        return false;
    }
    bool owns(const T& value) const {
        TM_ASSERT(ptr || (sz == 0 && capacity() == 0));
        return ::std::addressof(value) >= ptr && ::std::addressof(value) < ptr + sz;
    }
};

inline tm_size_t get_next_capacity(tm_size_t old_capacity, tm_size_t at_least) {
    tm_size_t result = old_capacity + (old_capacity / 2) + 1;
    if (result < old_capacity + at_least) result = old_capacity + at_least;
    return result;
}

template <class T>
struct static_assert_helper {
    static constexpr const bool value = false;
};

template <class T, typename AllocatorTag = malloc_allocator_tag>
struct small_vector_alloc {
    static_assert(static_assert_helper<T>::value, "Custom allocator support not implemented.");
};

template <class T>
struct small_vector_alloc<T, malloc_allocator_tag> : public small_vector_base<T> {
   protected:
    small_vector_guts<T> create(tm_size_t capacity) {
        TM_ASSERT_VALID_SIZE(capacity);
        small_vector_guts<T> result = {};
        void* new_data = TM_MALLOC(sizeof(T) * capacity, alignof(T));
        if (new_data) {
            result.ptr = (T*)new_data;
            result.set_capacity(capacity);
        }
        return result;
    }
    bool grow_by(small_vector_guts<T>* guts, tm_size_t amount) {
        TM_ASSERT_VALID_SIZE(amount);
        TM_ASSERT(small_vector_guts<T>::max_count - guts->sz >= amount);  // Check overflow.
        if (!guts->ptr) {
            TM_ASSERT(guts->sz == 0);
            TM_ASSERT(guts->capacity() == 0);
            *guts = create(amount);
            return guts->ptr != nullptr;
        }

        if (guts->sz + amount <= guts->capacity()) return true;

        tm_size_t new_capacity = get_next_capacity(guts->capacity(), amount);
#ifdef TM_REALLOC_IN_PLACE
        if (!guts->is_sbo()) {
            void* new_data = TM_REALLOC_IN_PLACE(guts->ptr, sizeof(T) * guts->capacity(), alignof(T),
                                                 sizeof(T) * new_capacity, alignof(T));
            if (new_data) {
                guts->ptr = (T*)new_data;
                guts->set_capacity(new_capacity);
                return true;
            }
        }
#endif /* defined(TM_REALLOC_IN_PLACE) */

#ifdef TM_REALLOC
        if constexpr (::std::is_trivially_copyable<T>::value) {
            if (!guts->is_sbo()) {
                void* new_data = TM_REALLOC(guts->ptr, sizeof(T) * guts->capacity(), alignof(T),
                                            sizeof(T) * new_capacity, alignof(T));
                if (new_data) {
                    guts->ptr = (T*)new_data;
                    guts->set_capacity(new_capacity);
                    return true;
                }
            }
        }
#endif /* defined(TM_REALLOC) */

        void* new_ptr = TM_MALLOC(sizeof(T) * new_capacity, alignof(T));
        if (!new_ptr) return false;
        T* new_data = (T*)new_ptr;

        if constexpr (::std::is_trivially_move_constructible<T>::value) {
            TM_MEMCPY(new_data, guts->ptr, guts->sz * sizeof(T));
            if constexpr (!::std::is_trivially_destructible<T>::value) {
                T* ptr = guts->ptr;
                for (tm_size_t i = 0, sz = guts->sz; i < sz; ++i) {
                    ptr[i].~T();
                }
            }
        } else {
            T* ptr = guts->ptr;
            for (tm_size_t i = 0, sz = guts->sz; i < sz; ++i) {
                TM_PLACEMENT_NEW(&new_data[i]) T(::std::move(ptr[i]));
                if constexpr (!::std::is_trivially_destructible<T>::value) {
                    ptr[i].~T();
                }
            }
        }
        if (!guts->is_sbo()) {
            TM_FREE(guts->ptr, sizeof(T) * guts->capacity(), alignof(T));
        }

        guts->ptr = new_data;
        guts->set_capacity(new_capacity);
        return true;
    }
    void destroy(small_vector_guts<T>* guts) {
        if constexpr (!::std::is_trivially_destructible<T>::value) {
            T* ptr = guts->ptr;
            for (tm_size_t i = 0, count = guts->sz; i < count; ++i) {
                ptr[i].~T();
            }
        }
        if (!guts->is_sbo()) {
            TM_FREE(guts->ptr, sizeof(T) * guts->capacity(), alignof(T));
            guts->ptr = nullptr;
            guts->set_capacity(0);
        }
        guts->sz = 0;
    }
    static void shrink_to_fit(small_vector_guts<T>* guts) {
        TM_ASSERT_VALID_SIZE(guts->sz);
        if (guts->sz <= 0) {
            destroy(guts);
        } else {
            tm_size_t size = guts->sz;
#ifdef TM_REALLOC_IN_PLACE
            void* new_data =
                TM_REALLOC_IN_PLACE(guts->ptr, sizeof(T) * guts->capacity(), alignof(T), sizeof(T) * size, alignof(T));
            if (new_data) {
                guts->ptr = (T*)new_data;
                guts->set_capacity(size);
                return;
            }
#endif /* defined(TM_REALLOC_IN_PLACE) */

#ifdef TM_REALLOC
            if constexpr (::std::is_trivially_copyable<T>::value) {
                void* new_data =
                    TM_REALLOC(guts->ptr, sizeof(T) * guts->capacity(), alignof(T), sizeof(T) * size, alignof(T));
                if (new_data) {
                    guts->ptr = (T*)new_data;
                    guts->set_capacity(size);
                    return;
                }
            }
#endif /* defined(TM_REALLOC) */

            small_vector_guts<T> new_guts = create(size);
            if (!new_guts.ptr) return;
            if constexpr (::std::is_trivially_move_constructible<T>::value) {
                TM_MEMCPY(new_guts.ptr, guts->ptr, size * sizeof(T));
                if constexpr (!::std::is_trivially_destructible<T>::value) {
                    T* ptr = guts->ptr;
                    for (tm_size_t i = 0; i < size; ++i) {
                        ptr[i].~T();
                    }
                }
            } else {
                T* ptr = guts->ptr;
                for (tm_size_t i = 0; i < size; ++i) {
                    TM_PLACEMENT_NEW(&new_guts.ptr[i]) T(std::move(ptr[i]));
                    if constexpr (!::std::is_trivially_destructible<T>::value) {
                        ptr[i].~T();
                    }
                }
            }

            destroy(guts);
            *guts = new_guts;
        }
    }
};

template <class T>
struct small_vector_alloc<T, no_allocator_tag> : public small_vector_base<T> {
   protected:
    typedef small_vector_guts<T> guts_t;

    // static constexpr const bool has_state = true;
    guts_t create(tm_size_t capacity) { return {}; }
    bool grow_by(guts_t* guts, tm_size_t amount) const {
        TM_ASSERT_VALID_SIZE(amount);
        TM_ASSERT(guts->max_size() - guts->sz >= guts->amount);  // Check overflow.
        return guts->sz + guts->amount <= guts->capacity();
    }
    void destroy(guts_t* guts) {
        if constexpr (!::std::is_trivially_destructible<T>::value) {
            T* ptr = guts->ptr;
            for (tm_size_t i = 0, count = guts->sz; i < count; ++i) {
                ptr[i].~T();
            }
        }
        guts->sz = 0;
    }
    void shrink_to_fit(small_vector_guts<T>*) {}
};

template <class T, class AllocatorTag = malloc_allocator_tag>
class small_vector_impl : public small_vector_alloc<T, AllocatorTag> {
   protected:
    typedef small_vector_alloc<T, AllocatorTag> base;
    typedef small_vector_guts<T> guts_t;

   public:
    using base::begin;
    using base::capacity;
    using base::cbegin;
    using base::cend;
    using base::clear;
    using base::end;
    using base::max_count;
    using base::ptr;
    using base::size;
    using base::sz;
    using typename base::const_iterator;
    using typename base::const_pointer;
    using typename base::const_reference;
    using typename base::difference_type;
    using typename base::iterator;
    using typename base::pointer;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

   protected:
    using base::base;
    using base::cap_mask;
    using base::is_sbo;
    using base::owns;
    using typename base::usize_type;

   protected:
    void make_guts_from_sbo_uninitialized(T* sbo, tm_size_t cap, tm_size_t required) {
        typedef small_vector_guts<T> guts_t;
        TM_ASSERT(sbo || cap == 0);
        TM_ASSERT_VALID_SIZE(cap);
        TM_ASSERT(cap <= guts_t::max_count);
        if (required <= cap) {
            this->ptr = sbo;
            this->sz = 0;
            this->set_sbo_capacity(cap);
        } else {
            static_cast<guts_t&>(*this) = this->create(required);
        }
    }
    void make_guts_from_sbo_default_constructed(T* sbo, tm_size_t count, tm_size_t cap, tm_size_t required) {
        make_guts_from_sbo_uninitialized(sbo, cap, required);
        if (this->ptr) {
            if constexpr (std::is_trivially_constructible<T>::value) {
                TM_MEMSET(this->ptr, 0, sizeof(T) * this->count);
            } else {
                for (T *first = this->ptr, last = this->ptr + count; first != last; ++first) {
                    TM_PLACEMENT_NEW(first) T();
                }
            }
            this->sz = count;
        }
    }

   public:
    small_vector_impl() = default;
    // Named constructors.
    ~small_vector_impl() { base::destroy(this); }

    iterator insert(const_iterator pos, size_type n, const T& value) {
        if (!owns(value)) {
            return insert_n_impl(pos, n, value);
        } else {
            T temp{value};
            return insert_n_impl(pos, n, temp);
        }
    }

    iterator insert(const_iterator pos, const T& value) {
        if (!owns(value)) {
            return insert_value_impl(pos, value);
        } else {
            T temp{value};
            return insert_value_impl(pos, temp);
        }
    }
    iterator insert(const_iterator pos, T&& value) {
        if (!owns(value)) {
            return insert_value_impl(pos, ::std::move(value));
        } else {
            T temp{std::move(value)};
            return insert_value_impl(pos, ::std::move(temp));
        }
    }

    void push_back(const T& value) {
        if (!owns(value)) {
            insert_value_impl(end(), value);
        } else {
            T temp{value};
            insert_value_impl(end(), temp);
        }
    }
    void push_back(T&& value) {
        if (!owns(value)) {
            insert_value_impl(end(), ::std::move(value));
        } else {
            T temp{std::move(value)};
            insert_value_impl(end(), ::std::move(temp));
        }
    }

    bool resize(size_type count) { return resize(count, T{}); }
    bool resize(size_type count, const value_type& value) {
        TM_ASSERT_VALID_SIZE(count);
        size_type old_size = sz;
        if (sz > count) {
            this->erase(ptr + count, ptr + sz);
        } else if (count > sz) {
            this->insert(end(), count - sz, value);
        }
        return sz != old_size;
    }
    bool reserve(size_type count) {
        if (capacity() >= count) return true;
        return this->grow_by(this, count - capacity());
    }

    void shrink_to_fit() {
        TM_ASSERT(ptr || capacity() == 0);
        TM_ASSERT_VALID_SIZE(sz);
        this->shrink_to_fit(this);
    }

    template <class InputIt>
    auto insert(const_iterator pos, InputIt first, InputIt last)
        -> decltype((void)(typename ::tml::iterator_traits<InputIt>::iterator_category{}), iterator{}) {
        return insert_it_impl(pos, first, last);
    }
#ifndef TMSV_NO_INITIALIZER_LIST
    iterator insert(const_iterator pos, ::std::initializer_list<T> ilist) {
        return insert_it_impl(pos, ilist.begin(), ilist.end());
    }
#endif /* !defined(TMSV_NO_INITIALIZER_LIST) */

    bool assign(size_type n, const T& value) {
        if (!owns(value)) {
            return assign_n_impl(n, value);
        } else {
            T temp{value};
            return assign_n_impl(n, temp);
        }
    }
    template <class InputIt>
    auto assign(InputIt first, InputIt last)
        -> decltype((void)(typename ::tml::iterator_traits<InputIt>::iterator_category{}), bool()) {
        return assign_it_impl(first, last);
    }
#ifndef TMSV_NO_INITIALIZER_LIST
    bool assign(::std::initializer_list<T> ilist) { return assign_it_impl(ilist.begin(), ilist.end()); }
#endif /* !defined(TMSV_NO_INITIALIZER_LIST) */

    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        TM_ASSERT(owns(pos));
        ptrdiff_t prefix_count = pos - cbegin();
        TM_ASSERT((size_type)prefix_count <= sz);
        room_result room = {};
        if (make_room(pos, 1, &room)) {
            if constexpr (::std::is_trivially_copyable<T>::value) {
                TM_ASSERT(room.first_initialized != room.first_uninitialized);
                *room.first_initialized = {std::forward<Args>(args)...};
            } else {
                if (room.first_initialized != room.first_uninitialized) {
                    TM_ASSERT(room.first_initialized != room.first_uninitialized);
                    *room.first_initialized = {std::forward<Args>(args)...};
                } else {
                    TM_ASSERT(room.first_initialized == room.first_uninitialized);
                    TM_PLACEMENT_NEW(room.first_uninitialized) T(std::forward<Args>(args)...);
                }
            }
        }
        return ptr + prefix_count;
    }

    template <class... Args>
    T* emplace_back(Args&&... args) {
        room_result room = {};
        if (make_room(end(), 1, &room)) {
            if constexpr (::std::is_trivially_copyable<T>::value) {
                TM_ASSERT(room.first_initialized != room.first_uninitialized);
                *room.first_initialized = {std::forward<Args>(args)...};
                return room.first_initialized;
            } else {
                TM_ASSERT(room.first_initialized == room.first_uninitialized);
                return TM_PLACEMENT_NEW(room.first_uninitialized) T(std::forward<Args>(args)...);
            }
        }
        return nullptr;
    }

    bool swap(small_vector_impl& other) {
        if (this == &other) return true;

        using ::std::swap;
        if (other.is_sbo() || is_sbo()) {
            if (!reserve(other.size()) || !other.reserve(size())) return false;

            tm_size_t min_sz = (other.sz < sz) ? other.sz : sz;
            for (tm_size_t i = 0; i < min_sz; ++i) {
                swap(ptr[i], other.ptr[i]);
            }
            T* dest = nullptr;
            T* src = nullptr;
            tm_size_t count = 0;
            if (other.sz < sz) {
                dest = other.ptr + min_sz;
                src = ptr + min_sz;
                count = sz - other.sz;
            } else if (other.sz > sz) {
                dest = ptr + min_sz;
                src = other.ptr + min_sz;
                count = other.sz - sz;
            }
            if constexpr (::std::is_trivially_copyable<T>::value) {
                TM_MEMCPY(dest, src, count * sizeof(T));
            } else {
                for (tm_size_t i = 0; i < count; ++i, (void)++dest, (void)++src) {
                    TM_PLACEMENT_NEW(dest) T(::std::move(*src));
                    if constexpr (!std::is_trivially_destructible<T>::value) {
                        src->~T();
                    }
                }
            }
        } else {
            swap(ptr, other.ptr);
        }
        swap(sz, other.sz);
        tm_size_t self_capacity = capacity();
        tm_size_t other_capacity = other.capacity();
        if (is_sbo()) {
            this->set_sbo_capacity(other_capacity);
        } else {
            this->set_capacity(other_capacity);
        }
        if (other.is_sbo()) {
            other.set_sbo_capacity(self_capacity);
        } else {
            other.set_capacity(self_capacity);
        }
        return true;
    }

   protected:
    struct room_result {
        iterator first_initialized;
        iterator first_uninitialized;
    };
    bool make_room(const_iterator pos, size_type n, room_result* out) {
        *out = {};
        ptrdiff_t prefix_count = pos - cbegin();
        TM_ASSERT((size_type)prefix_count <= sz);
        if (!this->grow_by(this, n)) return false;

        T* dest = ptr + prefix_count;
        size_t suffix_count = (size_t)sz - (size_t)prefix_count;
        if constexpr (::std::is_trivially_copyable<T>::value) {
            if (suffix_count) TM_MEMMOVE(dest + n, dest, suffix_count * sizeof(T));
            *out = {dest, dest + n};
        } else {
            if (suffix_count) {
                // Move construct into uninitialized range.
                size_t uninitialized_move_count = ((size_t)n < suffix_count) ? (size_t)n : suffix_count;
                T* tail = ptr + sz;
                T* current = ptr + sz + (n - 1);
                for (size_t i = 0; i < uninitialized_move_count; ++i) {
                    --tail;
                    TM_PLACEMENT_NEW(current) T(std::move(*tail));
                    --current;
                }

                // Move assign into initialized overlapping range.
                size_t initialized_move_count = ((size_t)n < suffix_count) ? (suffix_count - (size_t)n) : 0;
                TM_ASSERT(current < ptr + sz || !initialized_move_count);
                for (size_t i = 0; i < initialized_move_count; ++i) {
                    --tail;
                    *current = ::std::move(*tail);
                    --current;
                }

                size_t initialized_copy_count = ((size_t)n < suffix_count) ? (size_t)n : suffix_count;
                out->first_initialized = dest;
                out->first_uninitialized = dest + initialized_copy_count;
            } else {
                out->first_initialized = dest;
                out->first_uninitialized = dest;
            }
        }
        sz += n;
        return true;
    }

    iterator insert_value_impl(const_iterator pos, const T& value) {
        TM_ASSERT(owns(pos));
        TM_ASSERT(!owns(value));

        ptrdiff_t prefix_count = pos - cbegin();
        TM_ASSERT((size_type)prefix_count <= sz);
        room_result room = {};
        if (make_room(pos, 1, &room)) {
            if constexpr (::std::is_trivially_copyable<T>::value) {
                *room.first_initialized = value;
            } else {
                if (room.first_initialized != room.first_uninitialized) {
                    *room.first_initialized = value;
                } else {
                    TM_PLACEMENT_NEW(room.first_uninitialized) T(value);
                }
            }
        }
        return ptr + prefix_count;
    }
    iterator insert_value_impl(const_iterator pos, T&& value) {
        TM_ASSERT(owns(pos));
        TM_ASSERT(!owns(value));

        ptrdiff_t prefix_count = pos - cbegin();
        TM_ASSERT((size_type)prefix_count <= sz);
        room_result room = {};
        if (make_room(pos, 1, &room)) {
            if constexpr (::std::is_trivially_copyable<T>::value) {
                *room.first_initialized = ::std::move(value);
            } else {
                if (room.first_initialized != room.first_uninitialized) {
                    *room.first_initialized = ::std::move(value);
                } else {
                    TM_PLACEMENT_NEW(room.first_uninitialized) T(std::move(value));
                }
            }
        }
        return ptr + prefix_count;
    }

    iterator insert_n_impl(const_iterator pos, size_type n, const T& value) {
        TM_ASSERT_VALID_SIZE(n);
        TM_ASSERT(owns(pos));
        TM_ASSERT(!owns(value));

        ptrdiff_t prefix_count = pos - cbegin();
        TM_ASSERT((size_type)prefix_count <= sz);
        room_result room = {};
        if (make_room(pos, n, &room)) {
            if constexpr (::std::is_trivially_copyable<T>::value) {
                for (T* dest = room.first_initialized; dest != room.first_uninitialized; ++dest) {
                    *dest = value;
                }
            } else {
                // Copy assign into initialized range.
                for (T* dest = room.first_initialized; dest != room.first_uninitialized; ++dest) {
                    *dest = value;
                }

                // Copy construct into uninitialized range.
                for (T *dest = room.first_uninitialized, *last = ptr + prefix_count + n; dest != last; ++dest) {
                    TM_PLACEMENT_NEW(dest) T(value);
                }
            }
        }
        return ptr + prefix_count;
    }

    template <class InputIt>
    iterator insert_it_impl(const_iterator pos, InputIt first, InputIt last) {
        TM_ASSERT(!owns(first));
        TM_ASSERT(!owns(last));

        size_type n = (size_type)std::distance(first, last);
        ptrdiff_t prefix_count = pos - cbegin();
        TM_ASSERT((size_type)prefix_count <= sz);
        if (n <= 0) return ptr + prefix_count;

        room_result room = {};
        if (make_room(pos, n, &room)) {
            if constexpr (::std::is_trivially_copyable<T>::value) {
                // Raw loop instead of memcpy/std::copy.
                // We can't use memcpy directly since iterators might not be contiguous.
                // We don't use std::copy so we don't instantiate templates with possible tag dispatch unnecessarily.
                // This should be optimized into a memcpy for contiguous memory anyway.
                for (T* dest = room.first_initialized; dest != room.first_uninitialized; ++dest, (void)++first) {
                    *dest = *first;
                }
            } else {
                // Copy assign into initialized range.
                for (T* dest = room.first_initialized; dest != room.first_uninitialized; ++dest, (void)++first) {
                    *dest = *first;
                }

                // Copy construct into uninitialized range.
                for (T *dest = room.first_uninitialized, *dest_end = ptr + prefix_count + n; dest != dest_end;
                     ++dest, (void)++first) {
                    TM_PLACEMENT_NEW(dest) T(*first);
                }
            }
        }
        return ptr + prefix_count;
    }

    bool assign_n_impl(size_type n, const T& value) {
        TM_ASSERT_VALID_SIZE(n);
        TM_ASSERT_VALID_SIZE(sz);
        TM_ASSERT(!owns(value));

        if (n > capacity()) {
            this->destroy(this);

            static_cast<small_vector_guts<T>&>(*this) = this->create(n);
            if (!ptr) return false;
        }

        TM_ASSERT(ptr);
        if constexpr (::std::is_trivially_copyable<T>::value) {
            for (size_type i = 0; i < n; ++i) {
                ptr[i] = value;
            }
        } else {
            size_type initialized_range = (sz < n) ? sz : n;
            size_type uninitialized_range = n - initialized_range;
            for (size_type i = 0; i < initialized_range; ++i) {
                ptr[i] = value;
            }

            for (size_type i = 0; i < uninitialized_range; ++i) {
                TM_PLACEMENT_NEW(&ptr[initialized_range + i]) T(value);
            }

            if constexpr (!std::is_trivially_destructible<T>::value) {
                if (sz > n) {
                    for (size_type i = n, last_index = sz; i < last_index; ++i) {
                        ptr[i].~T();
                    }
                }
            }
        }
        sz = n;
        return true;
    }

    template <class InputIt>
    bool assign_it_impl(InputIt first, InputIt last) {
        size_type n = (size_type)::std::distance(first, last);
        TM_ASSERT_VALID_SIZE(n);
        TM_ASSERT_VALID_SIZE(sz);

        if (n <= 0) {
            clear();
            return true;
        }

        guts_t* guts = this;
        guts_t new_guts = {};
        if (n > capacity()) {
            if (!owns(first)) {
                // We can safely free memory before allocation.
                // We don't use realloc, since we don't need to keep old memory around.
                this->destroy(this);
            }
            new_guts = this->create(n);
            guts = &new_guts;
            if (!new_guts.ptr) return false;
        }

        TM_ASSERT(guts->ptr);
        if constexpr (::std::is_trivially_copyable<T>::value) {
            // Raw loop instead of memcpy/std::copy.
            // We can't use memcpy directly since iterators might not be contiguous.
            // We don't use std::copy so we don't instantiate templates with possible tag dispatch unnecessarily.
            // This should be optimized into a memcpy for contiguous memory anyway.
            for (size_type i = 0; i < n; ++i, (void)++first) {
                guts->ptr[i] = *first;
            }
        } else {
            size_type initialized_range = (guts->sz < n) ? guts->sz : n;
            size_type uninitialized_range = n - initialized_range;
            for (size_type i = 0; i < initialized_range; ++i, (void)++first) {
                guts->ptr[i] = *first;
            }

            for (size_type i = 0; i < uninitialized_range; ++i, (void)++first) {
                TM_PLACEMENT_NEW(&guts->ptr[initialized_range + i]) T(*first);
            }

            if constexpr (!std::is_trivially_destructible<T>::value) {
                if (guts->sz > n) {
                    for (size_type i = n, last_index = guts->sz; i < last_index; ++i) {
                        guts->ptr[i].~T();
                    }
                }
            }
        }
        guts->sz = n;

        if (guts == &new_guts) {
            this->destroy(this);

            static_cast<guts_t&>(*this) = new_guts;
        }
        return true;
    }
};

template <class T, tm_size_t N>
class small_vector_sbo {
   public:
    alignas(alignof(T)) char sbo[N * sizeof(T)];

    T* get_sbo() { return (T*)sbo; }
};

template <class T>
class small_vector_sbo<T, 0> {
   public:
    T* get_sbo() { return nullptr; }
};

}  // namespace detail

template <class T, class AllocatorTag = malloc_allocator_tag>
class small_vector_base : public detail::small_vector_impl<T, AllocatorTag> {};

template <class T, tm_size_t N, class AllocatorTag = malloc_allocator_tag>
class small_vector : public small_vector_base<T, AllocatorTag>, private detail::small_vector_sbo<T, N> {
#ifdef TM_SIZE_T_IS_SIGNED
    static_assert(N >= 0, "N must be positive.");
#endif

   private:
    typedef detail::small_vector_impl<T> base;
    typedef detail::small_vector_alloc<T, AllocatorTag> alloc_t;

   public:
    small_vector() { this->make_guts_from_sbo_uninitialized(this->get_sbo(), N, 0); }

    template <tm_size_t OTHER_N>
    bool swap(small_vector<T, OTHER_N>& other) {
        return this->base::swap(other);
    }

    void shrink_to_fit() {
        detail::small_vector_impl<T, AllocatorTag>::shrink_to_fit();
        // Shrink to fit might release all memory.
        if (!this->ptr || this->capacity() == 0) {
            // In that case restore sbo.
            this->make_guts_from_sbo_uninitialized(this->get_sbo(), N, 0);
        }
    }
};

template <class T, tm_size_t A_N, tm_size_t B_N>
bool swap(small_vector<T, A_N>& a, small_vector<T, B_N>& b) {
    return a.swap(b);
}

} /* namespace tml */

#endif  // _TM_SMALL_VECTOR_H_INCLUDED_02E65020_6126_478E_AECB_31129B77DED8_

#define MERGE_YEAR 2019
#include "../common/LICENSE.inc"