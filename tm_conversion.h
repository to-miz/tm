/*
tm_conversion.h v0.9.3 - public domain
author: Tolga Mizrak 2016

no warranty; use at your own risk

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
		  There is no standard way to do that, but a lot of non standard functions with different
		  api's like itoa.
		- Have default values for conversions in case a conversion from string to value wasn't
		  successful. This is possible because the scan functions in this library only write the
		  result into the out parameter on result.
		  In C this is not possible with the strto_ family of functions without inspecting errno or
		  inspecting the string before passing it into the strto_ functions.
		  See examples for how to use default values with this library.

SWITCHES
	There are a couple of #define switches that enable some additional definitions to be included
	or do something extra.
	Those are:
	    TMC_OWN_TYPES:
	        define this if you want to change the typedefs used by this library
		TMC_CONVENIENCE:
			define this if you want to include some convenience functions that make it easier to
			use in usage code.
		TMC_CPP_OVERLOADS:
			define this if you want cpp overloads for the conversion functions
			that don't have the _n suffix
		TMC_CPP_TEMPLATED:
			define this if you want to include templated versions of the conversion functions
		TMC_STRING_VIEW:
			define this if you have some sort of string_view class and want overloads of the
			conversion functions for it.
			Needs TMC_CPP_OVERLOADS, TMC_STRING_VIEW_DATA( x ) and TMC_STRING_VIEW_SIZE( x ) to be
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
				#define TMC_STRING_VIEW string_view
				#define TMC_STRING_VIEW_DATA( str ) ( str ).data()
				#define TMC_STRING_VIEW_SIZE( str ) ( str ).size()

ISSUES
	- PF_SCIENTIFIC does nothing at the moment
	- print_double, print_float don't produce correct decimal strings for values above/below +-1e19.
	This is because we use floating arithmetic and we lose too much precision when dealing with
	very big numbers. This could be fixed by using arbitrary precision math, but that pretty much
	is beyond the scope of this library. Do not use these functions for big numbers.

HISTORY
	v0.9.3  30.08.16 added TMC_CPP_OVERLOADS and cpp overloads of functions
	                 added TMC_STRING_VIEW for string view support of functions
	                 added TMC_CONVENIENCE and convenience functions
	                 added TMC_CPP_TEMPLATED
	                 fixed compile error when using print_strnrev
	                 added PURPOSE, WHY, SWITCHES, EXAMPLES
	v0.9.2	07.08.16 fixed a bug in print_double not being able to print 10 (magnitude calculation
	                 was wrong)
	                 fixed a bug in print_double rounding wrong for 0.99 and precision 1
	v0.9.1  10.07.16 strncasecmp to strnicmp & print_Reverse to print_strnrev
	v0.9a   01.07.16 improved C99 conformity
	v0.9    23.06.16 initial commit

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.
*/

/*
EXAMPLES
Note that examples use <cstdio> and printf just to demonstrate that the outputs are correct
*/

#if 0
// different ways to convert a string to int

// main.cpp
#define TM_CONVERSION_IMPLEMENTATION
#define TMC_CONVENIENCE
#define TMC_CPP_OVERLOADS
#define TMC_CPP_TEMPLATED
#include <tm_conversion.h>

#include <cstdio>
#include <cassert>

int main()
{
	// initialize value to default
	int value = -1;
	// scan will not write to the out parameter, so value will be -1 on error in this case
	scan_i32( "error", 10, &value );
	assert( value == -1 );

	// this version requires TMC_CPP_OVERLOADS
	scan( "2016", 10, &value );

	// this version requires TMC_CONVENIENCE and TMC_CPP_OVERLOADS, it is the same as the above
	value = to_i32( "2016", Radix{10}, /*default value=*/-1 );

	// convert_to requires TMC_CPP_TEMPLATED
	value = convert_to( "2016", -1 ); // convert_to knows to convert to int because of the
	                                  // default value (-1) that we supplied

	// supplying the template parameter makes it more readable
	value = convert_to< int >( "2016" ); // in this case we need to supply the return type as a
	                                     // template parameter, because we do not supply a default
	                                     // value. It will return 0 on error.

	printf( "%d", value );
	return 0;
}

// OUTPUT: 2016
#endif

#if 0
// example of scanning multiple values in a row and how to advance to the next number using scan

// main.cpp
#define TM_CONVERSION_IMPLEMENTATION
#include <tm_conversion.h>

#include <cstdio>

