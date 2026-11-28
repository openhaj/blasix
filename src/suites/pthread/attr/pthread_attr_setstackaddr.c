/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setstackaddr.c
 * @brief Tests for pthread_attr_setstackaddr() (<pthread.h>).
 * @Created: 2026/11/28 18:02:43 by Moutig
 * @Updated: 2026/11/28 18:05:20 by Moutig
 *
 * pthread_attr_setstackaddr(attr, stackaddr) sets the stackaddr attribute
 * and returns 0 (TSA, obsolescent in 2001, removed in 2008). No thread is
 * created with it: whether stackaddr is the base or the top of the
 * storage is not said. Not tested: EINVAL (may fail), EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_attr_setstackaddr() (POSIX.1-2001).
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_setstackaddr()")
{
	EXPECT_TYPE(&pthread_attr_setstackaddr, int (*)(pthread_attr_t *, void *));
}

/* 0, and the address is kept. */
TEST_OPT(sets_the_address, .ref = "XSH pthread_attr_setstackaddr()")
{
	static char		storage[64];
	pthread_attr_t	a;
	void			*v = NULL;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setstackaddr(&a, storage + 32), 0);
	ASSERT_EQ(pthread_attr_getstackaddr(&a, &v), 0);
	EXPECT_PTREQ(v, storage + 32);
	(void)pthread_attr_destroy(&a);
}
