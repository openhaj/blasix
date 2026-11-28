/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file file_types.h
 * @brief Cases of S_IFMT and the S_IF* file types (XSI).
 * @Created: 2026/11/28 12:18:02 by Moutig
 * @Updated: 2026/11/28 12:18:37 by Moutig
 *
 * XBD <sys/stat.h> (XSI): S_IFMT, the type of file field of st_mode, and
 * the file types S_IFBLK, S_IFCHR, S_IFIFO, S_IFREG, S_IFDIR, S_IFLNK,
 * S_IFSOCK encoded in it, all usable in #if.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_STAT_FILE_TYPES_H
# define BITS_SYS_STAT_FILE_TYPES_H

# include "framework/lc.h"

# if defined(LC_REQ_S_IFMT) || defined(LC_DEFS_ALL)
TEST_SYM(S_IFMT__if, "S_IFMT", suitable_for_use_in_if, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
#  if S_IFMT == 0
	EXPECT_EQ(S_IFMT, 0);
#  else
	EXPECT_NE(S_IFMT, 0);
#  endif
}
# endif

# if defined(LC_REQ_S_IFBLK) || defined(LC_DEFS_ALL)
TEST_SYM(S_IFBLK__if, "S_IFBLK", suitable_for_use_in_if, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
#  if S_IFBLK == 0
	EXPECT_EQ(S_IFBLK, 0);
#  else
	EXPECT_NE(S_IFBLK, 0);
#  endif
}
# endif

# if defined(LC_REQ_S_IFCHR) || defined(LC_DEFS_ALL)
TEST_SYM(S_IFCHR__if, "S_IFCHR", suitable_for_use_in_if, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
#  if S_IFCHR == 0
	EXPECT_EQ(S_IFCHR, 0);
#  else
	EXPECT_NE(S_IFCHR, 0);
#  endif
}
# endif

# if defined(LC_REQ_S_IFIFO) || defined(LC_DEFS_ALL)
TEST_SYM(S_IFIFO__if, "S_IFIFO", suitable_for_use_in_if, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
#  if S_IFIFO == 0
	EXPECT_EQ(S_IFIFO, 0);
#  else
	EXPECT_NE(S_IFIFO, 0);
#  endif
}
# endif

# if defined(LC_REQ_S_IFREG) || defined(LC_DEFS_ALL)
TEST_SYM(S_IFREG__if, "S_IFREG", suitable_for_use_in_if, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
#  if S_IFREG == 0
	EXPECT_EQ(S_IFREG, 0);
#  else
	EXPECT_NE(S_IFREG, 0);
#  endif
}
# endif

# if defined(LC_REQ_S_IFDIR) || defined(LC_DEFS_ALL)
TEST_SYM(S_IFDIR__if, "S_IFDIR", suitable_for_use_in_if, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
#  if S_IFDIR == 0
	EXPECT_EQ(S_IFDIR, 0);
#  else
	EXPECT_NE(S_IFDIR, 0);
#  endif
}
# endif

# if defined(LC_REQ_S_IFLNK) || defined(LC_DEFS_ALL)
TEST_SYM(S_IFLNK__if, "S_IFLNK", suitable_for_use_in_if, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
#  if S_IFLNK == 0
	EXPECT_EQ(S_IFLNK, 0);
#  else
	EXPECT_NE(S_IFLNK, 0);
#  endif
}
# endif

# if defined(LC_REQ_S_IFSOCK) || defined(LC_DEFS_ALL)
TEST_SYM(S_IFSOCK__if, "S_IFSOCK", suitable_for_use_in_if, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
#  if S_IFSOCK == 0
	EXPECT_EQ(S_IFSOCK, 0);
#  else
	EXPECT_NE(S_IFSOCK, 0);
#  endif
}
# endif

# if (defined(LC_REQ_S_IFMT) && defined(LC_REQ_S_IFBLK) && defined(LC_REQ_S_IFCHR) && defined(LC_REQ_S_IFIFO) && defined(LC_REQ_S_IFREG) && defined(LC_REQ_S_IFDIR) && defined(LC_REQ_S_IFLNK) && defined(LC_REQ_S_IFSOCK)) || defined(LC_DEFS_ALL)
/* Each file type is a value of the S_IFMT field: within it, and different from the others. */
TEST_SYM(S_IFMT__field, "S_IFMT", holds_distinct_file_types, .opt = "XSI", .ref = "XBD <sys/stat.h>")
{
	const long	v[7] = {S_IFBLK, S_IFCHR, S_IFIFO, S_IFREG, S_IFDIR, S_IFLNK, S_IFSOCK};
	int			bad = 0;

	for (int i = 0; i < 7; i++)
	{
		bad += v[i] == 0 || (v[i] & ~(long)S_IFMT) != 0;
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	}
	EXPECT_EQ(bad, 0);
}
# endif

# if (defined(LC_REQ_S_IFMT) && defined(LC_REQ_S_IRWXU) && defined(LC_REQ_S_IRWXG) \
	&& defined(LC_REQ_S_IRWXO) && defined(LC_REQ_S_ISUID) && defined(LC_REQ_S_ISGID) \
	&& defined(LC_REQ_S_ISVTX)) || defined(LC_DEFS_ALL)
/* The file type field and the mode bits do not overlap. */
TEST_SYM(S_IFMT__apart_from_the_mode_bits, "S_IFMT", apart_from_the_mode_bits, .opt = "XSI",
	.ref = "XBD <sys/stat.h>")
{
	EXPECT_EQ(S_IFMT & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX), 0);
}
# endif

#endif
