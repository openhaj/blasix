/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getstackaddr.c
 * @brief Tests for pthread_attr_getstackaddr() (<pthread.h>).
 * @Created: 2026/11/28 18:01:28 by Moutig
 * @Updated: 2026/11/28 18:04:03 by Moutig
 *
 * pthread_attr_getstackaddr(attr, stackaddr) stores the stackaddr
 * attribute and returns 0 (TSA, obsolescent in 2001, removed in 2008).
 * What the address designates (base or top of the stack) is not said, so
 * only the value set is checked. Not tested: EINVAL (may fail), EINTR
 * (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getstackaddr() (POSIX.1-2001).
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_getstackaddr()")
{
	EXPECT_TYPE(&pthread_attr_getstackaddr, int (*)(const pthread_attr_t *, void **));
}

/* It gives the address last set. */
TEST_OPT(gives_the_value_set, .ref = "XSH pthread_attr_getstackaddr()")
{
	static char		storage[64];
	pthread_attr_t	a;
	void			*v = NULL;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setstackaddr(&a, storage), 0);
	EXPECT_EQ(pthread_attr_getstackaddr(&a, &v), 0);
	EXPECT_PTREQ(v, storage);
	(void)pthread_attr_destroy(&a);
}
