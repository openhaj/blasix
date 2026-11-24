/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcpy.c
 * @brief Tests for strcpy() (<string.h>).
 * @Created: 2026/11/24 05:20:00 by Moutig
 * @Updated: 2026/11/24 05:55:10 by Moutig
 *
 * strcpy() copies the string s2, terminating null character included,
 * into s1 and returns s1.
 *
 * Clauses: C99 7.21.2.3; C11, C17 7.24.2.3; C23 7.26.2.4. POSIX: XSH
 * strcpy(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.2.3p1")
{
	EXPECT_TYPE(&strcpy, char *(*)(char *restrict, const char *restrict));
}

TEST_OPT(copies_string_and_nul, .ref = "C11 7.24.2.3p2-3")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_PTREQ(strcpy(dst, "abc"), dst);
	EXPECT_MEMEQ(dst, "abc\0xxx", 8);
}

TEST_OPT(empty_string, .ref = "C11 7.24.2.3p2")
{
	char	dst[4] = "xxx";

	EXPECT_PTREQ(strcpy(dst, ""), dst);
	EXPECT_MEMEQ(dst, "\0xx", 4);
}

TEST_OPT(stops_at_first_nul, .ref = "C11 7.24.2.3p2")
{
	char	dst[6] = "xxxxx";

	(void)strcpy(dst, "a\0bc");
	EXPECT_MEMEQ(dst, "a\0xxx", 6);
}

TEST_OPT(every_size_and_alignment, .ref = "C11 7.24.2.3p2")
{
	char	src[80];
	char	dst[96];
	int		bad = -1;

	for (int len = 0; len <= 64 && bad < 0; len++)
		for (int off = 0; off < 8 && bad < 0; off++)
		{
			for (int i = 0; i < len; i++)
				src[off + i] = (char)('a' + i % 26);
			src[off + len] = '\0';
			lc_fill(dst, 'x', sizeof(dst));
			if (strcpy(dst + (7 - off), src + off) != dst + (7 - off))
				bad = len * 8 + off;
			for (int i = 0; i < 96 && bad < 0; i++)
			{
				int		j = i - (7 - off);
				char	want = (j < 0 || j > len) ? 'x' : (j == len ? '\0' : src[off + j]);

				if (dst[i] != want)
					bad = len * 8 + off;
			}
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[4];
	char	b[4];

	(void)strcpy(a, "ab");
	(void)(strcpy)(b, "ab");
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX strcpy()",
	char d[4];
	(void)strcpy(d, "abc");)
