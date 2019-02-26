#if defined(TMU_USE_WINDOWS_H)
	#define TMU_CHAR16LEN TMU_WCSLEN
#else
	#if !defined(__linux__) && (defined(_WIN32) || WCHAR_MAX == 0xFFFFu || WCHAR_MAX == 0x7FFF)
		#define TMU_CHAR16LEN TMU_WCSLEN
	#else
		static size_t tmu_char16len(const tmu_char16* str) {
			const tmu_char16* p = str;
			while(*p) ++p;
			return (size_t)(p - str);
		}
		#define TMU_CHAR16LEN tmu_char16len
	#endif
#endif

#if !defined (TMU_TESTING_TCHAR_DEFINED)
	#if (defined(_WIN32) || defined(TMU_TESTING_MSVC_CRT) || defined(TMU_USE_WINDOWS_H))
	    typedef tmu_char16 tmu_tchar;
	#else
	    typedef char tmu_tchar;
	#endif
#endif
