/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strtol.c
 * @brief Tests for strtol() (<stdlib.h>).
 * @Created: 2026/11/27 12:19:43 by Moutig
 * @Updated: 2026/11/27 12:20:32 by Moutig
 *
 * strtol() converts the initial portion of a string to long int: white
 * space, an optional sign, digits in base 2 ... 36 or base 0; *endptr
 * past the subject sequence. Out of range: LONG_MIN or LONG_MAX and ERANGE.
 * The cases common to the family are in stdlib/strto_int.h. POSIX:
 * errno is kept on success (CX, POSIX.1-2001
 * on); an unsupported base "shall" give EINVAL from POSIX.1-2017 ("may"
 * before).
 *
 * Clauses: C99 7.20.1.4; C11, C17 7.22.1.4; C23 7.24.1.7. POSIX: XSH
 * strtol().
 */
#include "stdlib/stdlib_cases.h"
#include <limits.h>

TEST_OPT(prototype, .ref = "C11 7.22.1.4p1")
{
	EXPECT_TYPE(&strtol, long (*)(const char *restrict, char **restrict, int));
}

#define ST_FN				strtol
#define ST_T				long
#define ST_UT				unsigned long long
#define ST_SIGNED			1
#define ST_MIN				LONG_MIN
#define ST_MAX				LONG_MAX
#define ST_REF_FORM			"C11 7.22.1.4p2-5"
#define ST_REF_RET			"C11 7.22.1.4p7-8"
#define ST_ERRNO_SINCE		"POSIX.1-2001"
#define ST_EINVAL_SINCE		"POSIX.1-2017"
#include "stdlib/strto_int.h"
