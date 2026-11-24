/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcasecmp_l.c
 * @brief Tests for strcasecmp_l() (<strings.h>).
 * @Created: 2026/11/24 11:08:06 by Moutig
 * @Updated: 2026/11/24 11:12:43 by Moutig
 *
 * strcasecmp_l() (POSIX.1-2008) is strcasecmp() with the case of the
 * characters taken from the locale represented by its locale_t argument.
 *
 * In the POSIX locale (the locale at startup), these functions behave as
 * if both strings were converted to lowercase and then compared byte by
 * byte (XSH strcasecmp(); in every edition for strcasecmp() and
 * strncasecmp(), for the *_l functions from POSIX.1-2017). In other
 * locales the result is unspecified: not tested.
 *
 * The locale object comes from newlocale() of <locale.h> (skipped
 * without it). Undefined for LC_GLOBAL_LOCALE or an invalid handle (not
 * tested; POSIX.1-2008 only said "may fail" with EINVAL).
 *
 * POSIX: XSH strcasecmp() (strcasecmp_l, CX); POSIX.1-2024: errno
 * unchanged on valid input.
 */
#include "strings/strings_cases.h"

static locale_t	gLoc;

#define SS_CALL(a, b)	strcasecmp_l(a, b, gLoc)

/* Every pair of single-byte strings, against the POSIX-locale oracle. */
#define SS_ALL_PAIRS(CALL)													\
	do {																	\
		char	a_[2] = {0, 0};												\
		char	b_[2] = {0, 0};												\
		int		bad_ = -1;													\
																			\
		if (!ss_ascii())													\
			SKIP("execution character set is not ASCII");					\
		for (int i_ = 1; i_ <= UCHAR_MAX && bad_ < 0; i_++)					\
			for (int j_ = 1; j_ <= UCHAR_MAX && bad_ < 0; j_++)				\
			{																\
				a_[0] = (char)i_;											\
				b_[0] = (char)j_;											\
				if (ss_sign(CALL(a_, b_)) != ss_sign(ss_lower(i_) - ss_lower(j_)))	\
					bad_ = i_ * 256 + j_;									\
			}																\
		EXPECT_EQ(bad_, -1);												\
	} while (0)

TEST_OPT(prototype, .ref = "POSIX strcasecmp_l() SYNOPSIS")
{
	EXPECT_TYPE(&strcasecmp_l, int (*)(const char *, const char *, locale_t));
}

TEST_OPT(equal_ignoring_case, .ref = "POSIX strcasecmp_l() RETURN VALUE")
{
	SS_POSIX_LOCALE(loc);
	EXPECT_EQ(strcasecmp_l("Hello", "hELLO", loc), 0);
	EXPECT_EQ(strcasecmp_l("", "", loc), 0);
	SS_FREE_LOCALE(loc);
}

TEST_OPT(different_strings, .ref = "POSIX strcasecmp_l() RETURN VALUE")
{
	SS_POSIX_LOCALE(loc);
	EXPECT_NE(strcasecmp_l("abc", "ABD", loc), 0);
	EXPECT_NE(strcasecmp_l("ab", "abc", loc), 0);
	SS_FREE_LOCALE(loc);
}

TEST_OPT(compares_as_lowercase, .since = "POSIX.1-2017",
	.ref = "POSIX strcasecmp_l() DESCRIPTION")
{
	if (!ss_ascii())
		SKIP("execution character set is not ASCII");
	{
		SS_POSIX_LOCALE(loc);
		EXPECT_LT(strcasecmp_l("_", "A", loc), 0);
		EXPECT_LT(strcasecmp_l("abc", "ABD", loc), 0);
		EXPECT_LT(strcasecmp_l("AB", "abc", loc), 0);
		SS_FREE_LOCALE(loc);
	}
}

TEST_OPT(every_byte_pair_posix_object, .since = "POSIX.1-2017",
	.ref = "POSIX strcasecmp_l() DESCRIPTION")
{
	SS_POSIX_LOCALE(loc);
	gLoc = loc;
	SS_ALL_PAIRS(SS_CALL);
	SS_FREE_LOCALE(loc);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strcasecmp_l() SYNOPSIS")
{
	SS_POSIX_LOCALE(loc);
	EXPECT_EQ(ss_sign(strcasecmp_l("ab", "AC", loc)), ss_sign((strcasecmp_l)("ab", "AC", loc)));
	SS_FREE_LOCALE(loc);
}

TEST_OPT(errno_unchanged_on_valid_input, .since = "POSIX.1-2024",
	.ref = "POSIX strcasecmp_l() DESCRIPTION (Issue 8)")
{
	SS_POSIX_LOCALE(loc);
	errno = SS_ERRNO_MARK;
	(void)strcasecmp_l("Ab", "aC", loc);
	EXPECT_EQ(errno, SS_ERRNO_MARK);
	SS_FREE_LOCALE(loc);
}
