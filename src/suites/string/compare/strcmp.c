/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcmp.c
 * @brief Tests for strcmp() (<string.h>).
 * @Created: 2026/11/24 05:19:24 by Moutig
 * @Updated: 2026/11/24 05:51:34 by Moutig
 *
 * strcmp() compares two strings; the sign of a nonzero result is that of
 * the first differing pair of characters, interpreted as unsigned char.
 *
 * Clauses: C99 7.21.4.2; C11, C17 7.24.4.2; C23 7.26.4.2. POSIX: XSH
 * strcmp(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.4.2p1")
{
	EXPECT_TYPE(&strcmp, int (*)(const char *, const char *));
}

TEST_OPT(equal_strings_compare_zero, .ref = "C11 7.24.4.2p3")
{
	EXPECT_EQ(strcmp("abc", "abc"), 0);
	EXPECT_EQ(strcmp("", ""), 0);
}

TEST_OPT(sign_of_first_difference, .ref = "C11 7.24.4.2p3")
{
	EXPECT_LT(strcmp("abc", "abd"), 0);
	EXPECT_GT(strcmp("b", "abc"), 0);
}

TEST_OPT(prefix_is_smaller, .ref = "C11 7.24.4.2p2")
{
	EXPECT_LT(strcmp("ab", "abc"), 0);
	EXPECT_GT(strcmp("abc", "ab"), 0);
	EXPECT_LT(strcmp("", "a"), 0);
}

TEST_OPT(characters_compare_as_unsigned_char, .ref = "C11 7.24.4p1")
{
	EXPECT_GT(strcmp("\x80", "\x01"), 0);
	EXPECT_GT(strcmp("a\xff", "a\x7f"), 0);
}

TEST_OPT(stops_at_nul, .ref = "C11 7.24.4.2p2")
{
	EXPECT_EQ(strcmp("a\0x", "a\0y"), 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(st_sign(strcmp("ab", "ac")), st_sign((strcmp)("ab", "ac")));
}

ST_ERRNO_KEPT("POSIX strcmp()",
	(void)strcmp("ab", "ac");)
