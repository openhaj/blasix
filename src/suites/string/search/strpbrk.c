/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strpbrk.c
 * @brief Tests for strpbrk() (<string.h>).
 * @Created: 2026/11/24 05:24:18 by Moutig
 * @Updated: 2026/11/24 06:13:14 by Moutig
 *
 * strpbrk() locates the first character of s1 that is in the string s2,
 * or returns a null pointer.
 *
 * C23 makes it generic in the qualification of the searched array
 * (7.26.5.1): the result points to const when the argument does; the
 * function itself (macro suppressed) keeps the unqualified result type.
 *
 * Clauses: C99 7.21.5.4; C11, C17 7.24.5.4; C23 7.26.5.5. POSIX: XSH
 * strpbrk(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.5.4p1")
{
	EXPECT_TYPE(&strpbrk, char *(*)(const char *, const char *));
}

TEST_OPT(finds_first_from_set, .ref = "C11 7.24.5.4p2-3")
{
	static const char	s[] = "hello, world";

	EXPECT_PTREQ(strpbrk(s, ", "), s + 5);
	EXPECT_PTREQ(strpbrk(s, "ow"), s + 4);
}

TEST_OPT(none_from_set_returns_null, .ref = "C11 7.24.5.4p3")
{
	EXPECT_NULL(strpbrk("abc", "xyz"));
	EXPECT_NULL(strpbrk("", "abc"));
}

TEST_OPT(empty_set_returns_null, .ref = "C11 7.24.5.4p2")
{
	EXPECT_NULL(strpbrk("abc", ""));
}

/* The terminating null character of s2 is not part of the set. */
TEST_OPT(nul_not_in_set, .ref = "C11 7.24.5.4p2")
{
	EXPECT_NULL(strpbrk("abc", "x"));
}

TEST_OPT(high_bytes_in_set, .ref = "C11 7.24.1p3")
{
	static const char	s[] = "ab\xe9" "c";

	EXPECT_PTREQ(strpbrk(s, "\xe9"), s + 2);
}

TEST_OPT(const_preserving_in_c23, .since = "C23", .ref = "C23 7.26.5.1p1-2")
{
	static const char	cs[] = "abc";
	char				s[] = "abc";

	EXPECT_TYPE(strpbrk(cs, "b"), const char *);
	EXPECT_TYPE(strpbrk(s, "b"), char *);
	EXPECT_TYPE((strpbrk)(cs, "b"), char *);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(strpbrk(s, "cb"), (strpbrk)(s, "cb"));
}

ST_ERRNO_KEPT("POSIX strpbrk()",
	(void)strpbrk("abc", "c");
	(void)strpbrk("abc", "z");)
