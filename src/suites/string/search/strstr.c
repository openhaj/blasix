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
 * @Updated: 2026/11/25 10:39:46 by Moutig
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

TEST_OPT(one_character_needle, .ref = "C11 7.24.5.7p2-3")
{
	static const char	s[] = "abcb";

	EXPECT_PTREQ(strstr(s, "b"), s + 1);
	EXPECT_NULL(strstr(s, "z"));
}

/* A haystack shorter than the needle after its first possible start. */
TEST_OPT(haystack_shorter_than_needle, .ref = "C11 7.24.5.7p3")
{
	EXPECT_NULL(strstr("xxabc", "abcd"));
	EXPECT_NULL(strstr("abcdefg", "abcdefgh"));
	EXPECT_NULL(strstr("", "abcd"));
}

/* Matches far into a long haystack, past any first window it reads. */
TEST_OPT(match_far_in_a_long_haystack, .ref = "C11 7.24.5.7p2")
{
	static char	hay[6001];

	lc_fill(hay, 'x', 6000);
	hay[6000] = '\0';
	hay[5000] = 'a';
	hay[5001] = 'b';
	hay[5002] = 'c';
	hay[5003] = 'd';
	EXPECT_PTREQ(strstr(hay, "abcd"), hay + 5000);
	EXPECT_PTREQ(strstr(hay, "xxabcdxx"), hay + 4998);
	EXPECT_NULL(strstr(hay, "abce"));
	EXPECT_NULL(strstr(hay, "dxxxxxxxxxxxxxxxxxxy"));
}

/*
 * Fill s with n letters of an alphabet of a letters ('a' ...), then a
 * terminator.
 */
static void	randomText(char *s, int n, int a, unsigned *seed)
{
	for (int i = 0; i < n; i++)
		s[i] = (char)('a' + (int)(st_rand(seed) % (unsigned)a));
	s[n] = '\0';
}

/*
 * One case: a random haystack, a needle that is a piece of it (often a
 * letter changed after), compared with the brute-force search.
 *
 * @return 1 if strstr() and the reference agree.
 */
static int	agreesWithBruteForce(char *hay, char *ndl, int hl, int nl, int a,
		unsigned *seed)
{
	randomText(hay, hl, a, seed);
	if (nl <= hl && st_rand(seed) % 4 != 0)
	{
		int	at = (int)(st_rand(seed) % (unsigned)(hl - nl + 1));

		for (int i = 0; i < nl; i++)
			ndl[i] = hay[at + i];
		ndl[nl] = '\0';
		if (st_rand(seed) % 2)
			ndl[st_rand(seed) % (unsigned)nl] = (char)('a' + a - 1);
	}
	else
		randomText(ndl, nl, a, seed);
	return (strstr(hay, ndl) == st_refStrstr(hay, ndl));
}

/* Needles of 4 to 256 bytes on small alphabets: many partial matches. */
TEST_OPT(medium_needles_match_brute_force, .ref = "C11 7.24.5.7p2-3")
{
	static char	hay[1601];
	static char	ndl[257];
	unsigned	seed = 12345;
	int			bad = -1;

	for (int c = 0; c < 300 && bad < 0; c++)
	{
		int	nl = 4 + (int)(st_rand(&seed) % 253);
		int	hl = (int)(st_rand(&seed) % 1600);

		if (!agreesWithBruteForce(hay, ndl, hl, nl, 2 + c % 3, &seed))
			bad = c;
	}
	EXPECT_EQ(bad, -1);
}

/* Needles longer than 256 bytes (another algorithm in some libcs). */
TEST_OPT(long_needles_match_brute_force, .ref = "C11 7.24.5.7p2-3")
{
	static char	hay[4001];
	static char	ndl[601];
	unsigned	seed = 777;
	int			bad = -1;

	for (int c = 0; c < 60 && bad < 0; c++)
	{
		int	nl = 257 + (int)(st_rand(&seed) % 344);
		int	hl = (int)(st_rand(&seed) % 4000);

		if (!agreesWithBruteForce(hay, ndl, hl, nl, 2 + c % 2, &seed))
			bad = c;
	}
	EXPECT_EQ(bad, -1);
}

/* Long periodic needles, where a search that shifts too far misses. */
TEST_OPT(long_periodic_needles, .ref = "C11 7.24.5.7p2")
{
	static char	hay[3002];
	static char	ndl[302];

	for (int i = 0; i < 3000; i++)
		hay[i] = (char)('a' + i % 2);
	hay[3000] = 'c';
	hay[3001] = '\0';
	for (int i = 0; i < 300; i++)
		ndl[i] = (char)('a' + i % 2);
	ndl[300] = 'c';
	ndl[301] = '\0';
	EXPECT_PTREQ(strstr(hay, ndl), hay + 2700);
	for (int i = 0; i < 300; i++)
		ndl[i] = (char)('b' - i % 2);
	ndl[300] = '\0';
	EXPECT_PTREQ(strstr(hay, ndl), hay + 1);
	lc_fill(hay, 'a', 3000);
	hay[3000] = 'b';
	lc_fill(ndl, 'a', 300);
	ndl[300] = 'b';
	ndl[301] = '\0';
	EXPECT_PTREQ(strstr(hay, ndl), hay + 2700);
	ndl[0] = 'b';
	EXPECT_NULL(strstr(hay, ndl));
}

/* A long needle as long as the rest of the haystack. */
TEST_OPT(long_needle_as_long_as_haystack, .ref = "C11 7.24.5.7p2-3")
{
	static char	hay[402];
	static char	ndl[401];

	lc_fill(hay, 'q', 401);
	hay[401] = '\0';
	hay[0] = 'p';
	lc_fill(ndl, 'q', 400);
	ndl[400] = '\0';
	ndl[399] = 'r';
	EXPECT_NULL(strstr(hay, ndl));
	ndl[399] = 'q';
	EXPECT_PTREQ(strstr(hay, ndl), hay + 1);
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
