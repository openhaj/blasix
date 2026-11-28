/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getschedpolicy.c
 * @brief Tests for pthread_attr_getschedpolicy() (<pthread.h>).
 * @Created: 2026/11/28 18:07:38 by Moutig
 * @Updated: 2026/11/28 18:08:52 by Moutig
 *
 * pthread_attr_getschedpolicy(attr, policy) stores the schedpolicy
 * attribute and returns 0: the value last set, for SCHED_FIFO, SCHED_RR
 * and SCHED_OTHER. The default is not specified. Not tested: an object
 * that is not initialized (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getschedpolicy().
 */
#include "pthread/pthread_cases.h"

#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_getschedpolicy()")
{
	EXPECT_TYPE(&pthread_attr_getschedpolicy, int (*)(const pthread_attr_t *, int *));
}

/* 0 for a new object. */
TEST_OPT(returns_zero, .ref = "XSH pthread_attr_getschedpolicy()")
{
	pthread_attr_t	a;
	int				v = -12345;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_getschedpolicy(&a, &v), 0);
	(void)pthread_attr_destroy(&a);
}

/* It gives the value last set, for the three required policies. */
TEST_OPT(gives_the_value_set, .ref = "XSH pthread_attr_getschedpolicy()")
{
	const int		pol[3] = {SCHED_FIFO, SCHED_RR, SCHED_OTHER};
	pthread_attr_t	a;
	int				bad = 0;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	for (int i = 0; i < 3; i++)
	{
		int	v = -12345;

		bad += pthread_attr_setschedpolicy(&a, pol[i]) != 0
			|| pthread_attr_getschedpolicy(&a, &v) != 0 || v != pol[i];
	}
	EXPECT_EQ(bad, 0);
	(void)pthread_attr_destroy(&a);
}
