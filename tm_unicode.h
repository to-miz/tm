/*
tm_unicode.h v1.0.1 - public domain
author: Tolga Mizrak 2016

no warranty; use at your own risk

USAGE
	This file works as both the header and implementation.
	To implement the interfaces in this header,
		#define TM_UNICODE_IMPLEMENTATION
	in ONE C or C++ source file before #including this header.

NOTES
	Utf16 and Utf8 encoding functions without validation.
	Implemented are functions to get codepoints from utf16 and utf8 strings and to convert between
	each other. This library assumes non nullterminated strings, so knowledge of the length of the
	strings is needed when calling these functions.
	Utf8 strings are handled by treating any sequence of chars as utf8 encoded strings, so the
	functions can be called with ordinary c strings without casting.
	Whenever a function wants a length or size, it refers to the number of entries in the string
	array and NOT the number of codepoints (which may be less). To get the size of ordinary
	c strings strlen may be used, even with utf8 encoded nullterminated strings.

	Extracting codepoints from a utf8 string follows this formula:
		const char* current = your_utf8_encoded_string_here;
		size_t remaining = your_string_length_here;
		while( remaining ) {
			uint32_t codepoint = utf8NextCodepoint( &current, &remaining );
			// use codepoint ...
		}

	You can navigate through a utf8 string codepoint by codepoint by doing:
		size_t index = 0;
		const char* str = your_utf8_encoded_string_here;
		size_t length = your_string_length_here;

		// go to next codepoint
		index += utf8Advance( str, index, length );

		// go to prev codepoint
		index -= utf8Retreat( str, index );
	Advance and retreat are useful when for example implementing edit boxes and needing to move the
	caret to the next codepoint.

HISTORY
	v1.0.1  07.10.16 removed forcing to use unsigned arithmetic when tmu_size_t is signed
	v1.0b   02.07.16 changed #include <memory.h> into string.h
	v1.0a   01.07.16 improved C99 conformity
	v1.0   24.06.16 initial commit

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.
*/

#ifdef TM_UNICODE_IMPLEMENTATION
	// define these to avoid crt

	#ifndef TMU_ASSERT
		#include <assert.h>
		#define TMU_ASSERT assert
	#endif

	#ifndef TMU_MEMCPY
		#include <string.h>
		#define TMU_MEMCPY memcpy
	#endif
#endif

#ifndef _TM_UNICODE_H_INCLUDED_
#define _TM_UNICODE_H_INCLUDED_

#ifndef TMU_OWN_TYPES
	#include <stdint.h>
	typedef size_t tmu_size_t;
	#ifndef __cplusplus
		typedef int tmu_bool;
	#else
		typedef bool tmu_bool;
	#endif
	typedef uint16_t tmu_sequence_length;

	// these need to be unsigned so we can do bit twiddling
	typedef uint8_t tmu_char8;
	typedef uint16_t tmu_char16;
	typedef uint32_t tmu_char32;
#endif

#ifndef TMU_STATIC
	#define TMU_DEF extern
	#define TMU_STORAGE
#else
	#define TMU_DEF static
	#define TMU_STORAGE static
#endif

