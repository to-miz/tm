/*
tm_arrayview.h v1.1.2 - public domain
written by Tolga Mizrak 2016

no warranty; use at your own risk

NOTES
	ArrayView, UninitializedArrayView and GridView classes for POD types.
	These classes are designed to allow you to treat static arrays just like std containers,
	especially in the case of UninitializedArrayView, enabling to insert/erase entries.
	No memory managment is done by these classes, they only work with the memory provided to them.
	These are useful as the type of arguments to functions that expect arrays.
	This way you can pass static arrays, std::arrays, std::vectors etc into the same function.

	Another design choice was to let them be POD types themselves, so that having them as data
	members doesn't break PODnes.

	The classes are named views, because they do not own their memory, they only represent "views"
	into already existing arrays.

	ArrayView treats memory as an already initialized array.
	UninitializedArrayView treats memory as an uninitialized array, it allows for operations like
	push_back and erase (no resizing/reallocation of memory takes place).
	GridView treats memory as a two dimensional array, you access elements by row/column indices.

SWITCHES
	TMA_INT64_ACCOSSORS:
	    define this if tma_size_t is a 32bit value and you also want 64bit accessors
	    (ie operator[]). Also requires a typedef in the form:
	        typedef long long int tma_index_t;
	TMA_EMPLACE_BACK_RETURNS_POINTER:
	    changed the return type of emplace_back in UninitializedArrayView to return a pointer to the
	    newly emplaced back entry. Otherwise the return type is reference or T&, which is how C++1z
	    std containers implement emplace_back. So #defining TMA_EMPLACE_BACK_RETURNS_POINTER will
	    make UninitializedArrayView's interface incompatible to std containers, so switching from/to
	    std::container usage and UninitializedArrayView usage will require code changes.

HISTORY
	v1.1.2  07.10.16 removed get_index and unsigned int arithmetic when tma_size_t is signed
	v1.1.1  10.09.16 fixed a couple of typos in macro definitions
	                 added TMA_INT64_ACCOSSORS
	v1.1c   10.09.16 added TMA_EMPLACE_BACK_RETURNS_POINTER
	v1.1a   11.07.16 fixed a bug with preventing sign extensions not actually doing anything
	v1.1    11.07.16 added GridView
	v1.0    10.07.16 initial commit

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.
*/

#pragma once

#ifndef _TM_ARRAYVIEW_H_INCLUDED_
#define _TM_ARRAYVIEW_H_INCLUDED_

// define these if you don't use crt
#ifndef TMA_ASSERT
	#include <cassert>
	#define TMA_ASSERT assert
#endif
#ifndef TMA_MEMCPY
	#include <cstring>
	#define TMA_MEMCPY memcpy
#endif
#ifndef TMA_MEMMOVE
	#include <cstring>
	#define TMA_MEMMOVE memmove
#endif

// define this to redefine types used by this library
// see comments at tma_make_unsigned for valid types for tma_size_t
#ifndef TMA_USE_OWN_TYPES
	typedef size_t tma_size_t;
	typedef struct {
		tma_size_t x;
		tma_size_t y;
	} tma_point;
#endif

// define these if tma_size_t is a 32bit value and you also want 64bit accessors (ie operator[])
#if 0
	#define TMA_INT64_ACCOSSORS
	typedef long long int tma_index_t;
#endif

#ifndef TMA_NO_STD_ITERATOR
	#include <iterator>
#endif

