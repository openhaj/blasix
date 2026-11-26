/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strncmp.c
 * @brief Tests for strncmp() (<string.h>).
 * @Created: 2026/11/24 05:23:26 by Moutig
 * @Updated: 2026/11/25 10:38:56 by Moutig
 *
 * strncmp() compares at most n characters of two arrays, not past a null
 * character; signs as strcmp() (unsigned char). n = 0 gives 0.
 *
 * Clauses: C99 7.21.4.4; C11, C17 7.24.4.4; C23 7.26.4.4. POSIX: XSH
 * strncmp(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.4.4p1")
{
	EXPECT_TYPE(&strncmp, int (*)(const char *, const char *, size_t));
}

TEST_OPT(compares_at_most_n, .ref = "C11 7.24.4.4p2")
{
	EXPECT_EQ(strncmp("abcx", "abcy", 3), 0);
	EXPECT_LT(strncmp("abcx", "abcy", 4), 0);
}

TEST_OPT(stops_at_nul, .ref = "C11 7.24.4.4p2")
{
	EXPECT_EQ(strncmp("ab\0x", "ab\0y", 4), 0);
	EXPECT_LT(strncmp("ab", "abc", 5), 0);
}

TEST_OPT(characters_compare_as_unsigned_char, .ref = "C11 7.24.4p1")
{
	EXPECT_GT(strncmp("\x80", "\x01", 1), 0);
}

TEST_OPT(zero_length_compares_zero, .ref = "C11 7.24.1p2")
{
	EXPECT_EQ(strncmp("a", "b", 0), 0);
}

/* The arrays need not be terminated within n characters. */
TEST_OPT(arrays_need_not_be_terminated, .ref = "C11 7.24.4.4p2")
{
	char	*a = lc_guarded(3);
	char	*b = lc_guarded(3);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	a[0] = b[0] = 'x';
	a[1] = b[1] = 'y';
	a[2] = 'z';
	b[2] = 'z';
	EXPECT_EQ(strncmp(a, b, 3), 0);
	b[2] = 'a';
	EXPECT_GT(strncmp(a, b, 3), 0);
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
 * Every length (0 to 24), both alignments (0 to 7), every position of the
 * first difference and every n up to past the strings: n may stop before,
 * on or after the difference, in the middle of a word.
 */
TEST_OPT(every_length_alignment_difference_and_n, .ref = "C11 7.24.4.4p2-3")
{
	char	b1[40];
	char	b2[40];
	int		bad = -1;

	for (int o1 = 0; o1 < 8 && bad < 0; o1++)
		for (int o2 = 0; o2 < 8 && bad < 0; o2++)
			for (int len = 0; len <= 24 && bad < 0; len++)
				for (int p = 0; p <= len && bad < 0; p++)
					for (int n = 0; n <= len + 2 && bad < 0; n++)
					{
						char	*s1 = b1 + o1;
						char	*s2 = b2 + o2;

						makePair(s1, s2, len, p);
						if (st_sign(strncmp(s1, s2, (size_t)n))
								!= st_refStrncmp(s1, s2, (size_t)n)
							|| st_sign(strncmp(s2, s1, (size_t)n))
								!= st_refStrncmp(s2, s1, (size_t)n))
							bad = (((o1 * 8 + o2) * 25 + len) * 25 + p) * 27 + n;
					}
	EXPECT_EQ(bad, -1);
}

/* Aligned strings that differ at index 9: n = 9 stops just before. */
TEST_OPT(n_stops_in_the_middle_of_a_word, .ref = "C11 7.24.4.4p2")
{
	static const char	a[] __attribute__((aligned(16))) = "0123456789abcdefghij";
	static const char	b[] __attribute__((aligned(16))) = "012345678Xabcdefghij";

	EXPECT_EQ(strncmp(a, b, 9), 0);
	EXPECT_LT(strncmp(a, b, 10), 0);
	EXPECT_GT(strncmp(b, a, 17), 0);
	EXPECT_EQ(strncmp(a + 10, b + 10, 10), 0);
}

/* Equal high bytes for whole words, then a difference past them. */
TEST_OPT(high_bytes_through_whole_words, .ref = "C11 7.24.4p1")
{
	static const char	a[] = "\xff\xfe\x80\x81\xc0\xe9\xff\x80" "\xff\xfe\x80\x81\xc0\xe9\xff\x80" "\x80";
	static const char	b[] = "\xff\xfe\x80\x81\xc0\xe9\xff\x80" "\xff\xfe\x80\x81\xc0\xe9\xff\x80" "\x01";

	EXPECT_EQ(strncmp(a, b, 16), 0);
	EXPECT_GT(strncmp(a, b, 17), 0);
	EXPECT_LT(strncmp(b, a, 100), 0);
}

/*
 * An array of every length without terminator ends right before an
 * inaccessible page, the other is at every alignment: no more than n
 * characters may be read from either.
 */
TEST_OPT(unterminated_array_at_page_end, .ref = "C11 7.24.4.4p2")
{
	char	buf[56];
	int		bad = -1;

	for (int len = 1; len <= 40 && bad < 0; len++)
		for (int o = 0; o < 8 && bad < 0; o++)
		{
			char	*a = lc_guarded((lc_size_t)len);
			char	*b = buf + o;

			if (a == NULL)
				SKIP("no guarded mapping");
			for (int i = 0; i < len; i++)
				a[i] = b[i] = (char)('a' + i % 26);
			b[len] = 'x';
			if (strncmp(a, b, (size_t)len) != 0 || strncmp(b, a, (size_t)len) != 0)
				bad = len * 8 + o;
			b[len - 1] = 'Z';
			if (strncmp(a, b, (size_t)len) <= 0 || strncmp(b, a, (size_t)len) >= 0)
				bad = len * 8 + o;
		}
	EXPECT_EQ(bad, -1);
}

/*
 * A string ending right before an inaccessible page, a longer one at
 * another alignment, n far past both: nothing may be read past either
 * terminator across the page.
 */
TEST_OPT(string_ending_at_page_end, .ref = "C11 7.24.4.4p2")
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
			if (strncmp(s, g, 1000) <= 0 || strncmp(g, s, 1000) >= 0)
				bad = len * 8 + o;
			s[len] = '\0';
			if (strncmp(s, g, 1000) != 0 || strncmp(g, s, (size_t)-1) != 0)
				bad = len * 8 + o;
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(st_sign(strncmp("ab", "ac", 2)), st_sign((strncmp)("ab", "ac", 2)));
}

ST_ERRNO_KEPT("POSIX strncmp()",
	(void)strncmp("ab", "ac", 2);)
