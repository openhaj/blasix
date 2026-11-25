/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file MB_CUR_MAX.h
 * @brief Cases of the MB_CUR_MAX macro of <stdlib.h>.
 * @Created: 2026/11/25 12:45:27 by Moutig
 * @Updated: 2026/11/25 12:45:32 by Moutig
 *
 * MB_CUR_MAX expands to a positive integer expression with type size_t,
 * the maximum number of bytes in a multibyte character of the current
 * locale (C99 7.20p3; C11, C17 7.22p3; C23 7.24p4). It need not be a
 * constant. POSIX.1-2017 on: 1 in the POSIX locale, the locale a program
 * starts in (CX).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_MB_CUR_MAX, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDLIB_MB_CUR_MAX_H
# define BITS_STDLIB_MB_CUR_MAX_H

# include "framework/lc.h"

# if defined(LC_REQ_MB_CUR_MAX) || defined(LC_DEFS_ALL)

TEST_SYM(MB_CUR_MAX__size_t, "MB_CUR_MAX", has_type_size_t, .ref = "C11 7.22p3")
{
	EXPECT_TYPE(MB_CUR_MAX, size_t);
}

TEST_SYM(MB_CUR_MAX__positive, "MB_CUR_MAX", is_positive, .ref = "C11 7.22p3")
{
	EXPECT_UGT(MB_CUR_MAX, 0);
}

/* A program starts in the "C" (POSIX) locale (C11 7.11.1.1p4). */
TEST_SYM(MB_CUR_MAX__posix_locale, "MB_CUR_MAX", is_1_in_the_posix_locale,
	.since = "POSIX.1-2017", .ref = "XBD <stdlib.h>")
{
	EXPECT_UEQ(MB_CUR_MAX, 1);
}

# endif

#endif
