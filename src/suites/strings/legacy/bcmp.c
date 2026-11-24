/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file bcmp.c
 * @brief Tests for bcmp() (<strings.h>, POSIX.1-2001 LEGACY).
 * @Created: 2026/11/24 11:08:47 by Moutig
 * @Updated: 2026/11/24 11:13:25 by Moutig
 *
 * bcmp() compares the first n bytes of two areas: 0 if they are
 * identical, non-zero otherwise; 0 when n is 0.
 *
 * LEGACY (XSI) in POSIX.1-2001 and removed in POSIX.1-2008: this unit only
 * runs against a POSIX.1-2001 target with XSI.
 *
 * POSIX: XSH bcmp() (POSIX.1-2001).
 */
#include "strings/strings_cases.h"

TEST_OPT(prototype, .ref = "POSIX bcmp() SYNOPSIS")
{
	EXPECT_TYPE(&bcmp, int (*)(const void *, const void *, size_t));
}

TEST_OPT(identical_areas_give_zero, .ref = "POSIX bcmp() RETURN VALUE")
{
	EXPECT_EQ(bcmp("abc", "abc", 3), 0);
	EXPECT_EQ(bcmp("a\0b", "a\0b", 3), 0);
}

TEST_OPT(different_areas_give_nonzero, .ref = "POSIX bcmp() RETURN VALUE")
{
	EXPECT_NE(bcmp("abc", "abd", 3), 0);
	EXPECT_NE(bcmp("\x80", "\x01", 1), 0);
}

TEST_OPT(zero_length_gives_zero, .ref = "POSIX bcmp() RETURN VALUE")
{
	EXPECT_EQ(bcmp("a", "b", 0), 0);
}

TEST_OPT(compares_past_nul, .ref = "POSIX bcmp() DESCRIPTION")
{
	EXPECT_NE(bcmp("a\0b", "a\0c", 3), 0);
}

TEST_OPT(every_size_and_position, .ref = "POSIX bcmp() DESCRIPTION")
{
	unsigned char	a[72];
	unsigned char	b[72];
	int				bad = -1;

	for (int i = 0; i < 72; i++)
		a[i] = b[i] = (unsigned char)(i * 5 + 1);
	for (int len = 1; len <= 64 && bad < 0; len++)
	{
		if (bcmp(a, b, (size_t)len) != 0)
			bad = len;
		for (int p = 0; p < len && bad < 0; p++)
		{
			b[p] ^= 0x40;
			if (bcmp(a, b, (size_t)len) == 0)
				bad = len * 64 + p;
			b[p] ^= 0x40;
		}
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX bcmp() SYNOPSIS")
{
	EXPECT_EQ(bcmp("ab", "ab", 2), (bcmp)("ab", "ab", 2));
}