int main()
{
	const char* str = "1234 5678 0xFF not_a_number";
	int base = 0;  // scan will attempt to determine base depending on input if base is 0
	for( ;; ) {
		// set a default value
		int value = 0;
		size_t consumed = scan_i32( str, base, &value );
		str += consumed; // advance str by number of bytes consumed by scan
		printf( "%d\n", value );
		if( consumed == 0 ) {
			// scan will not consume any bytes if input isn't a number
			break;
		}
		if( *str ) {
			++str; // skip space
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
// C++ style operator<< overloads

// main.cpp
#define TM_CONVERSION_IMPLEMENTATION
#define TMC_CONVENIENCE
#define TMC_CPP_OVERLOADS
#include <tm_conversion.h>

#include <cstdio>
#include <cstring>

struct string_builder {
	char* ptr;
	size_t sz;
	size_t cap;
	PrintFormat format;

	char* data() { return ptr; }
	size_t size() { return sz; }
	char* end() { return ptr + sz; }
	size_t remaining() { return cap - sz; }

	string_builder( char* ptr, size_t cap )
	: ptr( ptr ), sz( 0 ), cap( cap ), format( defaultPrintFormat() )
	{
	}
	template < class T >
	string_builder& operator<<( T value )
	{
		sz += print( end(), remaining(), &format, value );
		return *this;
	}
	string_builder& operator<<( const char* str )
	{
		auto len = strlen( str );
		len      = ( len < remaining() ) ? ( len ) : ( remaining() );
		memcpy( end(), str, len );
		sz += len;
		return *this;
	}
};

int main()
{
	const size_t bufferSize = 10000;
	char buffer[bufferSize];
	string_builder builder = {buffer, bufferSize};
	builder << "Hello World! " << 10 << " " << 3.1;

	printf( "%.*s", (int)builder.size(), builder.data() );
	return 0;
}

// OUTPUT: Hello World! 10 3.1
#endif

#if 0
// demonstrates how to use a string_view class with the conversion functions

// main.cpp
#include <cstring> // strlen

// extremely simplified string_view class just for demonstration
// it represents non nullterminated views into strings
struct string_view {
	const char* ptr = nullptr;
	size_t sz = 0;

	string_view() = default;
	string_view( const char* str ) : ptr( str ), sz( ( str ) ? ( strlen( str ) ) : ( 0 ) ) {}
	string_view( const char* str, size_t len ) : ptr( str ), sz( len ) {}
	const char* data() { return ptr; }
	size_t size() { return sz; }
	string_view substr( size_t pos, size_t len = (size_t)-1 )
	{
		if( pos > sz ) {
			pos = sz;
		}
		if( len > sz - pos ) {
			len = sz - pos;
		}
		return string_view( ptr + pos, len );
	}
};

#define TM_CONVERSION_IMPLEMENTATION
#define TMC_CPP_OVERLOADS
#define TMC_CPP_TEMPLATED
#define TMC_STRING_VIEW string_view
#define TMC_STRING_VIEW_DATA( str ) ( str ).data()
#define TMC_STRING_VIEW_SIZE( str ) ( str ).size()
#include <tm_conversion.h>

#include <cstdio>

int main()
{
	string_view str = "12345678";
	string_view sub0 = str.substr( 2, 4 ); // sub0 is "3456"
	int value = convert_to< int >( sub0 );

	printf( "%d\n", value ); 
}
/*
OUTPUT:
3456

*/
#endif

#ifdef TM_CONVERSION_IMPLEMENTATION
	// define these to avoid crt

	#ifndef TMC_ASSERT
		#include <assert.h>
		#define TMC_ASSERT assert
	#endif

	#ifndef TMC_ISDIGIT
		#include <ctype.h>
		#define TMC_ISDIGIT isdigit
	#endif

	#ifndef TMC_ISUPPER
		#include <ctype.h>
		#define TMC_ISUPPER isupper
	#endif

	#ifndef TMC_ISLOWER
		#include <ctype.h>
		#define TMC_ISLOWER islower
	#endif

	#ifndef TMC_MEMCPY
		#include <string.h>
		#define TMC_MEMCPY memcpy
	#endif

	#ifndef TMC_MEMSET
		#include <string.h>
		#define TMC_MEMSET memset
	#endif

	#ifndef TMC_SIGNBIT
		#include <math.h>
		#define TMC_SIGNBIT signbit
	#endif

	#ifndef TMC_ISNAN
		#include <math.h>
		#define TMC_ISNAN isnan
	#endif

	#ifndef TMC_ISINF
		#include <math.h>
		#define TMC_ISINF isinf
	#endif

	#ifndef TMC_STRNICMP
		#define TMC_IMPLEMENT_STRNICMP
		#define TMC_STRNICMP scan_strnicmp
	#endif

	#ifndef TMC_STRNREV
		#define TMC_IMPLEMENT_STRNREV
		#define TMC_STRNREV print_strnrev
	#endif
#endif

#ifndef _TM_CONVERSION_H_INCLUDED_
#define _TM_CONVERSION_H_INCLUDED_

// define TMC_OWN_TYPES to override these typedefs and avoid stdint.h
#ifndef TMC_OWN_TYPES
	#include <stdint.h>
	typedef size_t tmc_size_t;
	typedef uint8_t tmc_uint8;
	typedef int32_t tmc_int32;
	typedef uint32_t tmc_uint32;
	typedef int64_t tmc_int64;
	typedef uint64_t tmc_uint64;
	#ifdef __cplusplus
		typedef bool tmc_bool;
		#define TMC_TRUE true
		#define TMC_FALSE false
	#else
		typedef int tmc_bool;
		#define TMC_TRUE 1
		#define TMC_FALSE 0
	#endif

	#define TMC_INT32_MAX INT32_MAX
	#define TMC_UINT32_MAX UINT32_MAX
	#define TMC_INT64_MAX INT64_MAX
	#define TMC_UINT64_MAX UINT64_MAX

	#define TMC_INT32_MIN INT32_MIN
	#define TMC_UINT32_MIN UINT32_MIN
	#define TMC_INT64_MIN INT64_MIN
	#define TMC_UINT64_MIN UINT64_MIN
#endif

#ifndef TMC_STATIC
	#define TMC_DEF extern
#else
	#define TMC_DEF static
#endif

#ifdef __cplusplus
	#define TMC_UNDERLYING_U32 : tmc_uint32
#else
	#define TMC_UNDERLYING_U32
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
These functions scan a value from a string and store it into out on success.
Params:
	nullterminated: a nullterminated input string
	base: the base to use when scanning, pass in 0 to guess base based on string
	out: output value, will not be modified if an error occurs
Return: number of bytes consumed, 0 if an error occurs

The _n variants take a string with a known length (they do not have to be nullterminated), useful
when scanning in substrings.
*/
TMC_DEF tmc_size_t scan_i32( const char* nullterminated, int base, tmc_int32* out );
TMC_DEF tmc_size_t scan_i32_n( const char* str, tmc_size_t len, int base, tmc_int32* out );
TMC_DEF tmc_size_t scan_u32( const char* nullterminated, int base, tmc_uint32* out );
TMC_DEF tmc_size_t scan_u32_n( const char* str, tmc_size_t len, int base, tmc_uint32* out );
TMC_DEF tmc_size_t scan_i64( const char* nullterminated, int base, tmc_int64* out );
TMC_DEF tmc_size_t scan_i64_n( const char* str, tmc_size_t len, int base, tmc_int64* out );
TMC_DEF tmc_size_t scan_u64( const char* nullterminated, int base, tmc_uint64* out );
TMC_DEF tmc_size_t scan_u64_n( const char* str, tmc_size_t len, int base, tmc_uint64* out );
TMC_DEF tmc_size_t scan_float( const char* nullterminated, float* out );
TMC_DEF tmc_size_t scan_float_n( const char* str, tmc_size_t len, float* out );
TMC_DEF tmc_size_t scan_double( const char* nullterminated, double* out );
TMC_DEF tmc_size_t scan_double_n( const char* str, tmc_size_t len, double* out );
TMC_DEF tmc_size_t scan_bool( const char* nullterminated, tmc_bool* out );
TMC_DEF tmc_size_t scan_bool_n( const char* str, tmc_size_t len, tmc_bool* out );

enum PrintFormatFlags TMC_UNDERLYING_U32 {
	PF_DEFAULT = 0,
	PF_SIGN = ( 1 << 0 ),
	PF_LOWERCASE = ( 1 << 1 ),
	PF_PADDING_ZEROES = ( 1 << 2 ),
	PF_BOOL_AS_NUMBER = ( 1 << 3 ),
	PF_SCIENTIFIC = ( 1 << 4 ),  // not implemented yet
};

typedef struct {
	int base;
	int precision;
	int width;
	unsigned int flags;
} PrintFormat;

/* returns a PrintFormat struct initialized with default values:
  base = 10, precision = 6, width = 0, flags = PF_DEFAULT
*/
PrintFormat defaultPrintFormat();

/*
These functions print a value into a destination buffer.
Params:
	dest: destination buffer
	maxlen: max length of buffer
	format: optional, pointer to PrintFormat to specify how the value should be printed
	value: value to be printed
Return: returns number of bytes printed
*/
TMC_DEF tmc_size_t print_i32( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_int32 value );
TMC_DEF tmc_size_t print_u32( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_uint32 value );
TMC_DEF tmc_size_t print_i64( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_int64 value );
TMC_DEF tmc_size_t print_u64( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_uint64 value );
TMC_DEF tmc_size_t print_double( char* dest, tmc_size_t maxlen, PrintFormat* format, double value );
TMC_DEF tmc_size_t print_float( char* dest, tmc_size_t maxlen, PrintFormat* format, float value );
TMC_DEF tmc_size_t print_bool( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_bool value );

#ifdef TMC_CONVENIENCE
	// we use a strong typedef for radix, so we don't accidentaly specify radix when wanting to
	// specify a default value (only an issue in C++ mode, but this way the signatures are the same
	// in both C/C++)
	struct Radix {
		int base;
	};

	/*
	Convert a string to a value directly. Pass in a def value that gets returned if an error occurs.
	Params:
		nullterminated: a nullterminated input string
		base: the base to use when scanning, pass in Radix{0} to guess base based on string
		def: default value to return if an error occurs
	Return: scanned value or def on error
	*/
	tmc_int32 to_i32( const char* nullterminated, Radix base, tmc_int32 def );
	tmc_int32 to_i32_n( const char* str, tmc_size_t len, Radix base, tmc_int32 def );
	tmc_uint32 to_u32( const char* nullterminated, Radix base, tmc_uint32 def );
	tmc_uint32 to_u32_n( const char* str, tmc_size_t len, Radix base, tmc_uint32 def );
	tmc_int64 to_i64( const char* nullterminated, Radix base, tmc_int64 def );
	tmc_int64 to_i64_n( const char* str, tmc_size_t len, Radix base, tmc_int64 def );
	tmc_uint64 to_u64( const char* nullterminated, Radix base, tmc_uint64 def );
	tmc_uint64 to_u64_n( const char* str, tmc_size_t len, Radix base, tmc_uint64 def );
	float to_float( const char* nullterminated, float def );
	float to_float_n( const char* str, tmc_size_t len, float def );
	double to_double( const char* nullterminated, double def );
	double to_double_n( const char* str, tmc_size_t len, double def );
	tmc_bool to_bool( const char* nullterminated, tmc_bool def );
	tmc_bool to_bool_n( const char* str, tmc_size_t len, tmc_bool def );
#endif // TMC_CONVENIENCE

#ifdef __cplusplus
}
#endif

// #define TMC_CPP_OVERLOADS if you want cpp overloads, see C versions further below for description
// these are defined as inline further below
#if defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )
	/*
	Overloads of scan, print functions. See C versions for documentation/usage
	*/
	tmc_size_t scan( const char* nullterminated, int base, tmc_int32* out );
	tmc_size_t scan( const char* nullterminated, int base, tmc_uint32* out );
	tmc_size_t scan( const char* nullterminated, int base, tmc_int64* out );
	tmc_size_t scan( const char* nullterminated, int base, tmc_uint64* out );
	tmc_size_t scan( const char* nullterminated, float* out );
	tmc_size_t scan( const char* nullterminated, double* out );
	tmc_size_t scan( const char* nullterminated, bool* out );

	tmc_size_t scan( const char* str, tmc_size_t len, int base, tmc_int32* out );
	tmc_size_t scan( const char* str, tmc_size_t len, int base, tmc_uint32* out );
	tmc_size_t scan( const char* str, tmc_size_t len, int base, tmc_int64* out );
	tmc_size_t scan( const char* str, tmc_size_t len, int base, tmc_uint64* out );
	tmc_size_t scan( const char* str, tmc_size_t len, float* out );
	tmc_size_t scan( const char* str, tmc_size_t len, double* out );
	tmc_size_t scan( const char* str, tmc_size_t len, bool* out );

	tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_int32 value );
	tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_uint32 value );
	tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_int64 value );
	tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_uint64 value );
	tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, double value );
	tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, float value );
	tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_bool value );

	tmc_size_t scan_i32( const char* str, tmc_size_t len, int base, tmc_int32* out );
	tmc_size_t scan_u32( const char* str, tmc_size_t len, int base, tmc_uint32* out );
	tmc_size_t scan_i64( const char* str, tmc_size_t len, int base, tmc_int64* out );
	tmc_size_t scan_u64( const char* str, tmc_size_t len, int base, tmc_uint64* out );
	tmc_size_t scan_float( const char* str, tmc_size_t len, float* out );
	tmc_size_t scan_double( const char* str, tmc_size_t len, double* out );
	tmc_size_t scan_bool( const char* str, tmc_size_t len, bool* out );
	// overloads for some sort of string_view class
	#ifdef TMC_STRING_VIEW
		tmc_size_t scan_i32( TMC_STRING_VIEW str, int base, tmc_int32* out );
		tmc_size_t scan_u32( TMC_STRING_VIEW str, int base, tmc_uint32* out );
		tmc_size_t scan_i64( TMC_STRING_VIEW str, int base, tmc_int64* out );
		tmc_size_t scan_u64( TMC_STRING_VIEW str, int base, tmc_uint64* out );
		tmc_size_t scan_float( TMC_STRING_VIEW str, float* out );
		tmc_size_t scan_double( TMC_STRING_VIEW str, double* out );
		tmc_size_t scan_bool( TMC_STRING_VIEW str, bool* out );
	#endif // TMC_STRING_VIEW
#endif // defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )

#if defined( TMC_CONVENIENCE ) && defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )
	/*
	Overloads of to_ functions. See C versions for documentation/usage
	*/
	// overloads with default values (Radix={10} and def={})
	tmc_int32 to_i32( const char* nullterminated, tmc_int32 def = {} );
	tmc_int32 to_i32( const char* str, tmc_size_t len, tmc_int32 def = {} );
	tmc_uint32 to_u32( const char* nullterminated, tmc_uint32 def = {} );
	tmc_uint32 to_u32( const char* str, tmc_size_t len, tmc_uint32 def = {} );
	tmc_int64 to_i64( const char* nullterminated, tmc_int64 def = {} );
	tmc_int64 to_i64( const char* str, tmc_size_t len, tmc_int64 def = {} );
	tmc_uint64 to_u64( const char* nullterminated, tmc_uint64 def = {} );
	tmc_uint64 to_u64( const char* str, tmc_size_t len, tmc_uint64 def = {} );
	float to_float( const char* nullterminated );
	float to_float( const char* str, tmc_size_t len, float def = {} );
	double to_double( const char* nullterminated );
	double to_double( const char* str, tmc_size_t len, double def = {} );
	tmc_bool to_bool( const char* nullterminated );
	tmc_bool to_bool( const char* str, tmc_size_t len, tmc_bool def = {} );

	#ifdef TMC_STRING_VIEW
		tmc_int32 to_i32( TMC_STRING_VIEW str, Radix base, tmc_int32 def = {} );
		tmc_uint32 to_u32( TMC_STRING_VIEW str, Radix base, tmc_uint32 def = {} );
		tmc_int64 to_i64( TMC_STRING_VIEW str, Radix base, tmc_int64 def = {} );
		tmc_uint64 to_u64( TMC_STRING_VIEW str, Radix base, tmc_uint64 def = {} );
		tmc_int32 to_i32( TMC_STRING_VIEW str, tmc_int32 def = {} );
		tmc_uint32 to_u32( TMC_STRING_VIEW str, tmc_uint32 def = {} );
		tmc_int64 to_i64( TMC_STRING_VIEW str, tmc_int64 def = {} );
		tmc_uint64 to_u64( TMC_STRING_VIEW str, tmc_uint64 def = {} );
		float to_float( TMC_STRING_VIEW str, float def = {} );
		double to_double( TMC_STRING_VIEW str, double def = {} );
		tmc_bool to_bool( TMC_STRING_VIEW str, tmc_bool def = {} );
	#endif // TMC_STRING_VIEW
	
	/*
	to_string functions
	Turn value into string and store into dest.
	Returns number of bytes written.
	Similar to print, but does not require a PrintFormat structure to be passed in.
	*/
	tmc_size_t to_string( tmc_int32 value, char* dest, tmc_size_t maxlen, Radix base = Radix{10} );
	tmc_size_t to_string( tmc_uint32 value, char* dest, tmc_size_t maxlen, Radix base = Radix{10} );
	tmc_size_t to_string( tmc_int64 value, char* dest, tmc_size_t maxlen, Radix base = Radix{10} );
	tmc_size_t to_string( tmc_uint64 value, char* dest, tmc_size_t maxlen, Radix base = Radix{10} );
	tmc_size_t to_string( double value, char* dest, tmc_size_t maxlen, int precision = 6 );
	tmc_size_t to_string( float value, char* dest, tmc_size_t maxlen, int precision = 6 );
	tmc_size_t to_string( tmc_bool value, char* dest, tmc_size_t maxlen, tmc_bool asNumber = false );
	#if defined TMC_STRING_VIEW && !defined( TMC_STRING_VIEW_NO_CONSTRUCTOR )
		TMC_STRING_VIEW to_string( tmc_bool value );
	#endif // TMC_STRING_VIEW

	// output hex number and prepend 0x
	tmc_size_t to_string_hex( tmc_uint32 value, char* dest, tmc_size_t maxlen );
	tmc_size_t to_string_hex( tmc_uint64 value, char* dest, tmc_size_t maxlen );
