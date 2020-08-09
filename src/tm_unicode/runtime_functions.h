/* clang-format off */
#if defined(TMU_USE_CRT) && (defined(TMU_PLATFORM_UNIX) || !defined(TMU_USE_WINDOWS_H))
	#if !defined(TMU_TESTING)
	    #include <string.h>
	    #include <stdlib.h>
	#endif /* !defined(TMU_TESTING) */

	/* Use malloc if provided, otherwise fall back to heap. */
	#ifdef TM_REALLOC
	    #define TMU_MALLOC TM_MALLOC
	    #define TMU_REALLOC TM_REALLOC
	    #define TMU_FREE TM_FREE
	#else
	    #define TMU_MALLOC(size, alignment) malloc((size))
	    #define TMU_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) realloc((ptr), (new_size))
	    #define TMU_FREE(ptr, size, alignment) free((ptr))
	#endif

	#ifdef TM_MEMMOVE
	    #define TMU_MEMMOVE TM_MEMMOVE
	#else
	    #define TMU_MEMMOVE memmove
	#endif

	#ifdef TM_MEMCPY
	    #define TMU_MEMCPY TM_MEMCPY
	#else
	    #define TMU_MEMCPY memcpy
	#endif

	#ifdef TM_WCSCHR
	    #define TMU_STRCHRW TM_WCSCHR
	#else
	    #define TMU_STRCHRW wcschr
	#endif
	/* clang-format on */

	#define TMU_STRLEN strlen
	#define TMU_WCSLEN wcslen
	#define TMU_MEMCHR memchr
	#define TMU_MEMCMP memcmp

#elif defined(_WIN32) && defined(TMU_USE_WINDOWS_H)

	/* Use malloc if provided, otherwise fall back to process heap. */
	#ifdef TM_REALLOC
	    #define TMU_MALLOC TM_MALLOC
	    #define TMU_REALLOC TM_REALLOC
	    #define TMU_FREE TM_FREE
	#else
	    #define TMU_MALLOC(size, alignment) HeapAlloc(GetProcessHeap(), 0, (size))
	    #define TMU_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) \
	        HeapReAlloc(GetProcessHeap(), 0, (ptr), (new_size))
	    #define TMU_FREE(ptr, size, alignment) HeapFree(GetProcessHeap(), 0, (ptr))
	#endif

	#ifdef TM_MEMMOVE
	    #define TMU_MEMMOVE TM_MEMMOVE
	#else
	    #define TMU_MEMMOVE MoveMemory
	#endif

	#ifdef TM_MEMCPY
	    #define TMU_MEMCPY TM_MEMCPY
	#else
	    #define TMU_MEMCPY CopyMemory
	#endif

	#if defined(TM_MEMCMP)
	    #define TMU_MEMCMP TM_MEMCMP
	#elif defined(TMU_USE_CRT)
	    #ifndef TMU_TESTING
	        #include <string.h>
	    #endif
	    #define TMU_MEMCMP memcmp
	#else
	    /* There is no memcmp, implement a simple one here. */
	    static int tmu_memcmp(const void* first, const void* second, size_t size) {
	        const char* a = (const char*)first;
	        const char* b = (const char*)second;

	        while (size) {
	            int diff = (int)*a++ - (int)*b++;
	            if (diff != 0) return (diff < 0) ? -1 : 1;
	            --size;
	        }
	        return 0;
	    }
	    #define TMU_MEMCMP tmu_memcmp
	#endif

	#ifndef TMU_TEXT
		#define TMU_TEXT(x) L##x
	#endif
	#define TMU_TEXTLEN TMU_WCSLEN
	#define TMU_TEXTCHR TMU_STRCHRW
	#define TMU_DIR_DELIM L'\\'
	#define TMU_STRLEN(x) ((tm_size_t)lstrlenA((x)))

	/* String functions, use shlwapi if provided, otherwise use fallback version. */
	#ifdef TMU_USE_SHLWAPI_H
		#define TMU_STRCHRW StrChrW
	#elif defined(TM_WCSCHR)
		#define TMU_STRCHRW TM_WCSCHR
	#else
		static WCHAR* tmu_strchrw(WCHAR* str, WCHAR c) {
		    TM_ASSERT(str);
		    while (*str && *str != c) ++str;
		    if (!*str) return TM_NULL;
		    return str;
		}
		#define TMU_STRCHRW tmu_strchrw
	#endif

	#define TMU_WCSLEN(str) (size_t)lstrlenW((str))
#endif /* defined(TMU_USE_WINDOWS_H) */
/* clang-format on */