#define TM_UTILITY_IMPLEMENTATION
#define TM_USE_OWN_BEGIN_END
#include "../tm_utility.h"
using namespace utility;

#include "tm_test_suite.h"

#include <cstdio>
#include <cstdint>

int median_test0() { return median( 0, 1, 2 ) == 1; }
int median_test1() { return median( 2, 1, 0 ) == 1; }
int median_test2() { return median( 1, 2, 0 ) == 1; }
int median_test3() { return median( 1, 0, 2 ) == 1; }
int median_test4() { return median( 2, 0, 1 ) == 1; }
int median_test5() { return median( 0, 2, 1 ) == 1; }
int median_test6() { return median( 1, 1, 1 ) == 1; }
int median_test7()
{
	int a = 0, b = 1, c = 2;
	return &median( a, b, c ) == &b;
}
int median_test8()
{
	int a = 2, b = 1, c = 0;
	return &median( a, b, c ) == &b;
}
int median_test9()
{
	int a = 1, b = 2, c = 0;
	return &median( a, b, c ) == &a;
}
int median_test10()
{
	int a = 1, b = 0, c = 2;
	return &median( a, b, c ) == &a;
}
int median_test11()
{
	int a = 2, b = 0, c = 1;
	return &median( a, b, c ) == &c;
}
int median_test12()
{
	int a = 0, b = 2, c = 1;
	return &median( a, b, c ) == &c;
}
int median_test13()
{
	int a = 1, b = 1, c = 1;
	return &median( a, b, c ) == &b;
}
TM_TEST( median_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( median_test0 );
	TM_RUN_TEST( median_test1 );
	TM_RUN_TEST( median_test2 );
	TM_RUN_TEST( median_test3 );
	TM_RUN_TEST( median_test4 );
	TM_RUN_TEST( median_test5 );
	TM_RUN_TEST( median_test6 );
	TM_RUN_TEST( median_test7 );
	TM_RUN_TEST( median_test8 );
	TM_RUN_TEST( median_test9 );
	TM_RUN_TEST( median_test10 );
	TM_RUN_TEST( median_test11 );
	TM_RUN_TEST( median_test12 );
	TM_RUN_TEST( median_test13 );
	TM_END_TESTING();
}

int max_test0() { return max( 0, 1 ) == 1; }
int max_test1() { return max( 1, 0 ) == 1; }
int max_test2() { return max( 1, 1 ) == 1; }
int max_test3() { return max( 0, 1, 2 ) == 2; }
int max_test4() { return max( 0, 2, 1 ) == 2; }
int max_test5() { return max( 2, 0, 1 ) == 2; }
int max_test6() { return max( 2, 1, 0 ) == 2; }
int max_test7() { return max( 1, 2, 0 ) == 2; }
int max_test8() { return max( 1, 0, 2 ) == 2; }
int max_test9() { return max( 1, 1, 1 ) == 1; }
int max_test10()
{
	int a = 0, b = 1;
	return &max( a, b ) == &b;
}
int max_test11()
{
	int a = 1, b = 0;
	return &max( a, b ) == &a;
}
int max_test12()
{
	int a = 1, b = 1;
	return &max( a, b ) == &a;
}
int max_test13()
{
	int a = 0, b = 1, c = 2;
	return &max( a, b, c ) == &c;
}
int max_test14()
{
	int a = 0, b = 2, c = 1;
	return &max( a, b, c ) == &b;
}
int max_test15()
{
	int a = 2, b = 0, c = 1;
	return &max( a, b, c ) == &a;
}
int max_test16()
{
	int a = 2, b = 1, c = 0;
	return &max( a, b, c ) == &a;
}
int max_test17()
{
	int a = 1, b = 2, c = 0;
	return &max( a, b, c ) == &b;
}
int max_test18()
{
	int a = 1, b = 0, c = 2;
	return &max( a, b, c ) == &c;
}
int max_test19()
{
	int a = 1, b = 1, c = 1;
	return &max( a, b, c ) == &a;
}
TM_TEST( max_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( max_test0 );
	TM_RUN_TEST( max_test1 );
	TM_RUN_TEST( max_test2 );
	TM_RUN_TEST( max_test3 );
	TM_RUN_TEST( max_test4 );
	TM_RUN_TEST( max_test5 );
	TM_RUN_TEST( max_test6 );
	TM_RUN_TEST( max_test7 );
	TM_RUN_TEST( max_test8 );
	TM_RUN_TEST( max_test9 );
	TM_RUN_TEST( max_test10 );
	TM_RUN_TEST( max_test11 );
	TM_RUN_TEST( max_test12 );
	TM_RUN_TEST( max_test13 );
	TM_RUN_TEST( max_test14 );
	TM_RUN_TEST( max_test15 );
	TM_RUN_TEST( max_test16 );
	TM_RUN_TEST( max_test17 );
	TM_RUN_TEST( max_test18 );
	TM_RUN_TEST( max_test19 );
	TM_END_TESTING();
}

