/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file atoi.c
 * @brief Tests for atoi() (<stdlib.h>).
 * @Created: 2026/11/27 12:45:01 by Moutig
 * @Updated: 2026/11/27 12:47:52 by Moutig
 *
 * atoi(nptr) converts the initial portion of the string to int; except
 * for the behaviour on error, it is (int)strtol(nptr, NULL, 10). A value
 * int cannot represent is undefined (not tested). The cases are those of
 * stdlib/ato_int.h. POSIX: the same ("the handling of errors may differ"),
 * no error defined.
 *
 * Clauses: C99 7.20.1.2; C11, C17 7.22.1.2; C23 7.24.1.2. POSIX: XSH
 * atoi().
 */
#include "stdlib/stdlib_cases.h"
#include <limits.h>

TEST_OPT(prototype, .ref = "C11 7.22.1.2p1")
{
	EXPECT_TYPE(&atoi, int (*)(const char *));
}

#define AT_FN	atoi
#define AT_T	int
#define AT_MIN	INT_MIN
#define AT_MAX	INT_MAX
#include "stdlib/ato_int.h"
