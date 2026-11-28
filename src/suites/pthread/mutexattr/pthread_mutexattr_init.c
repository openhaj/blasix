/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_init.c
 * @brief Tests for pthread_mutexattr_init() (<pthread.h>).
 * @Created: 2026/11/28 18:29:52 by Moutig
 * @Updated: 2026/11/28 18:31:23 by Moutig
 *
 * pthread_mutexattr_init(attr) initializes a mutex attributes object with
 * the default value of every attribute and returns 0; a mutex initialized
 * with it works. The defaults are in the units of the get functions. Not
 * tested: ENOMEM (cannot be caused), initializing an object twice
 * (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_init().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_init()")
{
	EXPECT_TYPE(&pthread_mutexattr_init, int (*)(pthread_mutexattr_t *));
}

/* 0, and a mutex initialized with the object locks and unlocks. */
TEST_OPT(returns_zero_and_initializes_mutexes, .ref = "XSH pthread_mutexattr_init()")
{
	pthread_mutexattr_t	a;
	pthread_mutex_t		m;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	ASSERT_EQ(pthread_mutex_init(&m, &a), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pthread_mutex_destroy(&m), 0);
	EXPECT_EQ(pthread_mutexattr_destroy(&a), 0);
}
