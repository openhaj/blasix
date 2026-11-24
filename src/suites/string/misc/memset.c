/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memset.c
 * @brief Tests for memset() (<string.h>).
 * @Created: 2026/11/24 05:17:59 by Moutig
 * @Updated: 2026/11/24 05:46:22 by Moutig
 *
 * memset() copies c, converted to unsigned char, into the first n
 * characters of s and returns s.
 *
 * Clauses: C99 7.21.6.1; C11, C17 7.24.6.1; C23 7.26.6.1. POSIX: XSH
 * memset(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.6.1p1")
{
	EXPECT_TYPE(&memset, void *(*)(void *, int, size_t));
}

TEST_OPT(fills_n_bytes_and_returns_s, .ref = "C11 7.24.6.1p2-3")
{
	char	buf[6] = "abcde";

	EXPECT_PTREQ(memset(buf, 'z', 3), buf);
	EXPECT_STREQ(buf, "zzzde");
}

TEST_OPT(value_converted_to_unsigned_char, .ref = "C11 7.24.6.1p2")
{
	unsigned char	buf[3] = {0, 0, 0};

	(void)memset(buf, 0x141, 1);
	(void)memset(buf + 1, -1, 1);
	(void)memset(buf + 2, UCHAR_MAX + 1, 1);
	EXPECT_UEQ(buf[0], 0x41);
	EXPECT_UEQ(buf[1], UCHAR_MAX);
	EXPECT_UEQ(buf[2], 0);
}

TEST_OPT(zero_length_writes_nothing, .ref = "C11 7.24.1p2")
{
	char	buf[4] = "abc";

	EXPECT_PTREQ(memset(buf, 'z', 0), buf);
	EXPECT_STREQ(buf, "abc");
}

TEST_OPT(every_size_and_alignment, .ref = "C11 7.24.6.1p2")
{
	unsigned char	buf[96];
	int				bad = -1;

	for (int len = 0; len <= 64 && bad < 0; len++)
		for (int off = 0; off < 8 && bad < 0; off++)
		{
			lc_fill(buf, 0xee, sizeof(buf));
			(void)memset(buf + off, 0x5a, (size_t)len);
			for (int i = 0; i < 96 && bad < 0; i++)
				if (buf[i] != ((i >= off && i < off + len) ? 0x5a : 0xee))
					bad = len * 8 + off;
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[4] = "abc";
	char	b[4] = "abc";

	(void)memset(a, 'q', 2);
	(void)(memset)(b, 'q', 2);
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX memset()",
	char d[4];
	(void)memset(d, 0, sizeof(d));)
