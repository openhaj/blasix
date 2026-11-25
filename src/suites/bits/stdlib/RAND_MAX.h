/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file RAND_MAX.h
 * @brief Cases of the RAND_MAX macro of <stdlib.h>.
 * @Created: 2026/11/25 12:45:41 by Moutig
 * @Updated: 2026/11/25 12:46:43 by Moutig
 *
 * RAND_MAX expands to an integer constant expression, the maximum value
 * rand() returns (C99 7.20p3; C11, C17 7.22p3; C23 7.24p4); it is at
 * least 32767 (C99 7.20.2.1p5; C11, C17 7.22.2.1p5; C23 7.24.2.1p5).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_RAND_MAX, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDLIB_RAND_MAX_H
# define BITS_STDLIB_RAND_MAX_H

# include "framework/lc.h"

# if defined(LC_REQ_RAND_MAX) || defined(LC_DEFS_ALL)

/* An integer constant expression: usable in an array size. */
TEST_SYM(RAND_MAX__ice, "RAND_MAX", integer_constant_expression, .ref = "C11 7.22p3")
{
	static const char	ok[(RAND_MAX) * 0 + 1];

	EXPECT_UEQ(sizeof(ok), 1);
}

TEST_SYM(RAND_MAX__minimum, "RAND_MAX", at_least_32767, .ref = "C11 7.22.2.1p5")
{
	EXPECT_GE(RAND_MAX, 32767);
}

# endif

#endif
