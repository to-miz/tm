/*
tm_conversion.h v0.9.9.3 - public domain - https://github.com/to-miz/tm
author: Tolga Mizrak 2016

no warranty; use at your own risk

LICENSE
    see license notes at end of file

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_CONVERSION_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

    See SWITCHES for more options.

PURPOSE
    The conversion functions in this library are designed with flexibility in mind.
    They can be used to implement your own conversion functions (ie C library functions, printf...).
    Note that the float/double conversion functions are not suitable for release/shipping code,
    since they lose precision for big numbers (above/below +-1e19, see ISSUES).

    There are some switches that enable overloads and convenience functions, those are more
    suited to be used in usage code. See SWITCHES.

WHY
    The reason this library was written is because the standard libraries for both C and C++ are
    lacking in regards to flexibility in their string conversion functionality.
    Without going into specifics of why the standard libraries are lacking, here are the benefits
    of using this library:
        - Support for non nullterminated strings
          This not only allows you to write a string library that doesn't need to support
          nulltermination, but also it allows you to convert from string pieces without copying them
          into temporary nullterminated buffers
        - A unified way of converting/printing values to strings.
          In C you would be using snprintf (which is actually fine as an api, escpecially with
          modern compilers that can do format checking at compile time) but it is one abstraction
          layer too high if all you want is to get the string representation of a single value.
          There is no standard way to do that (except in C++17 with from/to_chars), but a lot of non
          standard functions with different api's like itoa.
        - Have default values for conversions in case a conversion from string to value wasn't
          successful. This is possible because the scan functions in this library only write the
          result into the out parameter on success.
          In C this is not possible with the strto_ family of functions without inspecting errno or
          inspecting the string before passing it into the strto_ functions.
          See examples for how to use default values with this library.
        - Proper error codes without global errno or exceptions
        - C++17 has from/to_chars functions that serve almost the same purpose as this library.
          For those the roundtrip guarantee only holds if the value is printed/scanned using the same
          implementation. You can see this library as a pre C++17 from/to_chars implementation, but
          also get the roundtrip guarantee across platforms.

SWITCHES
    There are a couple of #define switches that enable some additional definitions to be included
    or do something extra.
    Those are:
        TMC_CPP_OVERLOADS:
            Define this if you want cpp overloads for the conversion functions
            that don't have the _n suffix
        TMC_CPP_NO_DEFAULT_FUNCTION_ARGUMENTS:
            Define this if you want to allow function overloads with defaulted function arguments
        TM_STRING_VIEW:
            Define this if you have some sort of string_view class and want overloads of the
            conversion functions for it.
            Needs TMC_CPP_OVERLOADS, TM_STRING_VIEW_DATA( x ) and TM_STRING_VIEW_SIZE( x ) to be
            defined to work.
            Define TMC_STRING_VIEW_NO_CONSTRUCTOR if your stringview can't be constructed from
            a nullterminated string.
            Example:
                // assume we have a string_view class/struct like this: (simplified)
                struct string_view {
                    const char* ptr;
                    size_t sz;
                    size_t data() { return ptr; }
                    size_t size() { return sz; }
                };
                // then define these so that this library can work with your string_view class
                #define TMC_CPP_OVERLOADS
                #define TM_STRING_VIEW string_view
                #define TM_STRING_VIEW_DATA( str ) ( str ).data()
                #define TM_STRING_VIEW_SIZE( str ) ( str ).size()
        TMC_CLAMP_ON_RANGE_ERROR
            If defined to 1 output values of scan will be clamped to nearest max/min value when a
            range error is occured. Otherwise output values are untouched.
        TMC_CHECKED_WIDTH
        	If defined, functions that end with _w will validate width while printing.
        	This is only needed, if users call directly into these functions.

TODO
    - refactor to get rid of most code duplication
    - implement PF_FIXED, PF_HEX, PF_GENERAL, PF_SCIENTIFIC, PF_LOWERCASE

ISSUES
    - PF_FIXED, PF_HEX, PF_GENERAL, PF_SCIENTIFIC, PF_LOWERCASE does nothing at the moment
    - print_double, print_float don't produce correct decimal strings for values above/below +-1e19.
      This is because we use floating arithmetic and we lose too much precision when dealing with
      very big numbers. This could be fixed by using arbitrary precision math, but that pretty much
      is beyond the scope of this library. Do not use these functions for big numbers if accurate readings are
      needed.
      For precision > 9, decimal digits after the 9nth are not correctly rounded, but floating point
      values survive the roundtrip of print_float -> scan_float exactly at precision 14. This means
      print_float is safe to use for serializing with precision set to 14.
      DO NOT USE print_double -> scan_double for serialization of doubles, they are wildly inaccurate.
    - print_double, print_float need 64 bit arithmetic

HISTORY
    v0.9.9.3   24.09.18 removed tm_bool32
                        removed print format structure
                        simplified signature of scan and print functions
                        removed some formatting features that complicated printing functions
                                                removed complicated base deduction from scanning functions
                                                removed prefix detection from scanning functions
                                                removed strnrev dependency
                        exhaustive testing
    v0.9.9.2   09.09.18 refactored and simplified preprocessor directives
                        support for compilation to dynamic library
    v0.9.9.1e  26.08.18 fixed C99 compilation
    v0.9.9.1d  25.08.18 added repository link
    v0.9.9.1c  19.08.18 added TMC_CPP_NO_DEFAULT_FUNCTION_ARGUMENTS and changed some function signatures
    v0.9.9.1b  13.08.18 documentation changes, added PF_FIXED
    v0.9.9.1a  13.08.18 typos
    v0.9.9.1   12.08.18 many small bug fixes and formatting fixes
                        implemented proper width padding support
                        some refactoring of underlying code
    v0.9.9b    11.08.18 removed TMC_CONVENIENCE and TMC_CPP_TEMPLATED, too much boilerplate
                        moved TMC_CPP_OVERLOADS into own namespace tmc
                        fixed scan_bool and print_bool not being available in C builds
    v0.9.9a    10.08.18 changed interface of all conversion functions to return an additional error code
    v0.9.8.2   22.07.18 fixed C99 compilation
    v0.9.8.1   22.07.18 changed formatting
                        fixed some warnings on gcc regarding -Wtype-limits and -Wsign-compare
    v0.9.8     18.02.17 fixed a bug in print_hex_u32 and print_hex_u64 not being able to print 0
    v0.9.7     18.01.17 added some more utility overloads
    v0.9.6a    10.01.17 minor change from static const char* to static const char* const in print_bool
    v0.9.6     07.11.16 increased print_double max precision from 9 to 14
    v0.9.5     23.10.16 fixed a buffer underflow bug in print_hex_* and prind_decimal_*
    v0.9.4c    08.10.16 fixed a buffer underflow bug in print_hex_u*_impl
    v0.9.4b    07.10.16 typos
    v0.9.4a    29.09.16 made PrintFormat forward declarable
    v0.9.4     24.09.16 optimized base 10 and base 16 conversion paths based on the talk
                        "Three Optimization Tips for C++" by Andrei Alexandrescu.
                        added print_decimal_* and print_hex_* functions.
                        renamed PF_PADDING_ZEROES to PF_TRAILING_ZEROES
                        fixed a bug in print_bool where it would print the wrong string if bool was
                        printed as "true"/"false" and outputted a nullterminator
    v0.9.3     30.08.16 added TMC_CPP_OVERLOADS and cpp overloads of functions
                        added TM_STRING_VIEW for string view support of functions
                        added TMC_CONVENIENCE and convenience functions
                        added TMC_CPP_TEMPLATED
                        fixed compile error when using tm_strnrev
                        added PURPOSE, WHY, SWITCHES, EXAMPLES
    v0.9.2	   07.08.16 fixed a bug in print_double not being able to print 10 (magnitude calculation
                        was wrong)
                        fixed a bug in print_double rounding wrong for 0.99 and precision 1
    v0.9.1     10.07.16 strncasecmp to strnicmp & print_Reverse to tm_strnrev
    v0.9a      01.07.16 improved C99 conformity
    v0.9       23.06.16 initial commit
*/

/*
EXAMPLES
Note that examples use <cstdio> and printf just to demonstrate that the outputs are correct
*/

#if 0
// example of scanning multiple values in a row and how to advance to the next number using scan

// main.cpp
#define TM_CONVERSION_IMPLEMENTATION
#include <tm_conversion.h>

#include <cstdio>

int main() {
	const char* str = "1234 5678 0xFF not_a_number";
	int32_t base        = 0;  // scan will attempt to determine base depending on input if base is 0
	for(;;) {
		// set a default value
		int value = 0;
		tmc_conv_result scan_result = scan_i32(str, base, &value);
		str += scan_result.size;  // advance str by number of bytes consumed by scan
		printf("%d\n", value);
		if(scan_result.size == 0 || scan_result.ec != TM_OK) {
			// scan will not consume any bytes if input isn't a number
			break;
		}
		if(*str) {
			++str;  // skip space
		} else {
			break;
		}
	}
	return 0;
}

/*
OUTPUT:
1234
5678
255
0
*/
#endif

#if 0
// implementing a string_builder class that is similar to std::stringstream which has
// C++ style operator<< overloads and error state

// main.cpp
#define TM_CONVERSION_IMPLEMENTATION
#define TMC_CPP_OVERLOADS
#include <tm_conversion.h>

#include <cstdio>
#include <cstring>

struct string_builder {
	char* ptr;
	size_t sz;
	size_t cap;
	PrintFormat format;
	tm_errc ec;

	char* data() { return ptr; }
	size_t size() { return sz; }
	char* end() { return ptr + sz; }
	size_t remaining() { return cap - sz; }
	void clear() {
		sz = 0;
		ec = TM_OK;
	}

	string_builder(char* ptr, size_t cap) : ptr(ptr), sz(0), cap(cap), format(defaultPrintFormat()) {}
	template <class T>
	string_builder& operator<<(T value) {
		auto print_result = tmc::print(end(), remaining(), &format, value);
		sz += print_result.size;
		ec = tmc_combine_errc(ec, print_result.ec);
		return *this;
	}
	string_builder& operator<<(const char* str) {
		auto print_result = tmc_print_string(end(), remaining(), str, strlen(str));
		sz += print_result.size;
		ec = tmc_combine_errc(ec, print_result.ec);
		return *this;
	}
};

int main() {
	const size_t bufferSize = 10000;
	char buffer[bufferSize];
	string_builder builder = {buffer, bufferSize};
	builder << "Hello World! " << 10 << " " << 3.1;

	printf("%.*s\n", (int)builder.size(), builder.data());
	return 0;
}

// OUTPUT: Hello World! 10 3.1
#endif

#if 0
// demonstrates how to use a string_view class with the conversion functions

// main.cpp
#include <cstring>  // strlen

// extremely simplified string_view class just for demonstration
// it represents non nullterminated views into strings
struct string_view {
	const char* ptr = nullptr;
	size_t sz       = 0;

	string_view() = default;
	string_view(const char* str) : ptr(str), sz((str) ? (strlen(str)) : (0)) {}
	string_view(const char* str, size_t len) : ptr(str), sz(len) {}
	const char* data() { return ptr; }
	size_t size() { return sz; }
	string_view substr(size_t pos, size_t len = (size_t)-1) {
		if(pos > sz) {
			pos = sz;
		}
		if(len > sz - pos) {
			len = sz - pos;
		}
		return string_view(ptr + pos, len);
	}
};

