/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getstack.c
 * @brief Tests for pthread_attr_getstack() (<pthread.h>).
 * @Created: 2026/11/28 18:01:19 by Moutig
 * @Updated: 2026/11/28 18:03:55 by Moutig
 *
 * pthread_attr_getstack(attr, stackaddr, stacksize) stores the stack
 * attributes, the base (lowest address) and size of the storage for the
 * stack of the threads created with attr, and returns 0. Before
 * pthread_attr_setstack() the result is unspecified (2024), so only the
 * values set are checked. Not tested: an object that is not initialized
 * (undefined; EINVAL may fail in 2001), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getstack().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_getstack()")
{
	EXPECT_TYPE(&pthread_attr_getstack, int (*)(const pthread_attr_t *, void **, size_t *));
}

/* It gives the base and size last set. */
TEST_OPT(gives_the_values_set, .ref = "XSH pthread_attr_getstack()")
{
	pthread_attr_t	a;
	size_t			size = pt_stackMin() + 64 * pt_pageSize();
	void			*mem = lc_sysMmap(size);
	void			*addr = NULL;
	size_t			got = 0;

	ASSERT_NOTNULL(mem);
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setstack(&a, mem, size), 0);
	EXPECT_EQ(pthread_attr_getstack(&a, &addr, &got), 0);
	EXPECT_PTREQ(addr, mem);
	EXPECT_UEQ(got, size);
	(void)pthread_attr_destroy(&a);
}
