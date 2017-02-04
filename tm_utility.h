/*
tm_utility v1.1.2 - public domain
written by Tolga Mizrak 2016

USAGE
	This file works as both the header and implementation.
	To implement the interfaces in this header,
		#define TM_UTILITY_IMPLEMENTATION
	in ONE C or C++ source file before #including this header.

NOTES
	Utility and convenience functions that I use in my private projects.
	Mostly just overloads for safety and some operator like functions.
	See comments at declarations for more info.

HISTORY
	v1.1.2  24.01.17 changed clamp to take two different types as min and max
	v1.1.1a 02.10.16 fixed a minor warning of unused variable when asserts are not used
	v1.1.1  02.10.16 added TMUT_NO_CSTYLE_TAGGED_UNION_MACROS and c-style tagged union macros
	v1.1.0a 15.09.16 added an alternate implementation of countof
	v1.1.0  12.09.16 added isAligned
	                 added versions of min/max/minmax/median that take a Compare functor
	v1.0.9a 24.08.16 changed countof to use tmut_size_t
	v1.0.9  24.08.16 added countof, lerp and remap
	v1.0.8  23.08.16 changed min and max to use operator < instead of operator <= and added minmax
	                 removed assertion in strnicmp, because count >= 0 is always true
	v1.0.7  12.08.16 added mapToRange
	v1.0.6a 10.08.16 added paranthesis to getAlignmentOffset to make it clear that "-" has
	                 precedence over "&"
	v1.0.6  15.07.16 fixed floatEqZero & floatEqZeroSoft, they now use TMUT_ABS instead of abs
	                 added floatEqSoft & floatEq
	                 changed toleranceComparison name to floatToleranceComparison
	                 changed SCOPED macro to take __VA_ARGS__ instead of a single expression
	v1.0.5a  12.07.16 fixed a bug in promote_as_is_to, template arguments were reversed
	v1.0.5   12.07.16 added min_3/max_3 and median
	v1.0.4a  12.07.16 fixed bug with min/max where both would return the same value on equality
	v1.0.4   12.07.16 added WITH and SCOPED macros
	v1.0.3   10.07.16 added unsignedof and promote_as_is_to
	v1.0.2   10.07.16 added swap, alignment, isPowerOfTwo, isMemoryZero
	v1.0.1   10.07.16 added crt extension functions
	v1.0a    10.07.16 added flags, float, endian related stuff
	v1.0     10.07.16 initial commit

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.
*/

#ifdef TM_UTILITY_IMPLEMENTATION
	// define TMUT_IMPLEMENT_CTYPE_FUNCTIONS to implement the ctype.h family of functions with c++
	// overloads
	#ifndef TMUT_IMPLEMENT_CTYPE_FUNCTIONS
		#ifndef TMUT_TOUPPER
			#include <cctype>
			#define TMUT_TOUPPER toupper
		#endif
	#else
		#define TMUT_TOUPPER toupper
	#endif

	#ifndef TMUT_STRLEN
		#include <cstring>
		#define TMUT_STRLEN strlen
	#endif
#endif

#ifndef _TM_UTILITY_H_INCLUDED_
#define _TM_UTILITY_H_INCLUDED_

#ifndef TM_USE_OWN_BEGIN_END
	#include <iterator>  // std::begin/std::end
#else
// begin/end implementation for basic containers, if including iterator isn't an option,
// on MSVC including iterator generates warnings if exception handling is turned off
	template < class Container >
	inline auto begin( Container& container ) -> decltype( container.begin() )
	{
		return container.begin();
	}
	template < class Container >
	inline auto end( Container& container ) -> decltype( container.end() )
	{
		return container.end();
	}
	template < class Container >
	inline auto begin( const Container& container ) -> decltype( container.begin() )
	{
		return container.begin();
	}
	template < class Container >
	inline auto end( const Container& container ) -> decltype( container.end() )
	{
		return container.end();
	}
	template < class T, size_t N >
	inline T* begin( T( &container )[N] )
	{
		return container;
	}
	template < class T, size_t N >
	inline T* end( T( &container )[N] )
	{
		return container + N;
	}
#endif

#include <type_traits>  // std::underlying_type, std::remove_reference

// define these if you don't use crt and have your own versions of these functions
#ifndef TMUT_MEMCPY
	#include <cstring>
	#define TMUT_MEMCPY memcpy
#endif
#ifndef TMUT_MEMSET
	#include <cstring>
	#define TMUT_MEMSET memset
#endif
#ifndef TMUT_MEMCMP
	#include <cstring>
	#define TMUT_MEMCMP memcmp
