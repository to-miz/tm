/*
tm_unicode.h v0.1.0 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak MERGE_YEAR

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This is a helper class to manage resources allocated by other tm libraries
    in a RAII manner similar to std::unique_ptr like this:
        auto resource = tml::make_resource(some_allocating_function())

    You should define
        #define TM_USE_RESOURCE_PTR
    before including other tm libraries and include this library last,
    to make other tm libraries define customization points for resource management.

HISTORY    (DD.MM.YY)
    v0.1.0  12.08.20 Initial commit.
*/

#pragma once

#ifndef _TM_RESOURCE_PTR_INCLUDED_5D906556_3396_4BFF_B2BA_FD930F819F29
#define _TM_RESOURCE_PTR_INCLUDED_5D906556_3396_4BFF_B2BA_FD930F819F29

// Make other tm libraries define resource management customization points.
#ifndef TM_USE_RESOURCE_PTR
#define TM_USE_RESOURCE_PTR
#endif

namespace tml {

template <class T>
class resource_ptr {
   public:
    resource_ptr() : data() {}
    explicit resource_ptr(T data) : data(data) {}
    resource_ptr(resource_ptr&& other) : data(other.data) { other.data = T(); }

    resource_ptr& operator=(resource_ptr&& other) {
        this->swap(other);
        return *this;
    }

    ~resource_ptr() { tml::destroy_resource(&data); }

    T release() {
        T result = data;
        data = T();
        return result;
    }

    void reset(T new_data = T()) {
        tml::destroy_resource(&data);
        data = new_data;
    }

    void swap(resource_ptr& other) {
        T temp = other.data;
        other.data = data;
        data = temp;
    }

    T& get() { return data; }
    const T& get() const { return data; }

    explicit operator bool() const { return tml::valid_resource(data); }

    T& operator*() { return data; }
    const T& operator*() const { return data; }
    T* operator->() { return &data; }
    const T* operator->() const { return &data; }

    resource_ptr(const resource_ptr&) = delete;
    resource_ptr& operator=(const resource_ptr&) = delete;

   private:
    T data;
};

template <class T>
resource_ptr<T> make_resource(const T& resource) {
    return resource_ptr<T>(resource);
}

}  // namespace tml

#endif /* !defined(_TM_RESOURCE_PTR_INCLUDED_5D906556_3396_4BFF_B2BA_FD930F819F29) */

#include "../common/LICENSE.inc"