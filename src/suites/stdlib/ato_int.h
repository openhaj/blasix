/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ato_int.h
 * @brief The cases of atoi(), atol() and atoll(), for any of them.
 * @Created: 2026/11/27 12:44:45 by Moutig
 * @Updated: 2026/11/27 12:48:32 by Moutig
 *
 * Except for the behaviour on error, atoi(nptr) is (int)strtol(nptr,
 * NULL, 10), atol(nptr) is strtol(nptr, NULL, 10) and atoll(nptr) is
 * strtoll(nptr, NULL, 10) (C 7.24.1.2p2): white space, an optional sign,
 * decimal digits only (no 0x, no octal 0, no C23 0b), the value of the
 * longest initial sequence of that form, 0 when there is none. A value
 * that cannot be represented is undefined (7.24.1p1): never tested here;
 * errno need not change on an error: not tested either.
 *
 * A template, as strto_int.h: the unit defines AT_FN (the function),
 * AT_T (its return type), AT_MIN and AT_MAX (the range of AT_T), then
 * includes this file once. It includes no header of the libc and calls
 * nothing but the function.
 */

#if !defined(AT_FN) || !defined(AT_T) || !defined(AT_MIN) || !defined(AT_MAX)
# error "ato_int.h: define AT_FN, AT_T, AT_MIN, AT_MAX"
#endif

#include "framework/lc.h"

/* The decimal text of v (at least 32 bytes), without the libc. */
static char	*atFormat(char *buf, AT_T v)
{
	unsigned long long	m = v < 0 ? 0ULL - (unsigned long long)v : (unsigned long long)v;
	char				tmp[32];
	int					n = 0;
	int					i = 0;

	do
	{
		tmp[n++] = (char)('0' + (int)(m % 10));
		m /= 10;
	} while (m);
	if (v < 0)
		buf[i++] = '-';
	while (n)
		buf[i++] = tmp[--n];
	buf[i] = '\0';
	return (buf);
}

TEST_OPT(decimal_values, .ref = "C11 7.22.1.2p2")
{
	EXPECT_TRUE(AT_FN("0") == 0);
	EXPECT_TRUE(AT_FN("7") == 7);
	EXPECT_TRUE(AT_FN("12345") == 12345);
	EXPECT_TRUE(AT_FN("32767") == 32767);
}

TEST_OPT(sign, .ref = "C11 7.22.1.2p2")
{
	EXPECT_TRUE(AT_FN("+42") == 42);
	EXPECT_TRUE(AT_FN("-42") == -42);
	EXPECT_TRUE(AT_FN("-0") == 0);
	EXPECT_TRUE(AT_FN("+-1") == 0);
	EXPECT_TRUE(AT_FN("--1") == 0);
}

/* Every white-space character of the "C" locale may come first. */
TEST_OPT(leading_white_space, .ref = "C11 7.22.1.2p2")
{
	EXPECT_TRUE(AT_FN(" \t\n\v\f\r-17") == -17);
	EXPECT_TRUE(AT_FN("     9") == 9);
	EXPECT_TRUE(AT_FN("\n+3") == 3);
	EXPECT_TRUE(AT_FN("- 3") == 0);
}

/* The value of the digits before the first character that is not one. */
TEST_OPT(stops_at_the_first_non_digit, .ref = "C11 7.22.1.2p2")
{
	EXPECT_TRUE(AT_FN("123abc") == 123);
	EXPECT_TRUE(AT_FN("45 67") == 45);
	EXPECT_TRUE(AT_FN("8.9") == 8);
	EXPECT_TRUE(AT_FN("-6-") == -6);
	EXPECT_TRUE(AT_FN("1e5") == 1);
}

/* Base 10 always: 0x and 0b are no prefixes, a leading 0 is not octal. */
TEST_OPT(decimal_only, .ref = "C11 7.22.1.2p2")
{
	EXPECT_TRUE(AT_FN("010") == 10);
	EXPECT_TRUE(AT_FN("0x1f") == 0);
	EXPECT_TRUE(AT_FN("0b101") == 0);
	EXPECT_TRUE(AT_FN("1f") == 1);
	EXPECT_TRUE(AT_FN("0009") == 9);
}

/* No digit: no conversion, 0. */
TEST_OPT(no_digits_gives_zero, .ref = "C11 7.22.1.4p7-8")
{
	EXPECT_TRUE(AT_FN("") == 0);
	EXPECT_TRUE(AT_FN("   ") == 0);
	EXPECT_TRUE(AT_FN("-") == 0);
	EXPECT_TRUE(AT_FN("abc") == 0);
	EXPECT_TRUE(AT_FN(".5") == 0);
}

/* The limits of the type are representable: exact. */
TEST_OPT(limits_of_the_type, .ref = "C11 7.22.1.2p3")
{
	char	txt[32];

	EXPECT_TRUE(AT_FN(atFormat(txt, AT_MAX)) == AT_MAX);
	EXPECT_TRUE(AT_FN(atFormat(txt, AT_MIN)) == AT_MIN);
	EXPECT_TRUE(AT_FN(atFormat(txt, AT_MAX - 1)) == AT_MAX - 1);
	EXPECT_TRUE(AT_FN(atFormat(txt, AT_MIN + 1)) == AT_MIN + 1);
}

/* Many leading zeros: still the same value (no overflow on zeros). */
TEST_OPT(many_leading_zeros, .ref = "C11 7.22.1.2p2")
{
	char	txt[200];

	txt[0] = '-';
	lc_fill(txt + 1, '0', 150);
	(void)atFormat(txt + 151, AT_MAX);
	EXPECT_TRUE(AT_FN(txt) == -AT_MAX);
	EXPECT_TRUE(AT_FN(txt + 1) == AT_MAX);
}

/* Values over the whole range, in decimal: their value. */
TEST_OPT(round_trip_over_the_range, .ref = "C11 7.22.1.2p2-3")
{
	unsigned long long	x = 2026;
	int					bad = -1;

	for (int k = 0; k < 2000 && bad < 0; k++)
	{
		char	txt[32];
		AT_T	v;

		x = x * 6364136223846793005ULL + 1442695040888963407ULL;
		v = (AT_T)((x >> (k % 64)) % ((unsigned long long)AT_MAX + 1));
		if (k % 2)
			v = (AT_T)-v;
		if (AT_FN(atFormat(txt, v)) != v)
			bad = k;
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_TRUE(AT_FN(" -77") == (AT_FN)(" -77"));
}