#ifdef __cplusplus
extern "C" {
#endif

// utf16

#ifndef TMU_STATIC
extern const int utf16ByteOrderMarkSize;
extern const int utf16ByteOrderMarkSizeInBytes;
extern const tmu_char8 utf16LittleEndianByteOrderMark[2];
extern const tmu_char8 utf16BigEndianByteOrderMark[2];
#endif

// utf16 string as a string of chars
TMU_DEF tmu_bool utf16HasLittleEndianByteOrderMark( const char* str, tmu_size_t length );
// utf16 string as a string of char16s
TMU_DEF tmu_bool utf16HasLittleEndianByteOrderMark16( const tmu_char16* str, tmu_size_t length );
// utf16 string as a string of chars
TMU_DEF tmu_bool utf16HasBigEndianByteOrderMark( const char* str, tmu_size_t length );
// utf16 string as a string of char16s
TMU_DEF tmu_bool utf16HasBigEndianByteOrderMark16( const tmu_char16* str, tmu_size_t length );

TMU_DEF tmu_bool utf16IsSurrogateLead( tmu_char32 codepoint );

// advances it, decrements remaining and returns the next utf16 codepoint in str
TMU_DEF tmu_char32 utf16NextCodepoint( const tmu_char16** it, tmu_size_t* remaining );
TMU_DEF void utf16SwapEndian16( tmu_char16* str, tmu_size_t length );
TMU_DEF void utf16SwapEndian( char* str, tmu_size_t length );

typedef struct {
	tmu_char16 elements[2];
	tmu_sequence_length length;
} Utf16Sequence;

TMU_DEF Utf16Sequence toUtf16( tmu_char32 codepoint );
TMU_DEF tmu_char16 toUcs2( tmu_char32 codepoint );
TMU_DEF tmu_bool isUcs2( tmu_char32 codepoint );

TMU_DEF tmu_size_t convertUtf8ToUtf16( const char* utf8Start, tmu_size_t utf8Length,
                                       tmu_char16* out, tmu_size_t size );
TMU_DEF tmu_size_t convertUtf8ToUcs2( const char* utf8Start, tmu_size_t utf8Length, tmu_char16* out,
                                      tmu_size_t size );

// utf8

#ifndef TMU_STATIC
extern const int utf8ByteOrderMarkSize;
extern const int utf8ByteOrderMarkSizeInBytes;
extern const tmu_char8 utf8ByteOrderMark[3];
#endif

TMU_DEF tmu_bool utf8HasByteOrderMark( const char* str, tmu_size_t length );
TMU_DEF tmu_bool utf8HasByteOrderNullterminated( const char* str );

tmu_bool utf8IsLead( char c );

TMU_DEF tmu_char32 utf8NextCodepoint( const char** it, tmu_size_t* remaining );

// returns how many bytes to advance to skip one codepoint
// Args:
//		str: start of utf8 string
//		start: offset from str, where to start searching
//		size: total size of str
TMU_DEF tmu_size_t utf8Advance( const char* str, tmu_size_t start, tmu_size_t size );
// returns how many bytes to retreat to go back one codepoint
// Args:
//		str: start of utf8 string
//		start: offset from str, where to start searching
TMU_DEF tmu_size_t utf8Retreat( const char* str, tmu_size_t start );

typedef struct {
	char elements[4];
	tmu_sequence_length length;
} Utf8Sequence;

TMU_DEF Utf8Sequence toUtf8( tmu_char32 codepoint );
TMU_DEF tmu_size_t convertUtf16ToUtf8( const tmu_char16* utf16Start, tmu_size_t utf16Length,
                                       char* out, tmu_size_t size );
TMU_DEF tmu_size_t convertUcs2ToUtf8( const tmu_char16* ucs2Start, tmu_size_t ucs2Length, char* out,
                                      tmu_size_t size );

TMU_DEF tmu_size_t utf8CountCodepoints( const char* str, tmu_size_t size );

// inline impl
inline tmu_bool utf8IsLead( char c )
{
	return ( ( (tmu_char8)c ) & 0xC0 ) != 0x80;
}

#ifdef __cplusplus
}
#endif

#ifdef TMU_CONVENIENCE_STD_STRING
TMU_DEF std::string toUtf8String( tmu_char32 codepoint );
TMU_DEF std::string convertUtf16ToUtf8( const tmu_char16* utf16Start, tmu_size_t utf16Length );
inline std::string convertUtf16ToUtf8( const wchar_t* utf16Start, tmu_size_t utf16Length )
{
	static_assert(
		sizeof( wchar_t ) == sizeof( tmu_char16 ) && alignof( wchar_t ) == alignof( uint16 ),
		"wchar_t incompatibility" );
	return convertUtf16ToUtf8( (const tmu_char16*)utf16Start, utf16Length );
}
#endif

