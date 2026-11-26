/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ONCE_FLAG_INIT.h
 * @brief Cases of the ONCE_FLAG_INIT macro, for every header that defines it.
 * @Created: 2026/11/25 12:47:09 by Moutig
 * @Updated: 2026/11/25 12:47:22 by Moutig
 *
 * ONCE_FLAG_INIT expands to a value that can be used to initialize an
 * object of type once_flag (C11, C17 7.26.1p3 <threads.h>; C23 7.28.1p3,
 * and C23 7.24p4 <stdlib.h>).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_ONCE_FLAG_INIT, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_THREADS_ONCE_FLAG_INIT_H
# define BITS_THREADS_ONCE_FLAG_INIT_H

# include "framework/lc.h"

# if defined(LC_REQ_ONCE_FLAG_INIT) || defined(LC_DEFS_ALL)

TEST_SYM(ONCE_FLAG_INIT__defined, "ONCE_FLAG_INIT", is_defined, .ref = "C23 7.28.1p3")
{
#  ifdef ONCE_FLAG_INIT
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}

/* Static and automatic objects: an initializer is enough. */
TEST_SYM(ONCE_FLAG_INIT__initializes, "ONCE_FLAG_INIT", initializes_a_once_flag,
	.ref = "C23 7.28.1p3")
{
#  ifdef ONCE_FLAG_INIT
	static once_flag	s = ONCE_FLAG_INIT;
	once_flag			a = ONCE_FLAG_INIT;

	(void)a;
	EXPECT_TYPE(s, once_flag);
#  else
	SKIP("not defined (see is_defined)");
#  endif
}

# endif

#endif
