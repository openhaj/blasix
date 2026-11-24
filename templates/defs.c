/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) @YEAR@ @AUTHOR@ <@EMAIL@>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file defs.c
 * @brief Types and constants of <@HEADER@>: shared cases (bits/) in its context.
 * @Created: @DATE@ by @AUTHOR@
 * @Updated: @DATE@ by @AUTHOR@
 *
 * Every type and constant of <@HEADER@> in spec/@CAT@.spec has its cases
 * here: the shared ones of src/suites/bits/ (the same for every header
 * that defines the symbol), then what only <@HEADER@> says. Each block is
 * compiled only when the target requires its symbol and the header
 * declares it (-DLC_REQ_<name>) and is reported under that symbol.
 * Order: the header under test and nothing else first; the bits/ files
 * that include other headers (BITS_INCLUDES_OTHER_HEADERS) last.
 */

#include "framework/lc.h"
#include <@HEADER@>

/* ----- Shared cases (bits/): <@HEADER@> and nothing else is included ----- */

/* #include "bits/stddef/size_t.h" */

/* ----- What only <@HEADER@> says ----- */

/*
# if defined(LC_REQ_size_t) || defined(LC_DEFS_ALL)
TEST_SYM(size_t__<what>, "size_t", <requirement>, .ref = "<clause>")
{
}
# endif
*/

/* ----- Last: bits/ files that include other headers ----- */

/* #include "bits/locale/locale_t_same.h" */