#endif

// Implementation

#ifdef TM_UNICODE_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

TMU_STORAGE const int utf16ByteOrderMarkSize = 1;
TMU_STORAGE const int utf16ByteOrderMarkSizeInBytes = 2;
TMU_STORAGE const tmu_char8 utf16LittleEndianByteOrderMark[2] = {0xFF, 0xFE};
TMU_STORAGE const tmu_char8 utf16BigEndianByteOrderMark[2] = {0xFE, 0xFF};

TMU_DEF tmu_bool utf16HasLittleEndianByteOrderMark( const char* str, tmu_size_t length )
{
	if( length >= 2 ) {
		return (tmu_char8)str[0] == utf16LittleEndianByteOrderMark[0]
			   && (tmu_char8)str[1] == utf16LittleEndianByteOrderMark[1];
	} else {
		return 0;
	}
}
TMU_DEF tmu_bool utf16HasLittleEndianByteOrderMark16( const tmu_char16* str, tmu_size_t length )
{
	if( length >= 1 ) {
		char* p = (char*)str;
		return (tmu_char8)p[0] == utf16LittleEndianByteOrderMark[0]
			   && (tmu_char8)p[1] == utf16LittleEndianByteOrderMark[1];
	} else {
		return 0;
	}
}
TMU_DEF tmu_bool utf16HasBigEndianByteOrderMark( const char* str, tmu_size_t length )
{
	if( length >= 2 ) {
		return (tmu_char8)str[0] == utf16BigEndianByteOrderMark[0]
			   && (tmu_char8)str[1] == utf16BigEndianByteOrderMark[1];
	} else {
		return 0;
	}
}
TMU_DEF tmu_bool utf16HasBigEndianByteOrderMark16( const tmu_char16* str, tmu_size_t length )
{
	if( length >= 1 ) {
		char* p = (char*)str;
		return (tmu_char8)p[0] == utf16BigEndianByteOrderMark[0]
			   && (tmu_char8)p[1] == utf16BigEndianByteOrderMark[1];
	} else {
		return 0;
	}
}

static const tmu_char32 LEAD_SURROGATE_MIN = 0xD800;
static const tmu_char32 LEAD_SURROGATE_MAX = 0xDBFF;
static const tmu_char32 TRAILING_SURROGATE_MIN = 0xDC00;
static const tmu_char32 TRAILING_SURROGATE_MAX = 0xDFFF;
static const tmu_char32 LEAD_OFFSET = 0xD800 - ( 0x10000 >> 10 );
static const tmu_char32 SURROGATE_OFFSET = ( tmu_char32 )( 0x10000 - ( 0xD800 << 10 ) - 0xDC00 );

TMU_DEF tmu_bool utf16IsSurrogateLead( tmu_char32 codepoint )
{
	return codepoint >= LEAD_SURROGATE_MIN && codepoint <= LEAD_SURROGATE_MAX;
}

TMU_DEF tmu_char32 utf16NextCodepoint( const tmu_char16** it, tmu_size_t* remaining )
{
	TMU_ASSERT( it );
	TMU_ASSERT( remaining );

	tmu_char32 codepoint = **it;
	if( utf16IsSurrogateLead( codepoint ) ) {
		if( *remaining >= 2 ) {
			tmu_char32 trail = *( ++*it );
			codepoint = ( codepoint << 10 ) + trail + SURROGATE_OFFSET;
			*remaining -= 2;
		} else {
			codepoint = 0xFFFFFFFF;
			*remaining = 0;
			return 0;
		}
	} else if( codepoint > 0x10FFFF ) {
		codepoint = 0xFFFFFFFF;
	}
	++*it;
	--*remaining;
	return codepoint;
}
TMU_DEF void utf16SwapEndian16( tmu_char16* str, tmu_size_t length )
{
	for( tmu_size_t i = 0; i < length; ++i ) {
		str[i] = ( tmu_char16 )( ( ( ( tmu_char16 )( str[i] ) & (tmu_char16)0x00ffU ) << 8 )
		                         | ( ( ( tmu_char16 )( str[i] ) & (tmu_char16)0xff00U ) >> 8 ) );
	}
}
TMU_DEF void utf16SwapEndian( char* str, tmu_size_t length )
{
	TMU_ASSERT( length % 2 == 0 );
	// swap pairs of bytes around
	for( tmu_size_t i = 1; i < length; i += 2 ) {
		char t     = str[i];
		str[i]     = str[i - 1];
		str[i - 1] = t;
	}
}

