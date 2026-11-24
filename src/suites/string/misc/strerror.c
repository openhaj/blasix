/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strerror.c
 * @brief Tests for strerror() (<string.h>).
 * @Created: 2026/11/24 05:20:55 by Moutig
 * @Updated: 2026/11/24 05:58:51 by Moutig
 *
 * strerror() maps errnum to a message string and returns a pointer to it.
 * C23 and POSIX.1-2024 say "any value of type int" (POSIX.1-2017: a
 * pointer is returned "whether successful or not"); POSIX (CX) adds that
 * errno is unchanged on success. The text itself is locale-specific and
 * not checked, only that it is a non-empty string for a valid number.
 *
 * Clauses: C99 7.21.6.2; C11, C17 7.24.6.2; C23 7.26.6.3. POSIX: XSH
 * strerror().
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.6.2p1")
{
	EXPECT_TYPE(&strerror, char *(*)(int));
}

TEST_OPT(message_for_iso_c_errors, .ref = "C11 7.24.6.2p2-4")
{
	static const int	e[] = {EDOM, ERANGE, EILSEQ};

	for (int i = 0; i < 3; i++)
	{
		const char	*m = strerror(e[i]);

		ASSERT_NOTNULL(m);
		EXPECT_GT(strlen(m), 0);
	}
}

/* Different errors have different messages (they say what occurred). */
TEST_OPT(valid_errors_have_distinct_messages, .since = "POSIX.1-2008",
	.ref = "POSIX strerror() DESCRIPTION")
{
	char		a[256];
	const char	*m = strerror(EDOM);
	lc_size_t	i;

	/* Copied by hand: the next call may overwrite the first string. */
	ASSERT_NOTNULL(m);
	for (i = 0; m[i] && i < sizeof(a) - 1; i++)
		a[i] = m[i];
	a[i] = '\0';
	EXPECT_STRNE(a, strerror(ERANGE));
}

TEST_OPT(any_int_maps_to_a_message_c23, .since = "C23", .ref = "C23 7.26.6.3p2")
{
	static const int	e[] = {0, -1, INT_MIN, INT_MAX, 12345};

	for (int i = 0; i < 5; i++)
		EXPECT_NOTNULL(strerror(e[i]));
}

TEST_OPT(any_int_maps_to_a_message_posix, .since = "POSIX.1-2017",
	.ref = "POSIX strerror() RETURN VALUE")
{
	static const int	e[] = {0, -1, INT_MIN, INT_MAX, 12345};

	for (int i = 0; i < 5; i++)
		EXPECT_NOTNULL(strerror(e[i]));
}

TEST_OPT(errno_unchanged_on_success, .since = "POSIX.1-2001",
	.ref = "POSIX strerror() DESCRIPTION")
{
	errno = 0;
	(void)strerror(ERANGE);
	EXPECT_EQ(errno, 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_NOTNULL((strerror)(EDOM));
}
