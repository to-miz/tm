/*
tm_unicode.h v0.9.2 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak MERGE_YEAR

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_UNICODE_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

PURPOSE
    A full Utf-8 support library for C/C++. It is designed to allow using Utf-8 everywhere.
    There are also optional system call wrappers for windows/linux, so that those can be used
    with Utf-8 strings in a platform independent manner. The wrappers include file IO and
    converting the command line.

SWITCHES
    TMU_NO_UCD:
        If TMU_NO_UCD is defined, no Unicode data tables will be compiled into the binary.
        Unicode data tables consume a lot of memory (29937 bytes or 29.24 kilobytes currently).
        The tables are needed for things like case folding and case insensitive comparisons.
        Not including them will make for a smaller binary size.

        The secondary use of this switch is to enable more/less functionality than is provided
        by default. This requires new tables to be generated.
        For Unicode data table generation see https://github.com/to-miz/tm/tools/unicode_gen.

        Then include the generated header before including this file like this:
            #include <generated_ucd.h>
            #define TMU_NO_UCD
            #include <tm_unicode.h>

        And in a single translation unit:
            #include <generated_ucd.h>
            #include <generated_ucd.c>
            #define TMU_NO_UCD
            #define TM_UNICODE_IMPLEMENTATION
            #include <tm_unicode.h>

    TMU_USE_CRT:
        Allows the implementation to use CRT functions.

    TMU_USE_WINDOWS_H:
        Allows the implementation to use Winapi functions (those defined in windows.h).
        The header file <windows.h> still needs to be included manually like this before
        including this file:
            #include <windows.h>
            #define TMU_USE_WINDOWS_H
            #include <tm_unicode.h>

        Both TMU_USE_CRT and TMU_USE_WINDOWS_H can be defined at the same time.
        This allows CRT file IO to be accessible even with the Winapi backend.

    TMU_NO_SHELLAPI:
        Disables tmu_utf8_winapi_get_command_line on Windows, since that requires Shellapi.h
        and linking against Shell32.lib.

    TMU_NO_FILE_IO
        As the name suggests, if this is defined, no file IO functions are supplied.

    TMU_USE_CONSOLE:
        Enables tmu_console_output for Utf-8 console output. Needs file io.
        On Windows it is recommended to use Winapi (TMU_USE_WINDOWS_H) when using console output.
        See tmu_console_output documentation.

    TMU_DEFINE_MAIN:
        The implementation will define the main function.
        If UNICODE or _UNICODE is defined, the implementation uses wmain to then convert
        argv into Utf-8. Then tmu_main is called with the Utf-8 arguments.
        If UNICODE or _UNICODE is not defined, main just calls into tmu_main as is.
        Note when using Mingw on Windows, you need to pass -municode so that wmain is used.

        If TMU_USE_CONSOLE is also defined, tmu_console_output_init() will be called before
        entering tmu_main.

        tmu_main has to be supplied by the usage code. It has the signature:
            int tmu_main(int argc, const char *const * argv)

NOTES
    Compiling with -std=c99 on gcc/clang with file io:
    You need to additionally pass in -D_XOPEN_SOURCE=500 -D_DEFAULT_SOURCE as options, so that some more advanced
    posix functions are defined in the headers, that are otherwise not defined because of c99.

ISSUES
    - No locale support for case folding (some locales case fold differently,
      like turkic languages with dotted uppercase I).
    - No conditional special casing support, like for instance FINAL SIGMA
      (sigma character at the end of a word has a different lowercase variant).
    - tmu_atomic_write not implemented yet for CRT backend.
    - tmu_utf8_width not implemented properly yet, it calculates the width of all codepoints of a string instead of
      calculating the width of display glyphs.
    - Grapheme break detection not implemented yet.

HISTORY    (DD.MM.YY)
    v0.9.2  08.08.20 Added tmu_printf, tmu_vprintf, tmu_fprintf, tmu_vfprintf.
                     Added tests for console output.
    v0.9.1  06.08.20 Added tmu_module_filename, tmu_module_directory, tmu_open_directory,
                     tmu_close_directory, tmu_read_directory.
    v0.9.0  11.03.20 Updated generated Unicode data to Unicode Version 13.0.0.
    v0.1.9  01.01.20 Fixed compilation error on unix.
    v0.1.8  01.01.20 Added TMU_USE_CONSOLE and TMU_NO_SHELLAPI.
    v0.1.7  01.01.20 Added TMU_DEFINE_MAIN.
    v0.1.6  12.07.19 Fixed error in documentation.
    v0.1.5  30.05.19 Made error codes depend on <errno.h> by default.
    v0.1.4  02.04.19 Fixed gcc/clang compilation errors.
                     Implemented full case toggling.
    v0.1.3  21.03.19 Fixed tmu_get_ucd_width being used instead of tmu_ucd_get_width.
                     Changed generated unicode data to handle invalid codepoints.
    v0.1.2  10.03.19 Fixed unused function warning when compiling with TMU_NO_UCD.
    v0.1.1  25.02.19 Fixed MSVC compilation errors.
    v0.1.0  24.02.19 Initial commit of the complete rewrite.
*/

