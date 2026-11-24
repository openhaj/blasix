/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isspace.c
 * @brief Tests for isspace() (<ctype.h>).
 * @Created: 2026/11/24 03:40:13 by Moutig
 * @Updated: 2026/11/24 03:54:17 by Moutig
 *
 * isspace() tests for the standard white-space characters (space, \f,
 * \n, \r, \t, \v) or locale-specific characters for which isalnum() is
 * false; in the "C" locale, only the standard ones (C 7.4.1.10).
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.10; C23 7.4.2.10. POSIX: XSH isspace(), XBD 7.3.1.
 */
#include "ctype/ctype_cases.h"

CT_CLASSIFIER_TESTS(isspace, CT_SPACE, NULL,
	CT_R_SPACE,
	CT_R_SPACE,
	"C17 7.4.1.10")
