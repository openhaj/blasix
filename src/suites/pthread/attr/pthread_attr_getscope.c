/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getscope.c
 * @brief Tests for pthread_attr_getscope() (<pthread.h>).
 * @Created: 2026/11/28 18:07:45 by Moutig
 * @Updated: 2026/11/28 18:08:58 by Moutig
 *
 * pthread_attr_getscope(attr, contentionscope) stores the contentionscope
 * attribute, PTHREAD_SCOPE_SYSTEM or PTHREAD_SCOPE_PROCESS, and returns
 * 0: one of them for a new object, the value last set otherwise. Not
 * tested: an object that is not initialized (undefined), EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getscope().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_getscope()")
{
	EXPECT_TYPE(&pthread_attr_getscope, int (*)(const pthread_attr_t *, int *));
}

/* A new object: 0, and one of the two values. */
TEST_OPT(default_is_one_of_the_values, .ref = "XSH pthread_attr_getscope()")
{
	pthread_attr_t	a;
	int				v = -12345;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_getscope(&a, &v), 0);
	EXPECT_TRUE(v == PTHREAD_SCOPE_SYSTEM || v == PTHREAD_SCOPE_PROCESS);
	(void)pthread_attr_destroy(&a);
}

/* It gives the value last set, for each value the object accepts. */
TEST_OPT(gives_the_value_set, .ref = "XSH pthread_attr_getscope()")
{
	const int		scope[2] = {PTHREAD_SCOPE_SYSTEM, PTHREAD_SCOPE_PROCESS};
	pthread_attr_t	a;
	int				bad = 0;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	for (int i = 0; i < 2; i++)
	{
		int	v = -12345;

		if (pthread_attr_setscope(&a, scope[i]) != 0)
			continue ;
		bad += pthread_attr_getscope(&a, &v) != 0 || v != scope[i];
	}
	EXPECT_EQ(bad, 0);
	(void)pthread_attr_destroy(&a);
}