#include <initializer_list>

	template < class T >
	struct ArrayView {
		typedef tma_size_t size_type;

		T* ptr;
		size_type sz;

		// STL container stuff
		typedef T value_type;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T* iterator;
		typedef const T* const_iterator;
#ifndef TMA_NO_STD_ITERATOR
		typedef std::reverse_iterator< iterator > reverse_iterator;
		typedef std::reverse_iterator< const_iterator > const_reverse_iterator;
#endif
		typedef size_type difference_type;

		inline iterator begin() const { return iterator( ptr ); }
		inline iterator end() const { return iterator( ptr + sz ); }
		inline const_iterator cbegin() const { return const_iterator( ptr ); }
	inline const_iterator cend() const { return const_iterator( ptr + sz ); }

#ifndef TMA_NO_STD_ITERATOR
	inline reverse_iterator rbegin() const { return reverse_iterator( ptr + sz ); }
	inline reverse_iterator rend() const { return reverse_iterator( ptr ); }
	inline const_reverse_iterator crbegin() const { return const_reverse_iterator( ptr + sz ); }
	inline const_reverse_iterator crend() const { return const_reverse_iterator( ptr ); }
#endif

	inline size_type max_size() const { return sz; }
	inline size_type capacity() const { return sz; }

	inline pointer data() const { return ptr; }
	inline size_type size() const { return sz; }
	inline size_type length() const { return sz; }
	inline bool empty() const { return sz == 0; }

	inline explicit operator bool() { return sz != 0; }

	inline reference operator[]( size_type i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[i];
	}

	inline const_reference operator[]( size_type i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[i];
	}

	inline reference at( size_type i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[i];
	}

	inline const_reference at( size_type i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[i];
	}

#ifdef TMA_INT64_ACCOSSORS
	inline reference operator[]( tma_index_t i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < (tma_index_t)sz );
		return ptr[i];
	}

	inline const_reference operator[]( tma_index_t i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < (tma_index_t)sz );
		return ptr[i];
	}

	inline reference at( tma_index_t i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < (tma_index_t)sz );
		return ptr[i];
	}

	inline const_reference at( tma_index_t i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < (tma_index_t)sz );
		return ptr[i];
	}
#endif  // TMA_INT64_ACCOSSORS

	inline reference back()
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[sz - 1];
	}
	inline const_reference back() const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[sz - 1];
	}
	inline reference front()
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[0];
	}
	inline const_reference front() const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[0];
	}

	inline void assign( const_iterator first, const_iterator last )
	{
		TMA_ASSERT( first <= last );
		TMA_ASSERT( static_cast< size_type >( last - first ) == sz );
		TMA_ASSERT( &*first != begin() );
		TMA_MEMCPY( ptr, first, sz * sizeof( value_type ) );
	}
	inline void assign( const ArrayView other )
	{
		TMA_ASSERT( other.size() == size() );
		TMA_ASSERT( other.begin() != begin() );
		TMA_MEMCPY( ptr, other.ptr, sz * sizeof( value_type ) );
	}
	inline void assign( const std::initializer_list< T >& list )
	{
		TMA_ASSERT( list.size() == sz );
		TMA_MEMCPY( ptr, list.begin(), sz * sizeof( value_type ) );
	}
};

template < class T >
ArrayView< T > makeArrayView( T* ptr, tma_size_t sz )
{
	return {ptr, sz};
}
template < class T >
ArrayView< T > makeArrayView( T* first, T* last )
{
	TMA_ASSERT( first >= last );
	return {first, static_cast< tma_size_t >( last - first )};
}
template < class Container >
ArrayView< typename Container::value_type > makeArrayView( Container& container )
{
	return {container.data(), static_cast< tma_size_t >( container.size() )};
}
template < class T, size_t N >
ArrayView< T > makeArrayView( T ( &array )[N] )
{
	return {array, static_cast< tma_size_t >( N )};
}
template < class T >
ArrayView< const T > makeArrayView( const std::initializer_list< T >& list )
{
	return {list.begin(), static_cast< tma_size_t >( list.size() )};
}

// makeRangeView family of functions
// returns ArrayView of the subsequence [start, end) of the original container
template < class Container >
ArrayView< typename Container::value_type > makeRangeView( Container& container,
														   tma_size_t start )
{
	TMA_ASSERT( start >= 0 );
	if( start >= container.size() ) {
		start = static_cast< tma_size_t >( container.size() );
	}
	return {container.data() + start, static_cast< tma_size_t >( container.size() - start )};
}
template < class Container >
ArrayView< typename Container::value_type > makeRangeView( Container& container,
														   tma_size_t start, tma_size_t end )
{
	TMA_ASSERT( start >= 0 );
	TMA_ASSERT( end >= 0 );
	if( start >= container.size() ) {
		start = static_cast< tma_size_t >( container.size() );
	}
	if( end >= container.size() ) {
		end = static_cast< tma_size_t >( container.size() );
	}
	TMA_ASSERT( start <= end );
	return {container.data() + start, end - start};
}

