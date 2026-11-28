/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_setprotocol.c
 * @brief Tests for pthread_mutexattr_setprotocol() (<pthread.h>).
 * @Created: 2026/11/28 18:30:07 by Moutig
 * @Updated: 2026/11/28 18:31:34 by Moutig
 *
 * pthread_mutexattr_setprotocol(attr, protocol) sets the protocol
 * attribute and returns 0 (TPP; TPI, RPP, RPI); a value the
 * implementation does not support is ENOTSUP. PTHREAD_PRIO_NONE and, with
 * TPP, PTHREAD_PRIO_PROTECT are supported; PTHREAD_PRIO_INHERIT is with
 * TPI. A mutex is made with each. What the protocols do to priorities
 * needs realtime privileges and is not checked. Not tested: EINVAL and
 * EPERM (may fail), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_setprotocol().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_setprotocol()")
{
	EXPECT_TYPE(&pthread_mutexattr_setprotocol, int (*)(pthread_mutexattr_t *, int));
}

/* 0 for protocol, and a mutex is made with it. @return 0 or the step that failed. */
static int	pspMake(int protocol)
{
	pthread_mutexattr_t	a;
	pthread_mutex_t		m;
	int					bad = 0;

	if (pthread_mutexattr_init(&a) != 0)
		return (1);
	if (pthread_mutexattr_setprotocol(&a, protocol) != 0)
		bad = 2;
	else if (pthread_mutex_init(&m, &a) != 0)
		bad = 3;
	else
		(void)pthread_mutex_destroy(&m);
	(void)pthread_mutexattr_destroy(&a);
	return (bad);
}

/* PTHREAD_PRIO_NONE and PTHREAD_PRIO_PROTECT (TPP): 0, and mutexes are made. */
TEST_OPT(none_and_protect_supported, .ref = "XSH pthread_mutexattr_setprotocol()")
{
	EXPECT_EQ(pspMake(PTHREAD_PRIO_NONE), 0);
	EXPECT_EQ(pspMake(PTHREAD_PRIO_PROTECT), 0);
}

/* PTHREAD_PRIO_INHERIT (TPI): 0, and a mutex is made. */
TEST_OPT(inherit_supported, .opt = "TPI", .ref = "XSH pthread_mutexattr_setprotocol()")
{
	EXPECT_EQ(pspMake(PTHREAD_PRIO_INHERIT), 0);
}

/* None of the protocols: EINVAL or ENOTSUP, not 0. */
TEST_OPT(other_value_refused, .ref = "XSH pthread_mutexattr_setprotocol()")
{
	pthread_mutexattr_t	a;
	int					bad = 1000;
	int					e;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	e = pthread_mutexattr_setprotocol(&a, bad);
	EXPECT_TRUE(e == ENOTSUP || e == EINVAL);
	(void)pthread_mutexattr_destroy(&a);
}
