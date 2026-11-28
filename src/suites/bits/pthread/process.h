/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file process.h
 * @brief Cases of PTHREAD_PROCESS_PRIVATE and PTHREAD_PROCESS_SHARED.
 * @Created: 2026/11/28 17:34:39 by Moutig
 * @Updated: 2026/11/28 17:35:05 by Moutig
 *
 * The process-shared attribute of the synchronization objects: int
 * arguments of the *attr_setpshared() functions and of
 * pthread_spin_init(), two different behaviours (XBD <pthread.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_PTHREAD_PROCESS_H
# define BITS_PTHREAD_PROCESS_H

# include "framework/lc.h"

# if defined(LC_REQ_PTHREAD_PROCESS_PRIVATE) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_PROCESS_PRIVATE__int, "PTHREAD_PROCESS_PRIVATE", is_an_int_value,
	.ref = "XSH pthread_mutexattr_getpshared()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_PROCESS_PRIVATE), (long long)(PTHREAD_PROCESS_PRIVATE));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_PROCESS_PRIVATE__constant, "PTHREAD_PROCESS_PRIVATE", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_PROCESS_PRIVATE) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_PROCESS_PRIVATE) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_PROCESS_SHARED) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_PROCESS_SHARED__int, "PTHREAD_PROCESS_SHARED", is_an_int_value,
	.ref = "XSH pthread_mutexattr_getpshared()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_PROCESS_SHARED), (long long)(PTHREAD_PROCESS_SHARED));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_PROCESS_SHARED__constant, "PTHREAD_PROCESS_SHARED", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_PROCESS_SHARED) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_PROCESS_SHARED) != 0);
}
# endif

# if (defined(LC_REQ_PTHREAD_PROCESS_PRIVATE) && defined(LC_REQ_PTHREAD_PROCESS_SHARED)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_PROCESS_PRIVATE__private_and_shared_differ, "PTHREAD_PROCESS_PRIVATE", private_and_shared_differ,
	.ref = "XSH pthread_mutexattr_getpshared()")
{
	EXPECT_NE((long long)(PTHREAD_PROCESS_PRIVATE), (long long)(PTHREAD_PROCESS_SHARED));
}
# endif

#endif
