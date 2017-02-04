/*
tm_print.h v0.0.4d - public domain
author: Tolga Mizrak 2016

no warranty; use at your own risk

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_PRINT_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

ISSUES
	- The output between tm_conversion.h and snprintf based output isn't the same for floating
	points.
	The tm_conversion.h library doesn't output trailing zeroes by default, while printf will output
	trailing zeroes when using %f. To make the tm_conversion based implementation always output
	trailing zeroes, define:
		#define TMP_FLOAT_ALWAYS_TRAILING_ZEROES
	See Switches for more info.
	Otherwise outputting floats with {:f} will always produce trailing zeroes for both
	implementations.

	- The tm_conversion based implementation always outputs '.' as the decimal point character
	regardless of the locale, while the snprintf based output outputs the decimal point based on the
	current locale.

	- The print function (not snprint) will always use a buffer to print into that has the size
	TMP_PRINTF_BUFFER_SIZE. It then passes that buffer to printf to output. When using cusom
	printers and exceeding TMP_PRINTF_BUFFER_SIZE, your output will be truncated.
	Either increase TMP_PRINTF_BUFFER_SIZE (will increase stack memory usage) or use snprintf (which
	will use the buffer you supply, so no truncation will occur unless the buffer is insufficiently
	sized)

HISTORY
	v0.0.4d 10.01.17 minor change from static const char* to static const char* const in print_bool
	v0.0.4c 23.10.16 added some assertions for bounds checking
	v0.0.4b 07.10.16 fixed some casting issues when tmp_size_t is signed
	v0.0.4a 29.09.16 fixed a bug where inputting an escaped {{ was resulting in an infinite loop
	v0.0.4  29.09.16 added signed/unsigned char, short and long handling
	                 fixed compiler errors when compiling with clang
	v0.0.3  27.09.16 added printing custom types by overloading snprint
	                 added initialFormatting parameter to snprint so that custom printing can
	                 inherit formatting options
	v0.0.2  26.09.16 changed makeFlags to tmp_type_flags so that it is guaranteed to be a
	                 compile time constant
	                 added string view overloads so that print can accept a string view as the
	                 format parameter
	                 fixed some compiler warnings when tmp_size_t is defined as int
    v0.0.1  24.09.16 initial commit

LICENSE
    This software is dual-licensed to the public domain and under the following
    license: you are granted a perpetual, irrevocable license to copy, modify,
    publish, and distribute this file as you see fit.
*/

#ifdef TM_PRINT_IMPLEMENTATION
	#ifndef TMP_ASSERT
		#include <cassert>
		#define TMP_ASSERT assert
	#endif
	#ifndef TMP_MEMCPY
		#include <cstring>
		#define TMP_MEMCPY memcpy
	#endif
	#ifndef TMP_MEMCHR
		#include <cstring>
		#define TMP_MEMCHR memchr
	#endif
	#ifndef TMP_STRLEN
		#include <cstring>
		#define TMP_STRLEN strlen
	#endif

	#if defined( TMP_NO_TM_CONVERSION ) && !defined( TMP_NO_STDIO )
		#include <cstdio>
		#if defined( _MSC_VER ) && _MSC_VER <= 1800
			// in case we are not using tm_conversion.h and _MSC_VER is <= Visual Studio 2013
			// snprintf wasn't supported until after Visual Studio 2013
			#define TMP_SNPRINTF _snprintf
		#else
			#define TMP_SNPRINTF snprintf
		#endif
	#endif

	#if defined( TMP_NO_TM_CONVERSION ) && !defined( TMP_STRTOUL )
		#include <cstdlib>
		#define TMP_STRTOUL strtoul
	#endif

	#if defined( TMP_NO_TM_CONVERSION ) && defined( TMP_NO_STDIO ) && !defined( TMP_SNPRINTF )
		#error "TMP_SNPRINTF has to be defined if TMP_NO_STDIO and TMP_NO_TM_CONVERSION are defined"
	#endif

	// the buffer size of the buffer that printf will use when formatting
	// if the buffer size isn't big enough, using custom printing and exceeding this buffer size
	// will result in truncated output (see ISSUES)
	#ifndef TMP_PRINTF_BUFFER_SIZE
		#define TMP_PRINTF_BUFFER_SIZE 200
	#endif // !defined( TMP_PRINTF_BUFFER_SIZE )
#endif

#ifndef _TM_PRINT_H_INCLUDED_
#define _TM_PRINT_H_INCLUDED_

#ifndef TMP_NO_STDIO
	#include <cstdio>
#endif

#ifndef TMP_OWN_TYPES
	#include <cstdint>
	typedef int32_t tmp_int32;
	typedef uint32_t tmp_uint32;
	typedef int64_t tmp_int64;
	typedef uint64_t tmp_uint64;
	typedef size_t tmp_size_t;
#endif

#define TMP_BITFIELD( x ) ( 1 << ( x ) )
#define TMP_BITCOUNT( x ) ( sizeof( x ) * 8 )
#define TMP_MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )

// define this if you do not want to use tm_conversion at all
// the library will then use snprintf and cstdlib or a snprintf variant you supply instead
#ifndef TMP_NO_TM_CONVERSION
	// in case you want to use tm_conversion but just don't want this header to include it
	#ifndef TMP_NO_INCLUDE_TM_CONVERSION
		#include "tm_conversion.h"
	#endif
