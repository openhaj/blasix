/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strnlen_s.c
 * @brief Tests for strnlen_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:24:02 by Moutig
 * @Updated: 2026/11/24 06:13:03 by Moutig
 *
 * strnlen_s() returns 0 for a null pointer, otherwise the length of the
 * string, or maxsize when there is no null character in the first maxsize
 * characters; at most maxsize characters are accessed. It has no
 * runtime-constraint.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.4.4; C23 K.3.7.4.4.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.4.4p1")
{
	EXPECT_TYPE(&strnlen_s, size_t (*)(const char *, size_t));
}

TEST_OPT(length_when_shorter, .ref = "C11 K.3.7.4.4p4")
{
	EXPECT_UEQ(strnlen_s("abc", 10), 3);
	EXPECT_UEQ(strnlen_s("", 10), 0);
}

TEST_OPT(maxsize_when_longer, .ref = "C11 K.3.7.4.4p4")
{
	EXPECT_UEQ(strnlen_s("abcdef", 3), 3);
	EXPECT_UEQ(strnlen_s("abc", 0), 0);
}

TEST_OPT(null_pointer_is_zero, .ref = "C11 K.3.7.4.4p3")
{
	EXPECT_UEQ(strnlen_s(NULL, 10), 0);
}

TEST_OPT(accesses_at_most_maxsize, .ref = "C11 K.3.7.4.4p4")
{
	char	*s = lc_guarded(4);

	ASSERT_NOTNULL(s);
	s[0] = 'a';
	s[1] = 'b';
	s[2] = 'c';
	s[3] = 'd';
	EXPECT_UEQ(strnlen_s(s, 4), 4);
}

BITS_LIB_EXT1_TESTS
