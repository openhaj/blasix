/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file bzero.c
 * @brief Tests for bzero() (<strings.h>, POSIX.1-2001 LEGACY).
 * @Created: 2026/11/24 11:09:07 by Moutig
 * @Updated: 2026/11/24 11:13:48 by Moutig
 *
 * bzero() places n zero-valued bytes in the area s. It returns no value.
 *
 * LEGACY (XSI) in POSIX.1-2001 and removed in POSIX.1-2008: this unit only
 * runs against a POSIX.1-2001 target with XSI.
 *
 * POSIX: XSH bzero() (POSIX.1-2001).
 */
#include "strings/strings_cases.h"

TEST_OPT(prototype, .ref = "POSIX bzero() SYNOPSIS")
{
	EXPECT_TYPE(&bzero, void (*)(void *, size_t));
}

TEST_OPT(zeroes_n_bytes, .ref = "POSIX bzero() DESCRIPTION")
{
	char	buf[6] = "abcde";

	bzero(buf, 3);
	EXPECT_MEMEQ(buf, "\0\0\0de", 6);
}

TEST_OPT(zero_length_writes_nothing, .ref = "POSIX bzero() DESCRIPTION")
{
	char	buf[4] = "abc";

	bzero(buf, 0);
	EXPECT_STREQ(buf, "abc");
}

TEST_OPT(every_size_and_alignment, .ref = "POSIX bzero() DESCRIPTION")
{
	unsigned char	buf[80];
	int				bad = -1;

	for (int len = 0; len <= 64 && bad < 0; len++)
		for (int off = 0; off < 8 && bad < 0; off++)
		{
			lc_fill(buf, 0xee, sizeof(buf));
			bzero(buf + off, (size_t)len);
			for (int i = 0; i < 80 && bad < 0; i++)
				if (buf[i] != ((i >= off && i < off + len) ? 0 : 0xee))
					bad = len * 8 + off;
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX bzero() SYNOPSIS")
{
	char	a[4] = "abc";
	char	b[4] = "abc";

	bzero(a, 2);
	(bzero)(b, 2);
	EXPECT_MEMEQ(a, b, 4);
}
