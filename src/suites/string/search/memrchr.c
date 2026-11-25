/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memrchr.c
 * @brief Tests for memrchr() (<string.h>, extension).
 * @Created: 2026/11/25 10:30:09 by Moutig
 * @Updated: 2026/11/25 10:30:46 by Moutig
 *
 *
 * memrchr() is memchr() searching backward: it locates the last byte
 * equal to c (converted to unsigned char) in the first n bytes of s, and
 * returns a pointer to it, or a null pointer if there is none. A null
 * byte is a byte like any other; n == 0 finds nothing.
 *
 * In no standard (EXT=1). Reference: glibc manual, "Search Functions";
 * memrchr(3) of glibc, musl and the BSDs agree.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "glibc memrchr")
{
	EXPECT_TYPE(&memrchr, void *(*)(const void *, int, size_t));
}

TEST_OPT(finds_last_occurrence, .ref = "glibc memrchr")
{
	static const char	s[] = "abcabc";

	EXPECT_PTREQ(memrchr(s, 'a', 6), s + 3);
	EXPECT_PTREQ(memrchr(s, 'c', 6), s + 5);
	EXPECT_PTREQ(memrchr(s, 'b', 6), s + 4);
}

TEST_OPT(not_found_returns_null, .ref = "glibc memrchr")
{
	EXPECT_NULL(memrchr("abc", 'z', 3));
}

TEST_OPT(zero_length_finds_nothing, .ref = "glibc memrchr")
{
	unsigned char	*g = lc_guarded(0);

	EXPECT_NULL(memrchr("a", 'a', 0));
	ASSERT_NOTNULL(g);
	EXPECT_NULL(memrchr(g, 0, 0));
}

TEST_OPT(only_the_first_n_bytes, .ref = "glibc memrchr")
{
	static const char	s[] = "abca";

	EXPECT_PTREQ(memrchr(s, 'a', 3), s);
	EXPECT_NULL(memrchr(s, 'c', 2));
}

TEST_OPT(null_byte_is_an_ordinary_byte, .ref = "glibc memrchr")
{
	static const char	s[] = "a\0a\0b";

	EXPECT_PTREQ(memrchr(s, 'a', 5), s + 2);
	EXPECT_PTREQ(memrchr(s, '\0', 5), s + 3);
	EXPECT_PTREQ(memrchr(s, 'b', 5), s + 4);
}

TEST_OPT(c_converted_to_unsigned_char, .ref = "glibc memrchr")
{
	static const unsigned char	s[] = {0xe9, 'a', 0xe9, 'a'};

	EXPECT_PTREQ(memrchr(s, 0xe9, 4), s + 2);
	EXPECT_PTREQ(memrchr(s, (char)0xe9, 4), s + 2);
	EXPECT_PTREQ(memrchr(s, 'a' + 256, 4), s + 3);
}

/* Bytes c ^ 1 around one c: an exact byte comparison, high bytes too. */
TEST_OPT(near_miss_bytes_do_not_match, .ref = "glibc memrchr")
{
	static const int	cs[] = {0x01, 0x20, 0x7f, 0x80, 0x81, 0xfe, 0xff};
	unsigned char		buf[64];
	int					bad = -1;

	for (size_t k = 0; k < sizeof(cs) / sizeof(cs[0]) && bad < 0; k++)
		for (int p = 0; p < 64 && bad < 0; p++)
		{
			lc_fill(buf, cs[k] ^ 1, sizeof(buf));
			buf[p] = (unsigned char)cs[k];
			if (memrchr(buf, cs[k], sizeof(buf)) != buf + p)
				bad = cs[k];
		}
	EXPECT_EQ(bad, -1);
}

/*
 * Every length (0 to 64), alignment (0 to 7) and position of the last
 * match, with a second match before it and bytes after n that match too.
 */
TEST_OPT(every_length_alignment_and_position, .ref = "glibc memrchr")
{
	unsigned char	buf[96];
	int				bad = -1;

	for (int off = 0; off < 8 && bad < 0; off++)
		for (int len = 0; len <= 64 && bad < 0; len++)
			for (int p = -1; p < len && bad < 0; p++)
			{
				unsigned char	*s = buf + off;
				void			*want = p < 0 ? NULL : s + p;

				lc_fill(buf, 'x', sizeof(buf));
				lc_fill(s + len, 'c', sizeof(buf) - (size_t)(off + len));
				if (p > 0)
					s[0] = 'c';
				if (p >= 0)
					s[p] = 'c';
				if (memrchr(s, 'c', (size_t)len) != want)
					bad = off * 10000 + len * 100 + p;
			}
	EXPECT_EQ(bad, -1);
}

/* The n bytes end right before an inaccessible page: no read past them. */
TEST_OPT(does_not_read_past_n_bytes, .ref = "glibc memrchr")
{
	int	bad = -1;

	for (size_t n = 1; n <= 64 && bad < 0; n++)
	{
		unsigned char	*g = lc_guarded(n);

		if (g == NULL)
			SKIP("no guarded mapping");
		lc_fill(g, 'x', n);
		g[0] = 'c';
		if (memrchr(g, 'c', n) != g || memrchr(g, 'z', n) != NULL)
			bad = (int)n;
	}
	EXPECT_EQ(bad, -1);
}
