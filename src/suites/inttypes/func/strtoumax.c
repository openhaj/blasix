/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strtoumax.c
 * @brief Tests for strtoumax() (<inttypes.h>).
 * @Created: 2026/11/24 12:17:51 by Moutig
 * @Updated: 2026/11/24 12:19:50 by Moutig
 *
 * strtoumax() is strtoul() / strtoull(), converting to uintmax_t: optional white
 * space, sign, then digits in base 2 ... 36 (letters for 10 ... 35) or base
 * 0 (0x / 0 prefix, decimal otherwise); *endptr gets the end of the subject
 * sequence (nptr when there is none, and 0 is returned). An out-of-range
 * value gives UINTMAX_MAX and ERANGE in errno.
 *
 * Edition specifics: C23 accepts the 0b / 0B prefix (base 0 or 2), older
 * editions stop before the b. POSIX: errno unchanged on success (as
 * strtol(), CX, POSIX.1-2001 on); an unsupported base returns 0 with errno
 * EINVAL ("shall" from POSIX.1-2017, "may" before).
 *
 * Clauses: C99, C11, C17 7.8.2.3; C23 7.8.2.3. POSIX: XSH strtoumax().
 */
#include "inttypes/inttypes_cases.h"

TEST_OPT(prototype, .ref = "C11 7.8.2.3p1")
{
	EXPECT_TYPE(&strtoumax, uintmax_t (*)(const char *restrict, char **restrict, int));
}

TEST_OPT(decimal_and_end_pointer, .ref = "C11 7.8.2.3p2")
{
	static const char	s[] = "12345x";
	char				*end;

	EXPECT_TRUE(strtoumax(s, &end, 10) == 12345);
	EXPECT_PTREQ(end, s + 5);
}

TEST_OPT(leading_space_and_sign, .ref = "C11 7.8.2.3p2")
{
	static const char	s[] = " \t\n+42;";
	char				*end;

	EXPECT_TRUE(strtoumax(s, &end, 10) == 42);
	EXPECT_PTREQ(end, s + 6);
}

TEST_OPT(hexadecimal_prefix, .ref = "C11 7.8.2.3p2")
{
	static const char	s[] = "0x1Fg";
	char				*end;

	EXPECT_TRUE(strtoumax(s, &end, 16) == 31);
	EXPECT_PTREQ(end, s + 4);
	EXPECT_TRUE(strtoumax(s, &end, 0) == 31);
	EXPECT_TRUE(strtoumax("0XfF", NULL, 0) == 255);
}

TEST_OPT(base_zero_octal_and_decimal, .ref = "C11 7.8.2.3p2")
{
	EXPECT_TRUE(strtoumax("017", NULL, 0) == 15);
	EXPECT_TRUE(strtoumax("17", NULL, 0) == 17);
	EXPECT_TRUE(strtoumax("0", NULL, 0) == 0);
	EXPECT_TRUE(strtoumax("019", NULL, 0) == 1);
}

TEST_OPT(letters_up_to_base_36, .ref = "C11 7.8.2.3p2")
{
	EXPECT_TRUE(strtoumax("zZ", NULL, 36) == 35 * 36 + 35);
	EXPECT_TRUE(strtoumax("101", NULL, 2) == 5);
	EXPECT_TRUE(strtoumax("7", NULL, 7) == 0);
}

TEST_OPT(no_conversion_returns_zero, .ref = "C11 7.8.2.3p3")
{
	static const char	a[] = "  xyz";
	static const char	b[] = "-";
	static const char	c[] = "";
	char				*end;

	EXPECT_TRUE(strtoumax(a, &end, 10) == 0);
	EXPECT_PTREQ(end, a);
	EXPECT_TRUE(strtoumax(b, &end, 0) == 0);
	EXPECT_PTREQ(end, b);
	EXPECT_TRUE(strtoumax(c, &end, 10) == 0);
	EXPECT_PTREQ(end, c);
}

/* "0x" without hexadecimal digit: the subject sequence is "0". */
TEST_OPT(bare_hex_prefix_is_zero, .ref = "C11 7.8.2.3p2")
{
	static const char	s[] = "0xg";
	char				*end;

	EXPECT_TRUE(strtoumax(s, &end, 16) == 0);
	EXPECT_PTREQ(end, s + 1);
}

