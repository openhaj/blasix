/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setinheritsched.c
 * @brief Tests for pthread_attr_setinheritsched() (<pthread.h>).
 * @Created: 2026/11/28 18:07:54 by Moutig
 * @Updated: 2026/11/28 18:09:06 by Moutig
 *
 * pthread_attr_setinheritsched(attr, inheritsched) sets the inheritsched
 * attribute and returns 0; both values are supported (no ENOTSUP).
 * PTHREAD_INHERIT_SCHED: the thread gets the scheduling policy of its
 * creator and the policy in attr is ignored, so SCHED_FIFO in attr needs
 * no privilege. PTHREAD_EXPLICIT_SCHED: the thread gets the policy of
 * attr; SCHED_FIFO without the privilege is EPERM from pthread_create(),
 * and the case is then skipped. The thread reads its policy with
 * pthread_getschedparam(). Not tested: EINVAL (may fail), EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_attr_setinheritsched(), XSH pthread_create().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_setinheritsched()")
{
	EXPECT_TYPE(&pthread_attr_setinheritsched, int (*)(pthread_attr_t *, int));
}

/* Both values are supported: 0. */
TEST_OPT(both_values_are_supported, .ref = "XSH pthread_attr_setinheritsched()")
{
	pthread_attr_t	a;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setinheritsched(&a, PTHREAD_INHERIT_SCHED), 0);
	EXPECT_EQ(pthread_attr_setinheritsched(&a, PTHREAD_EXPLICIT_SCHED), 0);
	(void)pthread_attr_destroy(&a);
}

static void	*psiPolicy(void *arg)
{
	struct sched_param	p;
	int					pol = -1;

	if (pthread_getschedparam(pthread_self(), &pol, &p) != 0)
		pol = -1;
	*(int *)arg = pol;
	return (NULL);
}

/* An attribute object asking for SCHED_FIFO at the lowest priority. */
static int	psiFifo(pthread_attr_t *a, int inherit)
{
	struct sched_param	p;

	lc_fill(&p, 0, sizeof(p));
	p.sched_priority = sched_get_priority_min(SCHED_FIFO);
	if (pthread_attr_init(a) != 0)
		return (-1);
	if (pthread_attr_setinheritsched(a, inherit) != 0
		|| pthread_attr_setschedpolicy(a, SCHED_FIFO) != 0
		|| pthread_attr_setschedparam(a, &p) != 0)
		return (-1);
	return (0);
}

/* PTHREAD_INHERIT_SCHED: the creator's policy, not the one in attr. */
TEST_OPT(inherit_takes_the_creator_policy, .ref = "XSH pthread_attr_setinheritsched()")
{
	pthread_attr_t		a;
	pthread_t			t;
	struct sched_param	p;
	int					mine = -1;
	int					got = -2;

	ASSERT_EQ(pthread_getschedparam(pthread_self(), &mine, &p), 0);
	ASSERT_EQ(psiFifo(&a, PTHREAD_INHERIT_SCHED), 0);
	ASSERT_EQ(pthread_create(&t, &a, psiPolicy, &got), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(got, mine);
	(void)pthread_attr_destroy(&a);
}

/* PTHREAD_EXPLICIT_SCHED: the policy in attr (or EPERM without privilege). */
TEST_OPT(explicit_takes_the_attr_policy, .ref = "XSH pthread_attr_setinheritsched()")
{
	pthread_attr_t	a;
	pthread_t		t;
	int				got = -2;
	int				e;

	ASSERT_EQ(psiFifo(&a, PTHREAD_EXPLICIT_SCHED), 0);
	e = pthread_create(&t, &a, psiPolicy, &got);
	(void)pthread_attr_destroy(&a);
	if (e == EPERM)
		SKIP("no privilege for SCHED_FIFO (EPERM, XSH pthread_create())");
	ASSERT_EQ(e, 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(got, SCHED_FIFO);
}
