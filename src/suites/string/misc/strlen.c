/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strlen.c
 * @brief Tests for strlen() (<string.h>).
 * @Created: 2026/11/24 05:23:00 by Moutig
 * @Updated: 2026/11/24 06:10:40 by Moutig
 *
 * strlen() returns the number of characters before the terminating null
 * character of s.
 *
 * Clauses: C99 7.21.6.3; C11, C17 7.24.6.3; C23 7.26.6.5. POSIX: XSH
 * strlen(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.6.3p1")
{
	EXPECT_TYPE(&strlen, size_t (*)(const char *));
}

TEST_OPT(nominal, .ref = "C11 7.24.6.3p2-3")
{
	EXPECT_UEQ(strlen("abc"), 3);
	EXPECT_UEQ(strlen("a"), 1);
}

TEST_OPT(empty_string_is_zero, .ref = "C11 7.24.6.3p3")
{
	EXPECT_UEQ(strlen(""), 0);
}

TEST_OPT(stops_at_first_nul, .ref = "C11 7.24.6.3p3")
{
	EXPECT_UEQ(strlen("ab\0cd"), 2);
}

TEST_OPT(high_bytes_are_counted, .ref = "C11 7.24.1p3")
{
	EXPECT_UEQ(strlen("\x80\xff\xe9"), 3);
}

/* Every length and alignment, the null character on the last byte of a page. */
TEST_OPT(string_ending_at_page_end, .ref = "C11 7.24.6.3p2")
{
	int	bad = -1;

	for (int len = 0; len <= 80 && bad < 0; len++)
	{
		char	*s = lc_guarded((lc_size_t)len + 1);

		if (!s)
			SKIP("no guarded mapping");
		for (int i = 0; i < len; i++)
			s[i] = 'a';
		s[len] = '\0';
		if (strlen(s) != (size_t)len)
			bad = len;
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(every_size_and_alignment, .ref = "C11 7.24.6.3p2")
{
	char	buf[96];
	int		bad = -1;

	for (int off = 0; off < 8 && bad < 0; off++)
		for (int len = 0; len <= 80 && bad < 0; len++)
		{
			lc_fill(buf, 'x', sizeof(buf));
			buf[off + len] = '\0';
			if (strlen(buf + off) != (size_t)len)
				bad = off * 81 + len;
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_UEQ(strlen("abc"), (strlen)("abc"));
}

ST_ERRNO_KEPT("POSIX strlen()",
	(void)strlen("abc");)
