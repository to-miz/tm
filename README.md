# tm
single-file libraries for C/C++ in public domain

Current libraries

Library | Desciption | Language | Current Version
--- | --- | --- | ---
tm_conversion.h | from/to string conversion library with minimal dependencies | C99/C++ | v0.9a
tm_unicode.h | extracting codepoints from utf8/utf16 strings and conversion between them | C99/C++ | v1.0a
tm_bezier.h | calculate bezier curve points with forward differencing using only floating point addition | C99/C++ | v1.0a
tm_polygon.h | triangulation and clipping of polygons. Clipping supports union, difference and substraction | C99/C++ | v1.0a
tm_utility.h | Utility and convenience functions, used by my private projects | C++11 | v1.0
tm_arrayview.h | Lightweight ArrayView and UninitializedArrayView classes for POD types, allows treating static arrays and memory regions like std containers with insertion/erase. No memory allocation/resizing. | C++11 | v1.0

These libraries follow the same guidelines as [stb](https://github.com/nothings/stb).
They have minimal dependencies and are licensed in public domain.
