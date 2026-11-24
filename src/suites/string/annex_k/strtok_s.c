/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strtok_s.c
 * @brief Tests for strtok_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:25:06 by Moutig
 * @Updated: 2026/11/24 06:13:42 by Moutig
 *
 * strtok_s() splits like strtok() with its state in *ptr and the length
 * left in *s1max. Violations (s1max, s2 or ptr null; s1 and *ptr both
 * null; *s1max > RSIZE_MAX; token end beyond *s1max) return a null
 * pointer without storing into *ptr.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.3.1 (the EXAMPLE is checked as given); C23
 * K.3.7.3.1.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.3.1p1")
{
	EXPECT_TYPE(&strtok_s, char *(*)(char *restrict, rsize_t *restrict, const char *restrict, char **restrict));
}

TEST_OPT(standard_example, .ref = "C11 K.3.7.3.1p10")
{
	static char	str1[] = "?a???b,,,#c";
	static char	str2[] = "\t \t";
	char		*ptr1;
	char		*ptr2;
	rsize_t		max1 = sizeof(str1);
	rsize_t		max2 = sizeof(str2);

	ST_EXT1_IGNORE();
	EXPECT_PTREQ(strtok_s(str1, &max1, "?", &ptr1), str1 + 1);
	EXPECT_PTREQ(strtok_s(NULL, &max1, ",", &ptr1), str1 + 3);
	EXPECT_NULL(strtok_s(str2, &max2, " \t", &ptr2));
	EXPECT_PTREQ(strtok_s(NULL, &max1, "#,", &ptr1), str1 + 10);
	EXPECT_NULL(strtok_s(NULL, &max1, "?", &ptr1));
}

TEST_OPT(null_state_fails, .ref = "C11 K.3.7.3.1p2-3")
{
	char	s[] = "a b";
	rsize_t	max = sizeof(s);

	ST_EXT1_IGNORE();
	EXPECT_NULL(strtok_s(s, &max, " ", NULL));
}

TEST_OPT(token_beyond_s1max_fails, .ref = "C11 K.3.7.3.1p2")
{
	char	s[] = "abcdef";
	char	*ptr = (char *)1;
	rsize_t	max = 3;

	ST_EXT1_IGNORE();
	EXPECT_NULL(strtok_s(s, &max, " ", &ptr));
	EXPECT_PTREQ(ptr, (char *)1);
}

BITS_LIB_EXT1_TESTS
