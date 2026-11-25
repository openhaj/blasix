/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcmp.c
 * @brief Tests for strcmp() (<string.h>).
 * @Created: 2026/11/24 05:19:24 by Moutig
 * @Updated: 2026/11/25 10:38:35 by Moutig
 *
 * strcmp() compares two strings; the sign of a nonzero result is that of
 * the first differing pair of characters, interpreted as unsigned char.
 *
 * Clauses: C99 7.21.4.2; C11, C17 7.24.4.2; C23 7.26.4.2. POSIX: XSH
 * strcmp(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.4.2p1")
{
	EXPECT_TYPE(&strcmp, int (*)(const char *, const char *));
}

TEST_OPT(equal_strings_compare_zero, .ref = "C11 7.24.4.2p3")
{
	EXPECT_EQ(strcmp("abc", "abc"), 0);
	EXPECT_EQ(strcmp("", ""), 0);
}

TEST_OPT(sign_of_first_difference, .ref = "C11 7.24.4.2p3")
{
	EXPECT_LT(strcmp("abc", "abd"), 0);
	EXPECT_GT(strcmp("b", "abc"), 0);
}

TEST_OPT(prefix_is_smaller, .ref = "C11 7.24.4.2p2")
{
	EXPECT_LT(strcmp("ab", "abc"), 0);
	EXPECT_GT(strcmp("abc", "ab"), 0);
	EXPECT_LT(strcmp("", "a"), 0);
}

TEST_OPT(characters_compare_as_unsigned_char, .ref = "C11 7.24.4p1")
{
	EXPECT_GT(strcmp("\x80", "\x01"), 0);
	EXPECT_GT(strcmp("a\xff", "a\x7f"), 0);
}

TEST_OPT(stops_at_nul, .ref = "C11 7.24.4.2p2")
{
	EXPECT_EQ(strcmp("a\0x", "a\0y"), 0);
}

/*
 * Fill s1 and s2 with the same len characters, then make them differ at
 * p (p == len: equal) in one of three ways: s2 one more, s1 a high byte
 * (0x80 against 0x7f), s2 ends there. Bytes after the terminators differ.
 */
static void	makePair(char *s1, char *s2, int len, int p)
{
	for (int i = 0; i < len; i++)
		s1[i] = s2[i] = (char)('A' + (i * 7) % 50);
	s1[len] = s2[len] = '\0';
	s1[len + 1] = 'X';
	s2[len + 1] = 'Y';
	if (p < len && p % 3 == 0)
		s2[p] = (char)(s1[p] + 1);
	else if (p < len && p % 3 == 1)
	{
		s1[p] = (char)0x80;
		s2[p] = (char)0x7f;
	}
	else if (p < len)
		s2[p] = '\0';
}

/*
 * Every length (0 to 40), both alignments (0 to 7) and every position of
 * the first difference: the word-at-a-time paths and their tails.
 */
TEST_OPT(every_length_alignment_and_difference, .ref = "C11 7.24.4.2p3")
{
	char	b1[56];
	char	b2[56];
	int		bad = -1;

	for (int o1 = 0; o1 < 8 && bad < 0; o1++)
		for (int o2 = 0; o2 < 8 && bad < 0; o2++)
			for (int len = 0; len <= 40 && bad < 0; len++)
				for (int p = 0; p <= len && bad < 0; p++)
				{
					char	*s1 = b1 + o1;
					char	*s2 = b2 + o2;

					makePair(s1, s2, len, p);
					if (st_sign(strcmp(s1, s2)) != st_refStrncmp(s1, s2, (size_t)-1)
						|| st_sign(strcmp(s2, s1)) != st_refStrncmp(s2, s1, (size_t)-1))
						bad = ((o1 * 8 + o2) * 41 + len) * 41 + p;
				}
	EXPECT_EQ(bad, -1);
}

/* Equal high bytes for whole words, then a difference past them. */
TEST_OPT(high_bytes_through_whole_words, .ref = "C11 7.24.4p1")
{
	static const char	a[] = "\xff\xfe\x80\x81\xc0\xe9\xff\x80" "\xff\xfe\x80\x81\xc0\xe9\xff\x80" "\x80";
	static const char	b[] = "\xff\xfe\x80\x81\xc0\xe9\xff\x80" "\xff\xfe\x80\x81\xc0\xe9\xff\x80" "\x01";

	EXPECT_EQ(strcmp(a, a), 0);
	EXPECT_GT(strcmp(a, b), 0);
	EXPECT_LT(strcmp(b, a), 0);
}

/*
 * One string ends right before an inaccessible page, the other is longer
 * and at another alignment: nothing past either terminator may be read
 * across the page (a word read of the unaligned one would cross it).
 */
TEST_OPT(string_ending_at_page_end, .ref = "C11 7.24.4.2p2")
{
	char	buf[64];
	int		bad = -1;

	for (int len = 0; len <= 40 && bad < 0; len++)
		for (int o = 0; o < 8 && bad < 0; o++)
		{
			char	*g = lc_guarded((lc_size_t)len + 1);
			char	*s = buf + o;

			if (g == NULL)
				SKIP("no guarded mapping");
			for (int i = 0; i < len; i++)
				g[i] = s[i] = (char)('a' + i % 26);
			g[len] = '\0';
			s[len] = 'z';
			s[len + 1] = '\0';
			if (strcmp(s, g) <= 0 || strcmp(g, s) >= 0)
				bad = len * 8 + o;
			s[len] = '\0';
			if (strcmp(s, g) != 0 || strcmp(g, s) != 0)
				bad = len * 8 + o;
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(st_sign(strcmp("ab", "ac")), st_sign((strcmp)("ab", "ac")));
}

ST_ERRNO_KEPT("POSIX strcmp()",
	(void)strcmp("ab", "ac");)
