/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isblank.c
 * @brief Tests for isblank() (<ctype.h>).
 * @Created: 2026/11/24 03:36:26 by Moutig
 * @Updated: 2026/11/24 03:52:32 by Moutig
 *
 * isblank() tests for the standard blank characters (space, tab) or
 * locale-specific space characters that separate words in a line; in the
 * "C" locale, only space and tab (C 7.4.1.3, new in C99).
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.3; C23 7.4.2.3. POSIX: XSH isblank(), XBD 7.3.1.
 */
#include "ctype/ctype_cases.h"

CT_CLASSIFIER_TESTS(isblank, CT_BLANK, NULL,
	CT_R_BLANK,
	CT_R_BLANK,
	"C17 7.4.1.3")
