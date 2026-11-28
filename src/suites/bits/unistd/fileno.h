/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file fileno.h
 * @brief Cases of STDIN_FILENO, STDOUT_FILENO and STDERR_FILENO.
 * @Created: 2026/11/28 09:35:07 by Moutig
 * @Updated: 2026/11/28 09:35:18 by Moutig
 *
 * The file numbers of stdin, stdout and stderr: 0, 1 and 2 (XBD
 * <unistd.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_FILENO_H
# define BITS_UNISTD_FILENO_H

# include "framework/lc.h"

# if defined(LC_REQ_STDIN_FILENO) || defined(LC_DEFS_ALL)
TEST_SYM(STDIN_FILENO__value, "STDIN_FILENO", is_0, .ref = "XBD <unistd.h>")
{
	EXPECT_EQ(STDIN_FILENO, 0);
}
# endif

# if defined(LC_REQ_STDOUT_FILENO) || defined(LC_DEFS_ALL)
TEST_SYM(STDOUT_FILENO__value, "STDOUT_FILENO", is_1, .ref = "XBD <unistd.h>")
{
	EXPECT_EQ(STDOUT_FILENO, 1);
}
# endif

# if defined(LC_REQ_STDERR_FILENO) || defined(LC_DEFS_ALL)
TEST_SYM(STDERR_FILENO__value, "STDERR_FILENO", is_2, .ref = "XBD <unistd.h>")
{
	EXPECT_EQ(STDERR_FILENO, 2);
}
# endif


#endif
