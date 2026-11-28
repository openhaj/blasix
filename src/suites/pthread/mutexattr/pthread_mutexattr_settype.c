/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_settype.c
 * @brief Tests for pthread_mutexattr_settype() (<pthread.h>).
 * @Created: 2026/11/28 18:30:33 by Moutig
 * @Updated: 2026/11/28 18:31:55 by Moutig
 *
 * pthread_mutexattr_settype(attr, type) sets the type attribute and
 * returns 0 for each of PTHREAD_MUTEX_NORMAL, _ERRORCHECK, _RECURSIVE and
 * _DEFAULT; a value that is no type is EINVAL. The type is what a mutex made with
 * the object does: an ERRORCHECK mutex refuses a relock, a RECURSIVE one
 * accepts it (the details are in the pthread_mutex_* units). Not tested:
 * EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_settype().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_settype()")
{
	EXPECT_TYPE(&pthread_mutexattr_settype, int (*)(pthread_mutexattr_t *, int));
}

/* The four types: 0. */
TEST_OPT(valid_types_accepted, .ref = "XSH pthread_mutexattr_settype()")
{
	pthread_mutexattr_t	a;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_settype(&a, PTHREAD_MUTEX_NORMAL), 0);
	EXPECT_EQ(pthread_mutexattr_settype(&a, PTHREAD_MUTEX_ERRORCHECK), 0);
	EXPECT_EQ(pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE), 0);
	EXPECT_EQ(pthread_mutexattr_settype(&a, PTHREAD_MUTEX_DEFAULT), 0);
	(void)pthread_mutexattr_destroy(&a);
}

/*
 * A value that is no type: EINVAL. The valid types "include" the four, so
 * an implementation may have more (glibc: 3, PTHREAD_MUTEX_ADAPTIVE_NP);
 * -1 and 1000 are taken as invalid.
 */
TEST_OPT(invalid_type_einval, .ref = "XSH pthread_mutexattr_settype()")
{
	pthread_mutexattr_t	a;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_settype(&a, -1), EINVAL);
	EXPECT_EQ(pthread_mutexattr_settype(&a, 1000), EINVAL);
	(void)pthread_mutexattr_destroy(&a);
}

/* ERRORCHECK: a relock returns an error, the mutex stays locked once. */
TEST_OPT(errorcheck_refuses_a_relock, .ref = "XSH pthread_mutexattr_settype()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_NE(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* RECURSIVE: a relock succeeds. */
TEST_OPT(recursive_accepts_a_relock, .ref = "XSH pthread_mutexattr_settype()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_RECURSIVE), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}
