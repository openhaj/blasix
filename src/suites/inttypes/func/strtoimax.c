/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strtoimax.c
 * @brief Tests for strtoimax() (<inttypes.h>).
 * @Created: 2026/11/24 12:17:43 by Moutig
 * @Updated: 2026/11/27 12:19:22 by Moutig
 *
 * strtoimax() is strtol() / strtoll(), converting to intmax_t: optional white
 * space, sign, then digits in base 2 ... 36 (letters for 10 ... 35) or base
 * 0 (0x / 0 prefix, decimal otherwise); *endptr gets the end of the subject
 * sequence (nptr when there is none, and 0 is returned). An out-of-range
 * value gives INTMAX_MAX or INTMAX_MIN and ERANGE in errno.
 *
 * Edition specifics: C23 accepts the 0b / 0B prefix (base 0 or 2), older
 * editions stop before the b. POSIX: errno unchanged on success (as
 * strtol(), CX, POSIX.1-2001 on); an unsupported base returns 0 with errno
 * EINVAL ("shall" from POSIX.1-2017, "may" before).
 *
 * The cases common to the strtol() family are in stdlib/strto_int.h (a
 * template that includes no header of the libc besides <errno.h>).
 *
 * Clauses: C99, C11, C17 7.8.2.3; C23 7.8.2.3. POSIX: XSH strtoimax().
 */
#include "inttypes/inttypes_cases.h"

TEST_OPT(prototype, .ref = "C11 7.8.2.3p1")
{
	EXPECT_TYPE(&strtoimax, intmax_t (*)(const char *restrict, char **restrict, int));
}

#define ST_FN				strtoimax
#define ST_T				intmax_t
#define ST_UT				uintmax_t
#define ST_SIGNED			1
#define ST_MIN				INTMAX_MIN
#define ST_MAX				INTMAX_MAX
#define ST_REF_FORM			"C11 7.8.2.3p2"
#define ST_REF_RET			"C11 7.8.2.3p3"
#define ST_ERRNO_SINCE		"POSIX.1-2001"
#define ST_EINVAL_SINCE		"POSIX.1-2017"
#include "stdlib/strto_int.h"
