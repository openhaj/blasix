/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file iscntrl.c
 * @brief Tests for iscntrl() (<ctype.h>).
 * @Created: 2026/11/24 03:36:45 by Moutig
 * @Updated: 2026/11/24 03:53:06 by Moutig
 *
 * iscntrl() tests for control characters, a locale-specific set of
 * characters that are not printing characters (C 7.4p3, 7.4.1.4). ISO C
 * does not list them; the POSIX locale does (XBD 7.3.1.1: NUL, alert,
 * backspace, tab, newline, vertical-tab, form-feed, carriage-return,
 * SOH ... US, DEL), hence the exact case since POSIX.1-2001.
 *
 * The cases common to every classification function come from
 * ctype_cases.h (CT_CLASSIFIER_TESTS): prototype, EOF, the classes of
 * the "C" / POSIX locale, ASCII codes, macro and function forms, and the
 * relations of ISO C in the "C" locale and in every other installed
 * locale.
 *
 * Clauses: C99, C11, C17 7.4.1.4; C23 7.4.2.4. POSIX: XSH iscntrl(), XBD 7.3.1.
 */
#include "ctype_cases.h"

CT_CLASSIFIER_TESTS(iscntrl, CT_CNTRL, "POSIX.1-2001",
	CT_R_CNTRL,
	CT_R_CNTRL,
	"C17 7.4.1.4")