template < class T, size_t N >
ArrayView< T > makeRangeView( T ( &array )[N], tma_size_t start )
{
	TMA_ASSERT( start >= 0 );
	if( start >= N ) {
		start = static_cast< tma_size_t >( N );
	}
	return {array + start, static_cast< tma_size_t >( N - start )};
}
template < class T, size_t N >
ArrayView< T > makeRangeView( T ( &array )[N], tma_size_t start, tma_size_t end )
{
	TMA_ASSERT( start >= 0 );
	TMA_ASSERT( end >= 0 );
	if( start >= N ) {
		start = static_cast< tma_size_t >( N );
	}
	if( end >= N ) {
		end = static_cast< tma_size_t >( N );
	}
	TMA_ASSERT( start <= end );
	return {array + start, end - start};
}

// UninitializedArrayView

template < class T >
struct UninitializedArrayView {
	typedef tma_size_t size_type;

	T* ptr;
	size_type sz;
	size_type cap;

	// STL container stuff
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T* iterator;
	typedef const T* const_iterator;
#ifndef TMA_NO_STD_ITERATOR
	typedef std::reverse_iterator< iterator > reverse_iterator;
	typedef std::reverse_iterator< const_iterator > const_reverse_iterator;
#endif
	typedef tma_size_t difference_type;

	inline iterator begin() const { return iterator( ptr ); }
	inline iterator end() const { return iterator( ptr + sz ); }
	inline const_iterator cbegin() const { return const_iterator( ptr ); }
	inline const_iterator cend() const { return const_iterator( ptr + sz ); }

#ifndef TMA_NO_STD_ITERATOR
	inline reverse_iterator rbegin() const { return reverse_iterator( ptr + sz ); }
	inline reverse_iterator rend() const { return reverse_iterator( ptr ); }
	inline const_reverse_iterator crbegin() const { return const_reverse_iterator( ptr + sz ); }
	inline const_reverse_iterator crend() const { return const_reverse_iterator( ptr ); }
#endif

	inline size_type max_size() const { return cap; }
	inline size_type capacity() const { return cap; }

	inline pointer data() const { return ptr; }
	inline size_type size() const { return sz; }
	inline size_type length() const { return sz; }
	inline bool empty() const { return sz == 0; }
	inline bool full() const { return sz == cap; }
	inline size_type remaining() const { return cap - sz; }

	inline reference operator[]( size_type i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[i];
	}

	inline const_reference operator[]( size_type i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[i];
	}

	inline reference at( size_type i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[i];
	}

	inline const_reference at( size_type i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[i];
	}

#ifdef TMA_INT64_ACCOSSORS
	inline reference operator[]( tma_index_t i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < (tma_index_t)sz );
		return ptr[i];
	}

	inline const_reference operator[]( tma_index_t i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < (tma_index_t)sz );
		return ptr[i];
	}

	inline reference at( tma_index_t i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < (tma_index_t)sz );
		return ptr[i];
	}

	inline const_reference at( tma_index_t i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < (tma_index_t)sz );
		return ptr[i];
	}
#endif  // TMA_INT64_ACCOSSORS

	inline reference back()
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[sz - 1];
	}
	inline const_reference back() const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[sz - 1];
	}
	inline reference front()
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[0];
	}
	inline const_reference front() const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[0];
	}

	inline void push_back( const T& elem )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz + 1 <= cap );
		ptr[sz] = elem;
		++sz;
	}
	inline void pop_back()
	{
		--sz;
		TMA_ASSERT( sz >= 0 );
	}

