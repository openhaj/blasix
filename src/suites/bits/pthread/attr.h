/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file attr.h
 * @brief Cases of the thread attribute values (PTHREAD_CREATE_*, PTHREAD_*_SCHED, PTHREAD_SCOPE_*).
 * @Created: 2026/11/28 17:32:50 by Moutig
 * @Updated: 2026/11/28 17:34:49 by Moutig
 *
 * The values of the detachstate, inheritsched and contentionscope
 * attributes: int arguments of the pthread_attr_set*() functions, which
 * tell the two values of each attribute apart (XBD <pthread.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_PTHREAD_ATTR_H
# define BITS_PTHREAD_ATTR_H

# include "framework/lc.h"

# if defined(LC_REQ_PTHREAD_CREATE_JOINABLE) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_CREATE_JOINABLE__int, "PTHREAD_CREATE_JOINABLE", is_an_int_value,
	.ref = "XSH pthread_attr_getdetachstate()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_CREATE_JOINABLE), (long long)(PTHREAD_CREATE_JOINABLE));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_CREATE_JOINABLE__constant, "PTHREAD_CREATE_JOINABLE", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_CREATE_JOINABLE) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_CREATE_JOINABLE) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_CREATE_DETACHED) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_CREATE_DETACHED__int, "PTHREAD_CREATE_DETACHED", is_an_int_value,
	.ref = "XSH pthread_attr_getdetachstate()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_CREATE_DETACHED), (long long)(PTHREAD_CREATE_DETACHED));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_CREATE_DETACHED__constant, "PTHREAD_CREATE_DETACHED", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_CREATE_DETACHED) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_CREATE_DETACHED) != 0);
}
# endif

# if (defined(LC_REQ_PTHREAD_CREATE_JOINABLE) && defined(LC_REQ_PTHREAD_CREATE_DETACHED)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_CREATE_JOINABLE__joinable_and_detached_differ, "PTHREAD_CREATE_JOINABLE", joinable_and_detached_differ,
	.ref = "XSH pthread_attr_getdetachstate()")
{
	EXPECT_NE((long long)(PTHREAD_CREATE_JOINABLE), (long long)(PTHREAD_CREATE_DETACHED));
}
# endif

# if defined(LC_REQ_PTHREAD_INHERIT_SCHED) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_INHERIT_SCHED__int, "PTHREAD_INHERIT_SCHED", is_an_int_value,
	.ref = "XSH pthread_attr_getinheritsched()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_INHERIT_SCHED), (long long)(PTHREAD_INHERIT_SCHED));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_INHERIT_SCHED__constant, "PTHREAD_INHERIT_SCHED", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_INHERIT_SCHED) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_INHERIT_SCHED) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_EXPLICIT_SCHED) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_EXPLICIT_SCHED__int, "PTHREAD_EXPLICIT_SCHED", is_an_int_value,
	.ref = "XSH pthread_attr_getinheritsched()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_EXPLICIT_SCHED), (long long)(PTHREAD_EXPLICIT_SCHED));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_EXPLICIT_SCHED__constant, "PTHREAD_EXPLICIT_SCHED", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_EXPLICIT_SCHED) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_EXPLICIT_SCHED) != 0);
}
# endif

# if (defined(LC_REQ_PTHREAD_INHERIT_SCHED) && defined(LC_REQ_PTHREAD_EXPLICIT_SCHED)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_INHERIT_SCHED__inherit_and_explicit_differ, "PTHREAD_INHERIT_SCHED", inherit_and_explicit_differ,
	.ref = "XSH pthread_attr_getinheritsched()")
{
	EXPECT_NE((long long)(PTHREAD_INHERIT_SCHED), (long long)(PTHREAD_EXPLICIT_SCHED));
}
# endif

# if defined(LC_REQ_PTHREAD_SCOPE_SYSTEM) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_SCOPE_SYSTEM__int, "PTHREAD_SCOPE_SYSTEM", is_an_int_value,
	.ref = "XSH pthread_attr_getscope()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_SCOPE_SYSTEM), (long long)(PTHREAD_SCOPE_SYSTEM));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_SCOPE_SYSTEM__constant, "PTHREAD_SCOPE_SYSTEM", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_SCOPE_SYSTEM) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_SCOPE_SYSTEM) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_SCOPE_PROCESS) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_SCOPE_PROCESS__int, "PTHREAD_SCOPE_PROCESS", is_an_int_value,
	.ref = "XSH pthread_attr_getscope()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_SCOPE_PROCESS), (long long)(PTHREAD_SCOPE_PROCESS));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_SCOPE_PROCESS__constant, "PTHREAD_SCOPE_PROCESS", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_SCOPE_PROCESS) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_SCOPE_PROCESS) != 0);
}
# endif

# if (defined(LC_REQ_PTHREAD_SCOPE_SYSTEM) && defined(LC_REQ_PTHREAD_SCOPE_PROCESS)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_SCOPE_SYSTEM__system_and_process_differ, "PTHREAD_SCOPE_SYSTEM", system_and_process_differ,
	.ref = "XSH pthread_attr_getscope()")
{
	EXPECT_NE((long long)(PTHREAD_SCOPE_SYSTEM), (long long)(PTHREAD_SCOPE_PROCESS));
}
# endif

#endif