#endif // defined( TMC_CONVENIENCE ) && defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )

#if defined( TMC_CPP_TEMPLATED ) && defined( __cplusplus )
	/*
	Similar to to_i32 etc, but take return type by template argument.
	Enables you to write conversions like this:
	auto value = convert_to< int >( str );
	*/
	template< class T > T convert_to( const char* str, T def = {} );
	
	#ifdef TMC_STRING_VIEW
		template< class T > T convert_to( TMC_STRING_VIEW str, T def = {} );
	#endif // TMC_STRING_VIEW
#endif // defined( TMC_CPP_TEMPLATED ) && defined( __cplusplus )


// inline implementations
inline PrintFormat defaultPrintFormat()
{
	return PrintFormat{10, 6, 0, PF_DEFAULT};
}

#if defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )
	inline tmc_size_t scan( const char* nullterminated, int base, tmc_int32* out )
	{
		return scan_i32( nullterminated, base, out );
	}
	inline tmc_size_t scan( const char* nullterminated, int base, tmc_uint32* out )
	{
		return scan_u32( nullterminated, base, out );
	}
	inline tmc_size_t scan( const char* nullterminated, int base, tmc_int64* out )
	{
		return scan_i64( nullterminated, base, out );
	}
	inline tmc_size_t scan( const char* nullterminated, int base, tmc_uint64* out )
	{
		return scan_u64( nullterminated, base, out );
	}
	inline tmc_size_t scan( const char* nullterminated, float* out )
	{
		return scan_float( nullterminated, out );
	}
	inline tmc_size_t scan( const char* nullterminated, double* out )
	{
		return scan_double( nullterminated, out );
	}
	inline tmc_size_t scan( const char* nullterminated, bool* out )
	{
		return scan_bool( nullterminated, out );
	}

	inline tmc_size_t scan( const char* str, tmc_size_t len, int base, tmc_int32* out )
	{
		return scan_i32_n( str, len, base, out );
	}
	inline tmc_size_t scan( const char* str, tmc_size_t len, int base, tmc_uint32* out )
	{
		return scan_u32_n( str, len, base, out );
	}
	inline tmc_size_t scan( const char* str, tmc_size_t len, int base, tmc_int64* out )
	{
		return scan_i64_n( str, len, base, out );
	}
	inline tmc_size_t scan( const char* str, tmc_size_t len, int base, tmc_uint64* out )
	{
		return scan_u64_n( str, len, base, out );
	}
	inline tmc_size_t scan( const char* str, tmc_size_t len, float* out )
	{
		return scan_float_n( str, len, out );
	}
	inline tmc_size_t scan( const char* str, tmc_size_t len, double* out )
	{
		return scan_double_n( str, len, out );
	}
	inline tmc_size_t scan( const char* str, tmc_size_t len, bool* out )
	{
		return scan_bool_n( str, len, out );
	}

	inline tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_int32 value )
	{
		return print_i32( dest, maxlen, format, value );
	}
	inline tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_uint32 value )
	{
		return print_u32( dest, maxlen, format, value );
	}
	inline tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_int64 value )
	{
		return print_i64( dest, maxlen, format, value );
	}
	inline tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_uint64 value )
	{
		return print_u64( dest, maxlen, format, value );
	}
	inline tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, double value )
	{
		return print_double( dest, maxlen, format, value );
	}
	inline tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, float value )
	{
		return print_float( dest, maxlen, format, value );
	}
	inline tmc_size_t print( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_bool value )
	{
		return print_bool( dest, maxlen, format, value );
	}

	inline tmc_size_t scan_i32( const char* str, tmc_size_t len, int base, tmc_int32* out )
	{
		return scan_i32_n( str, len, base, out );
	}
	inline tmc_size_t scan_u32( const char* str, tmc_size_t len, int base, tmc_uint32* out )
	{
		return scan_u32_n( str, len, base, out );
	}
	inline tmc_size_t scan_i64( const char* str, tmc_size_t len, int base, tmc_int64* out )
	{
		return scan_i64_n( str, len, base, out );
	}
	inline tmc_size_t scan_u64( const char* str, tmc_size_t len, int base, tmc_uint64* out )
	{
		return scan_u64_n( str, len, base, out );
	}
	inline tmc_size_t scan_float( const char* str, tmc_size_t len, float* out )
	{
		return scan_float_n( str, len, out );
	}
	inline tmc_size_t scan_double( const char* str, tmc_size_t len, double* out )
	{
		return scan_double_n( str, len, out );
	}
	inline tmc_size_t scan_bool( const char* str, tmc_size_t len, bool* out )
	{
		return scan_bool_n( str, len, out );
	}
#ifdef TMC_STRING_VIEW
    inline tmc_size_t scan_i32( TMC_STRING_VIEW str, int base, tmc_int32* out )
    {
	    return scan_i32_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), base, out );
    }
    inline tmc_size_t scan_u32( TMC_STRING_VIEW str, int base, tmc_uint32* out )
    {
	    return scan_u32_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), base, out );
    }
    inline tmc_size_t scan_i64( TMC_STRING_VIEW str, int base, tmc_int64* out )
    {
	    return scan_i64_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), base, out );
    }
    inline tmc_size_t scan_u64( TMC_STRING_VIEW str, int base, tmc_uint64* out )
    {
	    return scan_u64_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), base, out );
    }
    inline tmc_size_t scan_float( TMC_STRING_VIEW str, float* out )
    {
	    return scan_float_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), out );
    }
    inline tmc_size_t scan_double( TMC_STRING_VIEW str, double* out )
    {
	    return scan_double_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), out );
    }
    inline tmc_size_t scan_bool( TMC_STRING_VIEW str, bool* out )
    {
	    return scan_bool_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), out );
    }
#endif  // TMC_STRING_VIEW
#endif // defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TMC_CONVENIENCE
	inline tmc_int32 to_i32( const char* nullterminated, Radix base, tmc_int32 def )
	{
		tmc_int32 result = def;
		scan_i32( nullterminated, base.base, &result );
		return result;
	}
	inline tmc_int32 to_i32_n( const char* str, tmc_size_t len, Radix base, tmc_int32 def )
	{
		tmc_int32 result = def;
		scan_i32_n( str, len, base.base, &result );
		return result;
	}
	tmc_uint32 to_u32( const char* nullterminated, Radix base, tmc_uint32 def )
	{
		tmc_uint32 result = def;
		scan_u32( nullterminated, base.base, &result );
		return result;
	}
	inline tmc_uint32 to_u32_n( const char* str, tmc_size_t len, Radix base, tmc_uint32 def )
	{
		tmc_uint32 result = def;
		scan_u32_n( str, len, base.base, &result );
		return result;
	}
	inline tmc_int64 to_i64( const char* nullterminated, Radix base, tmc_int64 def )
	{
		tmc_int64 result = def;
		scan_i64( nullterminated, base.base, &result );
		return result;
	}
	inline tmc_int64 to_i64_n( const char* str, tmc_size_t len, Radix base, tmc_int64 def )
	{
		tmc_int64 result = def;
		scan_i64_n( str, len, base.base, &result );
		return result;
	}
	inline tmc_uint64 to_u64( const char* nullterminated, Radix base, tmc_uint64 def )
	{
		tmc_uint64 result = def;
		scan_u64( nullterminated, base.base, &result );
		return result;
	}
	inline tmc_uint64 to_u64_n( const char* str, tmc_size_t len, Radix base, tmc_uint64 def )
	{
		tmc_uint64 result = def;
		scan_u64_n( str, len, base.base, &result );
		return result;
	}
	inline float to_float( const char* nullterminated, float def )
	{
		float result = def;
		scan_float( nullterminated, &result );
		return result;
	}
	inline float to_float_n( const char* str, tmc_size_t len, float def )
	{
		float result = def;
		scan_float_n( str, len, &result );
		return result;
	}
	inline double to_double( const char* nullterminated, double def )
	{
		double result = def;
		scan_double( nullterminated, &result );
		return result;
	}
	inline double to_double_n( const char* str, tmc_size_t len, double def )
	{
		double result = def;
		scan_double_n( str, len, &result );
		return result;
	}
	inline tmc_bool to_bool( const char* nullterminated, tmc_bool def )
	{
		tmc_bool result = def;
		scan_bool( nullterminated, &result );
		return result;
	}
	inline tmc_bool to_bool_n( const char* str, tmc_size_t len, tmc_bool def )
	{
		tmc_bool result = def;
		scan_bool_n( str, len, &result );
		return result;
	}
#endif // TMC_CONVENIENCE

#ifdef __cplusplus
}
#endif

