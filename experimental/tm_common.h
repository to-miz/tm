/* assert */
#ifndef TM_ASSERT
	#include <assert.h>
	#define TM_ASSERT assert
#endif /* !defined(TM_ASSERT) */

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* static_assert */
#ifndef TM_STATIC_ASSERT
	#if defined(__cplusplus) && defined(__cpp_static_assert)
		#define TM_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
	#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
		#define TM_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
	#elif
		/* from https://stackoverflow.com/a/3385694 */
		#define TM_STATIC_ASSERT(cond, msg) typedef char tm_static_assertion_##__LINE__[(cond) ? 1 : -1]
	#endif
#endif

/* ctype.h dependency */
#if !defined(TM_ISDIGIT) || !defined(TM_ISUPPER) || !defined(TM_ISLOWER) || !defined(TM_TOUPPER) || !defined(TM_TOLOWER) \
    || !defined(TM_ISSPACE)

	#include <ctype.h>
	#define TM_ISDIGIT isdigit
	#define TM_ISUPPER isupper
	#define TM_ISLOWER islower
	#define TM_ISSPACE isspace
	#define TM_TOUPPER toupper
	#define TM_TOLOWER tolower
#endif

/* string.h dependency */
#if !defined(TM_MEMCPY) || !defined(TM_MEMSET) || !defined(TM_MEMCMP) || !defined(TM_MEMMOVE)
	#include <string.h>
	#define TM_MEMCPY memcpy
	#define TM_MEMMOVE memmove
	#define TM_MEMSET memset
	#define TM_MEMCMP memcmp
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

#ifndef TM_STRNREV
	#define TM_IMPLEMENT_STRNREV
	#define TM_STRNREV tm_strnrev
#endif

/* constexpr */
#ifndef TM_CONSTEXPR
	#if defined(__cplusplus) && defined(__cpp_constexpr) && __cpp_constexpr >= 200704
		#define TM_CONSTEXPR constexpr
	#else
		#define TM_CONSTEXPR
	#endif
#endif

/* We use fixed width integer types for bool, so that dll import/export doesn't rely on implementation defined widths.
   You can override the bool type by defining TM_SIZED_BOOL_DEFINED and the typedefs yourself before including this file. */
#ifndef TM_SIZED_BOOL_DEFINED
	#define TM_SIZED_BOOL_DEFINED
	#define TM_TRUE 1
	#define TM_FALSE 0
	typedef int32_t tm_bool32;
	typedef int8_t tm_bool8;
#endif /* !defined(TM_SIZED_BOOL_DEFINED) */
/* Native bools for functions not meant to be exported */
#ifndef TM_BOOL_DEFINED
	#define TM_BOOL_DEFINED
	#ifdef __cplusplus
		typedef bool tm_bool;
	#else
		typedef _Bool tm_bool;
	#endif
#endif /* !defined(TM_BOOL_DEFINED) */

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
	#define TM_SIZE_T_DEFINED
	#define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
	#include <stddef.h> /* include C version so identifiers are in global namespace */
	typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

/* helper to check for valid index */
static inline tm_valid_index(tm_size_t index, tm_size_t size) {
#ifdef TM_SIZE_T_IS_SIGNED
    return index >= 0 && index < size;
#else
    return index < size;
#endif
}

#ifndef TM_ASSERT_VALID_SIZE
	#if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
		#define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
	#else
		/* always true if size_t is unsigned */
		#define TM_ASSERT_VALID_SIZE(x) ((void)0)
	#endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

/* countof macro, override with your flavor of (safe) countof by defining TM_COUNTOF before including this file.
   Note that TM_COUNTOF should return tm_size_t. */
#ifndef TM_COUNTOF
	#ifdef __cplusplus
		template <class T, tm_size_t N>
		tm_size_t tm_countof(T (&)[N]) {
			return N;
		}
		#define TM_COUNTOF(x) tm_countof(x)
	#else
		/* unnsafe C variant */
		#define TM_COUNTOF(x) (sizeof(x) / sizeof((x)[0]))
	#endif
#endif

/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
	#define TM_ERRC_DEFINED
	enum TM_ERRC_CODES {
		TM_OK        = 0,   /* same as std::errc() */
		TM_EOVERFLOW = 75,  /* same as std::errc::value_too_large */
		TM_ERANGE    = 34,  /* same as std::errc::result_out_of_range */
		TM_EINVAL    = 22,  /* same as std::errc::invalid_argument */
	}
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

#if defined(TM_IMPLEMENT_STRNICMP)
static int32_t tm_strnicmp(const char* a, const char* b, tm_size_t maxlen) {
	TM_ASSERT_VALID_SIZE(maxlen);
	while(*a && *b && maxlen--) {
		int32_t aUpper = TM_TOUPPER((uint8_t)(*a));
		int32_t bUpper = TM_TOUPPER((uint8_t)(*b));
		if(aUpper != bUpper) {
			break;
		}
		++a;
		++b;
	}
	if(!maxlen) {
		return 0;
	}
	int32_t aUpper = TM_TOUPPER((uint8_t)(*a));
	int32_t bUpper = TM_TOUPPER((uint8_t)(*b));
	return aUpper - bUpper;
}
#endif /* defined(TM_IMPLEMENT_STRNICMP) */

#ifdef TM_IMPLEMENT_STRNREV
static char* tm_strnrev(char* str, size_t count) {
	for(size_t i = 0, j = count - 1; i < j; ++i, --j) {
		char tmp = str[i];
		str[i]   = str[j];
		str[j]   = tmp;
	}
	return str;
}
#endif /* defined(TM_IMPLEMENT_STRNREV) */