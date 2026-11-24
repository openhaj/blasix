/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ispunct.c
 * @brief Tests for ispunct() (<ctype.h>).
 * @Created: 2026/11/24 03:39:53 by Moutig
 * @Updated: 2026/11/24 03:54:09 by Moutig
 *
 * ispunct() tests for printing characters that are neither space nor
 * alnum, from a locale-specific set; in the "C" locale, every such
 * printing character (C 7.4.1.9). In the POSIX locale: the 32 characters
 * of XBD 7.3.1.1.
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.9; C23 7.4.2.9. POSIX: XSH ispunct(), XBD 7.3.1.
 */
#include "ctype_cases.h"

CT_CLASSIFIER_TESTS(ispunct, CT_PUNCT, "POSIX.1-2001",
	CT_R_PUNCT | CT_R_PUNCT_C,
	CT_R_PUNCT,
	"C17 7.4.1.9")
