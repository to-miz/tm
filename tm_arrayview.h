/*
tm_arrayview.h v1.2.0 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2016

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

NOTES
    ArrayView, UninitializedArrayView and GridView classes for POD types.
    These classes are designed to allow you to treat static arrays just like std containers,
    especially in the case of UninitializedArrayView, enabling to insert/erase entries.
    No memory managment is done by these classes, they only work with the memory provided to them.
    These are useful as the type of arguments to functions that expect arrays.
    This way you can pass static arrays, std::arrays, std::vectors etc into the same function.

    Another design choice was to let them be POD types themselves, so that having them as data
    members doesn't break PODnes.

    The classes are named views, because they do not own their memory, they only represent "views"
    into already existing arrays.

    ArrayView treats memory as an already initialized array.
    UninitializedArrayView treats memory as an uninitialized array, it allows for operations like
    push_back and erase (no resizing/reallocation of memory takes place).
    GridView treats memory as a two dimensional array, you access elements by row/column indices.

SWITCHES
    TMA_INT64_ACCOSSORS:
        define this if tm_size_t is a 32bit value and you also want 64bit accessors
        (ie operator[]). Also requires a typedef in the form:
            typedef long long int tma_index_t;
    TMA_EMPLACE_BACK_RETURNS_POINTER:
        changed the return type of emplace_back in UninitializedArrayView to return a pointer to the
        newly emplaced back entry. Otherwise the return type is reference or T&, which is how C++1z
        std containers implement emplace_back. So #defining TMA_EMPLACE_BACK_RETURNS_POINTER will
        make UninitializedArrayView's interface incompatible to std containers, so switching from/to
        std::container usage and UninitializedArrayView usage will require code changes.

HISTORY
    v1.2.0  24.06.19 Added try_push_back and try_pop_back to UninitializedArrayView.
    v1.1.8  16.04.19 Added TM_CONSTEXPR.
                     Improved constexpr-ness of ArrayView.
    v1.1.7  13.04.19 Fixed gcc/clang compilation errors.
    v1.1.6  06.10.18 Changed formatting.
                     Changed some macro definitions to commonly used ones.
    v1.1.5  20.06.17 Improved constness of GridView.
    v1.1.4c 25.08.18 Added repository link.
    v1.1.4b 10.02.17 Removed inline since they are unnecessary.
    v1.1.4a 10.01.17 Fixed a warning for signed/unsigned mismatch if tm_size_t is signed.
    v1.1.4  10.01.17 Added a conversion operator overload from ArrayView<T> to.
                     ArrayView<const T>.
    v1.1.3  07.12.16 Added std::initializer_list assign to UninitializedArrayView.
    v1.1.2a 07.10.16 Minor adjustment of size_t usage.
                     Fixed a minor assertion error.
    v1.1.2  07.10.16 Removed get_index and unsigned int arithmetic when tm_size_t is signed.
    v1.1.1  10.09.16 Fixed a couple of typos in macro definitions.
                     Added TMA_INT64_ACCOSSORS.
    v1.1c   10.09.16 Added TMA_EMPLACE_BACK_RETURNS_POINTER.
    v1.1a   11.07.16 Fixed a bug with preventing sign extensions not actually doing anything.
    v1.1    11.07.16 Added GridView.
    v1.0    10.07.16 Initial commit.
*/

#pragma once

#ifndef _TM_ARRAYVIEW_H_INCLUDED_
#define _TM_ARRAYVIEW_H_INCLUDED_

// clang-format off

// define these if you don't use crt
#ifndef TM_ASSERT
    #include <cassert>
    #define TM_ASSERT assert
#endif
#ifndef TM_MEMCPY
    #include <cstring>
    #define TM_MEMCPY memcpy
#endif
#ifndef TM_MEMMOVE
    #include <cstring>
    #define TM_MEMMOVE memmove
#endif

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
    #include <stddef.h> /* include C version so identifiers are in global namespace */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

// Define this to redefine types used by this library.
#ifndef TMA_USE_OWN_TYPES
    typedef struct {
        tm_size_t x;
        tm_size_t y;
    } tma_point;
#endif

// Define this to constexpr if available.
#ifndef TM_CONSTEXPR
    #define TM_CONSTEXPR
#endif

// Define these like this if tm_size_t is a 32bit value and you also want 64bit accessors (ie operator[]).
#if 0
    #define TMA_INT64_ACCOSSORS
    typedef long long int tma_index_t;
#endif

#ifndef TMA_NO_STD_ITERATOR
    #include <iterator>
#endif

// clang-format on

#include <initializer_list>