int min_test0() { return min( 0, 1 ) == 0; }
int min_test1() { return min( 1, 0 ) == 0; }
int min_test2() { return min( 1, 1 ) == 1; }
int min_test3() { return min( 0, 1, 2 ) == 0; }
int min_test4() { return min( 0, 2, 1 ) == 0; }
int min_test5() { return min( 2, 0, 1 ) == 0; }
int min_test6() { return min( 2, 1, 0 ) == 0; }
int min_test7() { return min( 1, 2, 0 ) == 0; }
int min_test8() { return min( 1, 0, 2 ) == 0; }
int min_test9() { return min( 1, 1, 1 ) == 1; }
int min_test10()
{
	int a = 0, b = 1;
	return &min( a, b ) == &a;
}
int min_test11()
{
	int a = 1, b = 0;
	return &min( a, b ) == &b;
}
int min_test12()
{
	int a = 1, b = 1;
	return &min( a, b ) == &b;
}
int min_test13()
{
	int a = 0, b = 1, c = 2;
	return &min( a, b, c ) == &a;
}
int min_test14()
{
	int a = 0, b = 2, c = 1;
	return &min( a, b, c ) == &a;
}
int min_test15()
{
	int a = 2, b = 0, c = 1;
	return &min( a, b, c ) == &b;
}
int min_test16()
{
	int a = 2, b = 1, c = 0;
	return &min( a, b, c ) == &c;
}
int min_test17()
{
	int a = 1, b = 2, c = 0;
	return &min( a, b, c ) == &c;
}
int min_test18()
{
	int a = 1, b = 0, c = 2;
	return &min( a, b, c ) == &b;
}
int min_test19()
{
	int a = 1, b = 1, c = 1;
	return &min( a, b, c ) == &c;
}
TM_TEST( min_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( min_test0 );
	TM_RUN_TEST( min_test1 );
	TM_RUN_TEST( min_test2 );
	TM_RUN_TEST( min_test3 );
	TM_RUN_TEST( min_test4 );
	TM_RUN_TEST( min_test5 );
	TM_RUN_TEST( min_test6 );
	TM_RUN_TEST( min_test7 );
	TM_RUN_TEST( min_test8 );
	TM_RUN_TEST( min_test9 );
	TM_RUN_TEST( min_test10 );
	TM_RUN_TEST( min_test11 );
	TM_RUN_TEST( min_test12 );
	TM_RUN_TEST( min_test13 );
	TM_RUN_TEST( min_test14 );
	TM_RUN_TEST( min_test15 );
	TM_RUN_TEST( min_test16 );
	TM_RUN_TEST( min_test17 );
	TM_RUN_TEST( min_test18 );
	TM_RUN_TEST( min_test19 );
	TM_END_TESTING();
}

bool equals( const MinMaxPair< const int& >& a, const MinMaxPair< const int& >& b )
{
	return a.min == b.min && a.max == b.max;
}
bool equalsRef( const MinMaxPair< const int& >& a, const MinMaxPair< const int& >& b )
{
	return &a.min == &b.min && &a.max == &b.max;
}

bool minmax_test0() { return equals( minmax( 0, 1 ), {0, 1} ); }
bool minmax_test1() { return equals( minmax( 1, 0 ), {0, 1} ); }
bool minmax_test2() { return equals( minmax( 1, 1 ), {1, 1} ); }
bool minmax_test3() { return equals( minmax( 0, 1, 2 ), {0, 2} ); }
bool minmax_test4() { return equals( minmax( 2, 1, 0 ), {0, 2} ); }
bool minmax_test5() { return equals( minmax( 1, 2, 0 ), {0, 2} ); }
bool minmax_test6() { return equals( minmax( 1, 0, 2 ), {0, 2} ); }
bool minmax_test7() { return equals( minmax( 2, 0, 1 ), {0, 2} ); }
bool minmax_test8() { return equals( minmax( 0, 2, 1 ), {0, 2} ); }
bool minmax_test9() { return equals( minmax( 1, 1, 1 ), {1, 1} ); }

