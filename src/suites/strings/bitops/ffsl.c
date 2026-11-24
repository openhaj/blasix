/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ffsl.c
 * @brief Tests for ffsl() (<strings.h>).
 * @Created: 2026/11/24 11:07:37 by Moutig
 * @Updated: 2026/11/24 11:11:03 by Moutig
 *
 * ffsl() finds the first bit set in i, starting from the least significant
 * bit, and returns its index; bits are numbered from 1 (the least
 * significant bit). It returns 0 for i = 0.
 *
 * POSIX: XSH ffs() (XSI, POSIX.1-2024).
 */
#include "strings/strings_cases.h"

TEST_OPT(prototype, .ref = "POSIX ffsl() SYNOPSIS")
{
	EXPECT_TYPE(&ffsl, int (*)(long));
}

TEST_OPT(zero_gives_zero, .ref = "POSIX ffsl() RETURN VALUE")
{
	EXPECT_EQ(ffsl(0), 0);
}

TEST_OPT(least_significant_bit_is_one, .ref = "POSIX ffsl() DESCRIPTION")
{
	EXPECT_EQ(ffsl(1), 1);
	EXPECT_EQ(ffsl(2), 2);
	EXPECT_EQ(ffsl(3), 1);
}

/* Every single bit, the sign bit included. */
TEST_OPT(every_bit_position, .ref = "POSIX ffsl() DESCRIPTION")
{
	int	width = (int)(sizeof(long) * CHAR_BIT);
	int	bad = -1;

	for (int k = 0; k < width && bad < 0; k++)
	{
		unsigned long	u = 1ul << k;
		long	v;

		lc_fill(&v, 0, sizeof(v));
		for (unsigned i = 0; i < sizeof(v); i++)
			((unsigned char *)&v)[i] = ((unsigned char *)&u)[i];
		if (ffsl(v) != k + 1)
			bad = k;
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(lowest_set_bit_wins, .ref = "POSIX ffsl() DESCRIPTION")
{
	EXPECT_EQ(ffsl(0x30), 5);
	EXPECT_EQ(ffsl(0x7f00), 9);
}

TEST_OPT(negative_values, .ref = "POSIX ffsl() DESCRIPTION")
{
	EXPECT_EQ(ffsl(-1), 1);
	EXPECT_EQ(ffsl(-2), 2);
	EXPECT_EQ(ffsl(LONG_MIN), (int)(sizeof(long) * CHAR_BIT));
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX ffsl() SYNOPSIS")
{
	EXPECT_EQ(ffsl(0x40), (ffsl)(0x40));
}