template <class T>
struct Array {
    typedef tm_size_t size_type;

    // STL container stuff
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef size_type difference_type;

    iterator begin() const { return iterator(ptr); }
    iterator end() const { return iterator(ptr + sz); }
    const_iterator cbegin() const { return const_iterator(ptr); }
    const_iterator cend() const { return const_iterator(ptr + sz); }

#ifndef TMA_NO_STD_ITERATOR
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    reverse_iterator rbegin() const { return reverse_iterator(ptr + sz); }
    reverse_iterator rend() const { return reverse_iterator(ptr); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(ptr + sz); }
    const_reverse_iterator crend() const { return const_reverse_iterator(ptr); }
#endif

    size_type max_size() const { return sz; }
    size_type capacity() const { return sz; }

    pointer data() const { return ptr; }
    size_type size() const { return sz; }
    size_type length() const { return sz; }
    bool empty() const { return sz == 0; }

    explicit operator bool() { return sz != 0; }

    reference operator[](size_type i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < sz);
        return ptr[i];
    }

    const_reference operator[](size_type i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < sz);
        return ptr[i];
    }

    reference at(size_type i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < sz);
        return ptr[i];
    }

    const_reference at(size_type i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < sz);
        return ptr[i];
    }

#ifdef TMA_INT64_ACCOSSORS
    reference operator[](tma_index_t i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < (tma_index_t)sz);
        return ptr[i];
    }

    const_reference operator[](tma_index_t i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < (tma_index_t)sz);
        return ptr[i];
    }

    reference at(tma_index_t i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < (tma_index_t)sz);
        return ptr[i];
    }

    const_reference at(tma_index_t i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < (tma_index_t)sz);
        return ptr[i];
    }
#endif  // TMA_INT64_ACCOSSORS

    reference back() {
        TM_ASSERT(ptr);
        TM_ASSERT(sz);
        return ptr[sz - 1];
    }
    const_reference back() const {
        TM_ASSERT(ptr);
        TM_ASSERT(sz);
        return ptr[sz - 1];
    }
    reference front() {
        TM_ASSERT(ptr);
        TM_ASSERT(sz);
        return ptr[0];
    }
    const_reference front() const {
        TM_ASSERT(ptr);
        TM_ASSERT(sz);
        return ptr[0];
    }

    void assign(const_iterator first, const_iterator last) {
        TM_ASSERT(first <= last);
        TM_ASSERT(static_cast<size_type>(last - first) == sz);
        TM_ASSERT(&*first != begin());
        TM_MEMCPY(ptr, first, sz * sizeof(value_type));
    }
    void assign(const Array other) {
        TM_ASSERT(other.size() == size());
        TM_ASSERT(other.begin() != begin() || (!other.begin() && !begin()));
        TM_MEMCPY(ptr, other.ptr, sz * sizeof(value_type));
    }
    void assign(const std::initializer_list<T>& list) {
        TM_ASSERT(list.size() == sz);
        TM_MEMCPY(ptr, list.begin(), sz * sizeof(value_type));
    }

    operator Array<const T>() const { return {ptr, sz}; }

    T* ptr;
    size_type sz;
};

template <class T>
struct ArrayView : Array<T> {
    TM_CONSTEXPR ArrayView() : Array{nullptr, 0} {}
    ArrayView(const Array<T>& other) : Array<T>{other.data(), other.size()} {}
    ArrayView(const ArrayView&) = default;
    ArrayView& operator=(const ArrayView&) = default;

    TM_CONSTEXPR ArrayView(T* ptr, tm_size_t sz) : Array<T>{ptr, sz} {}
    TM_CONSTEXPR ArrayView(T* first, T* last) : Array{first, static_cast<tm_size_t>(last - first)} {}

    template <tm_size_t N>
    TM_CONSTEXPR ArrayView(T (&array)[N]) : Array<T>{array, N} {}

    template <class Container>
    ArrayView(Container& container) : Array<T>{container.data(), static_cast<tm_size_t>(container.size())} {}

    TM_CONSTEXPR ArrayView(const std::initializer_list<T>& list)
        : Array<T>{list.begin(), static_cast<tm_size_t>(list.size())} {}

    operator ArrayView<const T>() const { return ArrayView<const T>{this->ptr, this->sz}; }
};

