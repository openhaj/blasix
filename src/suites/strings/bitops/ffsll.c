/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ffsll.c
 * @brief Tests for ffsll() (<strings.h>).
 * @Created: 2026/11/24 11:07:47 by Moutig
 * @Updated: 2026/11/24 11:11:34 by Moutig
 *
 * ffsll() finds the first bit set in i, starting from the least significant
 * bit, and returns its index; bits are numbered from 1 (the least
 * significant bit). It returns 0 for i = 0.
 *
 * POSIX: XSH ffs() (XSI, POSIX.1-2024).
 */
#include "strings/strings_cases.h"

TEST_OPT(prototype, .ref = "POSIX ffsll() SYNOPSIS")
{
	EXPECT_TYPE(&ffsll, int (*)(long long));
}

TEST_OPT(zero_gives_zero, .ref = "POSIX ffsll() RETURN VALUE")
{
	EXPECT_EQ(ffsll(0), 0);
}

TEST_OPT(least_significant_bit_is_one, .ref = "POSIX ffsll() DESCRIPTION")
{
	EXPECT_EQ(ffsll(1), 1);
	EXPECT_EQ(ffsll(2), 2);
	EXPECT_EQ(ffsll(3), 1);
}

/* Every single bit, the sign bit included. */
TEST_OPT(every_bit_position, .ref = "POSIX ffsll() DESCRIPTION")
{
	int	width = (int)(sizeof(long long) * CHAR_BIT);
	int	bad = -1;

	for (int k = 0; k < width && bad < 0; k++)
	{
		unsigned long long	u = 1ull << k;
		long long	v;

		lc_fill(&v, 0, sizeof(v));
		for (unsigned i = 0; i < sizeof(v); i++)
			((unsigned char *)&v)[i] = ((unsigned char *)&u)[i];
		if (ffsll(v) != k + 1)
			bad = k;
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(lowest_set_bit_wins, .ref = "POSIX ffsll() DESCRIPTION")
{
	EXPECT_EQ(ffsll(0x30), 5);
	EXPECT_EQ(ffsll(0x7f00), 9);
}

TEST_OPT(negative_values, .ref = "POSIX ffsll() DESCRIPTION")
{
	EXPECT_EQ(ffsll(-1), 1);
	EXPECT_EQ(ffsll(-2), 2);
	EXPECT_EQ(ffsll(LLONG_MIN), (int)(sizeof(long long) * CHAR_BIT));
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX ffsll() SYNOPSIS")
{
	EXPECT_EQ(ffsll(0x40), (ffsll)(0x40));
}