#else
	// these are yanked from tm_conversion, we need these to do formatting our selves
	enum PrintFormatFlags : unsigned int {
		PF_SIGN            = TMP_BITFIELD( 0 ),
		PF_LOWERCASE       = TMP_BITFIELD( 1 ),
		PF_TRAILING_ZEROES = TMP_BITFIELD( 2 ),
		PF_BOOL_AS_NUMBER  = TMP_BITFIELD( 3 ),
		PF_SCIENTIFIC      = TMP_BITFIELD( 4 ),  // not implemented yet

		PF_DEFAULT = 0,

		PF_COUNT = 5,  // 5 flags in total
	};

	typedef struct PrintFormatStruct {
		int base;
		int precision;
		int width;
		unsigned int flags;
	} PrintFormat;

	inline PrintFormat defaultPrintFormat()
	{
		return PrintFormat{10, 6, 0, PF_DEFAULT};
	}
#endif

namespace PrintType
{
enum Values : tmp_uint64 {
	Char = 1,
	Bool,
	Int,
	UInt,
	Int64,
	UInt64,
	Float,
	Double,
	String,
	StringView,
	Custom,

	Last,  // only here to check against Count to see if they are the same (see static_asserts
	       // below)

	// count needs to be power of two so that Mask and Bits is valid
	Count = 16,
	Mask  = Count - 1,
	Bits  = 4
};
}

#ifndef TMP_STATIC
	#define TMP_DEF extern
#else
	#define TMP_DEF static
#endif

// sanity checks
static_assert( PrintType::Last <= PrintType::Count,
               "Values added to PrintType without adjusting Count and Bits" );
static_assert( ( PrintType::Count & ( PrintType::Count - 1 ) ) == 0, "Count must be power of two" );
static_assert( TMP_BITCOUNT( tmp_uint64 ) / PrintType::Bits >= PrintType::Count,
               "Can't store PrintType::Count in a tmp_uint64" );

#ifdef TMP_CUSTOM_PRINTING
	typedef tmp_size_t tmp_custom_printer_type( char* buffer, tmp_size_t len,
	                                            const PrintFormat& initialFormatting,
	                                            const void* data );
#endif

// these are needed to do a preprocessor branch on the size of long
// you can define TMP_SIZEOF_LONG yourself if you do not want to include climits and cstdint
// if your compiler has constexpr if (C++1z feature), you can define TMP_HAS_CONSTEXPR_IF and the
// preprocessor branching on the size of long won't be needed
#if !defined( TMP_SIZEOF_LONG ) && !defined( TMP_HAS_CONSTEXPR_IF )
	#include <climits>
	#include <cstdint>
	#if LONG_MAX == INT32_MAX
		#define TMP_SIZEOF_LONG 4
	#elif LONG_MAX == INT64_MAX
		#define TMP_SIZEOF_LONG 8
	#else
		#error unhandled size of long
	#endif
#endif // !defined( TMP_SIZEOF_LONG )

union PrintValue {
	char c;
	bool b;
	int i;
	unsigned int u;
	long long ll;
	unsigned long long ull;
	float f;
	double d;
	const char* s;
	struct {
		const char* data;
		tmp_size_t size;
	} v;
	#ifdef TMP_CUSTOM_PRINTING
		struct {
			const void* data;
			tmp_custom_printer_type* customPrint;
		} custom;
	#endif
};

struct PrintArgList {
	PrintValue args[PrintType::Count];
	tmp_uint64 flags;
	unsigned int size;
};

#ifndef TMP_NO_STDIO
	TMP_DEF void print( FILE* out, const char* format, const PrintArgList& args );
	void print( const char* format, const PrintArgList& args );
	#ifdef TMP_STRING_VIEW
		TMP_DEF void print( FILE* out, TMP_STRING_VIEW format, const PrintArgList& args );
		void print( TMP_STRING_VIEW format, const PrintArgList& args );
	#endif // TMP_STRING_VIEW
#endif  // TMP_NO_STDIO
TMP_DEF tmp_size_t snprint( char* dest, tmp_size_t len, const char* format,
                            const PrintArgList& args );
TMP_DEF tmp_size_t snprint( char* dest, tmp_size_t len, const char* format,
                            const PrintFormat& initialFormatting, const PrintArgList& args );
#ifdef TMP_STRING_VIEW
	TMP_DEF tmp_size_t snprint( char* dest, tmp_size_t len, TMP_STRING_VIEW format,
	                            const PrintArgList& args );
	TMP_DEF tmp_size_t snprint( char* dest, tmp_size_t len, TMP_STRING_VIEW format,
	                            const PrintFormat& initialFormatting, const PrintArgList& args );
#endif

