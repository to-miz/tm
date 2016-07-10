/*
tm_utility v1.0 - public domain
written by Tolga Mizrak 2016

NOTES
	Utility and convenience functions that I use in my private projects.
	Mostly just overloads for safety and some operator like functions.
	See comments at declarations for more info.

HISTORY
	v1.0	10.07.16	initial commit

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.
*/

#pragma once

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

// define this if you use another type as your size type, like int
#ifndef TMUT_OWN_TYPES
typedef size_t tmut_size_t;
// tmut_uintptr needs to be an unsigned integral as big as a pointer
typedef size_t tmut_uintptr;
#endif

#ifndef TMUT_NO_MINMAX
#define MIN( a, b ) ( ( ( a ) <= ( b ) ) ? ( a ) : ( b ) )
#define MAX( a, b ) ( ( ( a ) >= ( b ) ) ? ( a ) : ( b ) )
#endif

static_assert( sizeof( tmut_uintptr ) == sizeof( void* ), "uintptr is not pointer size" );
#define assert_alignment( ptr, alignment ) \
	TMUT_ASSERT( ( ( ( tmut_uintptr )( ptr ) ) % ( alignment ) ) == 0 )

// cast the bit representation to another type
// useful to get bit representation of a float or double into an unsigned integral
template < class result_type, class value_type >
result_type bit_cast( const value_type& value );

// return index of value in contiguous container
template < class Container, class ValueType >
tmut_size_t indexof( const Container& container, const ValueType& value );

// get type of value without it being a reference type
#define typeof( value ) std::remove_reference < decltype( value ) > ::type

// get enum value as underlying type
// Expample:
//	enum class MyEnum : char { A, B, C };
//	MyEnum val = MyEnum::C;
//	valueof( val ) == (char)2
template < class EnumType >
constexpr inline typename std::underlying_type< EnumType >::type valueof( EnumType value )
{
	return static_cast< typename std::underlying_type< EnumType >::type >( value );
}

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

// compare count elements of a and b
template < class T >
int compare( const T* a, const T* b, size_t count );
// zero memory of count elements in dest
template < class T >
void zeroMemory( T* dest, size_t count );

// return clamped value in [lower, upper]
template < class T >
T clamp( T val, T lower, T upper );
// used most often with float, so a simple overload with default values
float clamp( float val, float lower = 0, float upper = 1 );

// returns a/b if b != 0, def otherwise
float safeDivide( float a, float b, float def = 0.0f );

//////////////////////////////////////////////////////////////////
// implementation

template < class result_type, class value_type >
inline result_type bit_cast( const value_type& value )
{
	static_assert( sizeof( result_type ) == sizeof( value_type ), "bit_cast: size mismatch" );

	result_type ret;
	TMUT_MEMCPY( &ret, &value, sizeof( ret ) );
	return ret;
}

template < class Container, class ValueType >
tmut_size_t indexof( const Container& container, const ValueType& value )
{
#ifndef TM_USE_OWN_BEGIN_END
	using std::begin;
	using std::end;
#endif
	auto first = begin( container );
	auto last = end( container );
	// TODO: to be perfectly correct, we'd have to use std::addressof here instead of &
	// but then again, don't overload addressof & operator
	TMUT_ASSERT( &value >= &*first && &value < &*last );
	// we take the adress after dereferencing in case we are dealing with an iterator type instead
	// of pointers
	return static_cast< tmut_size_t >( &value - &*first );
}

template < class T >
inline T clamp( T val, T lower, T upper )
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
	assert_alignment( dest, alignof( T ) );
	TMUT_MEMCPY( dest, src, count * sizeof( T ) );
}

template < class T >
inline void move( T* dest, const void* src, size_t count )
{
	assert_alignment( dest, alignof( T ) );
	TMUT_MEMMOVE( dest, src, count * sizeof( T ) );
}

template < class T >
inline void fill( T* dest, const T& value, size_t count )
{
	assert_alignment( dest, alignof( T ) );
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

template < class T >
inline int compare( const T* a, const T* b, size_t count )
{
	return TMUT_MEMCMP( a, b, count * sizeof( T ) );
}

template < class T >
inline void zeroMemory( T* dest, size_t count )
{
	assert_alignment( dest, alignof( T ) );
	TMUT_MEMSET( dest, 0, count * sizeof( T ) );
}

#endif  // _TM_UTILITY_H_INCLUDED_