// define TMA_EMPLACE_BACK_RETURNS_POINTER if you want emplace_back to return a pointer instead
// of reference. The reference version is how std containers in C++1z implement emplace_back.
#ifdef TMA_EMPLACE_BACK_RETURNS_POINTER
	inline pointer emplace_back()
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz + 1 <= cap );
		++sz;
		return &ptr[sz - 1];
	}
#else
inline reference emplace_back()
{
	TMA_ASSERT( ptr );
	TMA_ASSERT( sz + 1 <= cap );
	++sz;
	return ptr[sz - 1];
}
#endif

	inline void clear() { sz = 0; }
	inline void resize( size_type sz )
	{
		TMA_ASSERT( sz >= 0 && sz <= cap );
		this->sz = sz;
	}
	inline void grow( size_type by )
	{
		sz += by;
		TMA_ASSERT( sz >= 0 && sz <= cap );
	}

	inline void assign( UninitializedArrayView other ) { assign( other.begin(), other.end() ); }
	void assign( const_iterator first, const_iterator last )
	{
		TMA_ASSERT( ( first < begin() || first >= end() )
					&& ( last < begin() || last >= end() ) );
		sz = static_cast< size_type >( last - first );
		TMA_ASSERT( sz <= cap );
		TMA_MEMCPY( ptr, first, sz * sizeof( value_type ) );
	}
	void assign( const_iterator first, size_type length )
	{
		TMA_ASSERT( length <= cap );
		TMA_ASSERT( ( first < begin() || first >= end() )
					&& ( first + length < begin() || first + length >= end() ) );

		sz = length;
		TMA_MEMCPY( ptr, first, sz * sizeof( value_type ) );
	}
	void assign( size_type n, const value_type& val )
	{
		TMA_ASSERT( n >= 0 );
		TMA_ASSERT( ptr );
		n          = ( n < cap ) ? ( n ) : ( cap );
		sz         = n;
		auto count = n;
		for( tma_size_t i = 0; i < count; ++i ) {
			ptr[i] = val;
		}
	}

	iterator insert( iterator position, size_type n, const value_type& val )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( position >= begin() && position <= end() );

		size_t rem    = remaining();
		size_t count  = ( n < rem ) ? n : ( rem );
		size_t suffix = end() - position;
		if( count > 0 ) {
			auto tmp = val;  // in case val is inside sequence
			// make room for insertion by moving suffix
			TMA_MEMMOVE( position + count, position, suffix * sizeof( value_type ) );

			sz += static_cast< size_type >( count );
			for( size_t i = 0; i < count; ++i ) {
				position[i] = tmp;
			}
		}
		return position;
	}
	iterator insert( iterator position, const_iterator first, const_iterator last )
	{
		TMA_ASSERT( first <= last );
		TMA_ASSERT( ptr || first == last );
		TMA_ASSERT( position >= begin() && position <= end() );

		auto rem     = remaining();
		tma_size_t count = static_cast< tma_size_t >( last - first );
		TMA_ASSERT( rem >= count );
		if( count > 0 && count <= rem ) {
			// range fits move entries to make room and copy
			TMA_MEMMOVE( position + count, position,
						 ( end() - position ) * sizeof( value_type ) );
			TMA_MEMCPY( position, first, count * sizeof( value_type ) );
			sz += static_cast< size_type >( count );
		}
		return position;
	}

	inline iterator append( const_iterator first, const_iterator last )
	{
		return insert( end(), first, last );
	}
	inline iterator append( size_type n, const value_type& val )
	{
		return insert( end(), n, val );
	}

	iterator erase( iterator position )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( position >= begin() && position <= end() );
		TMA_MEMMOVE( position, position + 1, ( end() - position - 1 ) * sizeof( value_type ) );
		--sz;
		return position;
	}
	iterator erase( iterator first, iterator last )
	{
		TMA_ASSERT( first <= last );
		TMA_ASSERT( ptr || first == last );
		if( first == begin() && last == end() ) {
			clear();
		} else if( first < last ) {
			TMA_ASSERT( first >= begin() && last <= end() );
			// move suffix to where the erased range used to be
			TMA_MEMMOVE( first, last, ( end() - last ) * sizeof( value_type ) );
			sz -= static_cast< size_type >( last - first );
		}
		return first;
	}
};

