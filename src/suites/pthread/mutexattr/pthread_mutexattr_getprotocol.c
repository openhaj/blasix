/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_getprotocol.c
 * @brief Tests for pthread_mutexattr_getprotocol() (<pthread.h>).
 * @Created: 2026/11/28 18:29:22 by Moutig
 * @Updated: 2026/11/28 18:30:58 by Moutig
 *
 * pthread_mutexattr_getprotocol(attr, protocol) stores the protocol
 * attribute and returns 0 (TPP; TPI, RPP, RPI): PTHREAD_PRIO_NONE by
 * default, the value last set otherwise. PTHREAD_PRIO_INHERIT is checked
 * with TPI only. Not tested: EINVAL and EPERM (may fail), EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_getprotocol().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_getprotocol()")
{
	EXPECT_TYPE(&pthread_mutexattr_getprotocol, int (*)(const pthread_mutexattr_t *, int *));
}

/* The default is PTHREAD_PRIO_NONE. */
TEST_OPT(default_is_none, .ref = "XSH pthread_mutexattr_getprotocol()")
{
	pthread_mutexattr_t	a;
	int					v = -12345;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_getprotocol(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_PRIO_NONE);
	(void)pthread_mutexattr_destroy(&a);
}

/* PTHREAD_PRIO_PROTECT (TPP): given back once set. */
TEST_OPT(gives_protect_once_set, .ref = "XSH pthread_mutexattr_getprotocol()")
{
	pthread_mutexattr_t	a;
	int					v = -12345;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	ASSERT_EQ(pthread_mutexattr_setprotocol(&a, PTHREAD_PRIO_PROTECT), 0);
	EXPECT_EQ(pthread_mutexattr_getprotocol(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_PRIO_PROTECT);
	(void)pthread_mutexattr_destroy(&a);
}

/* PTHREAD_PRIO_INHERIT (TPI): given back once set. */
TEST_OPT(gives_inherit_once_set, .opt = "TPI", .ref = "XSH pthread_mutexattr_getprotocol()")
{
	pthread_mutexattr_t	a;
	int					v = -12345;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	ASSERT_EQ(pthread_mutexattr_setprotocol(&a, PTHREAD_PRIO_INHERIT), 0);
	EXPECT_EQ(pthread_mutexattr_getprotocol(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_PRIO_INHERIT);
	(void)pthread_mutexattr_destroy(&a);
}
