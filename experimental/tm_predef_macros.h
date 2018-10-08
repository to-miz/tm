/* Compilers. */

#ifdef __BORLANDC__
	#define TM_COMPILER_BORLAND __BORLANDC__
#endif

#ifdef __clang__
	#define TM_COMPILER_CLANG
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
	#define TM_COMPILER_GCC __GNUC__
#endif

#ifdef __llvm__
	#define TM_COMPILER_LLVM
#endif

#ifdef _MSC_VER
	#define TM_COMPILER_MSC _MSC_VER
#endif

#if defined(__MINGW32__) || (__MINGW64__)
	#define TM_COMPILER_MINGW
#endif

#ifdef __WATCOMC__
	#define TM_COMPILER_WATCOM
#endif

/* Operating systems. */

#ifdef __ANDROID__
	#define TM_OS_ANDROID
#endif

#ifdef __CYGWIN__
	#define TM_OS_CYGWIN
#endif

#ifdef __gnu_linux__
	#define TM_OS_GNU_LINUX
#endif

#ifdef __linux__
	#define TM_OS_LINUX
#endif

#if defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
	#define TM_OS_MACOS

	#if defined(__APPLE__) && defined(__MACH__)
		#define TM_OS_MACOSX
	#else
		#define TM_OS_MACOS9
	#endif
#endif

#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
	#define TM_OS_WINDOWS
#endif

/* Architectures. */

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) \
    || defined(_M_AMD64)
#define TM_ARCH_AMD64
#define TM_ARCH_X64
#define TM_ARCH_X86_X64
#endif

#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT)
	#define TM_ARCH_ARM
#endif

#if defined(__aarch64__)
	#define TM_ARCH_ARM64
#endif

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_I86) || defined(_M_IX86) || defined(_X86_) \
    || defined(__X86__)

	#define TM_ARCH_X86
	#define TM_ARCH_IX86
	#define TM_ARCH_I86
#endif

#if defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64) || defined(_M_IA64) || defined(__itanium__)
	#define TM_ARCH_IA64
#endif

#if defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) \
    || defined(__ppc64__) || defined(__PPC__) || defined(__PPC64__) || defined(_ARCH_PPC) || defined(_ARCH_PPC64)    \
    || defined(_M_PPC)

	#define TM_ARCH_POWERPC

    #if defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
    	#define TM_ARCH_POWERPC64
    #endif
#endif

/* Environment. */

#if !defined(TM_ENV_64_BIT) && !defined(TM_ENV_32_BIT)
	#if defined(TM_ARCH_AMD64) || defined(TM_ARCH_ARM64) || defined(TM_ARCH_IA64) || defined(TM_ARCH_POWERPC64)
	    #define TM_ENV_64_BIT
	#elif
	    /* Assume 32 bit otherwise, don't support 16 bit. */
	    #define TM_ENV_32_BIT

	    /* Error for some known 16 bit architectures. */
	    #if defined(_M_I86) || defined(_WIN16)
	    	#error not implemented for 16bit
	    #endif
	#endif
#endif