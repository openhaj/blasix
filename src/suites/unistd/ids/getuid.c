/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file getuid.c
 * @brief Tests for getuid() (<unistd.h>).
 * @Created: 2026/11/28 10:37:00 by Moutig
 * @Updated: 2026/11/28 10:37:24 by Moutig
 *
 * getuid() returns the real user ID of the calling process; it always succeeds and no return value is
 * reserved to indicate an error. The reference is the raw syscall.
 *
 * Clauses: POSIX: XSH getuid().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH getuid()")
{
	EXPECT_TYPE(&getuid, uid_t (*)(void));
}

/* The value the kernel gives (the raw syscall). */
TEST_OPT(same_as_the_kernel, .ref = "XSH getuid()")
{
	EXPECT_EQ((long)getuid(), lc_sysGetuid());
}

/* Always successful: the same value on every call, errno never needed. */
TEST_OPT(always_successful_and_stable, .ref = "XSH getuid()")
{
	uid_t	first = getuid();
	int		bad = 0;

	for (int i = 0; i < 1000; i++)
		bad += getuid() != first;
	EXPECT_EQ(bad, 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ((long)getuid(), (long)(getuid)());
}
