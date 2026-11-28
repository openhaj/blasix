/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_cond_destroy.c
 * @brief Tests for pthread_cond_destroy() (<pthread.h>).
 * @Created: 2026/11/28 19:02:29 by Moutig
 * @Updated: 2026/11/28 19:03:51 by Moutig
 *
 * pthread_cond_destroy(cond) destroys a condition variable no thread is
 * blocked on (safe) and returns 0; it can then be initialized again. Also
 * after waiters have been woken and have returned, and for one made by
 * PTHREAD_COND_INITIALIZER. Not tested: destroying one with blocked
 * threads (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_cond_destroy().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_cond_destroy()")
{
	EXPECT_TYPE(&pthread_cond_destroy, int (*)(pthread_cond_t *));
}

/* Never waited on: 0, and it can be initialized again. */
TEST_OPT(unused_and_reinitialized, .ref = "XSH pthread_cond_destroy()")
{
	pthread_cond_t	c;

	ASSERT_EQ(pthread_cond_init(&c, NULL), 0);
	EXPECT_EQ(pthread_cond_destroy(&c), 0);
	EXPECT_EQ(pthread_cond_init(&c, NULL), 0);
	EXPECT_EQ(pthread_cond_signal(&c), 0);
	EXPECT_EQ(pthread_cond_destroy(&c), 0);
}

/* After its waiters were woken and returned: 0. */
TEST_OPT(after_the_waiters_returned, .ref = "XSH pthread_cond_destroy()")
{
	pt_cv		v;
	pthread_t	t[2];

	ASSERT_EQ(pt_cvInit(&v, 5 * LC_NS_PER_SEC), 0);
	ASSERT_EQ(pthread_create(&t[0], NULL, pt_cvWaiter, &v), 0);
	ASSERT_EQ(pthread_create(&t[1], NULL, pt_cvWaiter, &v), 0);
	EXPECT_TRUE(pt_cvBlocked(&v, 2));
	EXPECT_EQ(pthread_cond_broadcast(&v.c), 0);
	(void)pthread_join(t[0], NULL);
	(void)pthread_join(t[1], NULL);
	EXPECT_EQ(pthread_cond_destroy(&v.c), 0);
}

/* One of PTHREAD_COND_INITIALIZER: 0. */
TEST_OPT(static_cond, .ref = "XSH pthread_cond_destroy()")
{
	static pthread_cond_t	c = PTHREAD_COND_INITIALIZER;

	EXPECT_EQ(pthread_cond_destroy(&c), 0);
}
