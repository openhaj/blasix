/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strto_int.h
 * @brief The cases of the strtol() family, for any of its functions.
 * @Created: 2026/11/27 12:15:22 by Moutig
 * @Updated: 2026/11/27 12:19:14 by Moutig
 *
 * strtol, strtoll, strtoul, strtoull (C 7.24.1.7) and strtoimax,
 * strtoumax (C 7.8.2.3, "equivalent to" them) share one contract: white
 * space, a sign, then digits in base 2 ... 36 or base 0 (a C integer
 * constant), the longest initial subsequence of that form as the subject
 * sequence, *endptr past it (nptr when there is none, and 0 returned),
 * the value negated in the return type for a minus sign, the limit and
 * ERANGE out of range. C23 adds the 0b / 0B prefix and refuses digit
 * separators. POSIX (CX): errno unchanged on success, EINVAL for a base
 * that is not supported.
 *
 * Not a header with a guard: a template. The unit defines the parameters,
 * includes the header that declares the function (and the limits), then
 * includes this file once; it adds the cases common to the family (the
 * unit keeps its prototype case and what is its own):
 *
 *   ST_FN            the function (strtol, strtoumax, ...)
 *   ST_T             its return type
 *   ST_UT            an unsigned type that holds every magnitude of ST_T
 *   ST_SIGNED        1 for a signed ST_T, 0 for an unsigned one
 *   ST_MIN, ST_MAX   the limits ST_FN returns out of range (ST_MIN 0 if
 *                    unsigned)
 *   ST_REF_FORM      the clause of the subject sequence ("C11 7.22.1.4")
 *   ST_REF_RET       the clause of the returned value
 *   ST_ERRNO_SINCE   the POSIX edition from which errno is kept on success
 *   ST_EINVAL_SINCE  the POSIX edition from which an unsupported base
 *                    "shall" give EINVAL
 *
 * The oracle (the text of a value in a base) is computed here, without
 * the libc. Only <errno.h> is included, to read errno: nothing of
 * <stdlib.h> or <inttypes.h> leaks into the other header's units.
 */

#if !defined(ST_FN) || !defined(ST_T) || !defined(ST_UT) || !defined(ST_SIGNED) \
	|| !defined(ST_MIN) || !defined(ST_MAX) || !defined(ST_REF_FORM) \
	|| !defined(ST_REF_RET) || !defined(ST_ERRNO_SINCE) || !defined(ST_EINVAL_SINCE)
# error "strto_int.h: define ST_FN, ST_T, ST_UT, ST_SIGNED, ST_MIN, ST_MAX, ST_REF_*, ST_*_SINCE"
# define ST_SIGNED 0
#endif

#include "framework/lc.h"
#include <errno.h>

/** errno before a successful call: a value no valid call would set. */
#define ST_ERRNO_MARK	EDOM

/* ----- Oracle ----- */

/*
 * Write the text of -mag (neg) or mag in @p base (2 ... 36), lower or upper
 * case letters, after @p prefix ("", "0x", ...).
 *
 * @return @p buf (at least 80 bytes).
 */
