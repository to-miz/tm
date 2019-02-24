/* clang-format off */
#if (!defined(_WIN32) && !defined(TMU_TESTING_MSVC_CRT)) || defined(TMU_TESTING_UNIX)
	#if defined(__GNUC__) || defined(__clang__) || defined(__linux__) || defined(TMU_TESTING_UNIX)
		#define TMU_PLATFORM_UNIX
	#endif
#endif

/* Headers */
#if defined(TMU_USE_CRT) && !defined(TMU_USE_WINDOWS_H)
	#if !defined(TMU_TESTING)
	    #ifdef _MSC_VER
	        #include <sys/types.h> /* Required on msvc so that sys/stat.h and wchar.h define additional functions. */
	    #endif
	    #include <sys/stat.h> /* stat function */
	    #include <errno.h> /* errno */

	    #include <wchar.h>
	#endif /* !defined(TMU_TESTING) */
#endif /* defined(TMU_USE_CRT) && !defined(TMU_USE_WINDOWS_H) */

#if defined(_MSC_VER) || defined(TMU_TESTING_MSVC_CRT)
	#ifndef TMU_TEXT
	    #define TMU_TEXT(x) L##x
	#endif
	#define TMU_TEXTLEN TMU_WCSLEN
	#define TMU_TEXTCHR TMU_STRCHRW
	#define TMU_DIR_DELIM L'\\'

	#define TMU_STAT _wstat64
	#define TMU_STRUCT_STAT struct __stat64
	#ifndef TMU_S_ISDIR
	    #define TMU_S_ISDIR(mode) (((mode) & _S_IFDIR) != 0)
	    #define TMU_S_ISREG(mode) (((mode) & _S_IFREG) != 0)
	#endif
	#define TMU_MKDIR _wmkdir
	#define TMU_RMDIR _wrmdir
	#define TMU_REMOVE _wremove
	#define TMU_RENAME _wrename
	#define TMU_GETCWD _wgetcwd

#elif defined(TMU_PLATFORM_UNIX)
	#ifndef TMU_TESTING
	    #include <unistd.h> /* getcwd */
	#endif /* !defined(TMU_TESTING) */

	#ifndef TMU_TEXT
	    #define TMU_TEXT(x) x
	#endif
	#define TMU_TEXTLEN strlen
	#define TMU_TEXTCHR strchr
	#define TMU_DIR_DELIM '/'

	#define TMU_STAT stat
	#define TMU_STRUCT_STAT struct stat
	#ifndef TMU_S_ISDIR
	    #define TMU_S_ISDIR(x) S_ISDIR(x)
	    #define TMU_S_ISREG(x) S_ISREG(x)
	#endif
	#define TMU_MKDIR(dir) mkdir((dir), /*mode=*/0777u) /* 0777u is read, write, execute permissions for all types. */
	#define TMU_RMDIR rmdir
	#define TMU_REMOVE remove
	#define TMU_RENAME rename
	#define TMU_GETCWD getcwd

#endif /* defined(__GNUC__) || defined(__clang__) || defined(__linux__) || defined(TMU_TESTING_UNIX) */
/* clang-format on */
