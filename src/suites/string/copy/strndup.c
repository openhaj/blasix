/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strndup.c
 * @brief Tests for strndup() (<string.h>).
 * @Created: 2026/11/24 05:23:53 by Moutig
 * @Updated: 2026/11/24 06:12:50 by Moutig
 *
 * strndup() returns a new string, allocated as if by malloc(), holding at
 * most n characters of the array s up to its first null character, and
 * always terminated.
 *
 * Clauses: C23 7.26.2.7 (s is an array: it need not be terminated). POSIX:
 * XSH strdup() (strndup, CX, POSIX.1-2008 on); POSIX.1-2024: it "shall
 * not examine more than size bytes".
 */
#include <stdlib.h>
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C23 7.26.2.7p1")
{
	EXPECT_TYPE(&strndup, char *(*)(const char *, size_t));
}

TEST_OPT(truncates_to_n_and_terminates, .ref = "C23 7.26.2.7p2")
{
	char	*d = strndup("abcdef", 3);

	ASSERT_NOTNULL(d);
	EXPECT_STREQ(d, "abc");
	free(d);
}

TEST_OPT(short_string_copied_whole, .ref = "C23 7.26.2.7p2")
{
	char	*d = strndup("ab", 10);

	ASSERT_NOTNULL(d);
	EXPECT_STREQ(d, "ab");
	free(d);
}

TEST_OPT(zero_length_gives_empty_string, .ref = "C23 7.26.2.7p2")
{
	char	*d = strndup("abc", 0);

	ASSERT_NOTNULL(d);
	EXPECT_EQ(d[0], '\0');
	free(d);
}

TEST_OPT(stops_at_first_nul, .ref = "C23 7.26.2.7p2")
{
	char	*d = strndup("a\0bc", 4);

	ASSERT_NOTNULL(d);
	EXPECT_STREQ(d, "a");
	free(d);
}

/* An array with no null character in its n characters (C23). */
TEST_OPT(unterminated_array_c23, .since = "C23", .ref = "C23 7.26.2.7p2")
{
	char	*src = lc_guarded(3);
	char	*d;

	ASSERT_NOTNULL(src);
	src[0] = 'x';
	src[1] = 'y';
	src[2] = 'z';
	d = strndup(src, 3);
	ASSERT_NOTNULL(d);
	EXPECT_STREQ(d, "xyz");
	free(d);
}

TEST_OPT(never_examines_more_than_size, .since = "POSIX.1-2024",
	.ref = "POSIX strndup() DESCRIPTION (Issue 8)")
{
	char	*src = lc_guarded(3);
	char	*d;

	ASSERT_NOTNULL(src);
	src[0] = 'x';
	src[1] = 'y';
	src[2] = 'z';
	d = strndup(src, 3);
	ASSERT_NOTNULL(d);
	EXPECT_STREQ(d, "xyz");
	free(d);
}

TEST_OPT(macro_and_function_agree, .ref = "C23 7.1.4p1")
{
	char	*a = strndup("abc", 2);
	char	*b = (strndup)("abc", 2);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	EXPECT_STREQ(a, b);
	free(a);
	free(b);
}
