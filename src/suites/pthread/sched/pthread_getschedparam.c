/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_getschedparam.c
 * @brief Tests for pthread_getschedparam() (<pthread.h>).
 * @Created: 2026/11/28 18:15:29 by Moutig
 * @Updated: 2026/11/28 18:16:15 by Moutig
 *
 * pthread_getschedparam(thread, policy, param) stores the scheduling
 * policy and parameters of thread and returns 0 (TPS). The priority is
 * the one set by the most recent pthread_setschedparam(),
 * pthread_setschedprio() or pthread_create() affecting the thread. A
 * thread of the process can be asked about another one. The cases that
 * need SCHED_FIFO run in a thread made for them and are skipped on EPERM
 * (no privilege). Not tested: ESRCH (a dead thread ID is undefined to
 * use; may fail in 2001), priority inheritance and ceilings (the mutex
 * units), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_getschedparam().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <sched.h>

/* Run fn(arg) in a new thread (default attributes) and join it. */
static int	inThread(void *(*fn)(void *), void *arg)
{
	pt_run	r;

	lc_fill(&r, 0, sizeof(r));
	r.fn = fn;
	r.arg = arg;
	return (pt_runWith(NULL, 1, &r));
}

TEST_OPT(prototype, .ref = "XSH pthread_getschedparam()")
{
	EXPECT_TYPE(&pthread_getschedparam, int (*)(pthread_t, int *, struct sched_param *));
}

/* The calling thread: 0, and a priority valid for its policy. */
TEST_OPT(own_policy_and_priority, .ref = "XSH pthread_getschedparam()")
{
	struct sched_param	p;
	int					pol = -1;

	lc_fill(&p, 0, sizeof(p));
	ASSERT_EQ(pthread_getschedparam(pthread_self(), &pol, &p), 0);
	EXPECT_GE(p.sched_priority, sched_get_priority_min(pol));
	EXPECT_LE(p.sched_priority, sched_get_priority_max(pol));
}

typedef struct pgs_other
{
	int					pol;
	struct sched_param	p;
	int					ready;
	int					go;
}	pgs_other;

static void	*pgsSelf(void *arg)
{
	pgs_other	*o = arg;

	if (pthread_getschedparam(pthread_self(), &o->pol, &o->p) != 0)
		o->pol = -1;
	pt_set(&o->ready, 1);
	(void)pt_waitFor(&o->go, 1);
	return (NULL);
}

/* Another thread: the values that thread sees for itself. */
TEST_OPT(another_thread, .ref = "XSH pthread_getschedparam()")
{
	static pgs_other	o;
	pthread_t			t;
	struct sched_param	p;
	int					pol = -2;

	lc_fill(&o, 0, sizeof(o));
	lc_fill(&p, 0, sizeof(p));
	ASSERT_EQ(pthread_create(&t, NULL, pgsSelf, &o), 0);
	if (pt_waitFor(&o.ready, 1))
	{
		EXPECT_EQ(pthread_getschedparam(t, &pol, &p), 0);
		EXPECT_EQ(pol, o.pol);
		EXPECT_EQ(p.sched_priority, o.p.sched_priority);
	}
	else
		EXPECT_TRUE(0);
	pt_set(&o.go, 1);
	ASSERT_EQ(pthread_join(t, NULL), 0);
}

/* What a thread found: -1 no privilege, 0 as expected, 1 not. */
static int	gPgsResult;

static void	*pgsAfterSetparam(void *arg)
{
	struct sched_param	p;
	int					pol = -1;
	int					lo = sched_get_priority_min(SCHED_FIFO);

	(void)arg;
	lc_fill(&p, 0, sizeof(p));
	p.sched_priority = lo;
	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &p) == EPERM)
		return (gPgsResult = -1, NULL);
	lc_fill(&p, 0, sizeof(p));
	gPgsResult = pthread_getschedparam(pthread_self(), &pol, &p) != 0
		|| pol != SCHED_FIFO || p.sched_priority != lo;
	return (NULL);
}

/* After pthread_setschedparam(): its policy and priority. */
TEST_OPT(reflects_setschedparam, .ref = "XSH pthread_getschedparam()")
{
	gPgsResult = 1;
	ASSERT_EQ(inThread(pgsAfterSetparam, NULL), 0);
	if (gPgsResult == -1)
		SKIP("no privilege for SCHED_FIFO (EPERM)");
	EXPECT_EQ(gPgsResult, 0);
}

static void	*pgsAfterSetprio(void *arg)
{
	struct sched_param	p;
	int					pol = -1;
	int					lo = sched_get_priority_min(SCHED_FIFO);
	int					hi = sched_get_priority_max(SCHED_FIFO);
	int					want = lo < hi ? lo + 1 : lo;

	(void)arg;
	lc_fill(&p, 0, sizeof(p));
	p.sched_priority = lo;
	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &p) == EPERM)
		return (gPgsResult = -1, NULL);
	if (pthread_setschedprio(pthread_self(), want) == EPERM)
		return (gPgsResult = -1, NULL);
	lc_fill(&p, 0, sizeof(p));
	gPgsResult = pthread_getschedparam(pthread_self(), &pol, &p) != 0
		|| p.sched_priority != want;
	return (NULL);
}

/* After pthread_setschedprio(): its priority. */
TEST_OPT(reflects_setschedprio, .ref = "XSH pthread_getschedparam()")
{
	gPgsResult = 1;
	ASSERT_EQ(inThread(pgsAfterSetprio, NULL), 0);
	if (gPgsResult == -1)
		SKIP("no privilege for SCHED_FIFO (EPERM)");
	EXPECT_EQ(gPgsResult, 0);
}

static void	*pgsCreated(void *arg)
{
	struct sched_param	p;
	int					pol = -1;

	lc_fill(&p, 0, sizeof(p));
	gPgsResult = pthread_getschedparam(pthread_self(), &pol, &p) != 0
		|| pol != SCHED_FIFO || p.sched_priority != *(int *)arg;
	return (NULL);
}

/* After pthread_create() with explicit SCHED_FIFO: that policy and priority. */
TEST_OPT(reflects_pthread_create, .ref = "XSH pthread_getschedparam()")
{
	pthread_attr_t		a;
	pthread_t			t;
	struct sched_param	p;
	int					e;

	lc_fill(&p, 0, sizeof(p));
	p.sched_priority = sched_get_priority_min(SCHED_FIFO);
	gPgsResult = 1;
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setinheritsched(&a, PTHREAD_EXPLICIT_SCHED), 0);
	ASSERT_EQ(pthread_attr_setschedpolicy(&a, SCHED_FIFO), 0);
	ASSERT_EQ(pthread_attr_setschedparam(&a, &p), 0);
	e = pthread_create(&t, &a, pgsCreated, &p.sched_priority);
	(void)pthread_attr_destroy(&a);
	if (e == EPERM)
		SKIP("no privilege for SCHED_FIFO (EPERM)");
	ASSERT_EQ(e, 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(gPgsResult, 0);
}
