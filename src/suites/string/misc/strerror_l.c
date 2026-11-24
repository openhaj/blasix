/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strerror_l.c
 * @brief Tests for strerror_l() (<string.h>).
 * @Created: 2026/11/24 05:20:25 by Moutig
 * @Updated: 2026/11/24 05:56:49 by Moutig
 *
 * strerror_l() (POSIX.1-2008) is strerror() with the message in the
 * locale represented by its locale_t argument. For an invalid errnum
 * errno may be set to EINVAL but a string is still returned; errno is
 * unchanged on success. Undefined for LC_GLOBAL_LOCALE or an invalid
 * handle (not tested).
 *
 * POSIX: XSH strerror() (strerror_l, CX). The locale object comes from
 * newlocale() of <locale.h> (skipped without it).
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strerror_l() SYNOPSIS")
{
	EXPECT_TYPE(&strerror_l, char *(*)(int, locale_t));
}

TEST_OPT(message_for_valid_errors, .ref = "POSIX strerror_l() RETURN VALUE")
{
	static const int	e[] = {EDOM, ERANGE, EINVAL, ENOENT};

	ST_POSIX_LOCALE(loc);
	for (int i = 0; i < 4; i++)
	{
		const char	*m = strerror_l(e[i], loc);

		ASSERT_NOTNULL(m);
		EXPECT_GT(strlen(m), 0);
	}
	ST_FREE_LOCALE(loc);
}

TEST_OPT(invalid_error_still_gives_a_string, .ref = "POSIX strerror_l() RETURN VALUE")
{
	ST_POSIX_LOCALE(loc);
	EXPECT_NOTNULL(strerror_l(INT_MAX, loc));
	EXPECT_NOTNULL(strerror_l(-1, loc));
	ST_FREE_LOCALE(loc);
}

TEST_OPT(errno_unchanged_on_success, .ref = "POSIX strerror_l() DESCRIPTION")
{
	ST_POSIX_LOCALE(loc);
	errno = 0;
	(void)strerror_l(ERANGE, loc);
	EXPECT_EQ(errno, 0);
	ST_FREE_LOCALE(loc);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strerror_l() SYNOPSIS")
{
	ST_POSIX_LOCALE(loc);
	EXPECT_NOTNULL((strerror_l)(EDOM, loc));
	ST_FREE_LOCALE(loc);
}
