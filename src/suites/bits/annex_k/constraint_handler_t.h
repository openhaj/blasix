/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file constraint_handler_t.h
 * @brief Cases of constraint_handler_t (Annex K) of <stdlib.h>.
 * @Created: 2026/11/25 12:39:31 by Moutig
 * @Updated: 2026/11/25 12:39:38 by Moutig
 *
 * Annex K.3.6p2: constraint_handler_t is
 *     typedef void (*constraint_handler_t)(const char * restrict msg,
 *         void * restrict ptr, errno_t error);
 * declared with __STDC_WANT_LIB_EXT1__ 1 (the target defines it with
 * OPTS=LIB_EXT1).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_constraint_handler_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_ANNEX_K_CONSTRAINT_HANDLER_T_H
# define BITS_ANNEX_K_CONSTRAINT_HANDLER_T_H

# include "framework/lc.h"

# if defined(LC_REQ_constraint_handler_t) || defined(LC_DEFS_ALL)

static void	bitsNoHandler(const char *restrict msg, void *restrict ptr, errno_t error)
{
	(void)msg;
	(void)ptr;
	(void)error;
}

TEST_SYM(constraint_handler_t__definition, "constraint_handler_t",
	is_a_pointer_to_the_handler_function, .ref = "C11 K.3.6p2")
{
	constraint_handler_t	h = bitsNoHandler;

	EXPECT_TYPE(h, void (*)(const char *restrict, void *restrict, errno_t));
	EXPECT_TRUE(h == bitsNoHandler);
}

# endif

#endif
