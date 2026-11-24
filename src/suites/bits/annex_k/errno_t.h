/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file errno_t.h
 * @brief Cases of errno_t (Annex K), for every header that defines it.
 * @Created: 2026/11/24 07:37:21 by Moutig
 * @Updated: 2026/11/24 07:37:21 by Moutig
 *
 * Annex K: errno_t "which is type int" (K.3.2 <errno.h>; K.3.5 <stdio.h>,
 * K.3.6 <stdlib.h>, K.3.7 <string.h>, K.3.8 <time.h>, K.3.9 <wchar.h>),
 * declared with __STDC_WANT_LIB_EXT1__ 1 (the target defines it with
 * OPTS=LIB_EXT1).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_errno_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_ANNEX_K_ERRNO_T_H
# define BITS_ANNEX_K_ERRNO_T_H

# include "framework/lc.h"

# if defined(LC_REQ_errno_t) || defined(LC_DEFS_ALL)

TEST_SYM(errno_t__declared, "errno_t", declared_by_the_header, .ref = "C11 K.3.2p2")
{
	errno_t	e = 0;

	EXPECT_TRUE(e == 0);
}

TEST_SYM(errno_t__int, "errno_t", is_int, .ref = "C11 K.3.2p2")
{
	errno_t	e = 0;

	EXPECT_TYPE(e, int);
}

# endif

#endif
