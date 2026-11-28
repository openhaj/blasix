/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_destroy.c
 * @brief Tests for pthread_mutex_destroy() (<pthread.h>).
 * @Created: 2026/11/28 18:35:24 by Moutig
 * @Updated: 2026/11/28 18:36:21 by Moutig
 *
 * pthread_mutex_destroy(mutex) destroys an unlocked mutex (safe) and
 * returns 0; it can then be initialized again. A mutex made by
 * PTHREAD_MUTEX_INITIALIZER can be destroyed too. Not tested: destroying
 * a locked mutex or one in use (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_destroy().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_mutex_destroy()")
{
	EXPECT_TYPE(&pthread_mutex_destroy, int (*)(pthread_mutex_t *));
}

/* An unlocked mutex, also after use: 0. */
TEST_OPT(unlocked_mutex, .ref = "XSH pthread_mutex_destroy()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pthread_mutex_destroy(&m), 0);
	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	ASSERT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pthread_mutex_destroy(&m), 0);
}

/* A destroyed mutex can be initialized again and used. */
TEST_OPT(can_be_reinitialized, .ref = "XSH pthread_mutex_destroy()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	ASSERT_EQ(pthread_mutex_destroy(&m), 0);
	EXPECT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pthread_mutex_destroy(&m), 0);
}

/* A mutex of PTHREAD_MUTEX_INITIALIZER: 0. */
TEST_OPT(static_mutex, .ref = "XSH pthread_mutex_destroy()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;

	EXPECT_EQ(pthread_mutex_destroy(&m), 0);
}
