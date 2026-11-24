/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isalpha_l.c
 * @brief Tests for isalpha_l() (<ctype.h>).
 * @Created: 2026/11/24 03:35:46 by Moutig
 * @Updated: 2026/11/24 03:51:50 by Moutig
 *
 * isalpha_l() is isalpha() in the locale represented by its locale_t
 * argument instead of the current locale (XSH isalpha(), POSIX.1-2008 on).
 * The behaviour is undefined for LC_GLOBAL_LOCALE or an invalid handle
 * (not tested; POSIX.1-2008 and 2017 only said it "may fail" with EINVAL).
 *
 * The cases come from ctype_cases.h (CT_LOCALE_TESTS): prototype, EOF,
 * a "POSIX" locale object classifies as the "C" locale, the object is
 * used whatever the global locale is, and an object of another installed
 * locale classifies as isalpha() does under that locale. They need
 * newlocale() and freelocale() of <locale.h>.
 */
#include "ctype/ctype_cases.h"

CT_LOCALE_TESTS(isalpha_l, isalpha, CT_TRUTH, "POSIX isalpha_l()")