#if defined( TMC_CONVENIENCE ) && defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )
	inline tmc_int32 to_i32( const char* nullterminated, tmc_int32 def )
	{
		tmc_int32 result = def;
		scan_i32( nullterminated, 10, &result );
		return result;
	}
	inline tmc_int32 to_i32( const char* str, tmc_size_t len, tmc_int32 def )
	{
		tmc_int32 result = def;
		scan_i32_n( str, len, 10, &result );
		return result;
	}
	inline tmc_uint32 to_u32( const char* nullterminated, tmc_uint32 def )
	{
		tmc_uint32 result = def;
		scan_u32( nullterminated, 10, &result );
		return result;
	}
	inline tmc_uint32 to_u32( const char* str, tmc_size_t len, tmc_uint32 def )
	{
		tmc_uint32 result = def;
		scan_u32_n( str, len, 10, &result );
		return result;
	}
	inline tmc_int64 to_i64( const char* nullterminated, tmc_int64 def )
	{
		tmc_int64 result = def;
		scan_i64( nullterminated, 10, &result );
		return result;
	}
	inline tmc_int64 to_i64( const char* str, tmc_size_t len, tmc_int64 def )
	{
		tmc_int64 result = def;
		scan_i64_n( str, len, 10, &result );
		return result;
	}
	inline tmc_uint64 to_u64( const char* nullterminated, tmc_uint64 def )
	{
		tmc_uint64 result = def;
		scan_u64( nullterminated, 10, &result );
		return result;
	}
	inline tmc_uint64 to_u64( const char* str, tmc_size_t len, tmc_uint64 def )
	{
		tmc_uint64 result = def;
		scan_u64_n( str, len, 10, &result );
		return result;
	}
	inline float to_float( const char* nullterminated )
	{
		float result = 0;
		scan_float( nullterminated, &result );
		return result;
	}
	inline float to_float( const char* str, tmc_size_t len, float def )
	{
		float result = def;
		scan_float_n( str, len, &result );
		return result;
	}
	inline double to_double( const char* nullterminated )
	{
		double result = 0;
		scan_double( nullterminated, &result );
		return result;
	}
	inline double to_double( const char* str, tmc_size_t len, double def )
	{
		double result = def;
		scan_double_n( str, len, &result );
		return result;
	}
	inline tmc_bool to_bool( const char* nullterminated )
	{
		tmc_bool result = TMC_FALSE;
		scan_bool( nullterminated, &result );
		return result;
	}
	inline tmc_bool to_bool( const char* str, tmc_size_t len, tmc_bool def )
	{
		tmc_bool result = def;
		scan_bool_n( str, len, &result );
		return result;
	}

	#ifdef TMC_STRING_VIEW
		inline tmc_int32 to_i32( TMC_STRING_VIEW str, Radix base, tmc_int32 def )
		{
			tmc_int32 result = def;
			scan_i32_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), base.base, &result );
			return result;
		}
		inline tmc_uint32 to_u32( TMC_STRING_VIEW str, Radix base, tmc_uint32 def )
		{
			tmc_uint32 result = def;
			scan_u32_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), base.base, &result );
			return result;
		}
		inline tmc_int64 to_i64( TMC_STRING_VIEW str, Radix base, tmc_int64 def )
		{
			tmc_int64 result = def;
			scan_i64_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), base.base, &result );
			return result;
		}
		inline tmc_uint64 to_u64( TMC_STRING_VIEW str, Radix base, tmc_uint64 def )
		{
			tmc_uint64 result = def;
			scan_u64_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), base.base, &result );
			return result;
		}
		inline tmc_int32 to_i32( TMC_STRING_VIEW str, tmc_int32 def )
		{
			tmc_int32 result = def;
			scan_i32_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), 10, &result );
			return result;
		}
		inline tmc_uint32 to_u32( TMC_STRING_VIEW str, tmc_uint32 def )
		{
			tmc_uint32 result = def;
			scan_u32_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), 10, &result );
			return result;
		}
		inline tmc_int64 to_i64( TMC_STRING_VIEW str, tmc_int64 def )
		{
			tmc_int64 result = def;
			scan_i64_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), 10, &result );
			return result;
		}
		inline tmc_uint64 to_u64( TMC_STRING_VIEW str, tmc_uint64 def )
		{
			tmc_uint64 result = def;
			scan_u64_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), 10, &result );
			return result;
		}
		inline float to_float( TMC_STRING_VIEW str, float def )
		{
			float result = def;
			scan_float_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), &result );
			return result;
		}
		inline double to_double( TMC_STRING_VIEW str, double def )
		{
			double result = def;
			scan_double_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), &result );
			return result;
		}
		inline tmc_bool to_bool( TMC_STRING_VIEW str, tmc_bool def )
		{
			tmc_bool result = def;
			scan_bool_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), &result );
			return result;
		}
	#endif // TMC_STRING_VIEW
	
	// to_string functions
	inline tmc_size_t to_string( tmc_int32 value, char* dest, tmc_size_t maxlen, Radix base )
	{
		PrintFormat format = PrintFormat{base.base};
		return print_i32( dest, maxlen, &format, value );
	}
	inline tmc_size_t to_string( tmc_uint32 value, char* dest, tmc_size_t maxlen, Radix base )
	{
		PrintFormat format = PrintFormat{base.base};
		return print_u32( dest, maxlen, &format, value );
	}
	inline tmc_size_t to_string( tmc_int64 value, char* dest, tmc_size_t maxlen, Radix base )
	{
		PrintFormat format = PrintFormat{base.base};
		return print_i64( dest, maxlen, &format, value );
	}
	inline tmc_size_t to_string( tmc_uint64 value, char* dest, tmc_size_t maxlen, Radix base )
	{
		PrintFormat format = PrintFormat{base.base};
		return print_u64( dest, maxlen, &format, value );
	}
	inline tmc_size_t to_string( double value, char* dest, tmc_size_t maxlen, int precision )
	{
		PrintFormat format = PrintFormat{};
		format.precision = precision;
		return print_double( dest, maxlen, &format, value );
	}
	inline tmc_size_t to_string( float value, char* dest, tmc_size_t maxlen, int precision )
	{
		PrintFormat format = PrintFormat{};
		format.precision = precision;
		return print_float( dest, maxlen, &format, value );
	}
	inline tmc_size_t to_string( tmc_bool value, char* dest, tmc_size_t maxlen, tmc_bool asNumber )
	{
		PrintFormat format = PrintFormat{};
		format.flags = ( asNumber ) ? ( PF_BOOL_AS_NUMBER ) : ( 0 );
		return print_bool( dest, maxlen, &format, value );
	}
	#if defined TMC_STRING_VIEW && !defined( TMC_STRING_VIEW_NO_CONSTRUCTOR )
		TMC_STRING_VIEW to_string( tmc_bool value )
		{
			return ( value ) ? ( TMC_STRING_VIEW( "true" ) ) : ( TMC_STRING_VIEW( "false" ) );
		}
	#endif // defined TMC_STRING_VIEW && !defined( TMC_STRING_VIEW_NO_CONSTRUCTOR )
#endif // defined( TMC_CONVENIENCE ) && defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )

#if defined( TMC_CPP_TEMPLATED ) && defined( __cplusplus )
	template<> tmc_int32 convert_to< tmc_int32 >( const char* str, tmc_int32 def )
	{
		auto result = def;
		scan_i32( str, 10, &result );
		return result;
	}
	template<> tmc_uint32 convert_to< tmc_uint32 >( const char* str, tmc_uint32 def )
	{
		auto result = def;
		scan_u32( str, 10, &result );
		return result;
	}
	template<> tmc_int64 convert_to< tmc_int64 >( const char* str, tmc_int64 def )
	{
		auto result = def;
		scan_i64( str, 10, &result );
		return result;
	}
	template<> tmc_uint64 convert_to< tmc_uint64 >( const char* str, tmc_uint64 def )
	{
		auto result = def;
		scan_u64( str, 10, &result );
		return result;
	}
	template<> float convert_to< float >( const char* str, float def )
	{
		auto result = def;
		scan_float( str, &result );
		return result;
	}
	template<> double convert_to< double >( const char* str, double def )
	{
		auto result = def;
		scan_double( str, &result );
		return result;
	}
	template<> bool convert_to< bool >( const char* str, bool def )
	{
		auto result = def;
		scan_bool( str, &result );
		return result;
	}
	#ifdef TMC_STRING_VIEW
		template<> tmc_int32 convert_to< tmc_int32 >( TMC_STRING_VIEW str, tmc_int32 def )
		{
			auto result = def;
			scan_i32_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), 10, &result );
			return result;
		}
		template<> tmc_uint32 convert_to< tmc_uint32 >( TMC_STRING_VIEW str, tmc_uint32 def )
		{
			auto result = def;
			scan_u32_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), 10, &result );
			return result;
		}
		template<> tmc_int64 convert_to< tmc_int64 >( TMC_STRING_VIEW str, tmc_int64 def )
		{
			auto result = def;
			scan_i64_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), 10, &result );
			return result;
		}
		template<> tmc_uint64 convert_to< tmc_uint64 >( TMC_STRING_VIEW str, tmc_uint64 def )
		{
			auto result = def;
			scan_u64_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), 10, &result );
			return result;
		}
		template<> float convert_to< float >( TMC_STRING_VIEW str, float def )
		{
			auto result = def;
			scan_float_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), &result );
			return result;
		}
		template<> double convert_to< double >( TMC_STRING_VIEW str, double def )
		{
			auto result = def;
			scan_double_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), &result );
			return result;
		}
		template<> bool convert_to< bool >( TMC_STRING_VIEW str, bool def )
		{
			auto result = def;
			scan_bool_n( TMC_STRING_VIEW_DATA( str ), TMC_STRING_VIEW_SIZE( str ), &result );
			return result;
		}
	#endif // TMC_STRING_VIEW
#endif // defined( TMC_CPP_TEMPLATED ) && defined( __cplusplus )

#endif

// implementation

#ifdef TM_CONVERSION_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#define tmc_char_to_i32( x ) ( (tmc_int32)( (tmc_uint8)x ) )