template < class T >
UninitializedArrayView< T > makeUninitializedArrayView( T* ptr, tma_size_t capacity )
{
	return {ptr, 0, capacity};
}
template < class T >
UninitializedArrayView< T > makeInitializedArrayView( T* ptr, tma_size_t size,
													  tma_size_t capacity )
{
	return {ptr, size, capacity};
}
template < class T >
UninitializedArrayView< T > makeInitializedArrayView( T* ptr, tma_size_t size )
{
	return {ptr, size, size};
}
template < class T, size_t N >
UninitializedArrayView< T > makeUninitializedArrayView( T ( &array )[N] )
{
	return {array, 0, static_cast< tma_size_t >( N )};
}
template < class T, size_t N >
UninitializedArrayView< T > makeInitializedArrayView( T ( &array )[N] )
{
	return {array, static_cast< tma_size_t >( N ), static_cast< tma_size_t >( N )};
}

// GridView
template < class T >
struct GridView {
	T* ptr;
	tma_size_t width;
	tma_size_t height;

	// STL container stuff
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T* iterator;
	typedef const T* const_iterator;
#ifndef TMA_NO_STD_ITERATOR
	typedef std::reverse_iterator< iterator > reverse_iterator;
	typedef std::reverse_iterator< const_iterator > const_reverse_iterator;
#endif
	typedef tma_size_t difference_type;
	typedef tma_size_t size_type;

	inline bool isInBounds( tma_size_t x, tma_size_t y )
	{
		return x >= 0 && x < width && y >= 0 && y < height;
	}
	inline bool isInBounds( tma_point p ) { return isInBounds( p.x, p.y ); }

	inline tma_size_t size() const { return width * height; }
	inline tma_size_t length() const { return width * height; }
	inline tma_size_t index( tma_size_t x, tma_size_t y ) { return x + y * width; }
	inline tma_size_t index( tma_point p ) { return p.x + p.y * width; }
	inline tma_point coordinatesFromIndex( tma_size_t i )
	{
		TMA_ASSERT( i < size() );
		return {i % width, i / width};
	}
	inline tma_point coordinatesFromPtr( T* p )
	{
		TMA_ASSERT( p >= begin() && p < end() );
		auto index = static_cast< tma_size_t >( p - begin() );
		return coordinatesFromIndex( index );
	}
	inline tma_size_t indexFromPtr( T* p )
	{
		TMA_ASSERT( p >= begin() && p < end() );
		return static_cast< tma_size_t >( p - begin() );
	}

	inline T* data() const { return ptr; }
	inline T* begin() const { return ptr; }
	inline const T* cbegin() const { return ptr; }
	inline T* end() const { return ptr + width * height; }

	inline reference at( tma_size_t i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < size() );
		return ptr[i];
	}
	inline const_reference at( tma_size_t i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < size() );
		return ptr[i];
	}

	inline T& at( tma_size_t x, tma_size_t y )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( isInBounds( x, y ) );
		auto i = x + y * width;
		TMA_ASSERT( i < size() );
		return ptr[i];
	}

	inline T& at( tma_point p ) { return at( p.x, p.y ); }

	inline T* queryAt( tma_size_t index )
	{
		T* ret = nullptr;
		if( index >= 0 && index < size() ) {
			ret = ptr + index;
		}
		return ret;
	}
	inline T* queryAt( tma_size_t x, tma_size_t y )
	{
		T* ret = nullptr;
		if( isInBounds( x, y ) ) {
			ret = ptr + x + y * width;
		}
		return ret;
	}
	inline T* queryAt( tma_point p ) { return queryAt( p.x, p.y ); }

	inline reference operator[]( tma_size_t i ) { return at( i ); }
	inline const_reference operator[]( tma_size_t i ) const { return at( i ); }
};

template < class T >
GridView< T > makeGridView( T* ptr, tma_size_t width, tma_size_t height )
{
	return {ptr, width, height};
}

#endif // _TM_ARRAYVIEW_H_INCLUDED_
