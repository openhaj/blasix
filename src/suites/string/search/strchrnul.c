/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strchrnul.c
 * @brief Tests for strchrnul() (<string.h>, extension).
 * @Created: 2026/11/25 10:30:20 by Moutig
 * @Updated: 2026/11/25 10:30:54 by Moutig
 *
 * strchrnul() is strchr() that never fails: it returns a pointer to the
 * first c (converted to char) in the string s, or to its terminating null
 * character when c does not occur. With c == '\0' it finds the
 * terminator.
 *
 * In no standard (EXT=1). Reference: glibc manual, "Search Functions";
 * strchrnul(3) of glibc, musl and FreeBSD agree.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "glibc strchrnul")
{
	EXPECT_TYPE(&strchrnul, char *(*)(const char *, int));
}

TEST_OPT(finds_first_occurrence, .ref = "glibc strchrnul")
{
	static const char	s[] = "abcabc";

	EXPECT_PTREQ(strchrnul(s, 'b'), s + 1);
	EXPECT_PTREQ(strchrnul(s, 'a'), s);
}

TEST_OPT(not_found_returns_terminator, .ref = "glibc strchrnul")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(strchrnul(s, 'z'), s + 3);
}

TEST_OPT(nul_finds_terminator, .ref = "glibc strchrnul")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(strchrnul(s, '\0'), s + 3);
}

TEST_OPT(empty_string, .ref = "glibc strchrnul")
{
	static const char	s[] = "";

	EXPECT_PTREQ(strchrnul(s, 'a'), s);
	EXPECT_PTREQ(strchrnul(s, '\0'), s);
}

TEST_OPT(does_not_search_past_nul, .ref = "glibc strchrnul")
{
	static const char	s[] = "ab\0cb";

	EXPECT_PTREQ(strchrnul(s, 'c'), s + 2);
}

TEST_OPT(c_converted_to_char, .ref = "glibc strchrnul")
{
	static const char	s[] = "a\xe9" "a";

	EXPECT_PTREQ(strchrnul(s, 0xe9), s + 1);
	EXPECT_PTREQ(strchrnul(s, (char)0xe9), s + 1);
	EXPECT_PTREQ(strchrnul(s, 'a' + 256), s);
}

/*
 * Bytes c ^ 1 before one c: an exact byte comparison, high bytes too
 * (no c with c ^ 1 == '\0', which would end the string).
 */
TEST_OPT(near_miss_bytes_do_not_match, .ref = "glibc strchrnul")
{
	static const int	cs[] = {0x03, 0x20, 0x7f, 0x80, 0x81, 0xfe, 0xff};
	char				buf[65];
	int					bad = -1;

	for (size_t k = 0; k < sizeof(cs) / sizeof(cs[0]) && bad < 0; k++)
		for (int p = 0; p < 64 && bad < 0; p++)
		{
			lc_fill(buf, cs[k] ^ 1, 64);
			buf[64] = '\0';
			buf[p] = (char)cs[k];
			if (strchrnul(buf, cs[k]) != buf + p)
				bad = cs[k];
		}
	EXPECT_EQ(bad, -1);
}

/* Every length (0 to 64), alignment (0 to 7) and position, or none. */
TEST_OPT(every_length_alignment_and_position, .ref = "glibc strchrnul")
{
	char	buf[80];
	int		bad = -1;

	for (int off = 0; off < 8 && bad < 0; off++)
		for (int len = 0; len <= 64 && bad < 0; len++)
			for (int p = 0; p <= len && bad < 0; p++)
			{
				char	*s = buf + off;

				lc_fill(buf, 'x', sizeof(buf));
				s[len] = '\0';
				if (p < len)
					s[p] = 'c';
				s[len + 1] = 'c';
				if (strchrnul(s, 'c') != s + p)
					bad = off * 10000 + len * 100 + p;
			}
	EXPECT_EQ(bad, -1);
}

/* A string that ends with the page: no read past its terminator. */
TEST_OPT(string_ending_at_page_end, .ref = "glibc strchrnul")
{
	int	bad = -1;

	for (size_t len = 0; len <= 64 && bad < 0; len++)
	{
		char	*g = lc_guarded(len + 1);

		if (g == NULL)
			SKIP("no guarded mapping");
		lc_fill(g, 'x', len);
		g[len] = '\0';
		if (strchrnul(g, 'z') != g + len)
			bad = (int)len;
	}
	EXPECT_EQ(bad, -1);
}
