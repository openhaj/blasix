/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setguardsize.c
 * @brief Tests for pthread_attr_setguardsize() (<pthread.h>).
 * @Created: 2026/11/28 18:02:26 by Moutig
 * @Updated: 2026/11/28 18:04:53 by Moutig
 *
 * pthread_attr_setguardsize(attr, guardsize) sets the size of the guard
 * area at the overflow end of the stack of the threads created with attr
 * and returns 0; 0 means no guard area. With a guard area, overflowing
 * the stack into it is an error (possibly SIGSEGV): a thread recursing
 * without end on a small stack, in a child of fork(), kills the child by
 * a signal. Not tested: an invalid guardsize (EINVAL: the text does not
 * say which values are), EINTR (never returned). The value stored is in
 * pthread_attr_getguardsize.c.
 *
 * Clauses: POSIX: XSH pthread_attr_setguardsize().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_setguardsize()")
{
	EXPECT_TYPE(&pthread_attr_setguardsize, int (*)(pthread_attr_t *, size_t));
}

/* 0 and a page: 0, and threads are created with them. */
TEST_OPT(zero_and_a_page_are_accepted, .ref = "XSH pthread_attr_setguardsize()")
{
	pthread_attr_t	a;
	pt_run			r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setguardsize(&a, 0), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	EXPECT_EQ(pthread_attr_setguardsize(&a, pt_pageSize()), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	(void)pthread_attr_destroy(&a);
}

static volatile long	gPgsLimit = 1L << 40;

static long	pgsDeep(long n)
{
	volatile char	frame[1024];

	frame[0] = (char)n;
	if (n > gPgsLimit)
		return (0);
	return (pgsDeep(n + 1) + frame[0]);
}

static void	*pgsOverflow(void *arg)
{
	(void)arg;
	return ((void *)pgsDeep(0));
}

static int	pgsChild(void *arg)
{
	pthread_attr_t	a;
	pthread_t		t;

	(void)arg;
	if (pthread_attr_init(&a) != 0
		|| pthread_attr_setstacksize(&a, pt_stackMin() + 16 * pt_pageSize()) != 0
		|| pthread_attr_setguardsize(&a, 4 * pt_pageSize()) != 0)
		return (2);
	if (pthread_create(&t, &a, pgsOverflow, NULL) != 0)
		return (3);
	(void)pthread_join(t, NULL);
	return (4);
}

/* Overflowing into the guard area is an error: the process gets a signal. */
TEST_OPT(overflow_into_the_guard_is_an_error, .ref = "XSH pthread_attr_setguardsize()")
{
	PT_FORK(st, pgsChild, NULL);
	EXPECT_GE(st, 256);
}
