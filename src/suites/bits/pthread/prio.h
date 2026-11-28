/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file prio.h
 * @brief Cases of the mutex protocols (PTHREAD_PRIO_*).
 * @Created: 2026/11/28 17:34:28 by Moutig
 * @Updated: 2026/11/28 17:35:45 by Moutig
 *
 * The protocol attribute of a mutex (TPP, TPI; RPP, RPI from 2008): int
 * arguments of pthread_mutexattr_setprotocol(), three different
 * behaviours, so different values. Each pair is checked when the target
 * requires both (the options select them separately).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_PTHREAD_PRIO_H
# define BITS_PTHREAD_PRIO_H

# include "framework/lc.h"

# if defined(LC_REQ_PTHREAD_PRIO_NONE) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_PRIO_NONE__int, "PTHREAD_PRIO_NONE", is_an_int_value,
	.ref = "XSH pthread_mutexattr_getprotocol()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_PRIO_NONE), (long long)(PTHREAD_PRIO_NONE));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_PRIO_NONE__constant, "PTHREAD_PRIO_NONE", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_PRIO_NONE) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_PRIO_NONE) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_PRIO_INHERIT) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_PRIO_INHERIT__int, "PTHREAD_PRIO_INHERIT", is_an_int_value,
	.ref = "XSH pthread_mutexattr_getprotocol()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_PRIO_INHERIT), (long long)(PTHREAD_PRIO_INHERIT));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_PRIO_INHERIT__constant, "PTHREAD_PRIO_INHERIT", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_PRIO_INHERIT) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_PRIO_INHERIT) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_PRIO_PROTECT) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_PRIO_PROTECT__int, "PTHREAD_PRIO_PROTECT", is_an_int_value,
	.ref = "XSH pthread_mutexattr_getprotocol()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_PRIO_PROTECT), (long long)(PTHREAD_PRIO_PROTECT));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_PRIO_PROTECT__constant, "PTHREAD_PRIO_PROTECT", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_PRIO_PROTECT) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_PRIO_PROTECT) != 0);
}
# endif

# if (defined(LC_REQ_PTHREAD_PRIO_NONE) && defined(LC_REQ_PTHREAD_PRIO_PROTECT)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_PRIO_NONE__none_and_protect_differ, "PTHREAD_PRIO_NONE", none_and_protect_differ,
	.ref = "XSH pthread_mutexattr_getprotocol()")
{
	EXPECT_NE((long long)(PTHREAD_PRIO_NONE), (long long)(PTHREAD_PRIO_PROTECT));
}
# endif

# if (defined(LC_REQ_PTHREAD_PRIO_INHERIT) && defined(LC_REQ_PTHREAD_PRIO_NONE)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_PRIO_INHERIT__inherit_and_none_differ, "PTHREAD_PRIO_INHERIT", inherit_and_none_differ,
	.ref = "XSH pthread_mutexattr_getprotocol()")
{
	EXPECT_NE((long long)(PTHREAD_PRIO_INHERIT), (long long)(PTHREAD_PRIO_NONE));
}
# endif

# if (defined(LC_REQ_PTHREAD_PRIO_INHERIT) && defined(LC_REQ_PTHREAD_PRIO_PROTECT)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_PRIO_INHERIT__inherit_and_protect_differ, "PTHREAD_PRIO_INHERIT", inherit_and_protect_differ,
	.ref = "XSH pthread_mutexattr_getprotocol()")
{
	EXPECT_NE((long long)(PTHREAD_PRIO_INHERIT), (long long)(PTHREAD_PRIO_PROTECT));
}
# endif

#endif
