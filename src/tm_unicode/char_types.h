#if !defined(TMU_TESTING_CHAR16_DEFINED)
	#if defined(TMU_USE_WINDOWS_H)
		typedef WCHAR tmu_char16;
	#else
		#include <wchar.h>

		#if !defined(__linux__) && (defined(_WIN32) || WCHAR_MAX == 0xFFFFu || WCHAR_MAX == 0x7FFF)
			typedef wchar_t tmu_char16;
		#else
			typedef uint16_t tmu_char16;
		#endif
	#endif
#endif

TM_STATIC_ASSERT(sizeof(tmu_char16) == 2, tmu_char16_must_be_2_bytes);