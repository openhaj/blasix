/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_condattr_init.c
 * @brief Tests for pthread_condattr_init() (<pthread.h>).
 * @Created: 2026/11/28 19:00:22 by Moutig
 * @Updated: 2026/11/28 19:01:10 by Moutig
 *
 * pthread_condattr_init(attr) initializes a condition variable attributes
 * object with the default value of every attribute and returns 0; a
 * condition variable made with it works. The defaults are in the get
 * units. Not tested: ENOMEM (cannot be caused), initializing twice
 * (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_condattr_init().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_condattr_init()")
{
	EXPECT_TYPE(&pthread_condattr_init, int (*)(pthread_condattr_t *));
}

/* 0, and a condition variable made with it wakes a waiter. */
TEST_OPT(returns_zero_and_initializes_conds, .ref = "XSH pthread_condattr_init()")
{
	pthread_condattr_t	a;
	pt_cv				v;
	pthread_t			t;

	ASSERT_EQ(pt_cvInit(&v, 5 * LC_NS_PER_SEC), 0);
	(void)pthread_cond_destroy(&v.c);
	ASSERT_EQ(pthread_condattr_init(&a), 0);
	ASSERT_EQ(pthread_cond_init(&v.c, &a), 0);
	EXPECT_EQ(pthread_condattr_destroy(&a), 0);
	ASSERT_EQ(pthread_create(&t, NULL, pt_cvWaiter, &v), 0);
	EXPECT_TRUE(pt_cvBlocked(&v, 1));
	EXPECT_EQ(pthread_cond_signal(&v.c), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(pt_get(&v.woken), 1);
}
