/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcoll_l.c
 * @brief Tests for strcoll_l() (<string.h>).
 * @Created: 2026/11/24 05:19:31 by Moutig
 * @Updated: 2026/11/24 05:52:41 by Moutig
 *
 * strcoll_l() (POSIX.1-2008) is strcoll() in the locale represented by
 * its locale_t argument. Undefined for LC_GLOBAL_LOCALE or an invalid
 * handle (not tested).
 *
 * In the "C" locale nothing ties the order to the codes in ISO C; POSIX
 * (XBD 7.3.2.6) gives the POSIX locale the order of the portable
 * characters in the ASCII codeset (checked when the codeset is ASCII).
 *
 * POSIX: XSH strcoll() (strcoll_l, CX). The locale object comes from
 * newlocale() of <locale.h> (skipped without it).
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strcoll_l() SYNOPSIS")
{
	EXPECT_TYPE(&strcoll_l, int (*)(const char *, const char *, locale_t));
}

TEST_OPT(equal_strings_compare_zero, .ref = "POSIX strcoll_l() RETURN VALUE")
{
	ST_POSIX_LOCALE(loc);
	EXPECT_EQ(strcoll_l("abc", "abc", loc), 0);
	EXPECT_EQ(strcoll_l("", "", loc), 0);
	ST_FREE_LOCALE(loc);
}

TEST_OPT(posix_object_orders_like_ascii, .ref = "POSIX XBD 7.3.2.6")
{
	static const char	set[] = " !\"#$%&'()*+,-./0123456789:;<=>?@"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	char				a[2] = {0, 0};
	char				b[2] = {0, 0};
	int					bad = -1;

	if (!st_ascii())
		SKIP("execution character set is not ASCII");
	{
		ST_POSIX_LOCALE(loc);
		for (int i = 0; set[i] && bad < 0; i++)
			for (int j = 0; set[j] && bad < 0; j++)
			{
				a[0] = set[i];
				b[0] = set[j];
				if (st_sign(strcoll_l(a, b, loc)) != st_sign(i - j))
					bad = i * 128 + j;
			}
		ST_FREE_LOCALE(loc);
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(agrees_with_strxfrm_l, .ref = "POSIX strxfrm_l() DESCRIPTION")
{
	static const char	*s[] = {"", "a", "ab", "b", "B", "0", "a b", "~"};
	char				x[8][64];

	ST_POSIX_LOCALE(loc);
	for (int i = 0; i < 8; i++)
		ASSERT_LT(strxfrm_l(x[i], s[i], sizeof(x[i]), loc), sizeof(x[i]));
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			EXPECT_EQ(st_sign(strcoll_l(s[i], s[j], loc)), st_sign(strcmp(x[i], x[j])));
	ST_FREE_LOCALE(loc);
}

TEST_OPT(errno_unchanged_on_success, .ref = "POSIX strcoll_l() DESCRIPTION")
{
	ST_POSIX_LOCALE(loc);
	errno = ST_ERRNO_MARK;
	(void)strcoll_l("abc", "abd", loc);
	EXPECT_EQ(errno, ST_ERRNO_MARK);
	ST_FREE_LOCALE(loc);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strcoll_l() SYNOPSIS")
{
	ST_POSIX_LOCALE(loc);
	EXPECT_EQ(st_sign(strcoll_l("ab", "ac", loc)), st_sign((strcoll_l)("ab", "ac", loc)));
	ST_FREE_LOCALE(loc);
}
