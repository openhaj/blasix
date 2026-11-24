/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcat.c
 * @brief Tests for strcat() (<string.h>).
 * @Created: 2026/11/24 05:18:58 by Moutig
 * @Updated: 2026/11/24 05:50:21 by Moutig
 *
 * strcat() appends the string s2, terminating null character included,
 * to the end of the string s1; the first character of s2 overwrites the
 * null character of s1. It returns s1.
 *
 * Clauses: C99 7.21.3.1; C11, C17 7.24.3.1; C23 7.26.3.1. POSIX: XSH
 * strcat(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.3.1p1")
{
	EXPECT_TYPE(&strcat, char *(*)(char *restrict, const char *restrict));
}

TEST_OPT(appends_and_returns_s1, .ref = "C11 7.24.3.1p2-3")
{
	char	dst[10] = "ab\0xxxxxx";

	EXPECT_PTREQ(strcat(dst, "cde"), dst);
	EXPECT_MEMEQ(dst, "abcde\0xxx", 10);
}

TEST_OPT(empty_source_keeps_s1, .ref = "C11 7.24.3.1p2")
{
	char	dst[4] = "ab";

	(void)strcat(dst, "");
	EXPECT_STREQ(dst, "ab");
}

TEST_OPT(empty_destination, .ref = "C11 7.24.3.1p2")
{
	char	dst[6] = "\0xxxx";

	(void)strcat(dst, "abc");
	EXPECT_MEMEQ(dst, "abc\0x", 6);
}

TEST_OPT(every_size, .ref = "C11 7.24.3.1p2")
{
	char	src[40];
	char	dst[90];
	int		bad = -1;

	for (int a = 0; a <= 32 && bad < 0; a++)
		for (int b = 0; b <= 32 && bad < 0; b += 3)
		{
			lc_fill(dst, 'x', sizeof(dst));
			for (int i = 0; i < a; i++)
				dst[i] = 'p';
			dst[a] = '\0';
			for (int i = 0; i < b; i++)
				src[i] = 'q';
			src[b] = '\0';
			(void)strcat(dst, src);
			for (int i = 0; i < 90 && bad < 0; i++)
			{
				char	want = i < a ? 'p' : i < a + b ? 'q' : i == a + b ? '\0' : 'x';

				if (dst[i] != want)
					bad = a * 64 + b;
			}
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[6] = "ab";
	char	b[6] = "ab";

	(void)strcat(a, "cd");
	(void)(strcat)(b, "cd");
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX strcat()",
	char d[6] = "ab";
	(void)strcat(d, "cd");)
