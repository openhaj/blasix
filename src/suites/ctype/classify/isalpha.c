/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isalpha.c
 * @brief Tests for isalpha() (<ctype.h>).
 * @Created: 2026/11/24 03:35:56 by Moutig
 * @Updated: 2026/11/24 03:51:57 by Moutig
 *
 * isalpha() tests for upper and lower letters, or locale-specific
 * alphabetic characters that are none of cntrl, digit, punct, space;
 * in the "C" locale, only the letters (C 7.4.1.2).
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.2; C23 7.4.2.2. POSIX: XSH isalpha(), XBD 7.3.1.
 */
#include "ctype/ctype_cases.h"

CT_CLASSIFIER_TESTS(isalpha, CT_ALPHA, NULL,
	CT_R_ALPHA | CT_R_CASE_ALPHA,
	CT_R_ALPHA | CT_R_CASE_ALPHA,
	"C17 7.4.1.2")
