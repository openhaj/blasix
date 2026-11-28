/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_init.c
 * @brief Tests for pthread_mutex_init() (<pthread.h>).
 * @Created: 2026/11/28 18:35:42 by Moutig
 * @Updated: 2026/11/28 18:36:28 by Moutig
 *
 * pthread_mutex_init(mutex, attr) initializes the mutex, unlocked, with
 * the attributes of attr (the defaults when attr is NULL) and returns 0.
 * PTHREAD_MUTEX_INITIALIZER gives a static mutex the same as init with a
 * NULL attr. Not tested: EAGAIN, ENOMEM and EPERM (cannot be caused), a
 * robust mutex without pshared (EINVAL may fail), initializing twice
 * (undefined), copies (XSH 2.9.9), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_init().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_init()")
{
	EXPECT_TYPE(&pthread_mutex_init,
		int (*)(pthread_mutex_t *, const pthread_mutexattr_t *));
}

/* attr NULL: 0, and the mutex is unlocked. */
TEST_OPT(null_attr_gives_an_unlocked_mutex, .ref = "XSH pthread_mutex_init()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pt_tryOther(&m), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* The attributes of attr are used: an ERRORCHECK mutex refuses a relock. */
TEST_OPT(uses_the_attributes, .since = "POSIX.1-2008", .ref = "XSH pthread_mutex_init()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), EDEADLK);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* A default attributes object: as attr NULL. */
TEST_OPT(default_attr_object, .ref = "XSH pthread_mutex_init()")
{
	pthread_mutexattr_t	a;
	pthread_mutex_t		m;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutex_init(&m, &a), 0);
	(void)pthread_mutexattr_destroy(&a);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

static pthread_mutex_t	gPmiStatic = PTHREAD_MUTEX_INITIALIZER;

/* PTHREAD_MUTEX_INITIALIZER: an unlocked mutex with the default attributes. */
TEST_OPT(static_initializer, .ref = "XSH pthread_mutex_init()")
{
	EXPECT_EQ(pt_tryOther(&gPmiStatic), 0);
	EXPECT_EQ(pthread_mutex_lock(&gPmiStatic), 0);
	EXPECT_EQ(pt_tryOther(&gPmiStatic), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&gPmiStatic), 0);
}
