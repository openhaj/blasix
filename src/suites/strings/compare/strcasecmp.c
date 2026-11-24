/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcasecmp.c
 * @brief Tests for strcasecmp() (<strings.h>).
 * @Created: 2026/11/24 11:08:14 by Moutig
 * @Updated: 2026/11/24 11:12:54 by Moutig
 *
 * strcasecmp() compares two strings ignoring differences in case: the
 * result is greater than, equal to or less than 0 as s1 is, ignoring case,
 * greater than, equal to or less than s2.
 *
 * In the POSIX locale (the locale at startup), these functions behave as
 * if both strings were converted to lowercase and then compared byte by
 * byte (XSH strcasecmp(); in every edition for strcasecmp() and
 * strncasecmp(), for the *_l functions from POSIX.1-2017). In other
 * locales the result is unspecified: not tested.
 *
 * POSIX: XSH strcasecmp() (XSI in POSIX.1-2001, base from POSIX.1-2008);
 * POSIX.1-2024: errno unchanged on valid input.
 */
#include "strings/strings_cases.h"

#define SS_CALL(a, b)	strcasecmp(a, b)

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

TEST_OPT(prototype, .ref = "POSIX strcasecmp() SYNOPSIS")
{
	EXPECT_TYPE(&strcasecmp, int (*)(const char *, const char *));
}

TEST_OPT(equal_ignoring_case, .ref = "POSIX strcasecmp() RETURN VALUE")
{
	EXPECT_EQ(strcasecmp("Hello", "hELLO"), 0);
	EXPECT_EQ(strcasecmp("abc", "abc"), 0);
	EXPECT_EQ(strcasecmp("", ""), 0);
}

TEST_OPT(different_strings, .ref = "POSIX strcasecmp() RETURN VALUE")
{
	EXPECT_LT(strcasecmp("abc", "ABD"), 0);
	EXPECT_GT(strcasecmp("ABD", "abc"), 0);
}

TEST_OPT(prefix_is_smaller, .ref = "POSIX strcasecmp() RETURN VALUE")
{
	EXPECT_LT(strcasecmp("AB", "abc"), 0);
	EXPECT_GT(strcasecmp("abc", "AB"), 0);
	EXPECT_LT(strcasecmp("", "a"), 0);
}

/*
 * Compared as lowercase: '_' (0x5f) is between 'Z' (0x5a) and 'a'
 * (0x61), so "_" < "A" here while strcmp() says "A" < "_".
 */
TEST_OPT(compares_as_lowercase, .ref = "POSIX strcasecmp() DESCRIPTION")
{
	if (!ss_ascii())
		SKIP("execution character set is not ASCII");
	EXPECT_LT(strcasecmp("_", "A"), 0);
	EXPECT_GT(strcasecmp("A", "_"), 0);
	EXPECT_LT(strcasecmp("[", "a"), 0);
}

TEST_OPT(every_byte_pair_posix_locale, .ref = "POSIX strcasecmp() DESCRIPTION")
{
	SS_ALL_PAIRS(SS_CALL);
}

TEST_OPT(stops_at_nul, .ref = "POSIX strcasecmp() DESCRIPTION")
{
	EXPECT_EQ(strcasecmp("a\0x", "A\0y"), 0);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strcasecmp() SYNOPSIS")
{
	EXPECT_EQ(ss_sign(strcasecmp("ab", "AC")), ss_sign((strcasecmp)("ab", "AC")));
}

SS_ERRNO_KEPT("POSIX strcasecmp()",
	(void)strcasecmp("Ab", "aC");)
