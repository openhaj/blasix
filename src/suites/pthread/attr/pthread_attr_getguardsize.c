/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getguardsize.c
 * @brief Tests for pthread_attr_getguardsize() (<pthread.h>).
 * @Created: 2026/11/28 18:00:54 by Moutig
 * @Updated: 2026/11/28 18:03:46 by Moutig
 *
 * pthread_attr_getguardsize(attr, guardsize) stores the guardsize
 * attribute and returns 0: exactly the value last set, even when the
 * implementation rounds it up to {PAGESIZE}. The default is {PAGESIZE} in
 * 2001 and implementation-defined from 2008. Not tested: an invalid
 * guardsize (EINVAL: the text does not say which values are), an object
 * that is not initialized (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getguardsize().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_getguardsize()")
{
	EXPECT_TYPE(&pthread_attr_getguardsize, int (*)(const pthread_attr_t *, size_t *));
}

/* 2001: the default is {PAGESIZE}. */
TEST_OPT(default_is_pagesize, .until = "POSIX.1-2008", .ref = "XSH pthread_attr_getguardsize()")
{
	pthread_attr_t	a;
	size_t			v = 0;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_getguardsize(&a, &v), 0);
	EXPECT_UEQ(v, pt_pageSize());
	(void)pthread_attr_destroy(&a);
}

/* The value set, not a rounded one: 1 byte, a page and a bit, 0. */
TEST_OPT(gives_the_value_set_unrounded, .ref = "XSH pthread_attr_getguardsize()")
{
	pthread_attr_t	a;
	size_t			want[3] = {1, 3 * pt_pageSize() + 5, 0};
	int				bad = 0;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	for (int i = 0; i < 3; i++)
	{
		size_t	v = 12345;

		bad += pthread_attr_setguardsize(&a, want[i]) != 0
			|| pthread_attr_getguardsize(&a, &v) != 0 || v != want[i];
	}
	EXPECT_EQ(bad, 0);
	(void)pthread_attr_destroy(&a);
}
