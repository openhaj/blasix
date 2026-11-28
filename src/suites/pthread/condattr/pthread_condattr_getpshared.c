/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_condattr_getpshared.c
 * @brief Tests for pthread_condattr_getpshared() (<pthread.h>).
 * @Created: 2026/11/28 19:00:15 by Moutig
 * @Updated: 2026/11/28 19:01:14 by Moutig
 *
 * pthread_condattr_getpshared(attr, pshared) stores the process-shared
 * attribute and returns 0 (TSH): PTHREAD_PROCESS_PRIVATE by default, the
 * value last set otherwise. Not tested: an object that is not
 * initialized (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_condattr_getpshared().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_condattr_getpshared()")
{
	EXPECT_TYPE(&pthread_condattr_getpshared, int (*)(const pthread_condattr_t *, int *));
}

/* The default is PTHREAD_PROCESS_PRIVATE. */
TEST_OPT(default_is_private, .ref = "XSH pthread_condattr_getpshared()")
{
	pthread_condattr_t	a;
	int					v = -12345;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	EXPECT_EQ(pthread_condattr_getpshared(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_PROCESS_PRIVATE);
	(void)pthread_condattr_destroy(&a);
}

/* It gives the value last set, for both values. */
TEST_OPT(gives_the_value_set, .ref = "XSH pthread_condattr_getpshared()")
{
	pthread_condattr_t	a;
	int					v = -12345;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	ASSERT_EQ(pthread_condattr_setpshared(&a, PTHREAD_PROCESS_SHARED), 0);
	EXPECT_EQ(pthread_condattr_getpshared(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_PROCESS_SHARED);
	ASSERT_EQ(pthread_condattr_setpshared(&a, PTHREAD_PROCESS_PRIVATE), 0);
	EXPECT_EQ(pthread_condattr_getpshared(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_PROCESS_PRIVATE);
	(void)pthread_condattr_destroy(&a);
}
