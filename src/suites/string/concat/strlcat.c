/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strlcat.c
 * @brief Tests for strlcat() (<string.h>).
 * @Created: 2026/11/24 05:21:40 by Moutig
 * @Updated: 2026/11/24 06:10:02 by Moutig
 *
 * strlcat() (POSIX.1-2024) appends at most dstsize - strlen(dst) - 1
 * bytes of src to the string dst, NUL-terminating unless the NUL would be
 * at or beyond dst + dstsize. It returns strnlen(dst, dstsize) +
 * strlen(src), as computed before the call.
 *
 * POSIX: XSH strlcpy() (strlcat, CX, Issue 8); errno unchanged on valid
 * input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strlcat() SYNOPSIS")
{
	EXPECT_TYPE(&strlcat, size_t (*)(char *restrict, const char *restrict, size_t));
}

TEST_OPT(appends_when_it_fits, .ref = "POSIX strlcat() DESCRIPTION")
{
	char	dst[8] = "ab\0xxxx";

	EXPECT_UEQ(strlcat(dst, "cd", sizeof(dst)), 4);
	EXPECT_MEMEQ(dst, "abcd\0xx", 8);
}

TEST_OPT(truncates_and_terminates, .ref = "POSIX strlcat() DESCRIPTION")
{
	char	dst[8] = "ab\0xxxx";

	EXPECT_UEQ(strlcat(dst, "cdefgh", 5), 8);
	EXPECT_MEMEQ(dst, "abcd\0xx", 8);
}

/* dst already full: nothing appended, no NUL written beyond dstsize. */
TEST_OPT(full_destination_appends_nothing, .ref = "POSIX strlcat() DESCRIPTION")
{
	char	dst[8] = "abc\0xxx";

	EXPECT_UEQ(strlcat(dst, "de", 4), 5);
	EXPECT_MEMEQ(dst, "abc\0xxx", 8);
}

/* No NUL within dstsize: the result is dstsize + strlen(src). */
TEST_OPT(unterminated_within_size, .ref = "POSIX strlcat() RETURN VALUE")
{
	char	dst[8] = "abcdef";

	EXPECT_UEQ(strlcat(dst, "xy", 3), 5);
	EXPECT_STREQ(dst, "abcdef");
}

TEST_OPT(zero_size_writes_nothing, .ref = "POSIX strlcat() RETURN VALUE")
{
	char	dst[4] = "ab";

	EXPECT_UEQ(strlcat(dst, "cd", 0), 2);
	EXPECT_STREQ(dst, "ab");
}

TEST_OPT(empty_source, .ref = "POSIX strlcat() RETURN VALUE")
{
	char	dst[4] = "ab";

	EXPECT_UEQ(strlcat(dst, "", sizeof(dst)), 2);
	EXPECT_STREQ(dst, "ab");
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strlcat() SYNOPSIS")
{
	char	a[6] = "ab";
	char	b[6] = "ab";

	EXPECT_UEQ(strlcat(a, "cdefg", 5), (strlcat)(b, "cdefg", 5));
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX strlcat()",
	char d[4] = "a";
	(void)strlcat(d, "bcdef", sizeof(d));)