#include "../common/GENERATED_WARNING.inc"

#ifdef TM_UNICODE_IMPLEMENTATION
    #include "../common/tm_assert.inc"
#endif /* defined(TM_UNICODE_IMPLEMENTATION) */

#ifndef _TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765
#define _TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765

#include "../common/stdint.inc"

#include "../common/tm_size_t.inc"

#include "../common/tm_bool.inc"

#include "../common/tm_string_view.inc"

#include "../common/tm_errc.inc"
#define TMU_NO_SUCH_FILE_OR_DIRECTORY TM_ENOENT

#include "../common/tm_static_assert.inc"

#ifndef TMU_DEF
    #define TMU_DEF extern
#endif

#if defined(TMU_USE_CRT)
    #ifndef TMU_TESTING
        #include <stdio.h>
    #endif
    #if defined(TMU_USE_CONSOLE) && !defined(TMU_USE_WINDOWS_H)
        #include <stdarg.h>
    #endif
#endif

/* Unicode handling. */
#if !defined(TMU_NO_UCD)
#define TMU_UCD_DEF TMU_DEF
#include "generated/unicode_data.h"
#undef TMU_UCD_DEF
#endif

#if defined(TMU_USE_STL) && defined(__cplusplus)
    #include <vector>
#endif /* defined(TMU_USE_STL) && defined(__cplusplus) */

#include "char_types.h"

#include "unicode.h"

#if !defined(TMU_NO_FILE_IO)
#include "file_io.h"
#endif /* !defined(TMU_NO_FILE_IO) */

#endif  // _TM_UNICODE_H_INCLUDED_28D2399D_8C7A_4524_8865_E05090EE0765

#ifdef TM_UNICODE_IMPLEMENTATION

/* Small buffer optimization for path string allocations. */
#ifndef TMU_SBO_SIZE
    #define TMU_SBO_SIZE 260u
#endif

#include "../common/tm_unreferenced_param.inc"

#include "../common/tm_assert_valid_size.inc"

#include "../common/tm_null.inc"

#include "char_types.cpp"
#if !defined(TMU_NO_UCD)
#define TMU_UCD_DEF TMU_DEF
#include "generated/unicode_data.c"
#undef TMU_UCD_DEF
#endif /* !defined(TMU_NO_UCD) */

#include "runtime_functions.h"
#include "unicode.cpp"

#if !defined(TMU_NO_FILE_IO)

#include "file_io_implementation.cpp"

#endif /* !defined(TMU_NO_FILE_IO) */

#endif /* defined(TM_UNICODE_IMPLEMENTATION) */

#include "../common/LICENSE.inc"
