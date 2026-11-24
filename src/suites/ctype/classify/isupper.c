/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isupper.c
 * @brief Tests for isupper() (<ctype.h>).
 * @Created: 2026/11/24 03:40:31 by Moutig
 * @Updated: 2026/11/24 03:54:30 by Moutig
 *
 * isupper() tests for the uppercase letters, or locale-specific
 * characters that are none of cntrl, digit, punct, space; in the "C"
 * locale, only 'A' ... 'Z' (C 7.4.1.11).
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.11; C23 7.4.2.11. POSIX: XSH isupper(), XBD 7.3.1.
 */
#include "ctype/ctype_cases.h"

CT_CLASSIFIER_TESTS(isupper, CT_UPPER, NULL,
	CT_R_UPPER | CT_R_CASE_ALPHA,
	CT_R_UPPER | CT_R_CASE_ALPHA,
	"C17 7.4.1.11")
