#ifndef TM_INT_INCLUDED
#define TM_INT_INCLUDED

#ifndef TM_NO_STDDEF
	#include <cstddef> // size_t
#endif

typedef char tm_char;

// int types
#ifndef TM_NO_STDINT
	#ifdef __cplusplus
		#include <cstdint>
	#else
		#include <stdint.h>
	#endif

	// basic sized ints
	typedef int8_t tm_int8;
	typedef int16_t tm_int16;
	typedef int32_t tm_int32;

	typedef uint8_t tm_uint8;
	typedef uint16_t tm_uint16;
	typedef uint32_t tm_uint32;

	#ifndef TM_NO_64_INT
		typedef int64_t tm_int64;
		typedef uint64_t tm_uint64;
	#endif

	typedef intptr_t tm_intptr;
	typedef uintptr_t tm_uintptr;
#else
	// basic sized ints
	typedef signed char tm_int8;
	typedef short tm_int16;
	typedef int tm_int32;
	typedef unsigned char tm_uint8;
	typedef unsigned short tm_uint16;
	typedef unsigned int tm_uint32;

	#if defined(TM_ENV_64_BIT)
		typedef long long tm_intptr;
		typedef unsigned long long tm_uintptr;
	#else
		typedef int tm_intptr;
		typedef unsigned int tm_uintptr;
	#endif

	#ifndef TM_NO_64_INT
		typedef long long tm_int64;
		typedef unsigned long long tm_uint64;
	#endif
#endif /* !defined(TM_NO_STDINT) */

#define TM_TRUE 1
#define TM_FALSE 0
#if defined(__cplusplus)
	typedef bool tm_bool;
#else
	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
		typedef _Bool tm_bool;
	#elif
		typedef int tm_bool;
	#endif
#endif

/*
size_t is unsigned by default, but we also allow for signed size types
*/
#ifdef TM_SIZE_T_IS_32_BIT
	typedef int32_t tm_isize;
	typedef uint32_t tm_usize;

	#ifdef TM_SIZE_T_IS_SIGNED
		typedef tm_isize tm_size_t;
	#else
		typedef tm_usize tm_size_t;
	#endif
#else
	typedef tm_intptr tm_isize;
	typedef tm_uintptr tm_usize;

	#ifdef TM_SIZE_T_IS_SIGNED
		typedef tm_intptr tm_size_t;
	#else
		#ifndef TM_NO_STDDEF
			typedef size_t tm_size_t;
		#else
			typedef tm_uintptr tm_size_t;
		#endif
	#endif
#endif
typedef tm_size_t tm_size;

/* Assertion helpers for signed size_t */
static inline tm_valid_index(tm_size_t index, tm_size_t size) {
#ifdef TM_SIZE_T_IS_SIGNED
    return index >= 0 && index < size;
#else
    return index < size;
#endif
}
#define TM_ASSERT_VALID_INDEX(index, size) TM_ASSERT(tm_valid_index(index, size))

#ifdef TM_SIZE_T_IS_SIGNED
	#define TM_ASSERT_POSITIVE_SIZE(x) TM_ASSERT((x) >= 0)
#else
	/* always true if size_t is unsigned */
	#define TM_ASSERT_POSITIVE_SIZE(x) ((void)0)
#endif

#ifndef TM_USE_TM_PREFIX
	typedef tm_int8 int8;
	typedef tm_int16 int16;
	typedef tm_int32 int32;

	typedef tm_uint8 uint8;
	typedef tm_uint16 uint16;
	typedef tm_uint32 uint32;

	#ifndef TM_NO_64_INT
		typedef tm_int64 int64;
		typedef tm_uint64 uint64;
	#endif

	typedef tm_intptr intptr;
	typedef tm_uintptr uintptr;

	typedef tm_isize isize;
	typedef tm_usize usize;

#endif /* !defined(TM_USE_TM_PREFIX) */

#ifdef TM_INT_SHORTHANDS
	typedef tm_int8 i8;
	typedef tm_int16 i16;
	typedef tm_int32 i32;

	typedef tm_uint8 u8;
	typedef tm_uint16 u16;
	typedef tm_uint32 u32;

	#ifndef TM_NO_64_INT
		typedef tm_int64 i64;
		typedef tm_uint64 u64;
	#endif

	typedef tm_intptr iptr;
	typedef tm_uintptr uptr;

	typedef tm_isize isz;
	typedef tm_usize usz;
#endif

#endif /* !defined(TM_INT_INCLUDED) */