/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strdup.c
 * @brief Tests for strdup() (<string.h>).
 * @Created: 2026/11/24 05:20:15 by Moutig
 * @Updated: 2026/11/24 05:56:26 by Moutig
 *
 * strdup() returns a duplicate of the string s in space allocated as if
 * by malloc() (it can be passed to free()), or a null pointer if no space
 * can be allocated (not provoked: it would need to exhaust memory).
 *
 * Clauses: C23 7.26.2.6. POSIX: XSH strdup() (XSI in POSIX.1-2001, CX
 * from POSIX.1-2008).
 */
#include <stdlib.h>
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C23 7.26.2.6p1")
{
	EXPECT_TYPE(&strdup, char *(*)(const char *));
}

TEST_OPT(duplicates_the_string, .ref = "C23 7.26.2.6p2-3")
{
	const char	src[] = "hello";
	char		*d = strdup(src);

	ASSERT_NOTNULL(d);
	EXPECT_PTRNE(d, src);
	EXPECT_STREQ(d, "hello");
	free(d);
}

TEST_OPT(empty_string, .ref = "C23 7.26.2.6p2")
{
	char	*d = strdup("");

	ASSERT_NOTNULL(d);
	EXPECT_EQ(d[0], '\0');
	free(d);
}

/* The copy is independent of the source and writable. */
TEST_OPT(copy_is_independent, .ref = "C23 7.26.2.6p2")
{
	char	src[] = "abc";
	char	*d = strdup(src);

	ASSERT_NOTNULL(d);
	src[0] = 'z';
	d[1] = 'y';
	EXPECT_STREQ(d, "ayc");
	EXPECT_STREQ(src, "zbc");
	free(d);
}

TEST_OPT(long_string_with_high_bytes, .ref = "C23 7.26.2.6p2")
{
	char	src[300];
	char	*d;

	for (int i = 0; i < 299; i++)
		src[i] = (char)(1 + i % 255);
	src[299] = '\0';
	d = strdup(src);
	ASSERT_NOTNULL(d);
	EXPECT_MEMEQ(d, src, 300);
	free(d);
}

TEST_OPT(macro_and_function_agree, .ref = "C23 7.1.4p1")
{
	char	*a = strdup("ab");
	char	*b = (strdup)("ab");

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	EXPECT_STREQ(a, b);
	free(a);
	free(b);
}
