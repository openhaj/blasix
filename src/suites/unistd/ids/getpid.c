/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file getpid.c
 * @brief Tests for getpid() (<unistd.h>).
 * @Created: 2026/11/28 10:36:15 by Moutig
 * @Updated: 2026/11/28 10:36:34 by Moutig
 *
 * getpid() returns the process ID of the calling process; it always succeeds and no return value is
 * reserved to indicate an error. The reference is the raw syscall.
 *
 * Clauses: POSIX: XSH getpid().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH getpid()")
{
	EXPECT_TYPE(&getpid, pid_t (*)(void));
}

/* The value the kernel gives (the raw syscall). */
TEST_OPT(same_as_the_kernel, .ref = "XSH getpid()")
{
	EXPECT_EQ((long)getpid(), lc_sysGetpid());
}

/* Always successful: the same value on every call, errno never needed. */
TEST_OPT(always_successful_and_stable, .ref = "XSH getpid()")
{
	pid_t	first = getpid();
	int		bad = 0;

	for (int i = 0; i < 1000; i++)
		bad += getpid() != first;
	EXPECT_EQ(bad, 0);
}

/* In the child of fork(): the child's own process ID, not the parent's. */
TEST_OPT(child_of_fork_gets_its_own, .ref = "XSH getpid()")
{
	long	*seen = lc_sysMmapShared(2 * sizeof(long));
	pid_t	r;

	ASSERT_NOTNULL(seen);
	r = fork();
	if (r == 0)
	{
		seen[0] = (long)getpid();
		seen[1] = lc_sysGetpid();
		lc_sysExit(0);
	}
	ASSERT_TRUE(r > 0);
	(void)lc_sysWait4(r, 0, 0);
	EXPECT_EQ(seen[0], seen[1]);
	EXPECT_EQ(seen[0], (long)r);
	EXPECT_NE(seen[0], (long)getpid());
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ((long)getpid(), (long)(getpid)());
}
