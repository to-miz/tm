// #define TM_NO_STD_ITERATOR
#if 0
#define TMA_USE_OWN_TYPES
typedef int tma_size_t;
typedef struct {
	tma_size_t x;
	tma_size_t y;
} tma_point;
#endif
#include "../tm_arrayview.h"
#include "../tm_utility.h"

#include "tm_test_suite.h"

#include <vector>
#include <array>

template< class T, class U >
bool equal( T a, U b ) {
	if( a.size() != b.size() ) {
		return false;
	}
	if( a.begin() == b.begin() ) {
		return true;
	}
	return compare( a.begin(), b.begin(), a.size() ) == 0;
}

int array_equal_test0()
{
	auto array0 = makeArrayView( {0, 1, 2, 3, 4} );
	auto array1 = makeArrayView( {0, 1, 2, 3, 4} );
	return equal( array0, array1 );
}
int array_equal_test1()
{
	const int array0data[] = {0, 1, 2, 3, 4};
	auto array0 = makeArrayView( array0data );
	auto array1 = makeArrayView( {0, 1, 2, 3, 4} );
	return equal( array0, array1 );
}
int array_equal_test2()
{
	std::vector< int > array0data = {0, 1, 2, 3, 4};
	auto array0 = makeArrayView( array0data );
	auto array1 = makeArrayView( {0, 1, 2, 3, 4} );
	return equal( array0, array1 );
}
int array_equal_test3()
{
	std::array< const int, 5 > array0data = {0, 1, 2, 3, 4};
	auto array0 = makeArrayView( array0data );
	auto array1 = makeArrayView( {0, 1, 2, 3, 4} );
	return equal( array0, array1 );
}
int array_equal_test4()
{
	const int array0data[] = {0, 1, 2, 3, 4};
	auto array0 = makeArrayView( array0data, 5 );
	auto array1 = makeArrayView( {0, 1, 2, 3, 4} );
	return equal( array0, array1 );
}
int array_equal_test5()
{
	const int array0data[] = {0, 1, 2, 3, 4};
	auto array0 = makeRangeView( array0data, 1, 4 );
	auto array1 = makeArrayView( {1, 2, 3} );
	return equal( array0, array1 );
}
int array_equal_test6()
{
	std::array< const int, 5 > array0data = {0, 1, 2, 3, 4};
	auto array0 = makeRangeView( array0data, 1, 4 );
	auto array1 = makeArrayView( {1, 2, 3} );
	return equal( array0, array1 );
}
int array_equal_test7()
{
	std::array< const int, 5 > array0data = {0, 1, 2, 3, 4};
	auto array0 = makeRangeView( array0data, 1, 10 );
	auto array1 = makeArrayView( {1, 2, 3, 4} );
	return equal( array0, array1 );
}
TM_TEST( array_equal_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( array_equal_test0 );
	TM_RUN_TEST( array_equal_test1 );
	TM_RUN_TEST( array_equal_test2 );
	TM_RUN_TEST( array_equal_test3 );
	TM_RUN_TEST( array_equal_test4 );
	TM_RUN_TEST( array_equal_test5 );
	TM_RUN_TEST( array_equal_test6 );
	TM_RUN_TEST( array_equal_test7 );
	TM_END_TESTING();
}

int array_assign_test0()
{
	int array0data[5] = {0, 1, 2, 3, 4};
	auto array0 = makeArrayView( array0data );
	int array1data[5];
	auto array1 = makeArrayView( array1data );
	array1.assign( array0 );
	return equal( array0, array1 );
}
TM_TEST( array_assign_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( array_assign_test0 );
	TM_END_TESTING();
}

int array_iteration_test0() {
	auto array = makeArrayView( {0, 1, 2, 3, 4} );
	for( auto& entry : array ) {
		printf( "%d", entry );
	}
	printf( "\n" );
	return true;
}
TM_TEST( array_iteration_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( array_iteration_test0 );
	TM_END_TESTING();
}

int uarray_assign_test0()
{
	int array0data[5] = {0, 1, 2, 3, 4};
	int array1data[5];
	auto array1 = makeUninitializedArrayView( array1data );
	array1.assign( array0data, array0data + 5 );
	return equal( makeArrayView( array0data ), array1 );
}
int uarray_assign_test1()
{
	int array0data[5] = {0, 1, 2, 3, 4};
	auto array0 = makeArrayView( array0data );
	int array1data[5];
	auto array1 = makeUninitializedArrayView( array1data );
	array1.assign( array0.begin(), array0.end() );
	return equal( array0, array1 );
}
int uarray_assign_test2()
{
	int arrayData[5];
	auto array = makeUninitializedArrayView( arrayData );
	array.assign( 5, 1 );
	auto expected = {1, 1, 1, 1, 1};
	return equal( array, expected );
}
int uarray_assign_test3()
{
	int arrayData[5];
	auto array = makeUninitializedArrayView( arrayData );
	auto expected = {1, 1, 1, 1, 1};
	array.assign( expected.begin(), 5 );
	return equal( array, expected );
}
TM_TEST( uarray_assign_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( uarray_assign_test0 );
	TM_RUN_TEST( uarray_assign_test1 );
	TM_RUN_TEST( uarray_assign_test2 );
	TM_RUN_TEST( uarray_assign_test3 );
	TM_END_TESTING();
}

