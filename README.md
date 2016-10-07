# tm
single-file libraries for C/C++ in public domain

Current libraries

Library | Desciption | Language | Current Version
--- | --- | --- | ---
tm_conversion.h | from/to string conversion library with minimal dependencies | C99/C++ | v0.9.4b
tm_unicode.h | extracting codepoints from utf8/utf16 strings and conversion between them | C99/C++ | v1.0.1
tm_bezier.h | calculate bezier curve points with forward differencing using only floating point addition | C99/C++ | v1.0a
tm_polygon.h | triangulation and clipping of polygons. Clipping supports union, difference and substraction | C99/C++ | v1.0c
tm_utility.h | Utility and convenience functions, mainly for my personal use | C++11 | v1.1.1
tm_arrayview.h | Lightweight ArrayView and UninitializedArrayView classes for POD types, allows treating static arrays and memory regions like std containers with insertion/erase. No memory allocation/resizing. | C++11 | v1.1.2
tm_bin_packing.h | C port with minor modifications of the Guillotine and MaxRects algorithms originally written by [Jukka Jylänki](https://github.com/juj/RectangleBinPack) (also public domain) | C99/C++ | v1.0a
tm_json.h | C++ and C99 json parsing library that can both json reading and parsing into a json document for queries. Accepts a wide variety of flags, can be used to parse json in strict mode, or in json5 mode with the right combination of flags. | C99/C++ | v0.1.1a
tm_print.h | lightweight typesafe variadic print and snprint using C++11 and variadic templates | C++11 | v0.0.4b

These libraries follow the same guidelines as [stb](https://github.com/nothings/stb).
They have minimal dependencies and are licensed in public domain.