template < class... Types > struct tmp_type_flags;
template <> struct tmp_type_flags<> {
	enum : tmp_uint64 { Value = 0 };
};
template < class... Types > struct tmp_type_flags< char, Types... > {
	enum : tmp_uint64 {
		Value = PrintType::Char | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};

// for some types, we don't know what their sizes are
template <size_t N> struct tmp_int_size;
template <> struct tmp_int_size<1> { enum : tmp_uint64 { Value = PrintType::Int }; };
template <> struct tmp_int_size<2> { enum : tmp_uint64 { Value = PrintType::Int }; };
template <> struct tmp_int_size<4> { enum : tmp_uint64 { Value = PrintType::Int }; };
template <> struct tmp_int_size<8> { enum : tmp_uint64 { Value = PrintType::Int64 }; };
template <size_t N> struct tmp_uint_size;
template <> struct tmp_uint_size<1> { enum : tmp_uint64 { Value = PrintType::UInt }; };
template <> struct tmp_uint_size<2> { enum : tmp_uint64 { Value = PrintType::UInt }; };
template <> struct tmp_uint_size<4> { enum : tmp_uint64 { Value = PrintType::UInt }; };
template <> struct tmp_uint_size<8> { enum : tmp_uint64 { Value = PrintType::UInt64 }; };

template < class... Types > struct tmp_type_flags< signed char, Types... > {
	enum : tmp_uint64 {
		Value = tmp_int_size< sizeof( signed char ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< unsigned char, Types... > {
	enum : tmp_uint64 {
		Value = tmp_uint_size< sizeof( unsigned char ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< short, Types... > {
	enum : tmp_uint64 {
		Value = tmp_int_size< sizeof( short ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< unsigned short, Types... > {
	enum : tmp_uint64 {
		Value = tmp_uint_size< sizeof( unsigned short ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types >
struct tmp_type_flags< long, Types... > {
	enum : tmp_uint64 {
		Value = tmp_int_size< sizeof( long ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< unsigned long, Types... > {
	enum : tmp_uint64 {
		Value = tmp_uint_size< sizeof( unsigned long ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< int, Types... > {
	enum : tmp_uint64 {
		Value = tmp_int_size< sizeof( int ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< unsigned int, Types... > {
	enum : tmp_uint64 {
		Value = tmp_uint_size< sizeof( unsigned int ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< long long, Types... > {
	enum : tmp_uint64 {
		Value = tmp_int_size< sizeof( long long ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< unsigned long long, Types... > {
	enum : tmp_uint64 {
		Value = tmp_uint_size< sizeof( unsigned long long ) >::Value
		        | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};

template < class... Types > struct tmp_type_flags< bool, Types... > {
	enum : tmp_uint64 {
		Value = PrintType::Bool | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< float, Types... > {
	enum : tmp_uint64 {
		Value = PrintType::Float | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< double, Types... > {
	enum : tmp_uint64 {
		Value = PrintType::Double | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
template < class... Types > struct tmp_type_flags< const char*, Types... > {
	enum : tmp_uint64 {
		Value = PrintType::String | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
#ifdef TMP_STRING_VIEW
template < class... Types > struct tmp_type_flags< TMP_STRING_VIEW, Types... > {
	enum : tmp_uint64 {
		Value = PrintType::StringView | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
	};
};
#endif
#ifdef TMP_CUSTOM_PRINTING
	template < class T, class... Types > struct tmp_type_flags< T, Types... > {
		enum : tmp_uint64 {
			Value = PrintType::Custom | ( tmp_type_flags< Types... >::Value << PrintType::Bits )
		};
	};
#else
	template < class T, class... Types > struct tmp_type_flags< T, Types... > {
		enum : tmp_uint64 { Value = 0 };
	};
#endif

template < class... Types >
void fillPrintArgList( PrintArgList* list, char value, const Types&... args )
{
	list->args[list->size++].c = value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, signed char value, const Types&... args )
{
	list->args[list->size++].i = (int)value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, unsigned char value, const Types&... args )
{
	list->args[list->size++].u = (unsigned int)value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, short value, const Types&... args )
{
	list->args[list->size++].i = (int)value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, unsigned short value, const Types&... args )
{
	list->args[list->size++].u = (unsigned int)value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, long value, const Types&... args )
{
#ifdef TMP_HAS_CONSTEXPR_IF
	if constexpr( sizeof( long ) == sizeof( int ) ) {
		list->args[list->size++].i = (int)value;
		fillPrintArgList( list, args... );
	} else if constexpr ( sizeof( long ) == sizeof( long long ) ) {
		list->args[list->size++].ll = (long long)value;
		fillPrintArgList( list, args... );
	} else {
		static_assert( false, "unhandled size of long" );
	}
#elif TMP_SIZEOF_LONG == 4
	list->args[list->size++].i = (int)value;
	fillPrintArgList( list, args... );
#elif TMP_SIZEOF_LONG == 8
	list->args[list->size++].ll = (long long)value;
	fillPrintArgList( list, args... );
#else
	#error unhandled size of long
#endif
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, unsigned long value, const Types&... args )
{
#ifdef TMP_HAS_CONSTEXPR_IF
	if constexpr( sizeof( unsigned long ) == sizeof( unsigned int ) ) {
		list->args[list->size++].u = (unsigned int)value;
		fillPrintArgList( list, args... );
	} else if constexpr ( sizeof( unsigned long ) == sizeof( unsigned long long ) ) {
		list->args[list->size++].ull = (unsigned long long)value;
		fillPrintArgList( list, args... );
	} else {
		static_assert( false, "unhandled size of long" );
	}
#elif TMP_SIZEOF_LONG == 4
	list->args[list->size++].u = (unsigned int)value;
	fillPrintArgList( list, args... );
#elif TMP_SIZEOF_LONG == 8
	list->args[list->size++].ull = (unsigned long long)value;
	fillPrintArgList( list, args... );
#else
	#error unhandled size of long
#endif
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, bool value, const Types&... args )
{
	list->args[list->size++].b = value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, int value, const Types&... args )
{
	list->args[list->size++].i = value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, unsigned int value, const Types&... args )
{
	list->args[list->size++].u = value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, long long value, const Types&... args )
{
	list->args[list->size++].ll = value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, unsigned long long value, const Types&... args )
{
	list->args[list->size++].ull = value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, float value, const Types&... args )
{
	list->args[list->size++].f = value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, double value, const Types&... args )
{
	list->args[list->size++].d = value;
	fillPrintArgList( list, args... );
}
template < class... Types >
void fillPrintArgList( PrintArgList* list, const char* value, const Types&... args )
{
	list->args[list->size++].s = value;
	fillPrintArgList( list, args... );
}
#ifdef TMP_STRING_VIEW
	template < class... Types >
	void fillPrintArgList( PrintArgList* list, TMP_STRING_VIEW value, const Types&... args )
	{
		list->args[list->size].v.data = TMP_STRING_VIEW_DATA( value );
		list->args[list->size].v.size = TMP_STRING_VIEW_SIZE( value );
		++list->size;
		fillPrintArgList( list, args... );
	}
#endif
#ifdef TMP_CUSTOM_PRINTING
	// this looks very confusing, but it checks for the existence of a specific overload of snprint
	// this way we can do a static_assert on whether the overload exists and report an error
	// otherwise
    template < class T >
    class tmp_has_custom_printer
    {
    	typedef tmp_size_t printer_t( char*, tmp_size_t, const PrintFormat&, const T& );
	    typedef char no;

	    template < class C >
	    static auto test( C c ) -> decltype(
	        static_cast< tmp_size_t ( * )( char*, tmp_size_t, const PrintFormat&, const C& ) >(
	            &snprint ) );
	    template < class C >
	    static no test( ... );

	public:
	    enum { Value = ( sizeof( test< T >( T{} ) ) == sizeof( void* ) ) };
    };

    template < class T, class... Types >
	void fillPrintArgList( PrintArgList* list, const T& value, const Types&... args )
	{
	    static_assert( tmp_has_custom_printer< T >::Value,
	                   "T is not printable, there is no snprint that takes value of type T" );
	    // if the static assert fails, the compiler will also report that there are no overloads of
	    // snprint that accept the argument types. We could get rid of that error by using SFINAE
	    // but that introduces more boilerplate

	    // having constexpr if simplifies the error message
#ifdef TMP_HAS_CONSTEXPR_IF
	    if constexpr( tmp_has_custom_printer< T >::Value )
#endif
	    {
			auto custom         = &list->args[list->size++].custom;
			custom->data        = &value;
			custom->customPrint = []( char* buffer, tmp_size_t len,
			                          const PrintFormat& initialFormatting, const void* data ) {
			    return snprint( buffer, len, initialFormatting, *(const T*)data );
			};
		}
	    fillPrintArgList( list, args... );
    }
#else
	template < class T, class... Types >
	void fillPrintArgList( PrintArgList* list, const T& value, const Types&... args )
	{
		static_assert(
		    false,
		    "T is not printable, custom printing is disabled (TMP_CUSTOM_PRINTING not defined)" );
    }
#endif
void fillPrintArgList( PrintArgList* ) {}

template < class... Types >
void makePrintArgList( PrintArgList* list, const Types&... args )
{
	list->flags = tmp_type_flags< Types... >::Value;
	list->size  = 0;
	fillPrintArgList( list, args... );
}

#ifndef TMP_NO_STDIO
	template < class... Types >
	inline void print( const char* format, const Types&... args )
	{
		static_assert( sizeof...( args ) <= PrintType::Count,
		               "Invalid number of arguments to print" );
		PrintArgList argList;
		makePrintArgList( &argList, args... );
		print( format, argList );
	}
	template < class... Types >
	inline void print( FILE* out, const char* format, const Types&... args )
	{
		static_assert( sizeof...( args ) <= PrintType::Count,
		               "Invalid number of arguments to print" );
		PrintArgList argList;
		makePrintArgList( &argList, args... );
		print( out, format, argList );
	}
	// inline impl
	inline void print( const char* format, const PrintArgList& args )
	{
		print( stdout, format, args );
	}
	inline void print( TMP_STRING_VIEW format, const PrintArgList& args )
	{
		print( stdout, format, args );
	}

	#ifdef TMP_STRING_VIEW
		template < class... Types >
		inline void print( TMP_STRING_VIEW format, const Types&... args )
		{
			static_assert( sizeof...( args ) <= PrintType::Count,
			               "Invalid number of arguments to print" );
			PrintArgList argList;
			makePrintArgList( &argList, args... );
			print( format, argList );
		}
		template < class... Types >
		inline void print( FILE* out, TMP_STRING_VIEW format, const Types&... args )
		{
			static_assert( sizeof...( args ) <= PrintType::Count,
			               "Invalid number of arguments to print" );
			PrintArgList argList;
			makePrintArgList( &argList, args... );
			print( out, format, argList );
		}
	#endif // defined( TMP_STRING_VIEW )
#endif  // TMP_NO_STDIO

template < class... Types >
inline tmp_size_t snprint( char* dest, tmp_size_t len, const char* format, const Types&... args )
{
	static_assert( sizeof...( args ) <= PrintType::Count,
	               "Invalid number of arguments to snprint" );
	PrintArgList argList;
	makePrintArgList( &argList, args... );
	return snprint( dest, len, format, argList );
}
template < class... Types >
inline tmp_size_t snprint( char* dest, tmp_size_t len, const char* format,
                           const PrintFormat& initialFormatting, const Types&... args )
{
	static_assert( sizeof...( args ) <= PrintType::Count,
	               "Invalid number of arguments to snprint" );
	PrintArgList argList;
	makePrintArgList( &argList, args... );
	return snprint( dest, len, format, initialFormatting, argList );
}
#ifdef TMP_STRING_VIEW
	template < class... Types >
	inline tmp_size_t snprint( char* dest, tmp_size_t len, TMP_STRING_VIEW format,
	                           const Types&... args )
	{
		static_assert( sizeof...( args ) <= PrintType::Count,
		               "Invalid number of arguments to snprint" );
		PrintArgList argList;
		makePrintArgList( &argList, args... );
		return snprint( dest, len, format, argList );
	}
	template < class... Types >
	inline tmp_size_t snprint( char* dest, tmp_size_t len, TMP_STRING_VIEW format,
	                           const PrintFormat& initialFormatting, const Types&... args )
	{
		static_assert( sizeof...( args ) <= PrintType::Count,
		               "Invalid number of arguments to snprint" );
		PrintArgList argList;
		makePrintArgList( &argList, args... );
		return snprint( dest, len, format, initialFormatting, argList );
	}
#endif // defined( TMP_STRING_VIEW )

#endif  // _TM_PRINT_H_INCLUDED_

#ifdef TM_PRINT_IMPLEMENTATION

namespace
{
namespace FormatSpecifierFlags
{
enum Values : unsigned int {
	LeftJustify        = TMP_BITFIELD( 0 ),
	PoundSpecified     = TMP_BITFIELD( 1 ),
	PrependHex         = TMP_BITFIELD( 2 ),
	PrependBinary      = TMP_BITFIELD( 3 ),
	EmitDecimalPoint   = TMP_BITFIELD( 4 ),
	IndexSpecified     = TMP_BITFIELD( 5 ),
	WidthSpecified     = TMP_BITFIELD( 6 ),
	PrecisionSpecified = TMP_BITFIELD( 7 ),
};
}

#ifdef TMP_NO_TM_CONVERSION
static tmp_size_t scan_u32_n( const char* str, tmp_size_t len, int base, unsigned int* out )
{
	if( !len ) {
		return 0;
	}
	char* endptr = nullptr;
	auto value = TMP_STRTOUL( str, &endptr, base );
	tmp_size_t result = endptr - str;
	if( endptr && result > 0 && result <= len ) {
		*out = value;
		return (tmp_size_t)( endptr - str );
	}
	return 0;
}
#endif

static unsigned int parseFormatSpecifiers( const char* p, tmp_size_t len, PrintFormat* format,
                                           unsigned int* currentIndex )
{
	unsigned int result = 0;
	if( len != 0 ) {
		auto end = p + len;
		// parse what is inside {}
		auto indexLen = scan_u32_n( p, (tmp_size_t)( end - p ), 10, currentIndex );
		p += indexLen;
		if( indexLen ) {
			result |= FormatSpecifierFlags::IndexSpecified;
		}
		if( *p == ':' ) {
			++p;
			// custom formatting was defined, so we reject the initial formatting
			*format = defaultPrintFormat();
		}
		// parse flags
		{
			bool parseFlags = true;
			do {
				switch( *p ) {
					case '-': {
						result |= FormatSpecifierFlags::LeftJustify;
						++p;
						break;
					}
					case '+': {
						format->flags |= PF_SIGN;
						++p;
						break;
					}
					case ' ': {
						// format->flags |= PF_SPACE_IF_POSITIVE;
						++p;
						break;
					}
					case '#': {
						result |= FormatSpecifierFlags::PoundSpecified;
						++p;
						break;
					}
					case '0': {
						format->flags |= PF_TRAILING_ZEROES;
						++p;
						break;
					}
					default: {
						parseFlags = false;
						break;
					}
				}
			} while( parseFlags );
		}

		// parse width
		unsigned width   = 0;
		auto widthLength = scan_u32_n( p, (tmp_size_t)( end - p ), 10, &width );
		p += widthLength;
		if( widthLength ) {
			result |= FormatSpecifierFlags::WidthSpecified;
		}
		format->width = (int)width;

		// parse precision
		if( *p == '.' ) {
			++p;
			unsigned precision   = 0;
			auto precisionLength = scan_u32_n( p, (tmp_size_t)( end - p ), 10, &precision );
			p += precisionLength;
			if( precisionLength ) {
				result |= FormatSpecifierFlags::PrecisionSpecified;
			}
			format->precision = (int)precision;
		}

		switch( *p ) {
			case 'x': {
				format->flags |= PF_LOWERCASE;
				// fallthrough into case 'X'
			}
			case 'X': {
				format->base = 16;
				if( result & FormatSpecifierFlags::PoundSpecified ) {
					result |= FormatSpecifierFlags::PrependHex;
				}
				break;
			}
			case 'e': {
				format->flags |= PF_LOWERCASE;
				// fallthrough into case 'E'
			}
			case 'E': {
				format->flags |= PF_SCIENTIFIC;
				break;
			}
			case 'b': {
				format->flags |= PF_LOWERCASE;
				// fallthrough into case 'B'
			}
			case 'B': {
				format->base = 2;
				if( result & FormatSpecifierFlags::PoundSpecified ) {
					result |= FormatSpecifierFlags::PrependBinary;
				}
				break;
			}
			case 'f': {
				format->flags |= PF_TRAILING_ZEROES;
				break;
			}
		}
	}
	return result;
}

static const char* tmp_find( const char* first, const char* last, char c )
{
	return (const char*)TMP_MEMCHR( first, c, last - first );
}
}  // anonymous namespace

template < class Output >
static void print_impl( const char* format, size_t formatLen, const PrintFormat& initialFormatting,
                        const PrintArgList& args, Output&& printout )
{
	const char* formatFirst = format;
	const char* formatLast  = format + formatLen;
	auto index              = 0u;
	const char* p           = formatFirst;
	auto flags              = args.flags;
	while( flags && ( p = tmp_find( formatFirst, formatLast, '{' ) ) != nullptr ) {
		printout( formatFirst, ( tmp_size_t )( p - formatFirst ) );
		++p;
		if( *p == '{' ) {
			printout( "{", 1 );
			++p;
			formatFirst = p;
			continue;
		}

		// parse until '}'
		auto next = tmp_find( formatFirst, formatLast, '}' );
		if( !next ) {
			TMP_ASSERT( 0 && "illformed format" );
			break;
		}

		PrintFormat printFormat = initialFormatting;
		auto currentIndex       = index;
		auto current            = flags & PrintType::Mask;
		auto formatFlags =
		    parseFormatSpecifiers( p, ( tmp_size_t )( next - p ), &printFormat, &currentIndex );

		if( !( formatFlags & FormatSpecifierFlags::IndexSpecified ) ) {
			++index;
			flags >>= PrintType::Bits;
		} else {
			current = ( args.flags >> ( currentIndex * PrintType::Bits ) ) & PrintType::Mask;
		}
		formatFirst = next + 1;

		if( formatFlags & FormatSpecifierFlags::PrependHex ) {
			printout( ( printFormat.flags & PF_LOWERCASE ) ? ( "0x" ) : ( "0X" ), 2 );
		} else if( formatFlags & FormatSpecifierFlags::PrependBinary ) {
			printout( ( printFormat.flags & PF_LOWERCASE ) ? ( "0b" ) : ( "0B" ), 2 );
		}

		TMP_ASSERT( currentIndex < args.size );
		if( ( formatFlags & FormatSpecifierFlags::PrecisionSpecified )
		    && ( current == PrintType::Int || current == PrintType::UInt ) ) {

			if( formatFlags & FormatSpecifierFlags::WidthSpecified ) {
				printFormat.width = TMP_MIN( printFormat.precision, printFormat.width );
			} else {
				printFormat.width = printFormat.precision;
			}
		}
		printout( (int)current, args.args[currentIndex], printFormat );
	}
	if( *formatFirst ) {
		printout( formatFirst, ( tmp_size_t )( formatLast - formatFirst ) );
	}
}

namespace
{
#ifdef TMP_NO_TM_CONVERSION
static tmp_size_t print_i32( char* dest, tmp_size_t maxlen, PrintFormat* format, tmp_int32 value );
static tmp_size_t print_u32( char* dest, tmp_size_t maxlen, PrintFormat* format, tmp_uint32 value );
static tmp_size_t print_i64( char* dest, tmp_size_t maxlen, PrintFormat* format, tmp_int64 value );
static tmp_size_t print_u64( char* dest, tmp_size_t maxlen, PrintFormat* format, tmp_uint64 value );
static tmp_size_t print_double( char* dest, tmp_size_t maxlen, PrintFormat* format, double value );
static tmp_size_t print_float( char* dest, tmp_size_t maxlen, PrintFormat* format, float value );
static tmp_size_t print_bool( char* dest, tmp_size_t maxlen, PrintFormat* format, bool value );
#endif

struct PrintStream {
	char* data;
	tmp_size_t remaining;
};
#ifndef TMP_NO_STDIO
	struct fprinter {
		FILE* out;

		void operator()( int type, const PrintValue& value, PrintFormat& format )
		{
			enum { BUFFER_LEN = TMP_PRINTF_BUFFER_SIZE };
			char buffer[BUFFER_LEN];
			switch( type ) {
				case PrintType::Char: {
					fprintf( out, "%c", value.c );
					break;
				}
				case PrintType::Bool: {
					auto len = print_bool( buffer, BUFFER_LEN, &format, value.b );
					fprintf( out, "%.*s", (int)len, buffer );
					break;
				}
				case PrintType::Int: {
					auto len = print_i32( buffer, BUFFER_LEN, &format, value.i );
					fprintf( out, "%.*s", (int)len, buffer );
					break;
				}
				case PrintType::UInt: {
					auto len = print_u32( buffer, BUFFER_LEN, &format, value.u );
					fprintf( out, "%.*s", (int)len, buffer );
					break;
				}
				case PrintType::Int64: {
					auto len = print_i64( buffer, BUFFER_LEN, &format, value.ll );
					fprintf( out, "%.*s", (int)len, buffer );
					break;
				}
				case PrintType::UInt64: {
					auto len = print_u64( buffer, BUFFER_LEN, &format, value.ull );
					fprintf( out, "%.*s", (int)len, buffer );
					break;
				}
				case PrintType::Float: {
					auto len = print_float( buffer, BUFFER_LEN, &format, value.f );
					fprintf( out, "%.*s", (int)len, buffer );
					break;
				}
				case PrintType::Double: {
					auto len = print_double( buffer, BUFFER_LEN, &format, value.d );
					fprintf( out, "%.*s", (int)len, buffer );
					break;
				}
				case PrintType::String: {
					operator()( value.s );
					break;
				}
				case PrintType::StringView: {
					operator()( value.v.data, value.v.size );
					break;
				}
#ifdef TMP_CUSTOM_PRINTING
			    case PrintType::Custom: {
				    auto len =
				        value.custom.customPrint( buffer, BUFFER_LEN, format, value.custom.data );
					fprintf( out, "%.*s", (int)len, buffer );
					break;
				}
#endif
				default: {
					TMP_ASSERT( 0 && "invalid code path" );
					break;
				}
			}
		}
		void operator()( const char* str ) { fprintf( out, "%s", str ); }
		void operator()( const char* str, tmp_size_t len )
		{
			fprintf( out, "%.*s", (int)len, str );
		}
	};
#endif  // TMP_NO_STDIO

struct memoryprinter {
	char* data;
	tmp_size_t remaining;

	void operator()( int type, const PrintValue& value, PrintFormat& format )
	{
		switch( type ) {
			case PrintType::Char: {
				if( remaining ) {
					*data = value.c;
					++data;
					--remaining;
				}
				break;
			}
			case PrintType::Bool: {
				auto len = print_bool( data, remaining, &format, value.b );
				TMP_ASSERT( len <= remaining );
				data += len;
				remaining -= len;
				break;
			}
			case PrintType::Int: {
				auto len = print_i32( data, remaining, &format, value.i );
				TMP_ASSERT( len <= remaining );
				data += len;
				remaining -= len;
				break;
			}
			case PrintType::UInt: {
				auto len = print_u32( data, remaining, &format, value.u );
				TMP_ASSERT( len <= remaining );
				data += len;
				remaining -= len;
				break;
			}
			case PrintType::Int64: {
				auto len = print_i64( data, remaining, &format, value.ll );
				TMP_ASSERT( len <= remaining );
				data += len;
				remaining -= len;
				break;
			}
			case PrintType::UInt64: {
				auto len = print_u64( data, remaining, &format, value.ull );
				TMP_ASSERT( len <= remaining );
				data += len;
				remaining -= len;
				break;
			}
			case PrintType::Float: {
				#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
					auto flags = format.flags;
					format.flags |= PF_TRAILING_ZEROES;
				#endif
				auto len = print_float( data, remaining, &format, value.f );
				TMP_ASSERT( len <= remaining );
				data += len;
				remaining -= len;
				#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
					format.flags = flags;
				#endif
				break;
			}
			case PrintType::Double: {
				#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
					auto flags = format.flags;
					format.flags |= PF_TRAILING_ZEROES;
				#endif
				auto len = print_double( data, remaining, &format, value.d );
				TMP_ASSERT( len <= remaining );
				data += len;
				remaining -= len;
				#ifdef TMP_FLOAT_ALWAYS_TRAILING_ZEROES
					format.flags = flags;
				#endif
				break;
			}
			case PrintType::String: {
				operator()( value.s, (tmp_size_t)TMP_STRLEN( value.s ) );
				break;
			}
			case PrintType::StringView: {
				operator()( value.v.data, (tmp_size_t)value.v.size );
				break;
			}
#ifdef TMP_CUSTOM_PRINTING
			case PrintType::Custom: {
				auto len =  value.custom.customPrint( data, remaining, format, value.custom.data );
				TMP_ASSERT( len <= remaining );
				remaining -= len;
				data += len;
				break;
			}
#endif
			default: {
				TMP_ASSERT( 0 && "invalid code path" );
				break;
			}
		}
	}
	void operator()( const char* str ) { operator()( str, (tmp_size_t)TMP_STRLEN( str ) ); }
	void operator()( const char* str, tmp_size_t len )
	{
		auto size = TMP_MIN( len, remaining );
		assert( size >= 0 );
		TMP_MEMCPY( data, str, size );
		remaining -= size;
		data += size;
	}
};

#ifdef TMP_NO_TM_CONVERSION
static void print_make_format( memoryprinter& printout, PrintFormat* format, const char* type )
{
	auto flags = format->flags;
	printout( "%0", 2 );
	if( flags & PF_SIGN ) {
		printout( "+", 1 );
	}
	if( *type == 'f' && format->precision == 0 ) {
		// precision == 0 means something different for snprintf, it won't print 0 if precision is 0
		format->precision = 1;
	}

	if( format->width ) {
		if( flags & PF_SIGN ) {
			// snprintf takes sign into account for width
			++format->width;
		}
		printout( "*", 1 );
	}
	if( format->precision ) {
		printout( ".*", 2 );
	}
	printout( type );
	// make nullterminated
	TMP_ASSERT( printout.remaining );
	*printout.data = 0;
}
static void print_make_int_format( memoryprinter& printout, PrintFormat* format, const char* type )
{
	if( format->base == 2 ) {
		// TMP_ASSERT( 0 && "not implemented" );
		format->base = 10;
	}
	if( format->base == 16 ) {
		if( *type == 'l' ) {
			type = ( format->flags & PF_LOWERCASE ) ? ( "llx" ) : ( "llX" );
		} else {
			type = ( format->flags & PF_LOWERCASE ) ? ( "x" ) : ( "X" );
		}
	}
	format->precision = 0;
	print_make_format( printout, format, type );
}
template < class T >
static tmp_size_t print_value( char* dest, tmp_size_t maxlen, PrintFormat* format,
                               const char* formatBuffer, T value )
{
	if( format->width && format->precision ) {
		return TMP_SNPRINTF( dest, (size_t)maxlen, formatBuffer, format->width, format->precision,
		                     value );
	} else if( format->width ) {
		return TMP_SNPRINTF( dest, (size_t)maxlen, formatBuffer, format->width, value );
	} else if( format->precision ) {
		return TMP_SNPRINTF( dest, (size_t)maxlen, formatBuffer, format->precision, value );
	} else {
		return TMP_SNPRINTF( dest, (size_t)maxlen, formatBuffer, value );
	}
}

static tmp_size_t print_i32( char* dest, tmp_size_t maxlen, PrintFormat* format, tmp_int32 value )
{
	TMP_ASSERT( format );
	const int formatBufferSize = 10;
	char formatBuffer[formatBufferSize];
	memoryprinter snprint_format{formatBuffer, formatBufferSize};

	print_make_int_format( snprint_format, format, "d" );
	return print_value( dest, maxlen, format, formatBuffer, value );
}
static tmp_size_t print_u32( char* dest, tmp_size_t maxlen, PrintFormat* format, tmp_uint32 value )
{
	TMP_ASSERT( format );
	const int formatBufferSize = 10;
	char formatBuffer[formatBufferSize];
	memoryprinter snprint_format{formatBuffer, formatBufferSize};

	print_make_int_format( snprint_format, format, "u" );
	return print_value( dest, maxlen, format, formatBuffer, value );
}
static tmp_size_t print_i64( char* dest, tmp_size_t maxlen, PrintFormat* format, tmp_int64 value )
{
	TMP_ASSERT( format );
	const int formatBufferSize = 10;
	char formatBuffer[formatBufferSize];
	memoryprinter snprint_format{formatBuffer, formatBufferSize};

	print_make_int_format( snprint_format, format, "lld" );
	return print_value( dest, maxlen, format, formatBuffer, value );
}
static tmp_size_t print_u64( char* dest, tmp_size_t maxlen, PrintFormat* format, tmp_uint64 value )
{
	TMP_ASSERT( format );
	const int formatBufferSize = 10;
	char formatBuffer[formatBufferSize];
	memoryprinter snprint_format{formatBuffer, formatBufferSize};

	print_make_int_format( snprint_format, format, "llu" );
	return print_value( dest, maxlen, format, formatBuffer, value );
}
static tmp_size_t print_double( char* dest, tmp_size_t maxlen, PrintFormat* format, double value )
{
	TMP_ASSERT( format );
	const int formatBufferSize = 10;
	char formatBuffer[formatBufferSize];
	memoryprinter snprint_format{formatBuffer, formatBufferSize};

	print_make_format( snprint_format, format, "f" );
	return print_value( dest, maxlen, format, formatBuffer, value );
}
static tmp_size_t print_float( char* dest, tmp_size_t maxlen, PrintFormat* format, float value )
{
	TMP_ASSERT( format );
	const int formatBufferSize = 10;
	char formatBuffer[formatBufferSize];
	memoryprinter snprint_format{formatBuffer, formatBufferSize};

	print_make_format( snprint_format, format, "f" );
	auto result = print_value( dest, maxlen, format, formatBuffer, value );
	return result;
}
static tmp_size_t print_bool( char* dest, tmp_size_t maxlen, PrintFormat* format, bool value )
{
	TMP_ASSERT( format );
	if( format->flags & PF_BOOL_AS_NUMBER ) {
		if( maxlen ) {
			*dest = ( value ) ? ( '1' ) : ( '0' );
		}
		return 1;
	} else {
		static const char* const strings[] = {"false", "true"};
		static const tmp_size_t lengths[] = {sizeof( "false" ) - 1, sizeof( "true" ) - 1};
		size_t index = value != 0;

		auto size = TMP_MIN( lengths[index], maxlen );
		TMP_MEMCPY( dest, strings[index], size );
		return size;
	}
}
#endif
}

#ifndef TMP_NO_STDIO
	TMP_DEF void print( FILE* out, const char* format, const PrintArgList& args )
	{
		auto printFormat = defaultPrintFormat();
		print_impl( format, TMP_STRLEN( format ), printFormat, args, fprinter{out} );
	}
	#ifdef TMP_STRING_VIEW
		TMP_DEF void print( FILE* out, TMP_STRING_VIEW format, const PrintArgList& args )
        {
	        auto printFormat = defaultPrintFormat();
	        print_impl( TMP_STRING_VIEW_DATA( format ), TMP_STRING_VIEW_SIZE( format ), printFormat,
	                    args, fprinter{out} );
        }
	#endif
#endif

TMP_DEF tmp_size_t snprint( char* dest, tmp_size_t len, const char* format,
                            const PrintArgList& args )
{
	memoryprinter mem{dest, len};
	auto printFormat = defaultPrintFormat();
	print_impl( format, TMP_STRLEN( format ), printFormat, args, mem );
	return (tmp_size_t)( mem.data - dest );
}
TMP_DEF tmp_size_t snprint( char* dest, tmp_size_t len, const char* format,
                            const PrintFormat& initialFormatting, const PrintArgList& args )
{
	memoryprinter mem{dest, len};
	print_impl( format, TMP_STRLEN( format ), initialFormatting, args, mem );
	return (tmp_size_t)( mem.data - dest );
}
#ifdef TMP_STRING_VIEW
		TMP_DEF tmp_size_t snprint( char* dest, tmp_size_t len,TMP_STRING_VIEW format,
		                            const PrintArgList& args )
		{
			memoryprinter mem{dest, len};
			auto printFormat = defaultPrintFormat();
	        print_impl( TMP_STRING_VIEW_DATA( format ), TMP_STRING_VIEW_SIZE( format ), printFormat,
	                    args, mem );
	        return (tmp_size_t)( mem.data - dest );
        }
        TMP_DEF tmp_size_t snprint( char* dest, tmp_size_t len, TMP_STRING_VIEW format,
                                    const PrintFormat& initialFormatting, const PrintArgList& args )
        {
	        memoryprinter mem{dest, len};
	        print_impl( TMP_STRING_VIEW_DATA( format ), TMP_STRING_VIEW_SIZE( format ),
	                    initialFormatting, args, mem );
	        return ( tmp_size_t )( mem.data - dest );
        }
#endif

#endif