/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strspn.c
 * @brief Tests for strspn() (<string.h>).
 * @Created: 2026/11/24 05:24:43 by Moutig
 * @Updated: 2026/11/24 06:13:26 by Moutig
 *
 * strspn() returns the length of the initial segment of s1 made only of
 * characters of the string s2.
 *
 * Clauses: C99 7.21.5.6; C11, C17 7.24.5.6; C23 7.26.5.7. POSIX: XSH
 * strspn(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.5.6p1")
{
	EXPECT_TYPE(&strspn, size_t (*)(const char *, const char *));
}

TEST_OPT(length_of_initial_segment, .ref = "C11 7.24.5.6p2-3")
{
	EXPECT_UEQ(strspn("aabbcd", "ab"), 4);
	EXPECT_UEQ(strspn("abc", "cba"), 3);
}

TEST_OPT(first_char_not_in_set, .ref = "C11 7.24.5.6p3")
{
	EXPECT_UEQ(strspn("xab", "ab"), 0);
}

TEST_OPT(empty_strings, .ref = "C11 7.24.5.6p2")
{
	EXPECT_UEQ(strspn("abc", ""), 0);
	EXPECT_UEQ(strspn("", "abc"), 0);
}

TEST_OPT(high_bytes, .ref = "C11 7.24.1p3")
{
	EXPECT_UEQ(strspn("\xe9\xe9" "a", "\xe9"), 2);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_UEQ(strspn("aab", "a"), (strspn)("aab", "a"));
}

ST_ERRNO_KEPT("POSIX strspn()",
	(void)strspn("aab", "a");)
