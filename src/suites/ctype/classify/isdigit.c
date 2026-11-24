/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isdigit.c
 * @brief Tests for isdigit() (<ctype.h>).
 * @Created: 2026/11/24 03:37:12 by Moutig
 * @Updated: 2026/11/24 03:53:28 by Moutig
 *
 * isdigit() tests for the decimal digits '0' ... '9' (C 7.4.1.5, 5.2.1),
 * in every locale (also XBD 7.3.1 digit: "In all locales, only").
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.5; C23 7.4.2.5. POSIX: XSH isdigit(), XBD 7.3.1.
 */
#include "ctype/ctype_cases.h"

CT_CLASSIFIER_TESTS(isdigit, CT_DIGIT, NULL,
	CT_R_DIGIT,
	CT_R_DIGIT,
	"C17 7.4.1.5")
