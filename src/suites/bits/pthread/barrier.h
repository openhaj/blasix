/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file barrier.h
 * @brief Cases of PTHREAD_BARRIER_SERIAL_THREAD.
 * @Created: 2026/11/28 17:33:56 by Moutig
 * @Updated: 2026/11/28 17:35:18 by Moutig
 *
 * pthread_barrier_wait() returns it to one unspecified thread and zero to
 * the others: an int value other than zero (XSH pthread_barrier_wait()).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_PTHREAD_BARRIER_H
# define BITS_PTHREAD_BARRIER_H

# include "framework/lc.h"

# if defined(LC_REQ_PTHREAD_BARRIER_SERIAL_THREAD) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_BARRIER_SERIAL_THREAD__int, "PTHREAD_BARRIER_SERIAL_THREAD", is_an_int_value,
	.ref = "XSH pthread_barrier_wait()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_BARRIER_SERIAL_THREAD), (long long)(PTHREAD_BARRIER_SERIAL_THREAD));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_BARRIER_SERIAL_THREAD__constant, "PTHREAD_BARRIER_SERIAL_THREAD", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_BARRIER_SERIAL_THREAD) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_BARRIER_SERIAL_THREAD) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_BARRIER_SERIAL_THREAD) || defined(LC_DEFS_ALL)
/* Returned to one thread, zero to the others: it is not zero. */
TEST_SYM(PTHREAD_BARRIER_SERIAL_THREAD__nonzero, "PTHREAD_BARRIER_SERIAL_THREAD",
	differs_from_zero, .ref = "XSH pthread_barrier_wait()")
{
	EXPECT_NE(PTHREAD_BARRIER_SERIAL_THREAD, 0);
}
# endif

#endif