TMC_DEF tmc_size_t scan_i32( const char* nullterminated, int base, tmc_int32* out )
{
	typedef tmc_uint32 utype;
	typedef tmc_int32 stype;
	const stype MIN_VAL = TMC_INT32_MIN;
	const stype MAX_VAL = TMC_INT32_MAX;
	const utype UMIN_VAL = (utype)TMC_INT32_MIN;
	const utype UMAX_VAL = TMC_INT32_MAX;

	TMC_ASSERT( base >= 0 );

	const char* start = nullterminated;
	const char* p = nullterminated;

	// check for 0x prefix
	if( ( base <= 0 || base == 16 ) && ( *p == '0' && ( p[1] == 'x' || p[1] == 'X' ) ) ) {
		p += 2;
		base = 16;
	} else if( ( base <= 0 || base == 2 ) && ( *p == '0' && ( p[1] == 'b' || p[1] == 'B' ) ) ) {
		// check for 0b prefix
		p += 2;
		base = 2;
	}
	// check whether we have valid base
	if( base <= 0 ) {
		base = 10;  // default base is 10
	}

	int sign = 0;
	int negative = 0;
	if( base == 10 ) {
		if( *p == '-' ) {
			negative = 1;
			sign = 1;
			++p;
		} else if( *p == '+' ) {
			sign = 1;
			++p;
		}
	}

	utype ubase = (utype)base;
	int rangeError = 0;
	utype maxValue = ( negative ) ? ( UMIN_VAL ) : ( UMAX_VAL );
	utype maxDigit = maxValue % ubase;
	maxValue /= ubase;
	utype value = 0;
	for( ; *p; ++p ) {
		tmc_int32 cp = tmc_char_to_i32( *p );
		utype digit;
		if( TMC_ISDIGIT( cp ) ) {
			digit = cp - '0';
		} else if( TMC_ISUPPER( cp ) ) {
			digit = cp - 'A' + 10;
		} else if( TMC_ISLOWER( cp ) ) {
			digit = cp - 'a' + 10;
		} else {
			break;
		}
		if( digit >= ubase ) {
			break;
		}
		if( rangeError || value > maxValue || ( value == maxValue && digit > maxDigit ) ) {
			rangeError = 1;
		} else {
			value = value * ubase + digit;
		}
	}
	tmc_size_t dist = ( tmc_size_t )( p - start );
	// did we scan anything or just a sign?
	if( dist == 0 || ( sign && dist == 1 ) ) {
		return 0;
	}
	if( out ) {
		if( rangeError ) {
			*out = ( negative ) ? ( MIN_VAL ) : ( MAX_VAL );
		} else {
			*out = ( negative ) ? ( -(stype)value ) : ( (stype)value );
		}
	}
	return dist;
}
TMC_DEF tmc_size_t scan_i32_n( const char* str, tmc_size_t len, int base, tmc_int32* out )
{
	typedef tmc_uint32 utype;
	typedef tmc_int32 stype;
	const stype MIN_VAL = TMC_INT32_MIN;
	const stype MAX_VAL = TMC_INT32_MAX;
	const utype UMIN_VAL = (utype)TMC_INT32_MIN;
	const utype UMAX_VAL = TMC_INT32_MAX;

	TMC_ASSERT( base >= 0 );
	TMC_ASSERT( len >= 0 );
	if( len <= 0 ) {
		return 0;
	}

	const char* start = str;
	const char* p = str;

	// check for 0x prefix
	if( ( base <= 0 || base == 16 ) && len >= 2
		&& ( *p == '0' && ( p[1] == 'x' || p[1] == 'X' ) ) ) {
		p += 2;
		len -= 2;
		base = 16;
	} else if( ( base <= 0 || base == 2 ) && len >= 2
			   && ( *p == '0' && ( p[1] == 'b' || p[1] == 'B' ) ) ) {
		// check for 0b prefix
		p += 2;
		len -= 2;
		base = 2;
	}
	// check whether we have valid base
	if( base <= 0 ) {
		base = 10;  // default base is 10
	}

	int sign = 0;
	int negative = 0;
	if( base == 10 ) {
		if( *p == '-' ) {
			negative = 1;
			sign = 1;
			++p;
			--len;
		} else if( *p == '+' ) {
			sign = 1;
			++p;
			--len;
		}
	}

	utype ubase = (utype)base;
	int rangeError = 0;
	utype maxValue = ( negative ) ? ( UMIN_VAL ) : ( UMAX_VAL );
	utype maxDigit = maxValue % ubase;
	maxValue /= ubase;
	utype value = 0;
	for( ; len; ++p, --len ) {
		tmc_int32 cp = tmc_char_to_i32( *p );
		utype digit;
		if( TMC_ISDIGIT( cp ) ) {
			digit = cp - '0';
		} else if( TMC_ISUPPER( cp ) ) {
			digit = cp - 'A' + 10;
		} else if( TMC_ISLOWER( cp ) ) {
			digit = cp - 'a' + 10;
		} else {
			break;
		}
		if( digit >= ubase ) {
			break;
		}
		if( rangeError || value > maxValue || ( value == maxValue && digit > maxDigit ) ) {
			rangeError = 1;
		} else {
			value = value * ubase + digit;
		}
	}
	tmc_size_t dist = ( tmc_size_t )( p - start );
	// did we scan anything or just a sign?
	if( dist == 0 || ( sign && dist == 1 ) ) {
		return 0;
	}
	if( out ) {
		if( rangeError ) {
			*out = ( negative ) ? ( MIN_VAL ) : ( MAX_VAL );
		} else {
			*out = ( negative ) ? ( -(stype)value ) : ( (stype)value );
		}
	}
	return dist;
}
TMC_DEF tmc_size_t scan_u32( const char* nullterminated, int base, tmc_uint32* out )
{
	typedef tmc_uint32 utype;
	const utype UMAX_VAL = UINT32_MAX;

	TMC_ASSERT( base >= 0 );

	const char* start = nullterminated;
	const char* p = nullterminated;

	// check for 0x prefix
	if( ( base <= 0 || base == 16 ) && ( *p == '0' && ( p[1] == 'x' || p[1] == 'X' ) ) ) {
		p += 2;
		base = 16;
	} else if( ( base <= 0 || base == 2 ) && ( *p == '0' && ( p[1] == 'b' || p[1] == 'B' ) ) ) {
		// check for 0b prefix
		p += 2;
		base = 2;
	}
	// check whether we have valid base
	if( base <= 0 ) {
		base = 10;  // default base is 10
	}

	int sign = 0;
	if( *p == '+' && base == 10 ) {
		sign = 1;
		++p;
	}

	utype ubase = (utype)base;
	int rangeError = 0;
	utype maxValue = UMAX_VAL / ubase;
	utype maxDigit = UMAX_VAL % ubase;
	utype value = 0;
	for( ; *p; ++p ) {
		tmc_int32 cp = tmc_char_to_i32( *p );
		utype digit;
		if( TMC_ISDIGIT( cp ) ) {
			digit = cp - '0';
		} else if( TMC_ISUPPER( cp ) ) {
			digit = cp - 'A' + 10;
		} else if( TMC_ISLOWER( cp ) ) {
			digit = cp - 'a' + 10;
		} else {
			break;
		}
		if( digit >= ubase ) {
			break;
		}
		if( rangeError || value > maxValue || ( value == maxValue && digit > maxDigit ) ) {
			rangeError = 1;
		} else {
			value = value * ubase + digit;
		}
	}
	tmc_size_t dist = ( tmc_size_t )( p - start );
	// did we scan anything or just a sign?
	if( dist == 0 || ( sign && dist == 1 ) ) {
		return 0;
	}
	if( out ) {
		if( rangeError ) {
			*out = UMAX_VAL;
		} else {
			*out = value;
		}
	}
	return dist;
}
TMC_DEF tmc_size_t scan_u32_n( const char* str, tmc_size_t len, int base, tmc_uint32* out )
{
	typedef tmc_uint32 utype;
	const utype UMAX_VAL = UINT32_MAX;

	TMC_ASSERT( base >= 0 );
	TMC_ASSERT( len >= 0 );
	if( len <= 0 ) {
		return 0;
	}

	const char* start = str;
	const char* p = str;

	// check for 0x prefix
	if( ( base <= 0 || base == 16 ) && len >= 2
		&& ( *p == '0' && ( p[1] == 'x' || p[1] == 'X' ) ) ) {
		p += 2;
		len -= 2;
		base = 16;
	} else if( ( base <= 0 || base == 2 ) && len >= 2
			   && ( *p == '0' && ( p[1] == 'b' || p[1] == 'B' ) ) ) {
		// check for 0b prefix
		p += 2;
		len -= 2;
		base = 2;
	}
	// check whether we have valid base
	if( base <= 0 ) {
		base = 10;  // default base is 10
	}

	int sign = 0;
	if( *p == '+' && base == 10 ) {
		sign = 1;
		++p;
		--len;
	}

	utype ubase = (utype)base;
	int rangeError = 0;
	utype maxValue = UMAX_VAL / ubase;
	utype maxDigit = UMAX_VAL % ubase;
	utype value = 0;
	for( ; len; ++p, --len ) {
		tmc_int32 cp = tmc_char_to_i32( *p );
		utype digit;
		if( TMC_ISDIGIT( cp ) ) {
			digit = cp - '0';
		} else if( TMC_ISUPPER( cp ) ) {
			digit = cp - 'A' + 10;
		} else if( TMC_ISLOWER( cp ) ) {
			digit = cp - 'a' + 10;
		} else {
			break;
		}
		if( digit >= ubase ) {
			break;
		}
		if( rangeError || value > maxValue || ( value == maxValue && digit > maxDigit ) ) {
			rangeError = 1;
		} else {
			value = value * ubase + digit;
		}
	}
	tmc_size_t dist = ( tmc_size_t )( p - start );
	// did we scan anything or just a sign?
	if( dist == 0 || ( sign && dist == 1 ) ) {
		return 0;
	}
	if( out ) {
		if( rangeError ) {
			*out = UMAX_VAL;
		} else {
			*out = value;
		}
	}
	return dist;
}
TMC_DEF tmc_size_t scan_i64( const char* nullterminated, int base, tmc_int64* out )
{
	typedef tmc_uint64 utype;
	typedef tmc_int64 stype;
	const stype MIN_VAL = TMC_INT64_MIN;
	const stype MAX_VAL = TMC_INT64_MAX;
	const utype UMIN_VAL = (utype)TMC_INT64_MIN;
	const utype UMAX_VAL = TMC_INT64_MAX;

	TMC_ASSERT( base >= 0 );

	const char* start = nullterminated;
	const char* p = nullterminated;

	int sign = 0;
	int negative = 0;
	if( *p == '-' ) {
		negative = 1;
		sign = 1;
		++p;
	} else if( *p == '+' ) {
		sign = 1;
		++p;
	}

	// check for 0x prefix
	if( ( base <= 0 || base == 16 ) && ( *p == '0' && ( p[1] == 'x' || p[1] == 'X' ) ) ) {
		p += 2;
		base = 16;
	} else if( ( base <= 0 || base == 2 ) && ( *p == '0' && ( p[1] == 'b' || p[1] == 'B' ) ) ) {
		// check for 0b prefix
		p += 2;
		base = 2;
	}
	// check whether we have valid base
	if( base <= 0 ) {
		base = 10;  // default base is 10
	}

	utype ubase = (utype)base;
	int rangeError = 0;
	utype maxValue = ( negative ) ? ( UMIN_VAL ) : ( UMAX_VAL );
	utype maxDigit = maxValue % ubase;
	maxValue /= ubase;
	utype value = 0;
	for( ; *p; ++p ) {
		tmc_int32 cp = tmc_char_to_i32( *p );
		utype digit;
		if( TMC_ISDIGIT( cp ) ) {
			digit = cp - '0';
		} else if( TMC_ISUPPER( cp ) ) {
			digit = cp - 'A' + 10;
		} else if( TMC_ISLOWER( cp ) ) {
			digit = cp - 'a' + 10;
		} else {
			break;
		}
		if( digit >= ubase ) {
			break;
		}
		if( rangeError || value > maxValue || ( value == maxValue && digit > maxDigit ) ) {
			rangeError = 1;
		} else {
			value = value * ubase + digit;
		}
	}
	tmc_size_t dist = ( tmc_size_t )( p - start );
	// did we scan anything or just a sign?
	if( dist == 0 || ( sign && dist == 1 ) ) {
		return 0;
	}
	if( out ) {
		if( rangeError ) {
			*out = ( negative ) ? ( MIN_VAL ) : ( MAX_VAL );
		} else {
			*out = ( negative ) ? ( -(stype)value ) : ( (stype)value );
		}
	}
	return dist;
}
TMC_DEF tmc_size_t scan_i64_n( const char* str, tmc_size_t len, int base, tmc_int64* out )
{
	typedef tmc_uint64 utype;
	typedef tmc_int64 stype;
	const stype MIN_VAL = TMC_INT64_MIN;
	const stype MAX_VAL = TMC_INT64_MAX;
	const utype UMIN_VAL = (utype)TMC_INT64_MIN;
	const utype UMAX_VAL = TMC_INT64_MAX;

	TMC_ASSERT( base >= 0 );
	TMC_ASSERT( len >= 0 );
	if( len <= 0 ) {
		return 0;
	}

	const char* start = str;
	const char* p = str;

	int sign = 0;
	int negative = 0;
	if( *p == '-' ) {
		negative = 1;
		sign = 1;
		++p;
		--len;
	} else if( *p == '+' ) {
		sign = 1;
		++p;
		--len;
	}

	// check for 0x prefix
	if( ( base <= 0 || base == 16 ) && len >= 2
		&& ( *p == '0' && ( p[1] == 'x' || p[1] == 'X' ) ) ) {
		p += 2;
		len -= 2;
		base = 16;
	} else if( ( base <= 0 || base == 2 ) && len >= 2
			   && ( *p == '0' && ( p[1] == 'b' || p[1] == 'B' ) ) ) {
		// check for 0b prefix
		p += 2;
		len -= 2;
		base = 2;
	}
	// check whether we have valid base
	if( base <= 0 ) {
		base = 10;  // default base is 10
	}

	utype ubase = (utype)base;
	int rangeError = 0;
	utype maxValue = ( negative ) ? ( UMIN_VAL ) : ( UMAX_VAL );
	utype maxDigit = maxValue % ubase;
	maxValue /= ubase;
	utype value = 0;
	for( ; len; ++p, --len ) {
		tmc_int32 cp = tmc_char_to_i32( *p );
		utype digit;
		if( TMC_ISDIGIT( cp ) ) {
			digit = cp - '0';
		} else if( TMC_ISUPPER( cp ) ) {
			digit = cp - 'A' + 10;
		} else if( TMC_ISLOWER( cp ) ) {
			digit = cp - 'a' + 10;
		} else {
			break;
		}
		if( digit >= ubase ) {
			break;
		}
		if( rangeError || value > maxValue || ( value == maxValue && digit > maxDigit ) ) {
			rangeError = 1;
		} else {
			value = value * ubase + digit;
		}
	}
	tmc_size_t dist = ( tmc_size_t )( p - start );
	// did we scan anything or just a sign?
	if( dist == 0 || ( sign && dist == 1 ) ) {
		return 0;
	}
	if( out ) {
		if( rangeError ) {
			*out = ( negative ) ? ( MIN_VAL ) : ( MAX_VAL );
		} else {
			*out = ( negative ) ? ( -(stype)value ) : ( (stype)value );
		}
	}
	return dist;
}
TMC_DEF tmc_size_t scan_u64( const char* nullterminated, int base, tmc_uint64* out )
{
	typedef tmc_uint64 utype;
	const utype UMAX_VAL = UINT64_MAX;

	TMC_ASSERT( base >= 0 );

	const char* start = nullterminated;
	const char* p = nullterminated;

	int sign = 0;
	if( *p == '+' ) {
		sign = 1;
		++p;
	}

	// check for 0x prefix
	if( ( base <= 0 || base == 16 ) && ( *p == '0' && ( p[1] == 'x' || p[1] == 'X' ) ) ) {
		p += 2;
		base = 16;
	} else if( ( base <= 0 || base == 2 ) && ( *p == '0' && ( p[1] == 'b' || p[1] == 'B' ) ) ) {
		// check for 0b prefix
		p += 2;
		base = 2;
	}
	// check whether we have valid base
	if( base <= 0 ) {
		base = 10;  // default base is 10
	}

	utype ubase = (utype)base;
	int rangeError = 0;
	utype maxValue = UMAX_VAL / ubase;
	utype maxDigit = UMAX_VAL % ubase;
	utype value = 0;
	for( ; *p; ++p ) {
		tmc_int32 cp = tmc_char_to_i32( *p );
		utype digit;
		if( TMC_ISDIGIT( cp ) ) {
			digit = cp - '0';
		} else if( TMC_ISUPPER( cp ) ) {
			digit = cp - 'A' + 10;
		} else if( TMC_ISLOWER( cp ) ) {
			digit = cp - 'a' + 10;
		} else {
			break;
		}
		if( digit >= ubase ) {
			break;
		}
		if( rangeError || value > maxValue || ( value == maxValue && digit > maxDigit ) ) {
			rangeError = 1;
		} else {
			value = value * ubase + digit;
		}
	}
	tmc_size_t dist = ( tmc_size_t )( p - start );
	// did we scan anything or just a sign?
	if( dist == 0 || ( sign && dist == 1 ) ) {
		return 0;
	}
	if( out ) {
		if( rangeError ) {
			*out = UMAX_VAL;
		} else {
			*out = value;
		}
	}
	return dist;
}
TMC_DEF tmc_size_t scan_u64_n( const char* str, tmc_size_t len, int base, tmc_uint64* out )
{
	typedef tmc_uint64 utype;
	const utype UMAX_VAL = UINT64_MAX;

	TMC_ASSERT( base >= 0 );
	TMC_ASSERT( len >= 0 );
	if( len <= 0 ) {
		return 0;
	}

	const char* start = str;
	const char* p = str;

	int sign = 0;
	if( *p == '+' ) {
		sign = 1;
		++p;
		--len;
	}

	// check for 0x prefix
	if( ( base <= 0 || base == 16 ) && len >= 2
		&& ( *p == '0' && ( p[1] == 'x' || p[1] == 'X' ) ) ) {
		p += 2;
		len -= 2;
		base = 16;
	} else if( ( base <= 0 || base == 2 ) && len >= 2
			   && ( *p == '0' && ( p[1] == 'b' || p[1] == 'B' ) ) ) {
		// check for 0b prefix
		p += 2;
		len -= 2;
		base = 2;
	}
	// check whether we have valid base
	if( base <= 0 ) {
		base = 10;  // default base is 10
	}

	utype ubase = (utype)base;
	int rangeError = 0;
	utype maxValue = UMAX_VAL / ubase;
	utype maxDigit = UMAX_VAL % ubase;
	utype value = 0;
	for( ; len; ++p, --len ) {
		tmc_int32 cp = tmc_char_to_i32( *p );
		utype digit;
		if( TMC_ISDIGIT( cp ) ) {
			digit = cp - '0';
		} else if( TMC_ISUPPER( cp ) ) {
			digit = cp - 'A' + 10;
		} else if( TMC_ISLOWER( cp ) ) {
			digit = cp - 'a' + 10;
		} else {
			break;
		}
		if( digit >= ubase ) {
			break;
		}
		if( rangeError || value > maxValue || ( value == maxValue && digit > maxDigit ) ) {
			rangeError = 1;
		} else {
			value = value * ubase + digit;
		}
	}
	tmc_size_t dist = ( tmc_size_t )( p - start );
	// did we scan anything or just a sign?
	if( dist == 0 || ( sign && dist == 1 ) ) {
		return 0;
	}
	if( out ) {
		if( rangeError ) {
			*out = UMAX_VAL;
		} else {
			*out = value;
		}
	}
	return dist;
}
TMC_DEF tmc_size_t scan_float( const char* nullterminated, float* out )
{
	double val;
	tmc_size_t ret = scan_double( nullterminated, &val );
	if( ret && out ) {
		*out = (float)val;
	}
	return ret;
}
TMC_DEF tmc_size_t scan_float_n( const char* str, tmc_size_t len, float* out )
{
	double val;
	tmc_size_t ret = scan_double_n( str, len, &val );
	if( ret && out ) {
		*out = (float)val;
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

static tmc_size_t scan_LeadingZeroes( const char* str, tmc_size_t len )
{
	const char* start = str;
	while( len && ( *str == '0' || *str == '.' ) ) {
		++str;
		--len;
	}
	return ( tmc_size_t )( str - start );
}
static tmc_size_t scan_ScanMantissa( const char* str, tmc_size_t len, double* out )
{
	// there are three ways to scan a string into a double
	// scan directly into a double, using floating arithmetic (slightly slow)
	// scan using int64 and convert to double (uint64 might not be available)
	// scan using two tmc_int32's and combine into double (portable and fast)
	// we will be using the third option

	// check whether the digits can be represented in a double
	if( len > 18 ) {
		len = 18;
		// overflow
	}

	const char* start = str;

	tmc_int32 highPart = 0;
	while( len > 9 ) {
		tmc_int32 cp = tmc_char_to_i32( *str );
		// skip dot
		if( cp == '.' ) {
			++str;
			// we do not decrease len when we encounter the decimal point, because len only counts
			// digits
			cp = tmc_char_to_i32( *str );
		}
		highPart = highPart * 10 + ( cp - '0' );
		++str;
		--len;
	}
	tmc_int32 lowPart = 0;
	while( len ) {
		tmc_int32 cp = tmc_char_to_i32( *str );
		// skip dot
		if( cp == '.' ) {
			++str;
			// we do not decrease len when we encounter the decimal point, because len only counts
			// digits
			cp = tmc_char_to_i32( *str );
		}
		lowPart = lowPart * 10 + ( cp - '0' );
		++str;
		--len;
	}
	*out = ( 1.0e9 * highPart ) + lowPart;
	return ( tmc_size_t )( str - start );
}
static double str_Pow10( double value, int exponent )
{
	TMC_ASSERT( exponent < scan_MaxExponent );
	// we use the exponentiation by squaring algorithm to calculate pow(10, exponent),
	// while using a lookup table for the squared powers of 10
	static const double PowersOfTen[] = {
		10, 100, 1.0e4, 1.0e8, 1.0e16, 1.0e32, 1.0e64, 1.0e128, 1.0e256,
	};

	int exponentNegative = exponent < 0;
	if( exponentNegative ) {
		exponent = -exponent;
	}

	double tenToThePower = 1.0;
	tmc_uint32 uexp = (tmc_uint32)exponent;  // convert to tmc_uint32 for bit twiddling
	for( const double* entry = PowersOfTen; uexp; uexp >>= 1, ++entry ) {
		if( uexp & 0x1 ) {
			tenToThePower *= *entry;
		}
	}
	if( exponentNegative ) {
		value /= tenToThePower;
	} else {
		value *= tenToThePower;
	}
	return value;
}

TMC_DEF tmc_size_t scan_double( const char* nullterminated, double* out )
{
	const char* start = nullterminated;
	const char* p = nullterminated;

	int negative = 0;
	if( *p == '-' ) {
		negative = 1;
		++p;
	} else if( *p == '+' ) {
		++p;
	}

	// scan integer and frational parts
	int decimalPos = -1;
	int mantissaLength = 0;
	int integerLength = 0;
	int fractionalLength = 0;
	for( const char* s = p;; ++s, ++mantissaLength ) {
		tmc_int32 cp = tmc_char_to_i32( *s );
		if( cp == '.' && decimalPos < 0 ) {
			decimalPos = mantissaLength + 1;
			integerLength = mantissaLength;
		} else if( !TMC_ISDIGIT( cp ) ) {
			break;
		}
	}
	if( decimalPos < 0 && mantissaLength == 0 ) {
		return 0;
	}
	if( decimalPos < 0 ) {
		decimalPos = mantissaLength;
		fractionalLength = 0;
		integerLength = mantissaLength;
	} else {
		fractionalLength = mantissaLength - decimalPos;
	}

	// scan exponent
	const char* pExp = p + mantissaLength;
	int exponentNegative = 0;
	int exponent = 0;
	if( *pExp == 'e' || *pExp == 'E' ) {
		++pExp;
		if( *pExp == '-' ) {
			exponentNegative = 1;
			++pExp;
		} else if( *pExp == '+' ) {
			++pExp;
		}
		if( TMC_ISDIGIT( tmc_char_to_i32( *pExp ) ) ) {
			pExp += scan_i32( pExp, 10, &exponent );
			if( exponent > scan_MaxExponent ) {
				exponent = scan_MaxExponent;
				// range error
			}
		} else {
			pExp = p + mantissaLength;
		}
	}

	// adjust integerLength and fractionalLength based on exponent
	if( exponentNegative ) {
		if( integerLength > exponent ) {
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
		if( fractionalLength > exponent ) {
			fractionalLength -= exponent;
			integerLength += exponent;
			exponent = 0;
		} else {
			exponent -= fractionalLength;
			integerLength += fractionalLength;
			fractionalLength = 0;
		}
	}

	if( out ) {
		double integerPart = 0;
		double fractionalPart = 0;
		tmc_size_t leadingZeroes = scan_LeadingZeroes( p, integerLength );
		if( leadingZeroes < integerLength ) {
			p += leadingZeroes;
			integerLength -= (int)leadingZeroes;
			p += scan_ScanMantissa( p, integerLength, &integerPart );
		} else {
			p += integerLength;
		}
		scan_ScanMantissa( p, fractionalLength, &fractionalPart );
		fractionalPart = str_Pow10( fractionalPart, -fractionalLength );

		double value = str_Pow10( integerPart + fractionalPart, exponent );
		*out = ( negative ) ? ( -value ) : ( value );
	}
	return ( tmc_size_t )( pExp - start );
}
TMC_DEF tmc_size_t scan_double_n( const char* str, tmc_size_t len, double* out )
{
	TMC_ASSERT( len >= 0 );
	if( len <= 0 ) {
		return 0;
	}

	const char* start = str;
	const char* p = str;

	int negative = 0;
	if( *p == '-' ) {
		negative = 1;
		++p;
		--len;
	} else if( *p == '+' ) {
		++p;
		--len;
	}

	// scan integer and frational parts
	int decimalPos = -1;
	int mantissaLength = 0;
	int integerLength = 0;
	int fractionalLength = 0;
	for( const char* s = p; len; ++s, ++mantissaLength, --len ) {
		tmc_int32 cp = tmc_char_to_i32( *s );
		if( cp == '.' && decimalPos < 0 ) {
			decimalPos = mantissaLength + 1;
			integerLength = mantissaLength;
		} else if( !TMC_ISDIGIT( cp ) ) {
			break;
		}
	}
	if( decimalPos < 0 && mantissaLength == 0 ) {
		return 0;
	}
	if( decimalPos < 0 ) {
		decimalPos = mantissaLength;
		fractionalLength = 0;
		integerLength = mantissaLength;
	} else {
		fractionalLength = mantissaLength - decimalPos;
	}

	// scan exponent
	const char* pExp = p + mantissaLength;
	int exponentNegative = 0;
	int exponent = 0;
	if( len && ( *pExp == 'e' || *pExp == 'E' ) ) {
		++pExp;
		--len;
		if( len && *pExp == '-' ) {
			exponentNegative = 1;
			++pExp;
			--len;
		} else if( len && *pExp == '+' ) {
			++pExp;
			--len;
		}
		if( len && TMC_ISDIGIT( tmc_char_to_i32( *pExp ) ) ) {
			pExp += scan_i32_n( pExp, len, 10, &exponent );
			if( exponent > scan_MaxExponent ) {
				exponent = scan_MaxExponent;
				// range error
			}
		} else {
			pExp = p + mantissaLength;
		}
	}

	// adjust integerLength and fractionalLength based on exponent
	if( exponentNegative ) {
		if( integerLength > exponent ) {
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
		if( fractionalLength > exponent ) {
			fractionalLength -= exponent;
			integerLength += exponent;
			exponent = 0;
		} else {
			exponent -= fractionalLength;
			integerLength += fractionalLength;
			fractionalLength = 0;
		}
	}

	if( out ) {
		double integerPart = 0;
		double fractionalPart = 0;
		tmc_size_t leadingZeroes = scan_LeadingZeroes( p, integerLength );
		if( leadingZeroes < integerLength ) {
			p += leadingZeroes;
			integerLength -= (int)leadingZeroes;
			p += scan_ScanMantissa( p, integerLength, &integerPart );
		} else {
			p += integerLength;
		}
		scan_ScanMantissa( p, fractionalLength, &fractionalPart );
		fractionalPart = str_Pow10( fractionalPart, -fractionalLength );

		double value = str_Pow10( integerPart + fractionalPart, exponent );
		*out = ( negative ) ? ( -value ) : ( value );
	}
	return ( tmc_size_t )( pExp - start );
}

static tmc_size_t print_string( char* dest, tmc_size_t maxlen, const char* src, tmc_size_t srclen )
{
	tmc_size_t len = ( srclen < maxlen ) ? ( srclen ) : ( maxlen );
	TMC_MEMCPY( dest, src, len );
	return len;
}

#ifdef __cplusplus

#ifdef TMC_IMPLEMENT_STRNICMP
	static tmc_int32 scan_strnicmp( const char* a, const char* b, tmc_size_t maxlen )
	{
		TMC_ASSERT( maxlen >= 0 );
		while( *a && *b && maxlen-- ) {
			tmc_int32 aUpper = toupper( tmc_char_to_i32( *a ) );
			tmc_int32 bUpper = toupper( tmc_char_to_i32( *b ) );
			if( aUpper != bUpper ) {
				break;
			}
			++a;
			++b;
		}
		if( !maxlen ) {
			return 0;
		}
		tmc_int32 aUpper = toupper( tmc_char_to_i32( *a ) );
		tmc_int32 bUpper = toupper( tmc_char_to_i32( *b ) );
		return aUpper - bUpper;
	}
#endif

TMC_DEF tmc_size_t scan_bool( const char* nullterminated, bool* out )
{
	if( *nullterminated == '1' ) {
		if( out ) {
			*out = TMC_TRUE;
		}
		return 1;
	} else if( *nullterminated == '0' ) {
		if( out ) {
			*out = TMC_FALSE;
		}
		return 1;
	} else if( TMC_STRNICMP( nullterminated, "true", 4 ) == 0 ) {
		if( out ) {
			*out = TMC_TRUE;
		}
		return 4;
	} else if( TMC_STRNICMP( nullterminated, "false", 5 ) == 0 ) {
		if( out ) {
			*out = TMC_FALSE;
		}
		return 5;
	}
	return 0;
}
TMC_DEF tmc_size_t scan_bool_n( const char* str, tmc_size_t len, bool* out )
{
	TMC_ASSERT( len >= 0 );
	if( len <= 0 ) {
		return 0;
	}
	if( *str == '1' ) {
		if( out ) {
			*out = TMC_TRUE;
		}
		return 1;
	} else if( *str == '0' ) {
		if( out ) {
			*out = TMC_FALSE;
		}
		return 1;
	} else if( TMC_STRNICMP( str, "true", len ) == 0 ) {
		if( out ) {
			*out = TMC_TRUE;
		}
		return 4;
	} else if( TMC_STRNICMP( str, "false", len ) == 0 ) {
		if( out ) {
			*out = TMC_FALSE;
		}
		return 5;
	}
	return 0;
}

TMC_DEF tmc_size_t print_bool( char* dest, tmc_size_t maxlen, PrintFormat* format, bool value )
{
	if( format && ( format->flags & PF_BOOL_AS_NUMBER ) ) {
		return print_string( dest, maxlen, ( value ) ? ( "1" ) : ( "0" ), 1 );
	} else {
		static const char* strings[] = {"true", "false"};
		static tmc_size_t lengths[] = {sizeof( "true" ), sizeof( "false" )};
		size_t index = value != 0;
		return print_string( dest, maxlen, strings[index], lengths[index] );
	}
}
#endif

// print implementation

static const char print_NumberToCharTable[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

static const double print_PowersOfTen[] = {1,	 10,	100,   1.0e3, 1.0e4,
										   1.0e5, 1.0e6, 1.0e7, 1.0e8, 1.0e9};
// magnitude is the number of digits before decimal point
static tmc_int32 print_magnitude( double value )
{
	// calculate integer based log10
	tmc_int32 n = 0;
	if( value < 0 ) {
		value = -value;
	}
	while( value >= 10 ) {
		if( value >= 1.0e32 ) {
			n += 32;
			value /= 1.0e32;
		}
		if( value >= 1.0e16 ) {
			n += 16;
			value /= 1.0e16;
		}
		if( value >= 1.0e8 ) {
			n += 8;
			value /= 1.0e8;
		}
		if( value >= 1.0e4 ) {
			n += 4;
			value /= 1.0e4;
		}
		if( value >= 1.0e2 ) {
			n += 2;
			value /= 1.0e2;
		}
		if( value >= 1.0e1 ) {
			n += 1;
			value /= 1.0e1;
		}
	}

	// return magnitude = log10( value ) + 1
	return n + 1;
}

#ifdef TMC_IMPLEMENT_STRNREV
static char* print_strnrev( char* str, size_t count )
{
	for( size_t i = 0, j = count - 1; i < j; ++i, --j ) {
		char tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}
	return str;
}
#endif

TMC_DEF tmc_size_t print_i32( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_int32 value )
{
	TMC_ASSERT( dest );
	TMC_ASSERT( maxlen >= 0 );
	if( maxlen <= 0 ) {
		return 0;
	}

	tmc_size_t ret = 0;
	tmc_int32 base = 10;
	tmc_uint32 flags = 0;
	if( format ) {
		base = format->base;
		flags = format->flags;
	}
	if( value < 0 && base == 10 ) {
		*dest++ = '-';
		--maxlen;
		++ret;
		value = -value;
		if( format ) {
			// clear sign flag, so that print_u32 doesn't print '+'
			format->flags &= ~PF_SIGN;
		}
	}

	ret += print_u32( dest, maxlen, format, (tmc_uint32)value );
	if( format ) {
		format->flags = flags;
	}
	return ret;
}
TMC_DEF tmc_size_t print_u32( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_uint32 value )
{
	TMC_ASSERT( dest );
	TMC_ASSERT( maxlen >= 0 );
	if( maxlen <= 0 ) {
		return 0;
	}
	tmc_int32 base = 10;
	if( format ) {
		base = format->base;
		if( format->width > 0 ) {
			if( maxlen < format->width ) {
				return 0;
			}
			TMC_MEMSET( dest, '0', format->width );
			maxlen = format->width;
		}
	}
	if( base <= 1 ) {
		base = 10;
	} else if( base > 16 ) {
		base = 16;
	}

	tmc_size_t start = maxlen;
	int sign = 0;
	if( format && ( format->flags & PF_SIGN ) && base == 10 ) {
		if( maxlen <= 1 ) {
			return 0;
		}
		sign = 1;
		*dest++ = '+';
		--maxlen;
	}

	char* p = dest;
	if( value ) {
		while( maxlen && value ) {
			*p++ = print_NumberToCharTable[value % base];
			--maxlen;
			value /= base;
		}
	} else {
		*p++ = '0';
		--maxlen;
	}

	tmc_size_t len = start - maxlen;
	if( format && format->width ) {
		len = format->width;
	}
	if( !sign ) {
		TMC_STRNREV( dest, len );
	} else {
		TMC_STRNREV( dest, len - 1 );
	}

	if( base > 10 && format && ( format->flags & PF_LOWERCASE ) ) {
		// we use size_t here directly because we iterate over dest
		for( size_t i = 0; i < (size_t)len; ++i ) {
			dest[i] = (char)tolower( tmc_char_to_i32( dest[i] ) );
		}
	}
	return len;
}
TMC_DEF tmc_size_t print_i64( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_int64 value )
{
	TMC_ASSERT( dest );
	TMC_ASSERT( maxlen >= 0 );
	if( maxlen <= 0 ) {
		return 0;
	}

	tmc_size_t ret = 0;
	tmc_int32 base = 10;
	tmc_uint32 flags = 0;
	if( format ) {
		base = format->base;
		flags = format->flags;
	}
	if( value < 0 && base == 10 ) {
		*dest++ = '-';
		--maxlen;
		++ret;
		value = -value;
		if( format ) {
			// clear sign flag, so that print_u32 doesn't print '+'
			format->flags &= ~PF_SIGN;
		}
	}

	ret += print_u64( dest, maxlen, format, (tmc_uint64)value );
	if( format ) {
		format->flags = flags;
	}
	return ret;
}
TMC_DEF tmc_size_t print_u64( char* dest, tmc_size_t maxlen, PrintFormat* format, tmc_uint64 value )
{
	TMC_ASSERT( dest );
	TMC_ASSERT( maxlen >= 0 );
	if( maxlen <= 0 ) {
		return 0;
	}
	tmc_int32 base = 10;
	if( format ) {
		base = format->base;
		if( format->width > 0 ) {
			if( maxlen < format->width ) {
				return 0;
			}
			TMC_MEMSET( dest, '0', format->width );
			maxlen = format->width;
		}
	}
	if( base <= 1 ) {
		base = 10;
	} else if( base > 16 ) {
		base = 16;
	}

	tmc_size_t start = maxlen;
	int sign = 0;
	if( format && ( format->flags & PF_SIGN ) && base == 10 ) {
		if( maxlen <= 1 ) {
			return 0;
		}
		sign = 1;
		*dest++ = '+';
		--maxlen;
	}

	char* p = dest;
	if( value ) {
		while( maxlen && value ) {
			*p++ = print_NumberToCharTable[value % base];
			--maxlen;
			value /= base;
		}
	} else {
		*p++ = '0';
		--maxlen;
	}

	tmc_size_t len = start - maxlen;
	if( format && format->width ) {
		len = format->width;
	}
	if( !sign ) {
		TMC_STRNREV( dest, len );
	} else {
		TMC_STRNREV( dest, len - 1 );
	}

	if( base > 10 && format && ( format->flags & PF_LOWERCASE ) ) {
		// we use size_t here directly because we iterate over dest
		for( size_t i = 0; i < (size_t)len; ++i ) {
			dest[i] = (char)tolower( tmc_char_to_i32( dest[i] ) );
		}
	}
	return len;
}
TMC_DEF tmc_size_t print_double( char* dest, tmc_size_t maxlen, PrintFormat* format, double value )
{
	TMC_ASSERT( dest );
	TMC_ASSERT( maxlen >= 0 );
	if( maxlen <= 0 ) {
		return 0;
	}

	tmc_size_t start = maxlen;
	if( TMC_SIGNBIT( value ) ) {
		if( maxlen <= 1 ) {
			return 0;
		}
		*dest++ = '-';
		--maxlen;
		value = -value;
	} else if( format && ( format->flags & PF_SIGN ) ) {
		if( maxlen <= 1 ) {
			return 0;
		}
		*dest++ = '+';
		--maxlen;
	}

	if( TMC_ISNAN( value ) ) {
		tmc_size_t len = start - maxlen;
		if( format && ( format->flags & PF_LOWERCASE ) ) {
			return len + print_string( dest, maxlen, "nan", 3 );
		} else {
			return len + print_string( dest, maxlen, "NAN", 3 );
		}
	}
	if( TMC_ISINF( value ) ) {
		tmc_size_t len = start - maxlen;
		if( format && ( format->flags & PF_LOWERCASE ) ) {
			return len + print_string( dest, maxlen, "inf", 3 );
		} else {
			return len + print_string( dest, maxlen, "INF", 3 );
		}
	}

	tmc_int32 precision = 6;
	if( format ) {
		precision = format->precision;
		if( precision < 0 ) {
			precision = 0;
		} else if( precision > 9 ) {
			precision = 9;
		}
	}

	// quick and dirty method to convert double value to decimal string
	// produces wrong results for very big numbers > around 1e18

	// check for rollover due to precision
	double rollover = value + str_Pow10( 0.5, -precision );
	if( (tmc_uint32)rollover > (tmc_uint32)value ) {
		value += 1;
	}

	tmc_int32 magnitude = print_magnitude( value );
	if( magnitude >= scan_MaxExponent ) {
		magnitude = scan_MaxExponent - 1;
	}
	PrintFormat uint32Format = {10};
	while( magnitude > 0 && maxlen ) {
		double part;
		if( magnitude > 9 ) {
			part = str_Pow10( value, -( magnitude - 9 ) );
			value -= str_Pow10( (tmc_uint32)part, magnitude - 9 );
		} else {
			part = value;
			value -= (double)( (tmc_uint32)value );
			uint32Format.width = magnitude;
		}
		tmc_uint32 digits = (tmc_uint32)part;
		tmc_size_t len = print_u32( dest, maxlen, &uint32Format, digits );
		uint32Format.width = 9;
		magnitude -= 9;
		maxlen -= len;
		dest += len;
	}
	double fractionalPart = value;

	if( precision ) {
		uint32Format.width = precision;
		// fractionalPart is positive, so we can round by adding 0.5 before truncating
		tmc_uint32 fractionalDigits =
			( tmc_uint32 )( ( fractionalPart * print_PowersOfTen[precision] ) + 0.5 );
		if( fractionalDigits ) {
			if( maxlen ) {
				*dest++ = '.';
				--maxlen;
				if( format && !( format->flags & PF_PADDING_ZEROES ) ) {
					// get rid of trailing zeroes
					for( ;; ) {
						tmc_uint32 digit = fractionalDigits % 10;
						if( !digit ) {
							fractionalDigits /= 10;
							--uint32Format.width;
						} else {
							break;
						}
					}
				}
				// check whether we got rid of all digits when trimming trailing zeroes
				if( uint32Format.width > 0 ) {
					tmc_size_t fractionalLen =
						print_u32( dest, maxlen, &uint32Format, fractionalDigits );
					maxlen -= fractionalLen;
					dest += fractionalLen;
				} else {
					// output a zero manually in that case
					*dest++ = '0';
					--maxlen;
				}
			}
		} else if( format && ( format->flags & PF_PADDING_ZEROES ) && maxlen > 1 ) {
			*dest++ = '.';
			--maxlen;
			while( precision-- && maxlen ) {
				*dest++ = '0';
				--maxlen;
			}
		}
	}

	return start - maxlen;
}
tmc_size_t print_float( char* dest, tmc_size_t maxlen, PrintFormat* format, float value )
{
	return print_double( dest, maxlen, format, (double)value );
}

#ifdef __cplusplus
}
#endif

#if defined( TMC_CONVENIENCE ) && defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )
tmc_size_t to_string_hex( tmc_uint32 value, char* dest, tmc_size_t maxlen )
{
	switch( maxlen ) {
		case 0: {
			return 0;
		}
		case 1: {
			dest[0] = '0';
			return 1;
		}
		case 2: {
			dest[0] = '0';
			dest[1] = 'x';
			return 2;
		}
	}
	dest[0] = '0';
	dest[1] = 'x';
	PrintFormat format = PrintFormat{16};
	return print_u32( dest + 2, maxlen - 2, &format, value ) + 2;
}
tmc_size_t to_string_hex( tmc_uint64 value, char* dest, tmc_size_t maxlen )
{
	switch( maxlen ) {
		case 0: {
			return 0;
		}
		case 1: {
			dest[0] = '0';
			return 1;
		}
		case 2: {
			dest[0] = '0';
			dest[1] = 'x';
			return 2;
		}
	}
	dest[0] = '0';
	dest[1] = 'x';
	PrintFormat format = PrintFormat{16};
	return print_u64( dest + 2, maxlen - 2, &format, value ) + 2;
}
#endif // defined( TMC_CONVENIENCE ) && defined( TMC_CPP_OVERLOADS ) && defined( __cplusplus )

#endif  // TM_CONVERSION_IMPLEMENTATION
