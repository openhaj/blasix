/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strchr.c
 * @brief Tests for strchr() (<string.h>).
 * @Created: 2026/11/24 05:19:09 by Moutig
 * @Updated: 2026/11/24 05:51:02 by Moutig
 *
 * strchr() locates the first c (converted to char) in the string s; the
 * terminating null character is part of the string. Null pointer if c is
 * not found.
 *
 * C23 makes it generic in the qualification of the searched array
 * (7.26.5.1): the result points to const when the argument does; the
 * function itself (macro suppressed) keeps the unqualified result type.
 *
 * Clauses: C99 7.21.5.2; C11, C17 7.24.5.2; C23 7.26.5.3. POSIX: XSH
 * strchr(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.5.2p1")
{
	EXPECT_TYPE(&strchr, char *(*)(const char *, int));
}

TEST_OPT(finds_first_occurrence, .ref = "C11 7.24.5.2p2-3")
{
	static const char	s[] = "abcabc";

	EXPECT_PTREQ(strchr(s, 'c'), s + 2);
}

TEST_OPT(not_found_returns_null, .ref = "C11 7.24.5.2p3")
{
	EXPECT_NULL(strchr("abc", 'z'));
	EXPECT_NULL(strchr("", 'a'));
}

TEST_OPT(finds_terminating_nul, .ref = "C11 7.24.5.2p2")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(strchr(s, '\0'), s + 3);
	EXPECT_PTREQ(strchr(s + 3, '\0'), s + 3);
}

TEST_OPT(does_not_search_past_nul, .ref = "C11 7.24.5.2p2")
{
	EXPECT_NULL(strchr("ab\0cd", 'c'));
}

TEST_OPT(c_converted_to_char, .ref = "C11 7.24.5.2p2")
{
	static const char	s[] = "a\xe9" "b";

	EXPECT_PTREQ(strchr(s, 0xe9), s + 1);
	EXPECT_PTREQ(strchr(s, (char)0xe9), s + 1);
	EXPECT_PTREQ(strchr(s, 'b' + 256), s + 2);
}

TEST_OPT(every_length_and_position, .ref = "C11 7.24.5.2p2")
{
	char	buf[80];
	int		bad = -1;

	for (int off = 0; off < 8 && bad < 0; off++)
		for (int len = 0; len <= 64 && bad < 0; len++)
			for (int p = 0; p <= len && bad < 0; p++)
			{
				lc_fill(buf, 'x', sizeof(buf));
				buf[off + len] = '\0';
				if (p < len)
					buf[off + p] = 'Q';
				if (strchr(buf + off, 'Q') != (p < len ? buf + off + p : NULL))
					bad = (off * 65 + len) * 65 + p;
			}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(const_preserving_in_c23, .since = "C23", .ref = "C23 7.26.5.1p1-2")
{
	static const char	cs[] = "abc";
	char				s[] = "abc";

	EXPECT_TYPE(strchr(cs, 'b'), const char *);
	EXPECT_TYPE(strchr(s, 'b'), char *);
	EXPECT_TYPE((strchr)(cs, 'b'), char *);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(strchr(s, 'c'), (strchr)(s, 'c'));
}

ST_ERRNO_KEPT("POSIX strchr()",
	(void)strchr("abc", 'c');
	(void)strchr("abc", 'z');)
