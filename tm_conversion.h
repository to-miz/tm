/*
tm_conversion.h v0.9 - public domain
author: Tolga Mizrak 2016

no warranty; use at your own risk

USAGE
	This file works as both the header and implementation.
	To implement the interfaces in this header,
		#define TM_CONVERSION_IMPLEMENTATION
	in ONE C or C++ source file before #including this header.

ISSUES
	- PF_SCIENTIFIC does nothing at the moment
	- print_double, print_float don't produce correct decimal strings for values above/below +-1e19.
	This is because we use floating arithmetic and we lose too much precision when dealing with
	very big numbers. This could be fixed by using arbitrary precision math, but that pretty much
	is beyond the scope of this library. Do not use these functions for big numbers.

HISTORY
	v0.9 23.06.16 initial commit

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.
*/

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
		#include <memory.h>
		#define TMC_MEMCPY memcpy
	#endif

	#ifndef TMC_MEMSET
		#include <memory.h>
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

	#ifndef TMC_STRNCASECMP
		#define TMC_STRNCASECMP scan_strncasecmp
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
These functions scan a value from a string and store it into out.
Args:
	nullterminated: a nullterminated string
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

enum PrintFormatFlags TMC_UNDERLYING_U32 {
	PF_DEFAULT = 0,
	PF_SIGN = ( 1 << 0 ),
	PF_LOWERCASE = ( 1 << 1 ),
	PF_PADDING_ZEROES = ( 1 << 2 ),
	PF_BOOL_AS_NUMBER = ( 1 << 3 ),
	PF_SCIENTIFIC = ( 1 << 4 ),  // not implemented yet
};

typedef struct {
	tmc_int32 base;
	tmc_int32 precision;
	tmc_int32 width;
	tmc_uint32 flags;
} PrintFormat;

/*
These functions print a value into a destination buffer.
Args:
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


#ifdef __cplusplus
TMC_DEF tmc_size_t scan_bool( const char* nullterminated, bool* out );
TMC_DEF tmc_size_t scan_bool_n( const char* str, tmc_size_t len, bool* out );

TMC_DEF tmc_size_t print_bool( char* dest, tmc_size_t maxlen, PrintFormat* format, bool value );
#endif

#ifdef __cplusplus
}
#endif

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
	if( sign && dist == 1 ) {
		// we only scanned a sign character, no actual value
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
	if( sign && dist == 1 ) {
		// we only scanned a sign character, no actual value
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
	if( sign && dist == 1 ) {
		// we only scanned a sign character, no actual value
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
	if( sign && dist == 1 ) {
		// we only scanned a sign character, no actual value
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
	if( sign && dist == 1 ) {
		// we only scanned a sign character, no actual value
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
	if( sign && dist == 1 ) {
		// we only scanned a sign character, no actual value
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
	if( sign && dist == 1 ) {
		// we only scanned a sign character, no actual value
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
	if( sign && dist == 1 ) {
		// we only scanned a sign character, no actual value
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
static tmc_int32 scan_strncasecmp( const char* a, const char* b, tmc_size_t maxlen )
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

TMC_DEF tmc_size_t scan_bool( const char* nullterminated, bool* out )
{
	if( *nullterminated == '1' ) {
		if( out ) {
			*out = true;
		}
		return 1;
	} else if( *nullterminated == '0' ) {
		if( out ) {
			*out = false;
		}
		return 1;
	} else if( TMC_STRNCASECMP( nullterminated, "true", 4 ) == 0 ) {
		if( out ) {
			*out = true;
		}
		return 4;
	} else if( TMC_STRNCASECMP( nullterminated, "false", 5 ) == 0 ) {
		if( out ) {
			*out = false;
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
			*out = true;
		}
		return 1;
	} else if( *str == '0' ) {
		if( out ) {
			*out = false;
		}
		return 1;
	} else if( TMC_STRNCASECMP( str, "true", len ) == 0 ) {
		if( out ) {
			*out = true;
		}
		return 4;
	} else if( TMC_STRNCASECMP( str, "false", len ) == 0 ) {
		if( out ) {
			*out = false;
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
static tmc_int32 print_log10( double value )
{
	tmc_int32 n = 1;
	if( value < 0 ) {
		value = -value;
	}
	while( value > 10 ) {
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
	return n;
}

static void print_Reverse( char* dest, tmc_size_t l )
{
	size_t len = (size_t)l;
	size_t mid = len / 2;
	for( size_t i = 0; i < mid; ++i ) {
		size_t other = len - i - 1;
		char tmp = dest[i];
		dest[i] = dest[other];
		dest[other] = tmp;
	}
}

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
		print_Reverse( dest, len );
	} else {
		print_Reverse( dest, len - 1 );
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
		print_Reverse( dest, len );
	} else {
		print_Reverse( dest, len - 1 );
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

	tmc_int32 magnitude = print_log10( value );
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

	// FIXME: rollover for case 0.99 and precision = 1 doesn't result in 1.0
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
				tmc_size_t fractionalLen =
					print_u32( dest, maxlen, &uint32Format, fractionalDigits );
				maxlen -= fractionalLen;
				dest += fractionalLen;
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

#endif  // TM_CONVERSION_IMPLEMENTATION
