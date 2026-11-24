/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isalnum.c
 * @brief Tests for isalnum() (<ctype.h>).
 * @Created: 2026/11/24 03:35:29 by Moutig
 * @Updated: 2026/11/24 03:51:41 by Moutig
 *
 * isalnum() tests for any character for which isalpha() or isdigit() is
 * true (C 7.4.1.1); POSIX class alnum. Exact in the "C" locale since
 * alpha and digit are.
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.1; C23 7.4.2.1. POSIX: XSH isalnum(), XBD 7.3.1.
 */
#include "ctype_cases.h"

CT_CLASSIFIER_TESTS(isalnum, CT_ALNUM, NULL,
	CT_R_ALNUM | CT_R_ALNUM_PRINT,
	CT_R_ALNUM | CT_R_ALNUM_PRINT,
	"C17 7.4.1.1")
