/*
tm_test_suite.h - public domain
written by Tolga Mizrak 2016

NOTES
	Testing facility that I use in private projects.
*/

#pragma once

#ifndef _TM_TEST_SUITE_H_INCLUDED_
#define _TM_TEST_SUITE_H_INCLUDED_

#include <stdio.h>

#if defined(  __GNUC__ ) || defined( __clang__ )
	#define TM_UNREFERENCED_PARAM( x ) ( (void)( x ) )
#else
	#define TM_UNREFERENCED_PARAM( x ) ( x )
#endif

typedef struct {
	int testsRun;
	int testsSucceeded;
} TestResult;

#define TM_TEST( name ) TestResult name()
#define TM_BEGIN_TESTING() TestResult result = {0, 0};
#define TM_END_TESTING() return result;
#define TM_RUN_TEST( name )                             \
	{                                                   \
		++result.testsRun;                              \
		const char* resultString = "OK";                \
		if( !name() ) {                                 \
			resultString = "FAILED";                    \
		} else {                                        \
			++result.testsSucceeded;                    \
		}                                               \
		printf( "  %-25s\t%s\n", #name, resultString ); \
	}

#define TM_BEGIN_RUN_TESTS() TestResult overall = {0, 0};
#define TM_RUN_TESTS( name )                                                           \
	{                                                                                  \
		printf( "Running tests %s:\n", #name );                                        \
		TestResult result = name();                                                    \
		printf( "%d/%d Tests succeeded\n\n", result.testsSucceeded, result.testsRun ); \
		++overall.testsRun;                                                            \
		if( result.testsSucceeded == result.testsRun ) {                               \
			++overall.testsSucceeded;                                                  \
		}                                                                              \
	}
#define TM_END_RUN_TESTS() \
	printf( "Overall: %d/%d Tests succeeded\n", overall.testsSucceeded, overall.testsRun );

#endif  // _TM_TEST_SUITE_H_INCLUDED_