bool minmax_test10()
{
	int a = 0, b = 1;
	return equalsRef( minmax( a, b ), {a, b} );
}
bool minmax_test11()
{
	int a = 1, b = 0;
	return equalsRef( minmax( a, b ), {b, a} );
}
bool minmax_test12()
{
	int a = 1, b = 1;
	return equalsRef( minmax( a, b ), {b, a} );
}
bool minmax_test13()
{
	int a = 0, b = 1, c = 2;
	return equalsRef( minmax( a, b, c ), {a, c} );
}
bool minmax_test14()
{
	int a = 2, b = 1, c = 0;
	return equalsRef( minmax( a, b, c ), {c, a} );
}
bool minmax_test15()
{
	int a = 1, b = 2, c = 0;
	return equalsRef( minmax( a, b, c ), {c, b} );
}
bool minmax_test16()
{
	int a = 1, b = 0, c = 2;
	return equalsRef( minmax( a, b, c ), {b, c} );
}
bool minmax_test17()
{
	int a = 2, b = 0, c = 1;
	return equalsRef( minmax( a, b, c ), {b, a} );
}
bool minmax_test18()
{
	int a = 0, b = 2, c = 1;
	return equalsRef( minmax( a, b, c ), {a, b} );
}
bool minmax_test19()
{
	int a = 1, b = 1, c = 1;
	return equalsRef( minmax( a, b, c ), {c, a} );
}
TM_TEST( minmax_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( minmax_test0 );
	TM_RUN_TEST( minmax_test1 );
	TM_RUN_TEST( minmax_test2 );
	TM_RUN_TEST( minmax_test3 );
	TM_RUN_TEST( minmax_test4 );
	TM_RUN_TEST( minmax_test5 );
	TM_RUN_TEST( minmax_test6 );
	TM_RUN_TEST( minmax_test7 );
	TM_RUN_TEST( minmax_test8 );
	TM_RUN_TEST( minmax_test9 );
	TM_RUN_TEST( minmax_test10 );
	TM_RUN_TEST( minmax_test11 );
	TM_RUN_TEST( minmax_test12 );
	TM_RUN_TEST( minmax_test13 );
	TM_RUN_TEST( minmax_test14 );
	TM_RUN_TEST( minmax_test15 );
	TM_RUN_TEST( minmax_test16 );
	TM_RUN_TEST( minmax_test17 );
	TM_RUN_TEST( minmax_test18 );
	TM_RUN_TEST( minmax_test19 );
	TM_END_TESTING();
}

int unsignedof_test0() { return unsignedof( -1 ) == 0xFFFFFFFF; }
int unsignedof_test1() { return unsignedof( (int64_t)-1 ) == 0xFFFFFFFFFFFFFFFFull; }
int unsignedof_test2() { return unsignedof( 12ull ) == 12; }
int unsignedof_test3() { return unsignedof( 0x123456789ABCDEF0ull ) == 0x123456789ABCDEF0ull; }
int unsignedof_test4() { return unsignedof( 0x12345678u ) == 0x12345678; }
int unsignedof_test5() { return unsignedof( 12u ) == 12; }
TM_TEST( unsignedof_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( unsignedof_test0 );
	TM_RUN_TEST( unsignedof_test1 );
	TM_RUN_TEST( unsignedof_test2 );
	TM_RUN_TEST( unsignedof_test3 );
	TM_RUN_TEST( unsignedof_test4 );
	TM_RUN_TEST( unsignedof_test5 );
	TM_END_TESTING();
}

int promote_as_is_to_test0()
{
	return promote_as_is_to< uint64_t >( (int8_t)-1 ) == 0x00000000000000FFull;
}
int promote_as_is_to_test1()
{
	return promote_as_is_to< uint64_t >( (int16_t)-1 ) == 0x000000000000FFFFull;
}
int promote_as_is_to_test2()
{
	return promote_as_is_to< uint64_t >( (int32_t)-1 ) == 0x00000000FFFFFFFFull;
}
int promote_as_is_to_test3()
{
	return promote_as_is_to< uint64_t >( (int64_t)-1 ) == 0xFFFFFFFFFFFFFFFFull;
}
int promote_as_is_to_test4()
{
	return promote_as_is_to< uint64_t >( (uint8_t)0xFF ) == 0x00000000000000FFull;
}
int promote_as_is_to_test5()
{
	return promote_as_is_to< uint64_t >( (uint16_t)0xFFFF ) == 0x000000000000FFFFull;
}
int promote_as_is_to_test6()
{
	return promote_as_is_to< uint64_t >( 0xFFFFFFFFu ) == 0x00000000FFFFFFFFull;
}
int promote_as_is_to_test7()
{
	return promote_as_is_to< uint64_t >( 0xFFFFFFFFFFFFFFFFull ) == 0xFFFFFFFFFFFFFFFFull;
}
TM_TEST( promote_as_is_to_tests )
{
	TM_BEGIN_TESTING();
	TM_RUN_TEST( promote_as_is_to_test0 );
	TM_RUN_TEST( promote_as_is_to_test1 );
	TM_RUN_TEST( promote_as_is_to_test2 );
	TM_RUN_TEST( promote_as_is_to_test3 );
	TM_RUN_TEST( promote_as_is_to_test4 );
	TM_RUN_TEST( promote_as_is_to_test5 );
	TM_RUN_TEST( promote_as_is_to_test6 );
	TM_RUN_TEST( promote_as_is_to_test7 );
	TM_END_TESTING();
}

int main( int argc, char const *argv[] )
{
	TM_UNREFERENCED_PARAM( argc );
	TM_UNREFERENCED_PARAM( argv );

	TM_BEGIN_RUN_TESTS();
	TM_RUN_TESTS( median_tests );
	TM_RUN_TESTS( max_tests );
	TM_RUN_TESTS( min_tests );
	TM_RUN_TESTS( minmax_tests );
	TM_RUN_TESTS( unsignedof_tests );
	TM_RUN_TESTS( promote_as_is_to_tests );
	TM_END_RUN_TESTS();
	return 0;
}