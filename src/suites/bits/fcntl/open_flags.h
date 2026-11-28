/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file open_flags.h
 * @brief Cases of the O_* flags of <fcntl.h> (and of <stdlib.h>, <unistd.h>).
 * @Created: 2026/11/25 12:40:43 by Moutig
 * @Updated: 2026/11/28 11:58:52 by Moutig
 *
 * XBD <fcntl.h>: every O_* value is suitable for use in #if; the file
 * creation flags (O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC, O_CLOEXEC,
 * O_DIRECTORY, O_NOFOLLOW, O_CLOFORK) are bitwise-distinct, O_TTY_INIT
 * may be 0 or else is distinct too; the access modes are unique (O_EXEC
 * and O_SEARCH may be equal) and within O_ACCMODE. <stdlib.h> (2024) and
 * <unistd.h> (2024) define some of them as described there; the probes
 * (same=fcntl.h) check the values agree.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_FCNTL_OPEN_FLAGS_H
# define BITS_FCNTL_OPEN_FLAGS_H

# include "framework/lc.h"

# if defined(LC_REQ_O_CREAT) || defined(LC_DEFS_ALL)
TEST_SYM(O_CREAT__if, "O_CREAT", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_CREAT == 0
	EXPECT_EQ(O_CREAT, 0);
#  else
	EXPECT_NE(O_CREAT, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_EXCL) || defined(LC_DEFS_ALL)
TEST_SYM(O_EXCL__if, "O_EXCL", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_EXCL == 0
	EXPECT_EQ(O_EXCL, 0);
#  else
	EXPECT_NE(O_EXCL, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_NOCTTY) || defined(LC_DEFS_ALL)
TEST_SYM(O_NOCTTY__if, "O_NOCTTY", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_NOCTTY == 0
	EXPECT_EQ(O_NOCTTY, 0);
#  else
	EXPECT_NE(O_NOCTTY, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_TRUNC) || defined(LC_DEFS_ALL)
TEST_SYM(O_TRUNC__if, "O_TRUNC", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_TRUNC == 0
	EXPECT_EQ(O_TRUNC, 0);
#  else
	EXPECT_NE(O_TRUNC, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_CLOEXEC) || defined(LC_DEFS_ALL)
TEST_SYM(O_CLOEXEC__if, "O_CLOEXEC", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_CLOEXEC == 0
	EXPECT_EQ(O_CLOEXEC, 0);
#  else
	EXPECT_NE(O_CLOEXEC, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_DIRECTORY) || defined(LC_DEFS_ALL)
TEST_SYM(O_DIRECTORY__if, "O_DIRECTORY", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_DIRECTORY == 0
	EXPECT_EQ(O_DIRECTORY, 0);
#  else
	EXPECT_NE(O_DIRECTORY, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_NOFOLLOW) || defined(LC_DEFS_ALL)
TEST_SYM(O_NOFOLLOW__if, "O_NOFOLLOW", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_NOFOLLOW == 0
	EXPECT_EQ(O_NOFOLLOW, 0);
#  else
	EXPECT_NE(O_NOFOLLOW, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_CLOFORK) || defined(LC_DEFS_ALL)
TEST_SYM(O_CLOFORK__if, "O_CLOFORK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_CLOFORK == 0
	EXPECT_EQ(O_CLOFORK, 0);
#  else
	EXPECT_NE(O_CLOFORK, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_TTY_INIT) || defined(LC_DEFS_ALL)
TEST_SYM(O_TTY_INIT__if, "O_TTY_INIT", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_TTY_INIT == 0
	EXPECT_EQ(O_TTY_INIT, 0);
#  else
	EXPECT_NE(O_TTY_INIT, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_APPEND) || defined(LC_DEFS_ALL)
TEST_SYM(O_APPEND__if, "O_APPEND", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_APPEND == 0
	EXPECT_EQ(O_APPEND, 0);
#  else
	EXPECT_NE(O_APPEND, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_NONBLOCK) || defined(LC_DEFS_ALL)
TEST_SYM(O_NONBLOCK__if, "O_NONBLOCK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_NONBLOCK == 0
	EXPECT_EQ(O_NONBLOCK, 0);
#  else
	EXPECT_NE(O_NONBLOCK, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_SYNC) || defined(LC_DEFS_ALL)
TEST_SYM(O_SYNC__if, "O_SYNC", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_SYNC == 0
	EXPECT_EQ(O_SYNC, 0);
#  else
	EXPECT_NE(O_SYNC, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_DSYNC) || defined(LC_DEFS_ALL)
TEST_SYM(O_DSYNC__if, "O_DSYNC", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_DSYNC == 0
	EXPECT_EQ(O_DSYNC, 0);
#  else
	EXPECT_NE(O_DSYNC, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_RSYNC) || defined(LC_DEFS_ALL)
TEST_SYM(O_RSYNC__if, "O_RSYNC", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_RSYNC == 0
	EXPECT_EQ(O_RSYNC, 0);
#  else
	EXPECT_NE(O_RSYNC, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_ACCMODE) || defined(LC_DEFS_ALL)
TEST_SYM(O_ACCMODE__if, "O_ACCMODE", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_ACCMODE == 0
	EXPECT_EQ(O_ACCMODE, 0);
#  else
	EXPECT_NE(O_ACCMODE, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_RDONLY) || defined(LC_DEFS_ALL)
TEST_SYM(O_RDONLY__if, "O_RDONLY", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_RDONLY == 0
	EXPECT_EQ(O_RDONLY, 0);
#  else
	EXPECT_NE(O_RDONLY, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_RDWR) || defined(LC_DEFS_ALL)
TEST_SYM(O_RDWR__if, "O_RDWR", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_RDWR == 0
	EXPECT_EQ(O_RDWR, 0);
#  else
	EXPECT_NE(O_RDWR, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_WRONLY) || defined(LC_DEFS_ALL)
TEST_SYM(O_WRONLY__if, "O_WRONLY", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_WRONLY == 0
	EXPECT_EQ(O_WRONLY, 0);
#  else
	EXPECT_NE(O_WRONLY, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_EXEC) || defined(LC_DEFS_ALL)
TEST_SYM(O_EXEC__if, "O_EXEC", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_EXEC == 0
	EXPECT_EQ(O_EXEC, 0);
#  else
	EXPECT_NE(O_EXEC, 0);
#  endif
}
# endif

# if defined(LC_REQ_O_SEARCH) || defined(LC_DEFS_ALL)
TEST_SYM(O_SEARCH__if, "O_SEARCH", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if O_SEARCH == 0
	EXPECT_EQ(O_SEARCH, 0);
#  else
	EXPECT_NE(O_SEARCH, 0);
#  endif
}
# endif

# if (defined(LC_REQ_O_CLOEXEC) && defined(LC_REQ_O_CLOFORK)) || defined(LC_DEFS_ALL)
TEST_SYM(O_CLOFORK__distinct, "O_CLOFORK", bitwise_distinct_from_O_CLOEXEC, .ref = "XBD <fcntl.h>")
{
	EXPECT_NE(O_CLOFORK, 0);
	EXPECT_EQ(O_CLOFORK & O_CLOEXEC, 0);
}
# endif

# if (defined(LC_REQ_O_NOCTTY) && defined(LC_REQ_O_CLOEXEC) && defined(LC_REQ_O_CLOFORK)) \
	|| defined(LC_DEFS_ALL)
TEST_SYM(O_NOCTTY__distinct, "O_NOCTTY", bitwise_distinct_from_the_other_creation_flags,
	.ref = "XBD <fcntl.h>")
{
	EXPECT_NE(O_NOCTTY, 0);
	EXPECT_EQ(O_NOCTTY & O_CLOEXEC, 0);
	EXPECT_EQ(O_NOCTTY & O_CLOFORK, 0);
}
# endif

# if (defined(LC_REQ_O_CREAT) && defined(LC_REQ_O_EXCL) && defined(LC_REQ_O_NOCTTY) && defined(LC_REQ_O_TRUNC)) || defined(LC_DEFS_ALL)
TEST_SYM(O_CREAT__distinct, "O_CREAT", bitwise_distinct_from_the_other_2001_creation_flags, .ref = "XBD <fcntl.h>")
{
	const long	v[4] = {O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC};
	const int	n = 4;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += (v[i] & v[j]) != 0;
	for (int i = 0; i < n; i++)
		bad += v[i] == 0;
	EXPECT_EQ(bad, 0);
}
# endif

# if (defined(LC_REQ_O_CREAT) && defined(LC_REQ_O_EXCL) && defined(LC_REQ_O_NOCTTY) && defined(LC_REQ_O_TRUNC) && defined(LC_REQ_O_CLOEXEC) && defined(LC_REQ_O_DIRECTORY) && defined(LC_REQ_O_NOFOLLOW)) || defined(LC_DEFS_ALL)
TEST_SYM(O_DIRECTORY__distinct, "O_DIRECTORY", bitwise_distinct_from_the_other_creation_flags, .ref = "XBD <fcntl.h>")
{
	const long	v[7] = {O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC, O_CLOEXEC, O_DIRECTORY, O_NOFOLLOW};
	const int	n = 7;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += (v[i] & v[j]) != 0;
	for (int i = 0; i < n; i++)
		bad += v[i] == 0;
	EXPECT_EQ(bad, 0);
}
# endif

# if (defined(LC_REQ_O_TTY_INIT) && defined(LC_REQ_O_CREAT) && defined(LC_REQ_O_EXCL) && defined(LC_REQ_O_NOCTTY) && defined(LC_REQ_O_TRUNC) && defined(LC_REQ_O_CLOEXEC) && defined(LC_REQ_O_DIRECTORY) && defined(LC_REQ_O_NOFOLLOW)) || defined(LC_DEFS_ALL)
/* O_TTY_INIT may be 0; otherwise it is bitwise-distinct from the other creation flags. */
TEST_SYM(O_TTY_INIT__distinct, "O_TTY_INIT", zero_or_bitwise_distinct, .ref = "XBD <fcntl.h>")
{
	EXPECT_TRUE(O_TTY_INIT == 0 || (O_TTY_INIT & (O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC | O_CLOEXEC
		| O_DIRECTORY | O_NOFOLLOW)) == 0);
}
# endif

# if (defined(LC_REQ_O_RDONLY) && defined(LC_REQ_O_RDWR) && defined(LC_REQ_O_WRONLY)) || defined(LC_DEFS_ALL)
TEST_SYM(O_RDONLY__distinct, "O_RDONLY", distinct_from_the_other_access_modes, .ref = "XBD <fcntl.h>")
{
	const long	v[3] = {O_RDONLY, O_RDWR, O_WRONLY};
	const int	n = 3;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

# if (defined(LC_REQ_O_ACCMODE) && defined(LC_REQ_O_RDONLY) && defined(LC_REQ_O_RDWR) && defined(LC_REQ_O_WRONLY)) || defined(LC_DEFS_ALL)
/* O_ACCMODE is the mask of the access modes: each one is within it. */
TEST_SYM(O_ACCMODE__mask, "O_ACCMODE", masks_every_access_mode, .ref = "XBD <fcntl.h>")
{
	EXPECT_EQ(O_RDONLY & O_ACCMODE, O_RDONLY);
	EXPECT_EQ(O_WRONLY & O_ACCMODE, O_WRONLY);
	EXPECT_EQ(O_RDWR & O_ACCMODE, O_RDWR);
}
# endif

# if (defined(LC_REQ_O_EXEC) && defined(LC_REQ_O_SEARCH) && defined(LC_REQ_O_RDONLY) && defined(LC_REQ_O_RDWR) && defined(LC_REQ_O_WRONLY)) || defined(LC_DEFS_ALL)
/* O_EXEC and O_SEARCH differ from the other access modes (they may be equal to each other). */
TEST_SYM(O_EXEC__distinct, "O_EXEC", distinct_from_the_read_write_modes, .ref = "XBD <fcntl.h>")
{
	EXPECT_TRUE(O_EXEC != O_RDONLY && O_EXEC != O_WRONLY && O_EXEC != O_RDWR);
	EXPECT_TRUE(O_SEARCH != O_RDONLY && O_SEARCH != O_WRONLY && O_SEARCH != O_RDWR);
}
# endif

#endif