#endif
#ifndef TMUT_MEMMOVE
	#include <cstring>
	#define TMUT_MEMMOVE memmove
#endif
#ifndef TMUT_ASSERT
	#include <cassert>
	#define TMUT_ASSERT assert
#endif
#ifndef TMUT_ABS
	#include <cmath>
	#define TMUT_ABS fabsf
#endif
#ifndef TMUT_MOVE
	#include <utility>
	#define TMUT_MOVE std::move
#endif

// used to see if we are 64bit or 32bit
#ifndef TMUT_POINTER_SIZE
	#include <cstdint>
	#if INTPTR_MAX == INT64_MAX
		#define TMUT_POINTER_SIZE 8
	#elif INTPTR_MAX == INT32_MAX
		#define TMUT_POINTER_SIZE 4
	#endif
#endif

// define this if you use another type as your size type, like int
#ifndef TMUT_OWN_TYPES
	typedef size_t tmut_size_t;
	// tmut_uintptr needs to be an unsigned integral as big as a pointer
	typedef size_t tmut_uintptr;
#endif

static_assert( sizeof( tmut_uintptr ) == sizeof( void* ), "uintptr is not pointer size" );

// compatibility macros
#ifdef _MSC_VER
	#if _MSC_VER <= 1800
		#define TMUT_ALIGNOF( x ) __alignof( x )
		#define TMUT_CONSTEXPR
	#else
		#define TMUT_ALIGNOF( x ) alignof( x )
		#define TMUT_CONSTEXPR constexpr
	#endif
#else
	// TODO: more compilers
	#define TMUT_CONSTEXPR constexpr
	#define TMUT_ALIGNOF( x ) alignof( x )
#endif

namespace utility
{
// function versions of min/max
template< class T >
inline TMUT_CONSTEXPR const T& min( const T& a, const T& b ) { return ( a < b ) ? ( a ) : ( b ); }
template< class T >
inline TMUT_CONSTEXPR const T& max( const T& a, const T& b ) { return ( a < b ) ? ( b ) : ( a ); }

template< class T >
inline TMUT_CONSTEXPR const T& min( const T& a, const T& b, const T& c ) { return min( a, min( b, c ) ); }
template< class T >
inline const T& max( const T& a, const T& b, const T& c ) { return max( max( a, b ), c ); }

// function versions of min/max taking a compare functor
template< class T, class Compare >
inline TMUT_CONSTEXPR const T& min( const T& a, const T& b, Compare comp ) { return comp( a, b ) ? ( a ) : ( b ); }
template< class T, class Compare >
inline TMUT_CONSTEXPR const T& max( const T& a, const T& b, Compare comp ) { return comp( a, b ) ? ( b ) : ( a ); }

template < class T, class Compare >
inline TMUT_CONSTEXPR const T& min( const T& a, const T& b, const T& c, Compare comp )
{
	return min( a, min( b, c, comp ), comp );
}
template < class T, class Compare >
inline TMUT_CONSTEXPR const T& max( const T& a, const T& b, const T& c, Compare comp )
{
	return max( max( a, b, comp ), c, comp );
}

template< class T >
struct MinMaxPair {
	T first;
	T second;
};

template < class T >
inline TMUT_CONSTEXPR MinMaxPair< const T& > minmax( const T& a, const T& b )
{
	return ( a < b ) ? ( MinMaxPair< const T& >{a, b} ) : ( MinMaxPair< const T& >{b, a} );
}

template < class T, class Compare >
inline TMUT_CONSTEXPR MinMaxPair< const T& > minmax( const T& a, const T& b, Compare comp )
{
	return comp( a, b ) ? ( MinMaxPair< const T& >{a, b} ) : ( MinMaxPair< const T& >{b, a} );
}

template < class T >
inline MinMaxPair< const T& > minmax( const T& a, const T& b, const T& c )
{
	MinMaxPair< const T& > mm = minmax( a, c );
	if( b < mm.first ) {
		return MinMaxPair< const T& >{b, mm.second};
	} else if( mm.second < b ) {
		return MinMaxPair< const T& >{mm.first, b};
	}
	return mm;
}

template < class T, class Compare >
inline MinMaxPair< const T& > minmax( const T& a, const T& b, const T& c, Compare comp )
{
	MinMaxPair< const T& > mm = minmax( a, c, comp );
	if( comp( b, mm.first ) ) {
		return MinMaxPair< const T& >{b, mm.second};
	} else if( mm.second < b ) {
		return MinMaxPair< const T& >{mm.first, b};
	}
	return mm;
}

template < class T >
inline const T& median( const T& a, const T& b, const T& c )
{
	MinMaxPair< const T& > mm = minmax( a, b );
	if( mm.second < c ) {
		return mm.second;
	} else if( mm.first < c ) {
		return c;
	}
	return mm.first;
}
template < class T, class Compare >
inline const T& median( const T& a, const T& b, const T& c, Compare comp )
{
	MinMaxPair< const T& > mm = minmax( a, b, comp );
	if( comp( mm.second, c ) ) {
		return mm.second;
	} else if( comp( mm.min, c ) ) {
		return c;
	}
	return mm.min;
}
}

