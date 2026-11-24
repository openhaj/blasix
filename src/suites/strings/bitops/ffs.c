/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ffs.c
 * @brief Tests for ffs() (<strings.h>).
 * @Created: 2026/11/24 11:07:29 by Moutig
 * @Updated: 2026/11/24 11:10:22 by Moutig
 *
 * ffs() finds the first bit set in i, starting from the least significant
 * bit, and returns its index; bits are numbered from 1 (the least
 * significant bit). It returns 0 for i = 0.
 *
 * POSIX: XSH ffs() (XSI in every edition).
 */
#include "strings/strings_cases.h"

TEST_OPT(prototype, .ref = "POSIX ffs() SYNOPSIS")
{
	EXPECT_TYPE(&ffs, int (*)(int));
}

TEST_OPT(zero_gives_zero, .ref = "POSIX ffs() RETURN VALUE")
{
	EXPECT_EQ(ffs(0), 0);
}

TEST_OPT(least_significant_bit_is_one, .ref = "POSIX ffs() DESCRIPTION")
{
	EXPECT_EQ(ffs(1), 1);
	EXPECT_EQ(ffs(2), 2);
	EXPECT_EQ(ffs(3), 1);
}

/* Every single bit, the sign bit included. */
TEST_OPT(every_bit_position, .ref = "POSIX ffs() DESCRIPTION")
{
	int	width = (int)(sizeof(int) * CHAR_BIT);
	int	bad = -1;

	for (int k = 0; k < width && bad < 0; k++)
	{
		unsigned	u = 1u << k;
		int	v;

		lc_fill(&v, 0, sizeof(v));
		for (unsigned i = 0; i < sizeof(v); i++)
			((unsigned char *)&v)[i] = ((unsigned char *)&u)[i];
		if (ffs(v) != k + 1)
			bad = k;
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(lowest_set_bit_wins, .ref = "POSIX ffs() DESCRIPTION")
{
	EXPECT_EQ(ffs(0x30), 5);
	EXPECT_EQ(ffs(0x7f00), 9);
}

TEST_OPT(negative_values, .ref = "POSIX ffs() DESCRIPTION")
{
	EXPECT_EQ(ffs(-1), 1);
	EXPECT_EQ(ffs(-2), 2);
	EXPECT_EQ(ffs(INT_MIN), (int)(sizeof(int) * CHAR_BIT));
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX ffs() SYNOPSIS")
{
	EXPECT_EQ(ffs(0x40), (ffs)(0x40));
}