#define TM_CONVERSION_IMPLEMENTATION
#define TMC_CPP_OVERLOADS
#define TMC_CPP_TEMPLATED
#define TM_STRING_VIEW string_view
#define TM_STRING_VIEW_DATA(str) (str).data()
#define TM_STRING_VIEW_SIZE(str) (str).size()
#include <tm_conversion.h>

#include <cstdio>

int main() {
	string_view str  = "12345678";
	string_view sub0 = str.substr(2, 4);  // sub0 is "3456"
	int value        = 0;
	tmc::scan(sub0, 10, &value);

	printf("%d\n", value);
}
// OUTPUT: 3456
#endif

/* clang-format off */

/* assert */
#ifndef TM_ASSERT
	#include <assert.h>
	#define TM_ASSERT assert
#endif /* !defined(TM_ASSERT) */

#ifdef TM_CONVERSION_IMPLEMENTATION
	/* Define these to avoid external dependencies */

	/* ctype.h dependency */
	#if !defined(TM_ISDIGIT) || !defined(TM_ISUPPER) || !defined(TM_ISLOWER) || !defined(TM_TOUPPER)
		#include <ctype.h>
		#define TM_ISDIGIT isdigit
		#define TM_ISUPPER isupper
		#define TM_ISLOWER islower
		#define TM_TOUPPER toupper
	#endif

	/* string.h dependency */
	#if !defined(TM_MEMCPY) || !defined(TM_MEMSET)
		#include <string.h>
		#define TM_MEMCPY memcpy
		#define TM_MEMSET memset
	#endif

	/* math.h dependency */
	#if !defined(TM_SIGNBIT) || !defined(TM_ISNAN) || !defined(TM_ISINF)
		#include <math.h>
		#define TM_SIGNBIT signbit
		#define TM_ISNAN isnan
		#define TM_ISINF isinf
	#endif

	#ifndef TM_STRNICMP
		#define TM_IMPLEMENT_STRNICMP
		#define TM_STRNICMP tm_strnicmp
	#endif
#endif

#ifndef _TM_CONVERSION_H_INCLUDED_
#define _TM_CONVERSION_H_INCLUDED_

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* Linkage defaults to extern, to override define TMC_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMC_DEF
	#define TMC_DEF extern
#endif

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
	#define TM_SIZE_T_DEFINED
	#define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
	#include <stddef.h> /* include C version so identifiers are in global namespace */
	typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

/* Native bools, override by defining TM_BOOL_DEFINED yourself before including this file. */
#ifndef TM_BOOL_DEFINED
	#define TM_BOOL_DEFINED
	#ifdef __cplusplus
		typedef bool tm_bool;
		#define TM_TRUE true
		#define TM_FALSE false
	#else
		typedef _Bool tm_bool;
		#define TM_TRUE 1
		#define TM_FALSE 0
	#endif
#endif /* !defined(TM_BOOL_DEFINED) */

/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
	#define TM_ERRC_DEFINED
	enum TM_ERRC_CODES {
		TM_OK        = 0,   /* same as std::errc() */
		TM_EOVERFLOW = 75,  /* same as std::errc::value_too_large */
		TM_ERANGE    = 34,  /* same as std::errc::result_out_of_range */
		TM_EINVAL    = 22,  /* same as std::errc::invalid_argument */
	};
	typedef int tm_errc;
#endif

/* C++ string_view support. If TM_STRING_VIEW is defined, so must be TM_STRING_VIEW_DATA and TM_STRING_VIEW_SIZE.
   Example:
		#include <string_view>
		#define TM_STRING_VIEW std::string_view
		#define TM_STRING_VIEW_DATA(str) (str).data()
		#define TM_STRING_VIEW_SIZE(str) (str).size()
*/
#ifdef TM_STRING_VIEW
	#if !defined(TM_STRING_VIEW_DATA) || !defined(TM_STRING_VIEW_SIZE)
		#error Invalid TM_STRINV_VIEW. If TM_STRING_VIEW is defined, so must be TM_STRING_VIEW_DATA and TM_STRING_VIEW_SIZE.
	#endif
#endif

#ifdef __cplusplus
	#define TMC_UNDERLYING_U32 : uint32_t
#else
	#define TMC_UNDERLYING_U32
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Call this function with true to enable debug printing */
#if !defined(TMC_NO_DEBUG) && (defined(_DEBUG) || defined(TM_DEBUG)) && !defined(NDEBUG)
	extern void tmc_debug_enabled(tm_bool enabled);
#else
	#define tmc_debug_enabled(x) ((void)0)
#endif

/* clang-format on */

/* Feel free to typedef this to a shorter name */
typedef struct tmc_conv_result_struct {
    tm_size_t size;
    tm_errc ec;
} tmc_conv_result;

/* Helpers to combine two tmc_conv_results */
static inline tm_errc tmc_combine_errc(tm_errc a, tm_errc b) { return (a == TM_OK) ? b : a; }
static inline tmc_conv_result tmc_combine_conv_results(tmc_conv_result a, tmc_conv_result b) {
    tmc_conv_result result = {a.size + b.size, (a.ec == TM_OK) ? b.ec : a.ec};
    return result;
}
/* Helper to print strings that returns error codes when trying to print more than maxlen */
tmc_conv_result tmc_print_string(char* dest, tm_size_t maxlen, const char* src, tm_size_t srclen);

/*
These functions scan a value from a string and store it into out on success.
No prefixes on the string are allowed, like 0x or similar. Positive values can't have a plus sign in front.
A minus sign is only scanned for signed integer types.
Params:
    nullterminated: A nullterminated input string.
    base:           What base to scan in, valid values are 2 <= base <= 36.
    out:            Output value, will not be modified if an error occurs.
Return: Number of bytes consumed and error code if any,
    Error Codes:
        TM_OK            No error, size has number of bytes consumed.
        TM_EINVAL        If input string does not denote a number.
        TM_ERANGE        If input string cannot be represented, so result is out of range.

The _n variants take a string with a known length (they do not have to be nullterminated), useful
when scanning in substrings.
*/
TMC_DEF tmc_conv_result scan_i32(const char* nullterminated, int32_t* out, int32_t base);
TMC_DEF tmc_conv_result scan_i32_n(const char* str, tm_size_t len, int32_t* out, int32_t base);
TMC_DEF tmc_conv_result scan_u32(const char* nullterminated, uint32_t* out, int32_t base);
TMC_DEF tmc_conv_result scan_u32_n(const char* str, tm_size_t len, uint32_t* out, int32_t base);
TMC_DEF tmc_conv_result scan_i64(const char* nullterminated, int64_t* out, int32_t base);
TMC_DEF tmc_conv_result scan_i64_n(const char* str, tm_size_t len, int64_t* out, int32_t base);
TMC_DEF tmc_conv_result scan_u64(const char* nullterminated, uint64_t* out, int32_t base);
TMC_DEF tmc_conv_result scan_u64_n(const char* str, tm_size_t len, uint64_t* out, int32_t base);
TMC_DEF tmc_conv_result scan_float(const char* nullterminated, float* out, uint32_t flags);
TMC_DEF tmc_conv_result scan_float_n(const char* str, tm_size_t len, float* out, uint32_t flags);
TMC_DEF tmc_conv_result scan_double(const char* nullterminated, double* out, uint32_t flags);
TMC_DEF tmc_conv_result scan_double_n(const char* str, tm_size_t len, double* out, uint32_t flags);
TMC_DEF tmc_conv_result scan_bool(const char* nullterminated, tm_bool* out);
TMC_DEF tmc_conv_result scan_bool_n(const char* str, tm_size_t len, tm_bool* out);

enum PrintFlags TMC_UNDERLYING_U32 {
    PF_FIXED = (1u << 0u),      /* NOTE: Not implemented yet */
    PF_SCIENTIFIC = (1u << 1u), /* NOTE: Not implemented yet */
    PF_HEX = (1u << 2u),        /* NOTE: Not implemented yet */
    PF_TRAILING_ZEROES = (1u << 3u),
    PF_BOOL_AS_NUMBER = (1u << 4u),
    PF_LOWERCASE = (1u << 5u),
    PF_SIGNBIT = (1u << 6u) /* Whether to output -0 or 0 for negative 0. */
};

/*
These functions print a value into a destination buffer. Resulting string is not null terminated.
Params:
    dest:      Destination buffer.
    maxlen:    Max length of buffer.
    base:      What base to print in, valid values are 2 <= base <= 36.
    lowercase: Whether to print lowercase or uppercase digits when base > 10.
        flags: A combination of PrintFlags PF_* that control the printing behavior.
    precision: How many digits to print after the dot. A value of -1 denotes short(-ish) result.
    value:     Value to be printed.
Return: Returns number of bytes printed and error code if any.
    Error Codes:
        TM_OK            No error, size has number of bytes printed.
        TM_EOVERFLOW     If buffer denoted by dest and maxlen was not enough.
                         Returned size equals maxlen.
                         Output parameter dest is not safe to read, depending on the function it was't written into.
*/
TMC_DEF tmc_conv_result print_i32(char* dest, tm_size_t maxlen, int32_t value, int32_t base, tm_bool lowercase);
TMC_DEF tmc_conv_result print_u32(char* dest, tm_size_t maxlen, uint32_t value, int32_t base, tm_bool lowercase);
TMC_DEF tmc_conv_result print_i64(char* dest, tm_size_t maxlen, int64_t value, int32_t base, tm_bool lowercase);
TMC_DEF tmc_conv_result print_u64(char* dest, tm_size_t maxlen, uint64_t value, int32_t base, tm_bool lowercase);
TMC_DEF tmc_conv_result print_double(char* dest, tm_size_t maxlen, double value, uint32_t flags, int32_t precision);
TMC_DEF tmc_conv_result print_float(char* dest, tm_size_t maxlen, float value, uint32_t flags, int32_t precision);
TMC_DEF tmc_conv_result print_bool(char* dest, tm_size_t maxlen, tm_bool value, uint32_t flags);

/* Specialized printing functions that perform faster than the generic versions from above */

/* Prints base 10 number and returns length and error code if any */
TMC_DEF tmc_conv_result print_decimal_i32(char* dest, tm_size_t maxlen, int32_t value);
TMC_DEF tmc_conv_result print_decimal_u32(char* dest, tm_size_t maxlen, uint32_t value);
TMC_DEF tmc_conv_result print_decimal_i64(char* dest, tm_size_t maxlen, int64_t value);
TMC_DEF tmc_conv_result print_decimal_u64(char* dest, tm_size_t maxlen, uint64_t value);

/*
Prints base 16 number and returns length, does not prepend 0x before the number.
Params:
    dest:   Destination buffer.
    maxlen: Max length of buffer.
    lower:  Whether to use lowercase or uppercase to output hexadecimal digits A-F.
    value:  Value to be printed.
Return: Returns number of bytes printed and error code if any.
*/
TMC_DEF tmc_conv_result print_hex_i32(char* dest, tm_size_t maxlen, int32_t value, tm_bool lowercase);
TMC_DEF tmc_conv_result print_hex_u32(char* dest, tm_size_t maxlen, uint32_t value, tm_bool lowercase);
TMC_DEF tmc_conv_result print_hex_i64(char* dest, tm_size_t maxlen, int64_t value, tm_bool lowercase);
TMC_DEF tmc_conv_result print_hex_u64(char* dest, tm_size_t maxlen, uint64_t value, tm_bool lowercase);