static char	*stFormat(char *buf, ST_UT mag, int neg, unsigned base, int upper,
	const char *prefix)
{
	static const char	lo[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	static const char	up[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char				tmp[72];
	int					n = 0;
	int					i = 0;

	do
	{
		tmp[n++] = (upper ? up : lo)[mag % base];
		mag /= base;
	} while (mag);
	if (neg)
		buf[i++] = '-';
	while (*prefix)
		buf[i++] = *prefix++;
	while (n)
		buf[i++] = tmp[--n];
	buf[i] = '\0';
	return (buf);
}

/* Add one to the decimal number at the end of @p s ("9" -> "10"). */
static void	stIncrement(char *s)
{
	size_t	n = lc_strlen(s);
	size_t	i = n;

	while (i > 0 && s[i - 1] == '9')
		s[--i] = '0';
	if (i > 0 && s[i - 1] >= '0' && s[i - 1] <= '8')
	{
		s[i - 1]++;
		return ;
	}
	for (size_t k = n + 1; k > i; k--)
		s[k] = s[k - 1];
	s[i] = '1';
}

/* Convert @p s in @p base; is the value @p want and the end at s + @p len? */
static int	stIs(const char *s, int base, ST_T want, size_t len)
{
	char	*end = NULL;
	ST_T	got = ST_FN(s, &end, base);

	return (got == want && end == s + len);
}

/* ----- Subject sequence ----- */

TEST_OPT(decimal_and_end_pointer, .ref = ST_REF_FORM)
{
	static const char	s[] = "12345x";

	EXPECT_TRUE(stIs(s, 10, 12345, 5));
	EXPECT_TRUE(stIs("0", 10, 0, 1));
	EXPECT_TRUE(stIs("007", 10, 7, 3));
}

/* Every white-space character of the "C" locale may come first. */
TEST_OPT(leading_space_and_sign, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(stIs(" \t\n\v\f\r+42;", 10, 42, 9));
	EXPECT_TRUE(stIs("   7", 10, 7, 4));
	EXPECT_TRUE(stIs("\v-0", 10, 0, 3));
}

TEST_OPT(negative_values, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(stIs("-42", 10, (ST_T)-42, 3));
	EXPECT_TRUE(stIs("-0x10", 16, (ST_T)-16, 5));
	EXPECT_TRUE(stIs(" -z", 36, (ST_T)-35, 3));
}

/* Base 16: an optional 0x / 0X after the sign. */
TEST_OPT(hexadecimal_prefix, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(stIs("0x1Fg", 16, 31, 4));
	EXPECT_TRUE(stIs("0X1f", 16, 31, 4));
	EXPECT_TRUE(stIs("1F", 16, 31, 2));
	EXPECT_TRUE(stIs("+0xa", 16, 10, 4));
	EXPECT_TRUE(stIs("0x1Fg", 0, 31, 4));
	EXPECT_TRUE(stIs("0XfF", 0, 255, 4));
}

/* Base 0: decimal, octal after a 0, hexadecimal after 0x. */
TEST_OPT(base_zero_octal_and_decimal, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(stIs("017", 0, 15, 3));
	EXPECT_TRUE(stIs("17", 0, 17, 2));
	EXPECT_TRUE(stIs("0", 0, 0, 1));
	EXPECT_TRUE(stIs("019", 0, 1, 2));
	EXPECT_TRUE(stIs("08", 0, 0, 1));
	EXPECT_TRUE(stIs("-010", 0, (ST_T)-8, 4));
}

/* Letters a ... z (either case) are 10 ... 35, only below the base. */
TEST_OPT(letters_up_to_base_36, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(stIs("zZ", 36, 35 * 36 + 35, 2));
	EXPECT_TRUE(stIs("101", 2, 5, 3));
	EXPECT_TRUE(stIs("7", 7, 0, 0));
	EXPECT_TRUE(stIs("aA", 11, 10 * 11 + 10, 2));
	EXPECT_TRUE(stIs("bB", 11, 0, 0));
}

/* The first digit not below the base ends the subject sequence. */
TEST_OPT(invalid_digit_ends_the_subject, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(stIs("1238", 8, 83, 3));
	EXPECT_TRUE(stIs("12a", 10, 12, 2));
	EXPECT_TRUE(stIs("102", 2, 2, 2));
	EXPECT_TRUE(stIs("19z", 35, 1 * 35 + 9, 2));
	EXPECT_TRUE(stIs("1 2", 10, 1, 1));
	EXPECT_TRUE(stIs("4-2", 10, 4, 1));
}

/*
 * Every base from 2 to 36, values over the whole range (and their
 * negatives), lower and upper case letters, with and without 0x in base
 * 16 and through base 0: the value and the end of the text.
 */
TEST_OPT(every_base_round_trip, .ref = ST_REF_FORM)
{
	unsigned long long	x = 12345;
	int					bad = -1;

	for (int k = 0; k < 300 && bad < 0; k++)
	{
		ST_UT	mag;
		int		neg = k % 3 == 0;
		ST_T	want;

		x = x * 6364136223846793005ULL + 1442695040888963407ULL;
		mag = (ST_UT)x >> (k % 64);
		if (ST_SIGNED && mag > (ST_UT)ST_MAX)
			mag &= (ST_UT)ST_MAX;
#if ST_SIGNED
		want = neg ? (ST_T)-(ST_T)mag : (ST_T)mag;
#else
		want = neg ? (ST_T)((ST_UT)0 - mag) : (ST_T)mag;
#endif
		for (unsigned b = 2; b <= 36 && bad < 0; b++)
		{
			char	txt[80];

			(void)stFormat(txt, mag, neg, b, k % 2, "");
			if (!stIs(txt, (int)b, want, lc_strlen(txt)))
				bad = k * 100 + (int)b;
		}
		{
			char	h[80];
			char	o[80];
			char	d[80];

			(void)stFormat(h, mag, neg, 16, k % 2, k % 4 < 2 ? "0x" : "0X");
			(void)stFormat(o, mag, neg, 8, 0, "0");
			(void)stFormat(d, mag, neg, 10, 0, "");
			if (bad < 0 && (!stIs(h, 16, want, lc_strlen(h)) || !stIs(h, 0, want, lc_strlen(h))
				|| !stIs(o, 0, want, lc_strlen(o)) || !stIs(d, 0, want, lc_strlen(d))))
				bad = k * 100 + 99;
		}
	}
	EXPECT_EQ(bad, -1);
}

/* No digit at all: no conversion, 0, *endptr = nptr. */
TEST_OPT(no_conversion_returns_zero, .ref = ST_REF_RET)
{
	static const char	*s[] = {"", "   ", "-", "+", " x", "+-1", "-+1", "\t", ".5"};

	for (int i = 0; i < (int)(sizeof(s) / sizeof(s[0])); i++)
	{
		char	*end = NULL;

		EXPECT_TRUE(ST_FN(s[i], &end, 10) == 0);
		EXPECT_PTREQ(end, s[i]);
	}
}

/* "0x" (or "0X") without a hexadecimal digit: the subject sequence is "0". */
TEST_OPT(bare_hex_prefix_is_zero, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(stIs("0xg", 16, 0, 1));
	EXPECT_TRUE(stIs("0x", 16, 0, 1));
	EXPECT_TRUE(stIs("0X ", 0, 0, 1));
	EXPECT_TRUE(stIs("-0x", 16, 0, 2));
}

/* C23: digit separators are not part of the subject sequence (never were). */
TEST_OPT(digit_separators_are_not_accepted, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(stIs("1'000", 10, 1, 1));
	EXPECT_TRUE(stIs("0x1'0", 0, 1, 3));
}

TEST_OPT(endptr_may_be_null, .ref = ST_REF_FORM)
{
	EXPECT_TRUE(ST_FN("99", NULL, 10) == 99);
	EXPECT_TRUE(ST_FN("xyz", NULL, 10) == 0);
}

/* C23: 0b / 0B after the sign, in base 2 and base 0. */
TEST_OPT(binary_prefix_in_c23, .since = "C23", .ref = "C23 7.24.1.7p3")
{
	EXPECT_TRUE(stIs("0b101", 0, 5, 5));
	EXPECT_TRUE(stIs("0B11", 2, 3, 4));
	EXPECT_TRUE(stIs("-0b1", 0, (ST_T)-1, 4));
	EXPECT_TRUE(stIs("0b2", 0, 0, 1));
	EXPECT_TRUE(stIs("0b", 2, 0, 1));
}

/* Before C23, "0b101" is the subject sequence "0". */
TEST_OPT(no_binary_prefix_before_c23, .until = "C23", .ref = "C11 7.22.1.4p3")
{
	EXPECT_TRUE(stIs("0b101", 0, 0, 1));
	EXPECT_TRUE(stIs("0b101", 2, 0, 1));
}

/* ----- Range ----- */

/* The maximum, written in several bases: exact, errno untouched. */
TEST_OPT(maximum_value_exact, .ref = ST_REF_RET)
{
	char	txt[80];

	errno = 0;
	(void)stFormat(txt, (ST_UT)ST_MAX, 0, 10, 0, "");
	EXPECT_TRUE(stIs(txt, 10, ST_MAX, lc_strlen(txt)));
	(void)stFormat(txt, (ST_UT)ST_MAX, 0, 16, 1, "0x");
	EXPECT_TRUE(stIs(txt, 0, ST_MAX, lc_strlen(txt)));
	(void)stFormat(txt, (ST_UT)ST_MAX, 0, 36, 0, "");
	EXPECT_TRUE(stIs(txt, 36, ST_MAX, lc_strlen(txt)));
	(void)stFormat(txt, (ST_UT)ST_MAX, 0, 2, 0, "");
	EXPECT_TRUE(stIs(txt, 2, ST_MAX, lc_strlen(txt)));
	EXPECT_EQ(errno, 0);
}

/*
 * One more than the maximum, and far more: the maximum and ERANGE, with
 * *endptr past every digit.
 */
TEST_OPT(overflow_gives_max_and_erange, .ref = ST_REF_RET)
{
	char	txt[160];

	(void)stFormat(txt, (ST_UT)ST_MAX, 0, 10, 0, "");
	stIncrement(txt);
	errno = 0;
	EXPECT_TRUE(stIs(txt, 10, ST_MAX, lc_strlen(txt)));
	EXPECT_EQ(errno, ERANGE);
	lc_fill(txt, '9', 150);
	txt[150] = '\0';
	errno = 0;
	EXPECT_TRUE(stIs(txt, 10, ST_MAX, 150));
	EXPECT_EQ(errno, ERANGE);
	(void)stFormat(txt, (ST_UT)ST_MAX, 0, 16, 0, "0x");
	txt[lc_strlen(txt) + 1] = '\0';
	txt[lc_strlen(txt)] = 'f';
	errno = 0;
	EXPECT_TRUE(stIs(txt, 0, ST_MAX, lc_strlen(txt)));
	EXPECT_EQ(errno, ERANGE);
}

#if ST_SIGNED

/* The minimum, exact, errno untouched. */
TEST_OPT(minimum_value_exact, .ref = ST_REF_RET)
{
	char	txt[80];

	errno = 0;
	(void)stFormat(txt, (ST_UT)ST_MAX + 1, 1, 10, 0, "");
	EXPECT_TRUE(stIs(txt, 10, ST_MIN, lc_strlen(txt)));
	(void)stFormat(txt, (ST_UT)ST_MAX + 1, 1, 16, 0, "0x");
	EXPECT_TRUE(stIs(txt, 0, ST_MIN, lc_strlen(txt)));
	EXPECT_EQ(errno, 0);
}

/* One less than the minimum, and far less: the minimum and ERANGE. */
TEST_OPT(underflow_gives_min_and_erange, .ref = ST_REF_RET)
{
	char	txt[160];

	(void)stFormat(txt, (ST_UT)ST_MAX + 1, 1, 10, 0, "");
	stIncrement(txt);
	errno = 0;
	EXPECT_TRUE(stIs(txt, 10, ST_MIN, lc_strlen(txt)));
	EXPECT_EQ(errno, ERANGE);
	txt[0] = '-';
	lc_fill(txt + 1, '7', 150);
	txt[151] = '\0';
	errno = 0;
	EXPECT_TRUE(stIs(txt, 8, ST_MIN, 151));
	EXPECT_EQ(errno, ERANGE);
}

#else

/* Unsigned: a minus sign negates in the return type, not out of range. */
TEST_OPT(minus_sign_negates_in_unsigned, .ref = "C11 7.22.1.4p5")
{
	char	txt[80];

	errno = 0;
	EXPECT_TRUE(stIs("-1", 10, ST_MAX, 2));
	EXPECT_TRUE(stIs("-2", 10, ST_MAX - 1, 2));
	(void)stFormat(txt, (ST_UT)ST_MAX, 1, 10, 0, "");
	EXPECT_TRUE(stIs(txt, 10, 1, lc_strlen(txt)));
	EXPECT_EQ(errno, 0);
}

/* The negative of a value out of range: the maximum and ERANGE. */
TEST_OPT(negative_overflow_gives_max_and_erange, .ref = ST_REF_RET)
{
	char	txt[80];

	(void)stFormat(txt, (ST_UT)ST_MAX, 1, 10, 0, "");
	stIncrement(txt);
	errno = 0;
	EXPECT_TRUE(stIs(txt, 10, ST_MAX, lc_strlen(txt)));
	EXPECT_EQ(errno, ERANGE);
}

#endif

/* ----- POSIX ----- */

TEST_OPT(errno_unchanged_on_success, .since = ST_ERRNO_SINCE, .ref = "XSH strtol() DESCRIPTION")
{
	errno = ST_ERRNO_MARK;
	(void)ST_FN("123", NULL, 10);
	(void)ST_FN("-0x7f", NULL, 0);
	(void)ST_FN("zz", NULL, 36);
	EXPECT_EQ(errno, ST_ERRNO_MARK);
}

/* POSIX (CX): an unsupported base returns 0 and sets EINVAL. */
TEST_OPT(unsupported_base_fails_with_einval, .since = ST_EINVAL_SINCE,
	.ref = "XSH strtol() RETURN VALUE")
{
	static const int	bases[] = {1, 37, -1, 100};

	for (int i = 0; i < 4; i++)
	{
		errno = 0;
		EXPECT_TRUE(ST_FN("10", NULL, bases[i]) == 0);
		EXPECT_EQ(errno, EINVAL);
	}
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_TRUE(ST_FN("77", NULL, 8) == (ST_FN)("77", NULL, 8));
	EXPECT_TRUE(ST_FN("-z", NULL, 36) == (ST_FN)("-z", NULL, 36));
}

#undef ST_ERRNO_MARK
