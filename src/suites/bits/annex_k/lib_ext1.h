/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file lib_ext1.h
 * @brief __STDC_LIB_EXT1__ cases, ending every Annex K function unit.
 * @Created: 2026/11/24 07:37:21 by Moutig
 * @Updated: 2026/11/24 07:37:21 by Moutig
 *
 * An implementation that provides Annex K defines __STDC_LIB_EXT1__ (C11
 * 6.10.8.3, K.2p2) with the value of the edition: 201112L (C11; the N1548
 * draft of data/ says 201ymmL), 201710L (C17), 202311L (C23). Each Annex K
 * unit, whatever its header, ends with BITS_LIB_EXT1_TESTS: it only builds
 * when its interface is declared, so the macro must be there. A generator
 * (not a self-guarded file): the unit decides.
 */

#ifndef BITS_ANNEX_K_LIB_EXT1_H
# define BITS_ANNEX_K_LIB_EXT1_H

# include "framework/lc.h"

# ifdef __STDC_LIB_EXT1__
#  define BITS_HAS_LIB_EXT1		1
#  define BITS_LIB_EXT1_VALUE	((long)__STDC_LIB_EXT1__)
# else
#  define BITS_HAS_LIB_EXT1		0
#  define BITS_LIB_EXT1_VALUE	0L
# endif

# define BITS_LIB_EXT1_TESTS												\
	TEST_OPT(stdc_lib_ext1_defined, .since = "C11", .ref = "C11 6.10.8.3, K.2p2")	\
	{																		\
		EXPECT_TRUE(BITS_HAS_LIB_EXT1);										\
	}																		\
																			\
	TEST_OPT(stdc_lib_ext1_value_c11, .since = "C11", .until = "C17",		\
		.ref = "C11 6.10.8.3")												\
	{																		\
		EXPECT_EQ(BITS_LIB_EXT1_VALUE, 201112L);							\
	}																		\
																			\
	TEST_OPT(stdc_lib_ext1_value_c17, .since = "C17", .until = "C23",		\
		.ref = "C17 6.10.8.3")												\
	{																		\
		EXPECT_EQ(BITS_LIB_EXT1_VALUE, 201710L);							\
	}																		\
																			\
	TEST_OPT(stdc_lib_ext1_value_c23, .since = "C23", .ref = "C23 6.10.10.4")	\
	{																		\
		EXPECT_EQ(BITS_LIB_EXT1_VALUE, 202311L);							\
	}

#endif