/* Get how many digits will be printed for a given number when printed in decimal or hex */
TMC_DEF tm_size_t get_digits_count_decimal_u64(uint64_t number);
TMC_DEF tm_size_t get_digits_count_decimal_u32(uint32_t number);
TMC_DEF tm_size_t get_digits_count_hex_u32(uint32_t value);
TMC_DEF tm_size_t get_digits_count_hex_u64(uint64_t value);

TMC_DEF tm_size_t get_digits_count_u32(uint32_t number, int32_t base);
TMC_DEF tm_size_t get_digits_count_u64(uint64_t number, int32_t base);

/*
Meant for internal use, but can be useful outside for instance when implementing padded output.
Width has to equal get_digit_counts_*.
The only reason these functions exist is so that get_digit_counts_* isn't called twice
if caller also needs that information.
Calling these functions with the wrong width is undefined behaviour, the functions will write outside buffer bounds,
unless TMC_CHECKED_WIDTH is defined.
Define TMC_CHECKED_WIDTH to make these functions check the remaining size while printing.
See tmc_print_zero_padded_u64 for an example of padded output using these functions.
*/
TMC_DEF tmc_conv_result print_decimal_u32_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value);
TMC_DEF tmc_conv_result print_decimal_u64_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value);
TMC_DEF tmc_conv_result print_hex_u32_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value,
                                        tm_bool lowercase);
TMC_DEF tmc_conv_result print_hex_u64_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value,
                                        tm_bool lowercase);
TMC_DEF tmc_conv_result print_u32_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value, int32_t base,
                                    tm_bool lowercase);
TMC_DEF tmc_conv_result print_u64_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value, int32_t base,
                                    tm_bool lowercase);

#ifdef __cplusplus
}
#endif

/* #define TMC_CPP_OVERLOADS if you want cpp overloads, see C versions further below for description.
   These are defined as inline further below. */
#if defined(TMC_CPP_OVERLOADS) && defined(__cplusplus)

/* #define TMC_CPP_NO_DEFAULT_FUNCTION_ARGUMENTS if you want default function arguments. */
#if !defined(TMC_CPP_NO_DEFAULT_FUNCTION_ARGUMENTS)
TMC_DEF tmc_conv_result print_i32(char* dest, tm_size_t maxlen, int32_t value, int32_t base = 10,
                                  tm_bool lowercase = false);
TMC_DEF tmc_conv_result print_u32(char* dest, tm_size_t maxlen, uint32_t value, int32_t base = 10,
                                  tm_bool lowercase = false);
TMC_DEF tmc_conv_result print_i64(char* dest, tm_size_t maxlen, int64_t value, int32_t base = 10,
                                  tm_bool lowercase = false);
TMC_DEF tmc_conv_result print_u64(char* dest, tm_size_t maxlen, uint64_t value, int32_t base = 10,
                                  tm_bool lowercase = false);
TMC_DEF tmc_conv_result print_double(char* dest, tm_size_t maxlen, double value, uint32_t flags = 0,
                                     int32_t precision = -1);
TMC_DEF tmc_conv_result print_float(char* dest, tm_size_t maxlen, float value, uint32_t flags = 0,
                                    int32_t precision = -1);
TMC_DEF tmc_conv_result print_bool(char* dest, tm_size_t maxlen, tm_bool value, uint32_t flags = 0);
#endif /* defined(TMC_CPP_NO_DEFAULT_FUNCTION_ARGUMENTS) */

namespace tmc {
/* Overloads of scan, print functions. See C versions for documentation/usage */
tmc_conv_result scan(const char* nullterminated, int32_t* out, int32_t base = 10);
tmc_conv_result scan(const char* nullterminated, uint32_t* out, int32_t base = 10);
tmc_conv_result scan(const char* nullterminated, int64_t* out, int32_t base = 10);
tmc_conv_result scan(const char* nullterminated, uint64_t* out, int32_t base = 10);
tmc_conv_result scan(const char* nullterminated, float* out, uint32_t flags);
tmc_conv_result scan(const char* nullterminated, double* out, uint32_t flags);
tmc_conv_result scan(const char* nullterminated, bool* out);

tmc_conv_result scan(const char* str, tm_size_t len, int32_t* out, int32_t base = 10);
tmc_conv_result scan(const char* str, tm_size_t len, uint32_t* out, int32_t base = 10);
tmc_conv_result scan(const char* str, tm_size_t len, int64_t* out, int32_t base = 10);
tmc_conv_result scan(const char* str, tm_size_t len, uint64_t* out, int32_t base = 10);
tmc_conv_result scan(const char* str, tm_size_t len, float* out, uint32_t flags);
tmc_conv_result scan(const char* str, tm_size_t len, double* out, uint32_t flags);
tmc_conv_result scan(const char* str, tm_size_t len, bool* out);

tmc_conv_result print(char* dest, tm_size_t maxlen, int32_t value, int32_t base = 10, tm_bool lowercase = false);
tmc_conv_result print(char* dest, tm_size_t maxlen, uint32_t value, int32_t base = 10, tm_bool lowercase = false);
tmc_conv_result print(char* dest, tm_size_t maxlen, int64_t value, int32_t base = 10, tm_bool lowercase = false);
tmc_conv_result print(char* dest, tm_size_t maxlen, uint64_t value, int32_t base = 10, tm_bool lowercase = false);
tmc_conv_result print(char* dest, tm_size_t maxlen, double value, uint32_t flags = 0, int32_t precision = -1);
tmc_conv_result print(char* dest, tm_size_t maxlen, float value, uint32_t flags = 0, int32_t precision = -1);
/* Note that this is not external, so we can use a native bool overload here */
tmc_conv_result print(char* dest, tm_size_t maxlen, bool value, uint32_t flags = 0);

/* Overloads for some sort of string_view class */
#ifdef TM_STRING_VIEW
tmc_conv_result scan(TM_STRING_VIEW str, int32_t* out, int32_t base = 10);
tmc_conv_result scan(TM_STRING_VIEW str, uint32_t* out, int32_t base = 10);
tmc_conv_result scan(TM_STRING_VIEW str, int64_t* out, int32_t base = 10);
tmc_conv_result scan(TM_STRING_VIEW str, uint64_t* out, int32_t base = 10);
tmc_conv_result scan(TM_STRING_VIEW str, float* out);
tmc_conv_result scan(TM_STRING_VIEW str, double* out);
tmc_conv_result scan(TM_STRING_VIEW str, bool* out);
#endif /* TM_STRING_VIEW */
} /* namespace tmc */
#endif /* defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus ) */

#if defined(TMC_CPP_OVERLOADS) && defined(__cplusplus)
inline tmc_conv_result tmc::scan(const char* nullterminated, int32_t* out, int32_t base) {
    return scan_i32(nullterminated, out, base);
}
inline tmc_conv_result tmc::scan(const char* nullterminated, uint32_t* out, int32_t base) {
    return scan_u32(nullterminated, out, base);
}
inline tmc_conv_result tmc::scan(const char* nullterminated, int64_t* out, int32_t base) {
    return scan_i64(nullterminated, out, base);
}
inline tmc_conv_result tmc::scan(const char* nullterminated, uint64_t* out, int32_t base) {
    return scan_u64(nullterminated, out, base);
}
inline tmc_conv_result tmc::scan(const char* nullterminated, float* out, uint32_t flags) {
    return scan_float(nullterminated, out, flags);
}
inline tmc_conv_result tmc::scan(const char* nullterminated, double* out, uint32_t flags) {
    return scan_double(nullterminated, out, flags);
}
inline tmc_conv_result tmc::scan(const char* nullterminated, bool* out) { return scan_bool(nullterminated, out); }

inline tmc_conv_result tmc::scan(const char* str, tm_size_t len, int32_t* out, int32_t base) {
    return scan_i32_n(str, len, out, base);
}
inline tmc_conv_result tmc::scan(const char* str, tm_size_t len, uint32_t* out, int32_t base) {
    return scan_u32_n(str, len, out, base);
}
inline tmc_conv_result tmc::scan(const char* str, tm_size_t len, int64_t* out, int32_t base) {
    return scan_i64_n(str, len, out, base);
}
inline tmc_conv_result tmc::scan(const char* str, tm_size_t len, uint64_t* out, int32_t base) {
    return scan_u64_n(str, len, out, base);
}
inline tmc_conv_result tmc::scan(const char* str, tm_size_t len, float* out, uint32_t flags) {
    return scan_float_n(str, len, out, flags);
}
inline tmc_conv_result tmc::scan(const char* str, tm_size_t len, double* out, uint32_t flags) {
    return scan_double_n(str, len, out, flags);
}
inline tmc_conv_result tmc::scan(const char* str, tm_size_t len, bool* out) { return scan_bool_n(str, len, out); }

inline tmc_conv_result tmc::print(char* dest, tm_size_t maxlen, int32_t value, int32_t base, tm_bool lowercase) {
    return print_i32(dest, maxlen, value, base, lowercase);
}
inline tmc_conv_result tmc::print(char* dest, tm_size_t maxlen, uint32_t value, int32_t base, tm_bool lowercase) {
    return print_u32(dest, maxlen, value, base, lowercase);
}
inline tmc_conv_result tmc::print(char* dest, tm_size_t maxlen, int64_t value, int32_t base, tm_bool lowercase) {
    return print_i64(dest, maxlen, value, base, lowercase);
}
inline tmc_conv_result tmc::print(char* dest, tm_size_t maxlen, uint64_t value, int32_t base, tm_bool lowercase) {
    return print_u64(dest, maxlen, value, base, lowercase);
}
inline tmc_conv_result tmc::print(char* dest, tm_size_t maxlen, double value, uint32_t flags, int32_t precision) {
    return print_double(dest, maxlen, value, flags, precision);
}
inline tmc_conv_result tmc::print(char* dest, tm_size_t maxlen, float value, uint32_t flags, int32_t precision) {
    return print_float(dest, maxlen, value, flags, precision);
}
inline tmc_conv_result tmc::print(char* dest, tm_size_t maxlen, bool value, uint32_t flags) {
    return print_bool(dest, maxlen, value, flags);
}

#ifdef TM_STRING_VIEW
inline tmc_conv_result tmc::scan(TM_STRING_VIEW str, int32_t base, int32_t* out) {
    return scan_i32_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str), base, out);
}
inline tmc_conv_result tmc::scan(TM_STRING_VIEW str, int32_t base, uint32_t* out) {
    return scan_u32_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str), base, out);
}
inline tmc_conv_result tmc::scan(TM_STRING_VIEW str, int32_t base, int64_t* out) {
    return scan_i64_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str), base, out);
}
inline tmc_conv_result tmc::scan(TM_STRING_VIEW str, int32_t base, uint64_t* out) {
    return scan_u64_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str), base, out);
}
inline tmc_conv_result tmc::scan(TM_STRING_VIEW str, float* out) {
    return scan_float_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str), out);
}
inline tmc_conv_result tmc::scan(TM_STRING_VIEW str, double* out) {
    return scan_double_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str), out);
}
inline tmc_conv_result tmc::scan(TM_STRING_VIEW str, bool* out) {
    return scan_bool_n(TM_STRING_VIEW_DATA(str), TM_STRING_VIEW_SIZE(str), out);
}
#endif /* TM_STRING_VIEW */
#endif /* defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus ) */

