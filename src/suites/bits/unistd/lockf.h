/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file lockf.h
 * @brief Cases of F_LOCK, F_TEST, F_TLOCK and F_ULOCK of <unistd.h>.
 * @Created: 2026/11/28 09:35:24 by Moutig
 * @Updated: 2026/11/28 09:36:24 by Moutig
 *
 * The commands of lockf() (XSI): "possible values for the function
 * argument", so four different ones (XBD <unistd.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_LOCKF_H
# define BITS_UNISTD_LOCKF_H

# include "framework/lc.h"

# if defined(LC_REQ_F_LOCK) || defined(LC_DEFS_ALL)
TEST_SYM(F_LOCK__int, "F_LOCK", is_an_int_command, .ref = "XBD <unistd.h>")
{
	int	c = F_LOCK;

	EXPECT_TRUE(c == F_LOCK);
}
# endif

# if defined(LC_REQ_F_TEST) || defined(LC_DEFS_ALL)
TEST_SYM(F_TEST__int, "F_TEST", is_an_int_command, .ref = "XBD <unistd.h>")
{
	int	c = F_TEST;

	EXPECT_TRUE(c == F_TEST);
}
# endif

# if defined(LC_REQ_F_TLOCK) || defined(LC_DEFS_ALL)
TEST_SYM(F_TLOCK__int, "F_TLOCK", is_an_int_command, .ref = "XBD <unistd.h>")
{
	int	c = F_TLOCK;

	EXPECT_TRUE(c == F_TLOCK);
}
# endif

# if defined(LC_REQ_F_ULOCK) || defined(LC_DEFS_ALL)
TEST_SYM(F_ULOCK__int, "F_ULOCK", is_an_int_command, .ref = "XBD <unistd.h>")
{
	int	c = F_ULOCK;

	EXPECT_TRUE(c == F_ULOCK);
}
# endif

# if (defined(LC_REQ_F_LOCK) && defined(LC_REQ_F_TEST) && defined(LC_REQ_F_TLOCK) \
	&& defined(LC_REQ_F_ULOCK)) || defined(LC_DEFS_ALL)
/* Four commands of lockf(): four values. */
TEST_SYM(F_LOCK__distinct, "F_LOCK", distinct_from_the_other_lockf_commands, .ref = "XBD <unistd.h>")
{
	EXPECT_TRUE(F_LOCK != F_TEST && F_LOCK != F_TLOCK && F_LOCK != F_ULOCK);
	EXPECT_TRUE(F_TEST != F_TLOCK && F_TEST != F_ULOCK && F_TLOCK != F_ULOCK);
}
# endif

#endif
