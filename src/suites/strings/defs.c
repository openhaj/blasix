/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <strings.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/24 11:09:35 by Moutig
 * @Updated: 2026/11/24 11:14:42 by Moutig
 *
 * Every type and constant of <strings.h> in spec/strings.spec has its cases here:
 * the shared ones of src/suites/bits/ (the same for every header that
 * defines the symbol), then what only <strings.h> says. Each block is compiled
 * only when the target requires its symbol (-DLC_REQ_<name>, from lcrun)
 * and is reported under that symbol, not under "defs". Order: the header
 * under test and nothing else first; the bits/ files that include other
 * headers (BITS_INCLUDES_OTHER_HEADERS) last.
 *
 * <strings.h>: size_t (XSI in POSIX.1-2001, base from POSIX.1-2008),
 * locale_t (POSIX.1-2008 on).
 */

#include "framework/lc.h"
#include <strings.h>

/* ----- Shared cases (bits/): <strings.h> and nothing else is included ----- */

#include "bits/stddef/size_t.h"
#include "bits/locale/locale_t.h"

/* ----- What only <strings.h> says ----- */

# if defined(LC_REQ_size_t) || defined(LC_DEFS_ALL)
TEST_SYM(size_t__strncasecmp, "size_t", is_the_length_parameter_of_strncasecmp, .ref = "XBD <strings.h>")
{
	EXPECT_TYPE(&strncasecmp, int (*)(const char *, const char *, size_t));
}
# endif

# if defined(LC_REQ_locale_t) || defined(LC_DEFS_ALL)
TEST_SYM(locale_t__strings_l, "locale_t", is_the_parameter_of_the_l_functions, .ref = "XBD <strings.h>")
{
	EXPECT_TYPE(&strcasecmp_l, int (*)(const char *, const char *, locale_t));
	EXPECT_TYPE(&strncasecmp_l, int (*)(const char *, const char *, size_t, locale_t));
}
# endif

/* ----- Last: bits/ files that include other headers ----- */

#include "bits/locale/locale_t_same.h"
