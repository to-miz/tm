# tm
Single-file libraries for C/C++ in public domain.

Current libraries

Library | Desciption | Language | Current Version
--- | --- | --- | ---
tm_arrayview.h | Lightweight ArrayView and UninitializedArrayView classes for POD types, allows treating static arrays and memory regions like std containers with insertion/erase. No memory allocation/resizing, does not own memory. | C++11 | v1.1.6
tm_bezier.h | Calculate bezier curve points with forward differencing using only floating point addition. | C99/C++ | v1.1
tm_bin_packing.h | C port with minor modifications of the Guillotine and MaxRects algorithms originally written by [Jukka Jylänki](https://github.com/juj/RectangleBinPack) (also public domain). | C99/C++ | v1.0.7
tm_cli.h | Commandline parser that is similar to getopt_long, but doesn't rely on globals and very lightweight. | C99/C++ | v0.2.2
tm_conversion.h | From and to string conversion library with minimal dependencies. | C99/C++ | v0.9.9.6
tm_json.h | C++ and C99 json parsing library that can do both json reading and parsing into a json document for queries. Accepts a wide variety of flags, can be used to parse json in strict mode, or in json5 mode with the right combination of flags. | C99/C++ | v0.2.4
tm_polygon.h | Triangulation and clipping of polygons. Clipping supports union, difference and substraction. | C99/C++ | v1.1
tm_print.h | Lightweight typesafe variadic print and snprint using C++11 and variadic templates. | C++11 | v0.0.12
tm_stringutil.h | String utility functions, from matching, comparison to tokenizing. | C99/C++ | v0.3.3
tm_unicode.h | A Utf-8 everywhere library with Utf-8/Utf-16 conversion, caseless comparison and system call wrappers for cross platform windows/linux Utf-8 support. | C99/C++ | v0.1.3
tm_utility.h | Utility and convenience functions, mainly for my personal use. | C++11 | v1.1.5

These libraries follow the same guidelines as [stb](https://github.com/nothings/stb).
They have minimal dependencies and are dual licensed in both the public domain and MIT.
