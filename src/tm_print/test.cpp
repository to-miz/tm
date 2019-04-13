#define TM_PRINT_IMPLEMENTATION
#define TM_CONVERSION_IMPLEMENTATION
// #define TMP_INT_BACKEND_CRT
// #define TMP_INT_BACKEND_TM_CONVERSION
// #define TMP_INT_BACKEND_CHARCONV
// #define TMP_FLOAT_BACKEND_CRT
// #define TMP_FLOAT_BACKEND_TM_CONVERSION
// #define TMP_FLOAT_BACKEND_CHARCONV

#if !defined(TM_SIZE_T_DEFINED) && defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED == 1
    #define TM_SIZE_T_DEFINED
    typedef int tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

#include "main.cpp"
// #include "tm_print.h"

int main() {
	tml::print("{}\n", 1);
	tml::print("{}\n", 1.0f);
	tml::print("{}\n", 1.0);
	return 0;
}