/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setscope.c
 * @brief Tests for pthread_attr_setscope() (<pthread.h>).
 * @Created: 2026/11/28 18:08:16 by Moutig
 * @Updated: 2026/11/28 18:09:26 by Moutig
 *
 * pthread_attr_setscope(attr, contentionscope) sets the contentionscope
 * attribute and returns 0, or ENOTSUP for a value the implementation does
 * not support (Linux supports only PTHREAD_SCOPE_SYSTEM). The value a new
 * object holds is supported: it can be set again. Not tested: an invalid
 * value (EINVAL may fail), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_setscope().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_setscope()")
{
	EXPECT_TYPE(&pthread_attr_setscope, int (*)(pthread_attr_t *, int));
}

/* Each value: 0, or ENOTSUP when not supported. */
TEST_OPT(zero_or_enotsup, .ref = "XSH pthread_attr_setscope()")
{
	pthread_attr_t	a;
	int				e;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	e = pthread_attr_setscope(&a, PTHREAD_SCOPE_SYSTEM);
	EXPECT_TRUE(e == 0 || e == ENOTSUP);
	e = pthread_attr_setscope(&a, PTHREAD_SCOPE_PROCESS);
	EXPECT_TRUE(e == 0 || e == ENOTSUP);
	(void)pthread_attr_destroy(&a);
}

/* The default value is supported: 0, and threads are created with it. */
TEST_OPT(default_value_is_supported, .ref = "XSH pthread_attr_setscope()")
{
	pthread_attr_t	a;
	int				v = -12345;
	pt_run			r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_getscope(&a, &v), 0);
	EXPECT_EQ(pthread_attr_setscope(&a, v), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	(void)pthread_attr_destroy(&a);
}
