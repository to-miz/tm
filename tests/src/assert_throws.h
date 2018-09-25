#pragma once

#ifndef _ASSERT_THROWS_H_INCLUDED_
#define _ASSERT_THROWS_H_INCLUDED_

void enable_asserts(bool enabled);
void assert_throws(bool cond, const char* file, int line);

#endif // _ASSERT_THROWS_H_INCLUDED_
