/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strncat_s.c
 * @brief Tests for strncat_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:23:08 by Moutig
 * @Updated: 2026/11/24 06:10:54 by Moutig
 *
 * strncat_s() appends at most n characters of s2 (stopping at a null
 * character) to s1, always terminated, and returns 0; violations return
 * nonzero and set s1[0] to '\0' when s1 is usable.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.2.2 (EXAMPLE 1 is checked as given); C23
 * K.3.7.2.2.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.2.2p1")
{
	EXPECT_TYPE(&strncat_s, errno_t (*)(char *restrict, rsize_t, const char *restrict, rsize_t));
}

TEST_OPT(standard_example_appends, .ref = "C11 K.3.7.2.2p8")
{
	char	s1[100] = "good";
	char	s5[1000] = "bye";

	ST_EXT1_IGNORE();
	EXPECT_EQ(strncat_s(s1, 100, s5, 1000), 0);
	EXPECT_STREQ(s1, "goodbye");
}

TEST_OPT(standard_example_empty_source, .ref = "C11 K.3.7.2.2p8")
{
	char	s2[6] = "hello";

	ST_EXT1_IGNORE();
	EXPECT_EQ(strncat_s(s2, 6, "", 1), 0);
	EXPECT_STREQ(s2, "hello");
}

TEST_OPT(standard_example_no_room, .ref = "C11 K.3.7.2.2p8")
{
	char	s3[6] = "hello";

	ST_EXT1_IGNORE();
	EXPECT_NE(strncat_s(s3, 6, "X", 2), 0);
	EXPECT_EQ(s3[0], '\0');
}

TEST_OPT(standard_example_at_most_n, .ref = "C11 K.3.7.2.2p8")
{
	char	s4[7] = "abc";

	ST_EXT1_IGNORE();
	EXPECT_EQ(strncat_s(s4, 7, "defghijklmn", 3), 0);
	EXPECT_STREQ(s4, "abcdef");
}

BITS_LIB_EXT1_TESTS