// min/max macros
#ifndef TMUT_NO_MINMAX
	#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
	#define MAX( a, b ) ( ( ( a ) < ( b ) ) ? ( b ) : ( a ) )
#endif

#ifndef TMUT_NO_SWAP
	template< class T >
	void swap( T& a, T& b )
	{
		auto tmp = TMUT_MOVE( a );
		a = TMUT_MOVE( b );
		b = TMUT_MOVE( tmp );
	}
#endif

TMUT_CONSTEXPR inline bool isPowerOfTwo( unsigned int x ) { return x && !( x & ( x - 1 ) ); }

// returns how much ptr needs to be offset to be aligned to alignment
unsigned int getAlignmentOffset( const void* ptr, unsigned int alignment );

template< class T >
TMUT_CONSTEXPR bool isAligned( const T* p, unsigned int alignment = TMUT_ALIGNOF( T ) )
{
	return ( (tmut_uintptr)p % alignment ) == 0;
}
#define assert_alignment( ptr, alignment ) TMUT_ASSERT( isAligned( ptr, alignment ) )

// cast the bit representation to another type
// useful to get bit representation of a float or double into an unsigned integral
template < class result_type, class value_type >
result_type bit_cast( const value_type& value );

// return index of value in contiguous container
template < class Container, class ValueType, class ReturnType = tmut_size_t >
ReturnType indexof( const Container& container, const ValueType& value );

// get type of value without it being a reference type
#define typeof( value ) std::remove_reference < decltype( value ) > ::type

// get enum value as underlying type
// Expample:
//	enum class MyEnum : char { A, B, C };
//	MyEnum val = MyEnum::C;
//	valueof( val ) == (char)2
template < class EnumType >
TMUT_CONSTEXPR inline typename std::underlying_type< EnumType >::type valueof( EnumType value )
{
	return static_cast< typename std::underlying_type< EnumType >::type >( value );
}

// countof returns the element count of static arrays
// enable typechecked version of countof on debug builds or if TMUT_SAFE_COUNTOF is defined
#if ( defined( TMUT_SAFE_COUNTOF ) || defined( _DEBUG ) ) && !defined( TMUT_SAFE_COUNTOF_ALT )
	// implementation of countof that uses the comma operator to validate
	// has the benefit that the return type is a const literal and thus will not generate truncation
	// warnings when assigned to smaller integers, if the size fits without truncation
	template < class T >
	TMUT_CONSTEXPR inline void internal_is_array( const T& v )
	{
		static_assert( std::is_array< T >::value, "not static array" );
	}
	#define countof( x ) ( internal_is_array( ( x ) ), sizeof( ( x ) ) / sizeof( *( x ) ) )
#elif defined( TMUT_SAFE_COUNTOF_ALT ) || defined( _DEBUG )
	// alternate implementation of countof that always returns the size in tmut_size_t
	template< class T, tmut_size_t N >
	TMUT_CONSTEXPR tmut_size_t countof( T (&array)[N] )
	{
		return N;
	}
	template < class T >
	tmut_size_t countof( T )
	{
		static_assert( false, "countof can only be applied on static arrays" );
		return 0;
	}
#else
	#define countof( x ) ( sizeof( x ) / sizeof( x[0] ) )
#endif

