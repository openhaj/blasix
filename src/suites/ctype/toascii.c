/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file toascii.c
 * @brief Tests for toascii() (<ctype.h>).
 * @Created: 2026/11/24 03:41:15 by Moutig
 * @Updated: 2026/11/24 03:54:55 by Moutig
 *
 * toascii() converts its argument into a 7-bit ASCII character: it
 * returns (c & 0x7f) (XSH toascii()). XSI in POSIX.1-2001, obsolescent
 * (OB XSI) in 2008 and 2017, removed in POSIX.1-2024.
 */
#include "framework/lc.h"
#include <ctype.h>
#include <limits.h>

TEST_OPT(prototype, .ref = "POSIX toascii() SYNOPSIS")
{
	EXPECT_TYPE(&toascii, int (*)(int));
}

TEST_OPT(low_seven_bits, .ref = "POSIX toascii() RETURN VALUE")
{
	static const int	big[] = {INT_MIN, INT_MIN + 1, -0x10000, 0x10000, INT_MAX - 1, INT_MAX};
	int					first_wrong_value = 0;
	int					wrong = 0;

	for (int c = -1024; c <= 1024 && !wrong; c++)
		if (toascii(c) != (c & 0x7f) || (toascii)(c) != (c & 0x7f))
		{
			first_wrong_value = c;
			wrong = 1;
		}
	for (unsigned i = 0; i < sizeof(big) / sizeof(big[0]) && !wrong; i++)
		if (toascii(big[i]) != (big[i] & 0x7f) || (toascii)(big[i]) != (big[i] & 0x7f))
		{
			first_wrong_value = big[i];
			wrong = 1;
		}
	EXPECT_EQ(wrong, 0);
	EXPECT_EQ(first_wrong_value, 0);
}
