/*
tm_resource_ptr.h v0.1.0 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2020

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

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2020 Tolga Mizrak

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