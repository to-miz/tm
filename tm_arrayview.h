/*
tm_arrayview.h v1.0 - public domain
written by Tolga Mizrak 2016

no warranty; use at your own risk

NOTES
	ArrayView and UninitializedArrayView classes for POD types.
	These classes are designed to allow you to treat static arrays just like std containers,
	especially in the case of UninitializedArrayView, enabling to insert/erase entries.
	No memory managment is done by these classes, they only work with the memory provided to them.
	These are useful as the type of arguments to functions that expect arrays.
	This way you can pass static arrays, std::arrays, std::vectors etc into the same function.

	Another design choice was to let ArrayView and UninitializedArrayView be POD types themselves,
	so that having them as data members doesn't break PODnes.

	The classes are named views, because they do not own their memory, they only represent "views"
	into already existing arrays.

	A note on why there are static_cast< size_t >( X ) in the implementation:
	These are there in case you want signed int as your size_type in the containers. Std containers
	all use size_t, which means that you have to do a lot of size_t to int conversions if you are
	using int's as sizes/indexes throughout your code.
	These containers are designed in a way to allow you to redefine size_type of the containers to
	a signed int type.
	If your size_type is 32bit signed int and you are targeting x64, the compiler would then
	generate code to sign extend your 32bit signed int to 64bit signed int to then use as an index
	into the base pointer. To make the compiler not emit sign extension code, we cast to size_t.
	In all other cases, this cast is unnecessary.

HISTORY
	v1.0	10.07.16	initial commit

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

// define this to redefine the integral type used as size_type
#ifndef TM_USE_OWN_TYPES
	typedef size_t tma_size_t;
#endif

#ifndef TM_NO_STD_ITERATOR
	#include <iterator>
#endif

#include <initializer_list>

template< class T >
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
#ifndef TM_NO_STD_ITERATOR
	typedef std::reverse_iterator< iterator > reverse_iterator;
	typedef std::reverse_iterator< const_iterator > const_reverse_iterator;
#endif
	typedef size_type difference_type;

	inline iterator begin() const { return iterator( ptr ); }
	inline iterator end() const { return iterator( ptr + static_cast< size_t >( sz ) ); }
	inline const_iterator cbegin() const { return const_iterator( ptr ); }
	inline const_iterator cend() const
	{
		return const_iterator( ptr + static_cast< size_t >( sz ) );
	}

#ifndef TM_NO_STD_ITERATOR
	inline reverse_iterator rbegin() const
	{
		return reverse_iterator( ptr + static_cast< size_t >( sz ) );
	}
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
		return ptr[static_cast< size_t >( i )];
	}

	inline const reference operator[]( size_type i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[static_cast< size_t >( i )];
	}

	inline reference at( size_type i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[static_cast< size_t >( i )];
	}

	inline const reference at( size_type i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[static_cast< size_t >( i )];
	}

	inline reference back()
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[static_cast< size_t >( sz - 1 )];
	}
	inline const_reference back() const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[static_cast< size_t >( sz - 1 )];
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
		TMA_ASSERT( last - first == static_cast< size_t >( sz ) );
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
		TMA_ASSERT( list.size() == static_cast< size_t >( sz ) );
		TMA_MEMCPY( ptr, list.begin(), sz * sizeof( value_type ) );
	}
};

template< class T >
ArrayView< T > makeArrayView( T* ptr, tma_size_t sz )
{
	return {ptr, sz};
}
template< class T >
ArrayView< T > makeArrayView( T* first, T* last )
{
	TMA_ASSERT( first >= last );
	return {first, static_cast< tma_size_t >( last - first )};
}
template< class Container >
ArrayView< typename Container::value_type > makeArrayView( Container& container )
{
	return {container.data(), static_cast< tma_size_t >( container.size() )};
}
template< class T, size_t N >
ArrayView< T > makeArrayView( T ( &array )[N] )
{
	return {array, static_cast< tma_size_t >( N )};
}
template< class T >
ArrayView< const T > makeArrayView( const std::initializer_list< T >& list )
{
	return {list.begin(), static_cast< tma_size_t >( list.size() )};
}

template< class Container >
ArrayView< typename Container::value_type > makeRangeView( Container& container, tma_size_t start )
{
	TMA_ASSERT( start >= 0 );
	if( static_cast< size_t >( start ) >= container.size() ) {
		start = static_cast< tma_size_t >( container.size() );
	}
	return {container.data() + start,
			static_cast< tma_size_t >( container.size() - static_cast< size_t >( start ) )};
}
template < class Container >
ArrayView< typename Container::value_type > makeRangeView( Container& container, tma_size_t start,
														   tma_size_t end )
{
	TMA_ASSERT( start >= 0 );
	TMA_ASSERT( end >= 0 );
	if( static_cast< size_t >( start ) >= container.size() ) {
		start = static_cast< tma_size_t >( container.size() );
	}
	if( static_cast< size_t >( end ) >= container.size() ) {
		end = static_cast< tma_size_t >( container.size() );
	}
	TMA_ASSERT( start <= end );
	return {container.data() + start, end - start};
}

template < class T, size_t N >
ArrayView< T > makeRangeView( T( &array )[N], tma_size_t start )
{
	TMA_ASSERT( start >= 0 );
	if( static_cast< size_t >( start ) >= N ) {
		start = static_cast< tma_size_t >( N );
	}
	return {array + start, static_cast< tma_size_t >( N - static_cast< size_t >( start ) )};
}
template < class T, size_t N >
ArrayView< T > makeRangeView( T( &array )[N], tma_size_t start, tma_size_t end )
{
	TMA_ASSERT( start >= 0 );
	TMA_ASSERT( end >= 0 );
	if( static_cast< size_t >( start ) >= N ) {
		start = static_cast< tma_size_t >( N );
	}
	if( static_cast< size_t >( end ) >= N ) {
		end = static_cast< tma_size_t >( N );
	}
	TMA_ASSERT( start <= end );
	return {array + start, end - start};
}

// UninitializedArrayView

template< class T >
struct UninitializedArrayView
{
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
#ifndef TM_NO_STD_ITERATOR
	typedef std::reverse_iterator< iterator > reverse_iterator;
	typedef std::reverse_iterator< const_iterator > const_reverse_iterator;
#endif
	typedef tma_size_t difference_type;

	inline iterator begin() const { return iterator( ptr ); }
	inline iterator end() const { return iterator( ptr + static_cast< size_t >( sz ) ); }
	inline const_iterator cbegin() const { return const_iterator( ptr ); }
	inline const_iterator cend() const
	{
		return const_iterator( ptr + static_cast< size_t >( sz ) );
	}

#ifndef TM_NO_STD_ITERATOR
	inline reverse_iterator rbegin() const
	{
		return reverse_iterator( ptr + static_cast< size_t >( sz ) );
	}
	inline reverse_iterator rend() const { return reverse_iterator( ptr ); }
	inline const_reverse_iterator crbegin() const
	{
		return const_reverse_iterator( ptr + static_cast< size_t >( sz ) );
	}
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
		return ptr[static_cast< size_t >( i )];
	}

	inline const_reference operator[]( size_type i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[static_cast< size_t >( i )];
	}

	inline reference at( size_type i )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[static_cast< size_t >( i )];
	}

	inline const_reference at( size_type i ) const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( i >= 0 );
		TMA_ASSERT( i < sz );
		return ptr[static_cast< size_t >( i )];
	}

	inline reference back()
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[static_cast< size_t >( sz - 1 )];
	}
	inline const_reference back() const
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz );
		return ptr[static_cast< size_t >( sz - 1 )];
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
		ptr[static_cast< size_t >( sz )] = elem;
		++sz;
	}
	inline void pop_back() { --sz; TMA_ASSERT( sz >= 0 ); }
	inline pointer emplace_back()
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( sz + 1 <= cap );
		++sz;
		return &ptr[static_cast< size_t >( sz - 1 )];
	}

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
		TMA_ASSERT( ( first < begin() || first >= end() ) && ( last < begin() || last >= end() ) );
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
		n = ( n < cap ) ? ( n ) : ( cap );
		sz = n;
		auto count = static_cast< size_t >( n );
		for( size_t i = 0; i < count; ++i ) {
			ptr[i] = val;
		}
	}

	iterator insert( iterator position, size_type n, const value_type& val )
	{
		TMA_ASSERT( ptr );
		TMA_ASSERT( position >= begin() && position <= end() );

		size_t rem = static_cast< size_t >( remaining() );
		size_t count = ( static_cast< size_t >( n ) < rem ) ? static_cast< size_t >( n ) : ( rem );
		size_t suffix = static_cast< size_t >( end() - position );
		if( count > 0 ) {
			auto tmp = val; // in case val is inside sequence
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
		TMA_ASSERT( ptr || first == last );
		TMA_ASSERT( position >= begin() && position <= end() );

		auto rem = static_cast< size_t >( remaining() );
		size_t count = static_cast< size_t >( last - first );
		TMA_ASSERT( rem >= count );
		if( count > 0 && count <= rem ) {
			// range fits move entries to make room and copy
			TMA_MEMMOVE( position + count, position, ( end() - position ) * sizeof( value_type ) );
			TMA_MEMCPY( position, first, count * sizeof( value_type ) );
			sz += static_cast< size_type >( count );
		}
		return position;
	}

	inline iterator append( const_iterator first, const_iterator last )
	{
		return insert( end(), first, last );
	}
	inline iterator append( size_type n, const value_type& val ) { return insert( end(), n, val ); }

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
UninitializedArrayView< T > makeInitializedArrayView( T* ptr, tma_size_t size, tma_size_t capacity )
{
	return {ptr, size, capacity};
}
template < class T >
UninitializedArrayView< T > makeInitializedArrayView( T* ptr, tma_size_t size )
{
	return {ptr, size, size};
}
template< class T, size_t N >
UninitializedArrayView< T > makeUninitializedArrayView( T (&array)[N] )
{
	return {array, 0, static_cast< tma_size_t >( N )};
}
template< class T, size_t N >
UninitializedArrayView< T > makeInitializedArrayView( T (&array)[N] )
{
	return {array, static_cast< tma_size_t >( N ), static_cast< tma_size_t >( N )};
}

#endif // _TM_ARRAYVIEW_H_INCLUDED_
