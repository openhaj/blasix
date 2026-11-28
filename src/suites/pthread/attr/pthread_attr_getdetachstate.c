/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getdetachstate.c
 * @brief Tests for pthread_attr_getdetachstate() (<pthread.h>).
 * @Created: 2026/11/28 18:00:44 by Moutig
 * @Updated: 2026/11/28 18:03:40 by Moutig
 *
 * pthread_attr_getdetachstate(attr, detachstate) stores the detachstate
 * attribute, PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE, and
 * returns 0. The default is PTHREAD_CREATE_JOINABLE. Not tested: an
 * object that is not initialized (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getdetachstate().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_getdetachstate()")
{
	EXPECT_TYPE(&pthread_attr_getdetachstate, int (*)(const pthread_attr_t *, int *));
}

/* The default is PTHREAD_CREATE_JOINABLE. */
TEST_OPT(default_is_joinable, .ref = "XSH pthread_attr_getdetachstate()")
{
	pthread_attr_t	a;
	int				v = -12345;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_getdetachstate(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_CREATE_JOINABLE);
	(void)pthread_attr_destroy(&a);
}

/* It gives the value last set, for both values. */
TEST_OPT(gives_the_value_set, .ref = "XSH pthread_attr_getdetachstate()")
{
	pthread_attr_t	a;
	int				v = -12345;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED), 0);
	EXPECT_EQ(pthread_attr_getdetachstate(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_CREATE_DETACHED);
	ASSERT_EQ(pthread_attr_setdetachstate(&a, PTHREAD_CREATE_JOINABLE), 0);
	EXPECT_EQ(pthread_attr_getdetachstate(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_CREATE_JOINABLE);
	(void)pthread_attr_destroy(&a);
}
