/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_unlock.c
 * @brief Tests for pthread_mutex_unlock() (<pthread.h>).
 * @Created: 2026/11/28 18:36:10 by Moutig
 * @Updated: 2026/11/28 18:36:59 by Moutig
 *
 * pthread_mutex_unlock(mutex) releases the mutex and returns 0: another
 * thread can lock it, and a thread blocked on it gets it. ERRORCHECK and
 * RECURSIVE: unlocking a mutex another thread owns, or an unlocked one,
 * is an error, EPERM for ERRORCHECK from 2008 and for both in 2024. Not
 * tested: NORMAL and DEFAULT in those cases (undefined), robust mutexes
 * (their unit), which waiter gets it (the scheduling policy), EINTR
 * (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_unlock(), XSH pthread_mutexattr_settype().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_unlock()")
{
	EXPECT_TYPE(&pthread_mutex_unlock, int (*)(pthread_mutex_t *));
}

/* 0, and another thread can lock it. */
TEST_OPT(releases_the_mutex, .ref = "XSH pthread_mutex_unlock()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* A thread blocked in pthread_mutex_lock() gets the mutex. */
TEST_OPT(blocked_thread_acquires, .ref = "XSH pthread_mutex_unlock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;

	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	lc_fill(&h, 0, sizeof(h));
	h.m = &m;
	ASSERT_EQ(pthread_create(&h.t, NULL, pt_holdBody, &h), 0);
	lc_sleep(20 * 1000000LL);
	EXPECT_EQ(pt_get(&h.held), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_TRUE(pt_waitFor(&h.held, 1));
	EXPECT_EQ(h.ret, 0);
	pt_holdEnd(&h);
}

/* ERRORCHECK or RECURSIVE: the result of unlocking it while another thread owns it. */
static int	pmuNotOwner(int type)
{
	pthread_mutex_t	m;
	pt_hold			h;
	int				e;

	if (pt_mutexOfType(&m, type) != 0 || pt_holdStart(&h, &m) != 0)
		return (-1);
	e = pthread_mutex_unlock(&m);
	pt_holdEnd(&h);
	(void)pthread_mutex_destroy(&m);
	return (e);
}

/* The result of unlocking an unlocked mutex of that type. */
static int	pmuUnlocked(int type)
{
	pthread_mutex_t	m;
	int				e;

	if (pt_mutexOfType(&m, type) != 0)
		return (-1);
	e = pthread_mutex_unlock(&m);
	(void)pthread_mutex_destroy(&m);
	return (e);
}

/* ERRORCHECK and RECURSIVE: another thread owns it, or nobody does: an error. */
TEST_OPT(not_owner_is_an_error, .ref = "XSH pthread_mutexattr_settype()")
{
	int	e;

	e = pmuNotOwner(PTHREAD_MUTEX_ERRORCHECK);
	EXPECT_TRUE(e > 0);
	e = pmuNotOwner(PTHREAD_MUTEX_RECURSIVE);
	EXPECT_TRUE(e > 0);
	e = pmuUnlocked(PTHREAD_MUTEX_ERRORCHECK);
	EXPECT_TRUE(e > 0);
	e = pmuUnlocked(PTHREAD_MUTEX_RECURSIVE);
	EXPECT_TRUE(e > 0);
}

/* ERRORCHECK, not the owner: EPERM (2008). */
TEST_OPT(errorcheck_not_owner_eperm, .since = "POSIX.1-2008", .ref = "XSH pthread_mutex_unlock()")
{
	EXPECT_EQ(pmuNotOwner(PTHREAD_MUTEX_ERRORCHECK), EPERM);
	EXPECT_EQ(pmuUnlocked(PTHREAD_MUTEX_ERRORCHECK), EPERM);
}

/* RECURSIVE, not the owner: EPERM (2024). */
TEST_OPT(recursive_not_owner_eperm, .since = "POSIX.1-2024", .ref = "XSH pthread_mutex_unlock()")
{
	EXPECT_EQ(pmuNotOwner(PTHREAD_MUTEX_RECURSIVE), EPERM);
	EXPECT_EQ(pmuUnlocked(PTHREAD_MUTEX_RECURSIVE), EPERM);
}
