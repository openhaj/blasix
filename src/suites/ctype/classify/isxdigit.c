/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isxdigit.c
 * @brief Tests for isxdigit() (<ctype.h>).
 * @Created: 2026/11/24 03:40:52 by Moutig
 * @Updated: 2026/11/24 03:54:40 by Moutig
 *
 * isxdigit() tests for the hexadecimal digits 0-9, A-F, a-f (C 7.4.1.12,
 * 6.4.4.1), in every locale (XBD 7.3.1 xdigit: "In all locales, only").
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.12; C23 7.4.2.12. POSIX: XSH isxdigit(), XBD 7.3.1.
 */
#include "ctype/ctype_cases.h"

CT_CLASSIFIER_TESTS(isxdigit, CT_XDIGIT, NULL,
	CT_R_XDIGIT,
	CT_R_XDIGIT,
	"C17 7.4.1.12")
