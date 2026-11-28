/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file usleep.c
 * @brief Tests for usleep() (<unistd.h>, XSI, POSIX.1-2001).
 * @Created: 2026/11/28 10:59:14 by Moutig
 * @Updated: 2026/11/28 11:00:09 by Moutig
 *
 * usleep(useconds) suspends the calling thread for at least useconds
 * microseconds (the argument shall be less than one million; 0 has no
 * effect) and returns 0; -1 and errno otherwise, EINVAL allowed for one
 * million or more. Obsolescent in POSIX.1-2001, removed in POSIX.1-2008:
 * the unit runs for POSIX.1-2001 + XSI only.
 *
 * The time is measured with the monotonic clock, raw (lc_now).
 *
 * Clauses: POSIX: XSH usleep() (POSIX.1-2001, OB XSI).
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH usleep()")
{
	EXPECT_TYPE(&usleep, int (*)(useconds_t));
}

/* 0, after at least the time asked. */
TEST_OPT(sleeps_at_least_the_time_asked, .ref = "XSH usleep()")
{
	static const useconds_t	us[] = {1, 1000, 20000, 150000};

	for (int i = 0; i < 4; i++)
	{
		long long	t0 = lc_now();

		EXPECT_EQ(usleep(us[i]), 0);
		EXPECT_GE(lc_now() - t0, (long long)us[i] * 1000);
	}
}

/* 0 has no effect: returns at once, with 0. */
TEST_OPT(zero_has_no_effect, .ref = "XSH usleep()")
{
	long long	t0 = lc_now();

	EXPECT_EQ(usleep(0), 0);
	EXPECT_LT(lc_now() - t0, 100LL * 1000 * 1000);
}

/* One million or more: 0 after sleeping, or -1 with EINVAL (allowed). */
TEST_OPT(one_million_is_einval_or_slept, .ref = "XSH usleep() ERRORS")
{
	long long	t0 = lc_now();
	int			r;

	errno = 0;
	r = usleep(1000000);
	if (r == -1)
	{
		EXPECT_EQ(errno, EINVAL);
	}
	else
	{
		EXPECT_EQ(r, 0);
		EXPECT_GE(lc_now() - t0, 1000LL * 1000 * 1000);
	}
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ((usleep)(10), 0);
}
