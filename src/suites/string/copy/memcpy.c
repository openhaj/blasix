/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memcpy.c
 * @brief Tests for memcpy() (<string.h>).
 * @Created: 2026/11/24 05:17:11 by Moutig
 * @Updated: 2026/11/24 05:43:36 by Moutig
 *
 * memcpy() copies n characters from s2 to s1 and returns s1; overlapping
 * objects are undefined (not tested). Every character is taken as an
 * unsigned char, NUL included (C 7.24.1p3, C23 7.26.1p4); n may be 0.
 *
 * Clauses: C99 7.21.2.1; C11, C17 7.24.2.1; C23 7.26.2.1. POSIX: XSH
 * memcpy(); POSIX.1-2024 also forbids changing errno on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.2.1p1")
{
	EXPECT_TYPE(&memcpy, void *(*)(void *restrict, const void *restrict, size_t));
}

TEST_OPT(copies_n_bytes_and_returns_s1, .ref = "C11 7.24.2.1p2-3")
{
	char		dst[8] = "xxxxxxx";
	const char	src[5] = {'a', '\0', 'b', '\xff', 'c'};

	EXPECT_PTREQ(memcpy(dst, src, 5), dst);
	EXPECT_MEMEQ(dst, "a\0b\xff" "cxx", 8);
}

TEST_OPT(zero_length_copies_nothing, .ref = "C11 7.24.1p2")
{
	char	dst[4] = "abc";

	EXPECT_PTREQ(memcpy(dst, "xyz", 0), dst);
	EXPECT_STREQ(dst, "abc");
}

/* Every length 0 ... 64 at every alignment 0 ... 7, nothing written past. */
TEST_OPT(every_size_and_alignment, .ref = "C11 7.24.2.1p2")
{
	unsigned char	src[80];
	unsigned char	dst[96];
	int				bad = -1;

	for (int i = 0; i < 80; i++)
		src[i] = (unsigned char)(i * 37 + 1);
	for (int len = 0; len <= 64 && bad < 0; len++)
		for (int off = 0; off < 8 && bad < 0; off++)
		{
			lc_fill(dst, 0xee, sizeof(dst));
			if (memcpy(dst + off, src + (7 - off), (size_t)len) != dst + off)
				bad = len * 8 + off;
			for (int i = 0; i < 96 && bad < 0; i++)
			{
				int	in = i >= off && i < off + len;

				if (dst[i] != (in ? src[7 - off + i - off] : 0xee))
					bad = len * 8 + off;
			}
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[4] = "xxx";
	char	b[4] = "xxx";

	(void)memcpy(a, "ab", 2);
	(void)(memcpy)(b, "ab", 2);
	EXPECT_MEMEQ(a, b, 4);
}

ST_ERRNO_KEPT("POSIX memcpy()",
	char d[4];
	(void)memcpy(d, "abc", 4);)
