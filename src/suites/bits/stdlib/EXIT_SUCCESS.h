/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file EXIT_SUCCESS.h
 * @brief Cases of the EXIT_SUCCESS macro of <stdlib.h>.
 * @Created: 2026/11/25 12:43:39 by Moutig
 * @Updated: 2026/11/25 12:43:46 by Moutig
 *
 * EXIT_SUCCESS expands to an integer constant expression, an argument of
 * exit() for a successful termination (C99 7.20p3; C11, C17 7.22p3; C23
 * 7.24p4). POSIX: its value is 0 (XBD <stdlib.h>: "evaluates to 0" from
 * 2001, "the value shall be 0" in 2024).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_EXIT_SUCCESS, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDLIB_EXIT_SUCCESS_H
# define BITS_STDLIB_EXIT_SUCCESS_H

# include "framework/lc.h"

# if defined(LC_REQ_EXIT_SUCCESS) || defined(LC_DEFS_ALL)

/* An integer constant expression: usable in an array size. */
TEST_SYM(EXIT_SUCCESS__ice, "EXIT_SUCCESS", integer_constant_expression, .ref = "C11 7.22p3")
{
	static const char	ok[(EXIT_SUCCESS) * 0 + 1];

	EXPECT_UEQ(sizeof(ok), 1);
}

TEST_SYM(EXIT_SUCCESS__zero, "EXIT_SUCCESS", value_is_0, .since = "POSIX.1-2001",
	.ref = "XBD <stdlib.h>")
{
	EXPECT_EQ(EXIT_SUCCESS, 0);
}

# endif

#endif
