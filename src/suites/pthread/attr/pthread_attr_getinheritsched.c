/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getinheritsched.c
 * @brief Tests for pthread_attr_getinheritsched() (<pthread.h>).
 * @Created: 2026/11/28 18:07:24 by Moutig
 * @Updated: 2026/11/28 18:08:30 by Moutig
 *
 * pthread_attr_getinheritsched(attr, inheritsched) stores the
 * inheritsched attribute and returns 0. The supported values are
 * PTHREAD_INHERIT_SCHED and PTHREAD_EXPLICIT_SCHED; the default is not
 * specified, so it is only checked to be one of them. Not tested: an
 * object that is not initialized (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getinheritsched().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_getinheritsched()")
{
	EXPECT_TYPE(&pthread_attr_getinheritsched, int (*)(const pthread_attr_t *, int *));
}

/* A new object: 0, and one of the two values. */
TEST_OPT(default_is_a_supported_value, .ref = "XSH pthread_attr_getinheritsched()")
{
	pthread_attr_t	a;
	int				v = -12345;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_getinheritsched(&a, &v), 0);
	EXPECT_TRUE(v == PTHREAD_INHERIT_SCHED || v == PTHREAD_EXPLICIT_SCHED);
	(void)pthread_attr_destroy(&a);
}

/* It gives the value last set, for both values. */
TEST_OPT(gives_the_value_set, .ref = "XSH pthread_attr_getinheritsched()")
{
	pthread_attr_t	a;
	int				v = -12345;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setinheritsched(&a, PTHREAD_EXPLICIT_SCHED), 0);
	EXPECT_EQ(pthread_attr_getinheritsched(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_EXPLICIT_SCHED);
	ASSERT_EQ(pthread_attr_setinheritsched(&a, PTHREAD_INHERIT_SCHED), 0);
	EXPECT_EQ(pthread_attr_getinheritsched(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_INHERIT_SCHED);
	(void)pthread_attr_destroy(&a);
}
