/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isascii.c
 * @brief Tests for isascii() (<ctype.h>).
 * @Created: 2026/11/24 03:36:04 by Moutig
 * @Updated: 2026/11/24 03:52:11 by Moutig
 *
 * isascii() tests whether c is a 7-bit US-ASCII code, 0 ... 0177, and is
 * defined on every int value (XSH isascii()). XSI in POSIX.1-2001,
 * obsolescent (OB XSI) in 2008 and 2017, removed in POSIX.1-2024.
 */
#include "framework/lc.h"
#include <ctype.h>
#include <limits.h>

TEST_OPT(prototype, .ref = "POSIX isascii() SYNOPSIS")
{
	EXPECT_TYPE(&isascii, int (*)(int));
}

TEST_OPT(codes_0_to_0177, .ref = "POSIX isascii() RETURN VALUE")
{
	int	first_wrong_value = -1;

	for (int c = 0; c <= 0177 && first_wrong_value < 0; c++)
		if (!isascii(c) || !(isascii)(c))
			first_wrong_value = c;
	EXPECT_EQ(first_wrong_value, -1);
}

TEST_OPT(every_other_int, .ref = "POSIX isascii() DESCRIPTION: defined on all integer values")
{
	static const int	big[] = {INT_MIN, INT_MIN + 1, -0x10000, 0x10000, INT_MAX - 1, INT_MAX};
	int					first_wrong_value = 0;

	for (int c = -1024; c <= 1024 && first_wrong_value == 0; c++)
		if (c < 0 || c > 0177)
			if (isascii(c) || (isascii)(c))
				first_wrong_value = c;
	for (unsigned i = 0; i < sizeof(big) / sizeof(big[0]); i++)
		if (isascii(big[i]) || (isascii)(big[i]))
			first_wrong_value = big[i];
	EXPECT_EQ(first_wrong_value, 0);
}