#endif

/* Implementation */

#ifdef TM_CONVERSION_IMPLEMENTATION

/* clang-format off */
#ifndef TMC_CLAMP_ON_RANGE_ERROR
	#define TMC_CLAMP_ON_RANGE_ERROR 0
#endif

#ifndef TM_ASSERT_VALID_SIZE
	#if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
		#define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
	#else
		/* always true if size_t is unsigned */
		#define TM_ASSERT_VALID_SIZE(x) ((void)0)
	#endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

#ifdef __cplusplus
extern "C" {
#endif

#define TMC_CHAR_TO_INT(x) ((int32_t)((uint8_t)x))
#define TMC_MAX_PRECISION 14
#define TMC_DEFAULT_BASE 10

#ifdef TMC_CHECKED_WIDTH
	#define TMC_CW(x) x
#else
	#define TMC_CW(x)
#endif

#if !defined(TMC_NO_DEBUG) && (defined(_DEBUG) || defined(TM_DEBUG)) && !defined(NDEBUG)
	#include <stdio.h>

	static tm_bool tmc_global_debug_enabled = TM_FALSE;
	extern void tmc_debug_enabled(tm_bool enabled) { tmc_global_debug_enabled = enabled; }

	#define TMC_DEBUG(x)                    \
	    do {                                \
	        if (tmc_global_debug_enabled) { \
	            x;                          \
	        }                               \
	    } while (0)
#else
	#define TMC_DEBUG(x) ((void)0)
#endif
/* clang-format on */

TMC_DEF tmc_conv_result scan_i32(const char* nullterminated, int32_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    typedef uint32_t utype;
    typedef int32_t stype;
#if TMC_CLAMP_ON_RANGE_ERROR
    const stype MIN_VAL = INT32_MIN;
    const stype MAX_VAL = INT32_MAX;
#endif
    const utype UMIN_VAL = (utype)INT32_MIN;
    const utype UMAX_VAL = INT32_MAX;

    if (!nullterminated) {
        tmc_conv_result result = {0, TM_EINVAL};
        return result;
    }
    const char* start = nullterminated;
    const char* p = nullterminated;

    int negative = 0;
    if (*p == '-') {
        negative = 1;
        ++p;
    }

    const utype ubase = (utype)base;
    int rangeError = 0;
    const utype MAX_VAL = ((negative) ? (UMIN_VAL) : (UMAX_VAL));
    const utype maxDigit = MAX_VAL % ubase;
    const utype maxValue = MAX_VAL / ubase;
    utype value = 0;
    for (; *p; ++p) {
        int32_t cp = TMC_CHAR_TO_INT(*p);
        utype digit;
        if (TM_ISDIGIT(cp)) {
            digit = cp - '0';
        } else if (TM_ISUPPER(cp)) {
            digit = cp - 'A' + 10;
        } else if (TM_ISLOWER(cp)) {
            digit = cp - 'a' + 10;
        } else {
            break;
        }
        if (digit >= ubase) {
            break;
        }
        if (rangeError || value > maxValue || (value == maxValue && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = value * ubase + digit;
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);
    tmc_conv_result result = {0, TM_EINVAL};

    /* Did we scan anything or just a sign? */
    if (dist == 0 || (negative && dist == 1)) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = (negative) ? (MIN_VAL) : (MAX_VAL);
#endif
        } else {
            *out = (negative) ? (-(stype)value) : ((stype)value);
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_i32_n(const char* str, tm_size_t len, int32_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len <= 0);

    typedef uint32_t utype;
    typedef int32_t stype;
#if TMC_CLAMP_ON_RANGE_ERROR
    const stype MIN_VAL = INT32_MIN;
    const stype MAX_VAL = INT32_MAX;
#endif
    const utype UMIN_VAL = (utype)INT32_MIN;
    const utype UMAX_VAL = INT32_MAX;

    tmc_conv_result result = {0, TM_EINVAL};
    if (len <= 0) {
        return result;
    }

    const char* start = str;
    const char* p = str;

    int negative = 0;
    if (*p == '-') {
        negative = 1;
        ++p;
        --len;
    }

    const utype ubase = (utype)base;
    int rangeError = 0;
    const utype MAX_VAL = (negative) ? (UMIN_VAL) : (UMAX_VAL);
    const utype maxDigit = MAX_VAL % ubase;
    const utype maxValue = MAX_VAL / ubase;
    utype value = 0;
    for (; len; ++p, --len) {
        int32_t cp = TMC_CHAR_TO_INT(*p);
        utype digit;
        if (TM_ISDIGIT(cp)) {
            digit = cp - '0';
        } else if (TM_ISUPPER(cp)) {
            digit = cp - 'A' + 10;
        } else if (TM_ISLOWER(cp)) {
            digit = cp - 'a' + 10;
        } else {
            break;
        }
        if (digit >= ubase) {
            break;
        }
        if (rangeError || value > maxValue || (value == maxValue && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = value * ubase + digit;
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);

    /* Did we scan anything or just a sign? */
    if (dist == 0 || (negative && dist == 1)) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = (negative) ? (MIN_VAL) : (MAX_VAL);
#endif
        } else {
            *out = (negative) ? (-(stype)value) : ((stype)value);
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_u32(const char* nullterminated, uint32_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    typedef uint32_t utype;
    const utype UMAX_VAL = UINT32_MAX;

    if (!nullterminated) {
        tmc_conv_result result = {0, TM_EINVAL};
        return result;
    }
    const char* start = nullterminated;
    const char* p = nullterminated;

    const utype ubase = (utype)base;
    int rangeError = 0;
    const utype maxValue = UMAX_VAL / ubase;
    const utype maxDigit = UMAX_VAL % ubase;
    utype value = 0;
    for (; *p; ++p) {
        int32_t cp = TMC_CHAR_TO_INT(*p);
        utype digit;
        if (TM_ISDIGIT(cp)) {
            digit = cp - '0';
        } else if (TM_ISUPPER(cp)) {
            digit = cp - 'A' + 10;
        } else if (TM_ISLOWER(cp)) {
            digit = cp - 'a' + 10;
        } else {
            break;
        }
        if (digit >= ubase) {
            break;
        }
        if (rangeError || value > maxValue || (value == maxValue && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = value * ubase + digit;
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);
    tmc_conv_result result = {0, TM_EINVAL};

    /* Did we scan anything? */
    if (dist == 0) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = UMAX_VAL;
#endif
        } else {
            *out = value;
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_u32_n(const char* str, tm_size_t len, uint32_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len <= 0);

    typedef uint32_t utype;
    const utype UMAX_VAL = UINT32_MAX;

    tmc_conv_result result = {0, TM_EINVAL};
    if (len <= 0) {
        return result;
    }
    const char* start = str;
    const char* p = str;

    const utype ubase = (utype)base;
    int rangeError = 0;
    const utype maxValue = UMAX_VAL / ubase;
    const utype maxDigit = UMAX_VAL % ubase;
    utype value = 0;
    for (; len; ++p, --len) {
        int32_t cp = TMC_CHAR_TO_INT(*p);
        utype digit;
        if (TM_ISDIGIT(cp)) {
            digit = cp - '0';
        } else if (TM_ISUPPER(cp)) {
            digit = cp - 'A' + 10;
        } else if (TM_ISLOWER(cp)) {
            digit = cp - 'a' + 10;
        } else {
            break;
        }
        if (digit >= ubase) {
            break;
        }
        if (rangeError || value > maxValue || (value == maxValue && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = value * ubase + digit;
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);

    /* Did we scan anything? */
    if (dist == 0) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = UMAX_VAL;
#endif
        } else {
            *out = value;
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_i64(const char* nullterminated, int64_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    typedef uint64_t utype;
    typedef int64_t stype;
#if TMC_CLAMP_ON_RANGE_ERROR
    const stype MIN_VAL = INT64_MIN;
    const stype MAX_VAL = INT64_MAX;
#endif
    const utype UMIN_VAL = (utype)INT64_MIN;
    const utype UMAX_VAL = INT64_MAX;

    if (!nullterminated) {
        tmc_conv_result result = {0, TM_EINVAL};
        return result;
    }
    const char* start = nullterminated;
    const char* p = nullterminated;

    int negative = 0;
    if (*p == '-') {
        negative = 1;
        ++p;
    }

    const utype ubase = (utype)base;
    int rangeError = 0;
    const utype MAX_VAL = (negative) ? (UMIN_VAL) : (UMAX_VAL);
    const utype maxDigit = MAX_VAL % ubase;
    const utype maxValue = MAX_VAL / ubase;
    utype value = 0;
    for (; *p; ++p) {
        int32_t cp = TMC_CHAR_TO_INT(*p);
        utype digit;
        if (TM_ISDIGIT(cp)) {
            digit = cp - '0';
        } else if (TM_ISUPPER(cp)) {
            digit = cp - 'A' + 10;
        } else if (TM_ISLOWER(cp)) {
            digit = cp - 'a' + 10;
        } else {
            break;
        }
        if (digit >= ubase) {
            break;
        }
        if (rangeError || value > maxValue || (value == maxValue && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = value * ubase + digit;
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);
    tmc_conv_result result = {0, TM_EINVAL};

    /* Did we scan anything or just a sign? */
    if (dist == 0 || (negative && dist == 1)) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = (negative) ? (MIN_VAL) : (MAX_VAL);
#endif
        } else {
            *out = (negative) ? (-(stype)value) : ((stype)value);
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_i64_n(const char* str, tm_size_t len, int64_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len <= 0);

    typedef uint64_t utype;
    typedef int64_t stype;
#if TMC_CLAMP_ON_RANGE_ERROR
    const stype MIN_VAL = INT64_MIN;
    const stype MAX_VAL = INT64_MAX;
#endif
    const utype UMIN_VAL = (utype)INT64_MIN;
    const utype UMAX_VAL = INT64_MAX;

    tmc_conv_result result = {0, TM_EINVAL};
    if (len <= 0) {
        return result;
    }
    const char* start = str;
    const char* p = str;

    int negative = 0;
    if (*p == '-') {
        negative = 1;
        ++p;
        --len;
    }

    const utype ubase = (utype)base;
    int rangeError = 0;
    const utype MAX_VAL = (negative) ? (UMIN_VAL) : (UMAX_VAL);
    const utype maxDigit = MAX_VAL % ubase;
    const utype maxValue = MAX_VAL / ubase;
    utype value = 0;
    for (; len; ++p, --len) {
        int32_t cp = TMC_CHAR_TO_INT(*p);
        utype digit;
        if (TM_ISDIGIT(cp)) {
            digit = cp - '0';
        } else if (TM_ISUPPER(cp)) {
            digit = cp - 'A' + 10;
        } else if (TM_ISLOWER(cp)) {
            digit = cp - 'a' + 10;
        } else {
            break;
        }
        if (digit >= ubase) {
            break;
        }
        if (rangeError || value > maxValue || (value == maxValue && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = value * ubase + digit;
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);
    /* Did we scan anything or just a sign? */
    if (dist == 0 || (negative && dist == 1)) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = (negative) ? (MIN_VAL) : (MAX_VAL);
#endif
        } else {
            *out = (negative) ? (-(stype)value) : ((stype)value);
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_u64(const char* nullterminated, uint64_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    typedef uint64_t utype;
    const utype UMAX_VAL = UINT64_MAX;

    TM_ASSERT(base >= 0);

    if (!nullterminated) {
        tmc_conv_result result = {0, TM_EINVAL};
        return result;
    }
    const char* start = nullterminated;
    const char* p = nullterminated;

    const utype ubase = (utype)base;
    int rangeError = 0;
    const utype maxValue = UMAX_VAL / ubase;
    const utype maxDigit = UMAX_VAL % ubase;
    utype value = 0;
    for (; *p; ++p) {
        int32_t cp = TMC_CHAR_TO_INT(*p);
        utype digit;
        if (TM_ISDIGIT(cp)) {
            digit = cp - '0';
        } else if (TM_ISUPPER(cp)) {
            digit = cp - 'A' + 10;
        } else if (TM_ISLOWER(cp)) {
            digit = cp - 'a' + 10;
        } else {
            break;
        }
        if (digit >= ubase) {
            break;
        }
        if (rangeError || value > maxValue || (value == maxValue && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = value * ubase + digit;
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);
    tmc_conv_result result = {0, TM_EINVAL};

    /* Did we scan anything? */
    if (dist == 0) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = UMAX_VAL;
#endif
        } else {
            *out = value;
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_u64_n(const char* str, tm_size_t len, uint64_t* out, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len <= 0);

    typedef uint64_t utype;
    const utype UMAX_VAL = UINT64_MAX;

    tmc_conv_result result = {0, TM_EINVAL};
    if (len <= 0) {
        return result;
    }
    const char* start = str;
    const char* p = str;

    const utype ubase = (utype)base;
    int rangeError = 0;
    const utype maxValue = UMAX_VAL / ubase;
    const utype maxDigit = UMAX_VAL % ubase;
    utype value = 0;
    for (; len; ++p, --len) {
        int32_t cp = TMC_CHAR_TO_INT(*p);
        utype digit;
        if (TM_ISDIGIT(cp)) {
            digit = cp - '0';
        } else if (TM_ISUPPER(cp)) {
            digit = cp - 'A' + 10;
        } else if (TM_ISLOWER(cp)) {
            digit = cp - 'a' + 10;
        } else {
            break;
        }
        if (digit >= ubase) {
            break;
        }
        if (rangeError || value > maxValue || (value == maxValue && digit > maxDigit)) {
            rangeError = 1;
        } else {
            value = value * ubase + digit;
        }
    }
    tm_size_t dist = (tm_size_t)(p - start);

    /* Did we scan anything? */
    if (dist == 0) {
        return result;
    }
    if (out) {
        if (rangeError) {
#if TMC_CLAMP_ON_RANGE_ERROR
            *out = UMAX_VAL;
#endif
        } else {
            *out = value;
        }
    }

    result.size = dist;
    result.ec = (rangeError) ? TM_ERANGE : TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_float(const char* nullterminated, float* out, uint32_t flags) {
    double val;
    tmc_conv_result ret = scan_double(nullterminated, &val, flags);
    if (ret.size && out) {
        if (ret.ec == TM_OK
#if TMC_CLAMP_ON_RANGE_ERROR
            || ret.ec == TM_ERANGE
#endif
        ) {
            *out = (float)val;
        }
    }
    return ret;
}
TMC_DEF tmc_conv_result scan_float_n(const char* str, tm_size_t len, float* out, uint32_t flags) {
    double val;
    tmc_conv_result ret = scan_double_n(str, len, &val, flags);
    if (ret.size && out) {
        if (ret.ec == TM_OK
#if TMC_CLAMP_ON_RANGE_ERROR
            || ret.ec == TM_ERANGE
#endif
        ) {
            *out = (float)val;
        }
    }
    return ret;
}

/*
 * Doubles in strings follow this format:
 * 1234.56789e12
 * We will scan doubles like this:
 * Scan the integer part and fractional part of the string marking where the decimal point is.
 * Scan the exponent if it exists.
 * Change where the decimal point is in the string based on the exponent as much as possible given
 * the number of digits and adjust exponent accordingly.
 * Scan the integer and fractional parts as integers and convert to double.
 * Apply exponent to integer part and exponent + decimal point to fractional part and add them
 * together.
 */
static const int scan_MaxExponent = 511;

static tm_size_t scan_LeadingZeroes(const char* str, tm_size_t len) {
    const char* start = str;
    while (len && (*str == '0' || *str == '.')) {
        ++str;
        --len;
    }
    return (tm_size_t)(str - start);
}
static tm_size_t scan_ScanMantissa(const char* str, tm_size_t len, double* out) {
    /*
     * There are three ways to scan a string into a double.
     * Scan directly into a double, using floating arithmetic (slightly slow).
     * Scan using int64 and convert to double (uint64 might not be available).
     * Scan using two int32_t's and combine into double (portable and fast).
     * We will be using the third option.
     */

    /* Check whether the digits can be represented in a double */
    if (len > 18) {
        len = 18;
        /* overflow */
    }

    const char* start = str;

    int32_t highPart = 0;
    while (len > 9) {
        int32_t cp = TMC_CHAR_TO_INT(*str);
        /* Skip dot */
        if (cp == '.') {
            ++str;
            /* We do not decrease len when we encounter the decimal point, because len only counts digits */
            cp = TMC_CHAR_TO_INT(*str);
        }
        highPart = highPart * 10 + (cp - '0');
        ++str;
        --len;
    }
    int32_t lowPart = 0;
    while (len) {
        int32_t cp = TMC_CHAR_TO_INT(*str);
        /* Skip dot */
        if (cp == '.') {
            ++str;
            /* We do not decrease len when we encounter the decimal point, because len only counts digits */
            cp = TMC_CHAR_TO_INT(*str);
        }
        lowPart = lowPart * 10 + (cp - '0');
        ++str;
        --len;
    }
    *out = (1.0e9 * highPart) + lowPart;
    return (tm_size_t)(str - start);
}
static double str_Pow10(double value, int exponent) {
    TM_ASSERT(exponent < scan_MaxExponent);
    /* We use the exponentiation by squaring algorithm to calculate pow(10, exponent),
       while using a lookup table for the squared powers of 10 */
    static const double PowersOfTen[] = {
        10, 100, 1.0e4, 1.0e8, 1.0e16, 1.0e32, 1.0e64, 1.0e128, 1.0e256,
    };

    int exponentNegative = exponent < 0;
    if (exponentNegative) {
        exponent = -exponent;
    }

    double tenToThePower = 1.0;
    uint32_t uexp = (uint32_t)exponent; /* Convert to uint32_t for bit twiddling. */
    for (const double* entry = PowersOfTen; uexp; uexp >>= 1, ++entry) {
        if (uexp & 0x1) {
            tenToThePower *= *entry;
        }
    }
    if (exponentNegative) {
        value /= tenToThePower;
    } else {
        value *= tenToThePower;
    }
    return value;
}

TMC_DEF tmc_conv_result scan_double(const char* nullterminated, double* out, uint32_t flags) {
    /* TODO: implement flags */
    (void)flags;

    tmc_conv_result result = {0, TM_EINVAL};

    if (!nullterminated) {
        return result;
    }

    const char* start = nullterminated;
    const char* p = nullterminated;

    int negative = 0;
    if (*p == '-') {
        negative = 1;
        ++p;
    }

    /* Scan integer and frational parts */
    int decimalPos = -1;
    int mantissaLength = 0;
    int integerLength = 0;
    int fractionalLength = 0;
    for (const char* s = p;; ++s, ++mantissaLength) {
        int32_t cp = TMC_CHAR_TO_INT(*s);
        if (cp == '.' && decimalPos < 0) {
            decimalPos = mantissaLength + 1;
            integerLength = mantissaLength;
        } else if (!TM_ISDIGIT(cp)) {
            break;
        }
    }
    if (decimalPos < 0 && mantissaLength == 0) {
        return result;
    }
    if (decimalPos < 0) {
        decimalPos = mantissaLength;
        fractionalLength = 0;
        integerLength = mantissaLength;
    } else {
        fractionalLength = mantissaLength - decimalPos;
    }

    /* Scan exponent */
    const char* pExp = p + mantissaLength;
    int exponentNegative = 0;
    int exponent = 0;
    if (*pExp == 'e' || *pExp == 'E') {
        ++pExp;
        if (*pExp == '-') {
            exponentNegative = 1;
            ++pExp;
        } else if (*pExp == '+') {
            ++pExp;
        }
        if (TM_ISDIGIT(TMC_CHAR_TO_INT(*pExp))) {
            tmc_conv_result exponentResult = scan_i32(pExp, &exponent, 10);
            pExp += exponentResult.size;
            if (exponentResult.ec != TM_OK) {
                exponentResult.size = (tm_size_t)(pExp - start);
                return exponentResult;
            }
            if (exponent > scan_MaxExponent) {
#if TMC_CLAMP_ON_RANGE_ERROR
                exponent = scan_MaxExponent;
#else
                result.size = (tm_size_t)(pExp - start);
                result.ec = TM_EOVERFLOW;
                return result;
#endif
            }
        } else {
            pExp = p + mantissaLength;
        }
    }

    /* Adjust integerLength and fractionalLength based on exponent */
    if (exponentNegative) {
        if (integerLength > exponent) {
            integerLength -= exponent;
            fractionalLength += exponent;
            exponent = 0;
        } else {
            exponent -= integerLength;
            fractionalLength += integerLength;
            integerLength = 0;
        }
        exponent = -exponent;
    } else {
        if (fractionalLength > exponent) {
            fractionalLength -= exponent;
            integerLength += exponent;
            exponent = 0;
        } else {
            exponent -= fractionalLength;
            integerLength += fractionalLength;
            fractionalLength = 0;
        }
    }

    if (out) {
        double integerPart = 0;
        double fractionalPart = 0;
        tm_size_t leadingZeroes = scan_LeadingZeroes(p, integerLength);
        if (leadingZeroes < (tm_size_t)integerLength) {
            p += leadingZeroes;
            integerLength -= (int)leadingZeroes;
            p += scan_ScanMantissa(p, integerLength, &integerPart);
        } else {
            p += integerLength;
        }
        scan_ScanMantissa(p, fractionalLength, &fractionalPart);
        fractionalPart = str_Pow10(fractionalPart, -fractionalLength);

        double value = str_Pow10(integerPart + fractionalPart, exponent);
        *out = (negative) ? (-value) : (value);
    }
    result.size = (tm_size_t)(pExp - start);
    result.ec = TM_OK;
    return result;
}
TMC_DEF tmc_conv_result scan_double_n(const char* str, tm_size_t len, double* out, uint32_t flags) {
    /* TODO: implement flags */
    (void)flags;

    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len <= 0);

    tmc_conv_result result = {0, TM_EINVAL};

    if (len <= 0) {
        return result;
    }

    const char* start = str;
    const char* p = str;

    int negative = 0;
    if (*p == '-') {
        negative = 1;
        ++p;
        --len;
    }

    /* Scan integer and frational parts */
    int decimalPos = -1;
    int mantissaLength = 0;
    int integerLength = 0;
    int fractionalLength = 0;
    for (const char* s = p; len; ++s, ++mantissaLength, --len) {
        int32_t cp = TMC_CHAR_TO_INT(*s);
        if (cp == '.' && decimalPos < 0) {
            decimalPos = mantissaLength + 1;
            integerLength = mantissaLength;
        } else if (!TM_ISDIGIT(cp)) {
            break;
        }
    }
    if (decimalPos < 0 && mantissaLength == 0) {
        return result;
    }
    if (decimalPos < 0) {
        decimalPos = mantissaLength;
        fractionalLength = 0;
        integerLength = mantissaLength;
    } else {
        fractionalLength = mantissaLength - decimalPos;
    }

    /* Scan exponent */
    const char* pExp = p + mantissaLength;
    int exponentNegative = 0;
    int exponent = 0;
    if (len && (*pExp == 'e' || *pExp == 'E')) {
        ++pExp;
        --len;
        if (len && *pExp == '-') {
            exponentNegative = 1;
            ++pExp;
            --len;
        } else if (len && *pExp == '+') {
            ++pExp;
            --len;
        }
        if (len && TM_ISDIGIT(TMC_CHAR_TO_INT(*pExp))) {
            tmc_conv_result exponentResult = scan_i32_n(pExp, len, &exponent, 10);
            pExp += exponentResult.size;
            if (exponentResult.ec != TM_OK) {
                exponentResult.size = (tm_size_t)(pExp - start);
                return exponentResult;
            }
            if (exponent > scan_MaxExponent) {
#if TMC_CLAMP_ON_RANGE_ERROR
                exponent = scan_MaxExponent;
#else
                result.size = (tm_size_t)(pExp - start);
                result.ec = TM_EOVERFLOW;
                return result;
#endif
            }
        } else {
            pExp = p + mantissaLength;
        }
    }

    /* Adjust integerLength and fractionalLength based on exponent */
    if (exponentNegative) {
        if (integerLength > exponent) {
            integerLength -= exponent;
            fractionalLength += exponent;
            exponent = 0;
        } else {
            exponent -= integerLength;
            fractionalLength += integerLength;
            integerLength = 0;
        }
        exponent = -exponent;
    } else {
        if (fractionalLength > exponent) {
            fractionalLength -= exponent;
            integerLength += exponent;
            exponent = 0;
        } else {
            exponent -= fractionalLength;
            integerLength += fractionalLength;
            fractionalLength = 0;
        }
    }

    if (out) {
        double integerPart = 0;
        double fractionalPart = 0;
        tm_size_t leadingZeroes = scan_LeadingZeroes(p, integerLength);
        if (leadingZeroes < (tm_size_t)integerLength) {
            p += leadingZeroes;
            integerLength -= (int)leadingZeroes;
            p += scan_ScanMantissa(p, integerLength, &integerPart);
        } else {
            p += integerLength;
        }
        scan_ScanMantissa(p, fractionalLength, &fractionalPart);
        fractionalPart = str_Pow10(fractionalPart, -fractionalLength);

        double value = str_Pow10(integerPart + fractionalPart, exponent);
        *out = (negative) ? (-value) : (value);
    }
    result.size = (tm_size_t)(pExp - start);
    result.ec = TM_OK;
    return result;
}

TMC_DEF tmc_conv_result tmc_print_string(char* dest, tm_size_t maxlen, const char* src, tm_size_t srclen) {
    tmc_conv_result result;
    if (maxlen < srclen) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
    } else {
        result.size = srclen;
        result.ec = TM_OK;
    }
    TM_ASSERT(dest);
    TM_ASSERT(src);
    TM_ASSERT(result.size > 0);
    TM_MEMCPY(dest, src, result.size);
    return result;
}

#if defined(TM_IMPLEMENT_STRNICMP)
static int32_t tm_strnicmp(const char* a, const char* b, tm_size_t maxlen) {
    TM_ASSERT_VALID_SIZE(maxlen);
    while (*a && *b && maxlen--) {
        int32_t aUpper = TM_TOUPPER(TMC_CHAR_TO_INT(*a));
        int32_t bUpper = TM_TOUPPER(TMC_CHAR_TO_INT(*b));
        if (aUpper != bUpper) {
            break;
        }
        ++a;
        ++b;
    }
    if (!maxlen) {
        return 0;
    }
    int32_t aUpper = TM_TOUPPER(TMC_CHAR_TO_INT(*a));
    int32_t bUpper = TM_TOUPPER(TMC_CHAR_TO_INT(*b));
    return aUpper - bUpper;
}
#endif /* defined(TM_IMPLEMENT_STRNICMP) */

TMC_DEF tmc_conv_result scan_bool(const char* nullterminated, tm_bool* out) {
    tmc_conv_result result = {0, TM_EINVAL};
    if (!nullterminated) {
        return result;
    }
    if (*nullterminated == '1') {
        if (out) {
            *out = TM_TRUE;
        }
        result.size = 1;
        result.ec = TM_OK;
    } else if (*nullterminated == '0') {
        if (out) {
            *out = TM_FALSE;
        }
        result.size = 1;
        result.ec = TM_OK;
    } else if (TM_STRNICMP(nullterminated, "true", 4) == 0) {
        if (out) {
            *out = TM_TRUE;
        }
        result.size = 4;
        result.ec = TM_OK;
    } else if (TM_STRNICMP(nullterminated, "false", 5) == 0) {
        if (out) {
            *out = TM_FALSE;
        }
        result.size = 5;
        result.ec = TM_OK;
    }
    return result;
}
TMC_DEF tmc_conv_result scan_bool_n(const char* str, tm_size_t len, tm_bool* out) {
    TM_ASSERT_VALID_SIZE(len);
    TM_ASSERT(str || len <= 0);

    tmc_conv_result result = {0, TM_EINVAL};
    if (len <= 0) {
        return result;
    }
    if (*str == '1') {
        if (out) {
            *out = TM_TRUE;
        }
        result.size = 1;
        result.ec = TM_OK;
    } else if (*str == '0') {
        if (out) {
            *out = TM_FALSE;
        }
        result.size = 1;
        result.ec = TM_OK;
    } else if (TM_STRNICMP(str, "true", len) == 0) {
        if (out) {
            *out = TM_TRUE;
        }
        result.size = 4;
        result.ec = TM_OK;
    } else if (TM_STRNICMP(str, "false", len) == 0) {
        if (out) {
            *out = TM_FALSE;
        }
        result.size = 5;
        result.ec = TM_OK;
    }
    return result;
}

/* print implementation */

TMC_DEF tmc_conv_result print_bool(char* dest, tm_size_t maxlen, tm_bool value, uint32_t flags) {
    if (flags & PF_BOOL_AS_NUMBER) {
        return tmc_print_string(dest, maxlen, (value) ? ("1") : ("0"), 1);
    } else {
        static const char* const strings[] = {"false", "true"};
        static const tm_size_t lengths[] = {sizeof("false") - 1, sizeof("true") - 1};
        int index = value != 0;
        return tmc_print_string(dest, maxlen, strings[index], lengths[index]);
    }
}

static const char print_NumberToCharTableUpper[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
                                                    'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                                    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
static const char print_NumberToCharTableLower[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
                                                    'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                                    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

/* table of double digit chars from 00 to 99 */
static const char print_DoubleDigitsToCharTable[200] = {
    '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9', '1', '0', '1',
    '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9', '2', '0', '2', '1', '2', '2',
    '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9', '3', '0', '3', '1', '3', '2', '3', '3', '3',
    '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9', '4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5',
    '4', '6', '4', '7', '4', '8', '4', '9', '5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5',
    '7', '5', '8', '5', '9', '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8',
    '6', '9', '7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9', '8',
    '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9', '9', '0', '9', '1',
    '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'};

static const double print_PowersOfTen[] = {1,     10,    100,    1.0e3,  1.0e4,  1.0e5,  1.0e6, 1.0e7,
                                           1.0e8, 1.0e9, 1.0e10, 1.0e11, 1.0e12, 1.0e13, 1.0e14};
/* magnitude is the number of digits before decimal point */
static int32_t print_magnitude(double value) {
    /* calculate integer based log10 */
    int32_t n = 0;
    if (value < 0) {
        value = -value;
    }
    while (value >= 10) {
        if (value >= 1.0e32) {
            n += 32;
            value /= 1.0e32;
        }
        if (value >= 1.0e16) {
            n += 16;
            value /= 1.0e16;
        }
        if (value >= 1.0e8) {
            n += 8;
            value /= 1.0e8;
        }
        if (value >= 1.0e4) {
            n += 4;
            value /= 1.0e4;
        }
        if (value >= 1.0e2) {
            n += 2;
            value /= 1.0e2;
        }
        if (value >= 1.0e1) {
            n += 1;
            value /= 1.0e1;
        }
    }

    /* return magnitude = log10(value) + 1 */
    return n + 1;
}

static const uint32_t tmc_UintPowerOfTen[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

TMC_DEF tm_size_t get_digits_count_decimal_u64(uint64_t number) {
    const uint64_t P01 = 10;
    const uint64_t P02 = 100;
    const uint64_t P03 = 1000;
    const uint64_t P04 = 10000;
    const uint64_t P05 = 100000;
    const uint64_t P06 = 1000000;
    const uint64_t P07 = 10000000;
    const uint64_t P08 = 100000000;
    const uint64_t P09 = 1000000000;
    const uint64_t P10 = 10000000000;
    const uint64_t P11 = 100000000000;
    const uint64_t P12 = 1000000000000;
    if (number < P01) {
        return 1;
    }
    if (number < P02) {
        return 2;
    }
    if (number < P03) {
        return 3;
    }
    if (number < P12) {
        if (number < P08) {
            if (number < P06) {
                if (number < P04) {
                    return 4;
                }
                return 5 + (number >= P05);
            }
            return 7 + (number >= P07);
        }
        if (number < P10) {
            return 9 + (number >= P09);
        }
        return 11 + (number >= P11);
    }
    return 12 + get_digits_count_decimal_u64(number / P12);
}
TMC_DEF tm_size_t get_digits_count_decimal_u32(uint32_t number) {
    const uint32_t P01 = 10;
    const uint32_t P02 = 100;
    const uint32_t P03 = 1000;
    const uint32_t P04 = 10000;
    const uint32_t P05 = 100000;
    const uint32_t P06 = 1000000;
    const uint32_t P07 = 10000000;
    const uint32_t P08 = 100000000;
    const uint32_t P09 = 1000000000;
    if (number < P01) {
        return 1;
    }
    if (number < P02) {
        return 2;
    }
    if (number < P03) {
        return 3;
    }
    if (number < P08) {
        if (number < P06) {
            if (number < P04) {
                return 4;
            }
            return 5 + (number >= P05);
        }
        return 7 + (number >= P07);
    }
    return 9 + (number >= P09);
}

TMC_DEF tmc_conv_result print_decimal_u32_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value) {
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);
    /* See comment on declaration for why width has to equal a specific value. */
    TM_ASSERT(width > 0);
    TM_ASSERT(width == get_digits_count_decimal_u32(value));

    tmc_conv_result result = {0, TM_OK};
    if (width > maxlen TMC_CW(|| width <= 0)) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }

    result.size = width;
    char* p = dest + width - 1;
    while (value >= 100 TMC_CW(&& width >= 2)) {
        uint32_t index = (value % 100) * 2;
        TM_ASSERT(index < 200);
        value /= 100;
        *p = print_DoubleDigitsToCharTable[index + 1];
        *(p - 1) = print_DoubleDigitsToCharTable[index];
        p -= 2;
        TMC_CW(width -= 2);
    }

#ifdef TMC_CHECKED_WIDTH
    if ((value < 10 && width != 1) || (value >= 10 && width != 2)) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }
#endif

    /* Handle last 1-2 digits */
    if (value < 10) {
        *p = (char)('0' + (uint32_t)value);
    } else {
        TM_ASSERT(value < 100);
        uint32_t index = (uint32_t)value * 2;
        TM_ASSERT(index < 200);
        *p = print_DoubleDigitsToCharTable[index + 1];
        *(p - 1) = print_DoubleDigitsToCharTable[index];
    }

    return result;
}
TMC_DEF tmc_conv_result print_decimal_u64_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value) {
    TM_ASSERT_VALID_SIZE(width);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);
    /* See comment on declaration for why width has to equal a specific value. */
    TM_ASSERT(width > 0);
    TM_ASSERT(width == get_digits_count_decimal_u64(value));

    tmc_conv_result result = {0, TM_OK};
    if (width > maxlen TMC_CW(|| width <= 0)) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }

    result.size = width;
    char* p = dest + width - 1;
    while (value >= 100 TMC_CW(&& width >= 2)) {
        uint32_t index = (value % 100) * 2;
        TM_ASSERT(index < 200);
        value /= 100;
        *p = print_DoubleDigitsToCharTable[index + 1];
        *(p - 1) = print_DoubleDigitsToCharTable[index];
        p -= 2;
        TMC_CW(width -= 2);
    }

#ifdef TMC_CHECKED_WIDTH
    if ((value < 10 && width != 1) || (value >= 10 && width != 2)) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }
#endif

    /* Handle last 1-2 digits */
    if (value < 10) {
        *p = (char)('0' + (uint32_t)value);
    } else {
        TM_ASSERT(value < 100);
        uint32_t index = (uint32_t)value * 2;
        TM_ASSERT(index < 200);
        *p = print_DoubleDigitsToCharTable[index + 1];
        *(p - 1) = print_DoubleDigitsToCharTable[index];
    }

    return result;
}

TMC_DEF tmc_conv_result print_decimal_i32(char* dest, tm_size_t maxlen, int32_t value) {
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);

    tmc_conv_result result = {0, TM_OK};
    if (value < 0) {
        if (maxlen <= 0) {
            result.ec = TM_EOVERFLOW;
            return result;
        }
        TM_ASSERT(maxlen > 0);
        *dest++ = '-';
        --maxlen;
        value = -value;
        ++result.size;
    }

    uint32_t uvalue = (uint32_t)value;
    tm_size_t len = get_digits_count_decimal_u32(uvalue);
    return tmc_combine_conv_results(result, print_decimal_u32_w(dest, maxlen, len, uvalue));
}
TMC_DEF tmc_conv_result print_decimal_u32(char* dest, tm_size_t maxlen, uint32_t value) {
    return print_decimal_u32_w(dest, maxlen, get_digits_count_decimal_u32(value), value);
}
TMC_DEF tmc_conv_result print_decimal_i64(char* dest, tm_size_t maxlen, int64_t value) {
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);

    tmc_conv_result result = {0, TM_OK};
    if (value < 0) {
        if (maxlen <= 0) {
            result.ec = TM_EOVERFLOW;
            return result;
        }
        TM_ASSERT(maxlen > 0);
        *dest++ = '-';
        --maxlen;
        value = -value;
        ++result.size;
    }

    uint64_t uvalue = (uint64_t)value;
    tm_size_t len = get_digits_count_decimal_u64(uvalue);
    return tmc_combine_conv_results(result, print_decimal_u64_w(dest, maxlen, len, value));
}
TMC_DEF tmc_conv_result print_decimal_u64(char* dest, tm_size_t maxlen, uint64_t value) {
    return print_decimal_u64_w(dest, maxlen, get_digits_count_decimal_u64(value), value);
}

TMC_DEF tm_size_t get_digits_count_hex_u32(uint32_t value) {
    tm_size_t result = 0;
    do {
        value >>= 4;
        ++result;
    } while (value);
    return result;
}
TMC_DEF tm_size_t get_digits_count_hex_u64(uint64_t value) {
    tm_size_t result = 0;
    do {
        value >>= 4;
        ++result;
    } while (value);
    return result;
}

TMC_DEF tm_size_t get_digits_count_u32(uint32_t number, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    tm_size_t result = 1;

    const uint32_t ubase = (uint32_t)base;
    const uint32_t base_pow_1 = ubase;
    const uint32_t base_pow_2 = base_pow_1 * ubase;
    const uint32_t base_pow_3 = base_pow_2 * ubase;
    const uint32_t base_pow_4 = base_pow_3 * ubase;

    for (;;) {
        if (number < base_pow_4) {
            return result + (number >= base_pow_1) + (number >= base_pow_2) + (number >= base_pow_3);
        }
        number /= base_pow_4;
        result += 4;
    }
}
TMC_DEF tm_size_t get_digits_count_u64(uint64_t number, int32_t base) {
    TM_ASSERT(base >= 2 && base <= 36);

    tm_size_t result = 1;

    const uint32_t ubase = (uint32_t)base;
    const uint32_t base_pow_1 = ubase;
    const uint32_t base_pow_2 = base_pow_1 * ubase;
    const uint32_t base_pow_3 = base_pow_2 * ubase;
    const uint32_t base_pow_4 = base_pow_3 * ubase;

    for (;;) {
        if (number < base_pow_4) {
            return result + (number >= base_pow_1) + (number >= base_pow_2) + (number >= base_pow_3);
        }
        number /= base_pow_4;
        result += 4;
    }
}

TMC_DEF tmc_conv_result print_hex_u32_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value,
                                        tm_bool lowercase) {
    TM_ASSERT_VALID_SIZE(width);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);
    /* See comment on declaration for why width has to equal a specific value. */
    TM_ASSERT(width > 0);
    TM_ASSERT(width == get_digits_count_hex_u32(value));

    tmc_conv_result result = {0, TM_OK};
    if (width > maxlen TMC_CW(|| width <= 0)) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }

    if (!value) {
        TM_ASSERT(width == 1);
        *dest = '0';
        result.size = 1;
        return result;
    }

    TM_ASSERT(width > 0);
    const char* table = (lowercase) ? (print_NumberToCharTableLower) : (print_NumberToCharTableUpper);
    result.size = width;
    char* p = dest + width - 1;
    while (value >= 0x10 TMC_CW(&&width >= 2)) {
        *p = table[value & 0x0F];
        *(p - 1) = table[(value >> 4) & 0x0F];
        value >>= 8;
        p -= 2;
        TMC_CW(width -= 2);
    }

#ifdef TMC_CHECKED_WIDTH
    if ((value > 0 && value <= 0x0F && width != 1) || value > 0x0F) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }
#endif

    if (value) {
        *p = table[value & 0x0F];
    }
    return result;
}
TMC_DEF tmc_conv_result print_hex_u64_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value,
                                        tm_bool lowercase) {
    TM_ASSERT_VALID_SIZE(width);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);
    /* See comment on declaration for why width has to equal a specific value. */
    TM_ASSERT(width > 0);
    TM_ASSERT(width == get_digits_count_hex_u64(value));

    tmc_conv_result result = {0, TM_OK};
    if (width > maxlen TMC_CW(|| width <= 0)) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }

    if (!value) {
        TM_ASSERT(width == 1);
        *dest = '0';
        result.size = 1;
        return result;
    }

    TM_ASSERT(width > 0);
    const char* table = (lowercase) ? (print_NumberToCharTableLower) : (print_NumberToCharTableUpper);
    result.size = width;
    char* p = dest + width - 1;
    while (value >= 0x10 TMC_CW(&&width >= 2)) {
        *p = table[value & 0x0F];
        *(p - 1) = table[(value >> 4) & 0x0F];
        value >>= 8;
        p -= 2;
        TMC_CW(width -= 2);
    }

#ifdef TMC_CHECKED_WIDTH
    if ((value > 0 && value <= 0x0F && width != 1) || value > 0x0F) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }
#endif

    if (value) {
        *p = table[value & 0x0F];
    }
    return result;
}

TMC_DEF tmc_conv_result print_hex_i32(char* dest, tm_size_t maxlen, int32_t value, tm_bool lowercase) {
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);

    tmc_conv_result result = {0, TM_OK};
    if (value < 0) {
        if (maxlen <= 0) {
            result.ec = TM_EOVERFLOW;
            return result;
        }
        TM_ASSERT(maxlen > 0);
        *dest++ = '-';
        --maxlen;
        value = -value;
        ++result.size;
    }

    uint32_t uvalue = (uint32_t)value;
    tm_size_t len = get_digits_count_hex_u32(uvalue);
    return tmc_combine_conv_results(result, print_hex_u32_w(dest, maxlen, len, uvalue, lowercase));
}
TMC_DEF tmc_conv_result print_hex_u32(char* dest, tm_size_t maxlen, uint32_t value, tm_bool lowercase) {
    return print_hex_u32_w(dest, maxlen, get_digits_count_hex_u32(value), value, lowercase);
}
TMC_DEF tmc_conv_result print_hex_i64(char* dest, tm_size_t maxlen, int64_t value, tm_bool lowercase) {
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);

    tmc_conv_result result = {0, TM_OK};
    if (value < 0) {
        if (maxlen <= 0) {
            result.ec = TM_EOVERFLOW;
            return result;
        }
        TM_ASSERT(maxlen > 0);
        *dest++ = '-';
        --maxlen;
        value = -value;
        ++result.size;
    }

    uint64_t uvalue = (uint64_t)value;
    tm_size_t len = get_digits_count_hex_u64(uvalue);
    return tmc_combine_conv_results(result, print_hex_u64_w(dest, maxlen, len, uvalue, lowercase));
}
TMC_DEF tmc_conv_result print_hex_u64(char* dest, tm_size_t maxlen, uint64_t value, tm_bool lowercase) {
    return print_hex_u64_w(dest, maxlen, get_digits_count_hex_u64(value), value, lowercase);
}

