/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcasestr.c
 * @brief Tests for strcasestr() (<string.h>, extension).
 * @Created: 2026/11/25 10:29:56 by Moutig
 * @Updated: 2026/11/25 10:30:51 by Moutig
 *
 * strcasestr() is strstr() ignoring the case of letters: it returns a
 * pointer to the first occurrence of needle in haystack, the letters
 * compared as by tolower() in the current locale (here "C": only A-Z and
 * a-z have a case), or a null pointer. An empty needle matches at the
 * start of haystack.
 *
 * In no standard (EXT=1). Reference: glibc manual, "Search Functions";
 * strcasestr(3) of glibc, musl and the BSDs agree.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "glibc strcasestr")
{
	EXPECT_TYPE(&strcasestr, char *(*)(const char *, const char *));
}

TEST_OPT(finds_ignoring_case, .ref = "glibc strcasestr")
{
	static const char	h[] = "Hello World";

	EXPECT_PTREQ(strcasestr(h, "WORLD"), h + 6);
	EXPECT_PTREQ(strcasestr(h, "lLo"), h + 2);
	EXPECT_PTREQ(strcasestr(h, "hello world"), h);
}

TEST_OPT(finds_first_occurrence, .ref = "glibc strcasestr")
{
	static const char	h[] = "xAbab";

	EXPECT_PTREQ(strcasestr(h, "ab"), h + 1);
}

TEST_OPT(empty_needle_returns_haystack, .ref = "glibc strcasestr")
{
	static const char	h[] = "abc";
	static const char	e[] = "";

	EXPECT_PTREQ(strcasestr(h, ""), h);
	EXPECT_PTREQ(strcasestr(e, ""), e);
}

TEST_OPT(not_found_returns_null, .ref = "glibc strcasestr")
{
	EXPECT_NULL(strcasestr("abc", "abd"));
	EXPECT_NULL(strcasestr("ab", "abc"));
	EXPECT_NULL(strcasestr("", "a"));
}

/* Every letter matches its other case, at every position. */
TEST_OPT(every_letter_in_both_cases, .ref = "glibc strcasestr")
{
	char	h[8];
	char	n[2];
	int		bad = -1;

	if (!st_ascii())
		SKIP("execution character set is not ASCII");
	for (int c = 'a'; c <= 'z' && bad < 0; c++)
		for (int p = 0; p < 6 && bad < 0; p++)
		{
			lc_fill(h, '.', 6);
			h[6] = '\0';
			h[p] = (char)(c - 'a' + 'A');
			n[0] = (char)c;
			n[1] = '\0';
			if (strcasestr(h, n) != h + p)
				bad = c;
			n[0] = (char)(c - 'a' + 'A');
			h[p] = (char)c;
			if (strcasestr(h, n) != h + p)
				bad = c;
		}
	EXPECT_EQ(bad, -1);
}

/*
 * Only letters have a case: '@' and '`', '[' and '{', ... differ by 0x20
 * like 'A' and 'a' but must not match each other, nor high bytes.
 */
TEST_OPT(non_letters_compare_exactly, .ref = "glibc strcasestr")
{
	EXPECT_NULL(strcasestr("@[\\]^_", "`"));
	EXPECT_NULL(strcasestr("`{|}~", "@"));
	EXPECT_NULL(strcasestr("[", "{"));
	EXPECT_NULL(strcasestr("\xc9", "\xe9"));
	EXPECT_NULL(strcasestr("1", "\x11"));
}

/* A partial match ends the haystack, at the end of a page. */
TEST_OPT(partial_match_at_page_end, .ref = "glibc strcasestr")
{
	static const char	tail[] = "xxAB";
	char				*g = lc_guarded(sizeof(tail));

	if (g == NULL)
		SKIP("no guarded mapping");
	for (size_t i = 0; i < sizeof(tail); i++)
		g[i] = tail[i];
	EXPECT_NULL(strcasestr(g, "abc"));
	EXPECT_PTREQ(strcasestr(g, "ab"), g + 2);
}

/* Longer needles, where a naive search backtracks. */
TEST_OPT(long_and_periodic_needles, .ref = "glibc strcasestr")
{
	static const char	h[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
		"ABABABABABABABABABABABABABABABAC";

	EXPECT_PTREQ(strcasestr(h, "AAAAAAAAB"), h + 23);
	EXPECT_PTREQ(strcasestr(h, "babababac"), h + 55);
	EXPECT_NULL(strcasestr(h, "abababababababababad"));
}
