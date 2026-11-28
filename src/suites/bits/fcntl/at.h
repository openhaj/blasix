/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file at.h
 * @brief Cases of AT_FDCWD and the AT_* flags (POSIX.1-2008).
 * @Created: 2026/11/28 11:11:14 by Moutig
 * @Updated: 2026/11/28 11:58:45 by Moutig
 *
 * AT_FDCWD is the special value used in place of a descriptor by the *at()
 * functions (no open() can return it: it is not a non-negative
 * descriptor number); AT_EACCESS, AT_SYMLINK_NOFOLLOW, AT_SYMLINK_FOLLOW,
 * AT_REMOVEDIR are flags of those functions (XBD <fcntl.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_FCNTL_AT_H
# define BITS_FCNTL_AT_H

# include "framework/lc.h"

# if defined(LC_REQ_AT_FDCWD) || defined(LC_DEFS_ALL)
TEST_SYM(AT_FDCWD__int, "AT_FDCWD", is_an_int_value, .ref = "XBD <fcntl.h>")
{
	int	v = AT_FDCWD;

	EXPECT_TRUE(v == AT_FDCWD);
}
# endif

# if defined(LC_REQ_AT_EACCESS) || defined(LC_DEFS_ALL)
TEST_SYM(AT_EACCESS__int, "AT_EACCESS", is_an_int_value, .ref = "XBD <fcntl.h>")
{
	int	v = AT_EACCESS;

	EXPECT_TRUE(v == AT_EACCESS);
}
# endif

# if defined(LC_REQ_AT_SYMLINK_NOFOLLOW) || defined(LC_DEFS_ALL)
TEST_SYM(AT_SYMLINK_NOFOLLOW__int, "AT_SYMLINK_NOFOLLOW", is_an_int_value, .ref = "XBD <fcntl.h>")
{
	int	v = AT_SYMLINK_NOFOLLOW;

	EXPECT_TRUE(v == AT_SYMLINK_NOFOLLOW);
}
# endif

# if defined(LC_REQ_AT_SYMLINK_FOLLOW) || defined(LC_DEFS_ALL)
TEST_SYM(AT_SYMLINK_FOLLOW__int, "AT_SYMLINK_FOLLOW", is_an_int_value, .ref = "XBD <fcntl.h>")
{
	int	v = AT_SYMLINK_FOLLOW;

	EXPECT_TRUE(v == AT_SYMLINK_FOLLOW);
}
# endif

# if defined(LC_REQ_AT_REMOVEDIR) || defined(LC_DEFS_ALL)
TEST_SYM(AT_REMOVEDIR__int, "AT_REMOVEDIR", is_an_int_value, .ref = "XBD <fcntl.h>")
{
	int	v = AT_REMOVEDIR;

	EXPECT_TRUE(v == AT_REMOVEDIR);
}
# endif

# if defined(LC_REQ_AT_FDCWD) || defined(LC_DEFS_ALL)
/* A special value in place of a descriptor: it can be no descriptor, so it is negative. */
TEST_SYM(AT_FDCWD__not_a_descriptor, "AT_FDCWD", is_not_a_valid_descriptor_number, .ref = "XBD <fcntl.h>")
{
	EXPECT_LT(AT_FDCWD, 0);
}
# endif

#endif
