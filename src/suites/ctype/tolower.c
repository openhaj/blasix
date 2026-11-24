/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file tolower.c
 * @brief Tests for tolower() (<ctype.h>).
 * @Created: 2026/11/24 03:41:31 by Moutig
 * @Updated: 2026/11/24 03:55:04 by Moutig
 *
 * tolower() returns the lowercase letter corresponding to an uppercase
 * letter (one of them, always the same, if the locale has several), and
 * any other argument unchanged (C 7.4.2.1). In the "C" / POSIX locale:
 * 'A' ... 'Z' to 'a' ... 'z' (XBD 7.3.1.1 tolower).
 *
 * The cases come from ctype_cases.h (CT_CASEMAP_TESTS): prototype, EOF,
 * the 26 letters and every other value of the "C" locale, macro and
 * function forms, the relation of ISO C in every installed locale.
 *
 * Clauses: C99, C11, C17 7.4.2.1; C23 7.4.3.1. POSIX: XSH tolower().
 */
#include "ctype_cases.h"

CT_CASEMAP_TESTS(tolower, CT_UPPER_SET, CT_LOWER_SET, CT_R_TOLOWER, "C17 7.4.2.1")
