#pragma once

#ifndef _ASSERT_THROWS_H_INCLUDED_
#define _ASSERT_THROWS_H_INCLUDED_

void enable_asserts(bool enabled);
void assert_throws(bool cond, const char* file, int line);

#define TM_ASSERT(x) assert_throws(x, #x, __FILE__, __LINE__)
#define CHECK_ASSERTION_FAILURE CHECK_THROWS
#define CHECK_NOASSERT CHECK_NOTHROW

#endif  // _ASSERT_THROWS_H_INCLUDED_