int uarray_erase_test0()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( array.begin() + 1 );
	auto expected = {1, 3, 4, 5};
	return equal( array, expected );
}
int uarray_erase_test1()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( array.begin() );
	auto expected = {2, 3, 4, 5};
	return equal( array, expected );
}
int uarray_erase_test2()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( array.begin() + 1, array.begin() + 3 );
	auto expected = {1, 4, 5};
	return equal( array, expected );
}
int uarray_erase_test3()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( array.begin(), array.begin() + 3 );
	auto expected = {4, 5};
	return equal( array, expected );
}
int uarray_erase_test4()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( array.begin() + 3, array.begin() + 5 );
	auto expected = {1, 2, 3};
	return equal( array, expected );
}
int uarray_erase_test5()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( array.begin() + 1, array.begin() + 2 );
	auto expected = {1, 3, 4, 5};
	return equal( array, expected );
}
int uarray_erase_test6()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( array.begin(), array.end() );
	return array.empty();
}
int uarray_erase_test7()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( array.begin() );
	auto expected = {2, 3, 4, 5};
	return equal( array, expected );
}
int uarray_erase_test8()
{
	int arrayData[] = {1, 2, 3, 4, 5};
	auto array = makeInitializedArrayView( arrayData );
	array.erase( &array.back() );
	auto expected = {1, 2, 3, 4};
	return equal( array, expected );
}
int uarray_erase_test9()
{
	int arrayData[] = {2, 1, 2, 2, 2, 1, 1, 2};
	auto array = makeInitializedArrayView( arrayData );
	for( auto it = array.begin(); it != array.end(); ) {
		if( *it == 2 ) {
			it = array.erase( it );
			continue;
		}
		++it;
	}
	auto expected = {1, 1, 1};
	return equal( array, expected );
}
TM_TEST( uarray_erase_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( uarray_erase_test0 );
	TM_RUN_TEST( uarray_erase_test1 );
	TM_RUN_TEST( uarray_erase_test2 );
	TM_RUN_TEST( uarray_erase_test3 );
	TM_RUN_TEST( uarray_erase_test4 );
	TM_RUN_TEST( uarray_erase_test5 );
	TM_RUN_TEST( uarray_erase_test6 );
	TM_RUN_TEST( uarray_erase_test7 );
	TM_RUN_TEST( uarray_erase_test8 );
	TM_RUN_TEST( uarray_erase_test9 );
	TM_END_TESTING();
}

int uarray_insert_test0()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	auto expected = {1, 2, 3, 4};
	return equal( array, expected );
}
int uarray_insert_test1()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	array.insert( array.end(), 1, 5 );
	auto expected = {1, 2, 3, 4, 5};
	return equal( array, expected );
}
int uarray_insert_test2()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	array.insert( array.end(), 5, 5 );
	auto expected = {1, 2, 3, 4, 5, 5, 5, 5, 5};
	return equal( array, expected );
}
int uarray_insert_test3()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	array.insert( array.begin() + 1, 1, 5 );
	auto expected = {1, 5, 2, 3, 4};
	return equal( array, expected );
}
int uarray_insert_test4()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	array.insert( array.begin(), 1, 5 );
	auto expected = {5, 1, 2, 3, 4};
	return equal( array, expected );
}
int uarray_insert_test5()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	auto insertion = {5, 6, 7};
	array.insert( array.begin(), insertion.begin(), insertion.end() );
	auto expected = {5, 6, 7, 1, 2, 3, 4};
	return equal( array, expected );
}
int uarray_insert_test6()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	auto insertion = {5, 6, 7};
	array.insert( array.begin() + 2, insertion.begin(), insertion.end() );
	auto expected = {1, 2, 5, 6, 7, 3, 4};
	return equal( array, expected );
}
int uarray_insert_test7()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	auto insertion = {5, 6, 7};
	array.insert( array.end(), insertion.begin(), insertion.end() );
	auto expected = {1, 2, 3, 4, 5, 6, 7};
	return equal( array, expected );
}
int uarray_insert_test8()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	auto data = {1, 2, 3, 4};
	array.append( data.begin(), data.end() );
	auto insertion = {5, 6, 7};
	array.insert( array.end(), insertion.begin(), insertion.end() );
	auto expected = {1, 2, 3, 4, 5, 6, 7};
	return equal( array, expected );
}
TM_TEST( uarray_insert_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( uarray_insert_test0 );
	TM_RUN_TEST( uarray_insert_test1 );
	TM_RUN_TEST( uarray_insert_test2 );
	TM_RUN_TEST( uarray_insert_test3 );
	TM_RUN_TEST( uarray_insert_test4 );
	TM_RUN_TEST( uarray_insert_test5 );
	TM_RUN_TEST( uarray_insert_test6 );
	TM_RUN_TEST( uarray_insert_test7 );
	TM_RUN_TEST( uarray_insert_test8 );
	TM_END_TESTING();
}

int uarray_emplace_test0()
{
	int arrayData[10];
	auto array = makeUninitializedArrayView( arrayData );
	array.emplace_back() = 1;
	array.emplace_back() = 2;
	array.emplace_back() = 3;
	array.emplace_back() = 4;
	auto expected = {1, 2, 3, 4};
	return equal( array, expected );
}
TM_TEST( uarray_emplace_test )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( uarray_emplace_test0 );
	TM_END_TESTING();
}

int main( int argc, char const *argv[] )
{
	TM_UNREFERENCED_PARAM( argc );
	TM_UNREFERENCED_PARAM( argv );

	TM_BEGIN_RUN_TESTS();
	TM_RUN_TESTS( array_equal_tests );
	TM_RUN_TESTS( array_assign_tests );
	TM_RUN_TESTS( array_iteration_tests );
	TM_RUN_TESTS( uarray_assign_tests );
	TM_RUN_TESTS( uarray_erase_tests );
	TM_RUN_TESTS( uarray_emplace_test );
	TM_END_RUN_TESTS();

	return 0;
}