/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <ctype.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/24 07:38:34 by Moutig
 * @Updated: 2026/11/24 07:38:34 by Moutig
 *
 * Every type and constant of <ctype.h> in spec/ctype.spec has its cases here:
 * the shared ones of src/suites/bits/ (the same for every header that
 * defines the symbol), then what only <ctype.h> says. Each block is compiled
 * only when the target requires its symbol (-DLC_REQ_<name>, from lcrun)
 * and is reported under that symbol, not under "defs". Order: the header
 * under test and nothing else first; the bits/ files that include other
 * headers (BITS_INCLUDES_OTHER_HEADERS) last.
 *
 * <ctype.h>: locale_t (POSIX.1-2008 on, "as described in <locale.h>").
 */

#include "framework/lc.h"
#include <ctype.h>

/* ----- Shared cases (bits/): <ctype.h> and nothing else is included ----- */

#include "bits/locale/locale_t.h"

/* ----- What only <ctype.h> says ----- */

# if defined(LC_REQ_locale_t) || defined(LC_DEFS_ALL)
/* The locale parameter of the *_l functions. */
TEST_SYM(locale_t__ctype_l, "locale_t", is_the_parameter_of_the_l_functions, .ref = "XBD <ctype.h>")
{
	EXPECT_TYPE(&isalpha_l, int (*)(int, locale_t));
	EXPECT_TYPE(&tolower_l, int (*)(int, locale_t));
}
# endif

/* ----- Last: bits/ files that include other headers ----- */

#include "bits/locale/locale_t_same.h"
