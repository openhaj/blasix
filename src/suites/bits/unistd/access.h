/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file access.h
 * @brief Cases of F_OK, R_OK, W_OK and X_OK of <unistd.h>.
 * @Created: 2026/11/28 09:33:08 by Moutig
 * @Updated: 2026/11/28 09:33:53 by Moutig
 *
 * The access() modes: usable in #if, and F_OK, R_OK, W_OK, X_OK, R_OK|W_OK,
 * R_OK|X_OK and R_OK|W_OK|X_OK all have distinct values (XBD <unistd.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_ACCESS_H
# define BITS_UNISTD_ACCESS_H

# include "framework/lc.h"

# if defined(LC_REQ_F_OK) || defined(LC_DEFS_ALL)
TEST_SYM(F_OK__if, "F_OK", usable_in_if, .ref = "XBD <unistd.h>")
{
#  if F_OK == 0 || F_OK != 0
	EXPECT_TRUE(1);
#  endif
}
# endif

# if defined(LC_REQ_R_OK) || defined(LC_DEFS_ALL)
TEST_SYM(R_OK__if, "R_OK", usable_in_if, .ref = "XBD <unistd.h>")
{
#  if R_OK == 0 || R_OK != 0
	EXPECT_TRUE(1);
#  endif
}
# endif

# if defined(LC_REQ_W_OK) || defined(LC_DEFS_ALL)
TEST_SYM(W_OK__if, "W_OK", usable_in_if, .ref = "XBD <unistd.h>")
{
#  if W_OK == 0 || W_OK != 0
	EXPECT_TRUE(1);
#  endif
}
# endif

# if defined(LC_REQ_X_OK) || defined(LC_DEFS_ALL)
TEST_SYM(X_OK__if, "X_OK", usable_in_if, .ref = "XBD <unistd.h>")
{
#  if X_OK == 0 || X_OK != 0
	EXPECT_TRUE(1);
#  endif
}
# endif

# if (defined(LC_REQ_F_OK) && defined(LC_REQ_R_OK) && defined(LC_REQ_W_OK) && defined(LC_REQ_X_OK)) \
	|| defined(LC_DEFS_ALL)
/* F_OK, R_OK, W_OK, X_OK, R_OK|W_OK, R_OK|X_OK and R_OK|W_OK|X_OK all differ. */
TEST_SYM(R_OK__distinct, "R_OK", distinct_from_the_other_modes_and_their_combinations,
	.ref = "XBD <unistd.h>")
{
	const long	v[7] = {F_OK, R_OK, W_OK, X_OK, R_OK | W_OK, R_OK | X_OK, R_OK | W_OK | X_OK};
	int			bad = 0;

	for (int i = 0; i < 7; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

#endif