TMC_DEF tmc_conv_result print_i32(char* dest, tm_size_t maxlen, int32_t value, int32_t base, tm_bool lowercase) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);

    tmc_conv_result result = {0, TM_OK};
    if (value < 0) {
        if (maxlen <= 0) {
            result.ec = TM_EOVERFLOW;
            return result;
        }
        TM_ASSERT(maxlen > 0);
        *dest++ = '-';
        --maxlen;
        value = -value;
        ++result.size;
    }

    return tmc_combine_conv_results(result, print_u32(dest, maxlen, (uint32_t)value, base, lowercase));
}
TMC_DEF tmc_conv_result print_u32_w(char* dest, tm_size_t maxlen, tm_size_t width, uint32_t value, int32_t base,
                                    tm_bool lowercase) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);
    /* See comment on declaration for why width has to equal a specific value. */
    TM_ASSERT(width > 0);
    TM_ASSERT(width == get_digits_count_u32(value, base));

    tmc_conv_result result = {0, TM_OK};
    if (width > maxlen TMC_CW(|| width <= 0 || (!value && width != 1))) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }

    result.size = width;
    char* p = dest + width - 1;

    /* string conversion */
    const char* table = (lowercase) ? print_NumberToCharTableLower : print_NumberToCharTableUpper;
    if (!value) {
        TM_ASSERT(width == 1);
        *p = '0';
    } else {
        TM_ASSERT(width > 0);
        do {
            TM_ASSERT(value % base < sizeof(print_NumberToCharTableLower) / sizeof(print_NumberToCharTableLower[0]));
            *p-- = table[value % base];
            value /= base;
            TMC_CW(--width);
        } while (value TMC_CW(&&width > 0));

#ifdef TMC_CHECKED_WIDTH
        if (value && width <= 0) {
            result.size = maxlen;
            result.ec = TM_EOVERFLOW;
            return result;
        }
#endif
    }
    return result;
}
TMC_DEF tmc_conv_result print_u32(char* dest, tm_size_t maxlen, uint32_t value, int32_t base, tm_bool lowercase) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);

    switch (base) {
        case 10: {
            return print_decimal_u32(dest, maxlen, value);
        }
        case 16: {
            return print_hex_u32(dest, maxlen, value, lowercase);
        }
        default: {
            return print_u32_w(dest, maxlen, get_digits_count_u32(value, base), value, base, lowercase);
        }
    }
}
TMC_DEF tmc_conv_result print_i64(char* dest, tm_size_t maxlen, int64_t value, int32_t base, tm_bool lowercase) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);

    tmc_conv_result result = {0, TM_OK};
    if (value < 0) {
        if (maxlen <= 0) {
            result.ec = TM_EOVERFLOW;
            return result;
        }
        TM_ASSERT(maxlen > 0);
        *dest++ = '-';
        --maxlen;
        value = -value;
        ++result.size;
    }

    return tmc_combine_conv_results(result, print_u64(dest, maxlen, (uint64_t)value, base, lowercase));
}
TMC_DEF tmc_conv_result print_u64_w(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value, int32_t base,
                                    tm_bool lowercase) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);
    /* See comment on declaration for why width has to equal a specific value. */
    TM_ASSERT(width > 0);
    TM_ASSERT(width == get_digits_count_u64(value, base));

    tmc_conv_result result = {0, TM_OK};
    if (width > maxlen TMC_CW(|| width <= 0 || (!value && width != 1))) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }

    result.size = width;
    char* p = dest + width - 1;

    /* string conversion */
    const char* table = (lowercase) ? print_NumberToCharTableLower : print_NumberToCharTableUpper;
    if (!value) {
        TM_ASSERT(width == 1);
        *p = '0';
    } else {
        TM_ASSERT(width > 0);
        do {
            TM_ASSERT(value % base < sizeof(print_NumberToCharTableLower) / sizeof(print_NumberToCharTableLower[0]));
            *p-- = table[value % base];
            value /= base;
            TMC_CW(--width);
        } while (value TMC_CW(&&width > 0));

#ifdef TMC_CHECKED_WIDTH
        if (value && width <= 0) {
            result.size = maxlen;
            result.ec = TM_EOVERFLOW;
            return result;
        }
#endif
    }
    return result;
}
TMC_DEF tmc_conv_result print_u64(char* dest, tm_size_t maxlen, uint64_t value, int32_t base, tm_bool lowercase) {
    TM_ASSERT(base >= 2 && base <= 36);
    TM_ASSERT_VALID_SIZE(maxlen);
    TM_ASSERT(dest || maxlen <= 0);

    switch (base) {
        case 10: {
            return print_decimal_u64(dest, maxlen, value);
        }
        case 16: {
            return print_hex_u64(dest, maxlen, value, lowercase);
        }
        default: {
            return print_u64_w(dest, maxlen, get_digits_count_u64(value, base), value, base, lowercase);
        }
    }
}

