/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file toupper.c
 * @brief Tests for toupper() (<ctype.h>).
 * @Created: 2026/11/24 03:41:49 by Moutig
 * @Updated: 2026/11/24 03:55:16 by Moutig
 *
 * toupper() returns the uppercase letter corresponding to a lowercase
 * letter, and any other argument unchanged (C 7.4.2.2). In the "C" /
 * POSIX locale: 'a' ... 'z' to 'A' ... 'Z' (XBD 7.3.1.1 toupper).
 *
 * The cases come from ctype_cases.h (CT_CASEMAP_TESTS): prototype, EOF,
 * the 26 letters and every other value of the "C" locale, macro and
 * function forms, the relation of ISO C in every installed locale.
 *
 * Clauses: C99, C11, C17 7.4.2.2; C23 7.4.3.2. POSIX: XSH toupper().
 */
#include "ctype_cases.h"

CT_CASEMAP_TESTS(toupper, CT_LOWER_SET, CT_UPPER_SET, CT_R_TOUPPER, "C17 7.4.2.2")
