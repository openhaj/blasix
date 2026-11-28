/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file mutex.h
 * @brief Cases of the mutex types and robustness values (PTHREAD_MUTEX_*).
 * @Created: 2026/11/28 17:34:18 by Moutig
 * @Updated: 2026/11/28 17:35:53 by Moutig
 *
 * The type attribute: NORMAL, ERRORCHECK and RECURSIVE behave differently,
 * so they differ; DEFAULT may be mapped to one of them. The robust
 * attribute: STALLED or ROBUST (2008). All are int arguments of
 * pthread_mutexattr_settype() and pthread_mutexattr_setrobust().
 * PTHREAD_MUTEX_INITIALIZER is in initializers.h.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_PTHREAD_MUTEX_H
# define BITS_PTHREAD_MUTEX_H

# include "framework/lc.h"

# if defined(LC_REQ_PTHREAD_MUTEX_NORMAL) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_MUTEX_NORMAL__int, "PTHREAD_MUTEX_NORMAL", is_an_int_value,
	.ref = "XSH pthread_mutexattr_gettype()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_MUTEX_NORMAL), (long long)(PTHREAD_MUTEX_NORMAL));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_MUTEX_NORMAL__constant, "PTHREAD_MUTEX_NORMAL", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_MUTEX_NORMAL) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_MUTEX_NORMAL) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_MUTEX_ERRORCHECK) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_MUTEX_ERRORCHECK__int, "PTHREAD_MUTEX_ERRORCHECK", is_an_int_value,
	.ref = "XSH pthread_mutexattr_gettype()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_MUTEX_ERRORCHECK), (long long)(PTHREAD_MUTEX_ERRORCHECK));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_MUTEX_ERRORCHECK__constant, "PTHREAD_MUTEX_ERRORCHECK", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_MUTEX_ERRORCHECK) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_MUTEX_ERRORCHECK) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_MUTEX_RECURSIVE) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_MUTEX_RECURSIVE__int, "PTHREAD_MUTEX_RECURSIVE", is_an_int_value,
	.ref = "XSH pthread_mutexattr_gettype()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_MUTEX_RECURSIVE), (long long)(PTHREAD_MUTEX_RECURSIVE));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_MUTEX_RECURSIVE__constant, "PTHREAD_MUTEX_RECURSIVE", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_MUTEX_RECURSIVE) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_MUTEX_RECURSIVE) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_MUTEX_DEFAULT) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_MUTEX_DEFAULT__int, "PTHREAD_MUTEX_DEFAULT", is_an_int_value,
	.ref = "XSH pthread_mutexattr_gettype()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_MUTEX_DEFAULT), (long long)(PTHREAD_MUTEX_DEFAULT));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_MUTEX_DEFAULT__constant, "PTHREAD_MUTEX_DEFAULT", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_MUTEX_DEFAULT) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_MUTEX_DEFAULT) != 0);
}
# endif

# if (defined(LC_REQ_PTHREAD_MUTEX_NORMAL) && defined(LC_REQ_PTHREAD_MUTEX_ERRORCHECK) && defined(LC_REQ_PTHREAD_MUTEX_RECURSIVE)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_MUTEX_NORMAL__normal_errorcheck_recursive_differ, "PTHREAD_MUTEX_NORMAL", normal_errorcheck_recursive_differ,
	.ref = "XSH pthread_mutexattr_gettype()")
{
	const long long	v[3] = {PTHREAD_MUTEX_NORMAL, PTHREAD_MUTEX_ERRORCHECK, PTHREAD_MUTEX_RECURSIVE};
	const int		n = 3;
	int				bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

# if defined(LC_REQ_PTHREAD_MUTEX_STALLED) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_MUTEX_STALLED__int, "PTHREAD_MUTEX_STALLED", is_an_int_value,
	.ref = "XSH pthread_mutexattr_getrobust()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_MUTEX_STALLED), (long long)(PTHREAD_MUTEX_STALLED));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_MUTEX_STALLED__constant, "PTHREAD_MUTEX_STALLED", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_MUTEX_STALLED) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_MUTEX_STALLED) != 0);
}
# endif

# if defined(LC_REQ_PTHREAD_MUTEX_ROBUST) || defined(LC_DEFS_ALL)
/* Passed as an int argument: the value survives the conversion to int. */
TEST_SYM(PTHREAD_MUTEX_ROBUST__int, "PTHREAD_MUTEX_ROBUST", is_an_int_value,
	.ref = "XSH pthread_mutexattr_getrobust()")
{
	EXPECT_EQ((long long)(int)(PTHREAD_MUTEX_ROBUST), (long long)(PTHREAD_MUTEX_ROBUST));
}

/* A symbolic constant: an integer constant expression (an enumerator value is one). */
TEST_SYM(PTHREAD_MUTEX_ROBUST__constant, "PTHREAD_MUTEX_ROBUST", is_an_integer_constant_expression,
	.since = "POSIX.1-2008", .ref = "XBD 3.363 Symbolic Constant")
{
	enum { lc_nonzero = (PTHREAD_MUTEX_ROBUST) != 0 };

	EXPECT_EQ(lc_nonzero, (PTHREAD_MUTEX_ROBUST) != 0);
}
# endif

# if (defined(LC_REQ_PTHREAD_MUTEX_STALLED) && defined(LC_REQ_PTHREAD_MUTEX_ROBUST)) || defined(LC_DEFS_ALL)
TEST_SYM(PTHREAD_MUTEX_STALLED__stalled_and_robust_differ, "PTHREAD_MUTEX_STALLED", stalled_and_robust_differ,
	.ref = "XSH pthread_mutexattr_getrobust()")
{
	EXPECT_NE((long long)(PTHREAD_MUTEX_STALLED), (long long)(PTHREAD_MUTEX_ROBUST));
}
# endif

#endif