TMU_DEF Utf16Sequence toUtf16( tmu_char32 codepoint )
{
	Utf16Sequence result = {{0, 0}, 0};
	if( codepoint < 0xD7FF ) {
		result.elements[0] = (tmu_char16)codepoint;
		result.length      = 1;
	} else if( codepoint >= 0xE000 && codepoint <= 0xFFFF ) {
		result.elements[0] = (tmu_char16)codepoint;
		result.length      = 1;
	} else if( codepoint >= 0x10000 && codepoint <= 0x10FFFF ) {
		codepoint -= 0x10000;
		result.elements[0] = 0xD800 + ( tmu_char16 )( codepoint >> 10 );
		result.elements[1] = 0xDC00 + ( tmu_char16 )( codepoint & 0x3FF );
		result.length      = 2;
	}
	return result;
}
TMU_DEF tmu_char16 toUcs2( tmu_char32 codepoint )
{
	if( isUcs2( codepoint ) ) {
		return (tmu_char16)codepoint;
	}
	return 0;
}
TMU_DEF tmu_bool isUcs2( tmu_char32 codepoint )
{
	return ( codepoint < 0xD7FF ) || ( codepoint >= 0xE000 && codepoint <= 0xFFFF );
}

TMU_DEF tmu_size_t convertUtf8ToUtf16( const char* utf8Start, tmu_size_t utf8Length,
                                       tmu_char16* out, tmu_size_t size )
{
	tmu_size_t result = 0;
	while( utf8Length ) {
		tmu_char32 codepoint = utf8NextCodepoint( &utf8Start, &utf8Length );
		if( !utf8Length && codepoint == 0xFFFFFFFF ) break;
		if( codepoint == 0xFFFFFFFF ) codepoint = 0;

		Utf16Sequence sequence = toUtf16( codepoint );
		if( sequence.length > size ) break;
		TMU_MEMCPY( out, sequence.elements, sequence.length * sizeof( tmu_char16 ) );
		out += sequence.length;
		size -= sequence.length;
		result += sequence.length;
	}
	return result;
}
TMU_DEF tmu_size_t convertUtf8ToUcs2( const char* utf8Start, tmu_size_t utf8Length, tmu_char16* out,
                                      tmu_size_t size )
{
	tmu_size_t result = 0;
	while( utf8Length ) {
		tmu_char32 codepoint = utf8NextCodepoint( &utf8Start, &utf8Length );
		if( !utf8Length && codepoint == 0xFFFFFFFF ) break;
		if( !isUcs2( codepoint ) ) continue;

		*out = (tmu_char16)codepoint;
		++out;
		--size;
		++result;
	}
	return result;
}

// utf8

TMU_STORAGE const int utf8ByteOrderMarkSize        = 3;
TMU_STORAGE const int utf8ByteOrderMarkSizeInBytes = 3;
TMU_STORAGE const tmu_char8 utf8ByteOrderMark[3]   = {0xEF, 0xBB, 0xBF};

