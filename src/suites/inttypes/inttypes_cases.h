/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file inttypes_cases.h
 * @brief Shared helpers and cases of the <inttypes.h> units.
 * @Created: 2026/11/24 12:18:28 by Moutig
 * @Updated: 2026/11/24 12:18:57 by Moutig
 *
 * What the function units of <inttypes.h> (func/) need: the converter
 * of bits/inttypes/convert.h (the text of a value, without the libc),
 * the wide helpers of bits/inttypes/wide.h, and the errno mark of the
 * strto* / wcsto* cases. The PRI / SCN macros and the types are
 * definitions: their cases are in bits/inttypes/, run by defs.c.
 *
 * <stddef.h> is included for wchar_t (C 7.8.2.4: "#include <stddef.h>
 * // for wchar_t"); <errno.h> and <limits.h> prepare data. Only
 * <inttypes.h> is under test.
 */

#ifndef INTTYPES_CASES_H
# define INTTYPES_CASES_H

# include "framework/lc.h"
# include <inttypes.h>
# include <stddef.h>
# include <errno.h>
# include <limits.h>
# include "bits/inttypes/convert.h"
# include "bits/inttypes/wide.h"

/** errno before a successful call: a value no valid call would set. */
# define IT_ERRNO_MARK	EDOM

#endif
