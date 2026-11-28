/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getstacksize.c
 * @brief Tests for pthread_attr_getstacksize() (<pthread.h>).
 * @Created: 2026/11/28 18:01:49 by Moutig
 * @Updated: 2026/11/28 18:04:20 by Moutig
 *
 * pthread_attr_getstacksize(attr, stacksize) stores the stacksize
 * attribute, the minimum stack size of the threads created with attr, and
 * returns 0. Its default is not specified. Not tested: an object that is
 * not initialized (undefined; EINVAL may fail in 2001), EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getstacksize().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_getstacksize()")
{
	EXPECT_TYPE(&pthread_attr_getstacksize, int (*)(const pthread_attr_t *, size_t *));
}

/* 0 for a new object. */
TEST_OPT(returns_zero, .ref = "XSH pthread_attr_getstacksize()")
{
	pthread_attr_t	a;
	size_t			v = 0;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_getstacksize(&a, &v), 0);
	(void)pthread_attr_destroy(&a);
}

/* It gives the value last set. */
TEST_OPT(gives_the_value_set, .ref = "XSH pthread_attr_getstacksize()")
{
	pthread_attr_t	a;
	size_t			want = pt_stackMin() + 16 * pt_pageSize();
	size_t			v = 0;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setstacksize(&a, want), 0);
	EXPECT_EQ(pthread_attr_getstacksize(&a, &v), 0);
	EXPECT_UEQ(v, want);
	(void)pthread_attr_destroy(&a);
}
