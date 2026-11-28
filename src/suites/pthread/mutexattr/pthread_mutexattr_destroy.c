/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_destroy.c
 * @brief Tests for pthread_mutexattr_destroy() (<pthread.h>).
 * @Created: 2026/11/28 18:29:05 by Moutig
 * @Updated: 2026/11/28 18:30:45 by Moutig
 *
 * pthread_mutexattr_destroy(attr) destroys a mutex attributes object and
 * returns 0; it can be initialized again. Changing or destroying the
 * object does not affect the mutexes already initialized with it: an
 * ERRORCHECK mutex stays ERRORCHECK. Not tested: any other use of a
 * destroyed object (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_destroy().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_destroy()")
{
	EXPECT_TYPE(&pthread_mutexattr_destroy, int (*)(pthread_mutexattr_t *));
}

/* 0, and the object can be initialized again. */
TEST_OPT(returns_zero_and_can_be_reinitialized, .ref = "XSH pthread_mutexattr_destroy()")
{
	pthread_mutexattr_t	a;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_destroy(&a), 0);
	EXPECT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_destroy(&a), 0);
}

/* Changed then destroyed: an ERRORCHECK mutex made with it still says EDEADLK. */
TEST_OPT(mutexes_already_initialized_unaffected, .since = "POSIX.1-2008",
	.ref = "XSH pthread_mutexattr_destroy()")
{
	pthread_mutexattr_t	a;
	pthread_mutex_t		m;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	ASSERT_EQ(pthread_mutexattr_settype(&a, PTHREAD_MUTEX_ERRORCHECK), 0);
	ASSERT_EQ(pthread_mutex_init(&m, &a), 0);
	EXPECT_EQ(pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE), 0);
	EXPECT_EQ(pthread_mutexattr_destroy(&a), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), EDEADLK);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}
