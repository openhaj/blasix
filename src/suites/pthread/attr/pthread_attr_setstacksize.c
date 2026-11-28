/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setstacksize.c
 * @brief Tests for pthread_attr_setstacksize() (<pthread.h>).
 * @Created: 2026/11/28 18:02:50 by Moutig
 * @Updated: 2026/11/28 18:05:30 by Moutig
 *
 * pthread_attr_setstacksize(attr, stacksize) sets the minimum stack size
 * of the threads created with attr and returns 0. Below
 * {PTHREAD_STACK_MIN}: EINVAL. A thread created with a stack size larger
 * than usual (32 MiB) can use most of it: it runs in a child of fork(),
 * so that a stack too small kills the child, not the unit. Not tested: a
 * size beyond the system limit (unknown), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_setstacksize().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_setstacksize()")
{
	EXPECT_TYPE(&pthread_attr_setstacksize, int (*)(pthread_attr_t *, size_t));
}

/* {PTHREAD_STACK_MIN} itself is accepted, and threads are created with it. */
TEST_OPT(stack_min_is_accepted, .ref = "XSH pthread_attr_setstacksize()")
{
	pthread_attr_t	a;
	pt_run			r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setstacksize(&a, pt_stackMin()), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	(void)pthread_attr_destroy(&a);
}

/* Less than {PTHREAD_STACK_MIN}: EINVAL. */
TEST_OPT(below_stack_min_einval, .ref = "XSH pthread_attr_setstacksize()")
{
	pthread_attr_t	a;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setstacksize(&a, pt_stackMin() - 1), EINVAL);
	EXPECT_EQ(pthread_attr_setstacksize(&a, 0), EINVAL);
	(void)pthread_attr_destroy(&a);
}

# define PSS_SIZE	(32UL << 20)
# define PSS_USED	(24UL << 20)

static void	*pssUseStack(void *arg)
{
	volatile char	big[PSS_USED];

	for (unsigned long i = 0; i < PSS_USED; i += 1024)
		big[i] = (char)i;
	*(int *)arg = big[PSS_USED - 1024] == (char)(PSS_USED - 1024);
	return (NULL);
}

static int	pssChild(void *arg)
{
	pthread_attr_t	a;
	pthread_t		t;
	int				ok = 0;

	(void)arg;
	if (pthread_attr_init(&a) != 0 || pthread_attr_setstacksize(&a, PSS_SIZE) != 0)
		return (2);
	if (pthread_create(&t, &a, pssUseStack, &ok) != 0 || pthread_join(t, NULL) != 0)
		return (3);
	return (ok ? 0 : 4);
}

/* The stack is at least stacksize bytes: 24 MiB of locals in 32 MiB. */
TEST_OPT(thread_gets_at_least_stacksize, .ref = "XSH pthread_attr_setstacksize()")
{
	PT_FORK(st, pssChild, NULL);
	EXPECT_EQ(st, 0);
}
