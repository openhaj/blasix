/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <inttypes.h>: shared cases (bits/) in its context.
 * @Created: 2026/11/24 12:18:19 by Moutig
 * @Updated: 2026/11/24 12:20:02 by Moutig
 *
 * Every type and constant of <inttypes.h> in spec/inttypes.spec has its cases here:
 * the shared ones of src/suites/bits/ (the same for every header that
 * defines the symbol), then what only <inttypes.h> says. Each block is compiled
 * only when the target requires its symbol (-DLC_REQ_<name>, from lcrun)
 * and is reported under that symbol, not under "defs". Order: the header
 * under test and nothing else first; the bits/ files that include other
 * headers (BITS_INCLUDES_OTHER_HEADERS) last.
 *
 * <inttypes.h>: imaxdiv_t; intmax_t and uintmax_t, through <stdint.h>
 * which it includes; wchar_t (POSIX.1-2017 on, CX);
 * __STDC_VERSION_INTTYPES_H__ (C23); the PRI / SCN macros (7.8.1), last:
 * their cases need <stdio.h> and <wchar.h>.
 */

#include "framework/lc.h"
#include <inttypes.h>

/* ----- Shared cases (bits/): <inttypes.h> and nothing else is included ----- */

#include "bits/stdint/intmax_t.h"
#include "bits/stdint/uintmax_t.h"
#include "bits/stddef/wchar_t.h"
#include "bits/inttypes/imaxdiv_t.h"
#include "bits/version/__STDC_VERSION_INTTYPES_H__.h"

/* ----- What only <inttypes.h> says ----- */

# if defined(LC_REQ_intmax_t) || defined(LC_DEFS_ALL)
TEST_SYM(intmax_t__limits, "intmax_t", limits_of_at_least_64_bits, .ref = "C11 7.20.2.5p1")
{
	EXPECT_TRUE(INTMAX_MAX >= 9223372036854775807LL);
	EXPECT_TRUE(INTMAX_MIN <= -9223372036854775807LL);
}

TEST_SYM(intmax_t__functions, "intmax_t", is_the_type_of_the_functions, .ref = "C11 7.8.2.1p1")
{
	EXPECT_TYPE(&imaxabs, intmax_t (*)(intmax_t));
	EXPECT_TYPE(imaxabs(0), intmax_t);
}

/* <stdint.h> is included: its types and macros are all visible. */
TEST_SYM(intmax_t__stdint, "intmax_t", includes_stdint_h, .ref = "C11 7.8p1")
{
	int8_t			a = INT8_MAX;
	uint_least16_t	b = UINT_LEAST16_MAX;
	int_fast32_t	c = INT_FAST32_MIN;
	uintptr_t		d = UINTPTR_MAX;

	EXPECT_EQ(a, 127);
	EXPECT_TRUE(b >= 65535u);
	EXPECT_TRUE(c <= -2147483647L);
	EXPECT_TRUE(d > 0);
	EXPECT_TRUE(INTMAX_C(1) == 1 && SIZE_MAX > 0);
}
# endif

# if defined(LC_REQ_uintmax_t) || defined(LC_DEFS_ALL)
TEST_SYM(uintmax_t__limit, "uintmax_t", limit_of_at_least_64_bits, .ref = "C11 7.20.2.5p1")
{
	EXPECT_TRUE(UINTMAX_MAX >= 18446744073709551615ULL);
	EXPECT_TRUE((uintmax_t)-1 == UINTMAX_MAX);
}

TEST_SYM(uintmax_t__strtoumax, "uintmax_t", is_the_type_of_strtoumax, .ref = "C11 7.8.2.3p1")
{
	EXPECT_TYPE(strtoumax("1", (char **)0, 10), uintmax_t);
}
# endif

# if defined(LC_REQ_wchar_t) || defined(LC_DEFS_ALL)
TEST_SYM(wchar_t__wcstoimax, "wchar_t", is_the_element_type_of_wcstoimax, .ref = "XBD <inttypes.h>")
{
	EXPECT_TYPE(&wcstoimax, intmax_t (*)(const wchar_t *restrict, wchar_t **restrict, int));
}
# endif

/* ----- Last: bits/ files that include other headers ----- */

#include "bits/inttypes/pri.h"
#include "bits/inttypes/scn.h"
