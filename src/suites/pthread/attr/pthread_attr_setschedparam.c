/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setschedparam.c
 * @brief Tests for pthread_attr_setschedparam() (<pthread.h>).
 * @Created: 2026/11/28 18:08:01 by Moutig
 * @Updated: 2026/11/28 18:09:14 by Moutig
 *
 * pthread_attr_setschedparam(attr, param) sets the scheduling parameters
 * of attr and returns 0. The parameters of a new object are accepted;
 * with TPS, the lowest and highest SCHED_FIFO and SCHED_RR priorities are.
 * What a thread gets from them is in pthread_attr_setinheritsched.c. Not
 * tested: an invalid param (EINVAL may fail), ENOTSUP (no value is known
 * to be unsupported), SCHED_SPORADIC (TSP), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_setschedparam().
 */
#include "pthread/pthread_cases.h"

#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_setschedparam()")
{
	EXPECT_TYPE(&pthread_attr_setschedparam,
		int (*)(pthread_attr_t *, const struct sched_param *));
}

/* The parameters of a new object: 0, and threads are created with them. */
TEST_OPT(default_parameters_accepted, .ref = "XSH pthread_attr_setschedparam()")
{
	pthread_attr_t		a;
	struct sched_param	p;
	pt_run				r;

	lc_fill(&p, 0, sizeof(p));
	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_getschedparam(&a, &p), 0);
	EXPECT_EQ(pthread_attr_setschedparam(&a, &p), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	(void)pthread_attr_destroy(&a);
}

/* TPS: the bounds of the SCHED_FIFO and SCHED_RR priorities are accepted. */
TEST_OPT(realtime_priority_bounds_accepted, .opt = "TPS",
	.ref = "XSH pthread_attr_setschedparam()")
{
	const int			pol[2] = {SCHED_FIFO, SCHED_RR};
	pthread_attr_t		a;
	int					bad = 0;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	for (int i = 0; i < 2; i++)
	{
		struct sched_param	p;

		lc_fill(&p, 0, sizeof(p));
		bad += pthread_attr_setschedpolicy(&a, pol[i]) != 0;
		p.sched_priority = sched_get_priority_min(pol[i]);
		bad += pthread_attr_setschedparam(&a, &p) != 0;
		p.sched_priority = sched_get_priority_max(pol[i]);
		bad += pthread_attr_setschedparam(&a, &p) != 0;
	}
	EXPECT_EQ(bad, 0);
	(void)pthread_attr_destroy(&a);
}
