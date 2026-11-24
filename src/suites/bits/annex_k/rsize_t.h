/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file rsize_t.h
 * @brief Cases of rsize_t (Annex K), for every header that defines it.
 * @Created: 2026/11/24 07:37:21 by Moutig
 * @Updated: 2026/11/24 07:37:21 by Moutig
 *
 * Annex K: rsize_t "which is the type size_t" (K.3.3 <stddef.h>; K.3.5
 * <stdio.h>, K.3.6 <stdlib.h>, K.3.7 <string.h>, K.3.8 <time.h>, K.3.9
 * <wchar.h>), declared with __STDC_WANT_LIB_EXT1__ 1. RSIZE_MAX belongs to
 * <stdint.h> (K.3.4).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_rsize_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_ANNEX_K_RSIZE_T_H
# define BITS_ANNEX_K_RSIZE_T_H

# include "framework/lc.h"

# if defined(LC_REQ_rsize_t) || defined(LC_DEFS_ALL)

TEST_SYM(rsize_t__declared, "rsize_t", declared_by_the_header, .ref = "C11 K.3.3p2")
{
	rsize_t	n = 0;

	EXPECT_UEQ(n, 0);
}

TEST_SYM(rsize_t__size_t, "rsize_t", is_size_t, .ref = "C11 K.3.3p2")
{
	rsize_t	n = 0;

	EXPECT_TYPE(n, __typeof__(sizeof(int)));
	EXPECT_TRUE((rsize_t)-1 > 0);
}

# endif

#endif
