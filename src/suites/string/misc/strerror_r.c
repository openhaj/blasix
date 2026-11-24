/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strerror_r.c
 * @brief Tests for strerror_r() (<string.h>).
 * @Created: 2026/11/24 05:20:37 by Moutig
 * @Updated: 2026/11/24 05:57:15 by Moutig
 *
 * strerror_r() writes the message of errnum into strerrbuf (buflen bytes)
 * and returns 0, or an error number. ERANGE when the buffer is too small:
 * "may fail" up to POSIX.1-2017, "shall fail" in POSIX.1-2024. This is
 * the POSIX (int) version: no _GNU_SOURCE is defined by the build.
 *
 * POSIX: XSH strerror() (strerror_r: TSF in POSIX.1-2001, base from
 * POSIX.1-2008).
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strerror_r() SYNOPSIS")
{
	EXPECT_TYPE(&strerror_r, int (*)(int, char *, size_t));
}

TEST_OPT(writes_message_and_returns_zero, .ref = "POSIX strerror_r() RETURN VALUE")
{
	char	buf[256];

	lc_fill(buf, 'x', sizeof(buf));
	EXPECT_EQ(strerror_r(EDOM, buf, sizeof(buf)), 0);
	EXPECT_EQ(buf[sizeof(buf) - 1], 'x');
	EXPECT_GT(strlen(buf), 0);
}

/* A buffer of exactly the message length plus one is enough. */
TEST_OPT(exact_size_is_enough, .ref = "POSIX strerror_r() DESCRIPTION")
{
	char	full[256];
	char	buf[256];
	size_t	len;

	ASSERT_EQ(strerror_r(ERANGE, full, sizeof(full)), 0);
	len = strlen(full);
	EXPECT_EQ(strerror_r(ERANGE, buf, len + 1), 0);
	EXPECT_STREQ(buf, full);
}

TEST_OPT(small_buffer_fails_with_erange, .since = "POSIX.1-2024",
	.ref = "POSIX strerror_r() ERRORS (Issue 8: shall fail)")
{
	char	full[256];
	char	buf[256];

	ASSERT_EQ(strerror_r(EDOM, full, sizeof(full)), 0);
	ASSERT_GT(strlen(full), 1);
	EXPECT_EQ(strerror_r(EDOM, buf, 2), ERANGE);
}

/* An unknown number still succeeds (unknown error) or fails with EINVAL. */
TEST_OPT(unknown_error_number, .ref = "POSIX strerror_r() ERRORS")
{
	char	buf[256];
	int		r = strerror_r(INT_MAX, buf, sizeof(buf));

	EXPECT_TRUE(r == 0 || r == EINVAL);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strerror_r() SYNOPSIS")
{
	char	a[256];
	char	b[256];

	EXPECT_EQ(strerror_r(EDOM, a, sizeof(a)), 0);
	EXPECT_EQ((strerror_r)(EDOM, b, sizeof(b)), 0);
	EXPECT_STREQ(a, b);
}