template <class T>
ArrayView<T> makeArrayView(T* ptr, tm_size_t sz) {
    return {ptr, sz};
}
template <class T>
ArrayView<T> makeArrayView(T* first, T* last) {
    TM_ASSERT(first <= last);
    return {first, static_cast<tm_size_t>(last - first)};
}
template <class Container>
ArrayView<typename Container::value_type> makeArrayView(Container& container) {
    return {container.data(), static_cast<tm_size_t>(container.size())};
}
template <class T, size_t N>
ArrayView<T> makeArrayView(T (&array)[N]) {
    return {array, static_cast<tm_size_t>(N)};
}
template <class T>
ArrayView<const T> makeArrayView(const std::initializer_list<T>& list) {
    return {list.begin(), static_cast<tm_size_t>(list.size())};
}

// makeRangeView family of functions
// returns ArrayView of the subsequence [start, end) of the original container
template <class Container>
ArrayView<typename Container::value_type> makeRangeView(Container& container, tm_size_t start) {
    TM_ASSERT(start >= 0);
    auto sz = static_cast<tm_size_t>(container.size());
    if (start >= sz) {
        start = sz;
    }
    return {container.data() + start, sz - start};
}
template <class Container>
ArrayView<typename Container::value_type> makeRangeView(Container& container, tm_size_t start, tm_size_t end) {
    TM_ASSERT(start >= 0);
    TM_ASSERT(end >= 0);
    tm_size_t sz = static_cast<tm_size_t>(container.size());
    if (start >= sz) {
        start = sz;
    }
    if (end >= sz) {
        end = sz;
    }
    TM_ASSERT(start <= end);
    return {container.data() + start, end - start};
}

template <class T, size_t N>
ArrayView<T> makeRangeView(T (&array)[N], tm_size_t start) {
    TM_ASSERT(start >= 0);
    const tm_size_t n = static_cast<tm_size_t>(N);
    if (start >= n) {
        start = n;
    }
    return {array + start, n - start};
}
template <class T, size_t N>
ArrayView<T> makeRangeView(T (&array)[N], tm_size_t start, tm_size_t end) {
    TM_ASSERT(start >= 0);
    TM_ASSERT(end >= 0);
    const tm_size_t n = static_cast<tm_size_t>(N);
    if (start >= n) {
        start = n;
    }
    if (end >= n) {
        end = n;
    }
    TM_ASSERT(start <= end);
    return {array + start, end - start};
}

// UninitializedArrayView

template <class T>
struct UninitializedArrayView {
    typedef tm_size_t size_type;

    T* ptr;
    size_type sz;
    size_type cap;

    // STL container stuff
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef tm_size_t difference_type;

    iterator begin() const { return iterator(ptr); }
    iterator end() const { return iterator(ptr + sz); }
    const_iterator cbegin() const { return const_iterator(ptr); }
    const_iterator cend() const { return const_iterator(ptr + sz); }

#ifndef TMA_NO_STD_ITERATOR
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    reverse_iterator rbegin() const { return reverse_iterator(ptr + sz); }
    reverse_iterator rend() const { return reverse_iterator(ptr); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(ptr + sz); }
    const_reverse_iterator crend() const { return const_reverse_iterator(ptr); }
#endif

    size_type max_size() const { return cap; }
    size_type capacity() const { return cap; }

    pointer data() const { return ptr; }
    size_type size() const { return sz; }
    size_type length() const { return sz; }
    bool empty() const { return sz == 0; }
    bool full() const { return sz == cap; }
    size_type remaining() const { return cap - sz; }

    reference operator[](size_type i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < sz);
        return ptr[i];
    }

    const_reference operator[](size_type i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < sz);
        return ptr[i];
    }

    reference at(size_type i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < sz);
        return ptr[i];
    }

    const_reference at(size_type i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < sz);
        return ptr[i];
    }

#ifdef TMA_INT64_ACCOSSORS
    reference operator[](tma_index_t i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < (tma_index_t)sz);
        return ptr[i];
    }

    const_reference operator[](tma_index_t i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < (tma_index_t)sz);
        return ptr[i];
    }

    reference at(tma_index_t i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < (tma_index_t)sz);
        return ptr[i];
    }

    const_reference at(tma_index_t i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < (tma_index_t)sz);
        return ptr[i];
    }
#endif  // TMA_INT64_ACCOSSORS

    reference back() {
        TM_ASSERT(ptr);
        TM_ASSERT(sz);
        return ptr[sz - 1];
    }
    const_reference back() const {
        TM_ASSERT(ptr);
        TM_ASSERT(sz);
        return ptr[sz - 1];
    }
    reference front() {
        TM_ASSERT(ptr);
        TM_ASSERT(sz);
        return ptr[0];
    }
    const_reference front() const {
        TM_ASSERT(ptr);
        TM_ASSERT(sz);
        return ptr[0];
    }

    void push_back(const T& elem) {
        TM_ASSERT(ptr);
        TM_ASSERT(sz + 1 <= cap);
        ptr[sz] = elem;
        ++sz;
    }
    void pop_back() {
        --sz;
        TM_ASSERT(sz >= 0);
    }

    bool try_push_back(const T& elem) {
        if (!remaining()) return false;
        push_back(elem);
        return true;
    }
    bool try_pop_back() {
        if (sz > 0) {
            --sz;
            return true;
        }
        return false;
    }

