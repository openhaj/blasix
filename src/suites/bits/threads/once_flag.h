/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file once_flag.h
 * @brief Cases of the once_flag type, for every header that defines it.
 * @Created: 2026/11/25 12:47:27 by Moutig
 * @Updated: 2026/11/25 12:47:55 by Moutig
 *
 * once_flag is a complete object type that holds a flag for use by
 * call_once (C11, C17 7.26.1p4 <threads.h>; C23 7.28.1p4, and C23 7.24p3
 * <stdlib.h>).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_once_flag, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_THREADS_ONCE_FLAG_H
# define BITS_THREADS_ONCE_FLAG_H

# include "framework/lc.h"

# if defined(LC_REQ_once_flag) || defined(LC_DEFS_ALL)

TEST_SYM(once_flag__declared, "once_flag", declared_by_the_header, .ref = "C23 7.24p3")
{
	static once_flag	f;

	EXPECT_TYPE(f, once_flag);
}

/* A complete object type: sizeof and objects of the type. */
TEST_SYM(once_flag__complete, "once_flag", is_a_complete_object_type, .ref = "C23 7.28.1p4")
{
	static once_flag	a[2];

	EXPECT_UGT(sizeof(once_flag), 0);
	EXPECT_UEQ(sizeof(a), 2 * sizeof(once_flag));
}

# endif

#endif
