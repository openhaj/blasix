/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file islower.c
 * @brief Tests for islower() (<ctype.h>).
 * @Created: 2026/11/24 03:37:54 by Moutig
 * @Updated: 2026/11/24 03:53:51 by Moutig
 *
 * islower() tests for the lowercase letters, or locale-specific
 * characters that are none of cntrl, digit, punct, space; in the "C"
 * locale, only 'a' ... 'z' (C 7.4.1.7).
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.7; C23 7.4.2.7. POSIX: XSH islower(), XBD 7.3.1.
 */
#include "ctype/ctype_cases.h"

CT_CLASSIFIER_TESTS(islower, CT_LOWER, NULL,
	CT_R_LOWER | CT_R_CASE_ALPHA,
	CT_R_LOWER | CT_R_CASE_ALPHA,
	"C17 7.4.1.7")
