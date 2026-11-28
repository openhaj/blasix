/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file initializers.h
 * @brief Cases of the static initializers (PTHREAD_*_INITIALIZER, PTHREAD_ONCE_INIT, PTHREAD_NULL).
 * @Created: 2026/11/28 17:34:11 by Moutig
 * @Updated: 2026/11/28 17:35:39 by Moutig
 *
 * XBD <pthread.h> (2008 on): compile-time constant expressions valid as
 * initializers of pthread_cond_t, pthread_mutex_t and pthread_rwlock_t;
 * PTHREAD_NULL (2024) of pthread_t. XSH pthread_once(): PTHREAD_ONCE_INIT
 * initializes a pthread_once_t. An object of static storage duration
 * needs a constant initializer: compiling it is the test. What the
 * objects do once initialized (and PTHREAD_NULL against existing thread
 * IDs) needs the functions: their units check it.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_PTHREAD_INITIALIZERS_H
# define BITS_PTHREAD_INITIALIZERS_H

# include "framework/lc.h"

# if defined(LC_REQ_PTHREAD_COND_INITIALIZER) || defined(LC_DEFS_ALL)
static pthread_cond_t	gBitsCond = PTHREAD_COND_INITIALIZER;

TEST_SYM(PTHREAD_COND_INITIALIZER__static, "PTHREAD_COND_INITIALIZER",
	initializes_a_static_pthread_cond_t, .ref = "XBD <pthread.h>")
{
	EXPECT_TYPE(gBitsCond, pthread_cond_t);
}
# endif

# if defined(LC_REQ_PTHREAD_MUTEX_INITIALIZER) || defined(LC_DEFS_ALL)
static pthread_mutex_t	gBitsMutex = PTHREAD_MUTEX_INITIALIZER;

TEST_SYM(PTHREAD_MUTEX_INITIALIZER__static, "PTHREAD_MUTEX_INITIALIZER",
	initializes_a_static_pthread_mutex_t, .ref = "XBD <pthread.h>")
{
	EXPECT_TYPE(gBitsMutex, pthread_mutex_t);
}
# endif

# if defined(LC_REQ_PTHREAD_RWLOCK_INITIALIZER) || defined(LC_DEFS_ALL)
static pthread_rwlock_t	gBitsRwlock = PTHREAD_RWLOCK_INITIALIZER;

TEST_SYM(PTHREAD_RWLOCK_INITIALIZER__static, "PTHREAD_RWLOCK_INITIALIZER",
	initializes_a_static_pthread_rwlock_t, .ref = "XBD <pthread.h>")
{
	EXPECT_TYPE(gBitsRwlock, pthread_rwlock_t);
}
# endif

# if defined(LC_REQ_PTHREAD_ONCE_INIT) || defined(LC_DEFS_ALL)
static pthread_once_t	gBitsOnce = PTHREAD_ONCE_INIT;

TEST_SYM(PTHREAD_ONCE_INIT__static, "PTHREAD_ONCE_INIT",
	initializes_a_static_pthread_once_t, .ref = "XSH pthread_once()")
{
	EXPECT_TYPE(gBitsOnce, pthread_once_t);
}
# endif

# if defined(LC_REQ_PTHREAD_NULL) || defined(LC_DEFS_ALL)
static pthread_t		gBitsNull = PTHREAD_NULL;

TEST_SYM(PTHREAD_NULL__static, "PTHREAD_NULL", initializes_a_static_pthread_t,
	.ref = "XBD <pthread.h>")
{
	EXPECT_TYPE(gBitsNull, pthread_t);
}
# endif

#endif
