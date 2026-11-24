/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strncasecmp.c
 * @brief Tests for strncasecmp() (<strings.h>).
 * @Created: 2026/11/24 11:08:30 by Moutig
 * @Updated: 2026/11/24 11:13:14 by Moutig
 *
 * strncasecmp() compares, ignoring differences in case, at most n bytes
 * of two possibly null-terminated arrays; n = 0 compares equal.
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

#define SS_CALL(a, b)	strncasecmp(a, b, 2)

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

TEST_OPT(prototype, .ref = "POSIX strncasecmp() SYNOPSIS")
{
	EXPECT_TYPE(&strncasecmp, int (*)(const char *, const char *, size_t));
}

TEST_OPT(compares_at_most_n, .ref = "POSIX strncasecmp() DESCRIPTION")
{
	EXPECT_EQ(strncasecmp("ABCx", "abcy", 3), 0);
	EXPECT_LT(strncasecmp("ABCx", "abcy", 4), 0);
}

TEST_OPT(equal_ignoring_case, .ref = "POSIX strncasecmp() RETURN VALUE")
{
	EXPECT_EQ(strncasecmp("Hello", "hELLO", 10), 0);
	EXPECT_EQ(strncasecmp("", "", 1), 0);
}

TEST_OPT(stops_at_nul, .ref = "POSIX strncasecmp() RETURN VALUE")
{
	EXPECT_EQ(strncasecmp("ab\0x", "AB\0y", 4), 0);
	EXPECT_LT(strncasecmp("ab", "ABC", 5), 0);
}

TEST_OPT(zero_length_compares_equal, .ref = "POSIX strncasecmp() DESCRIPTION")
{
	EXPECT_EQ(strncasecmp("a", "b", 0), 0);
}

TEST_OPT(compares_as_lowercase, .ref = "POSIX strncasecmp() DESCRIPTION")
{
	if (!ss_ascii())
		SKIP("execution character set is not ASCII");
	EXPECT_LT(strncasecmp("_x", "Ax", 2), 0);
	EXPECT_GT(strncasecmp("Ax", "_x", 2), 0);
}

TEST_OPT(every_byte_pair_posix_locale, .ref = "POSIX strncasecmp() DESCRIPTION")
{
	SS_ALL_PAIRS(SS_CALL);
}

/* "Possibly null-terminated arrays": n stops before their end. */
TEST_OPT(arrays_need_not_be_terminated, .ref = "POSIX strncasecmp() RETURN VALUE")
{
	char	*a = lc_guarded(3);
	char	*b = lc_guarded(3);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	a[0] = 'A';
	a[1] = 'b';
	a[2] = 'C';
	b[0] = 'a';
	b[1] = 'B';
	b[2] = 'c';
	EXPECT_EQ(strncasecmp(a, b, 3), 0);
	b[2] = 'd';
	EXPECT_LT(strncasecmp(a, b, 3), 0);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strncasecmp() SYNOPSIS")
{
	EXPECT_EQ(ss_sign(strncasecmp("ab", "AC", 2)), ss_sign((strncasecmp)("ab", "AC", 2)));
}

SS_ERRNO_KEPT("POSIX strncasecmp()",
	(void)strncasecmp("Ab", "aC", 2);)
