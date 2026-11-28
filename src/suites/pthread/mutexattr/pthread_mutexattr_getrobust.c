/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_getrobust.c
 * @brief Tests for pthread_mutexattr_getrobust() (<pthread.h>).
 * @Created: 2026/11/28 18:29:36 by Moutig
 * @Updated: 2026/11/28 18:31:13 by Moutig
 *
 * pthread_mutexattr_getrobust(attr, robust) stores the robust attribute
 * and returns 0 (2008): PTHREAD_MUTEX_STALLED by default, the value last
 * set otherwise. Not tested: an object that is not initialized
 * (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_getrobust().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_getrobust()")
{
	EXPECT_TYPE(&pthread_mutexattr_getrobust, int (*)(const pthread_mutexattr_t *, int *));
}

/* The default is PTHREAD_MUTEX_STALLED. */
TEST_OPT(default_is_stalled, .ref = "XSH pthread_mutexattr_getrobust()")
{
	pthread_mutexattr_t	a;
	int					v = -12345;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_getrobust(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_MUTEX_STALLED);
	(void)pthread_mutexattr_destroy(&a);
}

/* It gives the value last set, for both values. */
TEST_OPT(gives_the_value_set, .ref = "XSH pthread_mutexattr_getrobust()")
{
	pthread_mutexattr_t	a;
	int					v = -12345;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	ASSERT_EQ(pthread_mutexattr_setrobust(&a, PTHREAD_MUTEX_ROBUST), 0);
	EXPECT_EQ(pthread_mutexattr_getrobust(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_MUTEX_ROBUST);
	ASSERT_EQ(pthread_mutexattr_setrobust(&a, PTHREAD_MUTEX_STALLED), 0);
	EXPECT_EQ(pthread_mutexattr_getrobust(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_MUTEX_STALLED);
	(void)pthread_mutexattr_destroy(&a);
}