/* macros to work with c-style tagged unions, but with c++ enabled safety
define a tagged union like this:
struct tagged_union {
	enum { type_entry0, type_entry1 } type; // enum entries have the exact same name as the
	                                        // union entries, but prefixed with "type_"
	union {
		Entry0 entry0;
		Entry1 entry1;
	};
};

Usage:
	tagged_union tagged = ...;

	// set currently selected entry + initialize
	set_variant( tagged, entry0 ) = ...;
	// set currently selected entry and get reference
	auto& entry0 = set_variant( tagged, entry0 );
	// get entry by name, will runtime assert that it is actually selected
	auto& entry0 = get_variant( tagged, entry0 );
	// query entry by name, will return nullptr if it is not selected
	auto entry0 = query_variant( tagged, entry0 );
	// get entry by name or default, will return default if entry is not selected
	auto entry0 = get_variant_or_default( tagged, entry0, {} );
*/
#ifndef TMUT_NO_CSTYLE_TAGGED_UNION_MACROS
	#define get_variant( tagged, name ) \
		( assert( ( tagged ).type == typeof( tagged )::type_##name ), ( tagged ).name )
	#define query_variant( tagged, name ) \
		( ( ( tagged ).type == typeof( tagged )::type_##name ) ? ( &(tagged).name ) : ( nullptr ) )
	#define set_variant( tagged, name ) \
		( ( ( tagged ).type = typeof( tagged )::type_##name ), ( tagged ).name )
	#define get_variant_or_default( tagged, name, def ) \
		( ( ( tagged ).type == typeof( tagged )::type_##name ) ? ( ( tagged ).name ) : ( def ) )
#endif

// returns value as unsigned version of its type
template< class ValueType >
auto unsignedof( ValueType value ) -> typename std::make_unsigned< ValueType >::type;

// promotes value to ResultType without sign extension
// ie promoting a char -1 to unsigned int will result in 0x000000FF
template< class ResultType, class ValueType >
ResultType promote_as_is_to( ValueType value );

// copy count elements of src into dest
template < class T >
void copy( T* dest, const void* src, size_t count );
// move count elements of src into dest
template < class T >
void move( T* dest, const void* src, size_t count );
// fill count elements of dest with value
template < class T >
void fill( T* dest, const T& value, size_t count );
void fill( char* dest, char value, size_t count );
void fill( signed char* dest, signed char value, size_t count );
void fill( unsigned char* dest, unsigned char value, size_t count );
bool isMemoryZero( char* ptr, size_t size );

// compare count elements of a and b
template < class T >
int compare( const T* a, const T* b, size_t count );
// zero memory of count elements in dest
template < class T >
void zeroMemory( T* dest, size_t count );

// return clamped value in [lower, upper]
template < class T, class A, class B >
T clamp( T val, A lower, B upper );
// used most often with float, so a simple overload with default values
float clamp( float val, float lower = 0, float upper = 1 );

#ifndef TMUT_NO_LERP
	// lerps using t in [0, 1]
	template< class T >
	T lerp( float t, T a, T b )
	{
		return a + t * ( b - a );
	}
#endif

#ifndef TMUT_NO_REMAP
	// remaps t in [tMin,tMax] to [a,b], basically lerping by normalizing t first
	template < class T >
	T remap( float t, float tMin, float tMax, T a, T b )
	{
		return static_cast< T >( a + ( ( t - tMin ) / ( tMax - tMin ) ) * ( b - a ) );
	}
#endif

// returns a/b if b != 0, def otherwise
float safeDivide( float a, float b, float def = 0.0f );

// returns value mapped to [min, max) almost fairly
inline int mapToRange( int value, int min, int max )
{
#if TMUT_POINTER_SIZE == 8
	// 64bit architecture
	// use 64bit multiplication followed by a rightshift
	typedef unsigned long long uint64;
	typedef unsigned int uint32;
	return (int)( (uint64)( (uint32)value ) * (uint64)( (uint32)( max - min ) ) >> 32 ) + min;
#elif TMUT_POINTER_SIZE == 4
	// 32bit architecture
	return value % ( max - min ) + min;
#else
	#error unknown pointer size
#endif
}

// flags
#define setFlag( flags, flag_to_set ) ( flags ) |= ( flag_to_set )
#define clearFlag( flags, flag_to_clear ) ( flags ) &= ~( flag_to_clear )
#define setFlagCond( flags, flag_to_set, cond )  \
	if( cond ) {                                 \
		setFlag( ( flags ), ( flag_to_set ) );   \
	} else {                                     \
		clearFlag( ( flags ), ( flag_to_set ) ); \
	}
#define toggleFlag( flags, flag_to_toggle ) ( flags ) ^= ( flag_to_toggle )
#define isFlagSet( flags, flag_to_check ) ( ( ( flags ) & ( flag_to_check ) ) != 0 )

// float
namespace Float
{
static const float Epsilon        = 0.000001f;  // error tolerance for check
static const float BigEpsilon     = 0.001f;     // bigger error tolerance
static const int DecimalTolerance = 6;          // decimal places for float rounding
static const float AbsTolerance   = 0.000001f;
static const float RelTolerance   = 0.000001f;
}
// set float to 0 if within tolerance
#define floatZeroClamp( x ) ( ( TMUT_ABS( x ) < Float::Epsilon ) ? ( 0.0f ) : ( x ) )
// zero tests
#define floatEqZero( x ) ( TMUT_ABS( x ) < Float::Epsilon )
#define floatEqZeroSoft( x ) ( TMUT_ABS( x ) < Float::BigEpsilon )
#define floatGtZeroSoft( x ) ( ( x ) > Float::BigEpsilon )
#define floatLtZeroSoft( x ) ( ( x ) < Float::BigEpsilon )
#define floatEqZeroSoft( x ) ( TMUT_ABS( x ) < Float::BigEpsilon )
// NOTE: you should know when it is appropriate to use the following two macros, if you are not sure
// use floatToleranceComparison instead
#define floatEq( a, b ) floatEqZero( ( a ) - ( b ) )
#define floatEqSoft( a, b ) floatEqZeroSoft( ( a ) - ( b ) )
bool floatToleranceComparison( float a, float b );

// endian
inline char swapEndian( char val ) { return val; }
inline signed char swapEndian( signed char val ) { return val; }
inline unsigned char swapEndian( unsigned char val ) { return val; }
short swapEndian( short val );
unsigned short swapEndian( unsigned short val );
int swapEndian( int val );
unsigned int swapEndian( unsigned int val );
long long swapEndian( long long val );
unsigned long long swapEndian( unsigned long long val );
float swapEndian( float val );
double swapEndian( double val );

/* WITH macro, enables usage of scoped RAII resources
	Example usage:
	// create a RAII container for the resource
	struct FileScopedResource {
		FILE* file;
		FileScopedResource( FILE* file ) : file( file ) {};
		~FileScopedResource() { if( file ) fclose( file ); }
	};
	// overload makeScopedResource with your resource type
	FileScopedResource makeScopedResource( FILE* file )
	{
		return FileScopedResource( file );
	}

	// later in code:
	WITH( file, fopen( "foo", "w" ) ) {
		fwrite( "bar", 3, 1, file );
		// file will be closed automatically at the end of the scope
	}
*/
#define WITH( name, expression )                              \
	if( bool once_ = false ) {                                \
	} else                                                    \
		for( auto&& name = expression; !once_; once_ = true ) \
			for( auto scoped_ = makeScopedResource( ( name ) ); !once_; once_ = true )

// SCOPED macro, similar to WITH, but doesn't need a named variable
// Useful for begin/end blocks like glBegin
#define SCOPED( ... )   \
	if( bool once_ = false ) { \
	} else                     \
		for( auto scoped_ = makeScopedResource( __VA_ARGS__ ); !once_; once_ = true )

// misc macros
#define TMUT_ARG_2_OR_1_IMPL( a, b, ... ) b
#define TMUT_ARG_1( a, ... ) a
#define TMUT_ARG_2_OR_1( a, b, ... ) TMUT_ARG_2_OR_1_IMPL( __VA_ARGS__, __VA_ARGS__ )

// crt extensions, that are non standard and may not be provided
extern "C" {
#ifndef TMUT_NO_STRICMP
	extern int stricmp( const char* a, const char* b );
#endif
#ifndef TMUT_NO_STRNICMP
	extern int strnicmp( const char* a, const char* b, size_t count );
#endif

#ifndef TMUT_NO_STRREV
	extern char* strrev( char* str );
#endif
#ifndef TMUT_NO_STRNREV
	extern char* strnrev( char* str, size_t count );
#endif

#ifndef TMUT_NO_MEMRCHR
	extern void* memrchr( const void* ptr, int value, size_t len );
#endif
}

#define TMUT_WHITESPACE " \t\n\v\f\r"
namespace utility
{
const char whitespace[6] = {' ', '\t', '\n', '\v', '\f', '\r'};
}

// unsigned int versions
TMUT_CONSTEXPR inline int isspace( unsigned int cp )
{
	return cp == ' ' || cp == '\t' || cp == '\n' || cp == '\v' || cp == '\f' || cp == '\r';
}
TMUT_CONSTEXPR inline int isnewline( unsigned int cp ) { return cp == '\n' || cp == '\r'; }
TMUT_CONSTEXPR inline int isdigit( unsigned int cp ) { return cp >= '0' && cp <= '9'; }
TMUT_CONSTEXPR inline int ishex( unsigned int cp )
{
	return ( cp >= 'a' && cp <= 'f' ) || ( cp >= 'A' && cp <= 'F' );
}
TMUT_CONSTEXPR inline int isxdigit( unsigned int cp )
{
	return ( cp >= '0' && cp <= '9' ) || ( cp >= 'a' && cp <= 'f' ) || ( cp >= 'A' && cp <= 'F' );
}
TMUT_CONSTEXPR inline int isupper( unsigned int cp ) { return cp >= 'A' && cp <= 'Z'; }
TMUT_CONSTEXPR inline int islower( unsigned int cp ) { return cp >= 'a' && cp <= 'z'; }
TMUT_CONSTEXPR inline int isalpha( unsigned int cp )
{
	return ( cp >= 'a' && cp <= 'z' ) || ( cp >= 'A' && cp <= 'Z' );
}
TMUT_CONSTEXPR inline unsigned int toupper( unsigned int cp )
{
	return ( cp >= 'a' && cp <= 'z' ) ? ( 'A' + ( cp - 'a' ) ) : cp;
}
TMUT_CONSTEXPR inline unsigned int tolower( unsigned int cp )
{
	return ( cp >= 'A' && cp <= 'Z' ) ? ( 'a' + ( cp - 'A' ) ) : cp;
}

#ifdef TMUT_IMPLEMENT_CTYPE_FUNCTIONS
// int versions
TMUT_CONSTEXPR inline int isspace( int cp )
{
	return cp == ' ' || cp == '\t' || cp == '\n' || cp == '\v' || cp == '\f' || cp == '\r';
}
TMUT_CONSTEXPR inline int isnewline( int cp ) { return cp == '\n' || cp == '\r'; }
TMUT_CONSTEXPR inline int isdigit( int cp ) { return cp >= '0' && cp <= '9'; }
TMUT_CONSTEXPR inline int ishex( int cp )
{
	return ( cp >= 'a' && cp <= 'f' ) || ( cp >= 'A' && cp <= 'F' );
}
TMUT_CONSTEXPR inline int isxdigit( int cp )
{
	return ( cp >= '0' && cp <= '9' ) || ( cp >= 'a' && cp <= 'f' ) || ( cp >= 'A' && cp <= 'F' );
}
TMUT_CONSTEXPR inline int isupper( int cp ) { return cp >= 'A' && cp <= 'Z'; }
TMUT_CONSTEXPR inline int islower( int cp ) { return cp >= 'a' && cp <= 'z'; }
TMUT_CONSTEXPR inline int isalpha( int cp )
{
	return ( cp >= 'a' && cp <= 'z' ) || ( cp >= 'A' && cp <= 'Z' );
}
TMUT_CONSTEXPR inline int toupper( int cp )
{
	return ( cp >= 'a' && cp <= 'z' ) ? ( 'A' + ( cp - 'a' ) ) : cp;
}
TMUT_CONSTEXPR inline int tolower( int cp )
{
	return ( cp >= 'A' && cp <= 'Z' ) ? ( 'a' + ( cp - 'A' ) ) : cp;
}
#endif

// can't cast directly to uint/int because char is signed and negative c would result in a very
// large positive number not in range of 0 - 255
TMUT_CONSTEXPR inline unsigned int char_to_uint( char c )
{
	return (unsigned int)( (unsigned char)c );
}
TMUT_CONSTEXPR inline int char_to_int( char c ) { return (int)( (unsigned char)c ); }

//////////////////////////////////////////////////////////////////
// inline implementation

template < class result_type, class value_type >
inline result_type bit_cast( const value_type& value )
{
	static_assert( sizeof( result_type ) == sizeof( value_type ), "bit_cast: size mismatch" );

	result_type ret;
	TMUT_MEMCPY( &ret, &value, sizeof( ret ) );
	return ret;
}

template < class Container, class ValueType, class ReturnType >
inline ReturnType indexof( const Container& container, const ValueType& value )
{
#ifndef TM_USE_OWN_BEGIN_END
	using std::begin;
	using std::end;
#endif
	auto first = begin( container );
	// TODO: to be perfectly correct, we'd have to use std::addressof here instead of &
	// but then again, don't overload addressof & operator
	TMUT_ASSERT( &value >= &*first && &value < &*( end( container ) ) );
	// we take the adress after dereferencing in case we are dealing with an iterator type instead
	// of pointers
	return static_cast< ReturnType >( &value - &*first );
}

template< class ValueType >
inline auto unsignedof( ValueType value ) -> typename std::make_unsigned< ValueType >::type
{
	return static_cast< typename std::make_unsigned< ValueType >::type >( value );
}

template < class ResultType, class ValueType >
inline ResultType promote_as_is_to( ValueType value )
{
	static_assert( sizeof( ResultType ) >= sizeof( ValueType ),
				   "truncating not allowed when promoting" );
	return static_cast< ResultType >( unsignedof( value ) );
}

template < class T, class A, class B >
inline T clamp( T val, A lower, B upper )
{
	return ( val < lower ) ? ( lower ) : ( ( val > upper ) ? ( upper ) : ( val ) );
}
inline float clamp( float val, float lower, float upper )
{
	return ( val < lower ) ? ( lower ) : ( ( val > upper ) ? ( upper ) : ( val ) );
}

inline float safeDivide( float a, float b, float def )
{
	return ( b == 0.0f ) ? ( def ) : ( a / b );
}

template < class T >
inline void copy( T* dest, const void* src, size_t count )
{
	TMUT_ASSERT( isAligned( dest ) );
	TMUT_MEMCPY( dest, src, count * sizeof( T ) );
}

template < class T >
inline void move( T* dest, const void* src, size_t count )
{
	TMUT_ASSERT( isAligned( dest ) );
	TMUT_MEMMOVE( dest, src, count * sizeof( T ) );
}

template < class T >
inline void fill( T* dest, const T& value, size_t count )
{
	TMUT_ASSERT( isAligned( dest ) );
	for( ; count > 0; --count, ++dest ) {
		*dest = value;
	}
}
inline void fill( char* dest, char value, size_t count )
{
	// cast because we don't need sign extension
	TMUT_MEMSET( dest, static_cast< unsigned char >( value ), count );
}
inline void fill( signed char* dest, signed char value, size_t count )
{
	// cast because we don't need sign extension
	TMUT_MEMSET( dest, static_cast< unsigned char >( value ), count );
}
inline void fill( unsigned char* dest, unsigned char value, size_t count )
{
	TMUT_MEMSET( dest, value, count );
}
inline bool isMemoryZero( char* ptr, size_t size )
{
	TMUT_ASSERT( ptr );
	TMUT_ASSERT( size );
	return *ptr == 0 && memcmp( ptr, ptr + 1, size - 1 ) == 0;
}

template < class T >
inline int compare( const T* a, const T* b, size_t count )
{
	return TMUT_MEMCMP( a, b, count * sizeof( T ) );
}

template < class T >
inline void zeroMemory( T* dest, size_t count )
{
	TMUT_ASSERT( isAligned( dest ) );
	TMUT_MEMSET( dest, 0, count * sizeof( T ) );
}

// float
inline bool floatToleranceComparison( float a, float b )
{
	if( a == b ) {
		return true;
	}
	using utility::max;
	return ( TMUT_ABS( a - b ) <= Float::AbsTolerance )
		   || ( TMUT_ABS( a - b ) <= Float::RelTolerance * max( TMUT_ABS( a ), TMUT_ABS( b ) ) );
}

#endif  // _TM_UTILITY_H_INCLUDED_

//////////////////////////////////////////////////////////////////
// implementation

#ifdef TM_UTILITY_IMPLEMENTATION

unsigned int getAlignmentOffset( const void* ptr, unsigned int alignment )
{
	assert( alignment != 0 && isPowerOfTwo( alignment ) );
	unsigned int alignmentOffset = ( alignment - ( (tmut_uintptr)ptr ) ) & ( alignment - 1 );
	assert_alignment( (char*)ptr + alignmentOffset, alignment );
	return alignmentOffset;
}

// endian

short swapEndian( short val )
{
	typedef unsigned short u16;
	static_assert( sizeof( u16 ) == 2, "size mismatch" );
	return (short)( ( ( (u16)val & (u16)0x00ffu ) << 8 ) | ( ( (u16)val & (u16)0xff00u ) >> 8 ) );
}
unsigned short swapEndian( unsigned short val )
{
	typedef unsigned short u16;
	static_assert( sizeof( u16 ) == 2, "size mismatch" );
	return ( u16 )( ( ( val & (u16)0x00ffu ) << 8 ) | ( ( val & (u16)0xff00u ) >> 8 ) );
}
int swapEndian( int val )
{
	typedef unsigned int u32;
	static_assert( sizeof( u32 ) == 4, "size mismatch" );
	return (int)( ( ( (u32)val & 0x000000FFu ) << 24 ) | ( ( (u32)val & 0x0000FF00u ) << 8 )
				  | ( ( (u32)val & 0x00FF0000u ) >> 8 ) | ( ( (u32)val & 0xFF000000u ) >> 24 ) );
}
unsigned int swapEndian( unsigned int val )
{
	typedef unsigned int u32;
	static_assert( sizeof( u32 ) == 4, "size mismatch" );
	return (u32)( ( ( val & 0x000000FFu ) << 24 ) | ( ( val & 0x0000FF00u ) << 8 )
				  | ( ( val & 0x00FF0000u ) >> 8 ) | ( ( val & 0xFF000000u ) >> 24 ) );
}
long long swapEndian( long long val )
{
	typedef unsigned long long u64;
	static_assert( sizeof( u64 ) == 8, "size mismatch" );
	return (long long)( ( (u64)val & 0x00000000000000FFull ) << 56 )
		   | ( ( (u64)val & 0x000000000000FF00ull ) << 40 )
		   | ( ( (u64)val & 0x0000000000FF0000ull ) << 24 )
		   | ( ( (u64)val & 0x00000000FF000000ull ) << 8 )
		   | ( ( (u64)val & 0x000000FF00000000ull ) >> 8 )
		   | ( ( (u64)val & 0x0000FF0000000000ull ) >> 24 )
		   | ( ( (u64)val & 0x00FF000000000000ull ) >> 40 )
		   | ( ( (u64)val & 0xFF00000000000000ull ) >> 56 );
}
unsigned long long swapEndian( unsigned long long val )
{
	typedef unsigned long long u64;
	static_assert( sizeof( u64 ) == 8, "size mismatch" );
	return (u64)( ( val & 0x00000000000000FFull ) << 56 )
		   | ( ( val & 0x000000000000FF00ull ) << 40 ) | ( ( val & 0x0000000000FF0000ull ) << 24 )
		   | ( ( val & 0x00000000FF000000ull ) << 8 ) | ( ( val & 0x000000FF00000000ull ) >> 8 )
		   | ( ( val & 0x0000FF0000000000ull ) >> 24 ) | ( ( val & 0x00FF000000000000ull ) >> 40 )
		   | ( ( val & 0xFF00000000000000ull ) >> 56 );
}
float swapEndian( float val )
{
	static_assert( sizeof( float ) == sizeof( unsigned int ), "size mismatch" );
	return bit_cast< float >( swapEndian( bit_cast< unsigned int >( val ) ) );
}
double swapEndian( double val )
{
	static_assert( sizeof( double ) == sizeof( unsigned long long ), "size mismatch" );
	return bit_cast< double >( swapEndian( bit_cast< unsigned long long >( val ) ) );
}

// crt extensions
extern "C"{
#ifndef TMUT_NO_STRICMP
extern int stricmp( const char* a, const char* b )
{
	while( *a && *b ) {
		auto aUpper = TMUT_TOUPPER( char_to_int( *a ) );
		auto bUpper = TMUT_TOUPPER( char_to_int( *b ) );
		if( aUpper != bUpper ) {
			break;
		}
		++a;
		++b;
	}
	auto aUpper = TMUT_TOUPPER( char_to_int( *a ) );
	auto bUpper = TMUT_TOUPPER( char_to_int( *b ) );
	return aUpper - bUpper;
}
#endif
#ifndef TMUT_NO_STRNICMP
extern int strnicmp( const char* a, const char* b, size_t count )
{
	while( *a && *b && count-- ) {
		auto aUpper = TMUT_TOUPPER( char_to_int( *a ) );
		auto bUpper = TMUT_TOUPPER( char_to_int( *b ) );
		if( aUpper != bUpper ) {
			break;
		}
		++a;
		++b;
	}
	if( !count ) {
		return 0;
	}
	auto aUpper = TMUT_TOUPPER( char_to_int( *a ) );
	auto bUpper = TMUT_TOUPPER( char_to_int( *b ) );
	return aUpper - bUpper;
}
#endif

#ifndef TMUT_NO_STRREV
extern char* strrev( char* str )
{
	return strnrev( str, strlen( str ) );
}
#endif
#ifndef TMUT_NO_STRNREV
extern char* strnrev( char* str, size_t count )
{
	for( size_t i = 0, j = count - 1; i < j; ++i, --j ) {
		char tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}
	return str;
}
#endif

#ifndef TMUT_NO_MEMRCHR
extern void* memrchr( const void* ptr, int value, size_t len )
{
	const char* p = (const char*)ptr + len;
	while( len ) {
		--len;
		--p;
		if( (unsigned char)*p == value ) {
			return (void*)p;
		}
	}
	return nullptr;
}
#endif
}

#endif // TM_UTILITY_IMPLEMENTATION
