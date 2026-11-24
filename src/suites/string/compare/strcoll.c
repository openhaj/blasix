/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcoll.c
 * @brief Tests for strcoll() (<string.h>).
 * @Created: 2026/11/24 05:19:43 by Moutig
 * @Updated: 2026/11/24 05:54:46 by Moutig
 *
 * strcoll() compares two strings as interpreted by LC_COLLATE of the
 * current locale (the "C" locale here, the startup locale): zero exactly
 * when equal in that order, and consistent with strxfrm() + strcmp().
 *
 * In the "C" locale nothing ties the order to the codes in ISO C; POSIX
 * (XBD 7.3.2.6) gives the POSIX locale the order of the portable
 * characters in the ASCII codeset (checked when the codeset is ASCII).
 *
 * Clauses: C99 7.21.4.3; C11, C17 7.24.4.3; C23 7.26.4.3. POSIX: XSH
 * strcoll() (errno unchanged on success, CX).
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.4.3p1")
{
	EXPECT_TYPE(&strcoll, int (*)(const char *, const char *));
}

TEST_OPT(equal_strings_compare_zero, .ref = "C11 7.24.4.3p3")
{
	EXPECT_EQ(strcoll("abc", "abc"), 0);
	EXPECT_EQ(strcoll("", ""), 0);
}

TEST_OPT(order_is_antisymmetric, .ref = "C11 7.24.4.3p3")
{
	static const char	*s[] = {"", "a", "ab", "b", "B", "0", "a b", "~"};

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			EXPECT_EQ(st_sign(strcoll(s[i], s[j])), -st_sign(strcoll(s[j], s[i])));
}

/* strcmp() of the strxfrm() images orders like strcoll() (7.24.4.5p2). */
TEST_OPT(agrees_with_strxfrm, .ref = "C11 7.24.4.5p2")
{
	static const char	*s[] = {"", "a", "ab", "b", "B", "0", "a b", "~"};
	char				x[8][64];

	for (int i = 0; i < 8; i++)
		ASSERT_LT(strxfrm(x[i], s[i], sizeof(x[i])), sizeof(x[i]));
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			EXPECT_EQ(st_sign(strcoll(s[i], s[j])), st_sign(strcmp(x[i], x[j])));
}

TEST_OPT(posix_locale_orders_like_ascii, .since = "POSIX.1-2001",
	.ref = "POSIX XBD 7.3.2.6")
{
	static const char	set[] = " !\"#$%&'()*+,-./0123456789:;<=>?@"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	char				a[2] = {0, 0};
	char				b[2] = {0, 0};
	int					bad = -1;

	if (!st_ascii())
		SKIP("execution character set is not ASCII");
	for (int i = 0; set[i] && bad < 0; i++)
		for (int j = 0; set[j] && bad < 0; j++)
		{
			a[0] = set[i];
			b[0] = set[j];
			if (st_sign(strcoll(a, b)) != st_sign(i - j))
				bad = i * 128 + j;
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(errno_unchanged_on_success, .since = "POSIX.1-2001",
	.ref = "POSIX strcoll() DESCRIPTION")
{
	errno = ST_ERRNO_MARK;
	(void)strcoll("abc", "abd");
	EXPECT_EQ(errno, ST_ERRNO_MARK);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(st_sign(strcoll("ab", "ac")), st_sign((strcoll)("ab", "ac")));
}
