/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strxfrm_l.c
 * @brief Tests for strxfrm_l() (<string.h>).
 * @Created: 2026/11/24 05:25:28 by Moutig
 * @Updated: 2026/11/24 06:13:53 by Moutig
 *
 * strxfrm_l() (POSIX.1-2008) is strxfrm() in the locale represented by
 * its locale_t argument: strcmp() of two images orders like strcoll_l()
 * in that locale. Undefined for LC_GLOBAL_LOCALE or an invalid handle.
 *
 * POSIX: XSH strxfrm() (strxfrm_l, CX). The locale object comes from
 * newlocale() of <locale.h> (skipped without it).
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strxfrm_l() SYNOPSIS")
{
	EXPECT_TYPE(&strxfrm_l, size_t (*)(char *restrict, const char *restrict, size_t, locale_t));
}

TEST_OPT(returns_length_of_image, .ref = "POSIX strxfrm_l() RETURN VALUE")
{
	char	buf[64];
	size_t	n;

	ST_POSIX_LOCALE(loc);
	n = strxfrm_l(buf, "hello", sizeof(buf), loc);
	ASSERT_LT(n, sizeof(buf));
	EXPECT_UEQ(strlen(buf), n);
	ST_FREE_LOCALE(loc);
}

TEST_OPT(null_destination_with_zero_size, .ref = "POSIX strxfrm_l() DESCRIPTION")
{
	char	buf[64];

	ST_POSIX_LOCALE(loc);
	EXPECT_UEQ(strxfrm_l(NULL, "hello", 0, loc), strxfrm_l(buf, "hello", sizeof(buf), loc));
	ST_FREE_LOCALE(loc);
}

TEST_OPT(images_order_like_strcoll_l, .ref = "POSIX strxfrm_l() DESCRIPTION")
{
	static const char	*s[] = {"", "a", "ab", "b", "B", "0", "a b", "~"};
	char				x[8][64];

	ST_POSIX_LOCALE(loc);
	for (int i = 0; i < 8; i++)
		ASSERT_LT(strxfrm_l(x[i], s[i], sizeof(x[i]), loc), sizeof(x[i]));
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			EXPECT_EQ(st_sign(strcmp(x[i], x[j])), st_sign(strcoll_l(s[i], s[j], loc)));
	ST_FREE_LOCALE(loc);
}

TEST_OPT(errno_unchanged_on_success, .ref = "POSIX strxfrm_l() DESCRIPTION")
{
	char	buf[64];

	ST_POSIX_LOCALE(loc);
	errno = ST_ERRNO_MARK;
	(void)strxfrm_l(buf, "abc", sizeof(buf), loc);
	EXPECT_EQ(errno, ST_ERRNO_MARK);
	ST_FREE_LOCALE(loc);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strxfrm_l() SYNOPSIS")
{
	ST_POSIX_LOCALE(loc);
	EXPECT_UEQ(strxfrm_l(NULL, "abc", 0, loc), (strxfrm_l)(NULL, "abc", 0, loc));
	ST_FREE_LOCALE(loc);
}
