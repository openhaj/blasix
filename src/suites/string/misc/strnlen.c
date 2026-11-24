/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strnlen.c
 * @brief Tests for strnlen() (<string.h>).
 * @Created: 2026/11/24 05:24:10 by Moutig
 * @Updated: 2026/11/24 06:13:09 by Moutig
 *
 * strnlen() (POSIX.1-2008) returns the number of bytes before the first
 * NUL of s if there is one within the first maxlen bytes, otherwise
 * maxlen, and never examines more than maxlen bytes.
 *
 * POSIX: XSH strlen() (strnlen, CX); POSIX.1-2024: errno unchanged on
 * valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strnlen() SYNOPSIS")
{
	EXPECT_TYPE(&strnlen, size_t (*)(const char *, size_t));
}

TEST_OPT(length_when_shorter, .ref = "POSIX strnlen() RETURN VALUE")
{
	EXPECT_UEQ(strnlen("abc", 10), 3);
	EXPECT_UEQ(strnlen("abc", 4), 3);
	EXPECT_UEQ(strnlen("", 5), 0);
}

TEST_OPT(maxlen_when_longer, .ref = "POSIX strnlen() RETURN VALUE")
{
	EXPECT_UEQ(strnlen("abcdef", 3), 3);
	EXPECT_UEQ(strnlen("abc", 3), 3);
}

TEST_OPT(zero_maxlen_is_zero, .ref = "POSIX strnlen() RETURN VALUE")
{
	EXPECT_UEQ(strnlen("abc", 0), 0);
}

TEST_OPT(never_examines_more_than_maxlen, .ref = "POSIX strnlen() DESCRIPTION")
{
	int	bad = -1;

	for (int len = 0; len <= 40 && bad < 0; len++)
	{
		char	*s = lc_guarded((lc_size_t)len);

		if (!s)
			SKIP("no guarded mapping");
		for (int i = 0; i < len; i++)
			s[i] = 'a';
		if (strnlen(s, (size_t)len) != (size_t)len)
			bad = len;
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strnlen() SYNOPSIS")
{
	EXPECT_UEQ(strnlen("abc", 2), (strnlen)("abc", 2));
}

ST_ERRNO_KEPT("POSIX strnlen()",
	(void)strnlen("abc", 2);)