TMU_DEF tmu_bool utf8HasByteOrderMark( const char* str, tmu_size_t length )
{
	if( length >= utf8ByteOrderMarkSizeInBytes ) {
		return str[0] == utf8ByteOrderMark[0] && str[1] == utf8ByteOrderMark[1]
		       && str[2] == utf8ByteOrderMark[2];
	}
	return 0;
}
TMU_DEF tmu_bool utf8HasByteOrderNullterminated( const char* str )
{
	size_t i = 0;
	while( *str && i < 3 && (tmu_char8)*str == utf8ByteOrderMark[i] ) {
		++str;
		++i;
	}
	return i == 3;
}
TMU_DEF tmu_char32 utf8NextCodepoint( const char** it, tmu_size_t* remaining )
{
	tmu_char32 codepoint = ( tmu_char32 )( ( tmu_char8 ) * *it );

	if( codepoint < 0x80 ) {                  // 0xxxxxxx
		                                      // 1 byte sequence
	} else if( ( codepoint >> 5 ) == 0x6 ) {  // 110xxxxx 10xxxxxx
		// 2 byte sequence
		if( *remaining >= 2 ) {
			tmu_char32 second = ( tmu_char8 ) * ( ++*it );
			codepoint         = ( ( codepoint & 0x1F ) << 6 ) | ( second & 0x3F );
			--*remaining;
		} else {
			codepoint  = 0xFFFFFFFF;
			*remaining = 0;
			return 0;
		}
	} else if( ( codepoint >> 4 ) == 0xE ) {  // 1110xxxx 10xxxxxx 10xxxxxx
		// 3 byte sequence
		if( *remaining >= 3 ) {
			tmu_char32 second = ( tmu_char8 ) * ( ++*it );
			tmu_char32 third  = ( tmu_char8 ) * ( ++*it );
			codepoint =
			    ( ( codepoint & 0xF ) << 12 ) | ( ( second & 0x3F ) << 6 ) | ( third & 0x3F );
			*remaining -= 2;
		} else {
			codepoint  = 0xFFFFFFFF;
			*remaining = 0;
			return 0;
		}
	} else if( ( codepoint >> 3 ) == 0x1E ) {  // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		// 4 byte sequence
		if( *remaining >= 4 ) {
			tmu_char32 second = ( tmu_char8 ) * ( ++*it );
			tmu_char32 third  = ( tmu_char8 ) * ( ++*it );
			tmu_char32 fourth = ( tmu_char8 ) * ( ++*it );
			codepoint         = ( ( codepoint & 0x7 ) << 18 ) | ( ( second & 0x3F ) << 12 )
			            | ( ( third & 0x3F ) << 6 ) | ( fourth & 0x3f );
			*remaining -= 3;
		} else {
			codepoint  = 0xFFFFFFFF;
			*remaining = 0;
			return 0;
		}
	} else {
		// malformed
		codepoint = 0xFFFFFFFF;
	}

	++*it;
	--*remaining;
	return codepoint;
}
TMU_DEF tmu_size_t utf8Advance( const char* str, tmu_size_t start, tmu_size_t size )
{
	TMU_ASSERT( start <= size );
	TMU_ASSERT( size >= 0 );
	size -= start;
	if( size ) {
		tmu_size_t prev = size;
		str += start;
		utf8NextCodepoint( &str, &size );
		return prev - size;
	}
	return 0;
}
TMU_DEF tmu_size_t utf8Retreat( const char* str, tmu_size_t start )
{
	tmu_size_t current = start;
	if( current <= 0 ) {
		return 0;
	}

	do {
		--current;
	} while( current > 0 && !utf8IsLead( str[current] ) );
	return ( tmu_size_t )( start - current );
}
TMU_DEF Utf8Sequence toUtf8( tmu_char32 codepoint )
{
	Utf8Sequence result;
	if( codepoint < 0x80 ) {
		// 1 byte sequence
		result.elements[0] = (char)( codepoint );
		result.length = 1;
	} else if( codepoint < 0x800 ) {
		// 2 byte sequence 110xxxxx 10xxxxxx
		result.elements[0] = (char)( 0xC0 | (tmu_char8)( codepoint >> 6 ) );
		result.elements[1] = (char)( 0x80 | (tmu_char8)( codepoint & 0x3F ) );
		result.length = 2;
	} else if( codepoint < 0x10000 ) {
		// 3 byte sequence 1110xxxx 10xxxxxx 10xxxxxx
		result.elements[0] = (char)( 0xE0 | (tmu_char8)( codepoint >> 12 ) );
		result.elements[1] = (char)( 0x80 | ( (tmu_char8)( codepoint >> 6 ) & 0x3F ) );
		result.elements[2] = (char)( 0x80 | ( (tmu_char8)( codepoint & 0x3F ) ) );
		result.length = 3;
	} else {
		// 4 byte sequence 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		result.elements[0] = (char)( 0xF0 | ( (tmu_char8)( codepoint >> 18 ) & 0x7 ) );
		result.elements[1] = (char)( 0x80 | ( (tmu_char8)( codepoint >> 12 ) & 0x3F ) );
		result.elements[2] = (char)( 0x80 | ( (tmu_char8)( codepoint >> 6 ) & 0x3F ) );
		result.elements[3] = (char)( 0x80 | ( (tmu_char8)( codepoint & 0x3F ) ) );
		result.length = 4;
	}

	return result;
}
TMU_DEF tmu_size_t convertUtf16ToUtf8( const tmu_char16* utf16Start, tmu_size_t utf16Length,
									   char* out, tmu_size_t size )
{
	tmu_size_t result = 0;
	while( utf16Length ) {
		tmu_char32 codepoint = utf16NextCodepoint( &utf16Start, &utf16Length );
		if( !utf16Length && codepoint == 0xFFFFFFFF ) break;
		if( codepoint == 0xFFFFFFFF ) codepoint = 0;

		Utf8Sequence sequence = toUtf8( codepoint );
		if( sequence.length > size ) break;
		TMU_MEMCPY( out, sequence.elements, sequence.length );
		out += sequence.length;
		size -= sequence.length;
		result += sequence.length;
	}
	return result;
}
TMU_DEF tmu_size_t
convertUcs2ToUtf8( const tmu_char16* ucs2Start, tmu_size_t ucs2Length, char* out, tmu_size_t size )
{
	tmu_size_t result = 0;
	for( tmu_size_t i = 0; i < ucs2Length; ++i ) {
		if( !isUcs2( ucs2Start[i] ) ) continue;
		tmu_char32 codepoint = (tmu_char32)ucs2Start[i];

		Utf8Sequence sequence = toUtf8( codepoint );
		if( sequence.length > size ) break;
		TMU_MEMCPY( out, sequence.elements, sequence.length );
		out += sequence.length;
		size -= sequence.length;
		result += sequence.length;
	}
	return result;
}
TMU_DEF tmu_size_t utf8CountCodepoints( const char* str, tmu_size_t size )
{
	TMU_ASSERT( size >= 0 );
	tmu_size_t ret = 0;
	while( size ) {
		utf8NextCodepoint( &str, &size );
		++ret;
	}
	return ret;
}

#ifdef __cplusplus
}
#endif

#ifdef TMU_CONVENIENCE_STD_STRING
TMU_DEF std::string toUtf8String( uint32 codepoint )
{
	Utf8Sequence sequence = toUtf8( codepoint );
	return std::string( sequence.elements, sequence.length );
}
TMU_DEF std::string convertUtf16ToUtf8( const uint16* utf16Start, int32 utf16Length )
{
	std::string ret;
	ret.reserve( utf16Length );
	while( utf16Length ) {
		tmu_char32 codepoint = utf16::next( utf16Start, utf16Length );
		Utf8Sequence sequence = toUtf8( codepoint );
		ret.insert( ret.end(), sequence.elements, sequence.elements + sequence.length );
	}
	return ret;
}
#endif

#endif
