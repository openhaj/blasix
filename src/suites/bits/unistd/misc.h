/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file misc.h
 * @brief Cases of POSIX_CLOSE_RESTART and _POSIX_VDISABLE of <unistd.h>.
 * @Created: 2026/11/28 09:35:34 by Moutig
 * @Updated: 2026/11/28 09:36:34 by Moutig
 *
 * POSIX_CLOSE_RESTART (POSIX.1-2024) is the flag of posix_close();
 * _POSIX_VDISABLE is "the value of a character" that disables a terminal
 * special character (XBD <unistd.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_MISC_H
# define BITS_UNISTD_MISC_H

# include "framework/lc.h"

# if defined(LC_REQ_POSIX_CLOSE_RESTART) || defined(LC_DEFS_ALL)
/* A flag of posix_close(): a non-zero int (0 is "no flag"). */
TEST_SYM(POSIX_CLOSE_RESTART__flag, "POSIX_CLOSE_RESTART", is_a_nonzero_flag,
	.ref = "XBD <unistd.h> (Issue 8)")
{
	int	f = POSIX_CLOSE_RESTART;

	EXPECT_NE(f, 0);
}
# endif

# if defined(LC_REQ__POSIX_VDISABLE) || defined(LC_DEFS_ALL)
/* The value of a character (c_cc[] of <termios.h>): it fits in an unsigned char. */
TEST_SYM(_POSIX_VDISABLE__char, "_POSIX_VDISABLE", is_the_value_of_a_character, .ref = "XBD <unistd.h>")
{
	EXPECT_TRUE((unsigned char)_POSIX_VDISABLE == (_POSIX_VDISABLE & 0xff));
}
# endif

#endif
