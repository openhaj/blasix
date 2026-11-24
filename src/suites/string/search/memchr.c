/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memchr.c
 * @brief Tests for memchr() (<string.h>).
 * @Created: 2026/11/24 05:16:26 by Moutig
 * @Updated: 2026/11/24 05:41:43 by Moutig
 *
 * memchr() locates the first c (converted to unsigned char) in the first
 * n characters of s, or returns a null pointer; n = 0 finds nothing.
 *
 * C23 makes it generic in the qualification of the searched array
 * (7.26.5.1): the result points to const when the argument does; the
 * function itself (macro suppressed) keeps the unqualified result type.
 *
 * Clauses: C99 7.21.5.1; C11, C17 7.24.5.1; C23 7.26.5.2. POSIX: XSH
 * memchr(); POSIX.1-2017 on: it behaves as if reading byte by byte and
 * stopping at the match (n may exceed the object then); POSIX.1-2024:
 * errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.5.1p1")
{
	EXPECT_TYPE(&memchr, void *(*)(const void *, int, size_t));
}

TEST_OPT(finds_first_occurrence, .ref = "C11 7.24.5.1p2-3")
{
	static const char	s[] = "abcabc";

	EXPECT_PTREQ(memchr(s, 'b', 6), s + 1);
	EXPECT_PTREQ(memchr(s, 'a', 6), s);
}

TEST_OPT(not_found_returns_null, .ref = "C11 7.24.5.1p3")
{
	static const char	s[] = "abcdef";

	EXPECT_NULL(memchr(s, 'z', 6));
	EXPECT_NULL(memchr(s, 'e', 4));
}

TEST_OPT(zero_length_finds_nothing, .ref = "C11 7.24.1p2")
{
	EXPECT_NULL(memchr("abc", 'a', 0));
}

TEST_OPT(c_converted_to_unsigned_char, .ref = "C11 7.24.5.1p2")
{
	static const char	s[] = "a\xe9" "b\xff";

	EXPECT_PTREQ(memchr(s, 'b' + 256, 4), s + 2);
	EXPECT_PTREQ(memchr(s, 0xe9, 4), s + 1);
	EXPECT_PTREQ(memchr(s, -1, 4), s + 3);
}

TEST_OPT(nul_is_an_ordinary_byte, .ref = "C11 7.24.5.1p2")
{
	static const char	s[5] = {'a', '\0', 'b', '\0', 'c'};

	EXPECT_PTREQ(memchr(s, 'c', 5), s + 4);
	EXPECT_PTREQ(memchr(s, '\0', 5), s + 1);
}

TEST_OPT(every_size_and_position, .ref = "C11 7.24.5.1p2")
{
	unsigned char	buf[80];
	int				bad = -1;

	lc_fill(buf, 'x', sizeof(buf));
	for (int off = 0; off < 8 && bad < 0; off++)
		for (int len = 0; len <= 64 && bad < 0; len++)
			for (int p = 0; p <= len && bad < 0; p++)
			{
				if (p < len)
					buf[off + p] = 'Q';
				if (memchr(buf + off, 'Q', (size_t)len) != (p < len ? buf + off + p : NULL))
					bad = (off * 65 + len) * 65 + p;
				buf[off + p] = 'x';
			}
	EXPECT_EQ(bad, -1);
}

/* The match is before the end of the object, n goes past it. */
TEST_OPT(stops_at_the_match, .since = "POSIX.1-2017", .ref = "POSIX memchr() DESCRIPTION")
{
	char	*s = lc_guarded(4);

	ASSERT_NOTNULL(s);
	s[0] = 'a';
	s[1] = 'b';
	s[2] = 'c';
	s[3] = 'd';
	EXPECT_PTREQ(memchr(s, 'd', 1000), s + 3);
}

TEST_OPT(const_preserving_in_c23, .since = "C23", .ref = "C23 7.26.5.1p1-2")
{
	static const char	cs[] = "abc";
	char				s[] = "abc";

	EXPECT_TYPE(memchr(cs, 'b', 3), const void *);
	EXPECT_TYPE(memchr(s, 'b', 3), void *);
	EXPECT_TYPE((memchr)(cs, 'b', 3), void *);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(memchr(s, 'c', 3), (memchr)(s, 'c', 3));
}

ST_ERRNO_KEPT("POSIX memchr()",
	(void)memchr("abc", 'c', 3);
	(void)memchr("abc", 'z', 3);)
