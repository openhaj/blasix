/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strerror_s.c
 * @brief Tests for strerror_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:20:46 by Moutig
 * @Updated: 2026/11/24 05:57:58 by Moutig
 *
 * strerror_s() copies the message of errnum (any int) into s. If it does
 * not fit, maxsize - 1 characters are copied and terminated, the last
 * three ending with "..." when maxsize > 3, and the result is nonzero.
 * Violations (s null, maxsize 0 or > RSIZE_MAX) return nonzero.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.4.2; C23 K.3.7.4.2.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.4.2p1")
{
	EXPECT_TYPE(&strerror_s, errno_t (*)(char *, rsize_t, errno_t));
}

TEST_OPT(full_message_returns_zero, .ref = "C11 K.3.7.4.2p5-7")
{
	char	buf[256];

	ST_EXT1_IGNORE();
	EXPECT_EQ(strerror_s(buf, sizeof(buf), EDOM), 0);
	EXPECT_UEQ(strlen(buf), strerrorlen_s(EDOM));
	EXPECT_GT(strlen(buf), 0);
}

TEST_OPT(truncated_message_ends_with_dots, .ref = "C11 K.3.7.4.2p6-7")
{
	char	buf[6];

	ST_EXT1_IGNORE();
	ASSERT_GT(strerrorlen_s(EDOM), 5);
	EXPECT_NE(strerror_s(buf, sizeof(buf), EDOM), 0);
	EXPECT_EQ(buf[5], '\0');
	EXPECT_MEMEQ(buf + 2, "...", 3);
}

TEST_OPT(any_int_maps_to_a_message, .ref = "C11 K.3.7.4.2p4")
{
	char	buf[256];

	ST_EXT1_IGNORE();
	EXPECT_EQ(strerror_s(buf, sizeof(buf), INT_MAX), 0);
	EXPECT_EQ(strerror_s(buf, sizeof(buf), -1), 0);
}

TEST_OPT(null_buffer_fails, .ref = "C11 K.3.7.4.2p2")
{
	ST_EXT1_IGNORE();
	EXPECT_NE(strerror_s(NULL, 10, EDOM), 0);
}

BITS_LIB_EXT1_TESTS
