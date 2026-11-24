/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strerrorlen_s.c
 * @brief Tests for strerrorlen_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:21:04 by Moutig
 * @Updated: 2026/11/24 05:59:28 by Moutig
 *
 * strerrorlen_s() returns the length of the untruncated message that
 * strerror_s() maps errnum to (without the null character).
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.4.3; C23 K.3.7.4.3.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.4.3p1")
{
	EXPECT_TYPE(&strerrorlen_s, size_t (*)(errno_t));
}

TEST_OPT(length_of_strerror_s_message, .ref = "C11 K.3.7.4.3p2-3")
{
	static const int	e[] = {EDOM, ERANGE, EILSEQ};
	char				buf[512];

	ST_EXT1_IGNORE();
	for (int i = 0; i < 3; i++)
	{
		ASSERT_EQ(strerror_s(buf, sizeof(buf), e[i]), 0);
		EXPECT_UEQ(strerrorlen_s(e[i]), strlen(buf));
	}
}

BITS_LIB_EXT1_TESTS
