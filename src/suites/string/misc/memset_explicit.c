/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memset_explicit.c
 * @brief Tests for memset_explicit() (<string.h>).
 * @Created: 2026/11/24 05:17:44 by Moutig
 * @Updated: 2026/11/24 05:45:41 by Moutig
 *
 * memset_explicit() (C23) copies c, converted to unsigned char, into the
 * first n characters of s and returns s, like memset(); the store is meant
 * never to be elided (7.26.6.2p2, footnote), which a test cannot observe.
 *
 * Clause: C23 7.26.6.2.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C23 7.26.6.2p1")
{
	EXPECT_TYPE(&memset_explicit, void *(*)(void *, int, size_t));
}

TEST_OPT(fills_n_bytes_and_returns_s, .ref = "C23 7.26.6.2p2-3")
{
	char	buf[6] = "abcde";

	EXPECT_PTREQ(memset_explicit(buf, 'z', 3), buf);
	EXPECT_STREQ(buf, "zzzde");
}

TEST_OPT(value_converted_to_unsigned_char, .ref = "C23 7.26.6.2p2")
{
	unsigned char	buf[2] = {0, 0};

	(void)memset_explicit(buf, 0x141, 1);
	(void)memset_explicit(buf + 1, -1, 1);
	EXPECT_UEQ(buf[0], 0x41);
	EXPECT_UEQ(buf[1], UCHAR_MAX);
}

TEST_OPT(zero_length_writes_nothing, .ref = "C23 7.26.1p3")
{
	char	buf[4] = "abc";

	EXPECT_PTREQ(memset_explicit(buf, 'z', 0), buf);
	EXPECT_STREQ(buf, "abc");
}

/* A store just before the object dies is still made (observable here). */
TEST_OPT(clears_a_secret_before_reuse, .ref = "C23 7.26.6.2p2")
{
	volatile unsigned char	*p;
	unsigned char			secret[16];

	for (int i = 0; i < 16; i++)
		secret[i] = (unsigned char)(i + 1);
	(void)memset_explicit(secret, 0, sizeof(secret));
	p = secret;
	for (int i = 0; i < 16; i++)
		EXPECT_UEQ(p[i], 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C23 7.1.4p1")
{
	char	a[4] = "abc";
	char	b[4] = "abc";

	(void)memset_explicit(a, 'q', 2);
	(void)(memset_explicit)(b, 'q', 2);
	EXPECT_STREQ(a, b);
}
