/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strxfrm.c
 * @brief Tests for strxfrm() (<string.h>).
 * @Created: 2026/11/24 05:25:37 by Moutig
 * @Updated: 2026/11/24 06:14:01 by Moutig
 *
 * strxfrm() transforms s2 into s1 so that strcmp() of two images orders
 * like strcoll() of the originals; at most n characters, terminating null
 * included, are written; it returns the length of the image (without the
 * null character). With n = 0, s1 may be a null pointer. A result of n or
 * more leaves s1 indeterminate (not checked).
 *
 * Clauses: C99 7.21.4.5; C11, C17 7.24.4.5; C23 7.26.4.5. POSIX: XSH
 * strxfrm() (errno unchanged on success, CX).
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.4.5p1")
{
	EXPECT_TYPE(&strxfrm, size_t (*)(char *restrict, const char *restrict, size_t));
}

TEST_OPT(returns_length_of_image, .ref = "C11 7.24.4.5p3")
{
	char	buf[64];
	size_t	n = strxfrm(buf, "hello", sizeof(buf));

	ASSERT_LT(n, sizeof(buf));
	EXPECT_UEQ(strlen(buf), n);
}

TEST_OPT(null_destination_with_zero_size, .ref = "C11 7.24.4.5p2")
{
	char	buf[64];
	size_t	n = strxfrm(buf, "hello", sizeof(buf));

	EXPECT_UEQ(strxfrm(NULL, "hello", 0), n);
}

/* No more than n characters are written when the image fits. */
TEST_OPT(writes_at_most_n, .ref = "C11 7.24.4.5p2")
{
	char	buf[80];
	size_t	n = strxfrm(NULL, "abc", 0);

	ASSERT_LT(n, 60);
	lc_fill(buf, 'x', sizeof(buf));
	EXPECT_UEQ(strxfrm(buf, "abc", n + 1), n);
	EXPECT_EQ(buf[n], '\0');
	for (size_t i = n + 1; i < sizeof(buf); i++)
		EXPECT_EQ(buf[i], 'x');
}

TEST_OPT(images_order_like_strcoll, .ref = "C11 7.24.4.5p2")
{
	static const char	*s[] = {"", "a", "ab", "b", "B", "0", "a b", "~"};
	char				x[8][64];

	for (int i = 0; i < 8; i++)
		ASSERT_LT(strxfrm(x[i], s[i], sizeof(x[i])), sizeof(x[i]));
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			EXPECT_EQ(st_sign(strcmp(x[i], x[j])), st_sign(strcoll(s[i], s[j])));
}

TEST_OPT(empty_string, .ref = "C11 7.24.4.5p3")
{
	char	buf[16];
	size_t	n = strxfrm(buf, "", sizeof(buf));

	ASSERT_LT(n, sizeof(buf));
	EXPECT_UEQ(strlen(buf), n);
}

TEST_OPT(errno_unchanged_on_success, .since = "POSIX.1-2001",
	.ref = "POSIX strxfrm() DESCRIPTION")
{
	char	buf[64];

	errno = ST_ERRNO_MARK;
	(void)strxfrm(buf, "abc", sizeof(buf));
	EXPECT_EQ(errno, ST_ERRNO_MARK);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_UEQ(strxfrm(NULL, "abc", 0), (strxfrm)(NULL, "abc", 0));
}
