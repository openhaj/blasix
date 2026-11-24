/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strncmp.c
 * @brief Tests for strncmp() (<string.h>).
 * @Created: 2026/11/24 05:23:26 by Moutig
 * @Updated: 2026/11/24 06:12:15 by Moutig
 *
 * strncmp() compares at most n characters of two arrays, not past a null
 * character; signs as strcmp() (unsigned char). n = 0 gives 0.
 *
 * Clauses: C99 7.21.4.4; C11, C17 7.24.4.4; C23 7.26.4.4. POSIX: XSH
 * strncmp(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.4.4p1")
{
	EXPECT_TYPE(&strncmp, int (*)(const char *, const char *, size_t));
}

TEST_OPT(compares_at_most_n, .ref = "C11 7.24.4.4p2")
{
	EXPECT_EQ(strncmp("abcx", "abcy", 3), 0);
	EXPECT_LT(strncmp("abcx", "abcy", 4), 0);
}

TEST_OPT(stops_at_nul, .ref = "C11 7.24.4.4p2")
{
	EXPECT_EQ(strncmp("ab\0x", "ab\0y", 4), 0);
	EXPECT_LT(strncmp("ab", "abc", 5), 0);
}

TEST_OPT(characters_compare_as_unsigned_char, .ref = "C11 7.24.4p1")
{
	EXPECT_GT(strncmp("\x80", "\x01", 1), 0);
}

TEST_OPT(zero_length_compares_zero, .ref = "C11 7.24.1p2")
{
	EXPECT_EQ(strncmp("a", "b", 0), 0);
}

/* The arrays need not be terminated within n characters. */
TEST_OPT(arrays_need_not_be_terminated, .ref = "C11 7.24.4.4p2")
{
	char	*a = lc_guarded(3);
	char	*b = lc_guarded(3);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	a[0] = b[0] = 'x';
	a[1] = b[1] = 'y';
	a[2] = 'z';
	b[2] = 'z';
	EXPECT_EQ(strncmp(a, b, 3), 0);
	b[2] = 'a';
	EXPECT_GT(strncmp(a, b, 3), 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(st_sign(strncmp("ab", "ac", 2)), st_sign((strncmp)("ab", "ac", 2)));
}

ST_ERRNO_KEPT("POSIX strncmp()",
	(void)strncmp("ab", "ac", 2);)
