/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file open_flags.h
 * @brief Cases of the O_* constants <stdlib.h> takes from <fcntl.h>.
 * @Created: 2026/11/25 12:40:43 by Moutig
 * @Updated: 2026/11/25 12:40:51 by Moutig
 *
 * POSIX.1-2024 (CX, Austin Group Defects 593 and 1350): <stdlib.h>
 * defines O_APPEND, O_CLOEXEC, O_CLOFORK, O_SYNC, O_DSYNC and O_RSYNC
 * (SIO), O_NOCTTY and O_RDWR (XSI) as described in <fcntl.h>: every value
 * is suitable for use in #if directives, and the file creation flags
 * (O_CLOEXEC, O_CLOFORK, O_NOCTTY) are bitwise-distinct. The probe
 * (same=fcntl.h) checks that both headers give the same values.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * ( -DLC_REQ_<constant>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_FCNTL_OPEN_FLAGS_H
# define BITS_FCNTL_OPEN_FLAGS_H

# include "framework/lc.h"

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

#endif
