/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file EXIT_FAILURE.h
 * @brief Cases of the EXIT_FAILURE macro of <stdlib.h>.
 * @Created: 2026/11/25 12:43:27 by Moutig
 * @Updated: 2026/11/25 12:43:33 by Moutig
 *
 * EXIT_FAILURE expands to an integer constant expression, an argument of
 * exit() for an unsuccessful termination (C99 7.20p3; C11, C17 7.22p3;
 * C23 7.24p4). POSIX: nonzero (XBD <stdlib.h>, 2001 to 2017); between 1
 * and 125 inclusive (POSIX.1-2024, CX, Austin Group Defect 1229).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_EXIT_FAILURE, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDLIB_EXIT_FAILURE_H
# define BITS_STDLIB_EXIT_FAILURE_H

# include "framework/lc.h"

# if defined(LC_REQ_EXIT_FAILURE) || defined(LC_DEFS_ALL)

/* An integer constant expression: usable in an array size. */
TEST_SYM(EXIT_FAILURE__ice, "EXIT_FAILURE", integer_constant_expression, .ref = "C11 7.22p3")
{
	static const char	ok[(EXIT_FAILURE) * 0 + 1];

	EXPECT_UEQ(sizeof(ok), 1);
}

TEST_SYM(EXIT_FAILURE__nonzero, "EXIT_FAILURE", value_is_nonzero, .since = "POSIX.1-2001",
	.ref = "XBD <stdlib.h>")
{
	EXPECT_NE(EXIT_FAILURE, 0);
}

/* Not ambiguous for the shell (126 and up) nor truncated to 8 bits. */
TEST_SYM(EXIT_FAILURE__range, "EXIT_FAILURE", value_is_between_1_and_125,
	.since = "POSIX.1-2024", .ref = "XBD <stdlib.h> (Issue 8)")
{
	EXPECT_GE(EXIT_FAILURE, 1);
	EXPECT_LE(EXIT_FAILURE, 125);
}

# endif

#endif
