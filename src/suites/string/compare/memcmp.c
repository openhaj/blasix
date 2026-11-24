/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memcmp.c
 * @brief Tests for memcmp() (<string.h>).
 * @Created: 2026/11/24 05:16:41 by Moutig
 * @Updated: 2026/11/24 05:42:26 by Moutig
 *
 * memcmp() compares the first n characters of two objects; the sign of a
 * nonzero result is the sign of the difference of the first differing
 * pair, both interpreted as unsigned char (C 7.24.4p1). n = 0 gives 0.
 *
 * Clauses: C99 7.21.4.1; C11, C17 7.24.4.1; C23 7.26.4.1. POSIX: XSH
 * memcmp(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.4.1p1")
{
	EXPECT_TYPE(&memcmp, int (*)(const void *, const void *, size_t));
}

TEST_OPT(equal_objects_compare_zero, .ref = "C11 7.24.4.1p3")
{
	EXPECT_EQ(memcmp("abc", "abc", 3), 0);
	EXPECT_EQ(memcmp("a\0b", "a\0b", 3), 0);
}

TEST_OPT(sign_of_first_difference, .ref = "C11 7.24.4.1p3")
{
	EXPECT_LT(memcmp("abc", "abd", 3), 0);
	EXPECT_GT(memcmp("abd", "abc", 3), 0);
	EXPECT_LT(memcmp("azz", "baa", 3), 0);
}

TEST_OPT(bytes_compare_as_unsigned_char, .ref = "C11 7.24.4p1")
{
	EXPECT_GT(memcmp("\x80", "\x01", 1), 0);
	EXPECT_GT(memcmp("\xff", "\x7f", 1), 0);
	EXPECT_LT(memcmp("\x00", "\xff", 1), 0);
}

/* NUL is an ordinary character: comparison goes past it. */
TEST_OPT(compares_past_nul, .ref = "C11 7.24.4.1p2")
{
	EXPECT_LT(memcmp("a\0b", "a\0c", 3), 0);
}

TEST_OPT(zero_length_compares_zero, .ref = "C11 7.24.1p2")
{
	EXPECT_EQ(memcmp("a", "b", 0), 0);
}

/* Every length and alignment, one difference at every position. */
TEST_OPT(every_size_and_position, .ref = "C11 7.24.4.1p2")
{
	unsigned char	a[80];
	unsigned char	b[80];
	int				bad = -1;

	for (int i = 0; i < 80; i++)
		a[i] = b[i] = (unsigned char)(i * 7 + 3);
	for (int len = 1; len <= 64 && bad < 0; len++)
		for (int off = 0; off < 8 && bad < 0; off++)
		{
			if (memcmp(a + off, b + off, (size_t)len) != 0)
				bad = len * 8 + off;
			for (int p = 0; p < len && bad < 0; p++)
			{
				b[off + p] = (unsigned char)(a[off + p] + 0x80);
				if (st_sign(memcmp(a + off, b + off, (size_t)len))
					!= (a[off + p] < b[off + p] ? -1 : 1))
					bad = len * 8 + off;
				b[off + p] = a[off + p];
			}
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(st_sign(memcmp("ab", "ac", 2)), st_sign((memcmp)("ab", "ac", 2)));
}

ST_ERRNO_KEPT("POSIX memcmp()",
	(void)memcmp("ab", "ac", 2);)
