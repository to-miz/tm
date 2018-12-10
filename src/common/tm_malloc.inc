/* Global allocation function to use. */
#ifndef TM_MALLOC
	#include <stdlib.h>
	#define TM_MALLOC(type, size, alignment) (type*)malloc((size) * sizeof(type))
	#define TM_FREE(ptr, size, alignment) free(ptr)
#endif /* !defined(TM_MALLOC) */