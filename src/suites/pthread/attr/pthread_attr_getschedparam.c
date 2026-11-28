/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_getschedparam.c
 * @brief Tests for pthread_attr_getschedparam() (<pthread.h>).
 * @Created: 2026/11/28 18:07:31 by Moutig
 * @Updated: 2026/11/28 18:08:46 by Moutig
 *
 * pthread_attr_getschedparam(attr, param) stores the scheduling
 * parameters of attr (sched_priority for SCHED_FIFO and SCHED_RR) and
 * returns 0: those last set. With TPS, a SCHED_FIFO priority set in the
 * object is given back. Not tested: the SCHED_SPORADIC members (TSP), an
 * object that is not initialized (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_getschedparam().
 */
#include "pthread/pthread_cases.h"

#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_getschedparam()")
{
	EXPECT_TYPE(&pthread_attr_getschedparam,
		int (*)(const pthread_attr_t *, struct sched_param *));
}

/* A new object: 0, and the parameters can be set back as they are. */
TEST_OPT(default_parameters_round_trip, .ref = "XSH pthread_attr_getschedparam()")
{
	pthread_attr_t		a;
	struct sched_param	p;
	struct sched_param	q;

	lc_fill(&p, 0, sizeof(p));
	lc_fill(&q, 0xff, sizeof(q));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_getschedparam(&a, &p), 0);
	EXPECT_EQ(pthread_attr_setschedparam(&a, &p), 0);
	EXPECT_EQ(pthread_attr_getschedparam(&a, &q), 0);
	EXPECT_EQ(q.sched_priority, p.sched_priority);
	(void)pthread_attr_destroy(&a);
}

/* SCHED_FIFO (TPS): the priority set is given back. */
TEST_OPT(gives_the_fifo_priority_set, .opt = "TPS", .ref = "XSH pthread_attr_getschedparam()")
{
	pthread_attr_t		a;
	struct sched_param	p;
	struct sched_param	q;
	int					lo = sched_get_priority_min(SCHED_FIFO);
	int					hi = sched_get_priority_max(SCHED_FIFO);

	lc_fill(&p, 0, sizeof(p));
	lc_fill(&q, 0, sizeof(q));
	p.sched_priority = lo < hi ? lo + 1 : lo;
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setschedpolicy(&a, SCHED_FIFO), 0);
	ASSERT_EQ(pthread_attr_setschedparam(&a, &p), 0);
	EXPECT_EQ(pthread_attr_getschedparam(&a, &q), 0);
	EXPECT_EQ(q.sched_priority, p.sched_priority);
	(void)pthread_attr_destroy(&a);
}
