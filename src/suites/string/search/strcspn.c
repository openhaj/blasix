/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcspn.c
 * @brief Tests for strcspn() (<string.h>).
 * @Created: 2026/11/24 05:20:08 by Moutig
 * @Updated: 2026/11/24 05:55:43 by Moutig
 *
 * strcspn() returns the length of the initial segment of s1 made only of
 * characters not in the string s2.
 *
 * Clauses: C99 7.21.5.3; C11, C17 7.24.5.3; C23 7.26.5.4. POSIX: XSH
 * strcspn(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.5.3p1")
{
	EXPECT_TYPE(&strcspn, size_t (*)(const char *, const char *));
}

TEST_OPT(length_of_initial_segment, .ref = "C11 7.24.5.3p2-3")
{
	EXPECT_UEQ(strcspn("abcdef", "dx"), 3);
}

TEST_OPT(no_char_in_set_gives_length, .ref = "C11 7.24.5.3p3")
{
	EXPECT_UEQ(strcspn("abc", "xyz"), 3);
	EXPECT_UEQ(strcspn("abc", ""), 3);
}

TEST_OPT(first_char_in_set, .ref = "C11 7.24.5.3p3")
{
	EXPECT_UEQ(strcspn("abc", "a"), 0);
	EXPECT_UEQ(strcspn("", "a"), 0);
}

TEST_OPT(high_bytes, .ref = "C11 7.24.1p3")
{
	EXPECT_UEQ(strcspn("ab\xe9", "\xe9"), 2);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_UEQ(strcspn("abc", "c"), (strcspn)("abc", "c"));
}

ST_ERRNO_KEPT("POSIX strcspn()",
	(void)strcspn("abc", "c");)
