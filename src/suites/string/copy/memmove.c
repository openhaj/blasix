/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memmove.c
 * @brief Tests for memmove() (<string.h>).
 * @Created: 2026/11/24 05:17:36 by Moutig
 * @Updated: 2026/11/24 05:44:45 by Moutig
 *
 * memmove() copies n characters as if through a temporary array: the
 * objects may overlap, in either direction. It returns s1.
 *
 * Clauses: C99 7.21.2.2; C11, C17 7.24.2.2; C23 7.26.2.3. POSIX: XSH
 * memmove(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.2.2p1")
{
	EXPECT_TYPE(&memmove, void *(*)(void *, const void *, size_t));
}

TEST_OPT(copies_and_returns_s1, .ref = "C11 7.24.2.2p2-3")
{
	char	dst[6] = "xxxxx";

	EXPECT_PTREQ(memmove(dst, "a\0b", 3), dst);
	EXPECT_MEMEQ(dst, "a\0bxx", 6);
}

TEST_OPT(overlap_destination_after_source, .ref = "C11 7.24.2.2p2")
{
	char	buf[11] = "0123456789";

	(void)memmove(buf + 2, buf, 6);
	EXPECT_STREQ(buf, "0101234589");
}

TEST_OPT(overlap_destination_before_source, .ref = "C11 7.24.2.2p2")
{
	char	buf[11] = "0123456789";

	(void)memmove(buf, buf + 3, 6);
	EXPECT_STREQ(buf, "3456786789");
}

TEST_OPT(zero_length_copies_nothing, .ref = "C11 7.24.1p2")
{
	char	buf[4] = "abc";

	EXPECT_PTREQ(memmove(buf, buf + 1, 0), buf);
	EXPECT_STREQ(buf, "abc");
}

/* Every length and distance, both directions, within one buffer. */
TEST_OPT(every_overlap, .ref = "C11 7.24.2.2p2")
{
	unsigned char	buf[96];
	unsigned char	want[96];
	int				bad = -1;

	for (int len = 0; len <= 40 && bad < 0; len++)
		for (int src = 0; src < 24 && bad < 0; src++)
			for (int dst = 0; dst < 24 && bad < 0; dst++)
			{
				for (int i = 0; i < 96; i++)
					buf[i] = want[i] = (unsigned char)(i + 1);
				for (int i = 0; i < len; i++)
					want[dst + i] = (unsigned char)(src + i + 1);
				(void)memmove(buf + dst, buf + src, (size_t)len);
				for (int i = 0; i < 96 && bad < 0; i++)
					if (buf[i] != want[i])
						bad = (len * 24 + src) * 24 + dst;
			}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[6] = "abcde";
	char	b[6] = "abcde";

	(void)memmove(a + 1, a, 3);
	(void)(memmove)(b + 1, b, 3);
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX memmove()",
	char d[6] = "abcde";
	(void)memmove(d + 1, d, 3);)
