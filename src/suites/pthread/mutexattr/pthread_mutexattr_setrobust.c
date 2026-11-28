/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_setrobust.c
 * @brief Tests for pthread_mutexattr_setrobust() (<pthread.h>).
 * @Created: 2026/11/28 18:30:26 by Moutig
 * @Updated: 2026/11/28 18:31:44 by Moutig
 *
 * pthread_mutexattr_setrobust(attr, robust) sets the robust attribute and
 * returns 0 for PTHREAD_MUTEX_STALLED and PTHREAD_MUTEX_ROBUST (2008);
 * a value that is neither is EINVAL. A robust mutex made with it locks and
 * unlocks; what happens when its owner dies is in the robust mutex unit.
 * Not tested: EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_setrobust().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_setrobust()")
{
	EXPECT_TYPE(&pthread_mutexattr_setrobust, int (*)(pthread_mutexattr_t *, int));
}

/* Both values: 0, and a robust mutex locks and unlocks. */
TEST_OPT(both_values_accepted, .ref = "XSH pthread_mutexattr_setrobust()")
{
	pthread_mutexattr_t	a;
	pthread_mutex_t		m;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_setrobust(&a, PTHREAD_MUTEX_STALLED), 0);
	EXPECT_EQ(pthread_mutexattr_setrobust(&a, PTHREAD_MUTEX_ROBUST), 0);
	ASSERT_EQ(pthread_mutex_init(&m, &a), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
	(void)pthread_mutexattr_destroy(&a);
}

/* A value that is neither (the valid ones "include" both; -1 and 1000): EINVAL. */
TEST_OPT(invalid_value_einval, .ref = "XSH pthread_mutexattr_setrobust()")
{
	pthread_mutexattr_t	a;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_setrobust(&a, -1), EINVAL);
	EXPECT_EQ(pthread_mutexattr_setrobust(&a, 1000), EINVAL);
	(void)pthread_mutexattr_destroy(&a);
}