TEST_OPT(maximum_value_exact, .ref = "C11 7.8.2.3p3")
{
	char	txt[80];
	char	buf[80];
	char	*s;

	(void)it_format(txt, (uintmax_t)UINTMAX_MAX, 0, 10, 0);
	s = txt;
	errno = 0;
	EXPECT_TRUE(strtoumax(s, NULL, 10) == UINTMAX_MAX);
	EXPECT_EQ(errno, 0);
	(void)buf;
}

TEST_OPT(overflow_gives_max_and_erange, .ref = "C11 7.8.2.3p3")
{
	char	txt[80];
	char	buf[80];
	char	*s;
	char	*end;

	(void)it_format(txt, (uintmax_t)UINTMAX_MAX, 0, 10, 0);
	txt[lc_strlen(txt) + 1] = '\0';
	txt[lc_strlen(txt)] = '0';
	s = txt;
	errno = 0;
	EXPECT_TRUE(strtoumax(s, &end, 10) == UINTMAX_MAX);
	EXPECT_EQ(errno, ERANGE);
	EXPECT_EQ(*end, 0);
	(void)buf;
}

TEST_OPT(endptr_may_be_null, .ref = "C11 7.8.2.3p2")
{
	EXPECT_TRUE(strtoumax("99", NULL, 10) == 99);
}

TEST_OPT(errno_unchanged_on_success, .since = "POSIX.1-2001",
	.ref = "POSIX strtol() DESCRIPTION")
{
	errno = IT_ERRNO_MARK;
	(void)strtoumax("123", NULL, 10);
	EXPECT_EQ(errno, IT_ERRNO_MARK);
}

TEST_OPT(unsupported_base_fails_with_einval, .since = "POSIX.1-2017",
	.ref = "POSIX strtoumax() RETURN VALUE")
{
	errno = 0;
	EXPECT_TRUE(strtoumax("10", NULL, 1) == 0);
	EXPECT_EQ(errno, EINVAL);
	errno = 0;
	EXPECT_TRUE(strtoumax("10", NULL, 37) == 0);
	EXPECT_EQ(errno, EINVAL);
}

TEST_OPT(binary_prefix_in_c23, .since = "C23", .ref = "C23 7.24.1.7p3 (strtol)")
{
	static const char	s[] = "0b101";
	char				*end;

	EXPECT_TRUE(strtoumax(s, &end, 0) == 5);
	EXPECT_PTREQ(end, s + 5);
	EXPECT_TRUE(strtoumax("0B11", NULL, 2) == 3);
}

/* Before C23, "0b101" with base 0 is the subject sequence "0". */
TEST_OPT(no_binary_prefix_before_c23, .until = "C23", .ref = "C11 7.22.1.4p3 (strtol)")
{
	static const char	s[] = "0b101";
	char				*end;

	EXPECT_TRUE(strtoumax(s, &end, 0) == 0);
	EXPECT_PTREQ(end, s + 1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_TRUE(strtoumax("77", NULL, 8) == (strtoumax)("77", NULL, 8));
}

/* A minus sign negates the value in the return type (strtoul()). */
TEST_OPT(minus_sign_negates_in_unsigned, .ref = "C11 7.22.1.4p5 (strtoul)")
{
	errno = 0;
	EXPECT_TRUE(strtoumax("-1", NULL, 10) == UINTMAX_MAX);
	EXPECT_TRUE(strtoumax("-2", NULL, 10) == UINTMAX_MAX - 1);
	EXPECT_EQ(errno, 0);
}

TEST_OPT(negative_overflow_gives_max_and_erange, .ref = "C11 7.8.2.3p3")
{
	char	txt[80];
	char	buf[80];
	char	*s;

	txt[0] = '-';
	(void)it_format(txt + 1, UINTMAX_MAX, 0, 10, 0);
	txt[lc_strlen(txt) + 1] = '\0';
	txt[lc_strlen(txt)] = '0';
	s = txt;
	errno = 0;
	EXPECT_TRUE(strtoumax(s, NULL, 10) == UINTMAX_MAX);
	EXPECT_EQ(errno, ERANGE);
	(void)buf;
}
