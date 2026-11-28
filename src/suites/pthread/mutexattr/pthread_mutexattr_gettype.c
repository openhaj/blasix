/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_gettype.c
 * @brief Tests for pthread_mutexattr_gettype() (<pthread.h>).
 * @Created: 2026/11/28 18:29:44 by Moutig
 * @Updated: 2026/11/28 18:31:18 by Moutig
 *
 * pthread_mutexattr_gettype(attr, type) stores the type attribute and
 * returns 0. The default is PTHREAD_MUTEX_DEFAULT; otherwise the value
 * last set, for each of the four types. Not tested: an object that is not
 * initialized (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_gettype().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_gettype()")
{
	EXPECT_TYPE(&pthread_mutexattr_gettype, int (*)(const pthread_mutexattr_t *, int *));
}

/* The default is PTHREAD_MUTEX_DEFAULT. */
TEST_OPT(default_is_default, .ref = "XSH pthread_mutexattr_gettype()")
{
	pthread_mutexattr_t	a;
	int					v = -12345;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_gettype(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_MUTEX_DEFAULT);
	(void)pthread_mutexattr_destroy(&a);
}

/* It gives the type last set, for the four types. */
TEST_OPT(gives_the_type_set, .ref = "XSH pthread_mutexattr_gettype()")
{
	const int			type[4] = {PTHREAD_MUTEX_NORMAL, PTHREAD_MUTEX_ERRORCHECK,
		PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_DEFAULT};
	pthread_mutexattr_t	a;
	int					bad = 0;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	for (int i = 0; i < 4; i++)
	{
		int	v = -12345;

		bad += pthread_mutexattr_settype(&a, type[i]) != 0
			|| pthread_mutexattr_gettype(&a, &v) != 0 || v != type[i];
	}
	EXPECT_EQ(bad, 0);
	(void)pthread_mutexattr_destroy(&a);
}