static tmc_conv_result tmc_print_zero_padded_u64(char* dest, tm_size_t maxlen, tm_size_t width, uint64_t value) {
    tm_size_t count = get_digits_count_decimal_u64(value);
    tmc_conv_result result = {0, TM_OK};
    if (width > maxlen) {
        result.size = maxlen;
        result.ec = TM_EOVERFLOW;
        return result;
    }
    if (count < width) {
        tm_size_t padding = width - count;
        TM_MEMSET(dest, '0', padding);
        dest += padding;
        maxlen -= padding;
        result.size = padding;
    }
    return tmc_combine_conv_results(result, print_decimal_u64_w(dest, maxlen, count, value));
}
TMC_DEF tmc_conv_result print_double(char* dest, tm_size_t maxlen, double value, uint32_t flags, int32_t precision) {
    TM_ASSERT(dest);
    TM_ASSERT_VALID_SIZE(maxlen);
    if (maxlen <= 0) {
        tmc_conv_result result = {0, TM_EOVERFLOW};
        return result;
    }

    tm_size_t start = maxlen;
    if (((flags & PF_SIGNBIT) && TM_SIGNBIT(value)) || (value < 0)) {
        *dest++ = '-';
        --maxlen;
        value = -value;
        if (maxlen <= 0) {
            tmc_conv_result result = {start - maxlen, TM_EOVERFLOW};
            return result;
        }
    }

    if (TM_ISNAN(value)) {
        tmc_conv_result result = {start - maxlen, TM_OK};
        if (flags & PF_LOWERCASE) {
            return tmc_combine_conv_results(result, tmc_print_string(dest, maxlen, "nan", 3));
        } else {
            return tmc_combine_conv_results(result, tmc_print_string(dest, maxlen, "NAN", 3));
        }
    }
    if (TM_ISINF(value)) {
        tmc_conv_result result = {start - maxlen, TM_OK};
        if (flags & PF_LOWERCASE) {
            return tmc_combine_conv_results(result, tmc_print_string(dest, maxlen, "inf", 3));
        } else {
            return tmc_combine_conv_results(result, tmc_print_string(dest, maxlen, "INF", 3));
        }
    }

    tm_bool keep_short = precision < 0;
    TMC_DEBUG(printf("keep_short is: %d\n", keep_short));
    if (keep_short) {
        precision = TMC_MAX_PRECISION;
    } else {
        flags |= PF_TRAILING_ZEROES;
        if (precision > TMC_MAX_PRECISION) {
            precision = TMC_MAX_PRECISION;
        }
    }

    /* quick and dirty method to convert double value to decimal string
       produces wrong results for very big numbers > around 1e18 */

    /* check for rollover due to precision */
    double rollover = value + str_Pow10(0.5, -precision);
    if ((uint64_t)rollover > (uint64_t)value) {
        value += 1;
    }

    int32_t magnitude = print_magnitude(value);
    if (magnitude >= scan_MaxExponent) {
#ifdef TMC_CLAMP_ON_RANGE_ERROR
        magnitude = scan_MaxExponent - 1;
#else
        return {0, TM_EOVERFLOW};
#endif
    }
    tm_size_t printWidth = 0;
    while (magnitude > 0 && maxlen > 0) {
        double part;
        if (magnitude > TMC_MAX_PRECISION) {
            part = str_Pow10(value, -(magnitude - TMC_MAX_PRECISION));
            value -= str_Pow10((double)((uint64_t)part), magnitude - TMC_MAX_PRECISION);
        } else {
            part = value;
            value -= (double)((uint64_t)value);
            printWidth = magnitude;
        }
        uint64_t digits = (uint64_t)part;
        tmc_conv_result interm = tmc_print_zero_padded_u64(dest, maxlen, printWidth, digits);
        printWidth = TMC_MAX_PRECISION;
        magnitude -= TMC_MAX_PRECISION;
        maxlen -= interm.size;
        dest += interm.size;
        if (interm.ec != TM_OK) {
            tmc_conv_result result = {start - maxlen, interm.ec};
            return result;
        }
    }
    if (magnitude > 0 && maxlen <= 0) {
        tmc_conv_result result = {start - maxlen, TM_EOVERFLOW};
        return result;
    }
    double fractionalPart = value;

    tm_errc ec = TM_OK;
    if (precision > 0) {
        printWidth = precision;
        /* fractionalPart is positive, so we can round by adding 0.5 before truncating
           this might produce wrong rounding, but matches MSVC's printf rounding */
        uint64_t fractionalDigits = (uint64_t)((fractionalPart * print_PowersOfTen[precision]) + 0.5);
        if (fractionalDigits) {
            if (maxlen <= 0) {
                tmc_conv_result result = {start - maxlen, TM_EOVERFLOW};
                return result;
            }
            *dest++ = '.';
            --maxlen;
            if (maxlen <= 0) {
                tmc_conv_result result = {start - maxlen, TM_EOVERFLOW};
                return result;
            }
            if (!(flags & PF_TRAILING_ZEROES)) {
                /* get rid of trailing zeroes */
                for (;;) {
                    uint64_t digit = fractionalDigits % 10;
                    if (!digit) {
                        fractionalDigits /= 10;
                        --printWidth;
                    } else {
                        break;
                    }
                }
            }
            /* check whether we got rid of all digits when trimming trailing zeroes */
            if (printWidth <= 0) {
                /* output a zero manually in that case */
                TM_ASSERT(maxlen > 0);
                *dest++ = '0';
                --maxlen;
            } else {
                tmc_conv_result fractionalResult =
                    tmc_print_zero_padded_u64(dest, maxlen, printWidth, fractionalDigits);
                maxlen -= fractionalResult.size;
                dest += fractionalResult.size;
                if (fractionalResult.ec != TM_OK) {
                    tmc_conv_result result = {start - maxlen, fractionalResult.ec};
                    return result;
                }
            }
        } else if (flags & PF_TRAILING_ZEROES) {
            if (maxlen <= 0) {
                tmc_conv_result result = {start - maxlen, TM_EOVERFLOW};
                return result;
            }
            *dest++ = '.';
            --maxlen;
            if (maxlen <= 0) {
                tmc_conv_result result = {start - maxlen, TM_EOVERFLOW};
                return result;
            }

            if (keep_short) {
                *dest++ = '0';
                --maxlen;
            } else {
                if (maxlen < (tm_size_t)precision) {
                    precision = (int32_t)maxlen;
                    ec = TM_EOVERFLOW;
                }

                TM_MEMSET(dest, '0', precision);
                maxlen -= precision;
            }
        }
    }

    tmc_conv_result result = {start - maxlen, ec};
    return result;
}
tmc_conv_result print_float(char* dest, tm_size_t maxlen, float value, uint32_t flags, int32_t precision) {
    return print_double(dest, maxlen, (double)value, flags, precision);
}

#ifdef __cplusplus
}
#endif

#endif /* TM_CONVERSION_IMPLEMENTATION */

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