// define TMA_EMPLACE_BACK_RETURNS_POINTER if you want emplace_back to return a pointer instead
// of reference. The reference version is how std containers in C++1z implement emplace_back.
#ifdef TMA_EMPLACE_BACK_RETURNS_POINTER
    pointer emplace_back() {
        TM_ASSERT(ptr);
        TM_ASSERT(sz + 1 <= cap);
        ++sz;
        return &ptr[sz - 1];
    }
#else
    reference emplace_back() {
        TM_ASSERT(ptr);
        TM_ASSERT(sz + 1 <= cap);
        ++sz;
        return ptr[sz - 1];
    }
#endif

    void clear() { sz = 0; }
    void resize(size_type sz) {
        TM_ASSERT(sz >= 0 && sz <= cap);
        this->sz = sz;
    }
    void grow(size_type by) {
        sz += by;
        TM_ASSERT(sz >= 0 && sz <= cap);
    }

    void assign(UninitializedArrayView other) { assign(other.begin(), other.end()); }
    void assign(const_iterator first, const_iterator last) {
        TM_ASSERT((first < begin() || first >= end()) && (last < begin() || last >= end()));
        sz = static_cast<size_type>(last - first);
        TM_ASSERT(sz <= cap);
        TM_MEMCPY(ptr, first, sz * sizeof(value_type));
    }
    void assign(const_iterator first, size_type length) {
        TM_ASSERT(length <= cap);
        TM_ASSERT((first < begin() || first >= end()) && (first + length < begin() || first + length >= end()));

        sz = length;
        TM_MEMCPY(ptr, first, sz * sizeof(value_type));
    }
    void assign(size_type n, const value_type& val) {
        TM_ASSERT(n >= 0);
        TM_ASSERT(ptr);
        n = (n < cap) ? (n) : (cap);
        sz = n;
        auto count = n;
        for (tm_size_t i = 0; i < count; ++i) {
            ptr[i] = val;
        }
    }
    void assign(const std::initializer_list<T>& list) {
        TM_ASSERT(list.size() <= (size_t)capacity());
        sz = (size_type)list.size();
        TM_MEMCPY(ptr, list.begin(), sz * sizeof(value_type));
    }

    iterator insert(iterator position, size_type n, const value_type& val) {
        TM_ASSERT(ptr);
        TM_ASSERT(position >= begin() && position <= end());

        size_type rem = remaining();
        size_type count = (n < rem) ? n : (rem);
        auto suffix = end() - position;
        if (count > 0) {
            auto tmp = val;  // in case val is inside sequence
            // make room for insertion by moving suffix
            TM_MEMMOVE(position + count, position, suffix * sizeof(value_type));

            sz += static_cast<size_type>(count);
            for (int i = 0; i < count; ++i) {
                position[i] = tmp;
            }
        }
        return position;
    }
    iterator insert(iterator position, const_iterator first, const_iterator last) {
        TM_ASSERT(first <= last);
        TM_ASSERT(ptr || first == last);
        TM_ASSERT(position >= begin() && position <= end());

        auto rem = remaining();
        tm_size_t count = static_cast<tm_size_t>(last - first);
        TM_ASSERT(rem >= count);
        if (count > 0 && count <= rem) {
            // range fits move entries to make room and copy
            TM_MEMMOVE(position + count, position, (end() - position) * sizeof(value_type));
            TM_MEMCPY(position, first, count * sizeof(value_type));
            sz += static_cast<size_type>(count);
        }
        return position;
    }

    iterator append(const_iterator first, const_iterator last) { return insert(end(), first, last); }
    iterator append(size_type n, const value_type& val) { return insert(end(), n, val); }

    iterator erase(iterator position) {
        TM_ASSERT(ptr);
        TM_ASSERT(position >= begin() && position <= end());
        TM_MEMMOVE(position, position + 1, (end() - position - 1) * sizeof(value_type));
        --sz;
        return position;
    }
    iterator erase(iterator first, iterator last) {
        TM_ASSERT(first <= last);
        TM_ASSERT(ptr || first == last);
        if (first == begin() && last == end()) {
            clear();
        } else if (first < last) {
            TM_ASSERT(first >= begin() && last <= end());
            // move suffix to where the erased range used to be
            TM_MEMMOVE(first, last, (end() - last) * sizeof(value_type));
            sz -= static_cast<size_type>(last - first);
        }
        return first;
    }
};

