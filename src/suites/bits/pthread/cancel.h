/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file cancel.h
 * @brief Cases of PTHREAD_CANCEL_* and PTHREAD_CANCELED.
 * @Created: 2026/11/28 17:34:03 by Moutig
 * @Updated: 2026/11/28 17:35:29 by Moutig
 *
 * The cancelability states and types are the int arguments of
 * pthread_setcancelstate() and pthread_setcanceltype(), which tell each
 * pair apart; PTHREAD_CANCELED is the value pthread_join() gives for a
 * canceled thread (XBD <pthread.h>, XSH pthread_setcancelstate(),
 * pthread_join()).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_PTHREAD_CANCEL_H
# define BITS_PTHREAD_CANCEL_H

# include "framework/lc.h"

# if defined(LC_REQ_PTHREAD_CANCEL_ENABLE) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_CANCEL_ENABLE__int, "PTHREAD_CANCEL_ENABLE", is_an_int_value,
	.ref = "XSH pthread_setcancelstate()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_CANCEL_ENABLE), (long long)(PTHREAD_CANCEL_ENABLE));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_CANCEL_ENABLE__constant, "PTHREAD_CANCEL_ENABLE", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_CANCEL_ENABLE) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_CANCEL_ENABLE) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_CANCEL_DISABLE) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_CANCEL_DISABLE__int, "PTHREAD_CANCEL_DISABLE", is_an_int_value,
	.ref = "XSH pthread_setcancelstate()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_CANCEL_DISABLE), (long long)(PTHREAD_CANCEL_DISABLE));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_CANCEL_DISABLE__constant, "PTHREAD_CANCEL_DISABLE", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_CANCEL_DISABLE) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_CANCEL_DISABLE) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_CANCEL_DEFERRED) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_CANCEL_DEFERRED__int, "PTHREAD_CANCEL_DEFERRED", is_an_int_value,
	.ref = "XSH pthread_setcancelstate()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_CANCEL_DEFERRED), (long long)(PTHREAD_CANCEL_DEFERRED));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_CANCEL_DEFERRED__constant, "PTHREAD_CANCEL_DEFERRED", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_CANCEL_DEFERRED) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_CANCEL_DEFERRED) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_CANCEL_ASYNCHRONOUS) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_CANCEL_ASYNCHRONOUS__int, "PTHREAD_CANCEL_ASYNCHRONOUS", is_an_int_value,
	.ref = "XSH pthread_setcancelstate()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_CANCEL_ASYNCHRONOUS), (long long)(PTHREAD_CANCEL_ASYNCHRONOUS));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_CANCEL_ASYNCHRONOUS__constant, "PTHREAD_CANCEL_ASYNCHRONOUS", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_CANCEL_ASYNCHRONOUS) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_CANCEL_ASYNCHRONOUS) != 0);
}
# endif

# if (defined(LC_REQ_PTHREAD_CANCEL_ENABLE) && defined(LC_REQ_PTHREAD_CANCEL_DISABLE)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_CANCEL_ENABLE__enable_and_disable_differ, "PTHREAD_CANCEL_ENABLE", enable_and_disable_differ,
	.ref = "XSH pthread_setcancelstate()")
{
	EXPECT_NE((long long)(PTHREAD_CANCEL_ENABLE), (long long)(PTHREAD_CANCEL_DISABLE));
}
# endif

# if (defined(LC_REQ_PTHREAD_CANCEL_DEFERRED) && defined(LC_REQ_PTHREAD_CANCEL_ASYNCHRONOUS)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_CANCEL_DEFERRED__deferred_and_asynchronous_differ, "PTHREAD_CANCEL_DEFERRED", deferred_and_asynchronous_differ,
	.ref = "XSH pthread_setcancelstate()")
{
	EXPECT_NE((long long)(PTHREAD_CANCEL_DEFERRED), (long long)(PTHREAD_CANCEL_ASYNCHRONOUS));
}
# endif

# if defined(LC_REQ_PTHREAD_CANCELED) || defined(LC_DEFS_ALL)
/* pthread_join() stores it through a void **: a void * value. */
TEST_SYM(PTHREAD_CANCELED__pointer, "PTHREAD_CANCELED", converts_to_void_pointer,
	.ref = "XSH pthread_join()")
{
	void	*p = PTHREAD_CANCELED;

	EXPECT_PTREQ(p, (void *)PTHREAD_CANCELED);
}
# endif

#endif
