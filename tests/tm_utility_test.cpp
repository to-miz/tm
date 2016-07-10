#define TM_UTILITY_IMPLEMENTATION
#define TM_USE_OWN_BEGIN_END
#include "../tm_utility.h"

#include "tm_test_suite.h"

#include <cstdio>

int main( int argc, char const *argv[] )
{
	TM_UNREFERENCED_PARAM( argc );
	TM_UNREFERENCED_PARAM( argv );

	int array[] = {10, 20, 30};
	auto index = indexof( array, array[1] );
	printf( "%zd", index );
	return 0;
}