template <class T>
UninitializedArrayView<T> makeUninitializedArrayView(T* ptr, tm_size_t capacity) {
    return {ptr, 0, capacity};
}
template <class T>
UninitializedArrayView<T> makeInitializedArrayView(T* ptr, tm_size_t size, tm_size_t capacity) {
    return {ptr, size, capacity};
}
template <class T>
UninitializedArrayView<T> makeInitializedArrayView(T* ptr, tm_size_t size) {
    return {ptr, size, size};
}
template <class T, size_t N>
UninitializedArrayView<T> makeUninitializedArrayView(T (&array)[N]) {
    return {array, 0, static_cast<tm_size_t>(N)};
}
template <class T, size_t N>
UninitializedArrayView<T> makeInitializedArrayView(T (&array)[N]) {
    return {array, static_cast<tm_size_t>(N), static_cast<tm_size_t>(N)};
}

// GridView
template <class T>
struct GridView {
    T* ptr;
    tm_size_t width;
    tm_size_t height;

    // STL container stuff
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T* iterator;
    typedef const T* const_iterator;
#ifndef TMA_NO_STD_ITERATOR
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif
    typedef tm_size_t difference_type;
    typedef tm_size_t size_type;

    bool isInBounds(tm_size_t x, tm_size_t y) const { return x >= 0 && x < width && y >= 0 && y < height; }
    bool isInBounds(tma_point p) const { return isInBounds(p.x, p.y); }

    tm_size_t size() const { return width * height; }
    tm_size_t length() const { return width * height; }
    tm_size_t index(tm_size_t x, tm_size_t y) const { return x + y * width; }
    tm_size_t index(tma_point p) const { return p.x + p.y * width; }
    tma_point coordinatesFromIndex(tm_size_t i) const {
        TM_ASSERT(i < size());
        return {i % width, i / width};
    }
    tma_point coordinatesFromPtr(const T* p) const {
        TM_ASSERT(p >= begin() && p < end());
        auto index = static_cast<tm_size_t>(p - begin());
        return coordinatesFromIndex(index);
    }
    tm_size_t indexFromPtr(const T* p) const {
        TM_ASSERT(p >= begin() && p < end());
        return static_cast<tm_size_t>(p - begin());
    }

    T* data() const { return ptr; }
    T* begin() const { return ptr; }
    const T* cbegin() const { return ptr; }
    T* end() const { return ptr + width * height; }

    reference at(tm_size_t i) {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < size());
        return ptr[i];
    }
    const_reference at(tm_size_t i) const {
        TM_ASSERT(ptr);
        TM_ASSERT(i >= 0);
        TM_ASSERT(i < size());
        return ptr[i];
    }

    reference at(tm_size_t x, tm_size_t y) {
        TM_ASSERT(ptr);
        TM_ASSERT(isInBounds(x, y));
        auto i = x + y * width;
        TM_ASSERT(i < size());
        return ptr[i];
    }
    const_reference at(tm_size_t x, tm_size_t y) const {
        TM_ASSERT(ptr);
        TM_ASSERT(isInBounds(x, y));
        auto i = x + y * width;
        TM_ASSERT(i < size());
        return ptr[i];
    }

    reference at(tma_point p) { return at(p.x, p.y); }
    const_reference at(tma_point p) const { return at(p.x, p.y); }

    T* queryAt(tm_size_t index) const {
        T* ret = nullptr;
        if (index >= 0 && index < size()) {
            ret = ptr + index;
        }
        return ret;
    }
    T* queryAt(tm_size_t x, tm_size_t y) const {
        T* ret = nullptr;
        if (isInBounds(x, y)) {
            ret = ptr + x + y * width;
        }
        return ret;
    }
    T* queryAt(tma_point p) const { return queryAt(p.x, p.y); }

    reference operator[](tm_size_t i) { return at(i); }
    const_reference operator[](tm_size_t i) const { return at(i); }
};

template <class T>
GridView<T> makeGridView(T* ptr, tm_size_t width, tm_size_t height) {
    return {ptr, width, height};
}

#endif  // _TM_ARRAYVIEW_H_INCLUDED_

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2016 Tolga Mizrak

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------

Public Domain (www.unlicense.org):
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

---------------------------------------------------------------------------
*/
