/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strstr.c
 * @brief Tests for strstr() (<string.h>).
 * @Created: 2026/11/24 05:24:51 by Moutig
 * @Updated: 2026/11/24 06:13:32 by Moutig
 *
 * strstr() locates the first occurrence of the string s2 (without its
 * terminating null character) in the string s1; an empty s2 gives s1, no
 * occurrence gives a null pointer.
 *
 * C23 makes it generic in the qualification of the searched array
 * (7.26.5.1): the result points to const when the argument does; the
 * function itself (macro suppressed) keeps the unqualified result type.
 *
 * Clauses: C99 7.21.5.7; C11, C17 7.24.5.7; C23 7.26.5.8. POSIX: XSH
 * strstr(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.5.7p1")
{
	EXPECT_TYPE(&strstr, char *(*)(const char *, const char *));
}

TEST_OPT(finds_first_occurrence, .ref = "C11 7.24.5.7p2-3")
{
	static const char	s[] = "abcabc";

	EXPECT_PTREQ(strstr(s, "bc"), s + 1);
	EXPECT_PTREQ(strstr(s, "abc"), s);
	EXPECT_PTREQ(strstr(s, "cabc"), s + 2);
}

TEST_OPT(empty_needle_returns_s1, .ref = "C11 7.24.5.7p3")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(strstr(s, ""), s);
	EXPECT_PTREQ(strstr(s + 3, ""), s + 3);
}

TEST_OPT(not_found_returns_null, .ref = "C11 7.24.5.7p3")
{
	EXPECT_NULL(strstr("abc", "abd"));
	EXPECT_NULL(strstr("ab", "abc"));
	EXPECT_NULL(strstr("", "a"));
}

/* Partial matches that overlap the real one. */
TEST_OPT(overlapping_partial_matches, .ref = "C11 7.24.5.7p2")
{
	static const char	s[] = "aaaaab";
	static const char	t[] = "abababac";

	EXPECT_PTREQ(strstr(s, "aab"), s + 3);
	EXPECT_PTREQ(strstr(t, "ababac"), t + 2);
}

TEST_OPT(match_at_the_end, .ref = "C11 7.24.5.7p2")
{
	static const char	s[] = "xxxxxxxxxxxxxxxxxxxxyz";

	EXPECT_PTREQ(strstr(s, "yz"), s + 20);
	EXPECT_NULL(strstr(s, "yzz"));
}

TEST_OPT(long_needle, .ref = "C11 7.24.5.7p2")
{
	char	hay[300];
	char	ndl[100];

	for (int i = 0; i < 299; i++)
		hay[i] = (char)('a' + i % 3);
	hay[299] = '\0';
	for (int i = 0; i < 99; i++)
		ndl[i] = (char)('a' + (i + 1) % 3);
	ndl[99] = '\0';
	EXPECT_PTREQ(strstr(hay, ndl), hay + 1);
	ndl[98] = 'z';
	EXPECT_NULL(strstr(hay, ndl));
}

TEST_OPT(const_preserving_in_c23, .since = "C23", .ref = "C23 7.26.5.1p1-2")
{
	static const char	cs[] = "abc";
	char				s[] = "abc";

	EXPECT_TYPE(strstr(cs, "b"), const char *);
	EXPECT_TYPE(strstr(s, "b"), char *);
	EXPECT_TYPE((strstr)(cs, "b"), char *);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	static const char	s[] = "abcbc";

	EXPECT_PTREQ(strstr(s, "bc"), (strstr)(s, "bc"));
}

ST_ERRNO_KEPT("POSIX strstr()",
	(void)strstr("abc", "bc");
	(void)strstr("abc", "z");)
