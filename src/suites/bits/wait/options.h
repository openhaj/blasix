/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file options.h
 * @brief Cases of WNOHANG and WUNTRACED, for every header that defines them.
 * @Created: 2026/11/25 12:49:39 by Moutig
 * @Updated: 2026/11/25 12:49:57 by Moutig
 *
 * WNOHANG ("do not hang if no status is available") and WUNTRACED
 * ("report status of stopped child process") are symbolic constants for
 * the options argument of waitpid() (XBD <sys/wait.h>); <stdlib.h>
 * defines them as described there (XSI in POSIX.1-2001, CX from 2008).
 * Each asks for a behaviour of its own: neither is 0 (no option), and
 * they differ. What they do is tested with waitpid().
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_WNOHANG, -DLC_REQ_WUNTRACED, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_WAIT_OPTIONS_H
# define BITS_WAIT_OPTIONS_H

# include "framework/lc.h"

# if defined(LC_REQ_WNOHANG) || defined(LC_DEFS_ALL)

TEST_SYM(WNOHANG__option, "WNOHANG", is_a_nonzero_int_option, .ref = "XBD <sys/wait.h>")
{
	int	o = WNOHANG;

	EXPECT_NE(o, 0);
}

# endif

# if defined(LC_REQ_WUNTRACED) || defined(LC_DEFS_ALL)

TEST_SYM(WUNTRACED__option, "WUNTRACED", is_a_nonzero_int_option, .ref = "XBD <sys/wait.h>")
{
	int	o = WUNTRACED;

	EXPECT_NE(o, 0);
}

#  if defined(LC_REQ_WNOHANG) || defined(LC_DEFS_ALL)
TEST_SYM(WUNTRACED__distinct, "WUNTRACED", differs_from_WNOHANG, .ref = "XBD <sys/wait.h>")
{
	EXPECT_NE(WUNTRACED, WNOHANG);
	EXPECT_NE(WUNTRACED | WNOHANG, WNOHANG);
}
#  endif

# endif

#endif
